// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Float
 */
// ======================================================================

#ifndef TEXTGEN_FLOAT_H
#define TEXTGEN_FLOAT_H

#include "Glyph.h"
#include <string>

namespace TextGen
{
  class Dictionary;
  class TextFormatter;

  class Float : public Glyph
  {
  public:

	virtual ~Float();
	explicit Float(float theFloat,
				   int thePrecision = 1,
				   bool theComma = true);

#ifdef NO_COMPILER_GENERATED
	Float(const Float & theFloat);
	Float & operator=(const Float & theFloat);
#endif

	virtual boost::shared_ptr<Glyph> clone() const;
	virtual const std::string realize(const Dictionary & theDictionary) const;
	virtual const std::string realize(const TextFormatter & theFormatter) const;
	virtual bool isDelimiter() const;
	float value() const;
	int precision() const;
	bool comma() const;

  private:

	Float();
	float itsFloat;
	int itsPrecision;
	bool itsComma;

  }; // class Float
} // namespace Float

#endif // TEXTGEN_FLOAT_H

// ======================================================================
