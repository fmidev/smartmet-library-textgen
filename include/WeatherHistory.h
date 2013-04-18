#ifndef TEXTGEN_WEATHERHISTORY_H
#define TEXTGEN_WEATHERHISTORY_H

#include "TextGenTime.h"
#include <string>

namespace TextGen
{

  class WeatherHistory
  {
  public:

    WeatherHistory();
	WeatherHistory(const WeatherHistory& theWeatherHistory);
	//	WeatherHistory& operator=(const WeatherHistory& theWeatherHistory);

	void updateTimePhrase(const std::string& thePhrase, const TextGenTime& theTime);

	TextGenTime latestDate;
    std::string latestTimePhrase;
  };


} // namespace TextGen


#endif //  TEXTGEN_WEATHERHISTORY_H
