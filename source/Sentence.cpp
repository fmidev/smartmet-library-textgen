// ======================================================================
/*!
 * \file
 * \brief Implementation of class Sentence
 */
// ======================================================================

#include "Sentence.h"
#include "Dictionary.h"
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
   * \class TextGen::SentencePimple
   * \brief Implementation hiding pimple for class Sentence
   */
  // ----------------------------------------------------------------------

  class SentencePimple
  {
  public:
	~SentencePimple() { }
	SentencePimple() : itsData() { }

	typedef list<string> storage_type;
	storage_type itsData;

  }; // class SentencePimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  Sentence::~Sentence()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  Sentence::Sentence()
  {
	itsPimple.reset(new SentencePimple());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Copy constructor
   *
   * \param theSentence The sentence to be copied
   */
  // ----------------------------------------------------------------------

  Sentence::Sentence(const Sentence & theSentence)
  {
	itsPimple.reset(new SentencePimple(*theSentence.itsPimple));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param thePhrase The initial phrase
   */
  // ----------------------------------------------------------------------

  Sentence::Sentence(const std::string & thePhrase)
  {
	itsPimple.reset(new SentencePimple());
	*this += thePhrase;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Assignment operator
   *
   * \param theSentence The sentence to be copied
   * \return The sentence assigned to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator=(const Sentence & theSentence)
  {
	if(this != &theSentence)
	  itsPimple.reset(new SentencePimple(*theSentence.itsPimple));
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief swap method
   *
   * \param theSentence The sentence to swap contents with
   */
  // ----------------------------------------------------------------------

  void Sentence::swap(Sentence & theSentence)
  {
	SentencePimple * a = itsPimple.release();
	SentencePimple * b = theSentence.itsPimple.release();
	itsPimple.reset(b);
	theSentence.itsPimple.reset(a);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a phrase to a sentence
   *
   * \param thePhrase The phrase to be added
   * \result The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator+=(const std::string & thePhrase)
  {
	itsPimple->itsData.push_back(thePhrase);
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a sentence to the end of a sentence
   *
   * \param theSentence The sentence to be added
   * \result The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator+=(const Sentence & theSentence)
  {
	if(this != &theSentence)
	  {
		copy(theSentence.itsPimple->itsData.begin(),
			 theSentence.itsPimple->itsData.end(),
			 back_inserter(itsPimple->itsData));
	  }
	else
	  {
		// safety against x += x;
		Sentence tmp(theSentence);
		*this += tmp;
	  }
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the sentence
   *
   * The global singleton dictionary is used for the realization
   *
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Sentence::realize() const
  {
	return realize(TheDictionary::instance());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the sentence
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string Sentence::realize(const Dictionary & theDictionary) const
  {
	string ret;

	typedef SentencePimple::storage_type::const_iterator Iterator;
	for(Iterator it=itsPimple->itsData.begin();
		it!=itsPimple->itsData.end();
		++it)
	  {
		const string & word = theDictionary.find(*it);
		if(word.empty())
		  continue;
		if(!ret.empty())
		  ret += ' ';
		ret += word;
	  }

	if(ret.empty())
	  return ret;

	// capitalize

	setlocale(LC_ALL, "Finnish");
	ret[0] = ::toupper(ret[0]);
	setlocale(LC_ALL, "C");

	// Punctuate

	ret += '.';
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the sentence is empty
   *
   * \return True if the sentence is empty
   */
  // ----------------------------------------------------------------------

  bool Sentence::empty() const
  {
	return itsPimple->itsData.empty();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the size of the sentence (phrase count)
   *
   * \return The size
   */
  // ----------------------------------------------------------------------

  size_t Sentence::size() const
  {
	return itsPimple->itsData.size();
  }

  // ======================================================================
  //			FREE OPERATORS
  // ======================================================================


  // ----------------------------------------------------------------------
  /*!
   * \brief Addition of two sentences
   *
   * \param theLhs The start of the sentence
   * \param theRhs The end of the sentence
   * \return The complete sentence
   */
  // ----------------------------------------------------------------------

  Sentence operator+(const Sentence & theLhs, const Sentence & theRhs)
  {
	Sentence ret(theLhs);
	ret += theRhs;
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Addition of a sentence and a phrase
   *
   * \param theLhs The sentence
   * \param theRhs The phrase
   */
  // ----------------------------------------------------------------------

  Sentence operator+(const Sentence & theLhs, const std::string & theRhs)
  {
	Sentence ret(theLhs);
	ret += theRhs;
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Addition of a sentence and a phrase
   *
   * \param theLhs The phrase
   * \param theRhs The sentence
   */
  // ----------------------------------------------------------------------

  Sentence operator+(const std::string & theLhs, const Sentence & theRhs)
  {
	Sentence ret(theLhs);
	ret += theRhs;
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Swapping two sentences
   *
   * \param theLhs The first sentence
   * \param theRhs The second sentence
   */
  // ----------------------------------------------------------------------

  void swap(Sentence & theLhs, Sentence & theRhs)
  {
	theLhs.swap(theRhs);
  }

} // namespace TextGen

// ======================================================================
