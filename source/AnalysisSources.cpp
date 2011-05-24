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
#include "MaskSource.h"
#include "WeatherSource.h"
#include <boost/shared_ptr.hpp>

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
   * \brief Return the coast mask source
   *
   * \return The coast mask source
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<MaskSource> AnalysisSources::getCoastMaskSource() const
  {
	return itsCoastMaskSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the inland mask source
   *
   * \return The inland mask source
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<MaskSource> AnalysisSources::getInlandMaskSource() const
  {
	return itsInlandMaskSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the southern mask source
   *
   * \return The inland mask source
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<MaskSource> AnalysisSources::getSouthernMaskSource() const
  {
	return itsSouthernMaskSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the northern mask source
   *
   * \return The inland mask source
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<MaskSource> AnalysisSources::getNorthernMaskSource() const
  {
	return itsNorthernMaskSource;
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

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the coast mask source
   *
   * \param theSource The coast mask source
   */
  // ----------------------------------------------------------------------

  void AnalysisSources::setCoastMaskSource(const boost::shared_ptr<MaskSource> & theSource)
  {
	itsCoastMaskSource = theSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the inland mask source
   *
   * \param theSource The inland mask source
   */
  // ----------------------------------------------------------------------

  void AnalysisSources::setInlandMaskSource(const boost::shared_ptr<MaskSource> & theSource)
  {
	itsInlandMaskSource = theSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the southern mask source
   *
   * \param theSource The inland mask source
   */
  // ----------------------------------------------------------------------

  void AnalysisSources::setSouthernMaskSource(const boost::shared_ptr<MaskSource> & theSource)
  {
	itsSouthernMaskSource = theSource;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the northern mask source
   *
   * \param theSource The inland mask source
   */
  // ----------------------------------------------------------------------

  void AnalysisSources::setNorthernMaskSource(const boost::shared_ptr<MaskSource> & theSource)
  {
	itsNorthernMaskSource = theSource;
  }

} // namespace WeatherAnalysis

// ======================================================================

