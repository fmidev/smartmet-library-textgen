// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace SeasonTools
 */
// ======================================================================
/*!
 * \namespace WeatherAnalysis::SeasonTools
 *
 * \brief Utilities to determine the season of the given date
 *
 *
 */
// ======================================================================


#include "AreaTools.h"
#include "LandMaskSource.h"
#include "RegularMaskSource.h"

#include <newbase/NFmiIndexMask.h>
typedef RegularMaskSource::mask_type mask_type;


//#include "Settings.h"
//#include "TextGenError.h"
//#include <newbase/NFmiTime.h>
//#include <newbase/NFmiStringTools.h>

//#include <ctime>

//using namespace Settings;
//using namespace TextGen;


namespace TextGen
{
  namespace AreaTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Tests if the are1 is part of are2
	 *
	 * \param theWeatherSource The source for weather data
	 * \param theWeatherArea1 The weather area 1
	 * \param theWeatherArea2 The weather area 2
	 *
	 * \return True if area 1 is part of area 2 false otherwise
	 */
	// ----------------------------------------------------------------------

	const bool isPartOfArea(const UserWeatherSource& theWeatherSource,
							const WeatherArea& theWeatherArea1,
							const WeatherArea& theWeatherArea2)
	{
	  if(theWeatherArea1.isPoint())
		{
		  return theWeatherArea2.path().IsInside(theWeatherArea1.point());
		}
	  else
		{
		  LandMaskSource area1LandMaskSource(theWeatherArea1);

		  mask_type mask = area1LandMaskSource.mask(theWeatherArea2, "data", theWeatherSource);

		  int size_of_mask = mask->size();

		  return size_of_mask > 0;
		}
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Tests if the are1 is part of are2
	 *
	 * \param theWeatherArea1 The weather area 1
	 * \param theArea2SvgFile The svg-file, that defines area 2
	 * \param theQueryData The query data
	 *
	 * \return True if area 1 is part of area 2 false otherwise
	 */
	// ----------------------------------------------------------------------

	const bool isPartOfArea(const WeatherArea& theWeatherArea1,
							const std::string& theArea2SvgFile,
							const std::string theQueryData)
	{
	  shared_ptr<NFmiStreamQueryData> sqd(new NFmiStreamQueryData());
	  if(!sqd->ReadData(theQueryData))
		throw runtime_error("Failed to read '"+theQueryData+"'");
	
	  UserWeatherSource theWeatherSource;
	  theWeatherSource.insert("data", sqd);

	  WeatherArea theWeatherArea2(theArea2SvgFile);

	  return isPartOfArea(theWeatherSource, theWeatherArea1, theWeatherArea2);
	}
  } // namespace AreaTools
} // namespace TextGen
// ======================================================================
