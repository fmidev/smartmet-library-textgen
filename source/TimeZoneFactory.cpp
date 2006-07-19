// ======================================================================
/*!
 * \brief Implementation of singleton TimeZoneFactory
 */
// ======================================================================

#include "TimeZoneFactory.h"

using namespace std;

// ----------------------------------------------------------------------
/*!
 * \brief Implementation hiding pimple
 */
// ----------------------------------------------------------------------

class TimeZoneFactory::Pimple
{
public:
  string itsFile;
  boost::shared_ptr<boost::local_time::tz_database> itsDatabase;

}; // Pimple

// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

TimeZoneFactory::~TimeZoneFactory()
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Private constructor for instance()
 */
// ----------------------------------------------------------------------

TimeZoneFactory::TimeZoneFactory()
  : itsPimple(new Pimple())
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Return an instance of TimeZoneFactory
 */
// ----------------------------------------------------------------------

TimeZoneFactory & TimeZoneFactory::instance()
{
  static TimeZoneFactory factory;
  return factory;
}

// ----------------------------------------------------------------------
/*!
 * \brief Set the time zone database filename
 */
// ----------------------------------------------------------------------

void TimeZoneFactory::set_region_file(const string & file)
{
  itsPimple->itsFile = file;
  itsPimple->itsDatabase.reset(new boost::local_time::tz_database());
  itsPimple->itsDatabase->load_from_file(itsPimple->itsFile);
}

// ----------------------------------------------------------------------
/*!
 * \brief Create a time zone given a region name
 */
// ----------------------------------------------------------------------

boost::local_time::time_zone_ptr
TimeZoneFactory::time_zone_from_region(const string & id) const
{
  if(itsPimple->itsDatabase == 0)
	throw runtime_error("TimeZoneFactory not initialized with a file");
  boost::local_time::time_zone_ptr ptr = itsPimple->itsDatabase->time_zone_from_region(id);
  if(ptr == 0)
	throw runtime_error("TimeZoneFactory does not recognize region '"+id+"'");
  return ptr;
}

// ----------------------------------------------------------------------
/*!
 * \brief List the known databases
 */
// ----------------------------------------------------------------------

vector<string> TimeZoneFactory::region_list() const
{
  if(itsPimple->itsDatabase == 0)
	throw runtime_error("TimeZoneFactory not initialized with a file");
  return itsPimple->itsDatabase->region_list();
}
