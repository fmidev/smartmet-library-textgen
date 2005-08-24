// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::ParameterAnalyzerFactory
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::ParameterAnalyzerFactory
 *
 * \brief ParameterAnalyzer creation services
 *
 * The responsibility of the ParameterAnalyzerFactory namespace is to provide
 * ParameterAnalyzer creation services.
 *
 * For example,
 * \code
 * boost::shared_ptr<ParameterAnalyzer> analyzer = ParameterAnalyzerFactory::create(Temperature);
 * \endcode
 *
 */
// ======================================================================

#include "ParameterAnalyzerFactory.h"
#include "ModuloParameterAnalyzer.h"
#include "RegularParameterAnalyzer.h"
#include "WeatherAnalysisError.h"

using namespace boost;

namespace WeatherAnalysis
{
  namespace ParameterAnalyzerFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create a parameter analyzer for given parameter type
	 *
	 * \param theParameter The parameter to be analyzed
	 * \return An uninitialized text formatter
	 */
	// ----------------------------------------------------------------------
	
	shared_ptr<ParameterAnalyzer> create(WeatherParameter theParameter)
	{
	  typedef shared_ptr<ParameterAnalyzer> ReturnType;

	  switch(theParameter)
		{
		case WindDirection:
		  return ReturnType(new ModuloParameterAnalyzer("textgen::winddirection",
														"WindDirection",
														360));
		case WaveDirection:
		  return ReturnType(new ModuloParameterAnalyzer("textgen::wavedirection",
														"WaveDirection",
														360));
		case Temperature:
		  return ReturnType(new RegularParameterAnalyzer("textgen::temperature",
														 "Temperature"));
		case Precipitation:
		  return ReturnType(new RegularParameterAnalyzer("textgen::precipitation",
														 "Precipitation1h"));
		case Cloudiness:
		  return ReturnType(new RegularParameterAnalyzer("textgen::cloudiness",
														 "TotalCloudCover"));
		case Frost:
		  return ReturnType(new RegularParameterAnalyzer("textgen::frost",
														 "FrostProbability"));
		case SevereFrost:
		  return ReturnType(new RegularParameterAnalyzer("textgen::severefrost",
														 "SevereFrostProbability"));
		case RelativeHumidity:
		  return ReturnType(new RegularParameterAnalyzer("textgen::relativehumidity",
														 "Humidity"));
		case WindSpeed:
		  return ReturnType(new RegularParameterAnalyzer("textgen::windspeed",
														 "WindSpeedMS"));
		case Thunder:
		  return ReturnType(new RegularParameterAnalyzer("textgen::thunder",
														 "ProbabilityThunderstorm"));
		case PrecipitationType:
		  return ReturnType(new RegularParameterAnalyzer("textgen::precipitationtype",
														 "PrecipitationType"));
		case PrecipitationForm:
		  return ReturnType(new RegularParameterAnalyzer("textgen::precipitationform",
														 "PrecipitationForm"));
		case PrecipitationProbability:
		  return ReturnType(new RegularParameterAnalyzer("textgen::precipitationprobability",
														 "PoP"));
		case RoadTemperature:
		  return ReturnType(new RegularParameterAnalyzer("textgen::roadtemperature",
														 "RoadTemperature"));
		case RoadCondition:
		  return ReturnType(new RegularParameterAnalyzer("textgen::roadcondition",
														 "RoadCondition"));
		case WaveHeight:
		  return ReturnType(new RegularParameterAnalyzer("textgen::waveheight",
														 "SigWaveHeight"));

		case RoadWarning:
		  return ReturnType(new RegularParameterAnalyzer("textgen::roadwarning",
														 "RoadWarning"));
		  
		case ForestFireIndex:
		  return ReturnType(new RegularParameterAnalyzer("textgen::forestfireindex",
														 "ForestFireWarning"));
		case Evaporation:
		  return ReturnType(new RegularParameterAnalyzer("textgen::evaporation",
														 "Evaporation"));

		case DewPoint:
		  return ReturnType(new RegularParameterAnalyzer("textgen::dewpoint",
														 "DewPoint"));
		case GustSpeed:
		  return ReturnType(new RegularParameterAnalyzer("textgen::gustspeed",
														 "HourlyMaximumGust"));

		case Pressure:
		  return ReturnType(new RegularParameterAnalyzer("textgen::pressure",
														 "Pressure"));

	  }

	  // Should never reach this place

	  throw WeatherAnalysisError("Error: ParameterAnalyzerFactory does not recognize given parameter enumeration");
	}

  } // namespace ParameterAnalyzerFactory
} // namespace WeatherAnalysis

// ======================================================================
