// ======================================================================
/*!
 * \file
 * \brief Implementation of class PrecipitationStory
 */
// ======================================================================

#include "PrecipitationStory.h"
#include "DefaultAcceptor.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "NullPeriodGenerator.h"
#include "NumberRange.h"
#include "Paragraph.h"
#include "RangeAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "WeatherFunction.h"
#include "WeatherResult.h"

#include "NFmiStringTools.h"

#include "boost/lexical_cast.hpp"
#include <vector> 

using namespace WeatherAnalysis;
using namespace std;
using namespace boost;
using namespace TextGen;

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Parse rain amount class specification string
   *
   * The input is of the form "A...B,C...D,D...E"
   *
   * Throws if an error is detected in the range specifications.
   *
   * \param theVariable The variable containing the classes
   * \return List of range limits
   */
  // ----------------------------------------------------------------------
   
  list<pair<int,int> > parse_classes(const std::string & theVariable)
  {
	const string value = Settings::require(theVariable);

	if(value.empty())
	  throw TextGenError(theVariable + " value must not be empty");

	list<pair<int,int> > output;

	list<string> clist = NFmiStringTools::SplitWords(value);
	for(list<string>::const_iterator it=clist.begin(); it!=clist.end(); ++it)
	  {
		list<string> rlist = NFmiStringTools::SplitWords(*it,"...");
		if(rlist.size() != 2)
		  throw TextGenError(*it+" is not of form A...B in variable "+theVariable);
		int lolimit = lexical_cast<int>(rlist.front());
		int hilimit = lexical_cast<int>(rlist.back());
		if(hilimit<=lolimit)
		  throw TextGenError(*it+" has upper limit <= lower limit in variable "+theVariable);

		output.push_back(make_pair(lolimit,hilimit));
	  }

	return output;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Extract the maximum number from a list of pairs of integers
   *
   * \param theList The list of pairs of integers
   * \return The maximum number
   */
  // ----------------------------------------------------------------------

  int rainlimit(const list<pair<int,int> > & theList)
  {
	if(theList.empty())
	  throw TextGenError("Internal error, trying to extract maximum rain from empty list");

	int ret = theList.front().first;
	for(list<pair<int,int> >::const_iterator it=theList.begin();
		it!=theList.end();
		++it)
	  {
		ret = std::max(ret,it->first);
		ret = std::max(ret,it->second);
	  }
	return ret;
  }

} // namespace anonymous




namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------
  
  PrecipitationStory::~PrecipitationStory()
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theForecastTime The forecast time
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed
   * \param thePeriod The time interval to be analyzed
   * \param theVariable The associated configuration variable
   */
  // ----------------------------------------------------------------------
  
  PrecipitationStory::PrecipitationStory(const NFmiTime & theForecastTime,
										 const AnalysisSources & theSources,
										 const WeatherArea & theArea,
										 const WeatherPeriod & thePeriod,
										 const string & theVariable)
	: itsForecastTime(theForecastTime)
	, itsSources(theSources)
	, itsArea(theArea)
	, itsPeriod(thePeriod)
	, itsVar(theVariable)
  {
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Test whether the given story is known to this class
   *
   * \param theName The story name
   * \return True if this class can generate the story
   */
  // ----------------------------------------------------------------------
  
  bool PrecipitationStory::hasStory(const string & theName)
  {
	if(theName == "precipitation_total")
	  return true;
	if(theName == "precipitation_range")
	  return true;
	if(theName == "precipitation_classification")
	  return true;
	return false;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Generate the desired story
   *
   * Throws if the story name is not recognized.
   *
   * \param theName The story name
   * \return Paragraph containing the story
   */
  // ----------------------------------------------------------------------.
  
  Paragraph PrecipitationStory::makeStory(const string & theName) const
  {
	if(theName == "precipitation_total")
	  return total();
	if(theName == "precipitation_range")
	  return range();
	if(theName == "precipitation_classification")
	  return classification();

	throw TextGenError("PrecipitationStory cannot make story "+theName);

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on mean total precipitation
   *
   * Sample story: "Sadesumma 10 millimetriä."
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  Paragraph PrecipitationStory::total() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	RangeAcceptor rainlimits;
	rainlimits.lowerLimit(Settings::optional_double(itsVar+"::minrain",0));
	
	WeatherResult result = forecaster.analyze(itsVar+"::fake::mean",
											  itsSources,
											  Precipitation,
											  Mean,
											  Sum,
											  itsArea,
											  itsPeriod,
											  rainlimits);

	if(result.value() == kFloatMissing)
	  throw TextGenError("Total precipitation not available");

	Number<int> num = FmiRound(result.value());

	sentence << "sadesumma"
			 << num
			 << "millimetriä";
	paragraph << sentence;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on total precipitation range
   *
   * Sample story 1: "Sadesumma yli LIMIT millimetriä."	(when min>=LIMIT)
   * Sample story 2: "Sadesumma 4 millimetriä."			(when min==max)
   * Sample story 3: "Sadesumma 1-5 millimetriä."		(otherwise)
   *
   * where LIMIT is the value of textgen::precipitation_range::maxrain
   *
   * \return The story
   */
  // ----------------------------------------------------------------------

  Paragraph PrecipitationStory::range() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	RangeAcceptor rainlimits;
	rainlimits.lowerLimit(Settings::optional_double(itsVar+"::minrain",0));

	WeatherResult minresult = forecaster.analyze(itsVar+"::fake::minimum",
												 itsSources,
												 Precipitation,
												 Minimum,
												 Sum,
												 itsArea,
												 itsPeriod,
												 rainlimits);

	WeatherResult maxresult = forecaster.analyze(itsVar+"::fake::maximum",
												 itsSources,
												 Precipitation,
												 Maximum,
												 Sum,
												 itsArea,
												 itsPeriod,
												 rainlimits);

	if(minresult.value() == kFloatMissing ||
	   maxresult.value() == kFloatMissing)
	  throw TextGenError("Total precipitation not available");

	Number<int> minrain = FmiRound(minresult.value());
	Number<int> maxrain = FmiRound(maxresult.value());
	NumberRange<Number<int> > rainrange(minrain,maxrain);

	// optionaalinen maksimisade

	const string variable = itsVar + "::maxrain";
	const int rainlimit = Settings::optional_int(variable,-1);

	if(minrain.value() >= rainlimit && rainlimit>0)
	  {
		sentence << "sadesumma"
				 << "yli"
				 << rainlimit
				 << "millimetriä";
	  }
	else
	  {
		sentence << "sadesumma"
				 << rainrange
				 << "millimetriä";
	  }

	paragraph << sentence;
	return paragraph;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on total precipitation classification
   *
   * Sample stories:
   *
   * -# "Sadesumma yli N millimetriä."
   * -# "Sadesumma monin paikoin yli N millimetriä."
   * -# "Sadesumma paikoin yli N millimetriä."
   * -# "Sadesumma 0 millimetriä."
   * -# "Sadesumma N...M millimetriä."
   * -# "Sadesumma N...M millimetriä, paikoin enemmän."
   * -# "Sadesumma N...M millimetriä, monin paikoin enemmän."
   *
   * \return The story
   *
   */
  // ----------------------------------------------------------------------

  Paragraph PrecipitationStory::classification() const
  {
	Paragraph paragraph;
	Sentence sentence;

	GridForecaster forecaster;

	RangeAcceptor rainlimits;
	rainlimits.lowerLimit(Settings::optional_double(itsVar+"::minrain",0));

	// Gather the results

	WeatherResult minresult = forecaster.analyze(itsVar+"::fake::minimum",
												 itsSources,
												 Precipitation,
												 Minimum,
												 Sum,
												 itsArea,
												 itsPeriod,
												 rainlimits);

	WeatherResult meanresult = forecaster.analyze(itsVar+"::fake::mean",
												  itsSources,
												  Precipitation,
												  Mean,
												  Sum,
												  itsArea,
												  itsPeriod,
												  rainlimits);
	
	WeatherResult maxresult = forecaster.analyze(itsVar+"::fake::maximum",
												 itsSources,
												 Precipitation,
												 Maximum,
												 Sum,
												 itsArea,
												 itsPeriod,
												 rainlimits);

	// Check for invalid results
	if(minresult.value() == kFloatMissing ||
	   maxresult.value() == kFloatMissing ||
	   meanresult.value() == kFloatMissing)
	  throw TextGenError("Total precipitation not available");

	// Parse the classes

	const string classvariable = itsVar + "::classes";
	list<pair<int,int> > classes = parse_classes(classvariable);

	// Establish the maximum rain amount in the classes

	const int maxrainlimit = rainlimit(classes);

	// Handle the simple case of no rain

	if(maxresult.value() == 0)
	  {
		sentence << "sadesumma"
				 << 0
				 << "millimetriä";
		paragraph << sentence;
		return paragraph;
	  }
	
	// Handle the case when maximum rain exceeds the biggest
	// rain amount class

	if(maxresult.value() > maxrainlimit)
	  {
		const string variable1 = itsVar + "::maxlimit1";
		const string variable2 = itsVar + "::maxlimit2";

		int phrase = 1;

		if(minresult.value() < maxrainlimit)
		  {
			RangeAcceptor percentagelimits;
			percentagelimits.lowerLimit(maxrainlimit);

			WeatherResult probresult = forecaster.analyze(itsVar+"::fake::percentage",
														  itsSources,
														  Precipitation,
														  Percentage,
														  Sum,
														  itsArea,
														  itsPeriod,
														  rainlimits,
														  DefaultAcceptor(),
														  percentagelimits);
										
			const int limit1 = Settings::optional_int(variable1,-1);
			const int limit2 = Settings::optional_int(variable2,-1);
			
			if(probresult.value() == kFloatMissing)
			  phrase = 1;
			else if(probresult.value() >= limit2)
			  phrase = 3;
			else if(probresult.value() >= limit1)
			  phrase = 2;
			else
			  phrase = 4;
		  }

		if(phrase<4)
		  {
			sentence << "sadesumma";
			if(phrase==2)
			  sentence << "paikoin";
			else if(phrase==3)
			  sentence << "monin paikoin";
			sentence << "yli"
					 << maxrainlimit
					 << "millimetriä";
			paragraph << sentence;
			return paragraph;
		  }

		// FALLTHROUGH FOR PHRASE 4, WE RETURN N...M INSTEAD!

		// here we forge result to be back within the largest
		// available rain class

		if(meanresult.value() > maxrainlimit)
		  meanresult = WeatherResult(maxrainlimit,1);

	  }
		
	// Find the first class with the correct mean rain amount

	list<pair<int,int> >::const_iterator it;
	for(it = classes.begin(); it != classes.end(); ++it)
	  {
		if(meanresult.value() >= it->first &&
		   meanresult.value() <= it->second)
		  break;
	  }

	if(it==classes.end())
	  throw TextGenError(itsVar + " has gaps in the ranges");

	const int lolimit = it->first;
	const int hilimit = it->second;

	sentence << "sadesumma"
			 << NumberRange<Number<int> >(lolimit,hilimit)
			 << "millimetriä";
	
	// Lisää tarvittaessa "paikoin enemmän" tai "monin paikoin enemmän" perään

	if(maxresult.value() > hilimit)
	  {

		const string variable1 = itsVar + "::limit1";
		const string variable2 = itsVar + "::limit2";

		int phrase = 1;

		RangeAcceptor percentagelimits;
		percentagelimits.lowerLimit(hilimit);

		WeatherResult probresult = forecaster.analyze(itsVar+"::fake::percentage",
													  itsSources,
													  Precipitation,
													  Percentage,
													  Sum,
													  itsArea,
													  itsPeriod,
													  rainlimits,
													  DefaultAcceptor(),
													  percentagelimits);

		const int limit1 = Settings::optional_int(variable1,-1);
		const int limit2 = Settings::optional_int(variable2,-1);
			
		if(probresult.value() == kFloatMissing)
		  phrase = 1;
		else if(probresult.value() >= limit2)
		  phrase = 3;
		else if(probresult.value() >= limit1)
		  phrase = 2;
		else
		  phrase = 1;

		if(phrase==2)
		  sentence << Delimiter(",") << "paikoin enemmän";
		else if(phrase==3)
		  sentence << Delimiter(",") << "monin paikoin enemmän";

	  }

	paragraph << sentence;
	return paragraph;

  }

} // namespace TextGen
  
// ======================================================================
  
