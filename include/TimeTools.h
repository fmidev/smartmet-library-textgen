// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TimeTools
 */
// ======================================================================
/*!
 * \namespace TextGen::TimeTools
 *
 * \brief Various NFmiTime related utilities
 *
 * This namespace contains functions which depend on NFmiTime but
 * do not depend on any other textgen library classes or namespaces.
 *
 * That is, miscellaneous time utilities which could have been
 * in newbase too.
 *
 */
// ======================================================================

#ifndef TEXTGEN_TIMETOOLS_H
#define TEXTGEN_TIMETOOLS_H

class NFmiTime;

namespace TextGen
{
  namespace TimeTools
  {

	bool isSameDay(const NFmiTime & theDate1, const NFmiTime & theDate2);
	bool isNextDay(const NFmiTime & theDate1, const NFmiTime & theDate2);
	bool isSeveralDays(const NFmiTime & theDate1, const NFmiTime & theDate2);

  }
} // namespace TextGen

#endif // TEXTGEN_TIMETOOLS_H

// ======================================================================
