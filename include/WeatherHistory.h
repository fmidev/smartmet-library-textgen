#ifndef WEATHERANALYSIS_WEATHERHISTORY_H
#define WEATHERANALYSIS_WEATHERHISTORY_H

#include <newbase/NFmiTime.h>
#include <string>

namespace WeatherAnalysis
{

  class WeatherHistory
  {
  public:

    WeatherHistory();
	WeatherHistory(const WeatherHistory& theWeatherHistory);
	//	WeatherHistory& operator=(const WeatherHistory& theWeatherHistory);

	void updateTimePhrase(const std::string& thePhrase, const NFmiTime& theTime);

	NFmiTime latestDate;
    std::string latestTimePhrase;
  };


} // namespace WeatherAnalysis


#endif //  WEATHERANALYSIS_WEATHERHISTORY_H
