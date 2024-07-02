// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::SouthernMaskSource
 */
// ======================================================================

#pragma once

#include <calculator/MaskSource.h>

namespace TextGen
{
class SouthernMaskSource : public MaskSource
{
 public:
  using mask_type = MaskSource::mask_type;
  using masks_type = MaskSource::masks_type;

  SouthernMaskSource(const WeatherArea& theArea);

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  SouthernMaskSource() = delete;

  class Pimple;
  std::shared_ptr<Pimple> itsPimple;

};  // class SouthernMaskSource

}  // namespace TextGen

// ======================================================================
