// ======================================================================
/*!
 * \file
 * \brief Interface of class IdGenerator
 */
// ======================================================================

#include "IdGenerator.h"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a new ID
   *
   * \return The generated ID
   */
  // ----------------------------------------------------------------------

  long IdGenerator::generate()
  {
	static long id;
	return ++id;
  }

} // namespace namespace WeatherAnalysis

// ======================================================================

