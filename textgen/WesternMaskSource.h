// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::WesternMaskSource
 */
// ======================================================================

#pragma once

#include <calculator/MaskSource.h>

namespace TextGen
{
class WesternMaskSource : public MaskSource
{
 public:
  typedef MaskSource::mask_type mask_type;
  typedef MaskSource::masks_type masks_type;

  WesternMaskSource(const WeatherArea& theArea);

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  WesternMaskSource();

  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class WesternMaskSource

}  // namespace TextGen

// ======================================================================
