// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::NumberRange
 */
// ======================================================================
/*!
 * \class TextGen::NumberRange
 * \brief Representation of a number range
 */
// ======================================================================

#ifndef TEXTGEN_NUMBERRANGE_H
#define TEXTGEN_NUMBERRANGE_H

#include "Glyph.h"
#include "Number.h"
#include "TextFormatter.h"
#include <string>

namespace TextGen
{
  class Dictionary;

  template <typename T>
  class NumberRange : public Glyph
  {
  public:

	typedef T value_type;

	virtual ~NumberRange() { }

	NumberRange(value_type theStart, value_type theEnd)
	  : itsStart(theStart)
	  , itsEnd(theEnd)
	{ }

#ifdef NO_COMPILER_GENERATED
	NumberRange(const NumberRange & theNumberRange);
	NumberRange & operator=(const NumberRange & theNumberRange);
#endif

	virtual boost::shared_ptr<Glyph> clone() const
	{
	  boost::shared_ptr<Glyph> ret(new NumberRange(*this));
	  return ret;
	}

	virtual std::string realize(const Dictionary & theDictionary) const
	{
	  const std::string part1 = itsStart.realize(theDictionary);
	  const std::string part2 = itsEnd.realize(theDictionary);

	  if(part1==part2)
		return part1;
	  else
		return part1+"..."+part2;
	}

	virtual std::string realize(const TextFormatter & theFormatter) const
	{ return theFormatter.visit(*this); }

	virtual bool isDelimiter() const
	{ return false; }

  private:

	NumberRange();

	value_type itsStart;
	value_type itsEnd;

  }; // class Number

} // namespace TextGen

#endif // TEXTGEN_NUMBER_H

// ======================================================================
