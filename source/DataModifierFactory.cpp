// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace WeatherAnalysis::DataModifierFactory
 */
// ======================================================================

#include "DataModifierFactory.h"
#include "WeatherLimits.h"
#include "WeatherAnalysisError.h"

#include "NFmiDataModifierAvg.h"
#include "NFmiDataModifierMax.h"
#include "NFmiDataModifierMin.h"
#include "NFmiDataModifierSum.h"
#include "NFmiDataModifierProb.h"

#include "boost/lexical_cast.hpp"

#include <string>

using namespace boost;
using namespace std;

namespace WeatherAnalysis
{
  namespace DataModifierFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create a data modifier suitable for the given WeatherFunction
	 *
	 * Throws if there is no suitable data modifier.
	 *
	 * \param theFunction The weather function
	 * \return The data modifier
	 */
	// ----------------------------------------------------------------------

	shared_ptr<NFmiDataModifier> create(WeatherFunction theFunction)
	{
	  switch(theFunction)
		{
		case Mean:
		  return shared_ptr<NFmiDataModifier>(new NFmiDataModifierAvg);
		case Maximum:
		  return shared_ptr<NFmiDataModifier>(new NFmiDataModifierMax);
		case Minimum:
		  return shared_ptr<NFmiDataModifier>(new NFmiDataModifierMin);
		case Sum:
		  return shared_ptr<NFmiDataModifier>(new NFmiDataModifierSum);
		}

	  throw WeatherAnalysisError("DataModifierFactory failed to recognize the given function"+lexical_cast<string>(static_cast<int>(theFunction)));
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create a data modifier suitable for the given WeatherFunction
	 *
	 * Throws if there is no suitable data modifier.
	 *
	 * \param theFunction The weather function
	 * \param theLimits The weather limits
	 * \return The data modifier
	 */
	// ----------------------------------------------------------------------

	shared_ptr<NFmiDataModifier> create(WeatherFunction theFunction,
									const WeatherLimits & theLimits)
	{	
	  switch(theFunction)
		{
		case Mean:
		  return shared_ptr<NFmiDataModifier>(new NFmiDataModifierAvg);
		case Maximum:
		  return shared_ptr<NFmiDataModifier>(new NFmiDataModifierMax);
		case Minimum:
		  return shared_ptr<NFmiDataModifier>(new NFmiDataModifierMin);
		case Sum:
		  return shared_ptr<NFmiDataModifier>(new NFmiDataModifierSum);
		}

	  throw WeatherAnalysisError("DataModifierFactory failed to recognize the given function"+lexical_cast<string>(static_cast<int>(theFunction)));
	}

  } // namespace DataModifierFactory
} // namespace WeatherAnalysis

// ======================================================================
