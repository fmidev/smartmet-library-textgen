// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace Settings
 */
// ======================================================================
/*!
 * \namespace Settings
 *
 *†\brief Provides parsed and checked access to NFmiSettings
 *
 */
// ----------------------------------------------------------------------

// boost included laitettava ennen newbase:n NFmiGlobals-includea,
// muuten MSVC:ss‰ min max m‰‰rittelyt jo tehty

#include "boost/lexical_cast.hpp"

#include "Settings.h"
#include "WeatherResult.h"
#include "NFmiSettings.h"
#include "NFmiStringTools.h"
#include "NFmiTime.h"

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
	return NFmiSettings::IsSet(theName);
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
  
  const std::string require(const std::string & theName)
  {
	return NFmiSettings::Require<string>(theName.c_str());
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
  
  const std::string require_string(const std::string & theName)
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
	return NFmiSettings::Require<int>(theName.c_str());
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
	return NFmiSettings::Require<bool>(theName.c_str());
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
	return NFmiSettings::Require<double>(theName.c_str());
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
	return NFmiSettings::RequireRange<int>(theName.c_str(),0,23);
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
	const int maxdays = 100000;
	return NFmiSettings::RequireRange<int>(theName.c_str(),0,maxdays);
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
	return NFmiSettings::RequireRange<int>(theName.c_str(),0,100);
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
  
  const NFmiTime require_time(const std::string & theName)
  {
	const string value = require_string(theName.c_str());

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
	catch(exception & )
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

  const WeatherAnalysis::WeatherResult require_result(const std::string & theName)
  {
	const string value = require_string(theName.c_str());

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
	catch(exception & )
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

  const std::string optional_string(const std::string & theName,
									const std::string & theDefault)
  {
	return NFmiSettings::Optional<string>(theName.c_str(),theDefault);
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
	return NFmiSettings::Optional<int>(theName.c_str(),theDefault);
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
	return NFmiSettings::Optional<bool>(theName.c_str(),theDefault);
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
	return NFmiSettings::Optional<double>(theName.c_str(),theDefault);
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
	return NFmiSettings::OptionalRange<int>(theName.c_str(),theDefault,0,23);
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
	return NFmiSettings::OptionalRange<int>(theName.c_str(),theDefault,0,100);
  }

}

// ======================================================================
