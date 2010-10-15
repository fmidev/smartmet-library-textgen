#include "WeatherHistory.h"

using namespace std;

namespace WeatherAnalysis
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
										const NFmiTime& theTime)
  {
	latestTimePhrase = thePhrase;
	latestDate = theTime;
  }

}
