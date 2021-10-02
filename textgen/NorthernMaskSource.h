// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::NorthernMaskSource
 */
// ======================================================================

#pragma once

#include <calculator/MaskSource.h>

namespace TextGen
{
class NorthernMaskSource : public MaskSource
{
 public:
  typedef MaskSource::mask_type mask_type;
  typedef MaskSource::masks_type masks_type;

  NorthernMaskSource(const WeatherArea& theArea);

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  NorthernMaskSource();

  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class NorthernMaskSource

}  // namespace TextGen

// ======================================================================
