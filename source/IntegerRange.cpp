// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::IntegerRange
 */
// ======================================================================
/*!
 * \class TextGen::IntegerRange
 * \brief Representation of a generic IntegerRange
 */
// ======================================================================

#include "IntegerRange.h"
#include "Dictionary.h"
#include "TextGenError.h"

#include "boost/shared_ptr.hpp"
#include <sstream>

using namespace boost;
using namespace std;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  IntegerRange::~IntegerRange()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  IntegerRange::IntegerRange(int theStartValue, int theEndValue)
	: itsRangeSeparator("...")
	, itsStartValue(theStartValue)
	, itsEndValue(theEndValue)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   */
  // ----------------------------------------------------------------------

  shared_ptr<Glyph> IntegerRange::clone() const
  {
	shared_ptr<Glyph> ret(new IntegerRange(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the range
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string IntegerRange::realize(const Dictionary & theDictionary) const
  {
	std::ostringstream os;
	os << itsStartValue;
	if(itsStartValue != itsEndValue)
	  os << itsRangeSeparator << itsEndValue;
	return os.str();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the number
   *
   * \param theFormatter The formatter
   * \return The text
   */
  // ----------------------------------------------------------------------

  std::string IntegerRange::realize(const TextFormatter & theFormatter) const
  {
	return theFormatter.visit(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Returns false since IntegerRange is not a separator
   */
  // ----------------------------------------------------------------------

  bool IntegerRange::isDelimiter() const
  {
	return false;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the range separator string
   *
   * The default value set by the constructor is "...", but for example
   * for wind speeds it is customary to use "-".
   *
   * \param theSeparator The separator string
   */
  // ----------------------------------------------------------------------

  void IntegerRange::rangeSeparator(const std::string & theSeparator)
  {
	itsRangeSeparator = theSeparator;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the range separator string
   *
   * \return The range separator string
   */
  // ----------------------------------------------------------------------

  const std::string & IntegerRange::rangeSeparator() const
  {
	return itsRangeSeparator;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the start value of the range
   *
   * \return The start value
   */
  // ----------------------------------------------------------------------

  int IntegerRange::startValue() const
  {
	return itsStartValue;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the end value of the range
   *
   * \return The end value
   */
  // ----------------------------------------------------------------------

  int IntegerRange::endValue() const
  {
	return itsEndValue;
  }

} // namespace TextGen

// ======================================================================
