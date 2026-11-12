// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class TextGen::WeatherStatistician
 */
// ======================================================================

#pragma once

#include <calculator/DefaultAcceptor.h>
#include <calculator/NullAcceptor.h>
#include <calculator/WeatherAnalyzer.h>
#include <calculator/WeatherFunction.h>
#include <calculator/WeatherParameter.h>

namespace TextGen
{

class WeatherStatistician : public WeatherAnalyzer
{
 public:
  ~WeatherStatistician() override = default;
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
                        const Acceptor& theTester = NullAcceptor()) const override = 0;

};  // class WeatherStatistician

}  // namespace TextGen

// ======================================================================
