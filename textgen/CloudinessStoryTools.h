// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::CloudinessStoryTools
 */
// ======================================================================

#pragma once

#include <string>
#include <utility>
#include <vector>

namespace TextGen
{
class AnalysisSources;
class WeatherArea;
class WeatherPeriod;
}  // namespace TextGen

namespace TextGen
{
class Sentence;

namespace CloudinessStoryTools
{
// ----------------------------------------------------------------------
/*!
 * \brief Main cloudiness type classification
 *
 * Note that some algorithms may use only a subset of these
 * different cases.
 */
// ----------------------------------------------------------------------

enum CloudinessType
{
  Cloudy,                //!< pilvistä
  PartlyCloudy,          //!< puolipilvistä
  Clear,                 //!< selkeää
  MostlyCloudy,          //!< enimmäkseen pilvistä
  MostlyPartlyCloudy,    //!< enimmäkseen puolipilvistä
  MostlyClear,           //!< enimmäkseen selkeää
  CloudyOrPartlyCloudy,  //!< pilvistä tai puolipilvistä
  ClearOrPartlyCloudy,   //!< selkeää tai puolipilvistä
  DecreasingCloudiness,  //!< selkenevää
  IncreasingCloudiness,  //!< pilvistyvää
  VariableCloudiness     //!< vaihtelevaa pilvisyyttä
};

CloudinessType cloudiness_type(const std::string& theVar,
                               double theCloudyPercentage,
                               double theClearPercentage,
                               double theTrend);

using CommonCloudiness = std::pair<CloudinessType, bool>;

CommonCloudiness similar_type(CloudinessType theType1, CloudinessType theType2);

CommonCloudiness similar_type(const std::vector<CloudinessType>& theTypes);

CommonCloudiness similar_type(const std::vector<CloudinessType>& theTypes,
                              unsigned int theStart,
                              unsigned int theEnd);

Sentence cloudiness_phrase(CloudinessType theType);

Sentence cloudiness_phrase(const TextGen::AnalysisSources& theSources,
                           const TextGen::WeatherArea& theArea,
                           const TextGen::WeatherPeriod& thePeriod,
                           const std::string& theVar,
                           int theDay);

}  // namespace CloudinessStoryTools
}  // namespace TextGen

// ======================================================================
