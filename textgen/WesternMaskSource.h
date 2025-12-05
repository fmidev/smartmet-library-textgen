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
  using mask_type = MaskSource::mask_type;
  using masks_type = MaskSource::masks_type;

  WesternMaskSource() = delete;
  WesternMaskSource(const WeatherArea& theArea);

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  class Pimple;
  std::shared_ptr<Pimple> itsPimple;

};  // class WesternMaskSource

}  // namespace TextGen

// ======================================================================
