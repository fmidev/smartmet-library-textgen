// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace Settings
 */
// ======================================================================

#include "Settings.h"
#include "NFmiSettings.h"

#include "boost/lexical_cast.hpp"

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
  
  
}

// ======================================================================

