// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PhraseSeparator
 */
// ======================================================================
/*!
 * \class TextGen::PhraseSeparator
 * \brief Representation of a sentence separator, such as ","
 *
 */
// ======================================================================

#ifndef TEXTGEN_PHRASESEPARATOR_H
#define TEXTGEN_PHRASESEPARATOR_H

#include "Phrase.h"
#include <string>

namespace TextGen
{
  class Dictionary;

  class PhraseSeparator : public Phrase
  {
  public:

	virtual ~PhraseSeparator();
	PhraseSeparator(const std::string & theSeparator);
#ifdef NO_COMPILER_GENERATED
	PhraseSeparator(const PhraseSeparator & thePhrase);
	PhraseSeparator & operator=(const PhraseSeparator & thePhrase);
#endif
	virtual std::auto_ptr<Phrase> clone() const;

	virtual std::string wordseparator() const;
	virtual std::string realize() const;
	virtual std::string realize(const Dictionary & theDictionary) const;

  private:

	PhraseSeparator();

	std::string itsSeparator;

  }; // class PhraseSeparator

} // namespace TextGen

#endif // TEXTGEN_PHRASESEPARATOR_H

// ======================================================================
