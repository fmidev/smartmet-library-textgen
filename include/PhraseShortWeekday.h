// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PhraseShortWeekday
 */
// ======================================================================
/*!
 * \class TextGen::PhraseShortWeekday
 * \brief Representation of a dictionary word
 */
// ======================================================================

#ifndef TEXTGEN_PHRASESHORTWEEKDAY_H
#define TEXTGEN_PHRASESHORTWEEKDAY_H

#include "Phrase.h"
#include <string>

namespace TextGen
{
  class Dictionary;

  class PhraseShortWeekday : public Phrase
  {
  public:

	virtual ~PhraseShortWeekday();
	PhraseShortWeekday(int theDay);
#ifdef NO_COMPILER_GENERATED
	PhraseShortWeekday(const PhraseShortWeekday & thePhrase);
	PhraseShortWeekday & operator=(const PhraseShortWeekday & thePhrase);
#endif
	virtual std::auto_ptr<Phrase> clone() const;

	virtual std::string realize() const;
	virtual std::string realize(const Dictionary & theDictionary) const;

  private:

	PhraseShortWeekday();

	int itsDay;

  }; // class PhraseShortWeekday

} // namespace TextGen

#endif // TEXTGEN_PHRASESHORTWEEKDAY_H

// ======================================================================
