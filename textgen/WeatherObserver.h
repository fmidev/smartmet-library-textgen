// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class TextGen::WeatherObserver
 */
// ======================================================================

#pragma once

#include <calculator/WeatherAnalyzer.h>

namespace TextGen
{
class Acceptor;
class AnalysisSources;
class WeatherArea;
class WeatherPeriodGenerator;
class WeatherResult;

class WeatherObserver : public WeatherAnalyzer
{
 public:
  ~WeatherObserver() override {}
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

};  // class WeatherObserver

}  // namespace TextGen

// ======================================================================
