// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::PhraseNumber
 */
// ======================================================================
/*!
 * \class TextGen::PhraseNumber
 * \brief Representation of a number
 */
// ======================================================================

#ifndef TEXTGEN_PHRASENUMBER_H
#define TEXTGEN_PHRASENUMBER_H

#include "Phrase.h"
#include "DefaultNumberTraits.h"
#include <string>

namespace TextGen
{

  class Dictionary;

  template <typename Value, typename Traits=DefaultNumberTraits<Value> >
  class PhraseNumber : public Phrase, public Traits
  {
  public:

	typedef Traits traits;
	typedef Value value_type;

	virtual ~PhraseNumber() { }
	PhraseNumber(value_type theNumber) : itsNumber(theNumber) { }
#ifdef NO_COMPILER_GENERATED
	PhraseNumber(const PhraseNumber & thePhrase);
	PhraseNumber & operator=(const PhraseNumber & thePhrase);
#endif
	virtual std::auto_ptr<Phrase> clone() const
	{ return std::auto_ptr<Phrase>(new PhraseNumber(*this)); }

	virtual std::string wordseparator() const
	{ return std::string(" "); }

	virtual std::string realize() const
	{ return this->tostring(itsNumber); }

	virtual std::string realize(const Dictionary & theDictionary) const
	{ return this->tostring(itsNumber); }

  private:

	PhraseNumber();

	value_type itsNumber;

  }; // class PhraseNumber

} // namespace TextGen

#endif // TEXTGEN_PHRASENUMBER_H

// ======================================================================
