// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace HeaderFactory
 */
// ======================================================================

#include "HeaderFactory.h"
#include "Header.h"
#include "MessageLogger.h"
#include "Number.h"
#include "Settings.h"
#include "TextGenError.h"
#include "WeatherArea.h"
#include "WeatherPeriod.h"
#include "WeekdayTools.h"

#include "boost/lexical_cast.hpp"

using namespace WeatherAnalysis;
using namespace std;
using namespace boost;

// ======================================================================
//				IMPLEMENTATION HIDING FUNCTIONS
// ======================================================================

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return empty header
   *
   * \param thePeriod The weather period
   * \param theVariable The variable for extra settings
   * \return The header
   */
  // ----------------------------------------------------------------------

  TextGen::Header header_none(const WeatherPeriod & thePeriod,
							  const string & theVariable)
  {
	using namespace TextGen;
	Header header;
	return header;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return header of type "Odotettavissa tiistai-iltaan asti"
   *
   * Note that the ending time must be either 06 or 18 local time,
   * otherwise an exception is thrown.
   *
   * \param thePeriod The weather period
   * \param theVariable The variable for extra settings
   * \return The header
   */
  // ----------------------------------------------------------------------

  TextGen::Header header_until(const WeatherPeriod & thePeriod,
							   const string & theVariable)
  {
	using namespace TextGen;
	Header header;

	header << "odotettavissa"
		   << WeekdayTools::until_weekday_time(thePeriod.localEndTime())
		   << "asti";

	return header;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return header of type "Odotettavissa tiistai-illasta torstaiaamuun asti"
   *
   * Note that the start and end times must be either 06 or 18 local time,
   * otherwise an exception is thrown.
   *
   * \param thePeriod The weather period
   * \param theVariable The variable for extra settings
   * \return The header
   */
  // ----------------------------------------------------------------------

  TextGen::Header header_from_until(const WeatherPeriod & thePeriod,
									const string & theVariable)
  {
	using namespace TextGen;
	Header header;

	header << "odotettavissa"
		   << WeekdayTools::from_weekday_time(thePeriod.localStartTime())
		   << WeekdayTools::until_weekday_time(thePeriod.localEndTime());

	return header;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return header of type "Odotettavissa seuraavan viiden vuorokauden aikana"
   *
   * The period start time must be 06 or 18. The period length must be
   * a multiple of 24.
   *
   * \param thePeriod The weather period
   * \param theVariable The variable for extra settings
   * \return The header
   */
  // ----------------------------------------------------------------------

  TextGen::Header header_several_days(const WeatherPeriod & thePeriod,
									  const string & theVariable)
  {
	using namespace TextGen;
	Header header;

	const NFmiTime & startTime = thePeriod.localStartTime();
	const NFmiTime & endTime = thePeriod.localEndTime();
	const long diff = endTime.DifferenceInHours(startTime);
	const long days = diff/24;

	if(diff % 24 != 0)
	  throw TextGenError("HeaderFactory:: several_days must be N*24 hours long");
	header << WeekdayTools::from_weekday_time(thePeriod.localStartTime())
		   << "alkavan"
		   << lexical_cast<string>(days) + "-vuorokauden s‰‰";

	return header;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return report header for specific area
   *
   * \param theArea The area (must not be a point)
   * \param thePeriod The time period (only start time is relevant)
   * \param theVariable The variable for extra settings
   * \return The header
   */
  // ----------------------------------------------------------------------

  TextGen::Header header_report_area(const WeatherArea & theArea,
									 const WeatherPeriod & thePeriod,
									 const string & theVariable)
  {
	using namespace TextGen;

	Header header;

	if(!theArea.isNamed())
	  throw TextGenError("Cannot generate report_area title for an unnamed point");

	const int starthour = thePeriod.localStartTime().GetHour();

	header << "s‰‰ennuste"
		   << theArea.name()+":lle"
		   << WeekdayTools::on_weekday_time(thePeriod.localStartTime())
		   << Number<int>(starthour)
		   << "o'clock";

	return header;
  }

} // namespace anonymous

// ======================================================================
//				PUBLIC INTERFACE
// ======================================================================

namespace TextGen
{
  namespace HeaderFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Produce headers of desired type
	 *
	 * Throws if the type is unknown.
	 *
	 * \see page_aikavalit
	 *
	 * \param theArea The area concerned
	 * \param thePeriod The relevant weather period
	 * \param theVariable The variable
	 * \return The generated header
	 */
	// ----------------------------------------------------------------------

	Header create(const WeatherArea & theArea,
				  const WeatherPeriod & thePeriod,
				  const std::string & theVariable)
	{
	  MessageLogger log("HeaderFactory::create");

	  const string type = Settings::require_string(theVariable+"::type");

	  if(type == "none")
		return header_none(thePeriod,theVariable);
	  if(type == "until")
		return header_until(thePeriod,theVariable);
	  if(type == "from_until")
		return header_from_until(thePeriod,theVariable);
	  if(type == "several_days")
		return header_several_days(thePeriod,theVariable);
	  if(type == "report_area")
		return header_report_area(theArea,thePeriod,theVariable);

	  throw TextGenError("HeaderFactory does not recognize header type "+type);
	}

  } // namespace HeaderFactory
} // namespace TextGen

// ======================================================================
