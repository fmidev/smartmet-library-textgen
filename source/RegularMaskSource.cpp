// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::RegularMaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::RegularMaskSource
 *
 * \brief Provides mask services to clients
 *
 * The RegularMaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas.
 *
 * The constructor takes as an optional argument the distance in kilometers
 * by which each area may be expanded for a grid point to be considered
 * to be representative for the area. Normally we might use a distance
 * half the forecast grid spacing.
 */
// ======================================================================

#include "RegularMaskSource.h"

#include "MapSource.h"
#include "WeatherAnalysisError.h"
#include "WeatherArea.h"
#include "WeatherSource.h"

#include "NFmiGrid.h"
#include "NFmiQueryData.h"
#include "NFmiIndexMask.h"
#include "NFmiIndexMaskSource.h"
#include "NFmiIndexMaskTools.h"

#include <map>

using namespace std;
using namespace boost;
using namespace NFmiIndexMaskTools;


namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Utility structure to each cache management
   */
  // ----------------------------------------------------------------------

  struct WeatherAreaAndID
  {
	WeatherId itsID;
	WeatherArea itsArea;

	WeatherAreaAndID(const WeatherId & theID,
					 const WeatherArea & theArea)
	  : itsID(theID)
	  , itsArea(theArea)
	{ }

	bool operator==(const WeatherAreaAndID & theOther) const
	{
	  return (itsID == theOther.itsID &&
			  itsArea == theOther.itsArea);
	}

	bool operator<(const WeatherAreaAndID & theOther) const
	{
	  return (itsID != theOther.itsID ?
			  itsID < theOther.itsID :
			  itsArea < theOther.itsArea);
	}

  };

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

	typedef map<WeatherAreaAndID,mask_type> mask_storage;
	typedef map<WeatherAreaAndID,masks_type> masks_storage;

	mask_storage itsMaskStorage;
	masks_storage itsMasksStorage;

	mask_type find(const WeatherId & theID,
				   const WeatherArea & theArea) const;

	void insert(const WeatherId & theID,
				const WeatherArea & theArea,
				const mask_type & theMask);

	mask_type create_mask(const WeatherArea & theArea,
						  const std::string & theData,
						  const WeatherSource & theWeatherSource) const;

  }; // class RegularMaskSource::Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Find mask from cache
   *
   * Returns a 0-shared pointer if mask is not found
   *
   * \param theID The weather ID
   * \param theArea The weather area
   * \return shared_ptr to mask or 0
   */
  // ----------------------------------------------------------------------

  RegularMaskSource::mask_type
  RegularMaskSource::Pimple::find(const WeatherId & theID,
								  const WeatherArea & theArea) const
  {
	static shared_ptr<NFmiIndexMask> dummy;

	WeatherAreaAndID key(theID,theArea);

	mask_storage::const_iterator it = itsMaskStorage.find(key);
	if(it == itsMaskStorage.end())
	  return dummy;

	return it->second;

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Insert a new mask into the cache
   *
   * \param theID The data ID
   * \param theArea The weather area
   * \param theMask The mask itself
   */
  // ----------------------------------------------------------------------

  void RegularMaskSource::Pimple::insert(const WeatherId & theID,
										 const WeatherArea & theArea,
										 const mask_type & theMask)
  {
	typedef mask_storage::value_type value_type;

	WeatherAreaAndID key(theID,theArea);

	itsMaskStorage.insert(value_type(key,theMask));

	if(itsMaskStorage.insert(value_type(key,theMask)).second)
	  throw WeatherAnalysisError("Could not cache mask for "+theArea.name());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Create a new weather area
   *
   * \param theArea The area
   * \param theData The data name
   * \param theWeatherSource The weather source
   * \return The mask
   */
  // ----------------------------------------------------------------------

  RegularMaskSource::mask_type
  RegularMaskSource::Pimple::create_mask(const WeatherArea & theArea,
										 const std::string & theData,
										 const WeatherSource & theWeatherSource) const
  {
	const NFmiSvgPath & svg = itsMapSource->getMap(theArea.name());

	shared_ptr<NFmiQueryData> qd = theWeatherSource.data(theData);
	if(!qd->IsGrid())
	  throw WeatherAnalysisError("The data in "+theData+" is not gridded - cannot generate mask for it");

	NFmiIndexMask * areamask = new NFmiIndexMask(MaskExpand(qd->GridInfo(),
															svg,
															itsExpansionDistance));
	mask_type sharedmask(areamask);
	return sharedmask;
  }

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

  const RegularMaskSource::mask_type
  RegularMaskSource::mask(const WeatherArea & theArea,
						  const std::string & theData,
						  const WeatherSource & theWeatherSource) const
  {
	if(!theArea.isNamed())
	  throw WeatherAnalysisError("Trying to generate mask for point");

	// Establish the ID for the data

	WeatherId id = theWeatherSource.id(theData);

	// Try to find cached mask first

	mask_type areamask = itsPimple->find(id,theArea);

	if(areamask.get()!=0)
	  return areamask;

	// Calculate new mask and cache it

	areamask = itsPimple->create_mask(theArea,theData,theWeatherSource);
	itsPimple->insert(id,theArea,areamask);

	return areamask;
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

  const RegularMaskSource::masks_type
  RegularMaskSource::masks(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const
  {
	throw WeatherAnalysisError("RegularMaskSource::masks not implemented");
  }


} // namespace WeatherAnalysis

// ======================================================================
