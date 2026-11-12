// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::GridClimatology
 */
// ======================================================================

#pragma once

#include <calculator/DefaultAcceptor.h>
#include <calculator/NullAcceptor.h>
#include <calculator/WeatherAnalyzer.h>
#include <calculator/WeatherForecaster.h>
#include <calculator/WeatherFunction.h>
#include <calculator/WeatherParameter.h>

namespace TextGen
{

class GridClimatology : public WeatherForecaster
{
 public:
  using WeatherAnalyzer::analyze;

  WeatherResult analyze(const AnalysisSources& theSources,
                        const WeatherParameter& theParameter,
                        const WeatherFunction& theAreaFunction,
                        const WeatherFunction& theTimeFunction,
                        const WeatherFunction& theSubTimeFunction,
                        const WeatherArea& theArea,
                        const WeatherPeriodGenerator& thePeriods,
                        const Acceptor& theAreaAcceptor = DefaultAcceptor(),
                        const Acceptor& theTimeAcceptor = DefaultAcceptor(),
                        const Acceptor& theTester = NullAcceptor()) const override;

};  // class GridClimatology

}  // namespace TextGen

// ======================================================================
