// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::NullMaskSource
 */
// ======================================================================

#pragma once

#include <calculator/MaskSource.h>

namespace TextGen
{
class NullMaskSource : public MaskSource
{
 public:
  using mask_type = MaskSource::mask_type;
  using masks_type = MaskSource::masks_type;

  NullMaskSource();

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  std::shared_ptr<NFmiIndexMask> itsData;

};  // class NullMaskSource

}  // namespace TextGen

// ======================================================================
