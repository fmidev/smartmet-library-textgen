// ======================================================================
/*!
 * \file
 * \brief Enumeration of WeatherFunction
 */
// ======================================================================
/*!
 * \enum WeatherAnalysis::WeatherFunction
 *
 * \brief Enumeration of functions on parameters that can be analyzed
 *
 * This enumeration lists all the functions that can be applied
 * when analyzing facts from gridded data. For example, one can
 * calculate the Maximum, Minimum or Mean of the data over space
 * and time.
 *
 * This enumeration is used both as input to the analysis functions
 * and is stored in WeatherFact along with the results and other
 * parameters related to a single fact extracted in analysis.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERFUNCTION_H
#define WEATHERANALYSIS_WEATHERFUNCTION_H

namespace WeatherAnalysis
{

  enum WeatherFunction
	{
	  Maximum,
	  Minimum,
	  Mean

	}; // enum Weatherfunction

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERFUNCTION_H

// ======================================================================

