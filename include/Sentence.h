// ======================================================================
/*!
 * \file
 * \brief Interface of class Sentence
 */
// ======================================================================
/*!
 * \class TextGen::Sentence
 * \brief Representation of a sentence.
 *
 * The responsibility of the Sentence class is to store a sequence
 * of phrases and provide means for translating it by using a
 * dictionary.
 *
 * Sample code:
 * \code
 * std::auto_ptr<Dictionary> dict(DictionaryFactory::create("mysql"));
 * dict->init("fi");
 *
 * Sentence s;
 * s += "lämpötila";
 * s += PhraseWord("kaakko");
 * s += PhraseNumber<int>(12);
 * cout << s.realize(*dict) << endl;
 *
 * TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
 * TheDictionary::instance().init("en");
 *
 * cout << s.realize() << endl;
 *
 * \endcode
 */
// ======================================================================

#ifndef TEXTGEN_SENTENCE_H
#define TEXTGEN_SENTENCE_H

#include "Phrase.h"
#include <memory>
#include <string>

namespace TextGen
{
  class Dictionary;
  class SentencePimple;

  class Sentence
  {
  public:

	~Sentence();
	Sentence();
	Sentence(const Sentence & theSentence);
	Sentence(const Phrase & thePhrase);
	Sentence(const std::string & thePhrase);
	Sentence(int theValue);
	Sentence & operator=(const Sentence & theSentence);

	void swap(Sentence & theSentence);

	Sentence & operator+=(const Sentence & theSentence);
	Sentence & operator+=(const Phrase & thePhrase);
	Sentence & operator+=(const std::string & thePhrase);
	Sentence & operator+=(int theValue);

	Sentence & operator<<(const Sentence & theSentence);
	Sentence & operator<<(const Phrase & thePhrase);
	Sentence & operator<<(const std::string & thePhrase);
	Sentence & operator<<(int theValue);

	std::string realize() const;
	std::string realize(const Dictionary & theDictionary) const;

	bool empty() const;
	size_t size() const;

  private:

	std::auto_ptr<SentencePimple> itsPimple;

  }; // class Sentence

  // Free operators

  Sentence operator+(const Sentence & theLhs, const Sentence & theRhs);
  Sentence operator+(const Sentence & theLhs, const Phrase & theRhs);
  Sentence operator+(const Phrase & theLhs, const Sentence & theRhs);
  Sentence operator+(const Sentence & theLhs, const std::string & theRhs);
  Sentence operator+(const std::string & theLhs, const Sentence & theRhs);
  Sentence operator+(const Sentence & theLhs, int theRhs);
  Sentence operator+(int theLhs, const Sentence & theRhs);

  void swap(Sentence & theLhs, Sentence & theRhs);


} // namespace TextGen

#endif // TEXTGEN_SENTENCE_H

// ======================================================================
