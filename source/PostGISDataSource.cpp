#include "PostGISDataSource.h"
#include "TextGenError.h"
#include "WeatherArea.h"

#include <newbase/NFmiSvgPath.h>
#include <newbase/NFmiPoint.h>

#include <iostream>
#include <sstream>

#include <boost/algorithm/string/replace.hpp>

#include "ogrsf_frmts.h"

using namespace std;
using namespace boost;

namespace TextGen
{

  PostGISDataSource::PostGISDataSource()
  {
  }

  bool PostGISDataSource::readData(const std::string host, 
								   const std::string port,
								   const std::string dbname,
								   const std::string user,
								   const std::string password,
								   const std::string schema,
								   const std::string table,
								   const std::string fieldname)
  {

	try
	  {
		OGRRegisterAll();

		OGRSFDriver* pOGRDriver(OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("PostgreSQL"));
  
		if(!pOGRDriver)
		  {
			throw TextGenError("Error: PostgreSQL driver not found!");
		  }
	
		std::stringstream connection_ss;
	  
		connection_ss << "PG:host='" << host 
					  << "' port='" << port 
					  << "' dbname='" << dbname 
					  << "' user='" << user 
					  << "' password='" << password << "'";
  
		OGRDataSource* pDS =  pOGRDriver->Open(connection_ss.str().c_str());
		
		if(!pDS)
		  {
			throw TextGenError("Error: OGRSFDriver::Open(" + connection_ss.str() + ") failed!");
		  }

		std::stringstream schema_table_ss;
	  
		schema_table_ss << schema << "." << table;

		OGRLayer* pLayer = pDS->GetLayerByName(schema_table_ss.str().c_str());
	
		if(pLayer == NULL)
		  {
			throw TextGenError("Error: OGRDataSource::GetLayerByName(" + schema_table_ss.str() + ") failed!");
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
				throw TextGenError("Error: PostGISDataSource::readData() field " + fieldname + " not found!");
			  }

			// get geometry
			OGRGeometry* pGeometry(pFeature->GetGeometryRef());
		  
			if(pGeometry)
			  {
				OGRwkbGeometryType geometryType(wkbFlatten(pGeometry->getGeometryType()));

				if(geometryType == wkbPoint )
				  {
					OGRPoint* pPoint = (OGRPoint*) pGeometry;
					pointmap.insert(make_pair(area_name, NFmiPoint(pPoint->getX(), pPoint->getY())));
					//					cout << "pointdata: " << pPoint->getX() << ", " <<  pPoint->getY() << endl;
					/*
					char* wkt_buffer(0);	
					pPoint->exportToWkt(&wkt_buffer);					
					cout << area_name << ": " << wkt_buffer << endl;
					CPLFree(wkt_buffer);					
					*/
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
						//												cout << "RAWMULTIPOLYGON: " << svg_string << endl;
					  }
					else
					  {
						OGRPolygon* pPolygon = (OGRPolygon*) pGeometry;						
						char* wkt_buffer(0);						
						pPolygon->exportToWkt(&wkt_buffer);
						svg_string.append(wkt_buffer);
						CPLFree(wkt_buffer);
						//												cout << "RAWPOLYGON: " << svg_string << endl;
					  }


					replace_all(svg_string, "MULTIPOLYGON ", "");
					replace_all(svg_string, "POLYGON ", "");
					replace_all(svg_string, "),(", " Z M ");
					replace_all(svg_string, ",", " L ");
					replace_all(svg_string, "(", "");
					replace_all(svg_string, ")", "");
					svg_string.insert(0, "\"M ");
					svg_string.append(" Z\"\n");

					//					cout << "POLYGON in SVG format(" << area_name << "): " << svg_string << endl;

					stringstream ss;
					ss << svg_string;
					NFmiSvgPath svgPath;
					svgPath.Read(ss);

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
	
		// in the end destroy data source
		OGRDataSource::DestroyDataSource(pDS);
	  }
	catch(...)
	  {
		throw;
	  }

	return true;
  }

  NFmiSvgPath PostGISDataSource::getSVGPath(const std::string name)
  {
	if(polygonmap.find(name) != polygonmap.end())
	  return polygonmap[name];
	else
	  return NFmiSvgPath();
  }

  NFmiPoint PostGISDataSource::getPoint(const std::string name)
  {
	if(pointmap.find(name) != pointmap.end())
	  return pointmap[name];
	else
	  return NFmiPoint();
  }

  OGRDataSource* PostGISDataSource::connect(const std::string host, 
											const std::string port,
											const std::string dbname,
											const std::string user,
											const std::string password)
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
