// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Float
 */
// ======================================================================
/*!
 * \class TextGen::Float
 * \brief Representation of a generic floating point number
 */
// ======================================================================

#include "Float.h"
#include "Dictionary.h"
#include "TextFormatter.h"
#include "TextGenError.h"

#include "NFmiStringTools.h"

#include "boost/shared_ptr.hpp"

#include <sstream>
#include <iomanip>

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  Float::~Float()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  Float::Float(float theFloat,
			   int thePrecision,
			   bool theComma)
	: itsFloat(theFloat)
	, itsPrecision(thePrecision)
	, itsComma(theComma)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   */
  // ----------------------------------------------------------------------

  shared_ptr<Glyph> Float::clone() const
  {
	shared_ptr<Glyph> ret(new Float(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the Float
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  const std::string Float::realize(const Dictionary & theDictionary) const
  {
	ostringstream os;
	os << fixed
	   << setprecision(itsPrecision)
	   << itsFloat;
	string result = os.str();
	if(!itsComma)
	  NFmiStringTools::ReplaceChars(result,'.',',');
	return result;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the number
   *
   * \param theFormatter The formatter
   * \return The text
   */
  // ----------------------------------------------------------------------

  const std::string Float::realize(const TextFormatter & theFormatter) const
  {
	return theFormatter.visit(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Returns false since Float is not a separator
   */
  // ----------------------------------------------------------------------

  bool Float::isDelimiter() const
  {
	return false;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the value of the integer
   *
   * \return The integer
   */
  // ----------------------------------------------------------------------

  float Float::value() const
  {
	return itsFloat;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the precision of the float
   *
   * \return The precision
   */
  // ----------------------------------------------------------------------

  int Float::precision() const
  {
	return itsPrecision;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return true if comma is in use
   *
   * \return True, if comma is in use
   */
  // ----------------------------------------------------------------------

  bool Float::comma() const
  {
	return itsComma;
  }

} // namespace TextGen

// ======================================================================
