// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::CoastMaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::CoastMaskSource
 *
 * \brief Provides mask services to clients (masked to coastal areas)
 *
 * The CoastMaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas. This class is
 * differentiated from RegularMaskSource by the fact that any mask
 * is restricted to remain close to the coast. The acceptable coast
 * area is specified by giving the name of the coastal line
 * and the acceptable distance in kilometers from the coastal line.
 *
 */
// ======================================================================

#include "CoastMaskSource.h"
