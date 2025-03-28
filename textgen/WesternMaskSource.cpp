// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::WesternMaskSource
 */
// ======================================================================
/*!
 * \class TextGen::WesternMaskSource
 *
 * \brief Provides mask services to clients (masked to inlandal areas)
 *
 * The InlandMaskSource class provides access to masks calculated from
 * named SVG paths which represent geographic areas. This class is
 * differentiated from RegularMaskSource by the fact that any mask
 * is restricted to the northern part of the area.
 *
 */
// ======================================================================

#include "WesternMaskSource.h"

#include "SubMaskExtractor.h"
#include <macgyver/Exception.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherSource.h>

#include <newbase/NFmiFastQueryInfo.h>
#include <newbase/NFmiGrid.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiIndexMaskTools.h>
#include <newbase/NFmiQueryData.h>

#include <map>

using namespace std;

using namespace NFmiIndexMaskTools;

namespace TextGen
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

  WeatherAreaAndID(const WeatherId& theID, const WeatherArea& theArea)
      : itsID(theID), itsArea(theArea)
  {
  }

  bool operator==(const WeatherAreaAndID& theOther) const
  {
    return (itsID == theOther.itsID && itsArea == theOther.itsArea);
  }

  bool operator<(const WeatherAreaAndID& theOther) const
  {
    return (itsID != theOther.itsID ? itsID < theOther.itsID : itsArea < theOther.itsArea);
  }
};

// ----------------------------------------------------------------------
/*!
 * \brief Implementation hiding detail for TextGen::WesternMaskSource
 */
// ----------------------------------------------------------------------

class WesternMaskSource::Pimple
{
 public:
  Pimple(const WeatherArea& theArea);

  const WeatherArea itsArea;

  using mask_storage = map<WeatherAreaAndID, mask_type>;
  using masks_storage = map<WeatherAreaAndID, masks_type>;

  mutable mask_storage itsMaskStorage;
  mutable masks_storage itsMasksStorage;

  mask_type find(const WeatherId& theID, const WeatherArea& theArea) const;

  void insert(const WeatherId& theID, const WeatherArea& theArea, const mask_type& theMask) const;

  mask_type create_mask(const WeatherArea& theArea,
                        const std::string& theData,
                        const WeatherSource& theWeatherSource) const;

};  // class WesternMaskSource::Pimple

// ----------------------------------------------------------------------
/*!
 * \brief Pimple constructor
 */
// ----------------------------------------------------------------------

WesternMaskSource::Pimple::Pimple(const WeatherArea& theArea) : itsArea(theArea) {}

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

WesternMaskSource::mask_type WesternMaskSource::Pimple::find(const WeatherId& theID,
                                                             const WeatherArea& theArea) const
{
  static std::shared_ptr<NFmiIndexMask> dummy;

  mask_storage::const_iterator it;

  for (it = itsMaskStorage.begin(); it != itsMaskStorage.end(); ++it)
  {
    // identicalArea-function compares more than operator ==
    if (it->first.itsArea.identicalArea(theArea))
      return it->second;
  }

  mask_storage::iterator iter;

  WeatherAreaAndID key(theID, theArea);
  iter = itsMaskStorage.find(key);
  if (iter != itsMaskStorage.end())
    itsMaskStorage.erase(iter);

  return dummy;
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

void WesternMaskSource::Pimple::insert(const WeatherId& theID,
                                       const WeatherArea& theArea,
                                       const mask_type& theMask) const
{
  using value_type = mask_storage::value_type;

  WeatherAreaAndID key(theID, theArea);

  itsMaskStorage.insert(value_type(key, theMask));

  if (itsMaskStorage.insert(value_type(key, theMask)).second)
    throw Fmi::Exception(BCP, "Could not cache mask for " + theArea.name());
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

WesternMaskSource::mask_type WesternMaskSource::Pimple::create_mask(
    const WeatherArea& theArea,
    const std::string& theData,
    const WeatherSource& theWeatherSource) const
{
  // Establish the grid which to mask

  std::shared_ptr<NFmiQueryData> qdata = theWeatherSource.data(theData);
  NFmiFastQueryInfo qi = NFmiFastQueryInfo(qdata.get());
  if (!qi.IsGrid())
    throw Fmi::Exception(BCP, "The data in " + theData + " is not gridded - cannot generate mask for it");

  // First build the area mask

  mask_type return_mask(new NFmiIndexMask(MaskDirection(*(qi.Grid()), theArea, AreaTools::WEST)));
  return return_mask;
}

// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

WesternMaskSource::WesternMaskSource(const WeatherArea& theArea) : itsPimple(new Pimple(theArea)) {}
// ----------------------------------------------------------------------
/*!
 * \brief Return the mask for the given area
 *
 * \param theArea The weather area
 * \param theData The data name
 * \param theWeatherSource The source for weather data
 */
// ----------------------------------------------------------------------

WesternMaskSource::mask_type WesternMaskSource::mask(const WeatherArea& theArea,
                                                     const std::string& theData,
                                                     const WeatherSource& theWeatherSource) const
{
  if (theArea.isPoint())
    throw Fmi::Exception(BCP, "Trying to generate mask for point");

  // Establish the ID for the data

  WeatherId id = theWeatherSource.id(theData);

  // Try to find cached mask first

  mask_type areamask = itsPimple->find(id, theArea);

  if (areamask.get() != nullptr)
    return areamask;

  // Calculate new mask and cache it

  areamask = itsPimple->create_mask(theArea, theData, theWeatherSource);
  itsPimple->insert(id, theArea, areamask);

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

WesternMaskSource::masks_type WesternMaskSource::masks(
    const WeatherArea& /*theArea*/,
    const std::string& /*theData*/,
    const WeatherSource& /*theWeatherSource*/) const
{
  throw Fmi::Exception(BCP, "WesternMaskSource::masks not implemented");
}

}  // namespace TextGen

// ======================================================================
