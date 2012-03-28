// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::WeekdayTools
 */
// ======================================================================
/*!
 * \namespace TextGen::WeekdayTools
 *
 * \brief Tools for generating weekday phrases
 *
 */
// ----------------------------------------------------------------------

#include <boost/lexical_cast.hpp> // boost included laitettava ennen newbase:n NFmiGlobals-includea, muuten MSVC:ssa min max maarittelyt jo tehty

#include "WeatherHistory.h"
#include "WeekdayTools.h"
#include "TextGenError.h"
#include <newbase/NFmiTime.h>

using namespace std;
using namespace boost;

namespace TextGen
{
  namespace WeekdayTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-na" description based on the given time
	 *
	 * \param theTime The time of the day
	 * \return The "N-na" phrase
	 */
	// ----------------------------------------------------------------------

	string on_weekday(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-na");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-na kello" description based on the given time
	 *
	 * \param theTime The time of the day
	 * \return The "N-na kello" phrase
	 */
	// ----------------------------------------------------------------------

	string on_weekday_time(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-na kello");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-vastaisena yona" phrase
	 *
	 * \param theTime The time
	 * \return The "N-vastaisena yona" phrase
	 */
	// ----------------------------------------------------------------------

	string night_against_weekday(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-vastaisena yona");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamuun" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamuun" phrase
	 */
	// ----------------------------------------------------------------------

	string until_weekday_morning(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-aamuun");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-iltaan" phrase
	 *
	 * \param theTime The time
	 * \return The "N-iltaan" phrase
	 */
	// ----------------------------------------------------------------------

	string until_weekday_evening(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-iltaan");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamuun" or "N-iltaan" phrase
	 *
	 * \param theTime The time
	 * \return The phrase
	 */
	// ----------------------------------------------------------------------

	string until_weekday_time(const NFmiTime & theTime)
	{
	  const int hour = theTime.GetHour();
	  if(hour == 6)
		return until_weekday_morning(theTime);
	  else if(hour == 18)
		return until_weekday_evening(theTime);
	  else
		{
		  const string msg = "WeekdayTools::until_weekday_time: Cannot generate -aamuun or -iltaan phrase for hour "+lexical_cast<int>(hour);
		  throw TextGenError(msg);
		}
	}


	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-alkaen" phrase
	 *
	 * \param theTime The time
	 * \return The "N-alkaen" phrase
	 */
	// ----------------------------------------------------------------------

	string from_weekday(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-alkaen");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamusta" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamusta" phrase
	 */
	// ----------------------------------------------------------------------

	string from_weekday_morning(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-aamusta");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-illasta" phrase
	 *
	 * \param theTime The time
	 * \return The "N-illasta" phrase
	 */
	// ----------------------------------------------------------------------

	string from_weekday_evening(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-illasta");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamusta" or "N-illasta" phrase
	 *
	 * \param theTime The time
	 * \return The phrase
	 */
	// ----------------------------------------------------------------------

	string from_weekday_time(const NFmiTime & theTime)
	{
	  const int hour = theTime.GetHour();
	  if(hour == 6)
		return from_weekday_morning(theTime);
	  else if(hour == 18)
		return from_weekday_evening(theTime);
	  else
		{
		  const string msg = "WeekdayTools::from_weekday_time: Cannot generate -aamusta or -illasta phrase for hour "+lexical_cast<int>(hour);
		  throw TextGenError(msg);
		}
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamulla" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamulla" phrase
	 */
	// ----------------------------------------------------------------------

	string on_weekday_morning(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-aamulla");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-aamupaivalla" phrase
	 *
	 * \param theTime The time
	 * \return The "N-aamupaivalla" phrase
	 */
	// ----------------------------------------------------------------------

	string on_weekday_forenoon(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-aamupaivalla");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-iltapaivalla" phrase
	 *
	 * \param theTime The time
	 * \return The "N-iltapaivalla" phrase
	 */
	// ----------------------------------------------------------------------

	string on_weekday_afternoon(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-iltapaivalla");
	  return out;
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Return the "N-illalla" phrase
	 *
	 * \param theTime The time
	 * \return The "N-illalla" phrase
	 */
	// ----------------------------------------------------------------------

	string on_weekday_evening(const NFmiTime & theTime)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-illalla");
	  return out;
	}



	//using namespace WeatherAnalysis;

	std::string get_time_phrase(const NFmiTime & theTime, 
								const std::string theNewPhrase, 
								WeatherAnalysis::WeatherHistory& theHistory)
	{
	  NFmiTime oldTime = theHistory.latestDate;
	  const std::string& oldPhrase = theHistory.latestTimePhrase;

	  if(theTime.GetJulianDay() == oldTime.GetJulianDay())
		{
		  if(oldPhrase.compare(theNewPhrase) != 0)
			{
			  theHistory.updateTimePhrase(theNewPhrase, theTime);
			  return theNewPhrase;
			}
		  else
			{
			  return "";
			}
		}
	  else
		{
		  theHistory.updateTimePhrase(theNewPhrase, theTime);
		  return theNewPhrase;
		}

	  return "";
	}

	std::string on_weekday(const NFmiTime & theTime, 
						   WeatherAnalysis::WeatherHistory& theHistory)
	{

	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-na");

	  return get_time_phrase(theTime, out, theHistory);
	}
   
	string on_weekday_time(const NFmiTime & theTime,
						   WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-na kello");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string night_against_weekday(const NFmiTime & theTime,
								 WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-vastaisena yona");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string until_weekday_morning(const NFmiTime & theTime,
								 WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-aamuun");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string until_weekday_evening(const NFmiTime & theTime,
								 WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-iltaan");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string until_weekday_time(const NFmiTime & theTime,
							  WeatherAnalysis::WeatherHistory& theHistory)
	{
	  const int hour = theTime.GetHour();
	  if(hour == 6)
		return until_weekday_morning(theTime, theHistory);
	  else if(hour == 18)
		return until_weekday_evening(theTime, theHistory);
	  else
		{
		  const string msg = "WeekdayTools::until_weekday_time: Cannot generate -aamuun or -iltaan phrase for hour "+lexical_cast<int>(hour);
		  throw TextGenError(msg);
		}
	}

	string from_weekday(const NFmiTime & theTime,
						WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-alkaen");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string from_weekday_morning(const NFmiTime & theTime,
								WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-aamusta");
	  return get_time_phrase(theTime, out, theHistory);
	}
	
	string from_weekday_evening(const NFmiTime & theTime,
								WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-illasta");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string from_weekday_time(const NFmiTime & theTime,
							 WeatherAnalysis::WeatherHistory& theHistory)
	{
	  const int hour = theTime.GetHour();
	  if(hour == 6)
		return from_weekday_morning(theTime, theHistory);
	  else if(hour == 18)
		return from_weekday_evening(theTime, theHistory);
	  else
		{
		  const string msg = "WeekdayTools::from_weekday_time: Cannot generate -aamusta or -illasta phrase for hour "+lexical_cast<int>(hour);
		  throw TextGenError(msg);
		}
	}

	string on_weekday_morning(const NFmiTime & theTime,
							  WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-aamulla");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string on_weekday_forenoon(const NFmiTime & theTime,
							   WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-aamupaivalla");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string on_weekday_afternoon(const NFmiTime & theTime,
								WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-iltapaivalla");
	  return get_time_phrase(theTime, out, theHistory);
	}

	string on_weekday_evening(const NFmiTime & theTime,
							  WeatherAnalysis::WeatherHistory& theHistory)
	{
	  string out = (lexical_cast<string>(theTime.GetWeekday())
					+"-illalla");
	  return get_time_phrase(theTime, out, theHistory);
	}






	/*
	const std::string on_weekday_time(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string night_against_weekday(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string until_weekday_morning(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string until_weekday_evening(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string until_weekday_time(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string from_weekday(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string from_weekday_morning(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string from_weekday_evening(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string from_weekday_time(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string on_weekday_morning(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string on_weekday_forenoon(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string on_weekday_afternoon(const NFmiTime & theTime, const WeatherHistory& theHistory);
	const std::string on_weekday_evening(const NFmiTime & theTime, const WeatherHistory& theHistory);


WeekdayTools.h headeriin pitaa sitten lisata tarvittavia funktioita
tyyliin

  const std::string on_weekday_time(const NFmiTime & theTime,
                                    WeatherHistory & theHistory)
  {
      // jos paiva on sama kuin historiassa, tutkitaan vain kellon
      // aikaa, ja verrataan valittua fraasia viimeeksi kaytettyyn.
      // Muutoin pitaa generoida fraasi jossa mainitaan myos
      // paiva, ja se olisi joko tanaan, huomenna, ylihuomenna
      // tai viikonpaivana.
  }
	*/



  } // namespace WeekdayTools
} // namespace TextGen

// ======================================================================
