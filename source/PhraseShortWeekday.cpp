// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PhraseShortWeekday
 */
// ======================================================================

#include "PhraseShortWeekday.h"
#include "Dictionary.h"
#include "TheDictionary.h"

using namespace std;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  PhraseShortWeekday::~PhraseShortWeekday()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * The days are numbered as follows:
   *  - 0 = Sunday
   *  - 1 = Monday
   *  - 2 = Tuesday
   *  - 3 = Wednesday
   *  - 4 = Thursday
   *  - 5 = Friday
   *  - 6 = Saturday
   *
   * Input is taken modulo 7, hence for example 7 is also a Sunday.
   *
   * \param theDay The day number
   */
  // ----------------------------------------------------------------------

  PhraseShortWeekday::PhraseShortWeekday(int theDay)
	: itsDay(theDay % 7)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   *
   * \return The clone
   */
  // ----------------------------------------------------------------------

  std::auto_ptr<Phrase> PhraseShortWeekday::clone() const
  {
	return auto_ptr<Phrase>(new PhraseShortWeekday(*this));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using global dictionary
   *
   * \return The realized string
   */
  // ----------------------------------------------------------------------

  std::string PhraseShortWeekday::realize() const
  {
	return realize(TheDictionary::instance());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using the given dictionary
   *
   * \param theDictionary The dictionary to realize with
   * \return The realized string
   */
  // ----------------------------------------------------------------------

  std::string PhraseShortWeekday::realize(const Dictionary & theDictionary) const
  {
	static const char * weekdays[] = { "su", "ma", "ti", "ke", "to", "pe", "la" };
	return theDictionary.find(weekdays[itsDay]);
  }

} // namespace TextGen

// ======================================================================
