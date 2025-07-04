// ======================================================================
/*!
 * \file
 * \brief Implementation of FogForecast class
 */
// ======================================================================

#include "FogForecast.h"
#include "AreaTools.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "MessageLogger.h"
#include "NightAndDayPeriodGenerator.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "SubMaskExtractor.h"
#include "ValueAcceptor.h"
#include "WeatherStory.h"
#include "WeekdayTools.h"
#include <boost/lexical_cast.hpp>
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/NullPeriodGenerator.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/TimeTools.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiAreaTools.h>
#include <newbase/NFmiCombinedParam.h>
#include <map>
#include <vector>

namespace TextGen
{
using namespace Settings;
using namespace TextGen;
using namespace AreaTools;

using namespace std;

#define TIME_PLACE_INPLACES_FOG_COMPOSITE_PHRASE "[huomenna] [sisamaassa] [paikoin] sumua"
#define TIME_PLACE_FOG_COMPOSITE_PHRASE "[huomenna] [sisamaassa] sumua"
#define PLACE_INPLACES_FOG_COMPOSITE_PHRASE "[sisamaassa] [paikoin] sumua"
#define PLACE_FOG_COMPOSITE_PHRASE "[sisamaassa] sumua"
#define TIME_INPLACES_FOG_COMPOSITE_PHRASE "[huomenna] [paikoin] sumua"
#define TIME_FOG_COMPOSITE_PHRASE "[huomenna] sumua"
#define INPLACES_FOG_COMPOSITE_PHRASE "[paikoin] sumua"

#define TIME_PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [paikoin] sumua, joka voi olla sakeaa"
#define TIME_PLACE_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] [sisamaassa] sumua, joka voi olla sakeaa"
#define PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE \
  "[sisamaassa] [paikoin] sumua, joka voi olla sakeaa"
#define PLACE_FOG_DENSE_COMPOSITE_PHRASE "[sisamaassa] sumua, joka voi olla sakeaa"
#define TIME_INPLACES_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] [paikoin] sumua, joka voi olla sakeaa"
#define TIME_FOG_DENSE_COMPOSITE_PHRASE "[huomenna] sumua, joka voi olla sakeaa"
#define INPLACES_FOG_DENSE_COMPOSITE_PHRASE "[paikoin] sumua, joka voi olla sakeaa"

/*
std::ostream& operator<<(std::ostream & theOutput,
                                                 const FogIntensityDataItem&
theFogIntensityDataItem)
{
      theOutput << "moderate fog=" << theFogIntensityDataItem.moderateFogExtent << " ";
      theOutput << "dense fog=" << theFogIntensityDataItem.denseFogExtent << " ";
      return theOutput;
}

std::ostream& operator<<(std::ostream & theOutput,
                                                 const CloudinessDataItem& theCloudinessDataItem)
{
      if(theCloudinessDataItem.theCoastalData)
        {
              theOutput << "  Coastal" << endl;
              theOutput << *theCloudinessDataItem.theCoastalData;
        }
      if(theCloudinessDataItem.theInlandData)
        {
              theOutput << "  Inland" << endl;
              theOutput << *theCloudinessDataItem.theInlandData;
        }
      if(theCloudinessDataItem.theFullData)
        {
              theOutput << "  Full area" << endl;
              theOutput << *theCloudinessDataItem.theFullData;
        }
      return theOutput;
}
*/

pair<WeatherPeriod, FogIntensityDataItem> get_fog_wp(const TextGenPosixTime& theStartTime,
                                                     const TextGenPosixTime& theEndTime,
                                                     const float& theModerateFogExtent,
                                                     const float& theDenseFogExtent)
{
  return make_pair(WeatherPeriod(theStartTime, theEndTime),
                   FogIntensityDataItem(theModerateFogExtent, theDenseFogExtent));
}

pair<WeatherPeriod, fog_type_id> get_fog_type_wp(const TextGenPosixTime& theStartTime,
                                                 const TextGenPosixTime& theEndTime,
                                                 const fog_type_id& theFogTypeId)
{
  return make_pair(WeatherPeriod(theStartTime, theEndTime), theFogTypeId);
}

const char* get_fog_type_string(const fog_type_id& theFogTypeId)
{
  const char* retval = "";

#if ENABLE_DENSE_FOG
  switch (theFogTypeId)
  {
    case FOG:
      retval = "sumua";
      break;
    case FOG_POSSIBLY_DENSE:
      retval = "sumua, joka voi olla sakeaa";
      break;

    case FOG_IN_SOME_PLACES:
      retval = "paikoin sumua";
      break;
    case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
      retval = "paikoin sumua, joka voi olla sakeaa";
      break;

    case FOG_IN_MANY_PLACES:
      retval = "monin paikoin sumua";
      break;
    case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
      retval = "monin paikoin sumua, joka voi olla sakeaa";
      break;
    default:
      retval = "ei sumua";
      break;
  }
#else
  switch (theFogTypeId)
  {
    case FOG:
    case FOG_POSSIBLY_DENSE:
      retval = "sumua";
      break;

    case FOG_IN_SOME_PLACES:
    case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
      retval = "paikoin sumua";
      break;

    case FOG_IN_MANY_PLACES:
    case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
      retval = "monin paikoin sumua";
      break;
    default:
      retval = "ei sumua";
      break;
  }
#endif

  return retval;
}

fog_type_id get_fog_type(const float& theModerateFog,
                         const float& theDenseFog,
                         bool theReportInManyPlaces = true)
{
  float totalFog = theModerateFog + theDenseFog;

  if (totalFog < IN_SOME_PLACES_LOWER_LIMIT_FOG || theModerateFog == kFloatMissing ||
      theDenseFog == kFloatMissing)
  {
    return NO_FOG;
  }
  if (totalFog >= IN_SOME_PLACES_LOWER_LIMIT_FOG && totalFog <= IN_SOME_PLACES_UPPER_LIMIT)
  {
    if (theDenseFog < IN_SOME_PLACES_LOWER_LIMIT_FOG)
      return FOG_IN_SOME_PLACES;
    return FOG_IN_SOME_PLACES_POSSIBLY_DENSE;
  }
  if (totalFog > IN_MANY_PLACES_LOWER_LIMIT && totalFog < IN_MANY_PLACES_UPPER_LIMIT)
  {
    if (theDenseFog < IN_SOME_PLACES_LOWER_LIMIT_FOG)
      return (theReportInManyPlaces ? FOG_IN_MANY_PLACES : FOG_IN_SOME_PLACES);
    return (theReportInManyPlaces ? FOG_IN_MANY_PLACES_POSSIBLY_DENSE
                                  : FOG_IN_SOME_PLACES_POSSIBLY_DENSE);
  }
  else
  {
    if (theDenseFog < IN_SOME_PLACES_LOWER_LIMIT_FOG)
      return FOG;
    return FOG_POSSIBLY_DENSE;
  }
}

FogForecast::FogForecast(wf_story_params& parameters, bool visibilityForecast /*= false*/)
    : theParameters(parameters),
      theCoastalModerateFogData(nullptr),
      theInlandModerateFogData(nullptr),
      theFullAreaModerateFogData(nullptr),
      theCoastalDenseFogData(nullptr),
      theInlandDenseFogData(nullptr),
      theFullAreaDenseFogData(nullptr),
      theVisibityForecastAtSea(visibilityForecast)
{
  if (theParameters.theForecastArea & FULL_AREA)
  {
    theFullAreaModerateFogData =
        ((*theParameters.theCompleteData[FULL_AREA])[FOG_INTENSITY_MODERATE_DATA].get());
    theFullAreaDenseFogData =
        ((*theParameters.theCompleteData[FULL_AREA])[FOG_INTENSITY_DENSE_DATA].get());
  }
  if (theParameters.theForecastArea & COASTAL_AREA)
  {
    theCoastalModerateFogData =
        ((*theParameters.theCompleteData[COASTAL_AREA])[FOG_INTENSITY_MODERATE_DATA].get());
    theCoastalDenseFogData =
        ((*theParameters.theCompleteData[COASTAL_AREA])[FOG_INTENSITY_DENSE_DATA].get());
  }
  if (theParameters.theForecastArea & INLAND_AREA)
  {
    theInlandModerateFogData =
        ((*theParameters.theCompleteData[INLAND_AREA])[FOG_INTENSITY_MODERATE_DATA].get());
    theInlandDenseFogData =
        ((*theParameters.theCompleteData[INLAND_AREA])[FOG_INTENSITY_DENSE_DATA].get());
  }

  findOutFogPeriods();
  findOutFogTypePeriods();

#ifdef MYDEBUG
  std::cout << "FOG PERIODS: " << std::endl;
  for (auto a : theFullAreaFog)
    std::cout << as_string(a.first)
              << ": moderate ext, dense ext: " << a.second.theModerateFogExtent << ", "
              << a.second.theDenseFogExtent << std::endl;
  std::cout << " ----- " << std::endl << std::endl;

  std::cout << "FOG TYPE PERIODS: " << std::endl;
  for (auto a : theFullAreaFogType)
    std::cout << as_string(a.first) << " -> " << get_fog_type_string(a.second) << std::endl;
  std::cout << " ----- " << std::endl;
#endif
}

void FogForecast::findOutFogPeriods(const weather_result_data_item_vector* theModerateFogData,
                                    const weather_result_data_item_vector* theDenseFogData,
                                    fog_period_vector& theFogPeriods)
{
  if (theModerateFogData && theDenseFogData && !theModerateFogData->empty())
  {
    TextGenPosixTime fogPeriodStartTime(theModerateFogData->at(0)->thePeriod.localStartTime());

    for (unsigned int i = 1; i < theModerateFogData->size(); i++)
    {
      if (i == theModerateFogData->size() - 1)
      {
        if ((theModerateFogData->at(i)->theResult.value() !=
                 theModerateFogData->at(i - 1)->theResult.value() ||
             theDenseFogData->at(i)->theResult.value() !=
                 theDenseFogData->at(i - 1)->theResult.value()))
        {
          theFogPeriods.push_back(
              get_fog_wp(fogPeriodStartTime,
                         theModerateFogData->at(i - 1)->thePeriod.localEndTime(),
                         theModerateFogData->at(i - 1)->theResult.value(),
                         theDenseFogData->at(i - 1)->theResult.value()));
          theFogPeriods.push_back(get_fog_wp(theModerateFogData->at(i)->thePeriod.localStartTime(),
                                             theModerateFogData->at(i)->thePeriod.localEndTime(),
                                             theModerateFogData->at(i)->theResult.value(),
                                             theDenseFogData->at(i)->theResult.value()));
        }
        else
        {
          theFogPeriods.push_back(get_fog_wp(fogPeriodStartTime,
                                             theModerateFogData->at(i)->thePeriod.localEndTime(),
                                             theModerateFogData->at(i)->theResult.value(),
                                             theDenseFogData->at(i)->theResult.value()));
        }
      }
      else
      {
        if ((theModerateFogData->at(i)->theResult.value() !=
                 theModerateFogData->at(i - 1)->theResult.value() ||
             theDenseFogData->at(i)->theResult.value() !=
                 theDenseFogData->at(i - 1)->theResult.value()))
        {
          theFogPeriods.push_back(
              get_fog_wp(fogPeriodStartTime,
                         theModerateFogData->at(i - 1)->thePeriod.localEndTime(),
                         theModerateFogData->at(i - 1)->theResult.value(),
                         theDenseFogData->at(i - 1)->theResult.value()));
          fogPeriodStartTime = theModerateFogData->at(i)->thePeriod.localStartTime();
        }
      }
    }
  }
}

void FogForecast::findOutFogPeriods()
{
  findOutFogPeriods(theCoastalModerateFogData, theCoastalDenseFogData, theCoastalFog);
  findOutFogPeriods(theInlandModerateFogData, theInlandDenseFogData, theInlandFog);
  findOutFogPeriods(theFullAreaModerateFogData, theFullAreaDenseFogData, theFullAreaFog);
}

void FogForecast::findOutFogTypePeriods(const fog_period_vector& theFogPeriods,
                                        fog_type_period_vector& theFogTypePeriods) const
{
  if (theFogPeriods.empty())
    return;

  TextGenPosixTime fogPeriodStartTime(theFogPeriods.at(0).first.localStartTime());
  fog_type_id previousFogType = get_fog_type(theFogPeriods.at(0).second.theModerateFogExtent,
                                             theFogPeriods.at(0).second.theDenseFogExtent,
                                             !theVisibityForecastAtSea);
  for (unsigned int i = 1; i < theFogPeriods.size(); i++)
  {
    fog_type_id currentFogType = get_fog_type(theFogPeriods.at(i).second.theModerateFogExtent,
                                              theFogPeriods.at(i).second.theDenseFogExtent,
                                              !theVisibityForecastAtSea);

    if (i == theFogPeriods.size() - 1)
    {
      if (previousFogType != currentFogType)
      {
        if (previousFogType != NO_FOG)
          theFogTypePeriods.push_back(get_fog_type_wp(
              fogPeriodStartTime, theFogPeriods.at(i - 1).first.localEndTime(), previousFogType));
        if (currentFogType != NO_FOG)
          theFogTypePeriods.push_back(get_fog_type_wp(theFogPeriods.at(i).first.localStartTime(),
                                                      theFogPeriods.at(i).first.localEndTime(),
                                                      currentFogType));
      }
      else
      {
        if (currentFogType != NO_FOG)
          theFogTypePeriods.push_back(get_fog_type_wp(
              fogPeriodStartTime, theFogPeriods.at(i).first.localEndTime(), currentFogType));
      }
    }
    else
    {
      if (previousFogType != currentFogType)
      {
        if (previousFogType != NO_FOG)
          theFogTypePeriods.push_back(get_fog_type_wp(
              fogPeriodStartTime, theFogPeriods.at(i - 1).first.localEndTime(), previousFogType));
        fogPeriodStartTime = theFogPeriods.at(i).first.localStartTime();
        previousFogType = currentFogType;
      }
    }
  }
}

void FogForecast::findOutFogTypePeriods()
{
  findOutFogTypePeriods(theCoastalFog, theCoastalFogType);
  findOutFogTypePeriods(theInlandFog, theInlandFogType);
  findOutFogTypePeriods(theFullAreaFog, theFullAreaFogType);
}

void FogForecast::printOutFogPeriods(std::ostream& theOutput,
                                     const fog_period_vector& theFogPeriods) const
{
  for (const auto& theFogPeriod : theFogPeriods)
  {
    WeatherPeriod period(theFogPeriod.first.localStartTime(), theFogPeriod.first.localEndTime());

    float moderateFog(theFogPeriod.second.theModerateFogExtent);
    float denseFog(theFogPeriod.second.theDenseFogExtent);

    theOutput << period.localStartTime() << "..." << period.localEndTime()
              << ": moderate=" << moderateFog << " dense=" << denseFog << endl;
  }
}

void FogForecast::printOutFogData(std::ostream& theOutput,
                                  const std::string& theLinePrefix,
                                  const weather_result_data_item_vector& theFogData) const
{
  for (const auto& i : theFogData)
  {
    WeatherPeriod period(i->thePeriod.localStartTime(), i->thePeriod.localEndTime());

    theOutput << i->thePeriod.localStartTime() << "..." << i->thePeriod.localEndTime() << ": "
              << theLinePrefix << "=" << i->theResult.value() << endl;
  }
}

void FogForecast::printOutFogData(std::ostream& theOutput) const
{
  theOutput << "** FOG DATA **" << endl;
  bool fogDataExists = false;

  if (theCoastalModerateFogData && !theCoastalModerateFogData->empty())
  {
    theOutput << "** Coastal moderate (1) fog data **" << endl;
    printOutFogData(theOutput, "moderate", *theCoastalModerateFogData);
    fogDataExists = true;
  }
  if (theCoastalDenseFogData && !theCoastalDenseFogData->empty())
  {
    theOutput << "** Coastal dense (2) fog data **" << endl;
    printOutFogData(theOutput, "dense", *theCoastalDenseFogData);
    fogDataExists = true;
  }

  if (theInlandModerateFogData && !theInlandModerateFogData->empty())
  {
    theOutput << "** Inland moderate (1) fog data **" << endl;
    printOutFogData(theOutput, "moderate", *theInlandModerateFogData);
    fogDataExists = true;
  }
  if (theInlandDenseFogData && !theInlandDenseFogData->empty())
  {
    theOutput << "** Inland dense (2) fog data **" << endl;
    printOutFogData(theOutput, "dense", *theInlandDenseFogData);
    fogDataExists = true;
  }

  if (theFullAreaModerateFogData && !theFullAreaModerateFogData->empty())
  {
    theOutput << "** Full area moderate (1) fog data **" << endl;
    printOutFogData(theOutput, "moderate", *theFullAreaModerateFogData);
    fogDataExists = true;
  }
  if (theFullAreaDenseFogData && !theFullAreaDenseFogData->empty())
  {
    theOutput << "** Full dense (2) fog data **" << endl;
    printOutFogData(theOutput, "dense", *theFullAreaDenseFogData);
    fogDataExists = true;
  }

  if (!fogDataExists)
    theOutput << "No Fog on this forecast period!" << endl;
}

void FogForecast::printOutFogPeriods(std::ostream& theOutput) const
{
  theOutput << "** FOG PERIODS **" << endl;

  if (!theCoastalFog.empty())
  {
    theOutput << "Coastal fog: " << endl;
    printOutFogPeriods(theOutput, theCoastalFog);
  }
  if (!theInlandFog.empty())
  {
    theOutput << "Inland fog: " << endl;
    printOutFogPeriods(theOutput, theInlandFog);
  }
  if (!theFullAreaFog.empty())
  {
    theOutput << "Full area fog: " << endl;
    printOutFogPeriods(theOutput, theFullAreaFog);
  }
}

void FogForecast::printOutFogTypePeriods(std::ostream& theOutput,
                                         const fog_type_period_vector& theFogTypePeriods) const
{
  for (const auto& theFogTypePeriod : theFogTypePeriods)
  {
    WeatherPeriod period(theFogTypePeriod.first.localStartTime(),
                         theFogTypePeriod.first.localEndTime());

    theOutput << period.localStartTime() << "..." << period.localEndTime() << ": "
              << get_fog_type_string(theFogTypePeriod.second) << endl;
  }
}

void FogForecast::printOutFogTypePeriods(std::ostream& theOutput) const
{
  theOutput << "** FOG TYPE PERIODS **" << endl;

  if (!theCoastalFog.empty())
  {
    theOutput << "Coastal fog types: " << endl;
    printOutFogTypePeriods(theOutput, theCoastalFogType);
  }
  if (!theInlandFog.empty())
  {
    theOutput << "Inland fog types: " << endl;
    printOutFogTypePeriods(theOutput, theInlandFogType);
  }
  if (!theFullAreaFog.empty())
  {
    theOutput << "Full area fog types: " << endl;
    printOutFogTypePeriods(theOutput, theFullAreaFogType);
  }
}

float FogForecast::getMean(const fog_period_vector& theFogPeriods,
                           const WeatherPeriod& theWeatherPeriod) const
{
  float sum(0.0);
  unsigned int count(0);

  for (const auto& theFogPeriod : theFogPeriods)
  {
    float totalFog =
        theFogPeriod.second.theModerateFogExtent + theFogPeriod.second.theDenseFogExtent;
    if (theFogPeriod.first.localStartTime() >= theWeatherPeriod.localStartTime() &&
        theFogPeriod.first.localStartTime() <= theWeatherPeriod.localEndTime() &&
        theFogPeriod.first.localEndTime() >= theWeatherPeriod.localStartTime() &&
        theFogPeriod.first.localEndTime() <= theWeatherPeriod.localEndTime() && totalFog > 0)
    {
      sum += totalFog;
      count++;
    }
  }

  return (count > 0 ? sum / count : 0);
}

Sentence FogForecast::getFogPhrase(const fog_type_id& theFogTypeId) const
{
  Sentence sentence;

#if ENABLE_DENSE_FOG
  switch (theFogTypeId)
  {
    case FOG:
      sentence << SUMUA_WORD;
      break;
    case FOG_POSSIBLY_DENSE:
      sentence << SUMUA_WORD << Delimiter(",") << JOKA_VOI_OLLA_SAKEAA_PHRASE;
      break;
    case FOG_IN_SOME_PLACES:
      sentence << PAIKOIN_WORD << SUMUA_WORD;
      break;
    case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
      sentence << PAIKOIN_WORD << SUMUA_WORD << Delimiter(",") << JOKA_VOI_OLLA_SAKEAA_PHRASE;
      break;
    case FOG_IN_MANY_PLACES:
      sentence << MONIN_PAIKOIN_WORD << SUMUA_WORD;
      break;
    case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
      sentence << MONIN_PAIKOIN_WORD << SUMUA_WORD << Delimiter(",") << JOKA_VOI_OLLA_SAKEAA_PHRASE;
      break;
    case NO_FOG:
      break;
  }
#else
  switch (theFogTypeId)
  {
    case FOG:
    case FOG_POSSIBLY_DENSE:
      sentence << SUMUA_WORD;
      break;
    case FOG_IN_SOME_PLACES:
    case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
      sentence << PAIKOIN_WORD << SUMUA_WORD;
      break;
    case FOG_IN_MANY_PLACES:
    case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
      sentence << MONIN_PAIKOIN_WORD << SUMUA_WORD;
      break;
    case NO_FOG:
      break;
  }
#endif

  return sentence;
}

WeatherPeriod FogForecast::getActualFogPeriod(const WeatherPeriod& theForecastPeriod,
                                              const WeatherPeriod& theFogPeriod,
                                              bool& theFogPeriodOkFlag) const
{
  int start_year(0);
  int start_month(0);
  int start_day(0);
  int start_hour(0);
  int end_year(0);
  int end_month(0);
  int end_day(0);
  int end_hour(0);
  theFogPeriodOkFlag = false;

  if (is_inside(theFogPeriod.localStartTime(), theForecastPeriod) &&
      !is_inside(theFogPeriod.localEndTime(), theForecastPeriod))
  {
    start_year = theFogPeriod.localStartTime().GetYear();
    start_month = theFogPeriod.localStartTime().GetMonth();
    start_day = theFogPeriod.localStartTime().GetDay();
    start_hour = theFogPeriod.localStartTime().GetHour();
    end_year = theForecastPeriod.localEndTime().GetYear();
    end_month = theForecastPeriod.localEndTime().GetMonth();
    end_day = theForecastPeriod.localEndTime().GetDay();
    end_hour = theForecastPeriod.localEndTime().GetHour();
    theFogPeriodOkFlag = true;
  }
  else if (is_inside(theFogPeriod.localEndTime(), theForecastPeriod) &&
           !is_inside(theFogPeriod.localStartTime(), theForecastPeriod))
  {
    start_year = theForecastPeriod.localStartTime().GetYear();
    start_month = theForecastPeriod.localStartTime().GetMonth();
    start_day = theForecastPeriod.localStartTime().GetDay();
    start_hour = theForecastPeriod.localStartTime().GetHour();
    end_year = theFogPeriod.localEndTime().GetYear();
    end_month = theFogPeriod.localEndTime().GetMonth();
    end_day = theFogPeriod.localEndTime().GetDay();
    end_hour = theFogPeriod.localEndTime().GetHour();
    theFogPeriodOkFlag = true;
  }
  else if (is_inside(theFogPeriod.localEndTime(), theForecastPeriod) &&
           is_inside(theFogPeriod.localStartTime(), theForecastPeriod))
  {
    start_year = theFogPeriod.localStartTime().GetYear();
    start_month = theFogPeriod.localStartTime().GetMonth();
    start_day = theFogPeriod.localStartTime().GetDay();
    start_hour = theFogPeriod.localStartTime().GetHour();
    end_year = theFogPeriod.localEndTime().GetYear();
    end_month = theFogPeriod.localEndTime().GetMonth();
    end_day = theFogPeriod.localEndTime().GetDay();
    end_hour = theFogPeriod.localEndTime().GetHour();
    theFogPeriodOkFlag = true;
  }
  else if (theFogPeriod.localStartTime() <= theForecastPeriod.localStartTime() &&
           theFogPeriod.localEndTime() >= theForecastPeriod.localEndTime())
  {
    start_year = theForecastPeriod.localStartTime().GetYear();
    start_month = theForecastPeriod.localStartTime().GetMonth();
    start_day = theForecastPeriod.localStartTime().GetDay();
    start_hour = theForecastPeriod.localStartTime().GetHour();
    end_year = theForecastPeriod.localEndTime().GetYear();
    end_month = theForecastPeriod.localEndTime().GetMonth();
    end_day = theForecastPeriod.localEndTime().GetDay();
    end_hour = theForecastPeriod.localEndTime().GetHour();
    theFogPeriodOkFlag = true;
  }

  if (theFogPeriodOkFlag)
  {
    TextGenPosixTime startTime(start_year, start_month, start_day, start_hour);
    TextGenPosixTime endTime(end_year, end_month, end_day, end_hour);

    WeatherPeriod theResultFogPeriod(startTime, endTime);

    theParameters.theLog << "getActualFogPeriod -> ";
    theParameters.theLog << "result: ";
    theParameters.theLog << startTime;
    theParameters.theLog << "...";
    theParameters.theLog << endTime << endl;

    return theResultFogPeriod;
  }

  return theFogPeriod;
}

bool FogForecast::getFogPeriodAndId(const WeatherPeriod& theForecastPeriod,
                                    const fog_type_period_vector& theFogTypePeriods,
                                    WeatherPeriod& theResultPeriod,
                                    fog_type_id& theFogTypeId) const
{
  bool fogPeriodOk(false);

  if (theFogTypePeriods.size() == 1)
  {
    WeatherPeriod actualFogPeriod(
        getActualFogPeriod(theForecastPeriod, theFogTypePeriods.at(0).first, fogPeriodOk));

    if (fogPeriodOk)
    {
      if (!(actualFogPeriod.localEndTime().DifferenceInHours(actualFogPeriod.localStartTime()) ==
                1 &&
            (actualFogPeriod.localStartTime() == theForecastPeriod.localStartTime() ||
             actualFogPeriod.localEndTime() == theForecastPeriod.localEndTime())))
      {
        theResultPeriod = actualFogPeriod;
        theFogTypeId = theFogTypePeriods.at(0).second;
        return true;
      }
    }
  }
  else
  {
    float forecastPeriodLength(
        theForecastPeriod.localEndTime().DifferenceInHours(theForecastPeriod.localStartTime()));
    int longestFogPeriodIndex(-1);
    int firstPeriodIndex(-1);
    int lastPeriodIndex(-1);
    int fogIdSum(0);
    int fogPeriodCount(0);
    map<unsigned int, unsigned int> encounteredFogTypes;

    // Merge close periods. If one long fog-period use that and ignore the small ones,
    // otherwise theFogTypeId is weighted average of all fog-periods and the
    // returned fog-period encompasses all small periods.
    for (unsigned int i = 0; i < theFogTypePeriods.size(); i++)
    {
      WeatherPeriod actualFogPeriod(
          getActualFogPeriod(theForecastPeriod, theFogTypePeriods.at(i).first, fogPeriodOk));

      if (!fogPeriodOk)
        continue;

      int actualPeriodLength(
          actualFogPeriod.localEndTime().DifferenceInHours(actualFogPeriod.localStartTime()));
      /*
  if (actualFogPeriod.localEndTime().DifferenceInHours(actualFogPeriod.localStartTime()) == 1 &&
      (actualFogPeriod.localStartTime() == theForecastPeriod.localStartTime() ||
       actualFogPeriod.localEndTime() == theForecastPeriod.localEndTime()))
    continue;
      */

      if (longestFogPeriodIndex == -1)
      {
        longestFogPeriodIndex = i;
        firstPeriodIndex = i;
      }
      else
      {
        WeatherPeriod longestPeriod(theFogTypePeriods.at(longestFogPeriodIndex).first);

        if (actualPeriodLength >
            longestPeriod.localEndTime().DifferenceInHours(longestPeriod.localStartTime()))
        {
          longestFogPeriodIndex = i;
        }
      }

      lastPeriodIndex = i;
      fogIdSum += (theFogTypePeriods.at(i).second * actualPeriodLength);
      fogPeriodCount += actualPeriodLength;
      encounteredFogTypes.insert(
          make_pair(theFogTypePeriods.at(i).second, theFogTypePeriods.at(i).second));
    }

    if (longestFogPeriodIndex >= 0)
    {
      //      WeatherPeriod longestPeriod(theFogTypePeriods.at(longestFogPeriodIndex).first);
      WeatherPeriod longestPeriod(getActualFogPeriod(
          theForecastPeriod, theFogTypePeriods.at(longestFogPeriodIndex).first, fogPeriodOk));
      float longestPeriodLength(
          longestPeriod.localEndTime().DifferenceInHours(longestPeriod.localStartTime()));

      // if the longest period is more than 70% of the forecast period, use it
      if (longestPeriodLength / forecastPeriodLength > 0.70)
      {
        theResultPeriod = longestPeriod;
        theFogTypeId = theFogTypePeriods.at(longestFogPeriodIndex).second;
        return true;
      }

      WeatherPeriod firstPeriod(getActualFogPeriod(
          theForecastPeriod, theFogTypePeriods.at(firstPeriodIndex).first, fogPeriodOk));
      WeatherPeriod lastPeriod(getActualFogPeriod(
          theForecastPeriod, theFogTypePeriods.at(lastPeriodIndex).first, fogPeriodOk));
      theResultPeriod = WeatherPeriod(firstPeriod.localStartTime(), lastPeriod.localEndTime());
      float periodIdAverage(static_cast<float>(fogIdSum) / static_cast<float>(fogPeriodCount));

      // find the fog type that is closest to the average
      fog_type_id finalFogType = NO_FOG;
      float fogTypeGap = 10.0;
      for (unsigned int i = FOG; i < NO_FOG; i++)
      {
        if (encounteredFogTypes.find(i) != encounteredFogTypes.end())
        {
          if (finalFogType == NO_FOG)
          {
            finalFogType = static_cast<fog_type_id>(i);
            fogTypeGap = abs(periodIdAverage - i);
          }
          else
          {
            if (fogTypeGap > abs(periodIdAverage - static_cast<float>(i)))
            {
              finalFogType = static_cast<fog_type_id>(i);
              fogTypeGap = abs(periodIdAverage - i);
            }
          }
        }
      }
      theFogTypeId = finalFogType;
      return (theFogTypeId != NO_FOG);
    }
  }
  return false;
}

Sentence FogForecast::constructFogSentence(const std::string& theDayPhasePhrase,
                                           const std::string& theAreaString,
                                           const std::string& theInPlacesString,
                                           bool thePossiblyDenseFlag) const
{
  Sentence sentence;

  bool dayPhaseExists(!theDayPhasePhrase.empty());
  bool placeExists(!theAreaString.empty());
  bool inPlacesPhraseExists(!theInPlacesString.empty());

#if !ENABLE_DENSE_FOG
  thePossiblyDenseFlag = false;
#endif

  if (dayPhaseExists)
  {
    if (placeExists)
    {
      if (inPlacesPhraseExists)
      {
        if (thePossiblyDenseFlag)
          sentence << TIME_PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
        else
          sentence << TIME_PLACE_INPLACES_FOG_COMPOSITE_PHRASE;

        sentence << theDayPhasePhrase << theAreaString << theInPlacesString;
      }
      else
      {
        if (thePossiblyDenseFlag)
          sentence << TIME_PLACE_FOG_DENSE_COMPOSITE_PHRASE;
        else
          sentence << TIME_PLACE_FOG_COMPOSITE_PHRASE;

        sentence << theDayPhasePhrase << theAreaString;
      }
    }
    else
    {
      if (inPlacesPhraseExists)
      {
        if (thePossiblyDenseFlag)
          sentence << TIME_INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
        else
          sentence << TIME_INPLACES_FOG_COMPOSITE_PHRASE;
        sentence << theDayPhasePhrase << theInPlacesString;
      }
      else
      {
        if (thePossiblyDenseFlag)
          sentence << TIME_FOG_DENSE_COMPOSITE_PHRASE;
        else
          sentence << TIME_FOG_COMPOSITE_PHRASE;
        sentence << theDayPhasePhrase;
      }
    }
  }
  else
  {
    if (placeExists)
    {
      if (inPlacesPhraseExists)
      {
        if (thePossiblyDenseFlag)
          sentence << PLACE_INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
        else
          sentence << PLACE_INPLACES_FOG_COMPOSITE_PHRASE;
        sentence << theAreaString << theInPlacesString;
      }
      else
      {
        if (thePossiblyDenseFlag)
          sentence << INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
        else
          sentence << INPLACES_FOG_COMPOSITE_PHRASE;
        sentence << theAreaString;
      }
    }
    else
    {
      if (inPlacesPhraseExists)
      {
        if (thePossiblyDenseFlag)
          sentence << INPLACES_FOG_DENSE_COMPOSITE_PHRASE;
        else
          sentence << INPLACES_FOG_COMPOSITE_PHRASE;
        sentence << theInPlacesString;
      }
      else
      {
        sentence << SUMUA_WORD;
        if (thePossiblyDenseFlag)
          sentence << Delimiter(",") << JOKA_VOI_OLLA_SAKEAA_PHRASE;
      }
    }
  }

  return sentence;
}

Sentence FogForecast::fogSentence(const WeatherPeriod& thePeriod,
                                  const fog_type_period_vector& theFogTypePeriods,
                                  const std::string& theAreaString) const
{
  Sentence sentence;
  WeatherPeriod fogPeriod(thePeriod);
  fog_type_id fogTypeId(NO_FOG);

  if (getFogPeriodAndId(thePeriod, theFogTypePeriods, fogPeriod, fogTypeId))
  {
    Sentence todayPhrase;

    if (thePeriod.localEndTime().DifferenceInHours(
            theParameters.theForecastPeriod.localStartTime()) > 24)
    {
      todayPhrase << PeriodPhraseFactory::create("today",
                                                 theParameters.theVariable,
                                                 theParameters.theForecastTime,
                                                 fogPeriod,
                                                 theParameters.theArea);
    }
    theParameters.theLog << todayPhrase;

    vector<std::string> theStringVector;

    bool specifyDay =
        get_period_length(theParameters.theForecastPeriod) > 24 && !todayPhrase.empty();

    std::string dayPhasePhrase;
    part_of_the_day_id id;
    get_time_phrase_large(
        fogPeriod, specifyDay, theParameters.theVariable, dayPhasePhrase, false, id);

    WeatherHistory& thePhraseHistory = const_cast<WeatherArea&>(theParameters.theArea).history();

    if (dayPhasePhrase == thePhraseHistory.latestDayPhasePhrase)
      dayPhasePhrase = "";
    else
      thePhraseHistory.updateDayPhasePhrase(dayPhasePhrase);

    switch (fogTypeId)
    {
      case FOG:
        sentence << constructFogSentence(dayPhasePhrase, theAreaString, "", false);
        break;
      case FOG_POSSIBLY_DENSE:
        sentence << constructFogSentence(dayPhasePhrase, theAreaString, "", true);
        break;
      case FOG_IN_SOME_PLACES:
        sentence << constructFogSentence(dayPhasePhrase, theAreaString, PAIKOIN_WORD, false);
        break;
      case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
        sentence << constructFogSentence(dayPhasePhrase, theAreaString, PAIKOIN_WORD, true);
        break;
      case FOG_IN_MANY_PLACES:
        sentence << constructFogSentence(dayPhasePhrase, theAreaString, MONIN_PAIKOIN_WORD, false);
        break;
      case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
        sentence << constructFogSentence(dayPhasePhrase, theAreaString, MONIN_PAIKOIN_WORD, true);
        break;
      default:
        break;
    };
  }

  return sentence;
}

FogInfo FogForecast::fogInfo(const WeatherPeriod& thePeriod,
                             const fog_type_period_vector& theFogTypePeriods,
                             const std::string& theAreaString) const
{
  FogInfo ret;

  WeatherPeriod fogPeriod(thePeriod);
  fog_type_id fogTypeId(NO_FOG);

  if (getFogPeriodAndId(thePeriod, theFogTypePeriods, fogPeriod, fogTypeId))
  {
    // 1-hour period ignored
    if (get_period_length(fogPeriod) == 0)
      return ret;

    ret.period = fogPeriod;
    Sentence todayPhrase;

    if (thePeriod.localEndTime().DifferenceInHours(
            theParameters.theForecastPeriod.localStartTime()) > 24)
    {
      todayPhrase << PeriodPhraseFactory::create("today",
                                                 theParameters.theVariable,
                                                 theParameters.theForecastTime,
                                                 fogPeriod,
                                                 theParameters.theArea);
    }

    bool specifyDay =
        get_period_length(theParameters.theForecastPeriod) > 24 && !todayPhrase.empty();
    std::string dayPhasePhrase;
    part_of_the_day_id id;
    get_time_phrase_large(
        fogPeriod, specifyDay, theParameters.theVariable, dayPhasePhrase, false, id);

    // TODO: check also 12:00 and also during weak precipitation
    // dont report fog if it appears couple of hours before forecast period end
    if (theParameters.theForecastPeriod.localEndTime().GetHour() == 18 &&
        abs(theParameters.theForecastPeriod.localEndTime().DifferenceInHours(
            fogPeriod.localStartTime())) < 2)
    /* &&
           id == ILTA)*/
    {
      return ret;
    }

    theParameters.theLog << todayPhrase;

    vector<std::string> theStringVector;

    ret.timePhrase << dayPhasePhrase;
    dayPhasePhrase.clear();

    switch (fogTypeId)
    {
      case FOG:
        ret.sentence << constructFogSentence(dayPhasePhrase, theAreaString, "", false);
        break;
      case FOG_POSSIBLY_DENSE:
        ret.sentence << constructFogSentence(dayPhasePhrase, theAreaString, "", true);
        break;
      case FOG_IN_SOME_PLACES:
        ret.sentence << constructFogSentence(dayPhasePhrase, theAreaString, PAIKOIN_WORD, false);
        break;
      case FOG_IN_SOME_PLACES_POSSIBLY_DENSE:
        ret.sentence << constructFogSentence(dayPhasePhrase, theAreaString, PAIKOIN_WORD, true);
        break;
      case FOG_IN_MANY_PLACES:
        ret.sentence << constructFogSentence(
            dayPhasePhrase, theAreaString, MONIN_PAIKOIN_WORD, false);
        break;
      case FOG_IN_MANY_PLACES_POSSIBLY_DENSE:
        ret.sentence << constructFogSentence(
            dayPhasePhrase, theAreaString, MONIN_PAIKOIN_WORD, true);
        break;
      default:
        break;
    };
    ret.id = fogTypeId;
  }

  return ret;
}

FogInfo FogForecast::fogInfo(const WeatherPeriod& thePeriod) const
{
  FogInfo ret;

  if (theParameters.theArea.isMarine() || theParameters.theArea.isIsland())
  {
    ret = fogInfo(thePeriod, theInlandFogType, EMPTY_STRING);
  }
  else
  {
    if (theParameters.theForecastArea & FULL_AREA)
    {
      float coastalFogAvgExtent(getMean(theCoastalFog, thePeriod));
      float inlandFogAvgExtent(getMean(theInlandFog, thePeriod));

      if (coastalFogAvgExtent >= IN_SOME_PLACES_LOWER_LIMIT_FOG &&
          inlandFogAvgExtent >= IN_SOME_PLACES_LOWER_LIMIT_FOG)
      {
        // ARE 31.10.2011: if fog exisists on both areas report the whole area together
        ret = fogInfo(thePeriod, theFullAreaFogType, EMPTY_STRING);
      }
      else if (coastalFogAvgExtent >= IN_SOME_PLACES_LOWER_LIMIT_FOG &&
               inlandFogAvgExtent < IN_SOME_PLACES_LOWER_LIMIT_FOG)
      {
        ret = fogInfo(thePeriod, theCoastalFogType, COAST_PHRASE);
      }
      else if (coastalFogAvgExtent < IN_SOME_PLACES_LOWER_LIMIT_FOG &&
               inlandFogAvgExtent >= IN_SOME_PLACES_LOWER_LIMIT_FOG)
      {
        ret = fogInfo(thePeriod, theInlandFogType, INLAND_PHRASE);
      }
    }
    else if (theParameters.theForecastArea & INLAND_AREA)
    {
      ret = fogInfo(thePeriod, theInlandFogType, EMPTY_STRING);
    }
    else if (theParameters.theForecastArea & COASTAL_AREA)
    {
      ret = fogInfo(thePeriod, theCoastalFogType, EMPTY_STRING);
    }
  }

  return ret;
}

Sentence FogForecast::fogSentence(const WeatherPeriod& thePeriod) const
{
  Sentence sentence;

  if (theParameters.theArea.isMarine() || theParameters.theArea.isIsland())
  {
    sentence << fogSentence(thePeriod, theInlandFogType, EMPTY_STRING);
  }
  else
  {
    if (theParameters.theForecastArea & FULL_AREA)
    {
      float coastalFogAvgExtent(getMean(theCoastalFog, thePeriod));
      float inlandFogAvgExtent(getMean(theInlandFog, thePeriod));

      if (coastalFogAvgExtent >= IN_SOME_PLACES_LOWER_LIMIT_FOG &&
          inlandFogAvgExtent >= IN_SOME_PLACES_LOWER_LIMIT_FOG)
      {
        // ARE 31.10.2011: if fog exisis on both areas report the whole area together
        sentence << fogSentence(thePeriod, theFullAreaFogType, EMPTY_STRING);
      }
      else if (coastalFogAvgExtent >= IN_SOME_PLACES_LOWER_LIMIT_FOG &&
               inlandFogAvgExtent < IN_SOME_PLACES_LOWER_LIMIT_FOG)
      {
        sentence << fogSentence(thePeriod, theCoastalFogType, COAST_PHRASE);
      }
      else if (coastalFogAvgExtent < IN_SOME_PLACES_LOWER_LIMIT_FOG &&
               inlandFogAvgExtent >= IN_SOME_PLACES_LOWER_LIMIT_FOG)
      {
        sentence << fogSentence(thePeriod, theInlandFogType, INLAND_PHRASE);
      }
    }
    else if (theParameters.theForecastArea & INLAND_AREA)
    {
      sentence << fogSentence(thePeriod, theInlandFogType, EMPTY_STRING);
    }
    else if (theParameters.theForecastArea & COASTAL_AREA)
    {
      sentence << fogSentence(thePeriod, theCoastalFogType, EMPTY_STRING);
    }
  }

  return sentence;
}

Sentence FogForecast::areaSpecificSentence(const WeatherPeriod& thePeriod) const
{
  Sentence sentence;

  // If the area contains both coast and inland, we don't ude area specific sentence

  if (!(theParameters.theForecastArea & FULL_AREA))
  {
    WeatherResult northEastShare(kFloatMissing, 0);
    WeatherResult southEastShare(kFloatMissing, 0);
    WeatherResult southWestShare(kFloatMissing, 0);
    WeatherResult northWestShare(kFloatMissing, 0);
    RangeAcceptor acceptor;
    acceptor.lowerLimit(kTModerateFog);
    acceptor.upperLimit(kTDenseFog);

    AreaTools::getArealDistribution(theParameters.theSources,
                                    Fog,
                                    theParameters.theArea,
                                    thePeriod,
                                    acceptor,
                                    northEastShare,
                                    southEastShare,
                                    southWestShare,
                                    northWestShare);

    float north = northEastShare.value() + northWestShare.value();
    float south = southEastShare.value() + southWestShare.value();
    float east = northEastShare.value() + southEastShare.value();
    float west = northWestShare.value() + southWestShare.value();

    area_specific_sentence_id sentenceId = get_area_specific_sentence_id(north,
                                                                         south,
                                                                         east,
                                                                         west,
                                                                         northEastShare.value(),
                                                                         southEastShare.value(),
                                                                         southWestShare.value(),
                                                                         northWestShare.value(),
                                                                         false);

    Rect areaRect(theParameters.theArea);

    std::unique_ptr<NFmiArea> mercatorArea(
        NFmiAreaTools::CreateLegacyMercatorArea(areaRect.getBottomLeft(), areaRect.getTopRight()));

    float areaHeightWidthRatio =
        mercatorArea->WorldRect().Height() / mercatorArea->WorldRect().Width();

    Sentence areaSpecificSentence;
    areaSpecificSentence << area_specific_sentence(north,
                                                   south,
                                                   east,
                                                   west,
                                                   northEastShare.value(),
                                                   southEastShare.value(),
                                                   southWestShare.value(),
                                                   northWestShare.value(),
                                                   false);

    // If the area is too cigar-shaped, we can use only north-south/east-west specifier
    if ((areaHeightWidthRatio >= 0.6 && areaHeightWidthRatio <= 1.5) ||
        (areaHeightWidthRatio < 0.6 &&
         (sentenceId == ALUEEN_ITAOSASSA || sentenceId == ALUEEN_LANSIOSASSA ||
          sentenceId == ENIMMAKSEEN_ALUEEN_ITAOSASSA ||
          sentenceId == ENIMMAKSEEN_ALUEEN_LANSIOSASSA)) ||
        (areaHeightWidthRatio > 1.5 &&
         (sentenceId == ALUEEN_POHJOISOSASSA || sentenceId == ALUEEN_ETELAOSASSA ||
          sentenceId == ENIMMAKSEEN_ALUEEN_POHJOISOSASSA ||
          sentenceId == ENIMMAKSEEN_ALUEEN_ETELAOSASSA)))
    {
      sentence << areaSpecificSentence;
    }
  }

  return sentence;
}
}  // namespace TextGen
