// ======================================================================
/*!
 * \file
 * \brief Interface of singleton TimeZoneFactory
 */
// ======================================================================

#ifndef TIMEZONEFACTORY_H
#define TIMEZONEFACTORY_H

#include <boost/shared_ptr.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <string>

class TimeZoneFactory
{
public:
  static TimeZoneFactory & instance();

  void set_region_file(const std::string & file);
  boost::local_time::time_zone_ptr time_zone_from_region(const std::string & id) const;
  std::vector<std::string> region_list() const;

private:

  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

  // Singleton protection

  ~TimeZoneFactory();
  TimeZoneFactory();
  TimeZoneFactory(const TimeZoneFactory & theFactory);
  TimeZoneFactory & operator=(const TimeZoneFactory & theFactory);

}; // class TimeZoneFactory

#endif // TIMEZONEFACTORY_H

// ======================================================================
