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

using namespace WeatherAnalysis;


// ======================================================================
//				IMPLEMENTATION HIDING FUNCTIONS
// ======================================================================

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return header of type "Odotettavissa tiistai-iltaan asti"
   *
   * Note that the ending time must be either 06 or 18 local time,
   * otherwise an exception is thrown.
   *
   * \param thePeriod The weather period
   * \return The header
   */
  // ----------------------------------------------------------------------

  TextGen::Header header_until(const WeatherPeriod & thePeriod)
  {
	using namespace TextGen;
	Header header;
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
   * \return The header
   */
  // ----------------------------------------------------------------------

  TextGen::Header header_from_until(const WeatherPeriod & thePeriod)
  {
	using namespace TextGen;
	Header header;
	return header;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return header of type "Odotettavissa seuraavan viiden vuorokauden aikana"
   *
   * The given weather period is not used at the moment.
   *
   * \param thePeriod The weather period
   * \return The header
   */
  // ----------------------------------------------------------------------

  TextGen::Header header_five_days(const WeatherPeriod & thePeriod)
  {
	using namespace TextGen;
	Header header;
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
	 * \param theName The name of the desired header type
	 * \return The generated header
	 */
	// ----------------------------------------------------------------------

	Header create(const WeatherPeriod & thePeriod,
				  const std::string & theName)
	{
	  if(theName == "header_until")
		return header_until(thePeriod);
	  if(theName == "header_from_until")
		return header_from_until(thePeriod);
	  if(theName == "header_five_days")
		return header_five_days(thePeriod);
	  throw TextGenError("HeaderFactory does not recognize header type "+theName);
	}

  } // namespace HeaderFactory
} // namespace TextGen
