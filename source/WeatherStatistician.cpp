// ======================================================================
/*!
 * \file
 * \brief Implementation of abstract class WeatherAnalysis::WeatherStatistician
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherStatistician
 *
 * \brief Provides statistical analysis services to clients
 *
 * The WeatherStatistician class provides an uniform interface for
 * weather statistical analysis services. The only thing required by the
 * API is the ability to calculate a single result either for
 * an area or for a single point. All the remaining work is in
 * initializing the class implementing the abstract interface
 * with query data, map information and so on.
 *
 * The WeatehrStatistician class specializes in analyzing statistical
 * weather information, usually climatological values.
 *
 * \see WeatherForecaster, WeatherObserver
 */
// ======================================================================

#include "WeatherStatistician.h"

