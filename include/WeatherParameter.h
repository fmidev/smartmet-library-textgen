// ======================================================================
/*!
 * \file
 * \brief Enumeration of WeatherAnalysis::WeatherParameter
 */
// ======================================================================
/*!
 * \enum WeatherAnalysis::WeatherParameter
 *
 * \brief Enumeration of parameters that can be analyzed
 *
 * This enumeration is separate from that in newbase library because
 * we may enumerate phenomena which are not represented separately
 * in newbase. For example, we list Snow and Sleet as separate parameters
 * for which one can calculate a Probability.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERPARAMETER_H
#define WEATHERANALYSIS_WEATHERPARAMETER_H

namespace WeatherAnalysis
{

  enum WeatherParameter
	{
	  Temperature,
	  Precipitation,
	  Cloudiness,
	  Frost,
	  SevereFrost,
	  RelativeHumidity,
	  WindSpeed,
	  WindDirection,
	  Thunder

	}; // enum WeatherParameter

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERPARAMETER_H

// ======================================================================

