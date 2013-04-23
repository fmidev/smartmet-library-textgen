// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Time
 */
// ======================================================================

#ifndef TEXTGEN_TIME_H
#define TEXTGEN_TIME_H

#include "Glyph.h"
#include "TextGenPosixTime.h"
#include <string>

namespace TextGen
{
  class Dictionary;

  class Time : public Glyph
  {
  public:

	virtual ~Time();
	Time(const TextGenPosixTime& theTime);
#ifdef NO_COMPILER_GENERATED
	Time(const Time& theTime);
	Time & operator=(const Time& theTime);
#endif
	virtual boost::shared_ptr<Glyph> clone() const;
	virtual std::string realize(const Dictionary & theDictionary) const;
	virtual std::string realize(const TextFormatter & theFormatter) const;
	virtual bool isDelimiter() const;

	inline const TextGenPosixTime& nfmiTime() const { return itsTime; }
  private:

	Time();
	TextGenPosixTime itsTime;

  }; // class Time

} // namespace TextGen

#endif // TEXTGEN_TIME_H

// ======================================================================
