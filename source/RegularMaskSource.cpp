// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::RegularMaskSource
 */
// ======================================================================

#include "RegularMaskSource.h"

#include "MapSource.h"
#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherSource.h"

#include "NFmiIndexMask.h"
#include "NFmiIndexMaskSource.h"

#include <map>

using namespace std;
using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding detail for WeatherAnalysis::RegularMaskSource
   */
  // ----------------------------------------------------------------------

  class RegularMaskSource::Pimple
  {
  public:
	shared_ptr<MapSource> itsMapSource;
	double itsExpansionDistance;

	typedef map<WeatherArea,mask_type> mask_storage;
	typedef map<WeatherArea,masks_type> masks_storage;

	typedef map<string,mask_storage> data_mask_storage;
	typedef map<string,masks_storage> data_masks_storage;

	data_mask_storage itsMaskStorage;
	data_masks_storage itsMasksStorage;

  }; // class RegularMaskSource::Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * Note that default constructor is disabled on purpose.
   *
   * \param theMapSource The sharable map source
   * \param theExpansionDistance The fixed expansion distance for each area
   */
  // ----------------------------------------------------------------------

  RegularMaskSource::RegularMaskSource(const boost::shared_ptr<MapSource> theMapSource,
									   double theExpansionDistance)
	: itsPimple(new Pimple())
  {
	itsPimple->itsMapSource = theMapSource;
	itsPimple->itsExpansionDistance = theExpansionDistance;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Return the mask for the given area
   *
   * \param theArea The weather area
   * \param theData The data name
   * \param theWeatherSource The source for weather data
   */
  // ----------------------------------------------------------------------

  RegularMaskSource::mask_type
  RegularMaskSource::mask(const WeatherArea & theArea,
						  const std::string & theData,
						  const WeatherSource & theWeatherSource) const
  {

	// Try to find cached mask first
	Pimple::data_mask_storage::const_iterator it = itsPimple->itsMaskStorage.find(theData);
	if(it != itsPimple->itsMaskStorage.end())
	  {
		Pimple::mask_storage::const_iterator it2 = it->second.find(theArea);
		if(it2 != it->second.end())
		  return it2->second;
	  }
	else
	  {
		typedef Pimple::data_mask_storage::value_type value_type;
		typedef Pimple::data_mask_storage::const_iterator const_iterator;

		Pimple::mask_storage tmp;
		pair<const_iterator,bool> result = itsPimple->itsMaskStorage.insert(value_type(theData,tmp));
		if(result.second)
		  throw WeatherAnalysisError("Could not allocate room for new mask");
		it = result.first;
	  }

	// Create the mask and store it into container pointed to by it-iterator

	throw WeatherAnalysisError("Unfinished");

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the mask source for the given area
   *
   * \param theArea The weather area
   * \param theData The data name
   * \param theWeatherSource The source for weather data
   */
  // ----------------------------------------------------------------------

  RegularMaskSource::masks_type
  RegularMaskSource::masks(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const
  {
	throw WeatherAnalysisError("RegularMaskSource::masks not implemented");
  }


} // namespace WeatherAnalysis

// ======================================================================
