// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Sentence
 */
// ======================================================================

#include "Sentence.h"
#include "Dictionary.h"
#include "PhraseWord.h"
#include "PhraseNumber.h"
#include "TheDictionary.h"
#include <cctype>
#include <clocale>
#include <list>

using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \class SentencePimple
   * \brief Implementation hiding pimple for class Sentence
   */
  // ----------------------------------------------------------------------

  class SentencePimple
  {
  public:
	typedef list<Phrase *> storage_type;
	storage_type itsData;

	~SentencePimple()
	{
	  for(storage_type::iterator it=itsData.begin(); it!=itsData.end(); ++it)
		delete *it;
	}

	SentencePimple() : itsData() { }

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
   * \todo What about a(a) ?
   */
  // ----------------------------------------------------------------------

  Sentence::Sentence(const Sentence & theSentence)
  {
	itsPimple.reset(new SentencePimple());
	*this << theSentence;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param thePhrase The initial phrase
   */
  // ----------------------------------------------------------------------

  Sentence::Sentence(const Phrase & thePhrase)
  {
	itsPimple.reset(new SentencePimple());
	*this << thePhrase;
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
	*this << PhraseWord(thePhrase);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theValue The value starting the phrase
   */
  // ----------------------------------------------------------------------

  Sentence::Sentence(int theValue)
  {
	itsPimple.reset(new SentencePimple());
	*this << PhraseNumber<int>(theValue);
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
	  {
		itsPimple.reset(new SentencePimple);
		*this << theSentence;
	  }
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
   * \brief Adding a sentence to a sentence
   *
   * \param theSentence to be added
   * \result The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator<<(const Sentence & theSentence)
  {
	if(this != &theSentence)
	  {
		for(SentencePimple::storage_type::const_iterator it=theSentence.itsPimple->itsData.begin();
			it!=theSentence.itsPimple->itsData.end();
			++it)
		  {
			Phrase * tmp = *it;
			*this << *(tmp->clone().release());
		  }
	  }
	else
	  {
		// safety against x << x;
		Sentence tmp(theSentence);
		*this << tmp;
	  }
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a phrase to a sentence
   *
   * \param thePhrase to be added
   * \result The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator<<(const Phrase & thePhrase)
  {
	Phrase * tmp = thePhrase.clone().release();
	itsPimple->itsData.push_back(tmp);
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a phrase to a sentence
   *
   * \param thePhrase to be added
   * \result The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator<<(const std::string & thePhrase)
  {
	Phrase * tmp = new PhraseWord(thePhrase);
	itsPimple->itsData.push_back(tmp);
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Adding a value to a sentence
   *
   * \param theValue The value to be added
   * \result The sentence added to
   */
  // ----------------------------------------------------------------------

  Sentence & Sentence::operator<<(int theValue)
  {
	Phrase * tmp = new PhraseNumber<int>(theValue);
	itsPimple->itsData.push_back(tmp);
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
