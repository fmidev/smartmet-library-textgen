// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WeatherFact
 */
// ======================================================================

#pragma once

#include <calculator/WeatherArea.h>
#include <calculator/WeatherFunction.h>
#include <calculator/WeatherParameter.h>
#include <calculator/WeatherPeriod.h>
#include <calculator/WeatherResult.h>

namespace TextGen
{
class WeatherFact
{
 public:
  ~WeatherFact();
  WeatherFact(const WeatherFact& theFact);
  WeatherFact& operator=(const WeatherFact& theFact);

  WeatherFact(WeatherParameter theParameter,
              WeatherFunction theFunction,
              const WeatherPeriod& thePeriod,
              const WeatherArea& theArea,
              const WeatherResult& theResult);

  WeatherParameter parameter() const;
  WeatherFunction function() const;
  const WeatherPeriod& period() const;
  const WeatherArea& area() const;
  const WeatherResult& result() const;

 private:
  WeatherFact() = delete;

  WeatherParameter itsParameter;
  WeatherFunction itsFunction;
  WeatherPeriod itsPeriod;
  WeatherArea itsArea;
  WeatherResult itsResult;

};  // class WeatherFact

}  // namespace TextGen

// Free functions

bool operator==(const TextGen::WeatherFact& theLhs, const TextGen::WeatherFact& theRhs);

bool operator!=(const TextGen::WeatherFact& theLhs, const TextGen::WeatherFact& theRhs);

// ======================================================================
