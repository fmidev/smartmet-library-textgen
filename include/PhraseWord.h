// ======================================================================
/*!
 * \file
 * \brief Interface of class PhraseWord
 */
// ======================================================================
/*!
 * \class TextGen::PhraseWord
 * \brief Representation of a dictionary word
 */
// ======================================================================

#ifndef TEXTGEN_PHRASEWORD_H
#define TEXTGEN_PHRASEWORD_H

#include "Phrase.h"
#include <string>

namespace TextGen
{
  class Dictionary;

  class PhraseWord : public Phrase
  {
  public:

	virtual ~PhraseWord();
	PhraseWord(const std::string & theWord);
#ifdef NO_COMPILER_GENERATED
	PhraseWord(const PhraseWord & thePhrase);
	PhraseWord & operator=(const PhraseWord & thePhrase);
#endif

	virtual std::string realize() const;
	virtual std::string realize(const Dictionary & theDictionary) const;

  private:

	PhraseWord();

	std::string itsWord;

  }; // class PhraseWord

} // namespace TextGen

#endif // TEXTGEN_PHRASEWORD_H

// ======================================================================
