// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Number
 */
// ======================================================================
/*!
 * \class TextGen::Number
 * \brief Representation of a number
 */
// ======================================================================

#ifndef TEXTGEN_NUMBER_H
#define TEXTGEN_NUMBER_H

#include "Glyph.h"
#include "DefaultNumberTraits.h"
#include "TextFormatter.h"
#include <string>

namespace TextGen
{
  class Dictionary;

  template <typename Value, typename Traits=DefaultNumberTraits<Value> >
  class Number : public Glyph, public Traits
  {
  public:

	typedef Traits traits;
	typedef Value value_type;

	virtual ~Number() { }
	Number(value_type theNumber) : itsNumber(theNumber) { }
#ifdef NO_COMPILER_GENERATED
	Number(const Number & theNumber);
	Number & operator=(const Number & theNumber);
#endif

	virtual boost::shared_ptr<Glyph> clone() const
	{
	  boost::shared_ptr<Glyph> ret(new Number(*this));
	  return ret;
	}

	virtual std::string realize(const Dictionary & theDictionary) const
	{ return this->tostring(itsNumber); }

	virtual std::string realize(const TextFormatter & theFormatter) const
	{ return theFormatter.visit(*this); }

	virtual bool isDelimiter() const
	{ return false; }

  private:

	Number();

	value_type itsNumber;

  }; // class Number

} // namespace TextGen

#endif // TEXTGEN_NUMBER_H

// ======================================================================
