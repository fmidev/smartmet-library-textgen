// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::CloudinessStoryTools
 */
// ======================================================================

#ifndef TEXTGEN_CLOUDINESSSTORYTOOLS_H
#define TEXTGEN_CLOUDINESSSTORYTOOLS_H

#include <string>
#include <utility>

namespace TextGen
{
  namespace CloudinessStoryTools
  {
	// ----------------------------------------------------------------------
	/*!
	 * \brief Main cloudiness type classification
	 *
	 * Note that some algorithms may use only a subset of these
	 * different cases.
	 */
	// ----------------------------------------------------------------------

	enum CloudinessType
	  {
		Cloudy,					//!< pilvistä
		PartlyCloudy,			//!< puolipilvistä
		Clear,					//!< selkeää
		MostlyCloudy,			//!< enimmäkseen pilvistä
		MostlyPartlyCloudy,		//!< enimmäkseen puolipilvistä
		MostlyClear,			//!< enimmäkseen selkeää
		CloudyOrPartlyCloudy,	//!< pilvistä tai puolipilvistä
		ClearOrPartlyCloudy,	//!< selkeää tai puolipilvistä
		DecreasingCloudiness,	//!< selkenevää
		IncreasingCloudiness,	//!< pilvistyvää
		VariableCloudiness		//!< vaihtelevaa pilvisyyttä
	  };

	CloudinessType cloudinesstype(const std::string & theVar,
								  double theCloudyPercentage,
								  double theClearPercentage,
								  double theTrend);

	std::pair<CloudinessType,bool> similartype(CloudinessType theType1, CloudinessType theType2);


  } // namespace CloudinessStoryTools
} // namespace TextGen

#endif // TEXTGEN_CLOUDINESSSTORYTOOLS_H

// ======================================================================
