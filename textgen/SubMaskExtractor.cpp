#include "SubMaskExtractor.h"
#include <calculator/Acceptor.h>
#include <calculator/AnalysisSources.h>
#include <calculator/MaskSource.h>
#include <calculator/ParameterAnalyzer.h>
#include <calculator/QueryDataTools.h>
#include <calculator/Settings.h>
#include <calculator/WeatherDataType.h>
#include <calculator/WeatherPeriod.h>
#include <calculator/WeatherSource.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiArea.h>
#include <newbase/NFmiEnumConverter.h>
#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiGlobals.h>
#include <newbase/NFmiGrid.h>
#include <newbase/NFmiIndexMaskTools.h>
#include <newbase/NFmiParameterName.h>
#include <newbase/NFmiPoint.h>
#include <newbase/NFmiSvgPath.h>
#include <newbase/NFmiSvgTools.h>
#include <cmath>
#include <iostream>
#include <math.h>
#include <memory>
#include <string>

class NFmiQueryData;

using namespace std;

// ----------------------------------------------------------------------
/*!
 * \brief Integrate over space
 *
 * Integrate over space with current time, parameter and level.
 *
 * It is assumed that the index mask was generated from a grid
 * equivalent to the one in the query data.
 *
 * \param theQI The query info
 * \param theIndexMask The mask containing the space indices
 * \param theSpaceCalculator The modifier for accumulating the result
 * \return The accumulation result
 */
// ----------------------------------------------------------------------

namespace
{
NFmiEnumConverter converter;
}  // namespace

