// ======================================================================
/*!
 *�\file
 * \brief Implementation of namespace MathTools
 */
// ======================================================================

#include "MathTools.h"

#include "NFmiGlobals.h"

namespace MathTools
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Round the input value to the given integer precision
   *
   * \param theValue The floating value
   * \param thePrecision The precision >= 0
   * \return The rounded value
   */
  // ----------------------------------------------------------------------

  int to_precision(float theValue, int thePrecision)
  {
	if(thePrecision <=0)
	  return FmiRound(theValue);
	const int value = FmiRound(theValue/thePrecision)*thePrecision;
	return value;
  }

} // namespace MathTools

// ======================================================================