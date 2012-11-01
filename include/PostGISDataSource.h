// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PostGISDataSource
 */
// ======================================================================

#ifndef POSTGIS_DATA_SOURCE_H
#define POSTGIS_DATA_SOURCE_H

#include <string>
#include <map>

#include <boost/any.hpp>

class OGRDataSource;
class NFmiSvgPath;
class NFmiPoint;
class MessageLogger;

namespace TextGen
{

class WeatherArea;

  class PostGISDataSource
  {

  public:

	PostGISDataSource(MessageLogger& log);

	bool readData(const std::string& host, 
				  const std::string& port,
				  const std::string& dbname,
				  const std::string& user,
				  const std::string& password,
				  const std::string& schema,
				  const std::string& table,
				  const std::string& fieldname,
				  const std::string& client_encoding);

	TextGen::WeatherArea makeArea(const std::string& thePostGISName, const std::string& theConfigName);
	bool areaExists(const std::string& theName);
	void resetQueryParameters() { queryparametermap.clear(); }

  private:

	OGRDataSource* connect(const std::string & host, 
						   const std::string & port,
						   const std::string & dbname,
						   const std::string & user,
						   const std::string & password);

	bool isPolygon(const std::string & name) { return polygonmap.find(name) != polygonmap.end(); }
	bool isPoint(const std::string & name) { return pointmap.find(name) != pointmap.end(); }

	NFmiSvgPath getSVGPath(const std::string & name);	
	NFmiPoint getPoint(const std::string & name);	

	
	std::map<std::string, NFmiSvgPath> polygonmap;
	std::map<std::string, NFmiPoint> pointmap;
	std::map<std::string, int>  queryparametermap;
	

	MessageLogger& theLog;
  }; // class PostGISDataSource
}

#endif // POSTGIS_DATA_SOURCE_H

// ======================================================================
