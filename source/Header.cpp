// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Header
 */
// ======================================================================

#include "Header.h"
#include "Dictionary.h"
#include "PhraseWord.h"
#include "PhraseNumber.h"
#include "PhraseSeparator.h"
#include "TheDictionary.h"
#include <cctype>
#include <clocale>
#include <list>

using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding pimple for class Header
   */
  // ----------------------------------------------------------------------

  class Header::Pimple
  {
  public:
	typedef list<Phrase *> storage_type;
	storage_type itsData;

	~Pimple()
	{
	  for(storage_type::iterator it=itsData.begin(); it!=itsData.end(); ++it)
		delete *it;
	}

	Pimple() : itsData() { }

  }; // class Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  Header::~Header()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  Header::Header()
  {
	itsPimple.reset(new Pimple());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Copy constructor
   *
   * \param theHeader The header to be copied
   */
  // ----------------------------------------------------------------------

  Header::Header(const Header & theHeader)
  {
	itsPimple.reset(new Pimple());
	*this << theHeader;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param thePhrase The initial phrase
   */
  // ----------------------------------------------------------------------

  Header::Header(const Phrase & thePhrase)
  {
	itsPimple.reset(new Pimple());
	*this << thePhrase;
  }


  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param thePhrase The initial phrase
   */
  // ----------------------------------------------------------------------

  Header::Header(const std::string & thePhrase)
  {
	itsPimple.reset(new Pimple());
	*this << PhraseWord(thePhrase);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theValue The value starting the phrase
   */
  // ----------------------------------------------------------------------

  Header::Header(int theValue)
  {
	itsPimple.reset(new Pimple());
	*this << PhraseNumber<int>(theValue);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Assignment operator
   *
   * \param theHeader The header to be copied
   * \return The header assigned to
   */
  // ----------------------------------------------------------------------

  Header & Header::operator=(const Header & theHeader)
  {
	if(this != &theHeader)
	  {
		itsPimple.reset(new Pimple);
		*this << theHeader;
	  }
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief swap method
   *
   * \param theHeader The header to swap contents with
   */
  // ----------------------------------------------------------------------

  void Header::swap(Header & theHeader)
  {
	Pimple * a = itsPimple.release();
	Pimple * b = theHeader.itsPimple.release();
	itsPimple.reset(b);
	theHeader.itsPimple.reset(a);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a header to a header
   *
   * \param theHeader to be added
   * \result The header added to
   */
  // ----------------------------------------------------------------------

  Header & Header::operator<<(const Header & theHeader)
  {
	if(this != &theHeader)
	  {
		for(Pimple::storage_type::const_iterator it=theHeader.itsPimple->itsData.begin();
			it!=theHeader.itsPimple->itsData.end();
			++it)
		  {
			Phrase * tmp = *it;
			*this << *(tmp->clone().release());
		  }
	  }
	else
	  {
		// safety against x << x;
		Header tmp(theHeader);
		*this << tmp;
	  }
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a phrase to a header
   *
   * \param thePhrase to be added
   * \result The header added to
   */
  // ----------------------------------------------------------------------

  Header & Header::operator<<(const Phrase & thePhrase)
  {
	Phrase * tmp = thePhrase.clone().release();
	itsPimple->itsData.push_back(tmp);
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a phrase to a header
   *
   * The string is assumed to be the constructor argument for a PhraseWord,
   * unless the string is recognized to be a word separator.
   *
   * The only currently recognized separator is ','
   * 
   * \param thePhrase to be added
   * \result The header added to
   */
  // ----------------------------------------------------------------------

  Header & Header::operator<<(const std::string & thePhrase)
  {
	Phrase * tmp;

	if(thePhrase == ",")
	  tmp = new PhraseSeparator(thePhrase);
	else
	  tmp = new PhraseWord(thePhrase);

	itsPimple->itsData.push_back(tmp);
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a value to a header
   *
   * \param theValue The value to be added
   * \result The header added to
   */
  // ----------------------------------------------------------------------

  Header & Header::operator<<(int theValue)
  {
	Phrase * tmp = new PhraseNumber<int>(theValue);
	itsPimple->itsData.push_back(tmp);
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the header
   *
   * The global singleton dictionary is used for the realization
   *
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Header::realize() const
  {
	return realize(TheDictionary::instance());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the header
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Header::realize(const Dictionary & theDictionary) const
  {
	string ret;

	typedef Pimple::storage_type::const_iterator Iterator;
	for(Iterator it=itsPimple->itsData.begin();
		it!=itsPimple->itsData.end();
		++it)
	  {
		const string & word = (*it)->realize(theDictionary);
		if(word.empty())
		  continue;
		if(!ret.empty())
		  ret += (*it)->wordseparator();
		ret += word;
	  }

	if(ret.empty())
	  return ret;

	// capitalize

	setlocale(LC_ALL, "Finnish");
	ret[0] = ::toupper(ret[0]);
	setlocale(LC_ALL, "C");

	// Punctuate

	ret += ":\n";
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the header is empty
   *
   * \return True if the header is empty
   */
  // ----------------------------------------------------------------------

  bool Header::empty() const
  {
	return itsPimple->itsData.empty();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the size of the header (phrase count)
   *
   * \return The size
   */
  // ----------------------------------------------------------------------

  size_t Header::size() const
  {
	return itsPimple->itsData.size();
  }

  // ======================================================================
  //			FREE OPERATORS
  // ======================================================================


  // ----------------------------------------------------------------------
  /*!
   * \brief Swapping two headers
   *
   * \param theLhs The first header
   * \param theRhs The second header
   */
  // ----------------------------------------------------------------------

  void swap(Header & theLhs, Header & theRhs)
  {
	theLhs.swap(theRhs);
  }

} // namespace TextGen

// ======================================================================
