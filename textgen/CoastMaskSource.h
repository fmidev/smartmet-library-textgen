// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::CoastMaskSource
 */
// ======================================================================

#pragma once

#include <calculator/MaskSource.h>

namespace TextGen
{
class CoastMaskSource : public MaskSource
{
 public:
  using mask_type = MaskSource::mask_type;
  using masks_type = MaskSource::masks_type;

  CoastMaskSource(const WeatherArea& theCoast);

  mask_type mask(const WeatherArea& theArea,
                 const std::string& theData,
                 const WeatherSource& theWeatherSource) const override;

  masks_type masks(const WeatherArea& theArea,
                   const std::string& theData,
                   const WeatherSource& theWeatherSource) const override;

 private:
  CoastMaskSource();

  class Pimple;
  boost::shared_ptr<Pimple> itsPimple;

};  // class CoastMaskSource

}  // namespace TextGen

// ======================================================================
