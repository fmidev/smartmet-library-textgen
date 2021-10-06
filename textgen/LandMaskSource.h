// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::LandMaskSource
 */
// ======================================================================

#pragma once

#include <calculator/MaskSource.h>

namespace TextGen
{
class LandMaskSource : public MaskSource
{
 public:
  using mask_type = MaskSource::mask_type;
  using masks_type = MaskSource::masks_type;

  LandMaskSource(const WeatherArea& theLand);

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  LandMaskSource() = delete;

  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class LandMaskSource

}  // namespace TextGen

// ======================================================================
