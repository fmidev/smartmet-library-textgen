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
 */
// ======================================================================

#ifndef TEXTGEN_SENTENCE_H
#define TEXTGEN_SENTENCE_H

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
	Sentence(const std::string & thePhrase);
	Sentence & operator=(const Sentence & theSentence);

	void swap(Sentence & theSentence);

	Sentence & operator+=(const std::string & thePhrase);
	Sentence & operator+=(const Sentence & theSentence);

	std::string realize() const;
	std::string realize(const Dictionary & theDictionary) const;

	bool empty() const;
	size_t size() const;

  private:

	std::auto_ptr<SentencePimple> itsPimple;

  }; // class Sentence

  // Free operators

  Sentence operator+(const Sentence & theLhs, const Sentence & theRhs);
  Sentence operator+(const Sentence & theLhs, const std::string & theRhs);
  Sentence operator+(const std::string & theLhs, const Sentence & theRhs);

  void swap(Sentence & theLhs, Sentence & theRhs);


} // namespace TextGen

#endif // TEXTGEN_SENTENCE_H

// ======================================================================
