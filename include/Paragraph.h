// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Paragraph
 */
// ======================================================================
/*!
 * \class TextGen::Paragraph
 *
 * \brief A sequence of sentences.
 *
 * The responsibility of the Paragraph class is to contain
 * a list of sentences, which can then be converted into text.
 *
 */
// ======================================================================

#ifndef TEXTGEN_PARAGRAPH_H
#define TEXTGEN_PARAGRAPH_H

#include <memory>
#include <string>

namespace TextGen
{
  class Dictionary;
  class Sentence;
  
  class Paragraph
  {
  public:
	~Paragraph();
	Paragraph();
	Paragraph(const Paragraph & theParagraph);
	Paragraph(const Sentence & theSentence);
	Paragraph & operator=(const Paragraph & theParagraph);
	
	void swap(Paragraph & theParagraph);

	Paragraph & operator<<(const Paragraph & theParagraph);
	Paragraph & operator<<(const Sentence & theSentence);

	std::string realize() const;
	std::string realize(const Dictionary & theDictionary) const;

	bool empty() const;
	size_t size() const;

  private:

	class Pimple;
	std::auto_ptr<Pimple> itsPimple;

  }; // class Paragraph

  // Free operators

  void swap(Paragraph & theLhs, Paragraph & theRhs);


} // namespace TextGen

#endif // TEXTGEN_PARAGRAPH_H

// ======================================================================
