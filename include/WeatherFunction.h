// ======================================================================
/*!
 * \file
 * \brief Enumeration of WeatherAnalysis::WeatherFunction
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERFUNCTION_H
#define WEATHERANALYSIS_WEATHERFUNCTION_H

namespace WeatherAnalysis
{

  enum WeatherFunction
	{
	  NullFunction,
	  Mean,
	  Maximum,
	  Minimum,
	  Sum,
	  StandardDeviation,
	  Percentage,
	  Count,
	  Change,
	  Trend

	}; // enum Weatherfunction

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERFUNCTION_H

// ======================================================================

