// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::WeatherAnalysisError
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::WeatherAnalysisError
 *
 * \brief The generic exception thrown by WeatherAnalysis namespace
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_WEATHERANALYSISERROR_H
#define WEATHERANALYSIS_WEATHERANALYSISERROR_H

#include <stdexcept>
#include <string>

namespace WeatherAnalysis
{
  class WeatherAnalysisError : public std::exception
  {
  public:
	~WeatherAnalysisError() throw() { }
	WeatherAnalysisError(const std::string & theError);
	virtual const char * what(void) const throw();
  private:
	std::string itsError;

  }; // class WeatherAnalysisError
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_WEATHERANALYSISERROR_H

// ======================================================================
