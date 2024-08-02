// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::InlandMaskSource
 */
// ======================================================================

#pragma once

#include <calculator/MaskSource.h>

namespace TextGen
{
class InlandMaskSource : public MaskSource
{
 public:
  using mask_type = MaskSource::mask_type;
  using masks_type = MaskSource::masks_type;

  InlandMaskSource(const WeatherArea& theCoast);

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  InlandMaskSource() = delete;

  class Pimple;
  std::shared_ptr<Pimple> itsPimple;

};  // class InlandMaskSource

}  // namespace TextGen

// ======================================================================
