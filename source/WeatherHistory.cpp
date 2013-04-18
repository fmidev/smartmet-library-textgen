#include "WeatherHistory.h"

using namespace std;

namespace TextGen
{

  WeatherHistory::WeatherHistory()
	: latestDate(1970,1,1)
	, latestTimePhrase()
  {
  }

  WeatherHistory::WeatherHistory(const WeatherHistory& theWeatherHistory)
	: latestDate(theWeatherHistory.latestDate)
	, latestTimePhrase(theWeatherHistory.latestTimePhrase)
  {
  }
  /*
  WeatherHistory& WeatherHistory::operator=(const WeatherHistory& theWeatherHistory)
  {
	latestTimePhrase = theWeatherHistory.latestTimePhrase;
	latestDate = theWeatherHistory.latestDate;

	return *this;
  }
  */

  void WeatherHistory::updateTimePhrase(const std::string& thePhrase,
										const TextGenTime& theTime)
  {
	latestTimePhrase = thePhrase;
	latestDate = theTime;
  }

}
