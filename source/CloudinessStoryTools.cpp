// ======================================================================
/*!
 * \file
 * \brief Implementation for namespace TextGen::CloudinessStoryTools
 */
// ======================================================================
/*!
 * \namespace TextGen::CloudinessStoryTools
 *
 * \brief Tools for TextGen::CloudinessStory and similar classes
 *
 * This namespace contains utility functions which are not
 * exclusive for handling cloudiness alone. For example,
 * TextGen::WeatherStory most likely has use for these
 * functions.
 */
// ======================================================================

#include "CloudinessStoryTools.h"
#include "Settings.h"

namespace TextGen
{
  namespace CloudinessStoryTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate cloudiness type
	 *
	 * \param theVar The settings variable controlling the conversion
	 * \param theCloudyPercentage The percentage of cloudy
	 * \param theClearPercentage The percentage of clear
	 * \param theTrend The trend
	 * \return Respective cloudiness type
	 *
	 * \see \ref page_story_cloudiness_overview for the algorithm
	 *
	 */
	// ----------------------------------------------------------------------

	CloudinessType cloudinesstype(const std::string & theVar,
								  double theCloudyPercentage,
								  double theClearPercentage,
								  double theTrend)
	{
	  using namespace Settings;

	  const int single_class_limit = optional_percentage(theVar+"::single_class_limit",90);
	  const int mostly_class_limit = optional_percentage(theVar+"::mostly_class_limit",80);
	  const int no_class_limit = optional_percentage(theVar+"::no_class_limit",20);
	  const int trend_limit = optional_percentage(theVar+"::trend_limit",80);

	  const double partlycloudy = 100 - theCloudyPercentage - theClearPercentage;

	  if(theCloudyPercentage >= single_class_limit)
		return Cloudy;
	  if(theClearPercentage >= single_class_limit)
		return Clear;
	  if(partlycloudy >= single_class_limit)
		return PartlyCloudy;

	  if(theCloudyPercentage >= mostly_class_limit)
		return MostlyCloudy;
	  if(theClearPercentage >= mostly_class_limit)
		return MostlyClear;
	  if(partlycloudy >= mostly_class_limit)
		return MostlyPartlyCloudy;

	  if(theClearPercentage < no_class_limit)
		return CloudyOrPartlyCloudy;
	  if(theCloudyPercentage < no_class_limit)
		return ClearOrPartlyCloudy;

	  if(theTrend >= trend_limit)
		return IncreasingCloudiness;
	  if(-theTrend >= trend_limit)
		return DecreasingCloudiness;

	  return VariableCloudiness;

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Test two cloudiness types for similarity
	 *
	 * \param theType1 The first cloudiness type
	 * \param theType2 The second cloudiness type
	 * \return A pair, if second is true then first is the common type.
	 *         If second is false, value of first is unspecified.
	 *
	 * Note that the first cloudiness type is assumed to precede
	 * the second in time.
	 *
	 * The similarities of cloudiness types are defined at
	 * \ref subsection_cloudiness_overview_similarity
	 */
	// ----------------------------------------------------------------------

	std::pair<CloudinessType,bool> similartype(CloudinessType theType1,
											   CloudinessType theType2)
	{
	  static std::pair<CloudinessType,bool> notsimilar =
		std::make_pair(VariableCloudiness,false);

	  switch(theType1)
		{
		case Cloudy:
		  {
			switch(theType2)
			  {
			  case Cloudy:
				return std::make_pair(Cloudy,true);
			  case MostlyCloudy:
				return std::make_pair(MostlyCloudy,true);
			  case CloudyOrPartlyCloudy:
			  case MostlyPartlyCloudy:
			  case PartlyCloudy:
				return std::make_pair(CloudyOrPartlyCloudy,true);
			  case Clear:
			  case MostlyClear:
			  case ClearOrPartlyCloudy:
			  case DecreasingCloudiness:
			  case IncreasingCloudiness:
			  case VariableCloudiness:
				break;
			  }
		  }
		case IncreasingCloudiness:
		  {
			switch(theType2)
			  {
			  case IncreasingCloudiness:
			  case Cloudy:
			  case MostlyCloudy:
				return std::make_pair(IncreasingCloudiness,true);
			  case VariableCloudiness:
				return std::make_pair(VariableCloudiness,true);
			  case PartlyCloudy:
			  case Clear:
			  case MostlyPartlyCloudy:
			  case MostlyClear:
			  case CloudyOrPartlyCloudy:
			  case ClearOrPartlyCloudy:
			  case DecreasingCloudiness:
				break;
			  }
		  }
		case MostlyCloudy:
		  {
			switch(theType2)
			  {
			  case MostlyCloudy:
			  case Cloudy:
				return std::make_pair(MostlyCloudy,true);
			  case CloudyOrPartlyCloudy:
			  case MostlyPartlyCloudy:
			  case PartlyCloudy:
				return std::make_pair(CloudyOrPartlyCloudy,true);
			  case Clear:
			  case MostlyClear:
			  case ClearOrPartlyCloudy:
			  case DecreasingCloudiness:
			  case IncreasingCloudiness:
			  case VariableCloudiness:
				break;
			  }
		  }
		case CloudyOrPartlyCloudy:
		  {
			switch(theType2)
			  {
			  case Cloudy:
			  case CloudyOrPartlyCloudy:
			  case MostlyCloudy:
			  case MostlyPartlyCloudy:
			  case PartlyCloudy:
			  case ClearOrPartlyCloudy:
				return std::make_pair(CloudyOrPartlyCloudy,true);
			  case VariableCloudiness:
				return std::make_pair(VariableCloudiness,true);
			  case Clear:
			  case MostlyClear:
			  case DecreasingCloudiness:
			  case IncreasingCloudiness:
				break;
			  }
		  }
		case MostlyPartlyCloudy:
		  {
			switch(theType2)
			  {
			  case MostlyPartlyCloudy:
			  case PartlyCloudy:
				return std::make_pair(MostlyPartlyCloudy,true);
			  case Cloudy:
			  case MostlyCloudy:
				return std::make_pair(CloudyOrPartlyCloudy,true);
			  case CloudyOrPartlyCloudy:
			  case VariableCloudiness:
				return std::make_pair(VariableCloudiness,true);
			  case ClearOrPartlyCloudy:
			  case MostlyClear:
			  case Clear:
				return std::make_pair(ClearOrPartlyCloudy,true);
			  case DecreasingCloudiness:
			  case IncreasingCloudiness:
				break;
			  }
		  }
		case VariableCloudiness:
		  {
			switch(theType2)
			  {
			  case VariableCloudiness:
			  case IncreasingCloudiness:
			  case MostlyCloudy:
			  case CloudyOrPartlyCloudy:
			  case MostlyPartlyCloudy:
			  case PartlyCloudy:
			  case ClearOrPartlyCloudy:
			  case MostlyClear:
				return std::make_pair(VariableCloudiness,true);
			  case Cloudy:
			  case Clear:
			  case DecreasingCloudiness:
				break;
			  }
		  }
		case PartlyCloudy:
		  {
			switch(theType2)
			  {
			  case PartlyCloudy:
				return std::make_pair(PartlyCloudy,true);
			  case Cloudy:
			  case MostlyCloudy:
			  case CloudyOrPartlyCloudy:
				return std::make_pair(CloudyOrPartlyCloudy,true);
			  case MostlyPartlyCloudy:
				return std::make_pair(MostlyPartlyCloudy,true);
			  case VariableCloudiness:
				return std::make_pair(VariableCloudiness,true);
			  case ClearOrPartlyCloudy:
			  case MostlyClear:
			  case Clear:
				return std::make_pair(ClearOrPartlyCloudy,true);
			  case DecreasingCloudiness:
			  case IncreasingCloudiness:
				break;
			  }
		  }
		case ClearOrPartlyCloudy:
		  {
			switch(theType2)
			  {
			  case ClearOrPartlyCloudy:
			  case MostlyPartlyCloudy:
			  case PartlyCloudy:
			  case MostlyClear:
			  case Clear:
				return std::make_pair(ClearOrPartlyCloudy,true);
			  case MostlyCloudy:
			  case CloudyOrPartlyCloudy:
			  case VariableCloudiness:
				return std::make_pair(VariableCloudiness,true);
			  case Cloudy:
			  case DecreasingCloudiness:
			  case IncreasingCloudiness:
				break;
			  }
		  }
		case MostlyClear:
		  {
			switch(theType2)
			  {
			  case MostlyClear:
			  case Clear:
				return std::make_pair(MostlyClear,true);
			  case MostlyPartlyCloudy:
			  case PartlyCloudy:
			  case ClearOrPartlyCloudy:
				return std::make_pair(ClearOrPartlyCloudy,true);
			  case Cloudy:
			  case MostlyCloudy:
			  case CloudyOrPartlyCloudy:
			  case DecreasingCloudiness:
			  case IncreasingCloudiness:
			  case VariableCloudiness:
				break;
			  }
		  }
		case DecreasingCloudiness:
		  {
			switch(theType2)
			  {
			  case DecreasingCloudiness:
			  case MostlyClear:
			  case Clear:
				return std::make_pair(DecreasingCloudiness,true);
			  case Cloudy:
			  case PartlyCloudy:
			  case MostlyCloudy:
			  case MostlyPartlyCloudy:
			  case CloudyOrPartlyCloudy:
			  case ClearOrPartlyCloudy:
			  case IncreasingCloudiness:
			  case VariableCloudiness:
				break;
			  }
		  }
		case Clear:
		  {
			switch(theType2)
			  {
			  case Clear:
				return std::make_pair(Clear,true);
			  case MostlyPartlyCloudy:
			  case PartlyCloudy:
			  case CloudyOrPartlyCloudy:
				return std::make_pair(ClearOrPartlyCloudy,true);
			  case MostlyClear:
				return std::make_pair(MostlyClear,true);
			  case Cloudy:
			  case MostlyCloudy:
			  case ClearOrPartlyCloudy:
			  case DecreasingCloudiness:
			  case IncreasingCloudiness:
			  case VariableCloudiness:
				break;
			  }
		  }
		}
	  
	  // no matching pair found - return "not similar" answer
	  return notsimilar;

	}



  } // namespace CloudinessStoryTools
} // namespace TextGen

// ======================================================================
