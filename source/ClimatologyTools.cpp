// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace ClimatologyTools
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::ClimatologyTools
 *
 * \brief Utilities to determine the season of the given date
 *
 *
 */
// ======================================================================

#include "ClimatologyTools.h"
#include "WeatherSource.h"
#include "Settings.h"

#include <newbase/NFmiStreamQueryData.h>
#include <boost/shared_ptr.hpp>

namespace WeatherAnalysis
{
  namespace ClimatologyTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Returns a climatology period of the data
	 *
	 * \param thePeriod Analysis period
	 * \param theDataName The name of the data file
	 * \param theSources Analysis sources
	 *
	 * \return The climatology period
	 */
	// ----------------------------------------------------------------------
	

	WeatherPeriod getClimatologyPeriod(const WeatherAnalysis::WeatherPeriod& thePeriod, 
									   const std::string& theDataName, 
									   const WeatherAnalysis::AnalysisSources& theSources)
	{
	  const string datavar = theDataName + "_climatology";
	  const string dataname = Settings::require_string(datavar);
	  boost::shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
	  boost::shared_ptr<NFmiStreamQueryData> qd = wsource->data(dataname);
	  NFmiFastQueryInfo * qi = qd->QueryInfoIter();
	  qi->FirstTime();
	  int year = qi->ValidTime().GetYear();
	  NFmiTime t1 = thePeriod.localStartTime();
	  NFmiTime t2 = thePeriod.localEndTime();
	  t1.SetYear(year);
	  t2.SetYear(year);
	  return WeatherPeriod(t1,t2); 
	}


  } // namespace ClimatologyTools
} // namespace WeatherAnalysis
// ======================================================================
