// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WeatherFact
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherFact
 *
 * \brief Representation of a single fact about weather
 *
 * The purpose of a single WeatherFact object is to specify
 * a particular analysis type on the weather, and to provide
 * the result of the analysis along with its accuracy estimate.
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERFACT_H
#define WEATHERANALYSIS_WEATHERFACT_H

#include "WeatherArea.h"
#include "WeatherFunction.h"
#include "WeatherParameter.h"
#include "WeatherPeriod.h"
#include "WeatherResult.h"

namespace WeatherAnalysis
{
  class WeatherFact
  {
  public:

	~WeatherFact();
	WeatherFact(const WeatherFact & theFact);
	WeatherFact & operator=(const WeatherFact & theFact);

	WeatherFact(WeatherParameter theParameter,
				WeatherFunction theFunction,
				const WeatherPeriod & thePeriod,
				const WeatherArea & theArea,
				const WeatherResult & theResult);

	WeatherParameter parameter() const;
	WeatherFunction function() const;
	const WeatherPeriod & period() const;
	const WeatherArea & area() const;
	const WeatherResult & result() const;

  private:

	WeatherFact();

	WeatherParameter itsParameter;
	WeatherFunction itsFunction;
	WeatherPeriod itsPeriod;
	WeatherArea itsArea;
	WeatherResult itsResult;

  }; // class WeatherFact

} // namespace WeatherAnalysis

// Free functions

bool operator==(const WeatherAnalysis::WeatherFact & theLhs,
				const WeatherAnalysis::WeatherFact & theRhs);

bool operator!=(const WeatherAnalysis::WeatherFact & theLhs,
				const WeatherAnalysis::WeatherFact & theRhs);

#endif // WEATHERANALYSIS_WEATHERFACT_H

// ======================================================================
