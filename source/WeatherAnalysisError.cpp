// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::WeatherAnalysisError
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherAnalysisError
 *
 * \brief The generic exception thrown by WeatherAnalysis namespace
 *
 */
// ======================================================================

#include "WeatherAnalysisError.h"

namespace WeatherAnalysis
{
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theError The error message
   */
  // ----------------------------------------------------------------------

  WeatherAnalysisError::WeatherAnalysisError(const std::string & theError)
	: itsError(theError)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Returns the error message
   *
   * \return The error message
   */
  // ----------------------------------------------------------------------

  const char * WeatherAnalysisError::what(void) const throw()
  {
	return itsError.c_str();
  }

} // namespace WeatherAnalysis

// ======================================================================
