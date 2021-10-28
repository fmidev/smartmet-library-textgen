// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::EasternMaskSource
 */
// ======================================================================

#pragma once

#include <calculator/MaskSource.h>

namespace TextGen
{
class EasternMaskSource : public MaskSource
{
 public:
  using mask_type = MaskSource::mask_type;
  using masks_type = MaskSource::masks_type;

  EasternMaskSource(const WeatherArea& theArea);

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  EasternMaskSource() = delete;

  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class EasternMaskSource

}  // namespace TextGen

// ======================================================================
