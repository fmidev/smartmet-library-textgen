// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::AnalysisSources
 */
// ======================================================================

#ifndef WEATHERANALYSIS_ANALYSISSOURCES_H
#define WEATHERANALYSIS_ANALYSISSOURCES_H

#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  class MapSource;
  class MaskSource;
  class WeatherSource;

  class AnalysisSources
  {
  public:

	boost::shared_ptr<WeatherSource> getWeatherSource() const;
	boost::shared_ptr<MapSource> getMapSource() const;
	boost::shared_ptr<MaskSource> getMaskSource() const;
	boost::shared_ptr<MaskSource> getLandMaskSource() const;

	void setWeatherSource(const boost::shared_ptr<WeatherSource> & theSource);
	void setMapSource(const boost::shared_ptr<MapSource> & theSource);
	void setMaskSource(const boost::shared_ptr<MaskSource> & theSource);
	void setLandMaskSource(const boost::shared_ptr<MaskSource> & theSource);

  private:

	boost::shared_ptr<WeatherSource> itsWeatherSource;
	boost::shared_ptr<MapSource> itsMapSource;
	boost::shared_ptr<MaskSource> itsMaskSource;
	boost::shared_ptr<MaskSource> itsLandMaskSource;

  }; // class AnalysisSources

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_ANALYSISSOURCES_H

// ======================================================================

