// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::AnalysisSources
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::AnalysisSources
 *
 * \brief Contains weather analysis data sources and caches
 *
 */
// ======================================================================

#include "AnalysisSources.h"
#include "MapSource.h"
#include "MaskSource.h"
#include "WeatherSource.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the weather source
   *
   * \return The weather source
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<WeatherSource> AnalysisSources::getWeatherSource() const
  {
	return itsWeatherSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the map source
   *
   * \return The map source
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<MapSource> AnalysisSources::getMapSource() const
  {
	return itsMapSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the plain mask source
   *
   * \return The mask source
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<MaskSource> AnalysisSources::getMaskSource() const
  {
	return itsMaskSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the land mask source
   *
   * \return The land mask source
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<MaskSource> AnalysisSources::getLandMaskSource() const
  {
	return itsLandMaskSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the weather source
   *
   * \param theSource The weather source
   */
  // ----------------------------------------------------------------------

  void AnalysisSources::setWeatherSource(const boost::shared_ptr<WeatherSource> & theSource)
  {
	itsWeatherSource = theSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the map source
   *
   * \param theSource The map source
   */
  // ----------------------------------------------------------------------

  void AnalysisSources::setMapSource(const boost::shared_ptr<MapSource> & theSource)
  {
	itsMapSource = theSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the plain mask source
   *
   * \param theSource The mask source
   */
  // ----------------------------------------------------------------------

  void AnalysisSources::setMaskSource(const boost::shared_ptr<MaskSource> & theSource)
  {
	itsMaskSource = theSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the land mask source
   *
   * \param theSource The land mask source
   */
  // ----------------------------------------------------------------------

  void AnalysisSources::setLandMaskSource(const boost::shared_ptr<MaskSource> & theSource)
  {
	itsLandMaskSource = theSource;
  }

} // namespace WeatherAnalysis

// ======================================================================

