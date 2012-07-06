// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class TextGen::WeatherSource
 */
// ======================================================================

#ifndef TEXTGEN_WEATHERSOURCE_H
#define TEXTGEN_WEATHERSOURCE_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <newbase/NFmiStreamQueryData.h>

class NFmiStreamQueryData;

namespace TextGen
{
  typedef long WeatherId;

  class WeatherSource
  {
  public:

	virtual ~WeatherSource() { }
	virtual boost::shared_ptr<NFmiStreamQueryData> data(const std::string & theName) const = 0;
	virtual WeatherId id(const std::string & theName) const = 0;

  }; // class WeatherSource

} // namespace TextGen

#endif // TEXTGEN_WEATHERSOURCE_H

// ======================================================================

