// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::InlandMaskSource
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::InlandMaskSource
 *
 * \brief Provides mask services to clients (masked to inlandal areas)
 *
 * The InlandMaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas. This class is
 * differentiated from RegularMaskSource by the fact that any mask
 * is restricted to remain close to the inland. The acceptable inland
 * area is specified by giving the name of the inlandal line
 * and the acceptable distance in kilometers from the inlandal line.
 *
 */
// ======================================================================

#include "InlandMaskSource.h"

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
   * \brief Implementation hiding detail for WeatherAnalysis::InlandMaskSource
   */
  // ----------------------------------------------------------------------

  class InlandMaskSource::Pimple
  {
  public:

	Pimple(const WeatherArea & theInland);

	typedef map<WeatherAreaAndID,mask_type> mask_storage;
	typedef map<WeatherAreaAndID,masks_type> masks_storage;

	const WeatherArea itsCoast;

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

  }; // class InlandMaskSource::Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Pimple constructor
   */
  // ----------------------------------------------------------------------

  InlandMaskSource::Pimple::Pimple(const WeatherArea & theInland)
	: itsCoast(theInland)
	, itsMaskStorage()
	, itsMasksStorage()
  {
  }

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

  InlandMaskSource::mask_type
  InlandMaskSource::Pimple::find(const WeatherId & theID,
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

  void InlandMaskSource::Pimple::insert(const WeatherId & theID,
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

  InlandMaskSource::mask_type
  InlandMaskSource::Pimple::create_mask(const WeatherArea & theArea,
									   const std::string & theData,
									   const WeatherSource & theWeatherSource) const
  {
	// Establish the grid which to mask

	shared_ptr<NFmiStreamQueryData> qdata = theWeatherSource.data(theData);
	NFmiQueryData * qd = qdata->QueryData();
	if(!qd->IsGrid())
	  throw WeatherAnalysisError("The data in "+theData+" is not gridded - cannot generate mask for it");

	// First build the area mask

	const NFmiSvgPath svg = theArea.path();
	const float radius = theArea.radius();
	mask_type areamask(new NFmiIndexMask(MaskExpand(qd->GridInfo(),
													svg,
													radius)));
	
	// Then build the coast mask

	const NFmiSvgPath & csvg = itsCoast.path();
	const float cdistance = itsCoast.radius();
	mask_type coastmask(new NFmiIndexMask(MaskDistance(qd->GridInfo(),
													   csvg,
													   cdistance)));

	// Substract the coast from th area
	
	*areamask -= *coastmask;

	return areamask;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  InlandMaskSource::InlandMaskSource(const WeatherArea & theCoast)
	: itsPimple(new Pimple(theCoast))
  {
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

  const InlandMaskSource::mask_type
  InlandMaskSource::mask(const WeatherArea & theArea,
						const std::string & theData,
						const WeatherSource & theWeatherSource) const
  {
	if(theArea.isPoint())
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

  const InlandMaskSource::masks_type
  InlandMaskSource::masks(const WeatherArea & theArea,
						 const std::string & theData,
						 const WeatherSource & theWeatherSource) const
  {
	throw WeatherAnalysisError("InlandMaskSource::masks not implemented");
  }


} // namespace WeatherAnalysis

// ======================================================================