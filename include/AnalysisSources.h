// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::AnalysisSources
 */
// ======================================================================

#ifndef WEATHERANALYSIS_ANALYSISSOURCES_H
#define WEATHERANALYSIS_ANALYSISSOURCES_H

#include <boost/shared_ptr.hpp>

namespace WeatherAnalysis
{
  class MaskSource;
  class WeatherSource;

  class AnalysisSources
  {
  public:

	boost::shared_ptr<WeatherSource> getWeatherSource() const;
	boost::shared_ptr<MaskSource> getMaskSource() const;
	boost::shared_ptr<MaskSource> getLandMaskSource() const;
	boost::shared_ptr<MaskSource> getCoastMaskSource() const;
	boost::shared_ptr<MaskSource> getInlandMaskSource() const;

	void setWeatherSource(const boost::shared_ptr<WeatherSource> & theSource);
	void setMaskSource(const boost::shared_ptr<MaskSource> & theSource);
	void setLandMaskSource(const boost::shared_ptr<MaskSource> & theSource);
	void setCoastMaskSource(const boost::shared_ptr<MaskSource> & theSource);
	void setInlandMaskSource(const boost::shared_ptr<MaskSource> & theSource);

  private:

	boost::shared_ptr<WeatherSource> itsWeatherSource;
	boost::shared_ptr<MaskSource> itsMaskSource;
	boost::shared_ptr<MaskSource> itsLandMaskSource;
	boost::shared_ptr<MaskSource> itsCoastMaskSource;
	boost::shared_ptr<MaskSource> itsInlandMaskSource;

  }; // class AnalysisSources

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_ANALYSISSOURCES_H

// ======================================================================

