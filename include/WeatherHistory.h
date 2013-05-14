#ifndef TEXTGEN_WEATHERHISTORY_H
#define TEXTGEN_WEATHERHISTORY_H

#include "TextGenPosixTime.h"
#include <string>

namespace TextGen
{

  class WeatherHistory
  {
  public:

    WeatherHistory();
	WeatherHistory(const WeatherHistory& theWeatherHistory);

	void updateTimePhrase(const std::string& thePhrase, const TextGenPosixTime& theTime);

	TextGenPosixTime latestDate;
    std::string latestTimePhrase;
  };


} // namespace TextGen


#endif //  TEXTGEN_WEATHERHISTORY_H
