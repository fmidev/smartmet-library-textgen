// ======================================================================
/*!
 * \file
 * \brief Implementation of class Paragraph
 */
// ======================================================================

#include "Paragraph.h"
#include "Dictionary.h"
#include "Sentence.h"
#include "TheDictionary.h"
#include <algorithm>
#include <cctype>
#include <clocale>
#include <list>

using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \class TextGen::ParagraphPimple
   * \brief Implementation hiding pimple for class Paragraph
   */
  // ----------------------------------------------------------------------

  class ParagraphPimple
  {
  public:
	~ParagraphPimple() { }
	ParagraphPimple() : itsData() { }

	typedef list<Sentence> storage_type;
	storage_type itsData;

  }; // class ParagraphPimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  Paragraph::~Paragraph()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  Paragraph::Paragraph()
  {
	itsPimple.reset(new ParagraphPimple());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Copy constructor
   *
   * \param theParagraph The sentence to be copied
   */
  // ----------------------------------------------------------------------

  Paragraph::Paragraph(const Paragraph & theParagraph)
  {
	itsPimple.reset(new ParagraphPimple(*theParagraph.itsPimple));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theSentence The initial sentence
   */
  // ----------------------------------------------------------------------

  Paragraph::Paragraph(const Sentence & theSentence)
  {
	itsPimple.reset(new ParagraphPimple());
	*this << theSentence;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Assignment operator
   *
   * \param theParagraph The sentence to be copied
   * \return The paragraph assigned to
   */
  // ----------------------------------------------------------------------

  Paragraph & Paragraph::operator=(const Paragraph & theParagraph)
  {
	if(this != &theParagraph)
	  itsPimple.reset(new ParagraphPimple(*theParagraph.itsPimple));
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief swap method
   *
   * \param theParagraph The paragraph to swap contents with
   */
  // ----------------------------------------------------------------------

  void Paragraph::swap(Paragraph & theParagraph)
  {
	ParagraphPimple * a = itsPimple.release();
	ParagraphPimple * b = theParagraph.itsPimple.release();
	itsPimple.reset(b);
	theParagraph.itsPimple.reset(a);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a sentence to a paragraph
   *
   * \param theSentence The sentence to be added
   * \result The paragraph added to
   */
  // ----------------------------------------------------------------------

  Paragraph & Paragraph::operator<<(const Sentence & theSentence)
  {
	itsPimple->itsData.push_back(theSentence);
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a paragraph to the end of a paragraph
   *
   * \param theParagraph The paragraph to be added
   * \result The paragraph added to
   */
  // ----------------------------------------------------------------------

  Paragraph & Paragraph::operator<<(const Paragraph & theParagraph)
  {
	if(this != &theParagraph)
	  {
		copy(theParagraph.itsPimple->itsData.begin(),
			 theParagraph.itsPimple->itsData.end(),
			 back_inserter(itsPimple->itsData));
	  }
	else
	  {
		// safety against x << x;
		Paragraph tmp(theParagraph);
		*this << tmp;
	  }
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the paragraph
   *
   * The global singleton dictionary is used for the realization
   *
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Paragraph::realize() const
  {
	return realize(TheDictionary::instance());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the paragraph
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Paragraph::realize(const Dictionary & theDictionary) const
  {
	string ret;

	typedef ParagraphPimple::storage_type::const_iterator Iterator;
	for(Iterator it=itsPimple->itsData.begin();
		it!=itsPimple->itsData.end();
		++it)
	  {
		const string words = it->realize(theDictionary);
		if(words.empty())
		  continue;
		if(!ret.empty())
		  ret += ' ';
		ret += words;
	  }

	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the paragraph is empty
   *
   * \return True if the paragraph is empty
   */
  // ----------------------------------------------------------------------

  bool Paragraph::empty() const
  {
	return itsPimple->itsData.empty();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the size of the paragraph (sentence count)
   *
   * \return The size
   */
  // ----------------------------------------------------------------------

  size_t Paragraph::size() const
  {
	return itsPimple->itsData.size();
  }

  // ======================================================================
  //			FREE OPERATORS
  // ======================================================================


  // ----------------------------------------------------------------------
  /*!
   * \brief Swapping two paragraphs
   *
   * \param theLhs The first paragraph
   * \param theRhs The second paragraph
   */
  // ----------------------------------------------------------------------

  void swap(Paragraph & theLhs, Paragraph & theRhs)
  {
	theLhs.swap(theRhs);
  }

} // namespace TextGen

// ======================================================================
