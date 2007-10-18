// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::FireWarnings
 */
// ----------------------------------------------------------------------

#ifndef WEATHERANALYSIS_FIREWARNINGS_H
#define WEATHERANALYSIS_FIREWARNINGS_H

#include <newbase/NFmiTime.h>
#include <string>
#include <vector>

namespace WeatherAnalysis
{
  class FireWarnings
  {
  public:

	enum State
	  {
		None = 0,
		GrassFireWarning = 1,
		FireWarning = 2,
		Undefined = 999
	  };

	FireWarnings(const std::string & theDirectory,
				 const NFmiTime & theTime);
	State state(int theArea) const;

  private:

	FireWarnings();
	const NFmiTime itsTime;
	std::vector<State> itsWarnings;

  }; // class FireWarnings
} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_FIREWARNINGS_H

// ======================================================================
