// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::LandMaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::LandMaskSource
 *
 * \brief Provides mask services to clients (masked to remain on land)
 *
 * The LandMaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas. This class is
 * differentiated from RegularMaskSource by the fact that any mask
 * is restricted from expanding into the sea. The acceptable land
 * area is specified by giving the name of the land area mask
 * \em and the distance in kilometers by which it may be expanded.
 *
 */
// ======================================================================

#include "LandMaskSource.h"

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
   * \brief Implementation hiding detail for WeatherAnalysis::LandMaskSource
   */
  // ----------------------------------------------------------------------

  class LandMaskSource::Pimple
  {
  public:
	shared_ptr<MapSource> itsMapSource;
	double itsExpansionDistance;
	string itsLandMapName;

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
						  const WeatherSource & theWeatherSource,
						  bool useLandMask);

  }; // class LandMaskSource::Pimple

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

  LandMaskSource::mask_type
  LandMaskSource::Pimple::find(const WeatherId & theID,
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

  void LandMaskSource::Pimple::insert(const WeatherId & theID,
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
   * \param useLandMask True if land mask is to be used
   * \return The mask
   */
  // ----------------------------------------------------------------------

  LandMaskSource::mask_type
  LandMaskSource::Pimple::create_mask(const WeatherArea & theArea,
									  const std::string & theData,
									  const WeatherSource & theWeatherSource,
									  bool useLandMask)
  {
	const NFmiSvgPath & svg = itsMapSource->getMap(theArea.name());

	shared_ptr<NFmiQueryData> qd = theWeatherSource.data(theData);
	if(!qd->IsGrid())
	  throw WeatherAnalysisError("The data in "+theData+" is not gridded - cannot generate mask for it");

	NFmiIndexMask * areamask = new NFmiIndexMask(MaskExpand(qd->GridInfo(),
															svg,
															itsExpansionDistance));

	if(useLandMask)
	  {
		WeatherArea landarea(itsLandMapName);
		WeatherId id = theWeatherSource.id(theData);
		mask_type landmask = find(id,landarea);
		if(landmask.get()==0)
		  {
			landmask = create_mask(landarea,theData,theWeatherSource,false);
			insert(id,landarea,landmask);
		  }
		*areamask &= *landmask;
	  }

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
   * \param theLandMapName The name of the land map
   * \param theExpansionDistance The fixed expansion distance for each area
   */
  // ----------------------------------------------------------------------

  LandMaskSource::LandMaskSource(const boost::shared_ptr<MapSource> theMapSource,
								 const std::string & theLandMapName,
								 double theExpansionDistance)
	: itsPimple(new Pimple())
  {
	itsPimple->itsMapSource = theMapSource;
	itsPimple->itsExpansionDistance = theExpansionDistance;
	itsPimple->itsLandMapName = theLandMapName;
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

  const LandMaskSource::mask_type
  LandMaskSource::mask(const WeatherArea & theArea,
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

	areamask = itsPimple->create_mask(theArea,theData,theWeatherSource,true);
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

  const LandMaskSource::masks_type
  LandMaskSource::masks(const WeatherArea & theArea,
						   const std::string & theData,
						   const WeatherSource & theWeatherSource) const
  {
	throw WeatherAnalysisError("LandMaskSource::masks not implemented");
  }


} // namespace WeatherAnalysis

// ======================================================================
