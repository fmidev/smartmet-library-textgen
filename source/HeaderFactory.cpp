// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace HeaderFactory
 */
// ======================================================================

#include "HeaderFactory.h"
#include "Header.h"
#include "WeatherPeriod.h"
#include "TextGenError.h"

#include "NFmiSettings.h"

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

	header << "odotettavissa";

	const int endhour = thePeriod.localEndTime().GetHour();
	string tmp = lexical_cast<string>(thePeriod.localEndTime().GetWeekday());
	if(endhour == 6)
	  tmp += "-aamuun";
	else if(endhour == 18)
	  tmp += "-iltaan";
	else
	  throw TextGenError("HeaderFactory: until end time must be 06 or 18");
	header << tmp;
	header << "asti";

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

	const int starthour = thePeriod.localStartTime().GetHour();
	const int endhour = thePeriod.localEndTime().GetHour();

	header << "odotettavissa";

	string tmp = lexical_cast<string>(thePeriod.localStartTime().GetWeekday());
	if(starthour == 6)
	  tmp += "-aamusta";
	else if(starthour == 18)
	  tmp += "-illasta";
	else
	  throw TextGenError("HeaderFactory: from_until start time must be 06 or 18");

	header << tmp;

	tmp = lexical_cast<string>(thePeriod.localEndTime().GetWeekday());
	if(endhour == 6)
	  tmp += "-aamuun";
	else if(endhour == 18)
	  tmp += "-iltaan";
	else
	  throw TextGenError("HeaderFactory: from_until end time must be 06 or 18");

	header << tmp;

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

	const int starthour = thePeriod.localStartTime().GetHour();

	string tmp = lexical_cast<string>(thePeriod.localStartTime().GetWeekday());

	if(starthour == 6)
	  tmp += "-aamusta";
	else if(starthour == 18)
	  tmp += "-illasta";
	else
	  throw TextGenError("HeaderFactory:: several_days start time must be 06 or 18");

	header << tmp;
	header << "alkavan";

	const NFmiTime & startTime = thePeriod.localStartTime();
	const NFmiTime & endTime = thePeriod.localEndTime();
	const long diff = endTime.DifferenceInHours(startTime);

	if(diff % 24 != 0)
	  throw TextGenError("HeaderFactory:: several_days must be N*24 hours long");
	const long days = diff/24;
	tmp = lexical_cast<string>(days);
	tmp += "-vuorokauden s‰‰";

	header << tmp;

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
	 * \param thePeriod The relevant weather period
	 * \param theVariable The variable
	 * \return The generated header
	 */
	// ----------------------------------------------------------------------

	Header create(const WeatherPeriod & thePeriod,
				  const std::string & theVariable)
	{
	  const string type = NFmiSettings::instance().require(theVariable+"::type");

	  if(type == "none")
		return header_none(thePeriod,theVariable);
	  if(type == "until")
		return header_until(thePeriod,theVariable);
	  if(type == "from_until")
		return header_from_until(thePeriod,theVariable);
	  if(type == "several_days")
		return header_several_days(thePeriod,theVariable);
	  throw TextGenError("HeaderFactory does not recognize header type "+type);
	}

  } // namespace HeaderFactory
} // namespace TextGen
