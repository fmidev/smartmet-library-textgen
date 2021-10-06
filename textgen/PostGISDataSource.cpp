#include "PostGISDataSource.h"

#ifdef UNIX

#include <boost/algorithm/string/replace.hpp>
#include <iostream>
#include <ogrsf_frmts.h>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace BrainStorm
{
bool PostGISDataSource::readData(const postgis_identifier& postGISIdentifier,
                                 std::string& log_message)
{
  return readData(postGISIdentifier.postGISHost,
                  postGISIdentifier.postGISPort,
                  postGISIdentifier.postGISDatabase,
                  postGISIdentifier.postGISUsername,
                  postGISIdentifier.postGISPassword,
                  postGISIdentifier.postGISSchema,
                  postGISIdentifier.postGISTable,
                  postGISIdentifier.postGISField,
                  postGISIdentifier.postGISClientEncoding,
                  log_message);
}

bool PostGISDataSource::readData(const std::string& host,
                                 const std::string& port,
                                 const std::string& dbname,
                                 const std::string& user,
                                 const std::string& password,
                                 const std::string& schema,
                                 const std::string& table,
                                 const std::string& fieldname,
                                 const std::string& client_encoding,
                                 std::string& log_message)
{
  try
  {
    std::string queryparameter(host + port + dbname + schema + table + fieldname + client_encoding);
    if (queryparametermap.find(queryparameter) != queryparametermap.end())
      return true;

    std::stringstream connection_ss;

    connection_ss << "PG:host='" << host << "' port='" << port << "' dbname='" << dbname
                  << "' user='" << user << "' password='" << password << "'";

    OGRRegisterAll();

    /*
    OGRSFDriverRegistrar* reg = OGRSFDriverRegistrar::GetRegistrar();

    if(reg)
      {
            cout << "GetDriverCount(): " << reg->GetDriverCount() << endl;
            for(int i = 0; i < reg->GetDriverCount(); i++)
              {
                    cout << reg->GetDriver(i)->GetName() << endl;
              }

            OGRDataSource* pDS =
    reg->GetDriverByName("PostgreSQL")->Open(connection_ss.str().c_str(), FALSE);

            if(pDS)
              cout << "OK" << endl;
            else
              cout << "NOK" << endl;
      }
    */

#if GDAL_VERSION_MAJOR < 2
    OGRDataSource* pDS = OGRSFDriverRegistrar::Open(connection_ss.str().c_str(), FALSE);
#else
    auto* pDriver = GetGDALDriverManager()->GetDriverByName("PostgreSQL");
    GDALOpenInfo info(connection_ss.str().c_str(), GA_ReadOnly);
    std::unique_ptr<GDALDataset> pDS{pDriver->pfnOpen(&info)};
#endif

    if (!pDS)
    {
      throw std::runtime_error("Error: OGRSFDriverRegistrar::Open(" + connection_ss.str() +
                               ") failed!");
    }

    std::string sqlstmt("SET CLIENT_ENCODING TO '" + client_encoding + "'");
    pDS->ExecuteSQL(sqlstmt.c_str(), nullptr, nullptr);

    std::stringstream schema_table_ss;

    schema_table_ss << schema << "." << table;

    OGRLayer* pLayer = pDS->GetLayerByName(schema_table_ss.str().c_str());

    if (pLayer == nullptr)
    {
      throw std::runtime_error("Error: OGRDataSource::GetLayerByName(" + schema_table_ss.str() +
                               ") failed!");
    }

    // get spatial reference
    OGRSpatialReference* pLayerSRS = pLayer->GetSpatialRef();
    OGRCoordinateTransformation* pCoordinateTransform(nullptr);
    OGRSpatialReference targetTransformSRS;
    if (pLayerSRS)
    {
      /*
      int UTMZone(0);
      UTMZone = pLayerSRS->GetUTMZone();
      cout << "UTMZone: " << UTMZone << endl;
      cout << "IsGeographic: " << pLayerSRS->IsGeographic() << endl;
      cout << "IsProjected: " << pLayerSRS->IsProjected() << endl;
      char* wkt_buffer(0);
      pLayerSRS->exportToPrettyWkt(&wkt_buffer);
      cout <<  wkt_buffer << endl;
      CPLFree(wkt_buffer);
      */

      // set WGS84 coordinate system
      targetTransformSRS.SetWellKnownGeogCS("WGS84");

      // create transformation object
      pCoordinateTransform = OGRCreateCoordinateTransformation(pLayerSRS, &targetTransformSRS);
    }

    OGRFeature* pFeature(nullptr);
    pLayer->ResetReading();

    while ((pFeature = pLayer->GetNextFeature()) != nullptr)
    {
      OGRFeatureDefn* pFDefn = pLayer->GetLayerDefn();

      // find name for the area
      std::string area_name("");
      int iField;
      for (iField = 0; iField < pFDefn->GetFieldCount(); iField++)
      {
        OGRFieldDefn* pFieldDefn = pFDefn->GetFieldDefn(iField);

        if (fieldname == pFieldDefn->GetNameRef())
        {
          area_name = pFeature->GetFieldAsString(iField);
          break;
        }
      }

      if (area_name.empty())
      {
        log_message = "field " + fieldname + " not found for the feature " + pFDefn->GetName();
        continue;
      }

      // get geometry
      OGRGeometry* pGeometry(pFeature->GetGeometryRef());

      if (pGeometry)
      {
        if (pLayerSRS && pCoordinateTransform)
        {
          // transform the coordinates to wgs84
          if (OGRERR_NONE != pGeometry->transform(pCoordinateTransform))
            log_message = "pGeometry->transform() failed";
        }

        // wkbFlatten-macro uses old-style cast
        //				OGRwkbGeometryType
        // geometryType(wkbFlatten(pGeometry->getGeometryType()));
        auto geometryType =
            static_cast<OGRwkbGeometryType>(pGeometry->getGeometryType() & (~wkb25DBit));

        if (geometryType == wkbPoint)
        {
          auto* pPoint = reinterpret_cast<OGRPoint*>(pGeometry);
          if (pointmap.find(area_name) != pointmap.end())
            pointmap[area_name] = make_pair(pPoint->getX(), pPoint->getY());
          else
            pointmap.insert(make_pair(area_name, make_pair(pPoint->getX(), pPoint->getY())));
        }
        else if (geometryType == wkbMultiPolygon || geometryType == wkbPolygon)
        {
          string svg_string("");
          if (geometryType == wkbMultiPolygon)
          {
            auto* pMultiPolygon = reinterpret_cast<OGRMultiPolygon*>(pGeometry);
            char* wkt_buffer(nullptr);
            pMultiPolygon->exportToWkt(&wkt_buffer);
            svg_string.append(wkt_buffer);
            CPLFree(wkt_buffer);
            //	if(area_name.compare("Finland") == 0)
            // cout << "RAWMULTIPOLYGON: " << svg_string << endl;
          }
          else
          {
            auto* pPolygon = reinterpret_cast<OGRPolygon*>(pGeometry);

            char* wkt_buffer(nullptr);
            pPolygon->exportToWkt(&wkt_buffer);
            svg_string.append(wkt_buffer);
            CPLFree(wkt_buffer);
            // cout << "RAWPOLYGON: " << svg_string << endl;
          }

          boost::algorithm::replace_all(svg_string, "MULTIPOLYGON ", "");
          boost::algorithm::replace_all(svg_string, "POLYGON ", "");
          boost::algorithm::replace_all(svg_string, "),(", " Z M ");
          boost::algorithm::replace_all(svg_string, ",", " L ");
          boost::algorithm::replace_all(svg_string, "(", "");
          boost::algorithm::replace_all(svg_string, ")", "");
          svg_string.insert(0, "\"M ");
          svg_string.append(" Z\"\n");

          // cout << "POLYGON in SVG format: " << svg_string << endl;
          if (polygonmap.find(area_name) != polygonmap.end())
            polygonmap[area_name] = svg_string;
          else
            polygonmap.insert(make_pair(area_name, svg_string));
        }
        else if (geometryType == wkbMultiLineString || geometryType == wkbLineString)
        {
          string svg_string("");
          if (geometryType == wkbMultiLineString)
          {
            auto* pMultiLine = reinterpret_cast<OGRMultiLineString*>(pGeometry);

            char* wkt_buffer(nullptr);
            pMultiLine->exportToWkt(&wkt_buffer);
            svg_string.append(wkt_buffer);
            CPLFree(wkt_buffer);
          }
          else
          {
            auto* pLine = reinterpret_cast<OGRLineString*>(pGeometry);

            char* wkt_buffer(nullptr);
            pLine->exportToWkt(&wkt_buffer);
            svg_string.append(wkt_buffer);
            CPLFree(wkt_buffer);
            //	cout << "LINESTRING: " << svg_string << endl;
          }

          boost::algorithm::replace_all(svg_string, "MULTILINESTRING ", "");
          boost::algorithm::replace_all(svg_string, "LINESTRING ", "");
          boost::algorithm::replace_all(svg_string, "))((", ",");
          boost::algorithm::replace_all(svg_string, ",", " L ");
          boost::algorithm::replace_all(svg_string, "(", "");
          boost::algorithm::replace_all(svg_string, ")", "");
          svg_string.append(" \"\n");

          if (linemap.find(area_name) != linemap.end())
          {
            string previous_part(linemap[area_name]);
            boost::algorithm::replace_all(previous_part, " \"", " ");
            boost::algorithm::replace_all(previous_part, " \n", " ");
            //						boost::algorithm::replace_all(previous_part,
            //"M", "L");
            svg_string = (previous_part + "L " + svg_string);
          }
          else
          {
            svg_string.insert(0, "\"M ");
          }

          // cout << "LINE in SVG format: " << svg_string << endl;
          if (linemap.find(area_name) != linemap.end())
            linemap[area_name] = svg_string;
          else
            linemap.insert(make_pair(area_name, svg_string));
        }
        else
        {
          // no other geometries handled
        }
      }
      // destroy feature
      OGRFeature::DestroyFeature(pFeature);
    }
    if (pCoordinateTransform)
    {
      delete pCoordinateTransform;
    }

    // in the end destroy data source
#if GDAL_VERSION_MAJOR < 2
    OGRDataSource::DestroyDataSource(pDS);
#endif

    queryparametermap.insert(make_pair(queryparameter, 1));
  }
  catch (...)
  {
    throw;
  }

  return true;
}

std::string PostGISDataSource::getSVGPath(const std::string& name) const
{
  if (polygonmap.find(name) != polygonmap.end())
    return polygonmap.at(name);
  else if (linemap.find(name) != linemap.end())
    return linemap.at(name);
  else
    return "";
}

std::pair<double, double> PostGISDataSource::getPoint(const std::string& name) const
{
  if (pointmap.find(name) != pointmap.end())
    return pointmap.at(name);
  else
    return make_pair(32700.0, 32700.0);
}

PostGISDataSource::GDALData* PostGISDataSource::connect(const std::string& host,
                                                        const std::string& port,
                                                        const std::string& dbname,
                                                        const std::string& user,
                                                        const std::string& password)
{
  OGRRegisterAll();

#if GDAL_VERSION_MAJOR < 2
  auto* pDriver(OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL"));
#else
  auto* pDriver = GetGDALDriverManager()->GetDriverByName("PostgreSQL");
#endif

  if (!pDriver)
  {
    throw std::runtime_error("Error: PostgreSQL driver not found!");
  }

  std::stringstream ss;

  ss << "PG:host='" << host << "' port='" << port << "' dbname='" << dbname << "' user='" << user
     << "' password='" << password << "'";

#if GDAL_VERSION_MAJOR < 2
  return pDriver->Open(ss.str().c_str());
#else
  GDALOpenInfo info(ss.str().c_str(), GA_ReadOnly);
  return pDriver->pfnOpen(&info);
#endif
}

bool PostGISDataSource::geoObjectExists(const std::string& name) const
{
  return (isPolygon(name) || isLine(name) || isPoint(name));
}

std::list<string> PostGISDataSource::areaNames() const
{
  std::list<string> return_list;
  using polygonmap_t = std::map<std::string, std::string>;
  using pointmap_t = std::map<std::string, std::pair<double, double>>;

  for (const polygonmap_t::value_type& vt : polygonmap)
  {
    return_list.push_back(vt.first);
  }
  for (const pointmap_t::value_type& vt : pointmap)
  {
    return_list.push_back(vt.first);
  }

  return return_list;
}

}  // namespace BrainStorm
#endif  // UNIX
