// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Settings
 */
// ----------------------------------------------------------------------
/*!
 * \namespace Settings
 *
 * \brief Provides parsed and checked access to NFmiSettings
 *
 */
// ----------------------------------------------------------------------

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

class NFmiTime;
namespace WeatherAnalysis
{
  class WeatherResult;
}

namespace Settings
{
  bool isset(const std::string & theName);
  std::string require(const std::string & theName);
  std::string require_string(const std::string & theName);
  int require_int(const std::string & theName);
  bool require_bool(const std::string & theName);
  double require_double(const std::string & theName);
  int require_hour(const std::string & theName);
  int require_days(const std::string & theName);
  int require_percentage(const std::string & theName);
  NFmiTime require_time(const std::string & theName);
  WeatherAnalysis::WeatherResult require_result(const std::string & theName);

  std::string optional_string(const std::string & theName, const std::string & theDefault);
  int optional_int(const std::string & theName, int theDefault);
  bool optional_bool(const std::string & theName, bool theDefault);
  double optional_double(const std::string & theName, double theDefault);
  int optional_percentage(const std::string & theName, int theDefault);

} // namespace Settings

#endif // SETTINGS_H

// ======================================================================