namespace TextGen
{
double GetLocationCoordinates(const AnalysisSources& theSources,
                              const WeatherParameter& theParameter,
                              const WeatherArea& theArea,
                              const WeatherPeriod& thePeriod,
                              const Acceptor& theAcceptor,
                              vector<NFmiPoint*>& theResultData)
{
  try
  {
    double retval = 0.0;

    std::string parameterName;
    std::string dataName;

    ParameterAnalyzer::getParameterStrings(theParameter, parameterName, dataName);
    const string default_forecast = Settings::optional_string("textgen::default_forecast", "");
    const string datavar = dataName + '_' + data_type_name(Forecast);
    const string dataname = Settings::optional_string(datavar, default_forecast);

    // Get the data into use

    std::shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
    std::shared_ptr<NFmiQueryData> qd = wsource->data(dataname);
    NFmiFastQueryInfo theQI = NFmiFastQueryInfo(qd.get());

    auto param = FmiParameterName(converter.ToEnum(parameterName));
    if (param == kFmiBadParameter)
      throw Fmi::Exception(BCP, "Parameter " + parameterName + " is not defined in newbase");

    if (!theQI.Param(param))
      throw Fmi::Exception(BCP, parameterName + " is not available in " + dataname);

    if (!theArea.isPoint())
    {
      MaskSource::mask_type theIndexMask;

      switch (theArea.type())
      {
        case WeatherArea::Full:
          theIndexMask = theSources.getMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Land:
          theIndexMask = theSources.getLandMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Coast:
          theIndexMask = theSources.getCoastMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Inland:
          theIndexMask = theSources.getInlandMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Northern:
          theIndexMask = theSources.getNorthernMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Southern:
          theIndexMask = theSources.getSouthernMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Eastern:
          theIndexMask = theSources.getEasternMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Western:
          theIndexMask = theSources.getWesternMaskSource()->mask(theArea, dataname, *wsource);
          break;
      }

      if (theIndexMask->empty())
        return 0;

      unsigned long startindex = 0;
      unsigned long endindex = 0;

      if (!QueryDataTools::findIndices(
              theQI, thePeriod.utcStartTime(), thePeriod.utcEndTime(), startindex, endindex))
      {
        return 0;
      }

      for (unsigned long it : *theIndexMask)
      {
        theQI.TimeIndex(startindex);

        for (bool more = true; more; more = theQI.NextTime() && theQI.TimeIndex() < endindex)
        {
          // possible -1 is handled by IndexFloatValue
          const unsigned long idx =
              theQI.Index(theQI.ParamIndex(), it, theQI.LevelIndex(), theQI.TimeIndex());
          const float tmp = theQI.GetFloatValue(idx);

          if (theAcceptor.accept(tmp))
          {
            theResultData.push_back(new NFmiPoint(theQI.LatLon(it)));
            retval += tmp;
          }
        }
      }
    }

    return retval;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

double ExtractMask(const AnalysisSources& theSources,
                   const WeatherParameter& theParameter,
                   const WeatherArea& theArea,
                   const WeatherPeriod& thePeriod,
                   const Acceptor& theAcceptor,
                   NFmiIndexMask& theResultIndexMask)
{
  try
  {
    double retval = 0.0;

    std::string parameterName;
    std::string dataName;

    ParameterAnalyzer::getParameterStrings(theParameter, parameterName, dataName);
    const string default_forecast = Settings::optional_string("textgen::default_forecast", "");
    const string datavar = dataName + '_' + data_type_name(Forecast);
    const string dataname = Settings::optional_string(datavar, default_forecast);

    // Get the data into use

    std::shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
    std::shared_ptr<NFmiQueryData> qd = wsource->data(dataname);
    NFmiFastQueryInfo theQI = NFmiFastQueryInfo(qd.get());

    auto param = FmiParameterName(converter.ToEnum(parameterName));
    if (param == kFmiBadParameter)
      throw Fmi::Exception(BCP, "Parameter " + parameterName + " is not defined in newbase");

    if (!theQI.Param(param))
      throw Fmi::Exception(BCP, parameterName + " is not available in " + dataname);

    if (!theArea.isPoint())
    {
      MaskSource::mask_type theIndexMask;

      switch (theArea.type())
      {
        case WeatherArea::Full:
          theIndexMask = theSources.getMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Land:
          theIndexMask = theSources.getLandMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Coast:
          theIndexMask = theSources.getCoastMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Inland:
          theIndexMask = theSources.getInlandMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Northern:
          theIndexMask = theSources.getNorthernMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Southern:
          theIndexMask = theSources.getSouthernMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Eastern:
          theIndexMask = theSources.getEasternMaskSource()->mask(theArea, dataname, *wsource);
          break;
        case WeatherArea::Western:
          theIndexMask = theSources.getWesternMaskSource()->mask(theArea, dataname, *wsource);
          break;
      }

      if (theIndexMask->empty())
        return 0;

      unsigned long startindex = 0;
      unsigned long endindex = 0;

      if (!QueryDataTools::findIndices(
              theQI, thePeriod.utcStartTime(), thePeriod.utcEndTime(), startindex, endindex))
        return 0;

      for (unsigned long it : *theIndexMask)
      {
        theQI.TimeIndex(startindex);

        for (bool more = true; more; more = theQI.NextTime() && theQI.TimeIndex() < endindex)
        {
          // possible -1 is handled by IndexFloatValue
          const unsigned long idx =
              theQI.Index(theQI.ParamIndex(), it, theQI.LevelIndex(), theQI.TimeIndex());
          const float tmp = theQI.GetFloatValue(idx);

          if (theAcceptor.accept(tmp))
          {
            theResultIndexMask.insert(it);
            retval += tmp;
          }
        }
      }
    }

    return retval;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

namespace
{
// Check if a point falls in the requested directional sub-region
bool point_in_direction(const NFmiPoint& p,
                        const AreaTools::direction_id& theDirectionId,
                        double latitudeDivisionLine,
                        double longitudeDivisionLine)
{
  try
  {
    switch (theDirectionId)
    {
      case AreaTools::NORTH:
        return p.Y() >= latitudeDivisionLine;
      case AreaTools::SOUTH:
        return p.Y() < latitudeDivisionLine;
      case AreaTools::EAST:
        return p.X() >= longitudeDivisionLine;
      case AreaTools::WEST:
        return p.X() < longitudeDivisionLine;
      case AreaTools::NORTHEAST:
        return p.Y() >= latitudeDivisionLine && p.X() >= longitudeDivisionLine;
      case AreaTools::SOUTHEAST:
        return p.Y() < latitudeDivisionLine && p.X() >= longitudeDivisionLine;
      case AreaTools::NORTHWEST:
        return p.Y() >= latitudeDivisionLine && p.X() < longitudeDivisionLine;
      case AreaTools::SOUTHWEST:
        return p.Y() < latitudeDivisionLine && p.X() < longitudeDivisionLine;
      default:
        return false;
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace

NFmiIndexMask MaskDirection(const NFmiGrid& theGrid,
                            const WeatherArea& theArea,
                            const AreaTools::direction_id& theDirectionId)
{
  NFmiIndexMask mask;

  const NFmiSvgPath& svgPath = theArea.path();

  if (svgPath.empty())
    return mask;

  double theXmin = NAN;
  double theYmin = NAN;
  double theXmax = NAN;
  double theYmax = NAN;

  NFmiSvgTools::BoundingBox(svgPath, theXmin, theYmin, theXmax, theYmax);

  double latitudeDivisionLine =
      (theArea.latitudeDivisionLineSet() ? theArea.getLatitudeDivisionLine()
                                         : theYmin + ((theYmax - theYmin) / 2.0));

  double longitudeDivisionLine =
      (theArea.longitudeDivisionLineSet() ? theArea.getLongitudeDivisionLine()
                                          : theXmin + ((theXmax - theXmin) / 2.0));

  // Looping through the entire grid would be slow for high resolution data. Hence we
  // first establish the grid points inside or close to the area, and then make the
  // exact tests only for them. The margin of two grid cells guarantees no point
  // inside the area is lost.

  const double dx = theGrid.Area()->WorldXYWidth() / (theGrid.XNumber() - 1);
  const double dy = theGrid.Area()->WorldXYHeight() / (theGrid.YNumber() - 1);
  const double margin_km = 2 * FmiMax(dx, dy) / 1000;

  const NFmiIndexMask candidates = NFmiIndexMaskTools::MaskExpand(theGrid, svgPath, margin_km);

  for (const unsigned long idx : candidates)
  {
    const NFmiPoint p = theGrid.LatLon(idx);

    if (NFmiSvgTools::IsInside(svgPath, p) &&
        point_in_direction(p, theDirectionId, latitudeDivisionLine, longitudeDivisionLine))
      mask.insert(idx);
  }

  return mask;
}

void PrintLatLon(const AnalysisSources& theSources,
                 const WeatherParameter& theParameter,
                 const NFmiIndexMask& theIndexMask)
{
  std::string parameterName;
  std::string dataName;

  ParameterAnalyzer::getParameterStrings(theParameter, parameterName, dataName);
  const string default_forecast = Settings::optional_string("textgen::default_forecast", "");
  const string datavar = dataName + '_' + data_type_name(Forecast);
  const string dataname = Settings::optional_string(datavar, default_forecast);

  std::shared_ptr<WeatherSource> wsource = theSources.getWeatherSource();
  std::shared_ptr<NFmiQueryData> qd = wsource->data(dataname);
  NFmiFastQueryInfo theQI = NFmiFastQueryInfo(qd.get());

  for (unsigned long it : theIndexMask)
  {
    cout << theQI.LatLon(it);
  }
}
}  // namespace TextGen
