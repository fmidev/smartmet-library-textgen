// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::CloudinessStoryTools
 */
// ======================================================================

#ifndef TEXTGEN_CLOUDINESSSTORYTOOLS_H
#define TEXTGEN_CLOUDINESSSTORYTOOLS_H

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
		Cloudy,					//!< pilvist�
		PartlyCloudy,			//!< puolipilvist�
		Clear,					//!< selke��
		MostlyCloudy,			//!< enimm�kseen pilvist�
		MostlyPartlyCloudy,		//!< enimm�kseen puolipilvist�
		MostlyClear,			//!< enimm�kseen selke��
		CloudyOrPartlyCloudy,	//!< pilvist� tai puolipilvist�
		ClearOrPartlyCloudy,	//!< selke�� tai puolipilvist�
		DecreasingCloudiness,	//!< selkenev��
		IncreasingCloudiness,	//!< pilvistyv��
		VariableCloudiness		//!< vaihtelevaa pilvisyytt�
	  };


  } // namespace CloudinessStoryTools
} // namespace TextGen

#endif // TEXTGEN_CLOUDINESSSTORYTOOLS_H

// ======================================================================
