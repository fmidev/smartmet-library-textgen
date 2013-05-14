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

  void WeatherHistory::updateTimePhrase(const std::string& thePhrase,
										const TextGenPosixTime& theTime)
  {
	latestTimePhrase = thePhrase;
	latestDate = theTime;
  }

}
