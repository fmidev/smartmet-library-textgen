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

namespace Settings
{
  bool isset(const std::string & theName);
  std::string require(const std::string & theName);
  std::string require_string(const std::string & theName);
  int require_int(const std::string & theName);
  double require_double(const std::string & theName);
  int require_hour(const std::string & theName);
  int require_days(const std::string & theName);
  int require_percentage(const std::string & theName);
  NFmiTime require_time(const std::string & theName);

} // namespace Settings

#endif // SETTINGS_H

// ======================================================================

