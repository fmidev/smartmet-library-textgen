// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::FireWarnings
 */
// ======================================================================

#include "FireWarnings.h"
#include "WeatherAnalysisError.h"
#include <newbase/NFmiFileSystem.h>
#include <newbase/NFmiStringTools.h>

using namespace std;

namespace WeatherAnalysis
{

  //! Firewarning areas run from 1 to 46

  const int MaxAreaCode = 46;

  // ----------------------------------------------------------------------
  /*!
   * \brief FireWarnings constructor
   */
  // ----------------------------------------------------------------------

  FireWarnings::FireWarnings(const string & theDirectory,
							 const NFmiTime & theTime)
	: itsTime(theTime)
	, itsWarnings(MaxAreaCode+1,Undefined)
  {
	if(!NFmiFileSystem::DirectoryExists(theDirectory))
	  throw WeatherAnalysisError("Directory '"+theDirectory+"' required by class FireWarnings does not exist");

	// Form the expected filename of form YYYYMMDD.palot_koodina
	// If we cannot find the file, an exception is thrown

	string filename = (theDirectory +
					   '/' +
					   theTime.ToStr(kYYYYMMDD).CharPtr() +
					   ".palot_koodina");
	if(!NFmiFileSystem::FileExists(filename))
	  {
		NFmiTime tmp = theTime;
		tmp.ChangeByDays(-1);
		filename = (theDirectory +
					'/' +
					tmp.ToStr(kYYYYMMDD).CharPtr() +
					".palot_koodina");
		if(!NFmiFileSystem::FileExists(filename))
		  throw WeatherAnalysisError("Cannot find warnings from '"+theDirectory+"'");
	  }

	// Read the file

	ifstream input(filename.c_str(), ios::in);
	if(!input)
	  throw WeatherAnalysisError("Failed to open '"+filename+"' for reading");

	// Skip the date
	string tmp;
	input >> tmp;

	// Read the areacode - state pairs

	int areacode;
	int areastate;
	while(input >> areacode >> areastate)
	  {
		if(areacode < 1 || areacode > MaxAreaCode)
		  throw WeatherAnalysisError("File '"+filename+"' contains invalid areacode "+NFmiStringTools::Convert(areacode));
		switch(State(areastate))
		  {
		  case None:
		  case GrassFireWarning:
		  case FireWarning:
			itsWarnings[areacode] = State(areastate);
			break;
		  default:
			throw WeatherAnalysisError("File '"+filename+"' contains invalid warningcode "+NFmiStringTools::Convert(areastate));
		  }
	  }
	input.close();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief get the warning state for the given area code
   *
   * \param theArea The area code (1...46)
   * \return The state
   */
  // ----------------------------------------------------------------------

  FireWarnings::State FireWarnings::state(int theArea) const
  {
	if(theArea < 1 || theArea > MaxAreaCode)
	  throw WeatherAnalysisError("Only area codes 1...46 are allowed in FireWarnings");
	return itsWarnings[theArea];
  }

  


} // namespace WeatherAnalysis

// ======================================================================
