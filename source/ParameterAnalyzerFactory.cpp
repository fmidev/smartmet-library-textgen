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
#include "WeatherAnalysisError.h"

#include "CloudinessAnalyzer.h"
#include "FrostAnalyzer.h"
#include "PrecipitationAnalyzer.h"
#include "PrecipitationFormAnalyzer.h"
#include "PrecipitationProbabilityAnalyzer.h"
#include "PrecipitationTypeAnalyzer.h"
#include "RelativeHumidityAnalyzer.h"
#include "RoadTemperatureAnalyzer.h"
#include "SevereFrostAnalyzer.h"
#include "TemperatureAnalyzer.h"
#include "ThunderAnalyzer.h"
#include "WindDirectionAnalyzer.h"
#include "WindSpeedAnalyzer.h"

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
		case Temperature:
		  return ReturnType(new TemperatureAnalyzer);
		case Precipitation:
		  return ReturnType(new PrecipitationAnalyzer);
		case Cloudiness:
		  return ReturnType(new CloudinessAnalyzer);
		case Frost:
		  return ReturnType(new FrostAnalyzer);
		case SevereFrost:
		  return ReturnType(new SevereFrostAnalyzer);
		case RelativeHumidity:
		  return ReturnType(new RelativeHumidityAnalyzer);
		case WindSpeed:
		  return ReturnType(new WindSpeedAnalyzer);
		case WindDirection:
		  return ReturnType(new WindDirectionAnalyzer);
		case Thunder:
		  return ReturnType(new ThunderAnalyzer);
		case PrecipitationType:
		  return ReturnType(new PrecipitationTypeAnalyzer);
		case PrecipitationForm:
		  return ReturnType(new PrecipitationFormAnalyzer);
		case PrecipitationProbability:
		  return ReturnType(new PrecipitationProbabilityAnalyzer);
		case RoadTemperature:
		  return ReturnType(new RoadTemperatureAnalyzer);
	  }

	  // Should never reach this place

	  throw WeatherAnalysisError("Error: ParameterAnalyzerFactory does not recognize given parameter enumeration");
	}

  } // namespace ParameterAnalyzerFactory
} // namespace WeatherAnalysis

// ======================================================================
