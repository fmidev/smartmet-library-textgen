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
 * boost::shared_ptr<ParameterAnalyzer> analyzer(ParameterAnalyzerFactory::create(Temperature));
 * \endcode
 *
 */
// ======================================================================

#include "ParameterAnalyzerFactory.h"
#include "ModuloParameterAnalyzer.h"
#include "RegularParameterAnalyzer.h"
#include "WindChillParameterAnalyzer.h"
#include "WeatherAnalysisError.h"

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
	
	ParameterAnalyzer * create(WeatherParameter theParameter)
	{
	  switch(theParameter)
		{
		case WindDirection:
		  return new ModuloParameterAnalyzer("textgen::winddirection",
											 "WindDirection",
											 360);
		case WaveDirection:
		  return new ModuloParameterAnalyzer("textgen::wavedirection",
											 "WaveDirection",
											 360);
		case Temperature:
		  return new RegularParameterAnalyzer("textgen::temperature",
											  "Temperature");
		case Precipitation:
		  return new RegularParameterAnalyzer("textgen::precipitation",
											  "Precipitation1h");
		case Cloudiness:
		  return new RegularParameterAnalyzer("textgen::cloudiness",
											  "TotalCloudCover");
		case Frost:
		  return new RegularParameterAnalyzer("textgen::frost",
											  "FrostProbability");
		case SevereFrost:
		  return new RegularParameterAnalyzer("textgen::severefrost",
											  "SevereFrostProbability");
		case RelativeHumidity:
		  return new RegularParameterAnalyzer("textgen::relativehumidity",
											  "Humidity");
		case WindSpeed:
		  return new RegularParameterAnalyzer("textgen::windspeed",
											  "WindSpeedMS");
		case Thunder:
		  return new RegularParameterAnalyzer("textgen::thunder",
											  "ProbabilityThunderstorm");
		case PrecipitationType:
		  return new RegularParameterAnalyzer("textgen::precipitationtype",
											  "PrecipitationType");
		case PrecipitationForm:
		  return new RegularParameterAnalyzer("textgen::precipitationform",
											  "PrecipitationForm");
		case PrecipitationProbability:
		  return new RegularParameterAnalyzer("textgen::precipitationprobability",
											  "PoP");
		case RoadTemperature:
		  return new RegularParameterAnalyzer("textgen::roadtemperature",
											  "RoadTemperature");
		case RoadCondition:
		  return new RegularParameterAnalyzer("textgen::roadcondition",
											  "RoadCondition");
		case WaveHeight:
		  return new RegularParameterAnalyzer("textgen::waveheight",
											  "SigWaveHeight");

		case RoadWarning:
		  return new RegularParameterAnalyzer("textgen::roadwarning",
											  "RoadWarning");
		  
		case ForestFireIndex:
		  return new RegularParameterAnalyzer("textgen::forestfireindex",
											  "ForestFireWarning");
		case Evaporation:
		  return new RegularParameterAnalyzer("textgen::evaporation",
											  "Evaporation");

		case DewPoint:
		  return new RegularParameterAnalyzer("textgen::dewpoint",
											  "DewPoint");
		case GustSpeed:
		  return new RegularParameterAnalyzer("textgen::gustspeed",
											  "HourlyMaximumGust");
		case Pressure:
		  return new RegularParameterAnalyzer("textgen::pressure",
											  "Pressure");
		case Fog:
		  return new RegularParameterAnalyzer("textgen::fog",
											  "FogIntensity");
		case MaximumWind:
		  return new RegularParameterAnalyzer("textgen::maximumwind",
											  "HourlyMaximumWindSpeed");

		case EffectiveTemperatureSum:
		  return new RegularParameterAnalyzer("textgen::effectivetemperaturesum",
											  "EffectiveTemperatureSum");

		case PrecipitationRate:
		  return new RegularParameterAnalyzer("textgen::precipitationrate",
											  "PrecipitationRate");
		case WaterEquivalentOfSnow:
		  return new RegularParameterAnalyzer("textgen::waterequivalentofsnow",
											  "WaterEquivalentOfSnow");

		case NormalMaxTemperatureF02:
		  return new RegularParameterAnalyzer("textgen::fractiles",
											  "NormalMaxTemperatureF02");

		case NormalMaxTemperatureF12:
		  return new RegularParameterAnalyzer("textgen::fractiles",
											  "NormalMaxTemperatureF12");


		case NormalMaxTemperatureF37:
		  return new RegularParameterAnalyzer("textgen::fractiles",
											  "NormalMaxTemperatureF37");


		case NormalMaxTemperatureF50:
		  return new RegularParameterAnalyzer("textgen::fractiles",
											  "NormalMaxTemperatureF50");


		case NormalMaxTemperatureF63:
		  return new RegularParameterAnalyzer("textgen::fractiles",
											  "NormalMaxTemperatureF63");


		case NormalMaxTemperatureF88:
		  return new RegularParameterAnalyzer("textgen::fractiles",
											  "NormalMaxTemperatureF88");


		case NormalMaxTemperatureF98:
		  return new RegularParameterAnalyzer("textgen::fractiles",
											  "NormalMaxTemperatureF98");

		case WindChill:
		  return new WindChillParameterAnalyzer("textgen::windchill",
												"WindChill");

	  }

	  // Should never reach this place

	  throw WeatherAnalysisError("Error: ParameterAnalyzerFactory does not recognize given parameter enumeration");
	}

  } // namespace ParameterAnalyzerFactory
} // namespace WeatherAnalysis

// ======================================================================
