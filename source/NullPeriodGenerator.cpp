// ======================================================================
/*!
 * \file
 * \brief Implementation of class NullPeriodGenerator
 */
// ======================================================================

#include "NullPeriodGenerator.h"
#include "WeatherAnalysisError.h"

#include "boost/lexical_cast.hpp"

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theMainPeriod The period to iterate
   */
  // ----------------------------------------------------------------------

  NullPeriodGenerator::NullPeriodGenerator(const WeatherPeriod & theMainPeriod)
	: itsMainPeriod(theMainPeriod)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test if the period is undivided
   *
   * \return Always true, null period is the original one
   */
  // ----------------------------------------------------------------------

  bool NullPeriodGenerator::undivided() const
  {
	return true;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the number of subperiods
   *
   * \return The number of subperiods (always 1)
   */
  // ----------------------------------------------------------------------

  NullPeriodGenerator::size_type
  NullPeriodGenerator::size() const
  {
	return 1;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the desired subperiod
   *
   * Throws if anything but the first period is requested, since by
   * definition there is always exactly 1 subperiod.
   *
   * \param thePeriod The index of the subperiod
   * \return The subperiod
   */
  // ----------------------------------------------------------------------

  WeatherPeriod NullPeriodGenerator::period(size_type thePeriod) const
  {
	if(thePeriod == 1)
	  return itsMainPeriod;

	const string msg = ("NullPeriodGenerator cannot return period "
						+lexical_cast<string>(thePeriod));
	throw WeatherAnalysisError(msg);
  }

} // namespace WeatherAnalysis

// ======================================================================
