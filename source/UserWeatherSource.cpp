// ======================================================================
/*!
 * \file
 * \brief Implementation of class UserWeatherSource
 */
// ======================================================================

#include "UserWeatherSource.h"
#include "IdGenerator.h"
#include "NFmiQueryData.h"
#include <cassert>
#include <map>
#include <stdexcept>

using namespace std;

namespace WeatherAnalysis
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding details for UserWeatherSource
   */
  // ----------------------------------------------------------------------

  class UserWeatherSource::Pimple
  {
  public:
	typedef map<string,boost::shared_ptr<NFmiQueryData> > container_type;
	container_type itsData;

	typedef map<string,long> id_container_type;
	id_container_type itsIdData;

  };

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * The constructor makes sure the internal shared data is initialized.
   */
  // ----------------------------------------------------------------------

  UserWeatherSource::UserWeatherSource()
	: itsPimple(new Pimple)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Get the desired data
   *
   * \param theName The name of the data
   * \return Shared pointer to the desired data
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<NFmiQueryData> UserWeatherSource::data(const std::string & theName) const
  {
	// See if we have a stored result
	typedef Pimple::container_type::const_iterator const_iterator;
	const_iterator it = itsPimple->itsData.find(theName);
	if(it == itsPimple->itsData.end())
	  throw runtime_error("No data named "+theName+" stored in UserWeatherSource");

	return it->second;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Get id of desired data
   *
   * \param theName The name of the data
   * \return The ID
   */
  // ----------------------------------------------------------------------

  long UserWeatherSource::id(const std::string & theName) const
  {
	// See if we have a stored result
	typedef Pimple::id_container_type::const_iterator const_iterator;
	const_iterator it = itsPimple->itsIdData.find(theName);
	if(it == itsPimple->itsIdData.end())
	  throw runtime_error("No data named "+theName+" stored in UserWeatherSource");

	return it->second;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Add new named data
   *
   * \param theName The name for the data
   * \param theData The data itself
   */
  // ----------------------------------------------------------------------

  void UserWeatherSource::insert(const std::string & theName,
								 boost::shared_ptr<NFmiQueryData> theData) const
  {
	// The constructor should guarantee valid pimple
	assert(itsPimple.get() != 0);

	{
	  typedef Pimple::container_type::value_type value_type;
	  itsPimple->itsData.insert(value_type(theName,theData));
	}

	{
	  long id = IdGenerator::generate();
	  typedef Pimple::id_container_type::value_type value_type;
	  itsPimple->itsIdData.insert(value_type(theName,id));
	}

  }


} // namespace WeatherAnalysis

// ======================================================================
