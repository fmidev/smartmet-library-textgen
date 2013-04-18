// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::Time
 */
// ======================================================================
/*!
 * \class TextGen::Time
 * \brief Representation of timestamp
 */
// ======================================================================

#include "Time.h"
#include "Dictionary.h"
#include "WeatherPeriod.h"
#include "TextFormatter.h"

using namespace std;
using namespace boost;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  Time::~Time()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param thePeriod The period
   */
  // ----------------------------------------------------------------------

  Time::Time(const TextGenTime& theTime)
	: itsTime(theTime)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   */
  // ----------------------------------------------------------------------

  shared_ptr<Glyph> Time::clone() const
  {
	shared_ptr<Glyph> ret(new Time(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using the given dictionary
   *
   * \param theDictionary The dictionary to realize with
   * \return The realized string
   */
  // ----------------------------------------------------------------------

  std::string Time::realize(const Dictionary & theDictionary) const
  {
	return "";
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using the given text formatter
   *
   * \param theFormatter The formatter
   * \return The formatter string
   */
  // ----------------------------------------------------------------------

  std::string Time::realize(const TextFormatter & theFormatter) const
  {
	return theFormatter.visit(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Returns false since phrase is not a separator
   */
  // ----------------------------------------------------------------------

  bool Time::isDelimiter() const
  {
	return false;
  }

} // namespace TextGen

// ======================================================================
