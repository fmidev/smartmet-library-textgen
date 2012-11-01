#include "PostGISDataSource.h"
#include "TextGenError.h"
#include "WeatherArea.h"
#include "MessageLogger.h"

#include <newbase/NFmiSvgPath.h>
#include <newbase/NFmiPoint.h>

#include <ogrsf_frmts.h>

#include <boost/algorithm/string/replace.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace boost;

namespace TextGen
{

  PostGISDataSource::PostGISDataSource(MessageLogger& log) : theLog(log)
  {
  }

  bool PostGISDataSource::readData(const std::string& host, 
								   const std::string& port,
								   const std::string& dbname,
								   const std::string& user,
								   const std::string& password,
								   const std::string& schema,
								   const std::string& table,
								   const std::string& fieldname,
								   const std::string& client_encoding)
  {

	try
	  {
		std::string queryparameter(host+port+dbname+schema+table+fieldname+client_encoding);
		if(queryparametermap.find(queryparameter) != queryparametermap.end())
		  return true;
		
		std::stringstream connection_ss;
	  
		connection_ss << "PG:host='" << host 
					  << "' port='" << port 
					  << "' dbname='" << dbname 
					  << "' user='" << user 
					  << "' password='" << password << "'";


		OGRRegisterAll();
  
		OGRDataSource* pDS = OGRSFDriverRegistrar::Open(connection_ss.str().c_str(), FALSE);
		
		if(!pDS)
		  {
			throw TextGenError("Error: OGRSFDriverRegistrar::Open(" + connection_ss.str() + ") failed!");
		  }

		std::string sqlstmt("SET CLIENT_ENCODING TO '" + client_encoding + "'");
		pDS->ExecuteSQL(sqlstmt.c_str(), 0, 0);

		std::stringstream schema_table_ss;
	  
		schema_table_ss << schema << "." << table;

		OGRLayer* pLayer = pDS->GetLayerByName(schema_table_ss.str().c_str());
	
		if(pLayer == NULL)
		  {
			throw TextGenError("Error: OGRDataSource::GetLayerByName(" + schema_table_ss.str() + ") failed!");
		  }
	
		// get spatial reference
		OGRSpatialReference* pLayerSRS = pLayer->GetSpatialRef();
		OGRCoordinateTransformation* pCoordinateTransform(0);
		OGRSpatialReference  targetTransformSRS;
		int UTMZone(0);
		if(pLayerSRS)
		  {
			UTMZone = pLayerSRS->GetUTMZone();
			/*
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
			pCoordinateTransform = OGRCreateCoordinateTransformation(pLayerSRS,
																	 &targetTransformSRS);
		  }

		OGRFeature* pFeature(0);	
		pLayer->ResetReading();

		while((pFeature = pLayer->GetNextFeature()) != NULL)
		  {
			OGRFeatureDefn* pFDefn = pLayer->GetLayerDefn();

			// find name for the area
			std::string area_name("");
			int iField;
			for( iField = 0; iField < pFDefn->GetFieldCount(); iField++ )
			  {
				OGRFieldDefn* pFieldDefn = pFDefn->GetFieldDefn( iField );
			  
				if(fieldname.compare(pFieldDefn->GetNameRef()) == 0)
				  {
					area_name = pFeature->GetFieldAsString(iField);
					break;
				  }
			  }

			if(area_name.empty())
			  {
				theLog << "field " << fieldname << " not found for the feature " << pFDefn->GetName() << endl;
				continue;
			  }

			// get geometry
			OGRGeometry* pGeometry(pFeature->GetGeometryRef());
		  
			if(pGeometry)
			  {				
				if(pLayerSRS && pCoordinateTransform)
				  {
					// transform the coordinates to wgs84
					if(OGRERR_NONE != pGeometry->transform(pCoordinateTransform))
					  theLog << "pGeometry->transform() failed" << endl;
				  }

				OGRwkbGeometryType geometryType(wkbFlatten(pGeometry->getGeometryType()));

				if(geometryType == wkbPoint)
				  {
					OGRPoint* pPoint = (OGRPoint*) pGeometry;
					if(pointmap.find(area_name) != pointmap.end())
					  pointmap[area_name] = NFmiPoint(pPoint->getX(), pPoint->getY());
					else
					  pointmap.insert(make_pair(area_name, NFmiPoint(pPoint->getX(), pPoint->getY())));
				  }
				else if(geometryType == wkbMultiPolygon || geometryType == wkbPolygon)
				  {
					string svg_string("");
					if(geometryType == wkbMultiPolygon)
					  {
						OGRMultiPolygon* pMultiPolygon = (OGRMultiPolygon*) pGeometry;						
						char* wkt_buffer(0);						
						pMultiPolygon->exportToWkt(&wkt_buffer);
						svg_string.append(wkt_buffer);
						CPLFree(wkt_buffer);
						//	if(area_name.compare("Finland") == 0)
						//cout << "RAWMULTIPOLYGON: " << svg_string << endl;
					  }
					else
					  {
						OGRPolygon* pPolygon = (OGRPolygon*) pGeometry;

						char* wkt_buffer(0);						
						pPolygon->exportToWkt(&wkt_buffer);
						svg_string.append(wkt_buffer);
						CPLFree(wkt_buffer);
						// cout << "RAWPOLYGON: " << svg_string << endl;
					  }

					replace_all(svg_string, "MULTIPOLYGON ", "");
					replace_all(svg_string, "POLYGON ", "");
					replace_all(svg_string, "),(", " Z M ");
					replace_all(svg_string, ",", " L ");
					replace_all(svg_string, "(", "");
					replace_all(svg_string, ")", "");
					svg_string.insert(0, "\"M ");
					svg_string.append(" Z\"\n");

					// cout << "POLYGON in SVG format: " << svg_string << endl;

					stringstream ss;
					ss << svg_string;
					NFmiSvgPath svgPath;
					svgPath.Read(ss);

					if(polygonmap.find(area_name) != polygonmap.end())
					  polygonmap[area_name] = svgPath;
					else
					  polygonmap.insert(make_pair(area_name, svgPath));
				  }
				else
				  {
					// no other geometries handled
				  } 
			  }
			// destroy feature
			OGRFeature::DestroyFeature(pFeature); 
		  }
		if(pCoordinateTransform) {
		  delete pCoordinateTransform;
		}

		// in the end destroy data source
		OGRDataSource::DestroyDataSource(pDS);

		queryparametermap.insert(make_pair(queryparameter, 1));
	  }
	catch(...)
	  {
		throw;
	  }

	return true;
  }

  NFmiSvgPath PostGISDataSource::getSVGPath(const std::string & name)
  {
	if(polygonmap.find(name) != polygonmap.end()) 
		return polygonmap[name];
	else
	  return NFmiSvgPath();
  }

  NFmiPoint PostGISDataSource::getPoint(const std::string & name)
  {
	if(pointmap.find(name) != pointmap.end())
	  return pointmap[name];
	else
	  return NFmiPoint();
  }

  OGRDataSource* PostGISDataSource::connect(const std::string & host, 
											const std::string & port,
											const std::string & dbname,
											const std::string & user,
											const std::string & password)
  {
	OGRRegisterAll();

	OGRSFDriver* pOGRDriver(OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL"));
  
	if(!pOGRDriver)
	  {
		throw TextGenError("Error: PostgreSQL driver not found!");
	  }
	
	std::stringstream ss;
	  
	ss << "PG:host='" << host 
	   << "' port='" << port 
	   << "' dbname='" << dbname 
	   << "' user='" << user 
	   << "' password='" << password << "'";
  
	return pOGRDriver->Open(ss.str().c_str());
  }

  TextGen::WeatherArea PostGISDataSource::makeArea(const std::string& thePostGISName, const std::string& theConfigName)
  {
	if(isPolygon(thePostGISName))
	  {
		const NFmiSvgPath svgPath(getSVGPath(thePostGISName));
		return TextGen::WeatherArea(svgPath, theConfigName);
	  }
	else if(isPoint(thePostGISName))
	  {
		const NFmiPoint point(getPoint(thePostGISName));
		return TextGen::WeatherArea(point, theConfigName);
	  }
	else
	  {
		return TextGen::WeatherArea(NFmiPoint(), "");
	  }
  }

  bool PostGISDataSource::areaExists(const std::string& theName)
  {
	return (isPolygon(theName) || isPoint(theName));
  }
  

} // namespace TextGen
