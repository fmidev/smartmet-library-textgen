// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace Settings
 */
// ======================================================================

#include "Settings.h"
#include "WeatherResult.h"
#include "NFmiSettings.h"
#include "NFmiStringTools.h"
#include "NFmiTime.h"

#include "boost/lexical_cast.hpp"
#include <cctype>	// for std::isdigit
#include <list>
#include <stdexcept>

using namespace std;
using namespace boost;

namespace Settings
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether the given variable is set
   *
   * \param theName The variable name
   * \return True if the variable is set
   */
  // ----------------------------------------------------------------------
  
  bool isset(const std::string & theName)
  {
	return NFmiSettings::instance().isset(theName);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Require the string value of the given variable
   *
   * Throws if the variable is not set
   *
   * \param theName The variable name
   * \return The value of the variable
   */
  // ----------------------------------------------------------------------
  
  std::string require(const std::string & theName)
  {
	if(!NFmiSettings::instance().isset(theName))
	  throw runtime_error("The variable "+theName+" is required");
	return NFmiSettings::instance().value(theName);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Require the nonempty string value of the given variable
   *
   * Throws if the variable is not set or if the value is empty
   *
   * \param theName The variable name
   * \return The integer value
   */
  // ----------------------------------------------------------------------
  
  std::string require_string(const std::string & theName)
  {
	const string value = require(theName);
	if(value.empty())
	  throw runtime_error("Value of "+theName+" must be nonempty");
	return value;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Require the integer value of the given variable
   *
   * Throws if the variable is not set or if the value is not an integer.
   *
   * \param theName The variable name
   * \return The integer value
   */
  // ----------------------------------------------------------------------
  
  int require_int(const std::string & theName)
  {
	const string value = require(theName);
	try
	  {
		int val = lexical_cast<int>(value);
		return val;
	  }
	catch(exception & e)
	  {
		throw runtime_error(theName + " value " + value + " is not an integer");
	  }
  }
  

  // ----------------------------------------------------------------------
  /*!
   * \brief Require the boolean value of the given variable
   *
   * Throws if the variable is not set or if the value is not
   * "true" or "false" (as strings).
   *
   * \param theName The variable name
   * \return The integer value
   */
  // ----------------------------------------------------------------------
  
  bool require_bool(const std::string & theName)
  {
	const string value = require(theName);

	const string msg = theName+" value '"+value+"' is not true/false";

	if(value == "true")
	  return true;
	if(value == "false")
	  return false;

	throw runtime_error(msg);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Require the double value of the given variable
   *
   * Throws if the variable is not set or if the value is not an integer.
   *
   * \param theName The variable name
   * \return The integer value
   */
  // ----------------------------------------------------------------------
  
  double require_double(const std::string & theName)
  {
	const string value = require(theName);
	try
	  {
		double val = lexical_cast<double>(value);
		return val;
	  }
	catch(exception & e)
	  {
		throw runtime_error(theName + " value " + value + " is not a double");
	  }
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Require the hour value of the given variable
   *
   * Throws if the variable is not set or if the value is not 0-23.
   *
   * \param theName The variable name
   * \return The integer value
   */
  // ----------------------------------------------------------------------
  
  int require_hour(const std::string & theName)
  {
	const int value = require_int(theName);
	if(value>=0 && value<24)
	  return value;
	throw runtime_error(theName+": value "+lexical_cast<string>(value)+" is not 0-23");
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Require the days value of the given variable
   *
   * Throws if the variable is not set or if the value is not >= 0.
   *
   * \param theName The variable name
   * \return The integer value
   */
  // ----------------------------------------------------------------------
  
  int require_days(const std::string & theName)
  {
	const int value = require_int(theName);
	if(value>=0)
	  return value;
	throw runtime_error(theName+": value "+lexical_cast<string>(value)+" is not >= 0");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Require the percentage value of the given variable
   *
   * Throws if the variable is not set or if the value is not 0-100.
   *
   * \param theName The variable name
   * \return The integer value
   */
  // ----------------------------------------------------------------------
  
  int require_percentage(const std::string & theName)
  {
	const int value = require_int(theName);
	if(value>=0 && value<=100)
	  return value;
	throw runtime_error(theName+": value "+lexical_cast<string>(value)+" is not 0-100");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Require the time value of the given variable
   *
   * Throws if the variable is not of the form YYYYMMDDHHMI
   *
   * \param theName The variable name
   * \return The integer value
   */
  // ----------------------------------------------------------------------
  
  NFmiTime require_time(const std::string & theName)
  {
	const string value = require_string(theName);

	const string msg = theName+" value "+value+" is not of form YYYYMMDDHHMI";

	if(value.size() != 12)
	  throw runtime_error(msg);

	for(string::const_iterator it=value.begin(); it!=value.end(); ++it)
	  if(!isdigit(*it))
		throw runtime_error(msg);

	try
	  {
		const int yy = lexical_cast<int>(value.substr(0,4));
		const int mm = lexical_cast<int>(value.substr(4,2));
		const int dd = lexical_cast<int>(value.substr(6,2));
		const int hh = lexical_cast<int>(value.substr(8,2));
		const int mi = lexical_cast<int>(value.substr(10,2));

		if(mm<1 || mm>12) throw runtime_error(msg);
		if(dd<1 || dd>31) throw runtime_error(msg);
		if(hh<0 || hh>23) throw runtime_error(msg);
		if(mi<0 || mi>59) throw runtime_error(msg);

		if(dd > NFmiTime::DaysInMonth(mm,yy))
		  throw runtime_error(msg);

		return NFmiTime(yy,mm,dd,hh,mi);

	  }
	catch(exception & e)
	  {
		throw runtime_error(msg);
	  }

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Require a WeatherResult value from the given variable
   *
   * \param theName The variable name
   * \return The result
   */
  // ----------------------------------------------------------------------

  WeatherAnalysis::WeatherResult require_result(const std::string & theName)
  {
	const string value = require_string(theName);

	const string msg(theName+" value "+value+" is not of form A,B");
	try
	  {
		list<string> values = NFmiStringTools::SplitWords(value);
		if(values.size() != 2)
		  throw runtime_error(msg);
		const float result = lexical_cast<float>(values.front());
		const float accuracy = lexical_cast<float>(values.back());
		return WeatherAnalysis::WeatherResult(result,accuracy);
	  }
	catch(exception & e)
	  {
		throw runtime_error(msg);
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return optional string valued variable, or the given value
   *
   * \param theName The variable name
   * \param theDefault The value to return if the variable is not set
   * \return The optional value
   */
  // ----------------------------------------------------------------------

  std::string optional_string(const std::string & theName,
							  const std::string & theDefault)
  {
	return (isset(theName) ? require_string(theName) : theDefault);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return optional integer valued variable, or the given value
   *
   * \param theName The variable name
   * \param theDefault The value to return if the variable is not set
   * \return The optional value
   */
  // ----------------------------------------------------------------------

  int optional_int(const std::string & theName, int theDefault)
  {
	return (isset(theName) ? require_int(theName) : theDefault);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return optional boolean valued variable, or the given value
   *
   * \param theName The variable name
   * \param theDefault The value to return if the variable is not set
   * \return The optional value
   */
  // ----------------------------------------------------------------------

  bool optional_bool(const std::string & theName, bool theDefault)
  {
	return (isset(theName) ? require_bool(theName) : theDefault);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return optional double valued variable, or the given value
   *
   * \param theName The variable name
   * \param theDefault The value to return if the variable is not set
   * \return The optional value
   */
  // ----------------------------------------------------------------------

  double optional_double(const std::string & theName, double theDefault)
  {
	return (isset(theName) ? require_double(theName) : theDefault);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return optional hour valued variable, or the given value
   *
   * \param theName The variable name
   * \param theDefault The value to return if the variable is not set
   * \return The optional value
   */
  // ----------------------------------------------------------------------

  int optional_hour(const std::string & theName, int theDefault)
  {
	return (isset(theName) ? require_hour(theName) : theDefault);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return optional percentage valued variable, or the given value
   *
   * \param theName The variable name
   * \param theDefault The value to return if the variable is not set
   * \return The optional value
   */
  // ----------------------------------------------------------------------

  int optional_percentage(const std::string & theName, int theDefault)
  {
	return (isset(theName) ? require_percentage(theName) : theDefault);
  }

}

// ======================================================================
