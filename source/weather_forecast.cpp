// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::overview
 */
// ======================================================================

#include "WeatherStory.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "NightAndDayPeriodGenerator.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "RangeAcceptor.h"
#include "ValueAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"
#include "NullPeriodGenerator.h"
#include "WeatherPeriodTools.h"
#include "AreaTools.h"
#include "MathTools.h"
#include "SeasonTools.h"
#include "SubMaskExtractor.h"
#include "PlainTextFormatter.h"
#include "Dictionary.h"
#include "DictionaryFactory.h"


#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>

namespace TextGen
{

using namespace Settings;
using namespace WeatherAnalysis;
using namespace AreaTools;
using namespace boost;
using namespace std;


#define INLAND_PHRASE "sisämaassa"
#define COAST_PHRASE "rannikolla"
#define SADE_WORD "sade"
#define ALKAA_WORD "alkaa"
#define SADE_ALKAA_PHRASE "sade alkaa"
#define AAMULLA_WORD "aamulla"
#define AAMUPAIVALLA_WORD "aamupäivällä"
#define ILTAPAIVALLA_WORD "iltapäivällä"
#define KESKIPAIVALLA_WORD "keskipäivällä"
#define ILLALLA_WORD "illalla"
#define ILTAYOSTA_WORD "iltayöstä"
#define KESKIYOLLA_WORD "keskiyöllä"
#define AAMUYOLLA_WORD "aamuyöllä"
#define YOLLA_WORD "yöllä"

#define SAA_WORD "sää"
#define ON_WORD "on"
#define HEIKKOA_WORD "heikkoa"
#define KOHTALAISTA_WORD "kohtalaista"
#define KOVAA_WORD "kovaa"
#define SADETTA_WORD "sadetta"
#define POUTAA_WORD "poutaa"
#define RUNSASTA_SADETTA_PHRASE "runsasta sadetta"
#define SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE "sää vaihtelee puolipilvisestä pilviseen"
#define VERRATTAIN_SELKEAA_PHRASE "verrattain selkeää"
#define VERRATTAIN_PILVISTA_PHRASE "verrattain pilvistä"
#define SELKEAA_WORD "selkeää"
#define PILVISTA_WORD "pilvistä"
#define SADETTA_WORD "sadetta"
#define MELKO_SELKEAA_PHRASE "melko selkeää"
#define PUOLIPILVISTA_WORD "puolipilvistä"
#define SAA_ON_POUTAINEN_PHRASE "poutaa"
#define HEIKKOA_SADETTA_PHRASE "heikkoa sadetta"
#define KOHTALAISTA_SADETTA_PHRASE "kohtalaista sadetta"
#define SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE "sää on enimmäkseen poutaista"
#define PAIKOIN_WORD "paikoin"
#define MONIN_PAIKOIN_WORD "monin paikoin"
#define PILVISTYVAA_WORD "pilvistyvää"
#define SELKENEVAA_WORD "selkenevää"
#define SAA_POUTAANTUU_PHRASE "sää poutaantuu"
#define POUTAANTUUVAA_WORD "poutaantuvaa"
#define MAHDOLLISESTI_UKKOSTA_PHRASE "mahdollisesti ukkosta"
#define PIENI_UKKOSEN_TODENNAKOISYYS_PHRASE "pieni ukkosen todennäköisyys"
#define AJOITTAIN_UKKOSTA_PHRASE "ajoittain ukkosta"

#define HEIKKOJA_SADEKUUROJA_PHRASE "heikkoja sadekuuroja"
#define VOIMAKKAITA_SADEKUUROJA_PHRASE "voimakkaita sadekuuroja"
#define SADEKUUROJA_WORD "sadekuuroja"
#define LUMIKUUROJA_WORD "lumikuuroja"
#define SADEKUUROJA_WORD "sadekuuroja"
#define RANTAKUUROJA_WORD "räntäkuuroja"
#define RANTASADETTA_WORD "räntäsadetta"
#define VESISADETTA_WORD "vesisadetta"
#define LUMISADETTA_WORD "lumisadetta"
#define TIHKUSADETTA_WORD "tihkusadetta"

#define HEIKKOJA_LUMIKUUROJA_PHRASE "heikkoja lumikuuroja"
#define SAKEITA_LUMIKUUROJA_PHRASE "sakeita lumikuuroja"
#define HEIKKOA_LUMISADETTA_PHRASE "heikkoa lumisadetta"
#define SAKEAA_LUMISADETTA_PHRASE "sakeaa lumisadetta"
#define VESI_TAVUVIIVA_WORD "vesi-"
#define RANTA_TAVUVIIVA_WORD "räntä-"
#define LUMI_TAVUVIIVA_WORD "lumi-"
#define TAI_WORD "tai"
#define VESI_KUUROJA_WORD "räntäkuuroja"
#define RANTA_KUUROJA_WORD "vesikuuroja"
#define JOKA_VOI_OLLA_JAATAVAA_PHRASE "joka voi olla jäätävää"
#define JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE "jotka voivat olla jäätäviä"
#define YKSITTAISET_SADEKUUROT_MAHDOLLISIA "yksittäiset sadekuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_RANTAKUUROT_MAHDOLLISIA "yksittäiset räntäkuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_LUMIKUUROT_MAHDOLLISIA "yksittäiset lumikuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA "yksittäiset vesi- tai räntäkuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA "yksittäiset räntä- tai vesikuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA "yksittäiset lumi- tai räntäkuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA "yksittäiset vesi- tai lumikuurot ovat kuitenkin mahdollisia"

#define ALUEEN_POHJOISOSISSA_PHRASE "alueen pohjoisosissa"
#define ALUEEN_ETELAOSISSA_PHRASE "alueen eteläosissa"
#define ALUEEN_ITAOSISSA_PHRASE "alueen itäosissa"
#define ALUEEN_LANSIOSISSA_PHRASE "alueen länsiosissa"
#define ALUEEN_KOILLISOSISSA_PHRASE "alueen koillisosissa"
#define ALUEEN_KAAKKOISOSISSA_PHRASE "alueen kaakkoisosissa"
#define ALUEEN_LOUNAISOSISSA_PHRASE "alueen lounaisosissa"
#define ALUEEN_LUOTEISOSISSA_PHRASE "alueen luoteisosissa"

#define AAMUSTA_ALKAEN_PHRASE "aamusta alkaen"
#define AAMUPAIVASTA_ALKAEN_PHRASE "aamupäivästä alkaen"
#define KESKIPAIVASTA_ALKAEN_PHRASE "keskipäivästä alkaen"
#define ILTAPAIVASTA_ALKAEN_PHRASE "iltapäivästä alkaen"
#define ILLASTA_ALKAEN_PHRASE "illasta alkaen"
#define ILTAYOSTA_ALKAEN_PHRASE "iltayöstä alkaen"
#define KESKIYOSTA_ALKAEN_PHRASE "keskiyöstä alkaen"
#define AAMUYOSTA_ALKAEN_PHRASE "aamuyöstä alkaen"

#define SELKEAA_LIMIT 9.9
#define MELKEIN_SELKEAA_LIMIT 35
#define PUOLIPILVISTA_LIMIT 65.0
#define VERRATTAIN_PILVISTA_LIMIT 85.0

#define TAYSIN_PILVETON_LIMIT 5.0
#define HYVIN_PILVINEN_LIMIT 70.0

#define DRY_WEATHER_LIMIT_WATER 0.04
#define DRY_WEATHER_LIMIT_DRIZZLE 0.02
#define DRY_WEATHER_LIMIT_SLEET 0.02
#define DRY_WEATHER_LIMIT_SNOW 0.02
#define WEAK_PRECIPITATION_LIMIT_WATER 0.4
#define WEAK_PRECIPITATION_LIMIT_SLEET 0.4
#define WEAK_PRECIPITATION_LIMIT_SNOW 0.4
#define HEAVY_PRECIPITATION_LIMIT_WATER 2.0
#define HEAVY_PRECIPITATION_LIMIT_SLEET 1.7
#define HEAVY_PRECIPITATION_LIMIT_SNOW 1.5
#define RAINSTORM_LIMIT 7.0
#define MOSTLY_DRY_WEATHER_LIMIT 10.0
#define IN_SOME_PLACES_LOWER_LIMIT 10.0
#define IN_SOME_PLACES_UPPER_LIMIT 50.0
#define IN_MANY_PLACES_LOWER_LIMIT 50.0
#define IN_MANY_PLACES_UPPER_LIMIT 90.0
#define FREEZING_RAIN_LIMIT 10
#define SHOWER_LIMIT 51
#define PRECIPITATION_FORM_REPORTING_LIMIT 10.0 // at least 10% must be of the type to be mentioned

#define TREND_CHANGE_COEFFICIENT_TRESHOLD 0.65 // pearson coefficient



  enum weather_result_data_id
	{
	  TEMPERATURE_DATA = 0x1,
	  CLOUDINESS_DATA,
	  PRECIPITATION_DATA,
	  PRECIPITATION_EXTENT_DATA,
	  PRECIPITATION_TYPE_DATA,
	  PRECIPITATION_FORM_WATER_DATA,
	  PRECIPITATION_FORM_DRIZZLE_DATA,
	  PRECIPITATION_FORM_SLEET_DATA,
	  PRECIPITATION_FORM_SNOW_DATA,
	  PRECIPITATION_FORM_FREEZING_DATA,
	  PRECIPITATION_NORTHEAST_SHARE_DATA,
	  PRECIPITATION_SOUTHEAST_SHARE_DATA,
	  PRECIPITATION_SOUTHWEST_SHARE_DATA,
	  PRECIPITATION_NORTHWEST_SHARE_DATA,
	  THUNDERPROBABILITY_DATA,
	  UNDEFINED_DATA_ID
	};
  /*
DRY
CONTINUOUS
SHOWERS
  */
  enum precipitation_form_id
	{
	  WATER_FORM = 0x1,
	  DRIZZLE_FORM = 0x2,
	  SLEET_FORM = 0x4,
	  SNOW_FORM = 0x8,
	  FREEZING_FORM = 0x10,
	  WATER_DRIZZLE_FORM = 0x3,
	  WATER_SLEET_FORM = 0x5,
	  WATER_SNOW_FORM = 0x9,
      WATER_FREEZING_FORM = 0x11,
	  WATER_DRIZZLE_SLEET_FORM = 0x7,
	  WATER_DRIZZLE_SNOW_FORM = 0xB,
	  WATER_DRIZZLE_FREEZING_FORM = 0x13,
	  WATER_SLEET_SNOW_FORM = 0xD,
	  WATER_SLEET_FREEZING_FORM = 0x15,
	  WATER_SNOW_FREEZING_FORM = 0x19,
	  DRIZZLE_SLEET_FORM = 0x6,
	  DRIZZLE_SNOW_FORM = 0xA,
	  DRIZZLE_FREEZING_FORM = 0x12,
	  DRIZZLE_SLEET_SNOW_FORM = 0xE,
	  DRIZZLE_SLEET_FREEZING_FORM = 0x16,
	  DRIZZLE_SNOW_FREEZING_FORM = 0x1A,
	  SLEET_SNOW_FORM = 0xC,
	  SLEET_FREEZING_FORM = 0x14,
	  SLEET_SNOW_FREEZING_FORM = 0x1C,
	  SNOW_FREEZING_FORM = 0x18,
	  MISSING_PRECIPITATION_FORM = 0x0
	};

  enum precipitation_intesity_id
	{
	  DRY_WEATHER,
	  WEAK_PRECIPITATION,
	  MODERATE_PRECIPITATION,
	  HEAVY_PRECIPITATION,
	  MOSTLY_DRY_WEATHER,
	  UNDEFINED_INTENSITY_ID
	};

  enum precipitation_category_id
	{
	  WEAK_WATER_PRECIPITATION,
	  MODERATE_WATER_PRECIPITATION,
	  HEAVY_WATER_PRECIPITATION,
	  WEAK_SLEET_PRECIPITATION,
	  MODERATE_SLEET_PRECIPITATION,
	  HEAVY_SLEET_PRECIPITATION,
	  WEAK_SNOW_PRECIPITATION,
	  MODERATE_SNOW_PRECIPITATION,
	  HEAVY_SNOW_PRECIPITATION,
	  DRY_WEATHER_CATEGORY
	};

  enum cloudiness_id
	{
	  SELKEAA,
	  MELKO_SELKEAA,
	  PUOLIPILVISTA,
	  VERRATTAIN_PILVISTA,
	  PILVISTA,
	  PUOLIPILVISTA_JA_PILVISTA,
	  MISSING_CLOUDINESS_ID
	};

  enum part_of_the_day_id
	{
	  AAMU, // 06-09
	  AAMUPAIVA, // 09-11
	  PITKA_AAMUPAIVA, // 09-12
	  KESKIPAIVA, // 11-13
	  ILTAPAIVA, // 13-18
	  PITKA_ILTAPAIVA, // 12-18
	  ILTA, // 18-21
	  ILTAYO, // 22-00
	  KESKIYO, // 00-03
	  AAMUYO, // 03-06
	  PAIVA, // 09-18
	  PITKA_PAIVA, // 06-18
	  YO, // 00-06
	  PITKA_YO, // 22-06
	  YOPUOLI, // 18-06
	  PAIVAPUOLI // 06-18
	};

  enum trend_id
	{
	  PILVISTYY,
	  SELKENEE,
	  POUTAANTUU,
	  SADE_ALKAA,
	  NO_TREND
	};

	struct WeatherResultDataItem
	{
	  WeatherResultDataItem(const WeatherPeriod& period, 
							const WeatherResult& result, 
							const part_of_the_day_id& partOfTheDay) :
		thePeriod(period),
		theResult(result),
		thePartOfTheDay(partOfTheDay)
	  {}

	  WeatherPeriod thePeriod;
	  WeatherResult theResult;
	  part_of_the_day_id thePartOfTheDay;
	};

  std::ostream& operator<<(std::ostream & theOutput,
						   const WeatherResultDataItem& theWeatherResultDataItem)
  {
	const WeatherResult theResult(theWeatherResultDataItem.theResult);

	theOutput << theWeatherResultDataItem.thePeriod.localStartTime()
			  << " ... "
			  << theWeatherResultDataItem.thePeriod.localEndTime()
			  << ": "
			  << '('
			  << theWeatherResultDataItem.theResult.value()
			  << ','
			  << theWeatherResultDataItem.theResult.error()
			  << ')'
			  << endl;

	return theOutput;
  }

  typedef vector<WeatherResultDataItem*> weather_result_data_item_vector;


  struct PrecipitationDataItemData
  {
 	PrecipitationDataItemData(const unsigned int& precipitationForm,
							  const float& precipitationIntensity,
							  const float& precipitationExtent,
							  const float& precipitationFormWater,
							  const float& precipitationFormDrizzle,
							  const float& precipitationFormSleet,
							  const float& precipitationFormSnow,
							  const float& precipitationFormFreezing,
							  const float& precipitationTypeShowers,
							  const trend_id trendId,
							  const float& pearsonCoefficient,
							  const NFmiTime& obervationTime) :
	  thePrecipitationForm(precipitationForm),
	  thePrecipitationIntensity(precipitationIntensity),
	  thePrecipitationExtent(precipitationExtent),
	  thePrecipitationFormWater(precipitationFormWater),
	  thePrecipitationFormDrizzle(precipitationFormDrizzle),
	  thePrecipitationFormSleet(precipitationFormSleet),
	  thePrecipitationFormSnow(precipitationFormSnow),
	  thePrecipitationFormFreezing(precipitationFormFreezing),
	  thePrecipitationTypeShowers(precipitationTypeShowers),
	  theTrendId(trendId),
	  thePearsonCoefficient(pearsonCoefficient),
	  theObervationTime(obervationTime),
	  thePrecipitationPercentageNorthEast(0.0),
	  thePrecipitationPercentageSouthEast(0.0),
	  thePrecipitationPercentageSouthWest(0.0),
	  thePrecipitationPercentageNorthWest(0.0)
	{}

	unsigned int thePrecipitationForm;
	float thePrecipitationIntensity;
	float thePrecipitationExtent;
	float thePrecipitationFormWater;
	float thePrecipitationFormDrizzle;
	float thePrecipitationFormSleet;
	float thePrecipitationFormSnow;
	float thePrecipitationFormFreezing;
	float thePrecipitationTypeShowers;
	trend_id theTrendId;
	float thePearsonCoefficient;
	NFmiTime theObervationTime;
	float thePrecipitationPercentageNorthEast;
	float thePrecipitationPercentageSouthEast;
	float thePrecipitationPercentageSouthWest;
	float thePrecipitationPercentageNorthWest;
	weather_result_data_item_vector thePrecipitationShareNorthEast;
	weather_result_data_item_vector thePrecipitationShareSouthEast;
	weather_result_data_item_vector thePrecipitationShareSouthWest;
	weather_result_data_item_vector thePrecipitationShareNorthWest;

	float precipitationPercentageNorth() 
	{ return thePrecipitationPercentageNorthEast + thePrecipitationPercentageNorthWest; }
	float precipitationPercentageSouth() 
	{ return thePrecipitationPercentageSouthEast + thePrecipitationPercentageSouthWest; }
	float precipitationPercentageEast() 
	{ return thePrecipitationPercentageNorthEast + thePrecipitationPercentageSouthEast; }
	float precipitationPercentageWest() 
	{ return thePrecipitationPercentageNorthWest + thePrecipitationPercentageSouthWest; }

	bool operator==(const PrecipitationDataItemData& theItem)
	{
	  return(thePrecipitationForm == theItem.thePrecipitationForm &&
			 thePrecipitationIntensity == theItem.thePrecipitationIntensity &&
			 thePrecipitationExtent == theItem.thePrecipitationExtent &&
			 thePrecipitationFormWater == theItem.thePrecipitationFormWater &&
			 thePrecipitationFormDrizzle == theItem.thePrecipitationFormDrizzle &&
			 thePrecipitationFormSleet == theItem.thePrecipitationFormSleet &&
			 thePrecipitationFormSnow == theItem.thePrecipitationFormSnow &&
			 thePrecipitationFormFreezing == theItem.thePrecipitationFormFreezing &&
			 thePrecipitationTypeShowers == theItem.thePrecipitationTypeShowers &&
			 theTrendId == theItem.theTrendId &&
			 thePearsonCoefficient == theItem.thePearsonCoefficient &&
			 thePrecipitationShareNorthEast == theItem.thePrecipitationShareNorthEast &&
			 thePrecipitationShareSouthEast == theItem.thePrecipitationShareSouthEast &&
			 thePrecipitationShareSouthWest == theItem.thePrecipitationShareSouthWest &&
			 thePrecipitationShareNorthWest == theItem.thePrecipitationShareNorthWest);
	}
  };


  struct PrecipitationDataItem
  {
	PrecipitationDataItem(const PrecipitationDataItemData* coastalData,
						  const PrecipitationDataItemData* inlandData,
						  const PrecipitationDataItemData* fullData) :
	  theCoastalData(coastalData),
	  theInlandData(inlandData),
	  theFullData(fullData)
	{}

	const PrecipitationDataItemData* theCoastalData;
	const PrecipitationDataItemData* theInlandData;
	const PrecipitationDataItemData* theFullData;
	
	~PrecipitationDataItem()
	{
	  if(theCoastalData)
		delete theCoastalData;
	  if(theInlandData)
		delete theInlandData;
	  if(theFullData)
		delete theFullData;
	}
  };

  /*
  struct PrecipitationDataItem
  {
	PrecipitationDataItem(const unsigned int& precipitationForm,
						  const float& precipitationIntensity,
						  const float& precipitationExtent,
						  const float& precipitationFormWater,
						  const float& precipitationFormDrizzle,
						  const float& precipitationFormSleet,
						  const float& precipitationFormSnow,
						  const float& precipitationFormFreezing,
						  const float& precipitationTypeShowers,
						  const trend_id trendId,
						  const float& pearsonCoefficient) :
	  thePrecipitationForm(precipitationForm),
	  thePrecipitationIntensity(precipitationIntensity),
	  thePrecipitationExtent(precipitationExtent),
	  thePrecipitationFormWater(precipitationFormWater),
	  thePrecipitationFormDrizzle(precipitationFormDrizzle),
	  thePrecipitationFormSleet(precipitationFormSleet),
	  thePrecipitationFormSnow(precipitationFormSnow),
	  thePrecipitationFormFreezing(precipitationFormFreezing),
	  thePrecipitationTypeShowers(precipitationTypeShowers),
	  theTrendId(trendId),
	  thePearsonCoefficient(pearsonCoefficient)
	{}
	
	unsigned int thePrecipitationForm;
	float thePrecipitationIntensity;
	float thePrecipitationExtent;
	float thePrecipitationFormWater;
	float thePrecipitationFormDrizzle;
	float thePrecipitationFormSleet;
	float thePrecipitationFormSnow;
	float thePrecipitationFormFreezing;
	float thePrecipitationTypeShowers;
	trend_id theTrendId;
	float thePearsonCoefficient;
	weather_result_data_item_vector thePrecipitationShareNorthEast;
	weather_result_data_item_vector thePrecipitationShareSouthEast;
	weather_result_data_item_vector thePrecipitationShareSouthWest;
	weather_result_data_item_vector thePrecipitationShareNorthWest;
	
	bool operator==(const PrecipitationDataItem& theItem)
	{
	  return(thePrecipitationForm == theItem.thePrecipitationForm &&
			 thePrecipitationIntensity == theItem.thePrecipitationIntensity &&
			 thePrecipitationExtent == theItem.thePrecipitationExtent &&
			 thePrecipitationFormWater == theItem.thePrecipitationFormWater &&
			 thePrecipitationFormDrizzle == theItem.thePrecipitationFormDrizzle &&
			 thePrecipitationFormSleet == theItem.thePrecipitationFormSleet &&
			 thePrecipitationFormSnow == theItem.thePrecipitationFormSnow &&
			 thePrecipitationFormFreezing == theItem.thePrecipitationFormFreezing &&
			 thePrecipitationTypeShowers == theItem.thePrecipitationTypeShowers &&
			 theTrendId == theItem.theTrendId &&
			 thePearsonCoefficient == theItem.thePearsonCoefficient &&
			 thePrecipitationShareNorthEast == theItem.thePrecipitationShareNorthEast &&
			 thePrecipitationShareSouthEast == theItem.thePrecipitationShareSouthEast &&
			 thePrecipitationShareSouthWest == theItem.thePrecipitationShareSouthWest &&
			 thePrecipitationShareNorthWest == theItem.thePrecipitationShareNorthWest);
	}
  };
  */

  struct CloudinessDataItemData
  {
	CloudinessDataItemData(const cloudiness_id& id,
						   const float& min,
						   const float& mean,
						   const float& max,
						   const float& standardDeviation,
						   const trend_id& trendId,
						   const float& pearsonCoefficient) :
	  theId(id),
	  theMin(min),
	  theMean(mean),
	  theMax(max),
	  theStandardDeviation(standardDeviation),
	  theTrendId(trendId),
	  thePearsonCoefficient(pearsonCoefficient)
	{
	}

	cloudiness_id theId;
	float theMin;
	float theMean;
	float theMax;
	float theStandardDeviation;
	trend_id theTrendId;
	float thePearsonCoefficient;
  };

  struct CloudinessDataItem
  {
	CloudinessDataItem(const CloudinessDataItemData* coastalData,
					   const CloudinessDataItemData* inlandData,
					   const CloudinessDataItemData* fullData) :
	  theCoastalData(coastalData),
	  theInlandData(inlandData),
	  theFullData(fullData)
	{}

	const CloudinessDataItemData* theCoastalData;
	const CloudinessDataItemData* theInlandData;
	const CloudinessDataItemData* theFullData;
	
	~CloudinessDataItem()
	{
	  if(theCoastalData)
		delete theCoastalData;
	  if(theInlandData)
		delete theInlandData;
	  if(theFullData)
		delete theFullData;
	}
  };

  struct ThunderDataItem
  {
	ThunderDataItem(const float& minProbability,
					const float& meanProbability,
					const float& maxProbability,
					const float& standardDeviationProbability):
	  theMinProbability(minProbability),
	  theMeanProbability(meanProbability),
	  theMaxProbability(maxProbability),
	  theStandardDeviationProbability(standardDeviationProbability)
	{}

	float theMinProbability;
	float theMeanProbability;
	float theMaxProbability;
	float theStandardDeviationProbability;
  };

  const char* cloudiness_string(const cloudiness_id& theCloudinessId)
  {
	const char* retval = "";

	switch(theCloudinessId)
	  {
	  case SELKEAA:
		retval = SELKEAA_WORD;
	  break;
	  case MELKO_SELKEAA:
		retval = MELKO_SELKEAA_PHRASE;
	  break;
	  case PUOLIPILVISTA:
		retval = PUOLIPILVISTA_WORD;
	  break;
	  case VERRATTAIN_PILVISTA:
		retval = VERRATTAIN_PILVISTA_PHRASE;
	  break;
	  case PILVISTA:
		retval = PILVISTA_WORD;
	  break;
	  case PUOLIPILVISTA_JA_PILVISTA:
		retval = SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
	  break;
	  default:
		retval = "missing cloudiness id";
	  break;
	  }

	return retval;
  }

 const char* trend_string(const trend_id& theTrendId)
  {
	const char* retval = "";

	switch(theTrendId)
	  {
	  case PILVISTYY:
		retval = PILVISTYVAA_WORD;
		break;
	  case SELKENEE:
		retval = SELKENEVAA_WORD;
		break;
	  case POUTAANTUU:
		retval = SAA_POUTAANTUU_PHRASE;
		break;
	  case SADE_ALKAA:
		retval = SADE_ALKAA_PHRASE;
		break;
	  default:
		retval = "no trend";
		break;
	  }

	return retval;
  }

  // overloaded stream operators
  std::ostream& operator<<(std::ostream & theOutput,
						   const CloudinessDataItemData& theCloudinessDataItemData)
  {
	string cloudinessIdStr(cloudiness_string(theCloudinessDataItemData.theId));

	string trendIdStr = trend_string(theCloudinessDataItemData.theTrendId);

	theOutput << "    " << cloudinessIdStr << ": ";
	theOutput << "min=" << theCloudinessDataItemData.theMin << " ";
	theOutput << "mean=" << theCloudinessDataItemData.theMean << " ";
	theOutput << "max=" << theCloudinessDataItemData.theMax << " ";
	theOutput << "std.dev=" << theCloudinessDataItemData.theStandardDeviation << endl;
	theOutput << "    trend: " << trendIdStr << endl;
	theOutput << "    pearson coefficient: " << theCloudinessDataItemData.thePearsonCoefficient << endl;

	return theOutput;
  }

  std::ostream& operator<<(std::ostream & theOutput,
						   const CloudinessDataItem& theCloudinessDataItem)
  {
	if(theCloudinessDataItem.theCoastalData)
	  {
		theOutput << "  Coastal" << endl;
		theOutput << *theCloudinessDataItem.theCoastalData;
	  }
	if(theCloudinessDataItem.theInlandData)
	  {
		theOutput << "  Inland" << endl;
		theOutput << *theCloudinessDataItem.theInlandData;
	  }
	if(theCloudinessDataItem.theFullData)
	  {
		theOutput << "  Full area" << endl;
		theOutput << *theCloudinessDataItem.theFullData;
	  }
	return theOutput;
  }


  std::ostream& operator<<(std::ostream & theOutput,
						   const PrecipitationDataItemData& thePrecipitationDataItemData)
  {
	theOutput << "timestamp = " << thePrecipitationDataItemData.theObervationTime << endl;
	theOutput << "intensity = " << thePrecipitationDataItemData.thePrecipitationIntensity << endl;
	theOutput << "extent = " << thePrecipitationDataItemData.thePrecipitationExtent << endl;
	theOutput << "water-form = " << thePrecipitationDataItemData.thePrecipitationFormWater << endl;
	theOutput << "drizzle-form = " << thePrecipitationDataItemData.thePrecipitationFormDrizzle << endl;
	theOutput << "sleet-form = " << thePrecipitationDataItemData.thePrecipitationFormSleet << endl;
	theOutput << "snow-form = " << thePrecipitationDataItemData.thePrecipitationFormSnow << endl;
	theOutput << "freezing-form = " << thePrecipitationDataItemData.thePrecipitationFormFreezing << endl;
	theOutput << "showers-type = " << thePrecipitationDataItemData.thePrecipitationTypeShowers << endl;


	/*

	unsigned int thePrecipitationForm;
	float thePrecipitationIntensity;
	float thePrecipitationExtent;
	float thePrecipitationFormWater;
	float thePrecipitationFormDrizzle;
	float thePrecipitationFormSleet;
	float thePrecipitationFormSnow;
	float thePrecipitationFormFreezing;
	float thePrecipitationTypeShowers;
	trend_id theTrendId;
	float thePearsonCoefficient;
	weather_result_data_item_vector thePrecipitationShareNorthEast;
	weather_result_data_item_vector thePrecipitationShareSouthEast;
	weather_result_data_item_vector thePrecipitationShareSouthWest;
	weather_result_data_item_vector thePrecipitationShareNorthWest;


	string cloudinessIdStr(cloudiness_string(theCloudinessDataItemData.theId));

	string trendIdStr = trend_string(theCloudinessDataItemData.theTrendId);

	theOutput << "    " << cloudinessIdStr << ": ";
	theOutput << "min=" << theCloudinessDataItemData.theMin << " ";
	theOutput << "mean=" << theCloudinessDataItemData.theMean << " ";
	theOutput << "max=" << theCloudinessDataItemData.theMax << " ";
	theOutput << "std.dev=" << theCloudinessDataItemData.theStandardDeviation << endl;
	theOutput << "    trend: " << trendIdStr << endl;
	theOutput << "    pearson coefficient: " << theCloudinessDataItemData.thePearsonCoefficient << endl;
	*/
	return theOutput;
  }


  typedef vector<PrecipitationDataItemData*> precipitation_data_vector;

  typedef map<int, weather_result_data_item_vector*> weather_forecast_result_container;
  typedef map<int, weather_forecast_result_container*> weather_forecast_data_container;
  typedef map<int, CloudinessDataItem*> cloudiness_data_item_container;
  typedef vector<cloudiness_data_item_container*> cloudiness_data_container;
  typedef map<int, PrecipitationDataItem*> precipitation_data_item_container;
  typedef vector<precipitation_data_item_container*> precipitation_data_container;
  typedef map<int, ThunderDataItem*> thunder_data_item_container;
  typedef vector<thunder_data_item_container*> thunder_data_container;
  typedef vector<NFmiPoint*> location_coordinate_vector;



	struct wf_story_params
	{								 
	  wf_story_params(const string& variable,
					  const WeatherArea& weatherArea,
					  const WeatherPeriod weatherPeriod,
					  const AnalysisSources& analysisSources,
					  MessageLogger& log) :
		theVariable(variable),
		theArea(weatherArea),
		thePeriod(weatherPeriod),
		theSources(analysisSources),
		theLog(log),
		theHourPeriodCount(0),
		theOriginalPeriodCount(0),
		theForecastArea(NO_AREA),
		thePrecipitationyMorning(false),
		thePrecipitationyAfternoon(false)
	  {
	  }

	  const string& theVariable;
	  const WeatherArea& theArea;
	  const WeatherPeriod thePeriod;
	  const AnalysisSources& theSources;
	  MessageLogger& theLog;
	  unsigned int theHourPeriodCount;
	  unsigned int theOriginalPeriodCount;
	  unsigned short theForecastArea;
	  bool thePrecipitationyMorning;
	  bool thePrecipitationyAfternoon;
	  float theDryWeatherLimitWater;
	  float theDryWeatherLimitDrizzle;
	  float theDryWeatherLimitSleet;
	  float theDryWeatherLimitSnow;
	  float theWeakPrecipitationLimitWater;
	  float theWeakPrecipitationLimitSleet;
	  float theWeakPrecipitationLimitSnow;
	  float theHeavyPrecipitationLimitWater;
	  float theHeavyPrecipitationLimitSleet;
	  float theHeavyPrecipitationLimitSnow;
	  float thePrecipitationStormLimit;
	  float theMostlyDryWeatherLimit;
	  float theInSomePlacesLowerLimit;
	  float theInSomePlacesUpperLimit;
	  float theInManyPlacesLowerLimit;
	  float theInManyPlacesUpperLimit;
	  float theFreezingPrecipitationLimit;
	  float theShowerLimit;
	  weather_forecast_data_container theCompleteData;
	  cloudiness_data_container theCloudinessData;
	  precipitation_data_container thePrecipitationData;
	  thunder_data_container theThunderData;
	};

  bool same_content(const Sentence& sentence1, const Sentence& sentence2)
  {
	shared_ptr<TextGen::Dictionary> dict;
	dict.reset(TextGen::DictionaryFactory::create("multimysql"));

	dict->init("fi");
	TextGen::PlainTextFormatter formatter;
	formatter.dictionary(dict);
	std::string string1(sentence1.realize(formatter));
	std::string string2(sentence2.realize(formatter));

	return (string1.compare(string2) == 0);
  }

  // depends on season: in summertime sadetta; in wintertime vesisadetta
  const char* precipitation_phrase(const NFmiTime& theTime, const string& theVariable)
  {
	return (SeasonTools::isWinterHalf(theTime, theVariable) ? VESISADETTA_WORD : SADETTA_WORD);
  }

  weather_forecast_data_container theCompleteData;
  part_of_the_day_id get_part_of_the_day_id(const WeatherPeriod& thePeriod)
  {
	if(thePeriod.localStartTime().GetHour() > 0 && thePeriod.localStartTime().GetHour() < 6)
	  return AAMUYO;
	else if(thePeriod.localStartTime().GetHour() >= 6 && thePeriod.localStartTime().GetHour() <= 12)
	  return AAMUPAIVA;
	else if(thePeriod.localStartTime().GetHour() > 12 && thePeriod.localStartTime().GetHour() <= 18)
	  return ILTAPAIVA;
	else //if(thePeriod.localStartTime().GetHour() > 18)
	  return ILTA;
	

	/*
  enum part_of_the_day_id
	{
	  AAMU, // 06-09
	  AAMUPAIVA, // 09-11
	  PITKA_AAMUPAIVA, // 09-12
	  KESKIPAIVA, // 11-13
	  ILTAPAIVA, // 13-18
	  PITKA_ILTAPAIVA, // 12-18
	  ILTA, // 18-21
	  ILTAYO, // 22-00
	  KESKIYO, // 00-03
	  AAMUYO, // 03-06
	  PAIVA, // 09-18
	  PITKA_PAIVA, // 06-18
	  YO, // 00-06
	  PITKA_YO, // 22-06
	  YOPUOLI, // 18-06
	  PAIVAPUOLI // 06-18
	};

	 */
  }

  void get_part_of_the_day(const part_of_the_day_id& thePartOfTheDayId, int& theStartHour, int& theEndHour)
  {
	switch(thePartOfTheDayId)
	  {
	  case AAMU:
		{
		  theStartHour = 6;
		  theEndHour = 9;
		}
		break;
	  case AAMUPAIVA:
		{
		  theStartHour = 9;
		  theEndHour = 11;
		}
		break;
	  case PITKA_AAMUPAIVA:
		{
		  theStartHour = 9;
		  theEndHour = 12;
		}
		break;
	  case KESKIPAIVA:
		{
		  theStartHour = 11;
		  theEndHour = 13;
		}
		break;
	  case ILTAPAIVA:
		{
		  theStartHour = 13;
		  theEndHour = 18;
		}
		break;
	  case PITKA_ILTAPAIVA:
		{
		  theStartHour = 12;
		  theEndHour = 18;
		}
		break;
	  case ILTA:
		{
		  theStartHour = 18;
		  theEndHour = 22;
		}
		break;
	  case ILTAYO:
		{
		  theStartHour = 22;
		  theEndHour = 0;
		}
		break;
	  case KESKIYO:
		{
		  theStartHour = 0;
		  theEndHour = 3;
		}
		break;
	  case AAMUYO:
		{
		  theStartHour = 3;
		  theEndHour = 6;
		}
		break;
	  case PAIVA:
		{
		  theStartHour = 9;
		  theEndHour = 18;
		}
		break;
		break;
	  case PITKA_PAIVA:
		{
		  theStartHour = 6;
		  theEndHour = 18;
		}
		break;
	  case YO:
		{
		  theStartHour = 0;
		  theEndHour = 6;
		}
		break;
	  case PITKA_YO:
		{
		  theStartHour = 22;
		  theEndHour = 6;
		}
		break;
	  case YOPUOLI:
		{
		  theStartHour = 18;
		  theEndHour = 6;
		}
		break;
	  case PAIVAPUOLI:
		{
		  theStartHour = 6;
		  theEndHour = 18;
		}
		break;
	  }

  }

  bool is_inside(const NFmiTime& theTimeStamp, 
				 const part_of_the_day_id& thePartOfTheDayId)
  {
	int startHour, endHour;
	get_part_of_the_day(thePartOfTheDayId, startHour, endHour);
	NFmiTime startTimeCompare(theTimeStamp);
	NFmiTime endTimeCompare(theTimeStamp);
	startTimeCompare.SetHour(startHour);
	endTimeCompare.SetHour(endHour);

	return(theTimeStamp >= startTimeCompare && 
		   theTimeStamp <= endTimeCompare);
  }

  Sentence get_time_phrase(const WeatherPeriod& theWeatherPeriod, bool theAlkaenPhrase = false)
  {
	Sentence sentence;

	if(is_inside(theWeatherPeriod.localStartTime(), AAMU))
	  sentence << (theAlkaenPhrase ? AAMUSTA_ALKAEN_PHRASE : AAMUYOLLA_WORD);
	else if(is_inside(theWeatherPeriod.localStartTime(), AAMUPAIVA))
	  sentence << (theAlkaenPhrase ? AAMUPAIVASTA_ALKAEN_PHRASE : AAMUPAIVALLA_WORD);
	else if(is_inside(theWeatherPeriod.localStartTime(), KESKIPAIVA))
	  sentence << (theAlkaenPhrase ? KESKIPAIVASTA_ALKAEN_PHRASE : KESKIPAIVALLA_WORD);
	else if(is_inside(theWeatherPeriod.localStartTime(), ILTAPAIVA))
	  sentence << (theAlkaenPhrase ? ILTAPAIVASTA_ALKAEN_PHRASE : ILTAPAIVALLA_WORD);
	else if(is_inside(theWeatherPeriod.localStartTime(), ILTA))
	  sentence << (theAlkaenPhrase ? ILLASTA_ALKAEN_PHRASE : ILLALLA_WORD);
	else if(is_inside(theWeatherPeriod.localStartTime(), ILTAYO))
	  sentence << (theAlkaenPhrase ? ILTAYOSTA_ALKAEN_PHRASE : ILTAYOSTA_WORD);
	else if(is_inside(theWeatherPeriod.localStartTime(), KESKIYO))
	  sentence << (theAlkaenPhrase ? KESKIYOSTA_ALKAEN_PHRASE : KESKIYOLLA_WORD);
	else if(is_inside(theWeatherPeriod.localStartTime(), AAMUYO))
	  sentence << (theAlkaenPhrase ? AAMUYOSTA_ALKAEN_PHRASE : AAMUYOLLA_WORD);

	return sentence;
  }


  bool get_part_of_the_day(const WeatherPeriod& theSourcePeriod, 
						   const part_of_the_day_id& thePartOfTheDayId, 
						   WeatherPeriod& theDestinationPeriod)
  {
	int starthour = -1;
	int endhour = -1;

	switch(thePartOfTheDayId)
	  {
	  case AAMUPAIVA:
		{
		  starthour = 7;
		  endhour = 12;
		break;
		}
	  case ILTAPAIVA:
		  starthour = 13;
		  endhour = 18;
		break;
	  case YO:
		  starthour = 19;
		  endhour = 6;
		break;
	  default:
		break;
	  }

	int new_start_year =  theSourcePeriod.localStartTime().GetYear();
	int new_start_month = theSourcePeriod.localStartTime().GetMonth();
	int new_start_day = theSourcePeriod.localStartTime().GetDay();
	int new_start_hour = -1;
	int new_end_year = theSourcePeriod.localEndTime().GetYear();
	int new_end_month = theSourcePeriod.localEndTime().GetMonth();
	int new_end_day = theSourcePeriod.localEndTime().GetDay();
	int new_end_hour = -1;
	int old_start_hour = theSourcePeriod.localStartTime().GetHour();
	int old_end_hour = theSourcePeriod.localEndTime().GetHour();

	if(old_start_hour <= starthour)
	  new_start_hour = starthour;
	else if(old_start_hour <= endhour)
	  new_start_hour = old_start_hour;

	if(old_end_hour >= endhour)
	  new_end_hour = endhour;
	else if(old_end_hour >= starthour)
	  new_end_hour = old_end_hour;
	  
	if(new_start_hour == -1 || new_end_hour == -1)
	  {
		return false;
	  }
	else
	  {
		NFmiTime start(new_start_year, new_start_month, new_start_day, new_start_hour);
		NFmiTime end(new_end_year, new_end_month, new_end_day, new_end_hour);
		WeatherPeriod wp(start, end);
		theDestinationPeriod = wp;
		return true;
	  }
  }



  void get_sub_time_series(const WeatherPeriod& thePeriod,
						   const weather_result_data_item_vector& theSourceVector,						   
						   weather_result_data_item_vector& theDestinationVector)
  {
	for(unsigned int i = 0; i < theSourceVector.size(); i++)
	  {
		WeatherResultDataItem* item = theSourceVector[i];
		if(item->thePeriod.localStartTime() >= thePeriod.localStartTime() &&
		   item->thePeriod.localEndTime() <= thePeriod.localEndTime())
		  theDestinationVector.push_back(item);		
	  }
  }

  void get_sub_time_series(const part_of_the_day_id& thePartOfTheDay,
						   const weather_result_data_item_vector& theSourceVector,
						   weather_result_data_item_vector& theDestinationVector)
  {
	for(unsigned int i = 0; i < theSourceVector.size(); i++)
	  {
		WeatherResultDataItem* item = theSourceVector[i];
		if(item->thePartOfTheDay == thePartOfTheDay)
		  theDestinationVector.push_back(item);		
	  }
  }

  void get_part_of_the_day_time_series(wf_story_params& theParameters,
									   const WeatherPeriod& theWeatherPeriod,
									   const part_of_the_day_id& thePartOfTheDayId, 
									   const weather_result_data_id& theDataId,
									   weather_result_data_item_vector& theResultVector)
  {
#ifdef LATER
	weather_result_data_item_vector* theCompleteDataVector = theParameters.theHourlyDataContainer[theDataId];

	for(unsigned int i = 0; i < theCompleteDataVector->size(); i++)
	  {
		//		WeatherResult& weatherResult = (*theCompleteDataVector)[i]->theResult;
		WeatherPeriod& weatherPeriod = (*theCompleteDataVector)[i]->thePeriod;
		if(weatherPeriod.localStartTime().EpochTime() >= theWeatherPeriod.localStartTime().EpochTime() &&
		   weatherPeriod.localEndTime().EpochTime() <= theWeatherPeriod.localEndTime().EpochTime() &&
		   weatherPeriod.localStartTime().GetJulianDay() == theWeatherPeriod.localStartTime().GetJulianDay() &&
		   weatherPeriod.localEndTime().GetJulianDay() == theWeatherPeriod.localEndTime().GetJulianDay())
		  {
			int starthour = -1;
			int endhour = -1;
			switch(thePartOfTheDayId)
			  {
			  case AAMU:
				{
				  starthour = 6;
				  endhour = 9;
				}
				break;
			  case AAMUPAIVA:
				{
				  starthour = 9;
				  endhour = 11;
				}
				break;
			  case PITKA_AAMUPAIVA:
				{
				  starthour = 9;
				  endhour = 12;
				}
				break;
			  case KESKIPAIVA:
				{
				  starthour = 11;
				  endhour = 13;
				}
				break;
			  case ILTAPAIVA:
				{
				  starthour = 13;
				  endhour = 18;
				}
				break;
			  case PITKA_ILTAPAIVA:
				{
				  starthour = 12;
				  endhour = 18;
				}
				break;
			  case ILTA:
				{
				  starthour = 18;
				  endhour = 22;
				}
				break;
			  case ILTAYO:
				{
				  starthour = 22;
				  endhour = 0;
				}
				break;
			  case KESKIYO:
				{
				  starthour = 0;
				  endhour = 3;
				}
				break;
			  case AAMUYO:
				{
				  starthour = 3;
				  endhour = 6;
				}
				break;
			  case PAIVA:
				{
				  starthour = 9;
				  endhour = 18;
				}
				break;
				break;
			  case PITKA_PAIVA:
				{
				  starthour = 6;
				  endhour = 18;
				}
				break;
			  case YO:
				{
				  starthour = 0;
				  endhour = 6;
				}
				break;
			  case PITKA_YO:
				{
				  starthour = 22;
				  endhour = 6;
				}
				break;
			  case YOPUOLI:
				{
				  starthour = 18;
				  endhour = 6;
				}
				break;
			  case PAIVAPUOLI:
				{
				  starthour = 6;
				  endhour = 18;
				}
				break;
			  }
			if(starthour != -1 && endhour != -1)
			  {
				if(weatherPeriod.localStartTime().GetHour() >= starthour &&
				   weatherPeriod.localEndTime().GetHour() <= endhour)
				  theResultVector.push_back((*theCompleteDataVector)[i]);
			  }
		  }
	  }
#endif
  }

	const void log_start_time_and_end_time(MessageLogger& theLog, 
										   const std::string& theLogMessage, 
										   const WeatherPeriod& thePeriod)
	{
	  theLog << NFmiStringTools::Convert(theLogMessage)
			 << thePeriod.localStartTime()
			 << " ... "
			 << thePeriod.localEndTime()
			 << endl;
	}
  /*
  const void log_weather_result_data_item(MessageLogger& theLog, 
										  const WeatherResultDataItem& theWeatherResultDataItem)
  {
	theLog << theWeatherResultDataItem.thePeriod.localStartTime()
		   << " ... "
		   << theWeatherResultDataItem.thePeriod.localEndTime()
		   << ": "
		   << theWeatherResultDataItem.theResult
		   << endl;
  }
  */
  const void log_weather_result_time_series(MessageLogger& theLog, 
											const std::string& theLogMessage, 
											const weather_result_data_item_vector& theTimeSeries)
  {
	theLog << NFmiStringTools::Convert(theLogMessage) << endl;

	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		//		const WeatherResultDataItem& theWeatherResultDataItem = *theTimeSeries[i]; 
		//log_weather_result_data_item(theLog, *theTimeSeries[i]);
		theLog << *theTimeSeries[i];
	  } 
  }

  void log_weather_result_data(MessageLogger& theLog, 
							   weather_forecast_result_container& theDataContainer)
  {
	std::string theLogMessage;
	weather_result_data_item_vector* timeSeries = 0;
	for(unsigned int i = TEMPERATURE_DATA; i < UNDEFINED_DATA_ID; i++)
	  {
		timeSeries = theDataContainer[i];
		if(i == TEMPERATURE_DATA)
		  theLogMessage = "*** temperature ****";
		else if(i == PRECIPITATION_DATA)
		  theLogMessage = "*** precipitation ****";
		else if(i == PRECIPITATION_EXTENT_DATA)
		  theLogMessage = "*** precipitation extent ****";
		else if(i == PRECIPITATION_TYPE_DATA)
		  theLogMessage = "*** precipitation type ****";
		else if(i == CLOUDINESS_DATA)
		  theLogMessage = "*** cloudiness ****";
		else if(i == PRECIPITATION_FORM_WATER_DATA)
		  theLogMessage = "*** precipitation form water ****";
		else if(i == PRECIPITATION_FORM_DRIZZLE_DATA)
		  theLogMessage = "*** precipitation form drizzle ****";
		else if(i == PRECIPITATION_FORM_SLEET_DATA)
		  theLogMessage = "*** precipitation form sleet ****";
		else if(i == PRECIPITATION_FORM_SNOW_DATA)
		  theLogMessage = "*** precipitation form snow ****";
		else if(i == PRECIPITATION_FORM_FREEZING_DATA)
		  theLogMessage = "*** precipitation form freezing ****";
		else if(i == THUNDERPROBABILITY_DATA)
		  theLogMessage = "*** thunder probability ****";
		else if(i == PRECIPITATION_NORTHEAST_SHARE_DATA)
		  theLogMessage = "*** precipitation share northeast ****";
		else if(i == PRECIPITATION_SOUTHEAST_SHARE_DATA)
		  theLogMessage = "*** precipitation share southeast ****";
		else if(i == PRECIPITATION_SOUTHWEST_SHARE_DATA)
		  theLogMessage = "*** precipitation share southwest ****";
		else if(i == PRECIPITATION_NORTHWEST_SHARE_DATA)
		  theLogMessage = "*** precipitation share northwest ****";
		else
		  continue;

		log_weather_result_time_series(theLog, theLogMessage, *timeSeries);
	  }
  }

  const void log_weather_result_data(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		theParameters.theLog << "**  INLAND AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		theParameters.theLog << "**  COASTAL AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		theParameters.theLog << "**  FULL AREA DATA   **" << endl;
		log_weather_result_data(theParameters.theLog, *theParameters.theCompleteData[FULL_AREA]);
	  }
  }

const void log_subperiods(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++ )
	  {
		weather_result_data_item_vector resultVector;
		if(generator.isday(i))
		  {
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMU, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUPERIODI",
										   resultVector);
			resultVector.clear();

			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMUPAIVA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUPÄIVÄPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTAPAIVA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPÄIVÄPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPERIODI",
										   resultVector);
			resultVector.clear();

		  }
		else
		  {
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTA, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											ILTAYO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "ILTAYÖPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											YO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "YÖPERIODI",
										   resultVector);
			resultVector.clear();
			get_part_of_the_day_time_series(theParameters,
											generator.period(i),
											AAMUYO, 
											PRECIPITATION_DATA,
											resultVector);
			log_weather_result_time_series(theParameters.theLog, 
										   "AAMUYÖPERIODI",
										   resultVector);
			resultVector.clear();
		  }
	  }
  }

  double get_areal_distribution(const AnalysisSources& theSources,
							  const WeatherParameter& theParameter,
							  const WeatherArea& theArea,
							  const WeatherPeriod& thePeriod,
							  const Acceptor& theAcceptor,
							  WeatherResult& theNortEastShare,
							  WeatherResult& theSouthEastShare,
							  WeatherResult& theSouthWestShare,
							  WeatherResult& theNortWestShare)
  {
	NFmiIndexMask indexMask;
	
  	double cumulativeValue = ExtractMask(theSources,
										 theParameter,
										 theArea,
										 thePeriod,
										 theAcceptor,
										 indexMask);

	NFmiPoint averagePoint = AreaTools::getArealDistribution(theSources, 
															 theParameter, 
															 indexMask,
															 theNortEastShare,
															 theSouthEastShare,
															 theSouthWestShare,
															 theNortWestShare);


	return cumulativeValue;
  }



  void populate_temperature_time_series(const string& theVariable, 
										const AnalysisSources& theSources,
										const WeatherArea& theArea,										
										weather_result_data_item_vector& theResultData)
  {
	GridForecaster theForecaster;
	for(unsigned int i = 0; i < theResultData.size(); i++)
	  {
		theResultData[i]->theResult = theForecaster.analyze(theVariable,
															theSources,
															Temperature,
															Mean,
															Maximum,
															theArea,
															theResultData[i]->thePeriod);
	  }
  }

  void populate_temperature_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 inlandArea,
										 *(*theParameters.theCompleteData[INLAND_AREA])[TEMPERATURE_DATA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 coastalArea,
										 *(*theParameters.theCompleteData[COASTAL_AREA])[TEMPERATURE_DATA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_temperature_time_series(theParameters.theVariable,
										 theParameters.theSources,
										 theParameters.theArea,
										 *(*theParameters.theCompleteData[FULL_AREA])[TEMPERATURE_DATA]);
	  }
  }

  void populate_precipitation_time_series(const string& theVariable, 
										  const AnalysisSources& theSources,
										  const WeatherArea& theArea,	
										  weather_forecast_result_container& theHourlyDataContainer)
  {
	GridForecaster theForecaster;

	weather_result_data_item_vector* precipitationHourly = 
	  theHourlyDataContainer[PRECIPITATION_DATA];
	weather_result_data_item_vector* precipitationExtentHourly = 
	  theHourlyDataContainer[PRECIPITATION_EXTENT_DATA];
	weather_result_data_item_vector* precipitationFormWaterHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_WATER_DATA];
	weather_result_data_item_vector* precipitationFormDrizzleHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	weather_result_data_item_vector* precipitationFormSleetHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_SLEET_DATA];
	weather_result_data_item_vector* precipitationFormSnowHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_SNOW_DATA];
	weather_result_data_item_vector* precipitationFormFreezingHourly = 
	  theHourlyDataContainer[PRECIPITATION_FORM_FREEZING_DATA];
	weather_result_data_item_vector* precipitationTypeHourly = 
	  theHourlyDataContainer[PRECIPITATION_TYPE_DATA];
	weather_result_data_item_vector* precipitationShareNorthEastHourly = 
	  theHourlyDataContainer[PRECIPITATION_NORTHEAST_SHARE_DATA];
	weather_result_data_item_vector* precipitationShareSouthEastHourly = 
	  theHourlyDataContainer[PRECIPITATION_SOUTHEAST_SHARE_DATA];
	weather_result_data_item_vector* precipitationShareSouthWestHourly = 
	  theHourlyDataContainer[PRECIPITATION_SOUTHWEST_SHARE_DATA];
	weather_result_data_item_vector* precipitationShareNorthWestHourly = 
	  theHourlyDataContainer[PRECIPITATION_NORTHWEST_SHARE_DATA];

	RangeAcceptor precipitationlimits;
	precipitationlimits.lowerLimit(0.001);
	RangeAcceptor waterfilter;
	waterfilter.lowerLimit(0);	// 0 = drizzle
	waterfilter.upperLimit(1);	// 1 = water
	//RangeAcceptor percentagelimits;
	//percentagelimits.lowerLimit(maxprecipitationlimit);
	ValueAcceptor drizzlefilter;
	drizzlefilter.value(0);	// 0 = drizzle
	ValueAcceptor sleetfilter;
	sleetfilter.value(2);	// 2 = sleet
	ValueAcceptor snowfilter;
	snowfilter.value(3);	// 3 = snow
	ValueAcceptor freezingfilter;
	freezingfilter.value(4);	// 4 = freezing
	ValueAcceptor showerfilter;
	showerfilter.value(2);	// 1=large scale, 2=showers

	//ut << "  ALKU   " << endl;

	for(unsigned int i = 0; i < precipitationHourly->size(); i++)
	  {
        (*precipitationHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Precipitation,
								Mean,
								Sum,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								precipitationlimits);

        (*precipitationExtentHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Precipitation,
								Percentage,
								Sum,
								theArea,
								(*precipitationExtentHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								precipitationlimits);

		(*precipitationFormWaterHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								waterfilter);

		(*precipitationFormDrizzleHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								drizzlefilter);

		(*precipitationFormSleetHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								sleetfilter);

		(*precipitationFormSnowHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								snowfilter);

		(*precipitationFormFreezingHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationForm,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								freezingfilter);

		(*precipitationTypeHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								PrecipitationType,
								Mean,
								Percentage,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								DefaultAcceptor(),
								showerfilter);

		get_areal_distribution(theSources,
							   Precipitation,
							   theArea,
							   (*precipitationShareNorthEastHourly)[i]->thePeriod,
							   precipitationlimits,
							   (*precipitationShareNorthEastHourly)[i]->theResult,
							   (*precipitationShareSouthEastHourly)[i]->theResult,
							   (*precipitationShareSouthWestHourly)[i]->theResult,
							   (*precipitationShareNorthWestHourly)[i]->theResult);
	  }
  }

  void populate_precipitation_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   inlandArea,
										   *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   coastalArea,
										   *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_precipitation_time_series(theParameters.theVariable,
										   theParameters.theSources,
										   theParameters.theArea,
										   *theParameters.theCompleteData[FULL_AREA]);
	  }
  }

  void populate_thunderprobability_time_series(const string& theVariable, 
											   const AnalysisSources& theSources,
											   const WeatherArea& theArea,										
											   weather_forecast_result_container& theHourlyDataContainer)
  {

	weather_result_data_item_vector& thunderProbabilityHourly = 
	  *(theHourlyDataContainer[THUNDERPROBABILITY_DATA]);
	  
 	GridForecaster theForecaster;

	for(unsigned int i = 0; i < thunderProbabilityHourly.size(); i++)
	  {
		thunderProbabilityHourly[i]->theResult = theForecaster.analyze(theVariable,
																	   theSources,
																	   Thunder,
																	   Maximum,
																	   Maximum,
																	   theArea,
																	   thunderProbabilityHourly[i]->thePeriod);
	  }
  }

 void populate_thunderprobability_time_series(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												inlandArea,
												*theParameters.theCompleteData[INLAND_AREA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												coastalArea,
												*theParameters.theCompleteData[COASTAL_AREA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_thunderprobability_time_series(theParameters.theVariable,
												theParameters.theSources,
												theParameters.theArea,
												*theParameters.theCompleteData[FULL_AREA]);
	  }
  }


  void populate_cloudiness_time_series(const string& theVariable, 
										const AnalysisSources& theSources,
										const WeatherArea& theArea,										
										weather_result_data_item_vector& theResultData)
  {
 	GridForecaster theForecaster;
	for(unsigned int i = 0; i < theResultData.size(); i++)
	  {
		theResultData[i]->theResult = theForecaster.analyze(theVariable,
															theSources,
															Cloudiness,
															Maximum,
															Mean,
															theArea,
															theResultData[i]->thePeriod);
	  }
 }

  void populate_cloudiness_time_series(wf_story_params& theParameters)
  {
	// theParameters.theVariable+"::fake::"+hourstr+"::cloudiness",
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		WeatherArea inlandArea = theParameters.theArea;
		inlandArea.type(WeatherArea::Inland);
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										inlandArea,
										*(*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA]);
	  }

	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		WeatherArea coastalArea = theParameters.theArea;
		coastalArea.type(WeatherArea::Coast);
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										coastalArea,
										*(*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA]);
	  }

	if(theParameters.theForecastArea & FULL_AREA)
	  {
		populate_cloudiness_time_series(theParameters.theVariable,
										theParameters.theSources,
										theParameters.theArea,
										*(*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA]);
	  }
  }

  float get_mean(const weather_result_data_item_vector& theTimeSeries, 
				 const int& theStartIndex = 0, 
				 const int& theEndIndex = 0)
  {
	float precipitation_sum = 0.0;
	int counter = 0;
	unsigned int startIndex = theStartIndex > 0 ? theStartIndex : 0;
	unsigned int endIndex = theEndIndex > 0 ? theEndIndex : theTimeSeries.size();
	
	if(startIndex == endIndex || endIndex < startIndex || endIndex > theTimeSeries.size())
	  return kFloatMissing;

	for(unsigned int i = startIndex; i < endIndex; i++)
	  {
		if(theTimeSeries[i]->theResult.value() == kFloatMissing)
		  continue;
		precipitation_sum += theTimeSeries[i]->theResult.value();
		counter++;
	  }

	if((counter == 0 && theTimeSeries.size() > 0) || theTimeSeries.size() == 0)
	  return kFloatMissing;
	else
	  return precipitation_sum / counter;
  }

  
  float get_standard_deviation(const weather_result_data_item_vector& theTimeSeries)
  {
	float deviation_sum_pow2 = 0.0;
	float mean = get_mean(theTimeSeries);
	int counter = 0;
	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		if(theTimeSeries[i]->theResult.value() == kFloatMissing)
		  continue;
		deviation_sum_pow2 += std::pow(mean - theTimeSeries[i]->theResult.value(), 2);
		counter++;
	  }

	return std::sqrt(deviation_sum_pow2 / counter);
  }

  void get_min_max(const weather_result_data_item_vector& theTimeSeries, float& theMin, float& theMax)
  {
	theMin = theMax = kFloatMissing;

	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  {
		if(theTimeSeries[i]->theResult.value() == kFloatMissing)
		  continue;
		if(i == 0)
		  {
			theMin = theTimeSeries[i]->theResult.value();
			theMax = theTimeSeries[i]->theResult.value();
		  }
		else
		  {
			if(theMin > theTimeSeries[i]->theResult.value())
			  theMin = theTimeSeries[i]->theResult.value();
			else
			if(theMax < theTimeSeries[i]->theResult.value())
			  theMax = theTimeSeries[i]->theResult.value();
		  }
	  }
  }

  double get_pearson_coefficient(const weather_result_data_item_vector& theTimeSeries)
  {
	vector<double> precipitation;

	for(unsigned int i = 0; i < theTimeSeries.size(); i++)
	  precipitation.push_back(theTimeSeries[i]->theResult.value());

	return MathTools::pearson_coefficient(precipitation);
  }

  cloudiness_id get_cloudiness_id(const float& theCloudiness) 
  {
	cloudiness_id id(MISSING_CLOUDINESS_ID);

	if(theCloudiness < 0)
	  return id;

	if(theCloudiness <= SELKEAA_LIMIT)
	  {
		id = SELKEAA;
	  }
	else if(theCloudiness  <= MELKEIN_SELKEAA_LIMIT)
	  {
		id = MELKO_SELKEAA;
	  }
	else if(theCloudiness <= PUOLIPILVISTA_LIMIT)
	  {
		id = PUOLIPILVISTA;
	  }
	else if(theCloudiness <= VERRATTAIN_PILVISTA_LIMIT)
	  {
		id = VERRATTAIN_PILVISTA;
	  }
	else
	  {
		id = PILVISTA;
	  }

	return id;
  }

  cloudiness_id get_cloudiness_id(const float& theMin, 
								  const float& theMean, 
								  const float& theMax, 
								  const float& theStandardDeviation)
  {
	cloudiness_id id(MISSING_CLOUDINESS_ID);

	if(theMean == -1)
	  return id;

	//#define SELKEAA_LIMIT 9.9
	//#define MELKEIN_SELKEAA_LIMIT 35.0
	//#define PUOLIPILVISTA_LIMIT 65.0
	//#define VERRATTAIN_PILVISTA_LIMIT 85.0

	if(theMin >= MELKEIN_SELKEAA_LIMIT && 
	   theMin <= PUOLIPILVISTA_LIMIT && 
	   theMax >= VERRATTAIN_PILVISTA_LIMIT)
	  {
		id = PUOLIPILVISTA_JA_PILVISTA;
	  }
	else if(theMean <= SELKEAA_LIMIT)
	  {
		if(theMean + theStandardDeviation <= SELKEAA_LIMIT)
		  id = SELKEAA;
		else
		  id = MELKO_SELKEAA;
	  }
	else if(theMean  <= MELKEIN_SELKEAA_LIMIT)
	  {
		if(theMean + theStandardDeviation <= MELKEIN_SELKEAA_LIMIT)
		  id = MELKO_SELKEAA;
		else
		  id = PUOLIPILVISTA;
	  }
	else if(theMean <= PUOLIPILVISTA_LIMIT)
	  {
		if(theMean + theStandardDeviation <= PUOLIPILVISTA_LIMIT)
		  id = PUOLIPILVISTA;
		else
		  id = VERRATTAIN_PILVISTA;
	  }
	else if(theMean + theStandardDeviation <= VERRATTAIN_PILVISTA_LIMIT)
	  {
		  id = VERRATTAIN_PILVISTA;
	  }
	else
	  {
		id = PILVISTA;
	  }

	return id;
  }

  CloudinessDataItemData* get_cloudiness_data_item_data(wf_story_params& theParameters, 
														const WeatherPeriod& thePeriod,
														const forecast_area_id& theForeacastAreaId)
  {
	if(!(theParameters.theForecastArea & theForeacastAreaId))
	  return 0;

	CloudinessDataItemData* item = 0;

	cloudiness_id theCloudinessId(MISSING_CLOUDINESS_ID);

	float min = -1.0;
	float mean = -1.0;
	float max = -1.0;   
	float standard_deviation = -1.0;
	float pearson_coefficient = 0.0;
	
	weather_result_data_item_vector theInterestingData;

	trend_id theTrendId(NO_TREND);

	get_sub_time_series(thePeriod,
						*(*theParameters.theCompleteData[theForeacastAreaId])[CLOUDINESS_DATA],
						theInterestingData);

	if(theInterestingData.size() > 0)
	  {
		mean = get_mean(theInterestingData);
		get_min_max(theInterestingData, min, max);
		standard_deviation = get_standard_deviation(theInterestingData);	
		theCloudinessId = get_cloudiness_id(min, mean, max, standard_deviation);
		pearson_coefficient = get_pearson_coefficient(theInterestingData);
  
		if(theInterestingData.size() > 2)
		  {
			cloudiness_id cloudinessAtStart = 
			  get_cloudiness_id(theInterestingData[0]->theResult.value());
			cloudiness_id cloudinessAtEnd = 
			  get_cloudiness_id(theInterestingData[theInterestingData.size()-1]->theResult.value());

			bool dataOk = (cloudinessAtStart != kFloatMissing && cloudinessAtEnd != kFloatMissing);

			if(dataOk && cloudinessAtStart >= VERRATTAIN_PILVISTA && 
			   cloudinessAtEnd == SELKEAA && 
			   pearson_coefficient < -TREND_CHANGE_COEFFICIENT_TRESHOLD)
			  {
				theTrendId = SELKENEE;
			  }
			else if(dataOk && cloudinessAtStart <= MELKO_SELKEAA && 
					cloudinessAtEnd >= VERRATTAIN_PILVISTA &&
					pearson_coefficient >= TREND_CHANGE_COEFFICIENT_TRESHOLD)
			  {
				theTrendId = PILVISTYY;
			  }
		  }
		item = new CloudinessDataItemData(theCloudinessId, 
										  min, 
										  mean, 
										  max, 
										  standard_deviation, 
										  theTrendId, 
										  pearson_coefficient);
	  }

	return item;


  }

 CloudinessDataItem* get_cloudiness_data_item(wf_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	CloudinessDataItemData* dataItemCoastal = 
	  get_cloudiness_data_item_data(theParameters, thePeriod, COASTAL_AREA);
	CloudinessDataItemData* dataItemInland = 
	  get_cloudiness_data_item_data(theParameters, thePeriod, INLAND_AREA);
	CloudinessDataItemData* dataItemFull = 
	  get_cloudiness_data_item_data(theParameters, thePeriod, FULL_AREA);

	CloudinessDataItem* item = new CloudinessDataItem(dataItemCoastal, dataItemInland, dataItemFull);

	return item;
  }

  Sentence cloudiness_sentence(const cloudiness_id& theCloudinessId, const bool& theShortForm = false)
  {
	Sentence sentence;

	std::string cloudiness_phrase;
	switch(theCloudinessId)
	  {
	  case PUOLIPILVISTA_JA_PILVISTA:
		cloudiness_phrase = SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
		break;
	  case SELKEAA:
		cloudiness_phrase = SELKEAA_WORD;
		break;
	  case MELKO_SELKEAA:
		cloudiness_phrase = MELKO_SELKEAA_PHRASE;
		break;
	  case PUOLIPILVISTA:
		cloudiness_phrase = PUOLIPILVISTA_WORD;
		break;
	  case VERRATTAIN_PILVISTA:
		cloudiness_phrase = VERRATTAIN_PILVISTA_PHRASE;
		break;
	  case PILVISTA:
		cloudiness_phrase = PILVISTA_WORD;
		break;
	  default:
		break;
	  }

	if(cloudiness_phrase.length() > 0)
	  {
		if(theCloudinessId == PUOLIPILVISTA_JA_PILVISTA)
		  sentence << cloudiness_phrase;
		else
		  if(theShortForm)
			sentence << cloudiness_phrase;
		  else
			sentence << SAA_WORD << ON_WORD << cloudiness_phrase;
	  }
	
	return sentence;
  }

  Sentence cloudiness_change_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		trend_id theTrendMorning(NO_TREND);
		trend_id theTrendAfternoon(NO_TREND);
		trend_id theTrendNight(NO_TREND);

		const CloudinessDataItemData* morningDataItem = 
		  theResultData->find(AAMUPAIVA) != theResultData->end() ? ((*theResultData)[AAMUPAIVA])->theFullData : 0;
		const CloudinessDataItemData* afternoonDataItem = 
		  theResultData->find(ILTAPAIVA) != theResultData->end() ? ((*theResultData)[ILTAPAIVA])->theFullData : 0;
		const CloudinessDataItemData* nightDataItem = 
		  theResultData->find(YO) != theResultData->end() ? ((*theResultData)[YO])->theFullData : 0;

		// TODO: Think how coastal and inland cloudiness change should be reported
		if(morningDataItem)
		  theTrendMorning = morningDataItem->theTrendId;
		if(afternoonDataItem)
		  theTrendAfternoon = afternoonDataItem->theTrendId;
		if(nightDataItem)
		  theTrendNight = nightDataItem->theTrendId;

		if(theTrendNight == NO_TREND && theTrendMorning == PILVISTYY && theTrendAfternoon == NO_TREND)
		  sentence << AAMUPAIVALLA_WORD << PILVISTYVAA_WORD;
		else if(theTrendNight == NO_TREND && theTrendMorning == NO_TREND && theTrendAfternoon == PILVISTYY)
		  sentence << ILTAPAIVALLA_WORD << PILVISTYVAA_WORD;
		if(theTrendNight == NO_TREND && theTrendMorning == SELKENEE && theTrendAfternoon == NO_TREND)
		  sentence << AAMUPAIVALLA_WORD << SELKENEVAA_WORD;
		else if(theTrendNight == NO_TREND && theTrendMorning == NO_TREND && theTrendAfternoon == SELKENEE)
		  sentence << ILTAPAIVALLA_WORD << SELKENEVAA_WORD;
	  }

	return sentence;
  }

  bool puolipilvisesta_pilviseen(const cloudiness_id& theCloudinessId1,
								 const cloudiness_id& theCloudinessId2)
  {
	if(theCloudinessId1 != MISSING_CLOUDINESS_ID && 
	   theCloudinessId1 == PUOLIPILVISTA_JA_PILVISTA &&
	   theCloudinessId2 == PUOLIPILVISTA_JA_PILVISTA)
	  return true;

	return false;
  }


  Sentence cloudiness_night_sentence(const wf_story_params& theParameters, const CloudinessDataItem& theDataItem)
  {
	Sentence sentence;

	const CloudinessDataItemData* inlandData = theDataItem.theInlandData;
	const CloudinessDataItemData* coastalData = theDataItem.theCoastalData;
	const CloudinessDataItemData* fullData = theDataItem.theFullData;

	cloudiness_id theCloudinessIdInland(inlandData != 0 ? inlandData->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdCoastal(coastalData != 0 ? coastalData->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdFull(fullData != 0 ? fullData->theId : MISSING_CLOUDINESS_ID);

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		if(theCloudinessIdCoastal != MISSING_CLOUDINESS_ID &&
		   theCloudinessIdInland != MISSING_CLOUDINESS_ID &&
		   (coastalData->theMean <= TAYSIN_PILVETON_LIMIT && inlandData->theMean >= HYVIN_PILVINEN_LIMIT ||
			coastalData->theMean >= HYVIN_PILVINEN_LIMIT && inlandData->theMean <= TAYSIN_PILVETON_LIMIT))
		  {
			sentence << COAST_PHRASE << cloudiness_sentence(theCloudinessIdCoastal);
			sentence << Delimiter(",");
			sentence << INLAND_PHRASE << cloudiness_sentence(theCloudinessIdInland);
		  }
		else
		  {
			sentence << cloudiness_sentence(theCloudinessIdFull);
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		sentence << cloudiness_sentence(theCloudinessIdInland);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		sentence << cloudiness_sentence(theCloudinessIdCoastal);
	  }

	return sentence;
  }

  bool has_different_cloudiness(const cloudiness_id& theCloudinessId1, 
								const cloudiness_id& theCloudinessId2, 
								cloudiness_id& theCloudinessIdResult)
  {
	bool retval = false;

	if(abs(theCloudinessId2 - theCloudinessId1) >= 2)
	  {
		theCloudinessIdResult = theCloudinessId1 > theCloudinessId2 ? theCloudinessId1 : theCloudinessId2;
		retval = true;
	  }

	return retval;
  }

  Sentence cloudiness_day_sentence(const wf_story_params& theParameters, 
								   const CloudinessDataItem& theMorningDataItem,
								   const CloudinessDataItem& theAfternoonDataItem)
  {
	Sentence sentence;

	theParameters.theLog << "*Morning Cloudiness" << endl;
	theParameters.theLog << theMorningDataItem;
	theParameters.theLog << "*Afternoon Cloudiness" << endl;
	theParameters.theLog << theAfternoonDataItem;
	
	const CloudinessDataItemData* inlandDataMorning = theMorningDataItem.theInlandData;
	const CloudinessDataItemData* coastalDataMorning = theMorningDataItem.theCoastalData;
	const CloudinessDataItemData* fullDataMorning = theMorningDataItem.theFullData;
	const CloudinessDataItemData* inlandDataAfternoon = theAfternoonDataItem.theInlandData;
	const CloudinessDataItemData* coastalDataAfternoon = theAfternoonDataItem.theCoastalData;
	const CloudinessDataItemData* fullDataAfternoon = theAfternoonDataItem.theFullData;

	cloudiness_id theCloudinessIdFullMorning(fullDataMorning != 0 
											 ? fullDataMorning->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdCoastalMorning(coastalDataMorning != 0 
												? coastalDataMorning->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdInlandMorning(inlandDataMorning != 0 
												? inlandDataMorning->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdFullAfternoon(fullDataAfternoon != 0 
											 ? fullDataAfternoon->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdCoastalAfternoon(coastalDataAfternoon != 0
												? coastalDataAfternoon->theId : MISSING_CLOUDINESS_ID);
	cloudiness_id theCloudinessIdInlandAfternoon(inlandDataAfternoon != 0 
												? inlandDataAfternoon->theId : MISSING_CLOUDINESS_ID);


	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		bool separateCoastInlandMorning = (inlandDataMorning->theMean <= 5.0 && 
										   coastalDataMorning->theMean >= 70.0 ||
										   inlandDataMorning->theMean >= 70.0 && 
										   coastalDataMorning->theMean <= 5.0);
		  //(abs(theCloudinessIdCoastalMorning - theCloudinessIdInlandMorning) >= 2);
		bool separateCoastInlandAfternoon = (inlandDataAfternoon->theMean <= 5.0 && 
											 coastalDataAfternoon->theMean >= 7.0 ||
											 inlandDataAfternoon->theMean >= 70.0 && 
											 coastalDataAfternoon->theMean <= 5.0);
		  //(abs(theCloudinessIdCoastalAfternoon - theCloudinessIdInlandAfternoon) >= 2);
		bool separateMorningAfternoon = (abs(theCloudinessIdFullMorning - theCloudinessIdFullAfternoon) >= 2);

		theParameters.theLog << "separateCoastInlandMorning: " << static_cast<int>(separateCoastInlandMorning) << endl;
		theParameters.theLog << "separateCoastInlandAfternoon: " << static_cast<int>(separateCoastInlandAfternoon) << endl;
		theParameters.theLog << "separateMornigAfternoon: " << static_cast<int>(separateMorningAfternoon) << endl;

		if(!separateCoastInlandMorning && !separateCoastInlandAfternoon && !separateMorningAfternoon)
		  {
			sentence << cloudiness_sentence(theCloudinessIdFullMorning);
		  }
		else
		  {
			if(separateCoastInlandMorning)
			  {
				sentence << AAMUPAIVALLA_WORD << COAST_PHRASE
						 << cloudiness_sentence(theCloudinessIdCoastalMorning);
				sentence << Delimiter(",");
				sentence << INLAND_PHRASE << cloudiness_sentence(theCloudinessIdInlandMorning, true);
			  }
			else
			  {
				sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdFullMorning);
			  }
			sentence << Delimiter(",");
			if(separateCoastInlandAfternoon)
			  {
				sentence << ILTAPAIVALLA_WORD << COAST_PHRASE
						 << cloudiness_sentence(theCloudinessIdCoastalAfternoon);
				sentence << Delimiter(",");
				sentence << INLAND_PHRASE << cloudiness_sentence(theCloudinessIdInlandAfternoon, true);
			  }
			else
			  {
				sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdFullAfternoon);
			  }
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		if(puolipilvisesta_pilviseen(theCloudinessIdInlandMorning,
									 theCloudinessIdInlandAfternoon))
		  {
			sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
		  }
		else if(theCloudinessIdInlandMorning != MISSING_CLOUDINESS_ID &&
				theCloudinessIdInlandMorning != PUOLIPILVISTA_JA_PILVISTA && 
				theCloudinessIdInlandAfternoon != PUOLIPILVISTA_JA_PILVISTA &&
				abs(theCloudinessIdInlandMorning - theCloudinessIdInlandAfternoon) < 2)
		  {
			sentence << cloudiness_sentence(theCloudinessIdInlandMorning);
		  }
		else
		  {
			if(theCloudinessIdInlandMorning != MISSING_CLOUDINESS_ID)
			  {
				sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdInlandMorning);
				sentence << Delimiter(",");
				sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdInlandAfternoon, true);
			  }
			else
			  {
				sentence << cloudiness_sentence(theCloudinessIdInlandAfternoon);
			  }
		  }
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		if(puolipilvisesta_pilviseen(theCloudinessIdCoastalMorning,
									 theCloudinessIdCoastalAfternoon))
		  {
			sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
		  }
		else if(theCloudinessIdCoastalMorning != MISSING_CLOUDINESS_ID &&
				theCloudinessIdCoastalMorning != PUOLIPILVISTA_JA_PILVISTA && 
				theCloudinessIdCoastalAfternoon != PUOLIPILVISTA_JA_PILVISTA &&
				abs(theCloudinessIdCoastalMorning - theCloudinessIdCoastalAfternoon) < 2)
		  {
			sentence << cloudiness_sentence(theCloudinessIdCoastalMorning);
		  }
		else
		  {
			if(theCloudinessIdCoastalMorning != MISSING_CLOUDINESS_ID)
			  {
				sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdCoastalMorning);
				sentence << Delimiter(",");
				sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(theCloudinessIdCoastalAfternoon, true);
			  }
			else
			  {
				sentence << cloudiness_sentence(theCloudinessIdCoastalAfternoon);
			  }
		  }
	  }

	return sentence;
  }
  
  Sentence cloudiness_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(generator.isnight(thePeriodNumber))
		  {
			if(theResultData->find(YO) != theResultData->end())
			  sentence << cloudiness_night_sentence(theParameters, *((*theResultData)[YO]));
		  }
		else
		  {
			CloudinessDataItem* morningItem = 0;
			CloudinessDataItem* afternoonItem = 0;
			if(theResultData->find(AAMUPAIVA) != theResultData->end())
			  morningItem = (*theResultData)[AAMUPAIVA];
			if(theResultData->find(ILTAPAIVA) != theResultData->end())
			  afternoonItem = (*theResultData)[ILTAPAIVA];
			sentence << cloudiness_day_sentence(theParameters, *morningItem, *afternoonItem);
		  }
	  }

	return sentence;
  }

  void analyze_cloudiness_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		cloudiness_data_item_container* resultContainer = new cloudiness_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {				
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, dataItem));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, dataItem));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, dataItem));
			  }
		  }
		theParameters.theCloudinessData.push_back(resultContainer);
	  }
  }

 void preprocess_cloudiness_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		cloudiness_data_item_container* resultContainer = new cloudiness_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {				
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, dataItem));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, dataItem));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				CloudinessDataItem* dataItem = get_cloudiness_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, dataItem));
			  }
		  }
		theParameters.theCloudinessData.push_back(resultContainer);
	  }
  }

  unsigned int get_complete_precipitation_form(const string& theVariable,
									  const float thePrecipitationFormWater,
									  const float thePrecipitationFormDrizzle,
									  const float thePrecipitationFormSleet,
									  const float thePrecipitationFormSnow,
									  const float thePrecipitationFormFreezing)
  {
	unsigned int precipitation_form = 0;

	typedef std::pair<float, precipitation_form_id> precipitation_form_type;
	precipitation_form_type water(thePrecipitationFormWater, WATER_FORM);
	precipitation_form_type drizzle(thePrecipitationFormDrizzle, DRIZZLE_FORM);
	precipitation_form_type sleet(thePrecipitationFormSleet, SLEET_FORM);
	precipitation_form_type snow(thePrecipitationFormSnow, SNOW_FORM);
	precipitation_form_type freezing(thePrecipitationFormFreezing, FREEZING_FORM);

	vector<precipitation_form_type> precipitation_forms;
	precipitation_forms.push_back(water);
	precipitation_forms.push_back(drizzle);
	precipitation_forms.push_back(sleet);
	precipitation_forms.push_back(snow);
	precipitation_forms.push_back(freezing);

	sort(precipitation_forms.begin(),precipitation_forms.end());

	precipitation_form_id primaryPrecipitationForm = precipitation_forms[4].first > PRECIPITATION_FORM_REPORTING_LIMIT 
	  ? precipitation_forms[4].second : MISSING_PRECIPITATION_FORM;
	precipitation_form_id secondaryPrecipitationForm = precipitation_forms[3].first > PRECIPITATION_FORM_REPORTING_LIMIT 
	  ? precipitation_forms[3].second : MISSING_PRECIPITATION_FORM;
	precipitation_form_id tertiaryPrecipitationForm = precipitation_forms[2].first > PRECIPITATION_FORM_REPORTING_LIMIT
	  ? precipitation_forms[2].second : MISSING_PRECIPITATION_FORM;

	precipitation_form |= primaryPrecipitationForm;
	precipitation_form |= secondaryPrecipitationForm;
	precipitation_form |= tertiaryPrecipitationForm;

	return precipitation_form;
  }

  void get_dry_and_weak_limit(const wf_story_params& theParameters,
							  const unsigned int& thePrecipitationForm,
							  float& theDryWeatherLimit, 
							  float& theWeakPrecipitationLimit)
  {
	switch(thePrecipitationForm)
	  {
	  case WATER_FREEZING_FORM:
	  case FREEZING_FORM:
	  case WATER_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitWater;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitWater;
		}
		break;
	  case SLEET_FREEZING_FORM:
	  case SLEET_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitSleet;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitSleet;
		}
		break;
	  case SNOW_FORM:
	  case SNOW_FREEZING_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitSnow;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitSnow;
		}
		break;
	  case DRIZZLE_FORM:
	  case DRIZZLE_FREEZING_FORM:
	  case WATER_DRIZZLE_FREEZING_FORM:
	  case WATER_DRIZZLE_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitDrizzle;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitWater;
		}
		break;
	  case DRIZZLE_SLEET_FORM:
	  case DRIZZLE_SLEET_FREEZING_FORM:
	  case WATER_DRIZZLE_SLEET_FORM:
	  case WATER_SLEET_FREEZING_FORM:
	  case WATER_SLEET_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitSleet;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitSleet;
		}
		break;
	  case WATER_SNOW_FREEZING_FORM:
	  case WATER_SNOW_FORM:
	  case DRIZZLE_SNOW_FREEZING_FORM:
	  case DRIZZLE_SNOW_FORM:
	  case WATER_DRIZZLE_SNOW_FORM:
	  case WATER_SLEET_SNOW_FORM:
	  case DRIZZLE_SLEET_SNOW_FORM:
	  case SLEET_SNOW_FREEZING_FORM:
	  case SLEET_SNOW_FORM:
		{
		  theDryWeatherLimit = theParameters.theDryWeatherLimitSnow;
		  theWeakPrecipitationLimit = theParameters.theWeakPrecipitationLimitSnow;
		}
		break;
	  case MISSING_PRECIPITATION_FORM:
		break;
	  }
  }

  const weather_result_data_item_vector* get_precipitation_data(wf_story_params& theParameters,
														  const forecast_area_id& theAreaId,
														  const weather_result_data_id& theWeatherResultId)
  {
	return 	(*theParameters.theCompleteData[theAreaId])[theWeatherResultId];
  }

  void extract_the_interesting_precipitation_data(wf_story_params& theParameters,
												  const WeatherPeriod& thePeriod,
												  const forecast_area_id& theAreaId,
												  weather_result_data_item_vector& thePrecipitation,
												  weather_result_data_item_vector& thePrecipitationExtent,
												  weather_result_data_item_vector& thePrecipitationFromWater,
												  weather_result_data_item_vector& thePrecipitationFromDrizzle,
												  weather_result_data_item_vector& thePrecipitationFromSleet,
												  weather_result_data_item_vector& thePrecipitationFromSnow,
												  weather_result_data_item_vector& thePrecipitationFromFreezing,
												  weather_result_data_item_vector& thePrecipitationType,
												  weather_result_data_item_vector& thePrecipitationShareNorthEast,
												  weather_result_data_item_vector& thePrecipitationShareSouthEast,
												  weather_result_data_item_vector& thePrecipitationShareSouthWest,
												  weather_result_data_item_vector& thePrecipitationShareNorthWest)
  {
		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_DATA],
							thePrecipitation);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_EXTENT_DATA],
							thePrecipitationExtent);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_WATER_DATA],
							thePrecipitationFromWater);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_DRIZZLE_DATA],
							thePrecipitationFromDrizzle);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SLEET_DATA],
							thePrecipitationFromSleet);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SNOW_DATA],
							thePrecipitationFromSnow);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_FREEZING_DATA],
							thePrecipitationFromFreezing);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_TYPE_DATA],
							thePrecipitationType);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHEAST_SHARE_DATA],
							thePrecipitationShareNorthEast);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHEAST_SHARE_DATA],
							thePrecipitationShareSouthEast);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHWEST_SHARE_DATA],
							thePrecipitationShareSouthWest);

		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHWEST_SHARE_DATA],
							thePrecipitationShareNorthWest);
  }

  trend_id get_precipitation_trend(const wf_story_params& theParameters,
								   const weather_result_data_item_vector& thePrecipitationData, 
								   const unsigned int& precipitation_form)
  {
	trend_id precipitation_trend(NO_TREND);
  
	if(thePrecipitationData.size() <= 2)
	  return precipitation_trend;

	int iFromBegCount = thePrecipitationData.size() / 2;
	int iFromEndCount = thePrecipitationData.size() - iFromBegCount;
	
	// examine maximum 2 hours from the beginning and end
	iFromBegCount = iFromBegCount > 2 ? 2 : iFromBegCount;
	iFromEndCount = iFromEndCount > 2 ? 2 : iFromEndCount;
	
	float precipitationAtStart = get_mean(thePrecipitationData, 0, iFromBegCount);
	float precipitationAtEnd = get_mean(thePrecipitationData, thePrecipitationData.size()-iFromEndCount, 
										thePrecipitationData.size());

	float dryWeatherLimit;
	float weakPrecipitationLimit;

	get_dry_and_weak_limit(theParameters, precipitation_form, dryWeatherLimit, weakPrecipitationLimit);

	if(precipitationAtStart >= weakPrecipitationLimit && precipitationAtEnd <= dryWeatherLimit)
	  precipitation_trend = POUTAANTUU;
	else if(precipitationAtStart <= dryWeatherLimit && precipitationAtEnd >= weakPrecipitationLimit)
	  precipitation_trend = SADE_ALKAA;
	
	return precipitation_trend;
  }



  PrecipitationDataItemData* get_precipitation_data_item_data(wf_story_params& theParameters, 
															  const WeatherPeriod& thePeriod,
															  const forecast_area_id& theForeacastAreaId)
  {
	if(!(theParameters.theForecastArea & theForeacastAreaId))
	  return 0;

	PrecipitationDataItemData* item = new PrecipitationDataItemData(0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	0.0,
																	NO_TREND,
																	0.0,
																	thePeriod.localStartTime());

	weather_result_data_item_vector thePrecipitation;
	weather_result_data_item_vector thePrecipitationExtent;
	weather_result_data_item_vector thePrecipitationFromWater;
	weather_result_data_item_vector thePrecipitationFromDrizzle;
	weather_result_data_item_vector thePrecipitationFromSleet;
	weather_result_data_item_vector thePrecipitationFromSnow;
	weather_result_data_item_vector thePrecipitationFromFreezing;
	weather_result_data_item_vector thePrecipitationType;


	//	const double ignore_drizzle_limit = 0.02;

	extract_the_interesting_precipitation_data(theParameters,
											   thePeriod,
											   theForeacastAreaId,
											   thePrecipitation,
											   thePrecipitationExtent,
											   thePrecipitationFromWater,
											   thePrecipitationFromDrizzle,
											   thePrecipitationFromSleet,
											   thePrecipitationFromSnow,
											   thePrecipitationFromFreezing,
											   thePrecipitationType,
											   item->thePrecipitationShareNorthEast,
											   item->thePrecipitationShareSouthEast,
											   item->thePrecipitationShareSouthWest,
											   item->thePrecipitationShareNorthWest);
  


	float precipitation_intesity = get_mean(thePrecipitation);
	float precipitation_extent = get_mean(thePrecipitationExtent);
	float precipitation_form_water = get_mean(thePrecipitationFromWater);
	float precipitation_form_drizzle = get_mean(thePrecipitationFromDrizzle);
	float precipitation_form_sleet = get_mean(thePrecipitationFromSleet);
	float precipitation_form_snow = get_mean(thePrecipitationFromSnow);
	float precipitation_form_freezing = 0.0;
	float precipitation_type_showers = get_mean(thePrecipitationType);
	float pearson_coefficient = get_pearson_coefficient(thePrecipitation);
	float min_freezing = 0.0;
	float max_freezing = 0.0;
	get_min_max(thePrecipitationFromFreezing, min_freezing, max_freezing);
	precipitation_form_freezing = max_freezing;

	trend_id precipitation_trend(NO_TREND);

	const unsigned int precipitation_form = get_complete_precipitation_form(theParameters.theVariable,
														  precipitation_form_water,
														  precipitation_form_drizzle,
														  precipitation_form_sleet,
														  precipitation_form_snow,
														  precipitation_form_freezing);

	// TODO: if its raining save the location (north, east,...)

	// examine the trend
	float dryWeatherLimit;
	float weakPrecipitationLimit;
	float precipitationAtStart = thePrecipitation.at(0)->theResult.value();
	float precipitationAtEnd = thePrecipitation.at(thePrecipitation.size()-1)->theResult.value();

	get_dry_and_weak_limit(theParameters, precipitation_form, dryWeatherLimit, weakPrecipitationLimit);
	
	if((precipitationAtStart >= weakPrecipitationLimit && 
		precipitationAtEnd <= dryWeatherLimit) && 
	   pearson_coefficient < -TREND_CHANGE_COEFFICIENT_TRESHOLD)
	  {
		precipitation_trend = POUTAANTUU;
	  }
	else if((precipitationAtStart <= dryWeatherLimit && 
			 precipitationAtEnd >= weakPrecipitationLimit) && 
			pearson_coefficient >= TREND_CHANGE_COEFFICIENT_TRESHOLD)
	  {
		precipitation_trend = SADE_ALKAA;
	  }

	item->thePrecipitationForm = precipitation_form;
	item->thePrecipitationIntensity = precipitation_intesity;
	item->thePrecipitationExtent = precipitation_extent;
	item->thePrecipitationFormWater = precipitation_form_water;
	item->thePrecipitationFormDrizzle = precipitation_form_drizzle;
	item->thePrecipitationFormSleet = precipitation_form_sleet;
	item->thePrecipitationFormSnow = precipitation_form_snow;
	item->thePrecipitationFormFreezing = precipitation_form_freezing;
	item->thePrecipitationTypeShowers = precipitation_type_showers;
	item->theTrendId = precipitation_trend;
	item->thePearsonCoefficient = pearson_coefficient;

	return item;


  }

#ifdef LATER
PrecipitationDataItem* get_precipitation_data_item(wf_story_params& theParameters, 
												   const WeatherPeriod& thePeriod)
  {
	PrecipitationDataItem* item = new PrecipitationDataItem(0,
															0.0,
															0.0,
															0.0,
															0.0,
															0.0,
															0.0,
															0.0,
															0.0,
															NO_TREND,
															0.0);

	weather_result_data_item_vector thePrecipitationInland;
	weather_result_data_item_vector thePrecipitationExtentInland;
	weather_result_data_item_vector thePrecipitationFromWaterInland;
	weather_result_data_item_vector thePrecipitationFromDrizzleInland;
	weather_result_data_item_vector thePrecipitationFromSleetInland;
	weather_result_data_item_vector thePrecipitationFromSnowInland;
	weather_result_data_item_vector thePrecipitationFromFreezingInland;
	weather_result_data_item_vector thePrecipitationTypeInland;
	weather_result_data_item_vector thePrecipitationCoastal;
	weather_result_data_item_vector thePrecipitationExtentCoastal;
	weather_result_data_item_vector thePrecipitationFromWaterCoastal;
	weather_result_data_item_vector thePrecipitationFromDrizzleCoastal;
	weather_result_data_item_vector thePrecipitationFromSleetCoastal;
	weather_result_data_item_vector thePrecipitationFromSnowCoastal;
	weather_result_data_item_vector thePrecipitationFromFreezingCoastal;
	weather_result_data_item_vector thePrecipitationTypeCoastal;
	weather_result_data_item_vector thePrecipitationFull;
	weather_result_data_item_vector thePrecipitationExtentFull;
	weather_result_data_item_vector thePrecipitationFromWaterFull;
	weather_result_data_item_vector thePrecipitationFromDrizzleFull;
	weather_result_data_item_vector thePrecipitationFromSleetFull;
	weather_result_data_item_vector thePrecipitationFromSnowFull;
	weather_result_data_item_vector thePrecipitationFromFreezingFull;
	weather_result_data_item_vector thePrecipitationTypeFull;
	weather_result_data_item_vector& thePrecipitationTrendInputData = thePrecipitationFull;


	const double ignore_drizzle_limit = 0.02;


	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		extract_the_interesting_precipitation_data(theParameters,
												   thePeriod,
												   INLAND_AREA,
												   thePrecipitationInland,
												   thePrecipitationExtentInland,
												   thePrecipitationFromWaterInland,
												   thePrecipitationFromDrizzleInland,
												   thePrecipitationFromSleetInland,
												   thePrecipitationFromSnowInland,
												   thePrecipitationFromFreezingInland,
												   thePrecipitationTypeInland,
												   item->thePrecipitationShareNorthEast,
												   item->thePrecipitationShareSouthEast,
												   item->thePrecipitationShareSouthWest,
												   item->thePrecipitationShareNorthWest);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		extract_the_interesting_precipitation_data(theParameters,
												   thePeriod,
												   COASTAL_AREA,
												   thePrecipitationCoastal,
												   thePrecipitationExtentCoastal,
												   thePrecipitationFromWaterCoastal,
												   thePrecipitationFromDrizzleCoastal,
												   thePrecipitationFromSleetCoastal,
												   thePrecipitationFromSnowCoastal,
												   thePrecipitationFromFreezingCoastal,
												   thePrecipitationTypeCoastal,
												   item->thePrecipitationShareNorthEast,
												   item->thePrecipitationShareSouthEast,
												   item->thePrecipitationShareSouthWest,
												   item->thePrecipitationShareNorthWest);

	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		extract_the_interesting_precipitation_data(theParameters,
												   thePeriod,
												   FULL_AREA,
												   thePrecipitationFull,
												   thePrecipitationExtentFull,
												   thePrecipitationFromWaterFull,
												   thePrecipitationFromDrizzleFull,
												   thePrecipitationFromSleetFull,
												   thePrecipitationFromSnowFull,
												   thePrecipitationFromFreezingFull,
												   thePrecipitationTypeFull,
												   item->thePrecipitationShareNorthEast,
												   item->thePrecipitationShareSouthEast,
												   item->thePrecipitationShareSouthWest,
												   item->thePrecipitationShareNorthWest);
	  }

	float precipitation_intesity = 0.0;
	float precipitation_extent = 0.0;
	float precipitation_form_water = 0.0;
	float precipitation_form_drizzle = 0.0;
	float precipitation_form_sleet = 0.0;
	float precipitation_form_snow = 0.0;
	float precipitation_form_freezing = 0.0;
	float precipitation_type_showers = 0.0;
	float pearson_coefficient = 0.0;
	/*
	float north_east_precipitation = 0.0;
	float south_east_precipitation = 0.0;
	float south_west_precipitation = 0.0;
	float north_west_precipitation = 0.0;
	*/
	trend_id precipitation_trend(NO_TREND);


	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		precipitation_intesity = get_mean(thePrecipitationFull);
		precipitation_extent = get_mean(thePrecipitationExtentFull);
		thePrecipitationTrendInputData = thePrecipitationFull;
		if(precipitation_extent > 0.0 && precipitation_intesity > ignore_drizzle_limit)
		  {
			precipitation_form_water = get_mean(thePrecipitationFromWaterFull);
			precipitation_form_drizzle = get_mean(thePrecipitationFromDrizzleFull);
			precipitation_form_sleet = get_mean(thePrecipitationFromSleetFull);
			precipitation_form_snow = get_mean(thePrecipitationFromSnowFull);
			precipitation_type_showers = get_mean(thePrecipitationTypeFull);
			float min_freezing = 0.0;
			float max_freezing = 0.0;
			get_min_max(thePrecipitationFromFreezingFull, min_freezing, max_freezing);
			precipitation_form_freezing = max_freezing;
			pearson_coefficient = get_pearson_coefficient(thePrecipitationFull);
 		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		precipitation_intesity = get_mean(thePrecipitationInland);
		precipitation_extent = get_mean(thePrecipitationExtentInland);
		thePrecipitationTrendInputData = thePrecipitationInland;
		if(precipitation_extent > 0.0 && precipitation_intesity > ignore_drizzle_limit)
		  {
			precipitation_form_water = get_mean(thePrecipitationFromWaterInland);
			precipitation_form_drizzle = get_mean(thePrecipitationFromDrizzleInland);
			precipitation_form_sleet = get_mean(thePrecipitationFromSleetInland);
			precipitation_form_snow = get_mean(thePrecipitationFromSnowInland);
			precipitation_type_showers = get_mean(thePrecipitationTypeInland);
			float min_freezing = 0.0;
			float max_freezing = 0.0;
			get_min_max(thePrecipitationFromFreezingInland, min_freezing, max_freezing);
			precipitation_form_freezing = max_freezing;
			pearson_coefficient = get_pearson_coefficient(thePrecipitationInland);
		  }
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		precipitation_intesity = get_mean(thePrecipitationCoastal);
		precipitation_extent = get_mean(thePrecipitationExtentCoastal);
		thePrecipitationTrendInputData = thePrecipitationCoastal;
		if(precipitation_extent > 0.0 && precipitation_intesity > ignore_drizzle_limit)
		  {
			precipitation_form_water = get_mean(thePrecipitationFromWaterCoastal);
			precipitation_form_drizzle = get_mean(thePrecipitationFromDrizzleCoastal);
			precipitation_form_sleet = get_mean(thePrecipitationFromSleetCoastal);
			precipitation_form_snow = get_mean(thePrecipitationFromSnowCoastal);
			precipitation_type_showers = get_mean(thePrecipitationTypeCoastal);
			float min_freezing = 0.0;
			float max_freezing = 0.0;
			get_min_max(thePrecipitationFromFreezingCoastal, min_freezing, max_freezing);
			precipitation_form_freezing = max_freezing;
			pearson_coefficient = get_pearson_coefficient(thePrecipitationCoastal);
		  }
	  }

	const unsigned int precipitation_form = get_complete_precipitation_form(theParameters.theVariable,
														  precipitation_form_water,
														  precipitation_form_drizzle,
														  precipitation_form_sleet,
														  precipitation_form_snow,
														  precipitation_form_freezing);

	// TODO: if its raining save the location (north, east,...)

	//precipitation_trend = get_precipitation_trend(theParameters, thePrecipitationTrendInputData, precipitation_form);

	// examine the trend
	float dryWeatherLimit;
	float weakPrecipitationLimit;
	float precipitationAtStart = thePrecipitationTrendInputData.at(0)->theResult.value();
	float precipitationAtEnd = thePrecipitationTrendInputData.at(thePrecipitationTrendInputData.size()-1)->theResult.value();

	get_dry_and_weak_limit(theParameters, precipitation_form, dryWeatherLimit, weakPrecipitationLimit);
	
	if((precipitationAtStart >= weakPrecipitationLimit && 
		precipitationAtEnd <= dryWeatherLimit) && 
	   pearson_coefficient < -0.65)
	  {
		precipitation_trend = POUTAANTUU;
	  }
	else if((precipitationAtStart <= dryWeatherLimit && 
			 precipitationAtEnd >= weakPrecipitationLimit) && 
			pearson_coefficient >= 0.65)
	  {
		precipitation_trend = SADE_ALKAA;
	  }


	/*

	location_coordinate_vector locationCoordinates;

	RangeAcceptor precipitationlimits;
	precipitationlimits.lowerLimit(0.1);
	//	NFmiIndexMask indexMask;
  	double precipitationAmount = GetLocationCoordinates(itsSources,
											   Precipitation,
											   itsArea,
											   theExtendedPeriod,
											   precipitationlimits,
											   locationCoordinates);

	if(locationCoordinates.size() > 0)
	  {
		Rect precipitationRect(locationCoordinates);
		cout << "RAining at area " << static_cast<std::string>(precipitationRect) << "sum: " << precipitationAmount << endl;

		direction_id directionId(AreaTools::getDirection(areaRect, precipitationRect));
		std::string directionStr(AreaTools::getDirectionString(directionId));

		cout << "Precipitation area is in the " << directionStr << " in " << itsArea.name() << endl;

		map<direction_id, double> precipitationDistribution;

		AreaTools::getDirectionDistribution(locationCoordinates, precipitationDistribution);
		cout << "Precipitation distribution: " << endl;
		cout << "NORTH: " << precipitationDistribution[NORTH] << endl;
		cout << "SOUTH: " << precipitationDistribution[SOUTH] << endl;
		cout << "EAST: " << precipitationDistribution[EAST] << endl;
		cout << "WEST: " << precipitationDistribution[WEST] << endl;
		cout << "NORTHEAST: " << precipitationDistribution[NORTHEAST] << endl;
		cout << "SOUTHEAST: " << precipitationDistribution[SOUTHEAST] << endl;
		cout << "SOUTHWEST: " << precipitationDistribution[SOUTHWEST] << endl;
		cout << "NORTHWEST: " << precipitationDistribution[NORTHWEST] << endl;
	  }
	else
	  {
		cout << "No precipitation" << endl;
	  }


	*/

	item->thePrecipitationForm = precipitation_form;
	item->thePrecipitationIntensity = precipitation_intesity;
	item->thePrecipitationExtent = precipitation_extent;
	item->thePrecipitationFormWater = precipitation_form_water;
	item->thePrecipitationFormDrizzle = precipitation_form_drizzle;
	item->thePrecipitationFormSleet = precipitation_form_sleet;
	item->thePrecipitationFormSnow = precipitation_form_snow;
	item->thePrecipitationFormFreezing = precipitation_form_freezing;
	item->thePrecipitationTypeShowers = precipitation_type_showers;
	item->theTrendId = precipitation_trend;
	item->thePearsonCoefficient = pearson_coefficient;
	/*
	item = new PrecipitationDataItem(precipitation_form, 
									 precipitation_intesity, 
									 precipitation_extent,
									 precipitation_form_water, 
									 precipitation_form_drizzle, 
									 precipitation_form_sleet,
									 precipitation_form_snow, 
									 precipitation_form_freezing,
									 precipitation_type_showers,
									 precipitation_trend,
									 pearson_coefficient);
	*/

	return item;
  }
#endif

PrecipitationDataItem* get_precipitation_data_item(wf_story_params& theParameters, 
												   const WeatherPeriod& thePeriod)
  {
	PrecipitationDataItemData* dataItemCoastal = 
	  get_precipitation_data_item_data(theParameters, thePeriod, COASTAL_AREA);
	PrecipitationDataItemData* dataItemInland = 
	  get_precipitation_data_item_data(theParameters, thePeriod, INLAND_AREA);
	PrecipitationDataItemData* dataItemFull = 
	  get_precipitation_data_item_data(theParameters, thePeriod, FULL_AREA);
	
	PrecipitationDataItem* item = new PrecipitationDataItem(dataItemCoastal, dataItemInland, dataItemFull);

	return item;
  }




  void analyze_precipitation_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		precipitation_data_item_container* resultContainer = new precipitation_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, item));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, item));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				PrecipitationDataItem* item = get_precipitation_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, item));
			  }
		  }
		theParameters.thePrecipitationData.push_back(resultContainer);
	  }
  }

  bool is_dry_weather(const wf_story_params& theParameters,
					  const unsigned int& thePrecipitationForm,
					  const float& thePrecipitationIntensity,
					  const float& thePrecipitationExtent)
  {
	if(thePrecipitationExtent == 0.0)
	  return true;

	bool dry_weather = false;

	switch(thePrecipitationForm)
	  {
	  case WATER_FREEZING_FORM:
	  case FREEZING_FORM:
	  case WATER_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater)
		  dry_weather = true;
		break;
	  case SLEET_FREEZING_FORM:
	  case SLEET_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
		  dry_weather = true;
		break;
	  case SNOW_FORM:
	  case SNOW_FREEZING_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
		  dry_weather = true;
		break;
	  case DRIZZLE_FORM:
	  case DRIZZLE_FREEZING_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle)
		  dry_weather = true;
		break;
	  case WATER_DRIZZLE_FREEZING_FORM:
	  case WATER_DRIZZLE_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle)
		  dry_weather = true;
		break;
	  case DRIZZLE_SLEET_FORM:
	  case DRIZZLE_SLEET_FREEZING_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
		  dry_weather = true;
		break;
	  case WATER_DRIZZLE_SLEET_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
		  dry_weather = true;
		break;
	  case WATER_SLEET_FREEZING_FORM:
	  case WATER_SLEET_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet)
		  dry_weather = true;
		break;
	  case WATER_SNOW_FREEZING_FORM:
	  case WATER_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
		  dry_weather = true;
		break;
	  case DRIZZLE_SNOW_FREEZING_FORM:
	  case DRIZZLE_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
		  dry_weather = true;
		break;
	  case WATER_DRIZZLE_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
		  dry_weather = true;
		break;
	  case WATER_SLEET_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitWater &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
		  dry_weather = true;
		break;
	  case DRIZZLE_SLEET_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitDrizzle &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
		  dry_weather = true;
		break;
	  case SLEET_SNOW_FREEZING_FORM:
	  case SLEET_SNOW_FORM:
		if(thePrecipitationIntensity < theParameters.theDryWeatherLimitSleet &&
		   thePrecipitationIntensity < theParameters.theDryWeatherLimitSnow)
		  dry_weather = true;
		break;
	  case MISSING_PRECIPITATION_FORM:
		dry_weather = true;
		break;
	  }

	return dry_weather;
  }

  /*
  Sentence precipitation_sentence(wf_story_params& theParameters,
								  const WeatherPeriod& previousPeriod,
								  const WeatherPeriod& currentPeriod,
								  const WeatherPeriod& currentPeriod,
								  const )
  */

  Sentence precipitation_sentence(wf_story_params& theParameters,
								  const unsigned int& thePrecipitationForm,
								  const float thePrecipitationIntensity,
								  const float thePrecipitationExtent,
								  const float thePrecipitationFormWater,
								  const float thePrecipitationFormDrizzle,
								  const float thePrecipitationFormSleet,
								  const float thePrecipitationFormSnow,
								  const float thePrecipitationFormFreezing,
								  const float thePrecipitationTypeShowers)
  {
	Sentence sentence;

	bool dry_weather = is_dry_weather(theParameters, thePrecipitationForm, thePrecipitationIntensity, thePrecipitationExtent);

	bool report_cloudiness = true;

	if(dry_weather)
	  {
		sentence << SAA_ON_POUTAINEN_PHRASE;
	  }
	else
	  {
		const bool has_showers = (thePrecipitationTypeShowers != kFloatMissing && 
								  thePrecipitationTypeShowers >= theParameters.theShowerLimit);
		const bool mostly_dry_weather = thePrecipitationExtent <= theParameters.theMostlyDryWeatherLimit;
		const bool in_some_places = thePrecipitationExtent > theParameters.theInSomePlacesLowerLimit && 
		  thePrecipitationExtent <= theParameters.theInSomePlacesUpperLimit;
		const bool in_many_places = thePrecipitationExtent > theParameters.theInManyPlacesLowerLimit && 
		  thePrecipitationExtent <= theParameters.theInManyPlacesUpperLimit;
		  		
		bool can_be_freezing =  thePrecipitationFormFreezing > theParameters.theFreezingPrecipitationLimit;

		if(has_showers)
		  ; // report the cloudiness?


		switch(thePrecipitationForm)
		  {
		  case WATER_FREEZING_FORM:
		  case FREEZING_FORM:
		  case WATER_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitWater)
				{
				  theParameters.theLog << "Precipitation type is WATER" << endl;
 
				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_SADEKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitWater)
							sentence << HEIKKOJA_SADEKUUROJA_PHRASE;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitWater)
							sentence << VOIMAKKAITA_SADEKUUROJA_PHRASE;
						  else
							sentence << SADEKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitWater)
							sentence << HEIKKOA_SADETTA_PHRASE;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitWater)
							sentence << RUNSASTA_SADETTA_PHRASE;
						  else
							sentence << precipitation_phrase(theParameters.thePeriod.localStartTime(), theParameters.theVariable);

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case SLEET_FREEZING_FORM:
		  case SLEET_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSleet)
				{
				  theParameters.theLog << "Precipitation type is SLEET" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_RANTAKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{						
						  sentence << RANTAKUUROJA_WORD;
						}
					  else
						{
						  sentence << RANTASADETTA_WORD;
						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}
		  case SNOW_FORM:
		  case SNOW_FREEZING_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSnow)
				{
				  theParameters.theLog << "Precipitation type is SNOW" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_LUMIKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << HEIKKOJA_LUMIKUUROJA_PHRASE;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << SAKEITA_LUMIKUUROJA_PHRASE;
						  else
							sentence << LUMIKUUROJA_WORD;
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << HEIKKOA_LUMISADETTA_PHRASE;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << SAKEAA_LUMISADETTA_PHRASE;
						  else
							sentence << LUMISADETTA_WORD;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case DRIZZLE_FORM:
		  case DRIZZLE_FREEZING_FORM:

			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  theParameters.theLog << "Precipitation type is DRIZZLE" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_SADEKUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  sentence << SADEKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  sentence << TIHKUSADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case WATER_DRIZZLE_FREEZING_FORM:
		  case WATER_DRIZZLE_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  theParameters.theLog << "Precipitation type is WATER_DRIZZLE" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						{
						  sentence << YKSITTAISET_SADEKUUROT_MAHDOLLISIA;
						}
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  sentence << SADEKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormDrizzle)
							sentence <<  precipitation_phrase(theParameters.thePeriod.localStartTime(), theParameters.theVariable);
						  else
							sentence << TIHKUSADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case DRIZZLE_SLEET_FORM:
		  case DRIZZLE_SLEET_FREEZING_FORM:
		  case WATER_DRIZZLE_SLEET_FORM:
		  case WATER_SLEET_FREEZING_FORM:
		  case WATER_SLEET_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  theParameters.theLog << "Precipitation type is WATER_SLEET" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							sentence << YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA;
						  else
							sentence << YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA;
						}
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << RANTA_KUUROJA_WORD;
						  else
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << VESI_KUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(thePrecipitationFormWater >= thePrecipitationFormSleet)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << RANTASADETTA_WORD;
						  else
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case WATER_SNOW_FORM:
		  case DRIZZLE_SNOW_FORM:
		  case WATER_DRIZZLE_SNOW_FORM:
		  case WATER_SLEET_SNOW_FORM:
		  case DRIZZLE_SNOW_FREEZING_FORM:
		  case WATER_SNOW_FREEZING_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitDrizzle)
				{
				  if(thePrecipitationForm == (WATER_FORM & SNOW_FORM & SLEET_FORM))
					theParameters.theLog << "Precipitation type is WATER_SNOW_SLEET" << endl;
				  else
					theParameters.theLog << "Precipitation type is WATER_SNOW" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;

					  if(has_showers)
						sentence << YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << VOIMAKKAITA_SADEKUUROJA_PHRASE << TAI_WORD << SAKEITA_LUMIKUUROJA_PHRASE;
						  else
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE;
						}
					  else
						{
						  if(thePrecipitationIntensity <  theParameters.theWeakPrecipitationLimitSnow)
							sentence << HEIKKOA_WORD;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << KOVAA_WORD;
						  if(thePrecipitationFormWater >= thePrecipitationFormSnow)
							sentence << VESI_TAVUVIIVA_WORD << TAI_WORD << LUMISADETTA_WORD;
						  else
							sentence << LUMI_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}

		  case DRIZZLE_SLEET_SNOW_FORM:
		  case SLEET_SNOW_FREEZING_FORM:
		  case SLEET_SNOW_FORM:
			{
			  if(thePrecipitationIntensity >= theParameters.theDryWeatherLimitSleet)
				{
				  theParameters.theLog << "Precipitation type is SNOW_SLEET" << endl;

				  if(mostly_dry_weather)
					{
					  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
					  if(has_showers)
						sentence << YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA;
					}
				  else
					{
					  sentence << (in_some_places ? PAIKOIN_WORD : (in_many_places ? MONIN_PAIKOIN_WORD : ""));

					  if(has_showers)
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << SAKEITA_LUMIKUUROJA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else
							sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << LUMIKUUROJA_WORD;
						}
					  else
						{
						  if(thePrecipitationIntensity < theParameters.theWeakPrecipitationLimitSnow)
							sentence << HEIKKOA_LUMISADETTA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else if(thePrecipitationIntensity >= theParameters.theHeavyPrecipitationLimitSnow)
							sentence << SAKEAA_LUMISADETTA_PHRASE << TAI_WORD << RANTASADETTA_WORD;
						  else
							{
							  if(thePrecipitationFormSnow >= thePrecipitationFormSleet)
								sentence << LUMI_TAVUVIIVA_WORD << TAI_WORD << RANTASADETTA_WORD;
							  else
								sentence << RANTA_TAVUVIIVA_WORD << TAI_WORD << VESISADETTA_WORD;
							}

						  if(can_be_freezing)
							sentence << Delimiter(",") << JOKA_VOI_OLLA_JAATAVAA_PHRASE;

						  report_cloudiness = false;
						}
					}
				}
			  else
				{
				  sentence << SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE;
				}
			  break;
			}
		  }
	  }

	return sentence;
  }

  /*
  Sentence cloudiness_change_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		trend_id theTrendMorning(NO_TREND);
		trend_id theTrendAfternoon(NO_TREND);
		trend_id theTrendNight(NO_TREND);

		const CloudinessDataItemData* morningDataItem = 
		  theResultData->find(AAMUPAIVA) != theResultData->end() ? ((*theResultData)[AAMUPAIVA])->theFullData : 0;
		const CloudinessDataItemData* afternoonDataItem = 
		  theResultData->find(ILTAPAIVA) != theResultData->end() ? ((*theResultData)[ILTAPAIVA])->theFullData : 0;
		const CloudinessDataItemData* nightDataItem = 
		  theResultData->find(YO) != theResultData->end() ? ((*theResultData)[YO])->theFullData : 0;

		// TODO: Think how coastal and inland cloudiness change should be reported
		if(morningDataItem)
		  theTrendMorning = morningDataItem->theTrendId;
		if(afternoonDataItem)
		  theTrendAfternoon = afternoonDataItem->theTrendId;
		if(nightDataItem)
		  theTrendNight = nightDataItem->theTrendId;

		if(theTrendNight == NO_TREND && theTrendMorning == PILVISTYY && theTrendAfternoon == NO_TREND)
		  sentence << AAMUPAIVALLA_WORD << PILVISTYVAA_WORD;
		else if(theTrendNight == NO_TREND && theTrendMorning == NO_TREND && theTrendAfternoon == PILVISTYY)
		  sentence << ILTAPAIVALLA_WORD << PILVISTYVAA_WORD;
		if(theTrendNight == NO_TREND && theTrendMorning == SELKENEE && theTrendAfternoon == NO_TREND)
		  sentence << AAMUPAIVALLA_WORD << SELKENEVAA_WORD;
		else if(theTrendNight == NO_TREND && theTrendMorning == NO_TREND && theTrendAfternoon == SELKENEE)
		  sentence << ILTAPAIVALLA_WORD << SELKENEVAA_WORD;
	  }

	return sentence;
  }
  */

  Sentence precipitation_change_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	/*
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber || thePeriodNumber < 2)
	  return sentence;

	precipitation_data_item_container* theCurrentResultData = theParameters.thePrecipitationData[thePeriodNumber-1];
	precipitation_data_item_container* thePreviousResultData = theParameters.thePrecipitationData[thePeriodNumber-2];

	if(generator.isday(thePeriodNumber))
	  {
		// night data
		const PrecipitationDataItemData* nightPrecipitationPreviousFull = 
		  thePreviousResultData->find(YO) != thePreviousResultData->end() ? 
		  ((*thePreviousResultData)[YO])->theFullData : 0;
		const PrecipitationDataItemData* nightPrecipitationCurrentFull = 
		  theCurrentResultData->find(YO) != theCurrentResultData->end() ? 
		  ((*theCurrentResultData)[YO])->theFullData : 0;
		const PrecipitationDataItemData* nightPrecipitationPreviousInland = 
		  thePreviousResultData->find(YO) != thePreviousResultData->end() ? 
		  ((*thePreviousResultData)[YO])->theInlandData : 0;
		const PrecipitationDataItemData* nightPrecipitationCurrentInland = 
		  theCurrentResultData->find(YO) != theCurrentResultData->end() ? 
		  ((*theCurrentResultData)[YO])->theInlandData : 0;
		const PrecipitationDataItemData* nightPrecipitationPreviousCoastal = 
		  thePreviousResultData->find(YO) != thePreviousResultData->end() ? 
		  ((*thePreviousResultData)[YO])->theCoastalData : 0;
		const PrecipitationDataItemData* nightPrecipitationCurrentCoastal = 
		  theCurrentResultData->find(YO) != theCurrentResultData->end() ? 
		  ((*theCurrentResultData)[YO])->theCoastalData : 0;
		// morning
		// afternoon
	  }

*/
	/*
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber || thePeriodNumber < 2)
	  return sentence;

	precipitation_data_item_container* theCurrentResultData = theParameters.thePrecipitationData[thePeriodNumber-1];
	precipitation_data_item_container* thePreviousResultData = theParameters.thePrecipitationData[thePeriodNumber-2];

	if(generator.isday(thePeriodNumber))
	  {
		PrecipitationDataItem* nightPrecipitation = ((*thePreviousResultData)[YO]);
		PrecipitationDataItem* morningPrecipitation = ((*theCurrentResultData)[AAMUPAIVA]);
		PrecipitationDataItem* afternoonPrecipitation = ((*theCurrentResultData)[ILTAPAIVA]);

		bool dryWeatherNight = is_dry_weather(theParameters,
											  nightPrecipitation->thePrecipitationForm,
											  nightPrecipitation->thePrecipitationIntensity,
											  nightPrecipitation->thePrecipitationExtent);
		bool dryWeatherMorning = is_dry_weather(theParameters,
												morningPrecipitation->thePrecipitationForm,
												morningPrecipitation->thePrecipitationIntensity,
												morningPrecipitation->thePrecipitationExtent);
		bool dryWeatherAfternoon = is_dry_weather(theParameters,
												  afternoonPrecipitation->thePrecipitationForm,
												  afternoonPrecipitation->thePrecipitationIntensity,
												  afternoonPrecipitation->thePrecipitationExtent);
				
		if(!dryWeatherNight && morningPrecipitation->theTrendId == POUTAANTUU && dryWeatherAfternoon)
		  {// compare morning to the previous night
			  sentence << AAMUPAIVALLA_WORD << SAA_POUTAANTUU_PHRASE;
		  }
		else if(dryWeatherNight && morningPrecipitation->theTrendId == SADE_ALKAA && !dryWeatherAfternoon)
		  { // compare morning to the previous night
			sentence << SADE_WORD << ALKAA_WORD << AAMUPAIVALLA_WORD;
		  }
		else if(!dryWeatherMorning && afternoonPrecipitation->theTrendId == POUTAANTUU)
		  {// compare afternoon to the morning
			  sentence << ILTAPAIVALLA_WORD << SAA_POUTAANTUU_PHRASE;
		  }
		else if(dryWeatherMorning && afternoonPrecipitation->theTrendId == SADE_ALKAA)
		  {// compare afternoon to the morning
			sentence << SADE_WORD << ALKAA_WORD  << ILTAPAIVALLA_WORD;
		  }
	  }
	else
	  {
		PrecipitationDataItem* nightPrecipitation = ((*theCurrentResultData)[YO]);
		PrecipitationDataItem* afternoonPrecipitation = ((*thePreviousResultData)[ILTAPAIVA]);

		//compare night to the previous afternoon
		bool dryWeatherAfternoon = is_dry_weather(theParameters,
												  afternoonPrecipitation->thePrecipitationForm,
												  afternoonPrecipitation->thePrecipitationIntensity,
												  afternoonPrecipitation->thePrecipitationExtent);

		if(!dryWeatherAfternoon && nightPrecipitation->theTrendId == POUTAANTUU)
			  sentence << YOLLA_WORD << SAA_POUTAANTUU_PHRASE;
	  }
	*/

	return sentence;
  }

  Sentence handle_morning_afternoon_sentence(Sentence& theMorningSentence, Sentence& theAfternoonSentence)
  {
	Sentence sentence;

	// TODO: is this OK?
	if(same_content(theMorningSentence, theAfternoonSentence))
	  {
		sentence << theAfternoonSentence;
	  }
	else
	  {
		if(!theMorningSentence.empty())
		  sentence << AAMUPAIVALLA_WORD << theMorningSentence;

		if(!theMorningSentence.empty() && !theAfternoonSentence.empty())
		  sentence << Delimiter(",");

		if(!theAfternoonSentence.empty())
		  if(!theMorningSentence.empty())
			sentence << ILTAPAIVALLA_WORD << theAfternoonSentence;
		else
			sentence << theAfternoonSentence;
	  }

	return sentence;
  }

  /*
  Sentence cloudiness_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(generator.isnight(thePeriodNumber))
		  {
			if(theResultData->find(YO) != theResultData->end())
			  sentence << cloudiness_night_sentence(theParameters, *((*theResultData)[YO]));
		  }
		else
		  {
			CloudinessDataItem* morningItem = 0;
			CloudinessDataItem* afternoonItem = 0;
			if(theResultData->find(AAMUPAIVA) != theResultData->end())
			  morningItem = (*theResultData)[AAMUPAIVA];
			if(theResultData->find(ILTAPAIVA) != theResultData->end())
			  afternoonItem = (*theResultData)[ILTAPAIVA];
			sentence << cloudiness_day_sentence(theParameters, *morningItem, *afternoonItem);
		  }
	  }

	return sentence;
  }
  */



  Sentence precipitation_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

#ifdef LATER
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	PrecipitationDataItem* morningDataItem = 0;
	PrecipitationDataItem* afternoonDataItem = 0;
	PrecipitationDataItem* nightDataItem = 0;

	precipitation_data_item_container* theResultData = theParameters.thePrecipitationData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(theResultData->find(AAMUPAIVA) != theResultData->end())
		  morningDataItem = (*theResultData)[AAMUPAIVA];
		if(theResultData->find(ILTAPAIVA) != theResultData->end())
		  afternoonDataItem = (*theResultData)[ILTAPAIVA];
		if(theResultData->find(YO) != theResultData->end())
		  nightDataItem = (*theResultData)[YO];
	  }

	Sentence morningSentence;
	Sentence afternoonSentence;

	/*
	if(morningDataItem && afternoonDataItem && (*morningDataItem == *afternoonDataItem))
	  {
		sentence << precipitation_sentence(theParameters,
										   afternoonDataItem->thePrecipitationForm,
										   afternoonDataItem->thePrecipitationIntensity,
										   afternoonDataItem->thePrecipitationExtent,
										   afternoonDataItem->thePrecipitationFormWater,
										   afternoonDataItem->thePrecipitationFormDrizzle,
										   afternoonDataItem->thePrecipitationFormSleet,
										   afternoonDataItem->thePrecipitationFormSnow,
										   afternoonDataItem->thePrecipitationFormFreezing,
										   afternoonDataItem->thePrecipitationTypeShowers);
		return sentence;
	  }
	*/

	if(morningDataItem)
		morningSentence << precipitation_sentence(theParameters,
												morningDataItem->thePrecipitationForm,
												morningDataItem->thePrecipitationIntensity,
												morningDataItem->thePrecipitationExtent,
												morningDataItem->thePrecipitationFormWater,
												morningDataItem->thePrecipitationFormDrizzle,
												morningDataItem->thePrecipitationFormSleet,
												morningDataItem->thePrecipitationFormSnow,
												morningDataItem->thePrecipitationFormFreezing,
												morningDataItem->thePrecipitationTypeShowers);


	if(afternoonDataItem)
	  afternoonSentence << precipitation_sentence(theParameters,
												  afternoonDataItem->thePrecipitationForm,
												  afternoonDataItem->thePrecipitationIntensity,
												  afternoonDataItem->thePrecipitationExtent,
												  afternoonDataItem->thePrecipitationFormWater,
												  afternoonDataItem->thePrecipitationFormDrizzle,
												  afternoonDataItem->thePrecipitationFormSleet,
												  afternoonDataItem->thePrecipitationFormSnow,
												  afternoonDataItem->thePrecipitationFormFreezing,
												  afternoonDataItem->thePrecipitationTypeShowers);


	
	theParameters.thePrecipitationyMorning = !is_dry_weather(theParameters,
													morningDataItem->thePrecipitationForm,
													morningDataItem->thePrecipitationIntensity,
													morningDataItem->thePrecipitationExtent);
	theParameters.thePrecipitationyAfternoon = !is_dry_weather(theParameters,
													  afternoonDataItem->thePrecipitationForm,
													  afternoonDataItem->thePrecipitationIntensity,
													  afternoonDataItem->thePrecipitationExtent);
	

	sentence << handle_morning_afternoon_sentence(morningSentence, afternoonSentence);

	if(!sentence.empty())
	  {
		//weather_result_data_item_vector thePrecipitationShareNorthEast;
		float meanNorthEast = get_mean(afternoonDataItem->thePrecipitationShareNorthEast);
		float meanSouthEast = get_mean(afternoonDataItem->thePrecipitationShareSouthEast);
		float meanSouthWest = get_mean(afternoonDataItem->thePrecipitationShareSouthWest);
		float meanNorthWest = get_mean(afternoonDataItem->thePrecipitationShareNorthWest);

		/*
		cout << "meanNorthEast" << meanNorthEast << endl;
		cout << "meanSouthEast" << meanSouthEast << endl;
		cout << "meanSouthWest" << meanSouthWest << endl;
		cout << "meanNorthWest" << meanNorthWest << endl;
		*/
		if(meanNorthEast > 60.0)
		  sentence << ALUEEN_KOILLISOSISSA_PHRASE;
		else if(meanSouthEast > 60.0)
		  sentence << ALUEEN_KAAKKOISOSISSA_PHRASE;
		else if(meanSouthWest > 60.0)
		  sentence << ALUEEN_LOUNAISOSISSA_PHRASE;
		else if(meanNorthWest > 60.0)
		  sentence << ALUEEN_LUOTEISOSISSA_PHRASE;
		else if(meanNorthEast + meanNorthWest > 60.0)
		  sentence << ALUEEN_POHJOISOSISSA_PHRASE;
		else if(meanSouthEast + meanSouthWest > 60.0)
		  sentence << ALUEEN_ETELAOSISSA_PHRASE;
	  }

	if(nightDataItem)
	  sentence << precipitation_sentence(theParameters,
										 nightDataItem->thePrecipitationForm,
										 nightDataItem->thePrecipitationIntensity,
										 nightDataItem->thePrecipitationExtent,
										 nightDataItem->thePrecipitationFormWater,
										 nightDataItem->thePrecipitationFormDrizzle,
										 nightDataItem->thePrecipitationFormSleet,
										 nightDataItem->thePrecipitationFormSnow,
										 nightDataItem->thePrecipitationFormFreezing,
										 nightDataItem->thePrecipitationTypeShowers);
#endif

	return sentence;
  }


  /*
  Sentence cloudiness_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(generator.isnight(thePeriodNumber))
		  {
			if(theResultData->find(YO) != theResultData->end())
			  sentence << cloudiness_night_sentence(theParameters, *((*theResultData)[YO]));
		  }
		else
		  {
			CloudinessDataItem* morningItem = 0;
			CloudinessDataItem* afternoonItem = 0;
			if(theResultData->find(AAMUPAIVA) != theResultData->end())
			  morningItem = (*theResultData)[AAMUPAIVA];
			if(theResultData->find(ILTAPAIVA) != theResultData->end())
			  afternoonItem = (*theResultData)[ILTAPAIVA];
			sentence << cloudiness_day_sentence(theParameters, *morningItem, *afternoonItem);
		  }
	  }

	return sentence;
  }



  */


  ThunderDataItem* get_thunder_data_item(wf_story_params& theParameters, const WeatherPeriod& thePeriod)
  {
	ThunderDataItem* item = 0;

	weather_result_data_item_vector theThunderProbabilityData;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[FULL_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[INLAND_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		get_sub_time_series(thePeriod,
							*(*theParameters.theCompleteData[COASTAL_AREA])[THUNDERPROBABILITY_DATA],
							theThunderProbabilityData);
	  }

	float min_probability = kFloatMissing;
	float mean_probability = kFloatMissing;
	float max_probability = kFloatMissing;
	float standard_deviation_probability = kFloatMissing;

	mean_probability = get_mean(theThunderProbabilityData);
	get_min_max(theThunderProbabilityData, min_probability, max_probability);
	standard_deviation_probability = get_standard_deviation(theThunderProbabilityData);
	

	item = new ThunderDataItem(min_probability,
							   mean_probability,
							   max_probability,
							   standard_deviation_probability);

	return item;
  }

  void analyze_thunder_data(wf_story_params& theParameters)
  {
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	for(unsigned int i = 1; i <= generator.size(); i++)
	  {
		thunder_data_item_container* resultContainer = new thunder_data_item_container();

		WeatherPeriod theWeatherPeriod =  generator.period(i);
		WeatherPeriod theSubPeriod(theWeatherPeriod);

		if(generator.isday(i))
		  {
			if(get_part_of_the_day(theWeatherPeriod, AAMUPAIVA, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(AAMUPAIVA, item));
			  }
			if(get_part_of_the_day(theWeatherPeriod, ILTAPAIVA, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(ILTAPAIVA, item));
			  }
		  }
		else
		  {
			if(get_part_of_the_day(theWeatherPeriod, YO, theSubPeriod))
			  {
				ThunderDataItem* item = get_thunder_data_item(theParameters, theSubPeriod);
				resultContainer->insert(make_pair(YO, item));
			  }
		  }
		theParameters.theThunderData.push_back(resultContainer);
	  }
  }

  Sentence thunder_sentence(wf_story_params& theParameters,
							const float& theMinProbability,
							const float& theMeanProbability,
							const float& theMaxProbability,
							const float& theStandardDeviationProbability)
  {
	Sentence sentence;

	if(theMaxProbability == 0)
	  return sentence;

	if(theMaxProbability <= 15.0)
	  {
		sentence << PIENI_UKKOSEN_TODENNAKOISYYS_PHRASE;
	  }
	else if(theMaxProbability <= 35.0)
	  {
		sentence << MAHDOLLISESTI_UKKOSTA_PHRASE;
	  }
	else
	  {
		sentence << AJOITTAIN_UKKOSTA_PHRASE;
	  }

	return sentence;
  }

  Sentence thunder_sentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

	ThunderDataItem* morningDataItem = 0;
	ThunderDataItem* afternoonDataItem = 0;
	ThunderDataItem* nightDataItem = 0;

	thunder_data_item_container* theResultData = theParameters.theThunderData[thePeriodNumber - 1];

	if(theResultData->find(AAMUPAIVA) != theResultData->end())
		morningDataItem = (*theResultData)[AAMUPAIVA];
	if(theResultData->find(ILTAPAIVA) != theResultData->end())
		afternoonDataItem = (*theResultData)[ILTAPAIVA];
	if(theResultData->find(YO) != theResultData->end())
		nightDataItem = (*theResultData)[YO];

	Sentence morningSentence;
	Sentence afternoonSentence;
	Sentence nightSentence;

	if(morningDataItem)
	  morningSentence << thunder_sentence(theParameters,
										  morningDataItem->theMinProbability,
										  morningDataItem->theMeanProbability,
										  morningDataItem->theMaxProbability,
										  morningDataItem->theStandardDeviationProbability);
	
	if(afternoonDataItem)
	  afternoonSentence << thunder_sentence(theParameters,
											afternoonDataItem->theMinProbability,
											afternoonDataItem->theMeanProbability,
											afternoonDataItem->theMaxProbability,
											afternoonDataItem->theStandardDeviationProbability);

	if(nightDataItem)
	  nightSentence << thunder_sentence(theParameters,
										nightDataItem->theMinProbability,
										nightDataItem->theMeanProbability,
										nightDataItem->theMaxProbability,
										nightDataItem->theStandardDeviationProbability);

	sentence << handle_morning_afternoon_sentence(morningSentence, afternoonSentence);

	sentence << nightSentence;

	return sentence;
  }

  void allocate_data_structures(wf_story_params& theParameters, const forecast_area_id& theAreaId)
  {
	weather_result_data_item_vector* hourlyTemperature = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitation = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationExtent = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyPrecipitationType = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormWater = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormDrizzle = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormSleet = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormSnow = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationFormFreezing = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyCloudiness = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyThunderProbability = new weather_result_data_item_vector();

	weather_result_data_item_vector* hourlyPrecipitationShareNortEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationShareSouthEast = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationShareSouthWest = new weather_result_data_item_vector();
	weather_result_data_item_vector* hourlyPrecipitationShareNorthWest = new weather_result_data_item_vector();

	// first split the whole period to one-hour subperiods
	NFmiTime periodStartTime = theParameters.thePeriod.localStartTime();

	theParameters.theHourPeriodCount = 0;
	while(periodStartTime.IsLessThan(theParameters.thePeriod.localEndTime()))
	  {
		NFmiTime periodEndTime = periodStartTime;
		periodEndTime.ChangeByHours(1);
		WeatherPeriod theWeatherPeriod(periodEndTime, periodEndTime);
		//		WeatherPeriod theWeatherPeriod(periodStartTime, periodEndTime);
		WeatherResult theWeatherResult(kFloatMissing, 0);

		part_of_the_day_id  partOfTheDayId = get_part_of_the_day_id(theWeatherPeriod);

		hourlyTemperature->push_back(new WeatherResultDataItem(theWeatherPeriod, 
															   theWeatherResult, 
															   partOfTheDayId));
		hourlyPrecipitation->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																 theWeatherResult, 
																 partOfTheDayId));
		hourlyPrecipitationExtent->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	   theWeatherResult, 
																	   partOfTheDayId));
		hourlyPrecipitationType->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	 theWeatherResult, 
																	 partOfTheDayId));
		hourlyPrecipitationFormWater->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		  theWeatherResult, 
																		  partOfTheDayId));
		hourlyPrecipitationFormDrizzle->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			theWeatherResult, 
																			partOfTheDayId));
		hourlyPrecipitationFormSleet->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		  theWeatherResult, 
																		  partOfTheDayId));
		hourlyPrecipitationFormSnow->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																		 theWeatherResult, 
																		 partOfTheDayId));
		hourlyPrecipitationFormFreezing->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			 theWeatherResult, 
																			 partOfTheDayId));
		hourlyCloudiness->push_back(new WeatherResultDataItem(theWeatherPeriod, 
															  theWeatherResult, 
															  partOfTheDayId));
		hourlyThunderProbability->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																	  theWeatherResult, 
																	  partOfTheDayId));
		hourlyPrecipitationShareNortEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			  theWeatherResult, 
																			  partOfTheDayId));
		hourlyPrecipitationShareSouthEast->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			   theWeatherResult, 
																			   partOfTheDayId));
		hourlyPrecipitationShareSouthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			   theWeatherResult, 
																			   partOfTheDayId));
		hourlyPrecipitationShareNorthWest->push_back(new WeatherResultDataItem(theWeatherPeriod, 
																			   theWeatherResult, 
																			   partOfTheDayId));
		
		periodStartTime.ChangeByHours(1);
		theParameters.theHourPeriodCount++;
	  }

	weather_forecast_result_container* resultContainer = new weather_forecast_result_container();

	resultContainer->insert(make_pair(TEMPERATURE_DATA, hourlyTemperature));
	resultContainer->insert(make_pair(PRECIPITATION_DATA, hourlyPrecipitation));
	resultContainer->insert(make_pair(PRECIPITATION_EXTENT_DATA, hourlyPrecipitationExtent));
	resultContainer->insert(make_pair(PRECIPITATION_TYPE_DATA, hourlyPrecipitationType));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_WATER_DATA, hourlyPrecipitationFormWater));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_DRIZZLE_DATA, hourlyPrecipitationFormDrizzle));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_SLEET_DATA, hourlyPrecipitationFormSleet));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_SNOW_DATA, hourlyPrecipitationFormSnow));
	resultContainer->insert(make_pair(PRECIPITATION_FORM_FREEZING_DATA, hourlyPrecipitationFormFreezing));
	resultContainer->insert(make_pair(CLOUDINESS_DATA, hourlyCloudiness));
	resultContainer->insert(make_pair(THUNDERPROBABILITY_DATA, hourlyThunderProbability));
	resultContainer->insert(make_pair(PRECIPITATION_NORTHEAST_SHARE_DATA, hourlyPrecipitationShareNortEast));
	resultContainer->insert(make_pair(PRECIPITATION_SOUTHEAST_SHARE_DATA, hourlyPrecipitationShareSouthEast));
	resultContainer->insert(make_pair(PRECIPITATION_SOUTHWEST_SHARE_DATA, hourlyPrecipitationShareSouthWest));
	resultContainer->insert(make_pair(PRECIPITATION_NORTHWEST_SHARE_DATA, hourlyPrecipitationShareNorthWest));

	theParameters.theCompleteData.insert(make_pair(theAreaId, resultContainer));
  }

  void create_data_structures(wf_story_params& theParameters)
  {
	// first check which areas exists and create the data structures accordinly
	WeatherArea fullArea = theParameters.theArea;
	WeatherArea inlandArea = theParameters.theArea;
	inlandArea.type(WeatherArea::Inland);
	WeatherArea coastalArea = theParameters.theArea;
	coastalArea.type(WeatherArea::Coast);
	bool inlandExists = false;
	bool coastExists = false;
 	GridForecaster theForecaster;
	WeatherResult result = theForecaster.analyze(theParameters.theVariable,
												 theParameters.theSources,
												 Temperature,
												 Mean,
												 Maximum,
												 inlandArea,
												 theParameters.thePeriod);
	if(result.value() != kFloatMissing)
	  {
		inlandExists = true;
	  }
	
	result = theForecaster.analyze(theParameters.theVariable,
								   theParameters.theSources,
								   Temperature,
								   Mean,
								   Maximum,
								   coastalArea,
								   theParameters.thePeriod);
	if(result.value() != kFloatMissing)
	  {
		coastExists = true;
	  }

	if(inlandExists && coastExists)
	  {
		// create data structures for inland, coast and full areas
		allocate_data_structures(theParameters, INLAND_AREA);
		allocate_data_structures(theParameters, COASTAL_AREA);
		allocate_data_structures(theParameters, FULL_AREA);
		theParameters.theForecastArea |= INLAND_AREA;
		theParameters.theForecastArea |= COASTAL_AREA;
		theParameters.theForecastArea |= FULL_AREA;

		theParameters.theLog << "inland area included" << endl;
		theParameters.theLog << "coastal area included" << endl;
		theParameters.theLog << "full area included" << endl;

	  }
	else if(inlandExists)
	  {
		// create data structures for inland only
		allocate_data_structures(theParameters, INLAND_AREA);
		theParameters.theForecastArea |= INLAND_AREA;
		theParameters.theLog << "inland area included" << endl;
	  }
	else if(coastExists)
	  {
		// create data structures for coast only
		allocate_data_structures(theParameters, COASTAL_AREA);
		theParameters.theForecastArea |= COASTAL_AREA;
		theParameters.theLog << "coastal area included" << endl;
	  }
	else
	  {
		// error not inland, nor coastal area included
	  }
  }

  void deallocate_data_structure(unsigned int& thePeriodCount, weather_forecast_result_container& theResultContainer)
  {
	for(unsigned int i = 0; i < thePeriodCount; i++)
	  {
		delete (*theResultContainer[TEMPERATURE_DATA])[i];
		delete (*theResultContainer[CLOUDINESS_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_EXTENT_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_TYPE_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_WATER_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_SLEET_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_SNOW_DATA])[i];
		delete (*theResultContainer[PRECIPITATION_FORM_FREEZING_DATA])[i];
		delete (*theResultContainer[THUNDERPROBABILITY_DATA])[i];
	  }

	theResultContainer[TEMPERATURE_DATA]->clear();
	theResultContainer[PRECIPITATION_DATA]->clear();
	theResultContainer[PRECIPITATION_EXTENT_DATA]->clear();
	theResultContainer[PRECIPITATION_TYPE_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_WATER_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_SLEET_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_SNOW_DATA]->clear();
	theResultContainer[PRECIPITATION_FORM_FREEZING_DATA]->clear();
	theResultContainer[THUNDERPROBABILITY_DATA]->clear();

	delete theResultContainer[TEMPERATURE_DATA];
	delete theResultContainer[CLOUDINESS_DATA];
	delete theResultContainer[PRECIPITATION_DATA];
	delete theResultContainer[PRECIPITATION_EXTENT_DATA];
	delete theResultContainer[PRECIPITATION_TYPE_DATA];
	delete theResultContainer[PRECIPITATION_FORM_WATER_DATA];
	delete theResultContainer[PRECIPITATION_FORM_DRIZZLE_DATA];
	delete theResultContainer[PRECIPITATION_FORM_SLEET_DATA];
	delete theResultContainer[PRECIPITATION_FORM_SNOW_DATA];
	delete theResultContainer[PRECIPITATION_FORM_FREEZING_DATA];
	delete theResultContainer[THUNDERPROBABILITY_DATA];
	delete theResultContainer[PRECIPITATION_NORTHEAST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_SOUTHEAST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_SOUTHWEST_SHARE_DATA];
	delete theResultContainer[PRECIPITATION_NORTHWEST_SHARE_DATA];

	theResultContainer.clear();
  }

  void delete_data_structures(wf_story_params& theParameters)
  {
	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[INLAND_AREA]);
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[COASTAL_AREA]);
	  }
	if(theParameters.theForecastArea & FULL_AREA)
	  {
		deallocate_data_structure(theParameters.theHourPeriodCount, *theParameters.theCompleteData[FULL_AREA]);
	  }

	for(unsigned int i = 0; i < theParameters.theCloudinessData.size(); i++)
	  {
		cloudiness_data_item_container* cloudinessDataItemContainer = theParameters.theCloudinessData[i];
		for(unsigned int k = 0; k < cloudinessDataItemContainer->size(); k++)
		  delete (*cloudinessDataItemContainer)[k];
		cloudinessDataItemContainer->clear();
	  }
	theParameters.theCloudinessData.clear();
	for(unsigned int i = 0; i < theParameters.thePrecipitationData.size(); i++)
	  {
		precipitation_data_item_container* precipitationDataItemContainer = theParameters.thePrecipitationData[i];
		for(unsigned int k = 0; k < precipitationDataItemContainer->size(); k++)
		  delete (*precipitationDataItemContainer)[k];
		precipitationDataItemContainer->clear();
	  }
	theParameters.thePrecipitationData.clear();

	for(unsigned int i = 0; i < theParameters.theThunderData.size(); i++)
	  {
		thunder_data_item_container* thunderDataItemContainer = theParameters.theThunderData[i];
		for(unsigned int k = 0; k < thunderDataItemContainer->size(); k++)
		  delete (*thunderDataItemContainer)[k];
		thunderDataItemContainer->clear();
	  }
	theParameters.theThunderData.clear();
  }

  void init_parameters(wf_story_params& theParameters)
  {
	create_data_structures(theParameters);

	theParameters.theDryWeatherLimitWater =
	  Settings::optional_double(theParameters.theVariable + "::dry_weather_limit_water", DRY_WEATHER_LIMIT_WATER);
	theParameters.theDryWeatherLimitDrizzle =
	  Settings::optional_double(theParameters.theVariable + "::dry_weather_limit_drizzle", DRY_WEATHER_LIMIT_DRIZZLE);
	theParameters.theDryWeatherLimitSleet =
	  Settings::optional_double(theParameters.theVariable + "::dry_weather_limit_sleet", DRY_WEATHER_LIMIT_SLEET);
	theParameters.theDryWeatherLimitSnow =
	  Settings::optional_double(theParameters.theVariable + "::dry_weather_limit_snow", DRY_WEATHER_LIMIT_SNOW);
	theParameters.theWeakPrecipitationLimitWater =
	  Settings::optional_double(theParameters.theVariable + "::weak_precipitation_limit_water", WEAK_PRECIPITATION_LIMIT_WATER);
	theParameters.theWeakPrecipitationLimitSleet =
	  Settings::optional_double(theParameters.theVariable + "::weak_precipitation_limit_sleet", WEAK_PRECIPITATION_LIMIT_SLEET);
	theParameters.theWeakPrecipitationLimitSnow =
	  Settings::optional_double(theParameters.theVariable + "::weak_precipitation_limit_snow", WEAK_PRECIPITATION_LIMIT_SNOW);
	theParameters.theHeavyPrecipitationLimitWater =
	  Settings::optional_double(theParameters.theVariable + "::heavy_precipitation_limit_water", HEAVY_PRECIPITATION_LIMIT_WATER);
	theParameters.theHeavyPrecipitationLimitSleet =
	  Settings::optional_double(theParameters.theVariable + "::heavy_precipitation_limit_sleet", HEAVY_PRECIPITATION_LIMIT_SLEET);
	theParameters.theHeavyPrecipitationLimitSnow =
	  Settings::optional_double(theParameters.theVariable + "::heavy_precipitation_limit_snow", HEAVY_PRECIPITATION_LIMIT_SNOW);
	theParameters.thePrecipitationStormLimit =
	  Settings::optional_double(theParameters.theVariable + "::rainstorm_limit", RAINSTORM_LIMIT);
	theParameters.theMostlyDryWeatherLimit =
	  Settings::optional_double(theParameters.theVariable + "::mostly_dry_weather_limit", MOSTLY_DRY_WEATHER_LIMIT);
	theParameters.theInSomePlacesLowerLimit =
	  Settings::optional_double(theParameters.theVariable + "::in_some_places_lower_limit", IN_SOME_PLACES_LOWER_LIMIT);
	theParameters.theInSomePlacesUpperLimit =
	  Settings::optional_double(theParameters.theVariable + "::in_some_places_upper_limit", IN_SOME_PLACES_UPPER_LIMIT);
	theParameters.theInManyPlacesLowerLimit =
	  Settings::optional_double(theParameters.theVariable + "::in_many_places_lower_limit", IN_MANY_PLACES_LOWER_LIMIT);
	theParameters.theInManyPlacesUpperLimit =
	  Settings::optional_double(theParameters.theVariable + "::in_many_places_upper_limit", IN_MANY_PLACES_UPPER_LIMIT);
	theParameters.theFreezingPrecipitationLimit =
	  Settings::optional_double(theParameters.theVariable + "::freezing_rain_limit", FREEZING_RAIN_LIMIT);
	theParameters.theShowerLimit =
	  Settings::optional_double(theParameters.theVariable + "::shower_limit", SHOWER_LIMIT);
  }







  typedef std::pair<WeatherPeriod, trend_id> weather_period_trend_id_pair;
  typedef vector<weather_period_trend_id_pair> trend_id_vector;


  class PrecipitationDataView
  {
  public:

	float getMean(const precipitation_data_vector& theData, 
				  const weather_result_data_id& theDataId,
				  const WeatherPeriod& theWeatherPeriod) const
	{
	  float retval = 0.0;
	  int count = 0;

	  for(unsigned int i = 0; i < theData.size(); i++)
		{
		  if(theData[i]->theObervationTime < theWeatherPeriod.localStartTime() ||
			 theData[i]->theObervationTime > theWeatherPeriod.localEndTime())
			continue;

		  switch(theDataId)
			{
			case PRECIPITATION_DATA:
			  {
				retval += theData[i]->thePrecipitationIntensity;
				count++;
			  }
			  break;
			case PRECIPITATION_EXTENT_DATA:
			  {
				retval += theData[i]->thePrecipitationExtent;
				count++;
			  }
			  break;
			case PRECIPITATION_FORM_WATER_DATA:
			  {
				if(theData[i]->thePrecipitationFormWater != kFloatMissing)
				  {
					retval += theData[i]->thePrecipitationFormWater;
					count++;
				  }
			  }
			  break;
			case PRECIPITATION_FORM_DRIZZLE_DATA:
			  {
				if(theData[i]->thePrecipitationFormDrizzle != kFloatMissing)
				  {
					retval += theData[i]->thePrecipitationFormDrizzle;
					count++;
				  }
			  }
			  break;
			case PRECIPITATION_FORM_SLEET_DATA:
			  {
				if(theData[i]->thePrecipitationFormSleet != kFloatMissing)
				  {
					retval += theData[i]->thePrecipitationFormSleet;
					count++;
				  }
			  }
			  break;
			case PRECIPITATION_FORM_SNOW_DATA:
			  {
				if(theData[i]->thePrecipitationFormSnow != kFloatMissing)
				  {
					retval += theData[i]->thePrecipitationFormSnow;
					count++;
				  }
			  }
			  break;
			case PRECIPITATION_FORM_FREEZING_DATA:
			  {
				if(theData[i]->thePrecipitationFormFreezing != kFloatMissing)
				  {
					retval += theData[i]->thePrecipitationFormFreezing;
					count++;
				  }
			  }
			  break;
			case PRECIPITATION_TYPE_DATA:
			  {
				if(theData[i]->thePrecipitationTypeShowers != kFloatMissing)
				  {
					retval += theData[i]->thePrecipitationTypeShowers;
					count++;
				  }
			  }
			  break;
			default:
			  break;
			}
		}

	  if(count > 0)
		retval = (retval / count);

	  return retval;
	}


	unsigned int getPrecipitationForm(const precipitation_data_vector& theData,
									  const WeatherPeriod& theWeatherPeriod) const
	{
	  unsigned int retval = MISSING_PRECIPITATION_FORM;

	  for(unsigned int i = 0; i < theData.size(); i++)
		{
		  if(theData[i]->theObervationTime < theWeatherPeriod.localStartTime() ||
			 theData[i]->theObervationTime > theWeatherPeriod.localEndTime())
			continue;

		  retval |= theData[i]->thePrecipitationForm;
		}

	  return retval;
	}

	unsigned int getPrecipitationGategory(const float& thePrecipitation, 
										  const unsigned int& theType) const
	{
	  unsigned int retval(DRY_WEATHER_CATEGORY);

	  switch(theType)
		{
		case WATER_FORM:
		case WATER_DRIZZLE_FORM:
		case WATER_SLEET_FORM:
		case WATER_SNOW_FORM:
		case WATER_FREEZING_FORM:
		case WATER_DRIZZLE_SLEET_FORM:
		case WATER_DRIZZLE_SNOW_FORM:
		case WATER_DRIZZLE_FREEZING_FORM:
		case WATER_SLEET_SNOW_FORM:
		case WATER_SLEET_FREEZING_FORM:
		case WATER_SNOW_FREEZING_FORM:
		case DRIZZLE_FORM:
		case DRIZZLE_SLEET_FORM:
		case DRIZZLE_SLEET_SNOW_FORM:
		case DRIZZLE_SLEET_FREEZING_FORM:
		case DRIZZLE_FREEZING_FORM:
		case DRIZZLE_SNOW_FORM:
		case DRIZZLE_SNOW_FREEZING_FORM:
		case FREEZING_FORM:
		  {
			if(thePrecipitation <= theParameters.theDryWeatherLimitWater)
			  retval = DRY_WEATHER_CATEGORY;
			else if(thePrecipitation >= theParameters.theWeakPrecipitationLimitWater &&
					thePrecipitation < theParameters.theHeavyPrecipitationLimitWater)
			  retval = MODERATE_WATER_PRECIPITATION;
			else if(thePrecipitation > theParameters.theHeavyPrecipitationLimitWater)
			  retval = HEAVY_WATER_PRECIPITATION;		  
		  }
		  break;
		case SLEET_FORM:
		case SLEET_FREEZING_FORM:
		case SLEET_SNOW_FREEZING_FORM:
		  {
			if(thePrecipitation <= theParameters.theDryWeatherLimitSleet)
			  retval = DRY_WEATHER_CATEGORY;
			else if(thePrecipitation >= theParameters.theWeakPrecipitationLimitSleet &&
					thePrecipitation < theParameters.theHeavyPrecipitationLimitSleet)
			  retval = MODERATE_WATER_PRECIPITATION;
			else if(thePrecipitation > theParameters.theHeavyPrecipitationLimitSleet)
			  retval = HEAVY_WATER_PRECIPITATION;		  
		  }
		  break;
		case SNOW_FORM:
		case SNOW_FREEZING_FORM:
		  {
			if(thePrecipitation <= theParameters.theDryWeatherLimitSnow)
			  retval = DRY_WEATHER_CATEGORY;
			else if(thePrecipitation >= theParameters.theWeakPrecipitationLimitSnow &&
					thePrecipitation < theParameters.theHeavyPrecipitationLimitSnow)
			  retval = MODERATE_WATER_PRECIPITATION;
			else if(thePrecipitation > theParameters.theHeavyPrecipitationLimitSnow)
			  retval = HEAVY_WATER_PRECIPITATION;		  
		  }
		  break;
		default:
		  break;
		}

	  return retval;
	}

	bool separateCoastInlandPrecipitation(const WeatherPeriod& theWeatherPeriod) const
	{
	  if(!(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA))
		return false;

	  float coastalPrecipitation = getMean(theCoastalData, PRECIPITATION_DATA, theWeatherPeriod);
	  float inlandPrecipitation = getMean(theInlandData, PRECIPITATION_DATA, theWeatherPeriod);
	  float coastalExtent = getMean(theCoastalData, PRECIPITATION_EXTENT_DATA, theWeatherPeriod);
	  float inlandExtent = getMean(theInlandData, PRECIPITATION_EXTENT_DATA, theWeatherPeriod);
	  unsigned int coastalPrecipitationForm = getPrecipitationForm(theCoastalData, theWeatherPeriod);
	  unsigned int inlandPrecipitationForm = getPrecipitationForm(theInlandData, theWeatherPeriod);
	  float coastalShowersPercentage = getMean(theCoastalData, PRECIPITATION_TYPE_DATA, theWeatherPeriod);
	  float inlandShowersPercentage = getMean(theInlandData, PRECIPITATION_TYPE_DATA, theWeatherPeriod);
	  unsigned int precipitationCategoryCoastal = getPrecipitationGategory(coastalPrecipitation, coastalPrecipitationForm);
	  unsigned int precipitationCategoryInland = getPrecipitationGategory(inlandPrecipitation, inlandPrecipitationForm);

	  if((coastalPrecipitation < 0.04 && inlandPrecipitation > 0.4) ||
		 (coastalPrecipitation > 0.4 && inlandPrecipitation < 0.04) ||
		 (coastalExtent < 20.0 && inlandExtent > 80.0) ||
		 (coastalExtent > 80.0 && inlandExtent < 30.0) ||
		 (coastalShowersPercentage < 20.0 && inlandShowersPercentage > 80.0) ||
		 (coastalShowersPercentage > 80.0 && inlandShowersPercentage < 20.0) ||
		 precipitationCategoryCoastal != precipitationCategoryInland)
		return true;
	  
	  return false;
	}


	void printOutPrecipitationVector(std::ostream & theOutput,
									 const precipitation_data_vector& thePrecipitationDataVector) const
	{
	  for(unsigned int i = 0; i < thePrecipitationDataVector.size(); i++)
		{
		  theOutput << *(thePrecipitationDataVector[i]);
		}
	}
	
	void printOutPrecipitationData(std::ostream & theOutput) const
	{
	  if(theParameters.theForecastArea & COASTAL_AREA)
		{
		  theOutput << "COASTAL PRECIPITATION: " << endl;
		  printOutPrecipitationVector(theOutput, theCoastalData);
		}
	  if(theParameters.theForecastArea & INLAND_AREA)
		{
		  theOutput << "INLAND PRECIPITATION: " << endl;
		  printOutPrecipitationVector(theOutput, theInlandData);
		}
	  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
		{
		  theOutput << "FULL AREA PRECIPITATION: " << endl;
		  printOutPrecipitationVector(theOutput, theFullData);
		}
	}


	void gatherPrecipitationData(wf_story_params& theParameters)
	{
	  if(theParameters.theForecastArea & COASTAL_AREA)
		fillInPrecipitationDataVector(theParameters, COASTAL_AREA);
	  if(theParameters.theForecastArea & INLAND_AREA)
		fillInPrecipitationDataVector(theParameters, INLAND_AREA);
	  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
		fillInPrecipitationDataVector(theParameters, FULL_AREA);
	}

	void fillInPrecipitationDataVector(wf_story_params& theParameters, const forecast_area_id& theAreaId)
	{


	  weather_result_data_item_vector* precipitationHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_DATA];
	  weather_result_data_item_vector* precipitationExtentHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_EXTENT_DATA];
	  weather_result_data_item_vector* precipitationFormWaterHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_WATER_DATA];
	  weather_result_data_item_vector* precipitationFormDrizzleHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_DRIZZLE_DATA];
	  weather_result_data_item_vector* precipitationFormSleetHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SLEET_DATA];
	  weather_result_data_item_vector* precipitationFormSnowHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_SNOW_DATA];
	  weather_result_data_item_vector* precipitationFormFreezingHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_FORM_FREEZING_DATA];
	  weather_result_data_item_vector* precipitationTypeHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_TYPE_DATA];
	  weather_result_data_item_vector* precipitationNorthEastShareHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHEAST_SHARE_DATA];
	  weather_result_data_item_vector* precipitationSouthEastShareHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHEAST_SHARE_DATA];
	  weather_result_data_item_vector* precipitationSouthWestShareHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_SOUTHWEST_SHARE_DATA];
	  weather_result_data_item_vector* precipitationNorthWestShareHourly = 
		(*theParameters.theCompleteData[theAreaId])[PRECIPITATION_NORTHWEST_SHARE_DATA];

	  precipitation_data_vector& dataVector = (theAreaId == COASTAL_AREA ? theCoastalData : 
											   (theAreaId == INLAND_AREA ? theInlandData : theFullData));

	  for(unsigned int i = 0; i < precipitationHourly->size(); i++)
		{
		  float precipitationIntesity = (*precipitationHourly)[i]->theResult.value();
		  float precipitationExtent = (*precipitationExtentHourly)[i]->theResult.value();
		  float precipitationFormWater = (*precipitationFormWaterHourly)[i]->theResult.value();
		  float precipitationFormDrizzle = (*precipitationFormDrizzleHourly)[i]->theResult.value();
		  float precipitationFormSleet = (*precipitationFormSleetHourly)[i]->theResult.value();
		  float precipitationFormSnow = (*precipitationFormSnowHourly)[i]->theResult.value();
		  float precipitationFormFreezing = (*precipitationFormFreezingHourly)[i]->theResult.value();
		  float precipitationTypeShowers = (*precipitationTypeHourly)[i]->theResult.value();
		  // float precipitationPercentageNorthEast = 
		  

		  const unsigned int precipitationForm = 
			get_complete_precipitation_form(theParameters.theVariable,
								   precipitationFormWater,
								   precipitationFormDrizzle,
								   precipitationFormSleet,
								   precipitationFormSnow,
								   precipitationFormFreezing);
		  
		  	PrecipitationDataItemData* item = 
			  new PrecipitationDataItemData(precipitationForm,
											precipitationIntesity,
											precipitationExtent,
											precipitationFormWater,
											precipitationFormDrizzle,
											precipitationFormSleet,
											precipitationFormSnow,
											precipitationFormFreezing,
											precipitationTypeShowers,
											NO_TREND,
											0.0,
											(*precipitationHourly)[i]->thePeriod.localStartTime());
			
			item->thePrecipitationPercentageNorthEast = (*precipitationNorthEastShareHourly)[i]->theResult.value();
			item->thePrecipitationPercentageSouthEast = (*precipitationSouthEastShareHourly)[i]->theResult.value();
			item->thePrecipitationPercentageSouthWest = (*precipitationSouthWestShareHourly)[i]->theResult.value();
			item->thePrecipitationPercentageNorthWest = (*precipitationNorthWestShareHourly)[i]->theResult.value();

			dataVector.push_back(item);
		}
	}

	void findOutPrecipitationPeriods()
	{
	  if(theParameters.theForecastArea & COASTAL_AREA)
		findOutPrecipitationPeriods(COASTAL_AREA);
	  if(theParameters.theForecastArea & INLAND_AREA)
		findOutPrecipitationPeriods(INLAND_AREA);
	  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
		findOutPrecipitationPeriods(FULL_AREA);
	}

	void findOutPrecipitationPeriods(const forecast_area_id& theAreaId)
	{
	  precipitation_data_vector* dataSourceVector = 0;
	  vector<WeatherPeriod>* dataDestinationVector = 0;

	  if(theAreaId & INLAND_AREA && theAreaId & COASTAL_AREA)
		{
		  dataSourceVector = &theFullData;
		  dataDestinationVector = &thePrecipitationPeriodsFull;
		}
	  else if(theAreaId & INLAND_AREA)
		{
		  dataSourceVector = &theInlandData;
		  dataDestinationVector = &thePrecipitationPeriodsInland;
		}
	  else if(theAreaId & COASTAL_AREA)
		{
		  dataSourceVector = &theCoastalData;
		  dataDestinationVector = &thePrecipitationPeriodsCoastal;
		}

	  if(!dataSourceVector)
		return;

	  bool isDry = true;
	  unsigned int periodStartIndex = 0;
	  for(unsigned int i = 0; i < dataSourceVector->size(); i++)
		{
		  if(i == 0)
			{
			  isDry = ((*dataSourceVector)[i]->thePrecipitationTypeShowers == kFloatMissing);
			  continue;
			}

		  if(((*dataSourceVector)[i-1]->thePrecipitationTypeShowers == kFloatMissing &&
			  (*dataSourceVector)[i]->thePrecipitationTypeShowers != kFloatMissing) ||
			 ((*dataSourceVector)[i-1]->thePrecipitationTypeShowers != kFloatMissing &&
			  (*dataSourceVector)[i]->thePrecipitationTypeShowers == kFloatMissing))
			{
			  if(!isDry)
				{
				  NFmiTime startTime((*dataSourceVector)[periodStartIndex]->theObervationTime);
				  NFmiTime endTime((*dataSourceVector)[i-1]->theObervationTime);
				  dataDestinationVector->push_back(WeatherPeriod(startTime, endTime));
				}
			  isDry = !isDry;
			  periodStartIndex = i;
			}
		}
	  if(!isDry && periodStartIndex != dataSourceVector->size() - 1)
		{
		  NFmiTime startTime((*dataSourceVector)[periodStartIndex]->theObervationTime);
		  NFmiTime endTime((*dataSourceVector)[dataSourceVector->size()-1]->theObervationTime);
		  dataDestinationVector->push_back(WeatherPeriod(startTime, endTime));
		}
	}

	/*
	  thePrecipitationForm(precipitationForm),
	  thePrecipitationIntensity(precipitationIntensity),
	  thePrecipitationExtent(precipitationExtent),
	  thePrecipitationFormWater(precipitationFormWater),
	  thePrecipitationFormDrizzle(precipitationFormDrizzle),
	  thePrecipitationFormSleet(precipitationFormSleet),
	  thePrecipitationFormSnow(precipitationFormSnow),
	  thePrecipitationFormFreezing(precipitationFormFreezing),
	  thePrecipitationTypeShowers(precipitationTypeShowers),
	  theTrendId(trendId),
	  thePearsonCoefficient(pearsonCoefficient),
	  theObervationTime(obervationTime),
	  thePrecipitationPercentageNorthEast(0.0),
	  thePrecipitationPercentageSouthEast(0.0),
	  thePrecipitationPercentageSouthWest(0.0),
	  thePrecipitationPercentageNorthWest(0.0)
	 */

	void findOutPrecipitationTrends()
	{
	  if(theParameters.theForecastArea & COASTAL_AREA)
		findOutPrecipitationTrends(theCoastalData, thePrecipitationTrendsCoastal);
	  if(theParameters.theForecastArea & INLAND_AREA)
		findOutPrecipitationTrends(theInlandData, thePrecipitationTrendsInland);
	  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
		findOutPrecipitationTrends(theFullData, thePrecipitationTrendsFull);
	}

	void findOutPrecipitationTrends(const precipitation_data_vector& theData,
									trend_id_vector& thePrecipitationTrends)
	{
	  float dryWeatherLimit;
	  float weakPrecipitationLimit;

	  
	  const unsigned int step = 4;
	  unsigned int index = 2;
	  while(index < theData.size() - step)
		{
		  WeatherPeriod period(theData[index]->theObervationTime,
							   theData[index+step-1]->theObervationTime);
		  
		  unsigned int precipitation_form = theData[index]->thePrecipitationForm;
		  float precipitationAtStart = (theData[index]->thePrecipitationIntensity + 
										theData[index+1]->thePrecipitationIntensity)/2.0;
		  float precipitationAtEnd = (theData[index+2]->thePrecipitationIntensity + 
									  theData[index+3]->thePrecipitationIntensity)/2.0;
		  /*
			float precipitationExtentAtStart = (theData[startIndex-1]->thePrecipitationExtent + 
			theData[startIndex-2]->thePrecipitationExtent)/2.0;
			float precipitationExtentAtEnd = (theData[endIndex+1]->thePrecipitationExtent + 
			theData[endIndex+2]->thePrecipitationExtent)/2.0;
		  */
		  
		  get_dry_and_weak_limit(theParameters, precipitation_form, dryWeatherLimit, weakPrecipitationLimit);
		  
		  trend_id trendId(NO_TREND);
		  
		  // TODO: check extent, type also
		  
		  
		  //		if((precipitationAtStart >= weakPrecipitationLimit && 
		  //precipitationAtEnd <= dryWeatherLimit)) //pearson_coefficient < -TREND_CHANGE_COEFFICIENT_TRESHOLD)
		  if((precipitationAtStart > 0.1 && 
			  precipitationAtEnd < 0.1))/* &&
										   (precipitationExtentAtStart > 50.0 &&
										   precipitationExtentAtEnd < 50.0))*/
			{
			  trendId = POUTAANTUU;
			}
		  else if((precipitationAtStart < 0.1 && 
				   precipitationAtEnd > 0.1))/* &&
												(precipitationExtentAtStart < 50.0 &&
												precipitationExtentAtEnd > 50.0))*/
			{
			  trendId = SADE_ALKAA;
			}
		  
		if(trendId != NO_TREND)
		  {
			thePrecipitationTrends.push_back(make_pair(WeatherPeriod(theData[index+2]->theObervationTime,
																	 theData[index+3]->theObervationTime), trendId));
		  }
		
		index += (trendId == NO_TREND ? 1 : step/2);
		}
	  
	  cleanUpPrecipitationTrends(thePrecipitationTrendsCoastal);
	  cleanUpPrecipitationTrends(thePrecipitationTrendsInland);
	  cleanUpPrecipitationTrends(thePrecipitationTrendsFull);
	}
	
	void removeRedundantTrends(trend_id_vector& thePrecipitationTrends, const vector<int>& theRemoveIndexes)
	{
	  if(theRemoveIndexes.size() > 0)
		{
		  for(unsigned int i = theRemoveIndexes.size(); i > 0; i--)
			{
			  thePrecipitationTrends.erase(thePrecipitationTrends.begin()+theRemoveIndexes[i-1]);
			}
		}
	}

	void removeDuplicatePrecipitationTrends(trend_id_vector& thePrecipitationTrends)
	{
	  vector<int> removeIndexes;

	  for(unsigned int i = 1; i < thePrecipitationTrends.size(); i++)
		{
		  trend_id previousPeriodTrendId(thePrecipitationTrends[i-1].second);
		  trend_id currentPeriodTrendId(thePrecipitationTrends[i].second);
		  if( previousPeriodTrendId == currentPeriodTrendId)
			removeIndexes.push_back(i);
		}
	  removeRedundantTrends(thePrecipitationTrends, removeIndexes);
	}

	// take into account the continuous precipitation periods
	void cleanUpPrecipitationTrends(trend_id_vector& thePrecipitationTrends)
	{
	  vector<int> removeIndexes;

	  for(unsigned int i = 1; i < thePrecipitationTrends.size(); i++)
		{
		  NFmiTime previousPeriodEndTime(thePrecipitationTrends[i-1].first.localEndTime());
		  NFmiTime currentPeriodStartTime(thePrecipitationTrends[i].first.localStartTime());
		  trend_id previousPeriodTrendId(thePrecipitationTrends[i-1].second);
		  trend_id currentPeriodTrendId(thePrecipitationTrends[i].second);
		  if(abs(previousPeriodEndTime.DifferenceInHours(currentPeriodStartTime)) <= 2 &&
			 previousPeriodTrendId == SADE_ALKAA && currentPeriodTrendId == POUTAANTUU)
			removeIndexes.push_back(i-1);
		}
	  removeRedundantTrends(thePrecipitationTrends, removeIndexes);
	  removeDuplicatePrecipitationTrends(thePrecipitationTrends);
	}

	void printOutPrecipitationLocation(std::ostream& theOutput) const
	{
	  theOutput << "Precipitation locations: " << endl; 
	  if(thePrecipitationTrendsCoastal.size() > 0)
		{
		  theOutput << "Coastal locations: " << endl; 
		  printOutPrecipitationLocation(theOutput, theCoastalData);
		}
	  if(thePrecipitationTrendsInland.size() > 0)
		{
		  theOutput << "Inland locations: " << endl; 
		  printOutPrecipitationLocation(theOutput, theInlandData);
		}
	  if(thePrecipitationTrendsFull.size() > 0)
		{
		  theOutput << "Full area location: " << endl; 
		  printOutPrecipitationLocation(theOutput, theFullData);
		}
	}

	void printOutPrecipitationLocation(std::ostream& theOutput, const precipitation_data_vector& theDataVector) const
	{
	  for(unsigned int i = 0; i < theDataVector.size(); i++)
		{
		  theOutput << "location(ne,se,sw,nw,n,s,e,w): " <<
			theDataVector[i]->theObervationTime << ", " <<
			theDataVector[i]->thePrecipitationPercentageNorthEast << ", " <<
			theDataVector[i]->thePrecipitationPercentageSouthEast << ", " <<
			theDataVector[i]->thePrecipitationPercentageSouthWest << ", " <<
			theDataVector[i]->thePrecipitationPercentageNorthWest << ", " <<
			theDataVector[i]->precipitationPercentageNorth() << ", " <<
			theDataVector[i]->precipitationPercentageSouth() << ", " <<
			theDataVector[i]->precipitationPercentageEast() << ", " <<
			theDataVector[i]->precipitationPercentageWest() << endl;
		  /*
		  theOutput << "northeast: " << theDataVector[i]->thePrecipitationPercentageNorthEast << endl;
		  theOutput << "southeast: " << theDataVector[i]->thePrecipitationPercentageSouthEast << endl;
		  theOutput << "southwest: " << theDataVector[i]->thePrecipitationPercentageSouthWest << endl;
		  theOutput << "northwest: " << theDataVector[i]->thePrecipitationPercentageNorthWest << endl;
		  theOutput << "north: " << theDataVector[i]->precipitationPercentageNorth() << endl;
		  theOutput << "south: " << theDataVector[i]->precipitationPercentageSouth() << endl;
		  theOutput << "east: " << theDataVector[i]->precipitationPercentageEast() << endl;
		  theOutput << "west: " << theDataVector[i]->precipitationPercentageWest() << endl;
		  */
		}
	}	

	void printOutPrecipitationTrends(std::ostream& theOutput) const
	{
	  theOutput << "Precipitation trends: " << endl; 
	  bool isTrends = false;
	  if(thePrecipitationTrendsCoastal.size() > 0)
		{
		  theOutput << "Coastal trends: " << endl; 
		  printOutPrecipitationTrends(theOutput, thePrecipitationTrendsCoastal);
		  isTrends = true;
		}
	  if(thePrecipitationTrendsInland.size() > 0)
		{
		  theOutput << "Inland trends: " << endl; 
		  printOutPrecipitationTrends(theOutput, thePrecipitationTrendsInland);
		  isTrends = true;
		}
	  if(thePrecipitationTrendsFull.size() > 0)
		{
		  theOutput << "Full area trends: " << endl; 
		  printOutPrecipitationTrends(theOutput, thePrecipitationTrendsFull);
		  isTrends = true;
		}

	  if(!isTrends)
		theOutput << "No trends!" << endl; 
	}

	void printOutPrecipitationTrends(std::ostream& theOutput, const trend_id_vector& thePrecipitationTrends) const
	{
	  for(unsigned int i = 0; i < thePrecipitationTrends.size(); i++)
		{
		  WeatherPeriod period(thePrecipitationTrends.at(i).first.localStartTime(),
							   thePrecipitationTrends.at(i).first.localEndTime());
		  trend_id trid(thePrecipitationTrends.at(i).second);
		  theOutput << period.localStartTime()
					<< "..."
					<< period.localEndTime()
					<< ": "
					<< trend_string(trid)
					<< endl;
		}
	}

	void printOutPrecipitationPeriods(std::ostream& theOutput) const
	{
	  theOutput << "Precipitation periods: " << endl; 
	  bool found = false;
	  if(thePrecipitationPeriodsCoastal.size() > 0)
		{
		  theOutput << "Coastal precipitation periods: " << endl; 
		  printOutPrecipitationPeriods(theOutput, thePrecipitationPeriodsCoastal);
		  found = true;
		}
	  if(thePrecipitationPeriodsInland.size() > 0)
		{
		  theOutput << "Inland precipitation periods: " << endl; 
		  printOutPrecipitationPeriods(theOutput, thePrecipitationPeriodsInland);
		  found = true;
		}
	  if(thePrecipitationPeriodsFull.size() > 0)
		{
		  theOutput << "Full precipitation periods: " << endl; 
		  printOutPrecipitationPeriods(theOutput, thePrecipitationPeriodsFull);
		  found = true;
		}

	  if(!found)
		theOutput << "No precipitation periods!" << endl; 
	}



	void printOutPrecipitationPeriods(std::ostream& theOutput,
									  const vector<WeatherPeriod>& thePrecipitationPeriods) const
	{
	  for(unsigned int i = 0; i < thePrecipitationPeriods.size(); i++)
		{
		  WeatherPeriod period(thePrecipitationPeriods.at(i).localStartTime(),
							   thePrecipitationPeriods.at(i).localEndTime());
		  theOutput << period.localStartTime()
					<< "..."
					<< period.localEndTime()
					<< endl;
		}
	}

	Sentence precipitationChangeSentence(wf_story_params& theParameters, const WeatherPeriod& thePeriod) const
	{
	  Sentence sentence;

	  for(unsigned int i = 0; i < thePrecipitationTrendsFull.size(); i++)
		{
		  WeatherPeriod period(thePrecipitationTrendsFull.at(i).first.localStartTime(),
							   thePrecipitationTrendsFull.at(i).first.localEndTime());
		  trend_id trid(thePrecipitationTrendsFull.at(i).second);

		  if(trid == POUTAANTUU)
			sentence << SAA_POUTAANTUU_PHRASE;
		  else
			sentence << SADE_WORD << ALKAA_WORD;

		  sentence << get_time_phrase(thePeriod);
		}	  

	  return sentence;
	}


	/*
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber || thePeriodNumber < 2)
	  return sentence;

	precipitation_data_item_container* theCurrentResultData = theParameters.thePrecipitationData[thePeriodNumber-1];
	precipitation_data_item_container* thePreviousResultData = theParameters.thePrecipitationData[thePeriodNumber-2];

	if(generator.isday(thePeriodNumber))
	  {
		// night data
		const PrecipitationDataItemData* nightPrecipitationPreviousFull = 
		  thePreviousResultData->find(YO) != thePreviousResultData->end() ? 
		  ((*thePreviousResultData)[YO])->theFullData : 0;
		const PrecipitationDataItemData* nightPrecipitationCurrentFull = 
		  theCurrentResultData->find(YO) != theCurrentResultData->end() ? 
		  ((*theCurrentResultData)[YO])->theFullData : 0;
		const PrecipitationDataItemData* nightPrecipitationPreviousInland = 
		  thePreviousResultData->find(YO) != thePreviousResultData->end() ? 
		  ((*thePreviousResultData)[YO])->theInlandData : 0;
		const PrecipitationDataItemData* nightPrecipitationCurrentInland = 
		  theCurrentResultData->find(YO) != theCurrentResultData->end() ? 
		  ((*theCurrentResultData)[YO])->theInlandData : 0;
		const PrecipitationDataItemData* nightPrecipitationPreviousCoastal = 
		  thePreviousResultData->find(YO) != thePreviousResultData->end() ? 
		  ((*thePreviousResultData)[YO])->theCoastalData : 0;
		const PrecipitationDataItemData* nightPrecipitationCurrentCoastal = 
		  theCurrentResultData->find(YO) != theCurrentResultData->end() ? 
		  ((*theCurrentResultData)[YO])->theCoastalData : 0;
		// morning
		// afternoon
	  }

*/
	/*
	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber || thePeriodNumber < 2)
	  return sentence;

	precipitation_data_item_container* theCurrentResultData = theParameters.thePrecipitationData[thePeriodNumber-1];
	precipitation_data_item_container* thePreviousResultData = theParameters.thePrecipitationData[thePeriodNumber-2];

	if(generator.isday(thePeriodNumber))
	  {
		PrecipitationDataItem* nightPrecipitation = ((*thePreviousResultData)[YO]);
		PrecipitationDataItem* morningPrecipitation = ((*theCurrentResultData)[AAMUPAIVA]);
		PrecipitationDataItem* afternoonPrecipitation = ((*theCurrentResultData)[ILTAPAIVA]);

		bool dryWeatherNight = is_dry_weather(theParameters,
											  nightPrecipitation->thePrecipitationForm,
											  nightPrecipitation->thePrecipitationIntensity,
											  nightPrecipitation->thePrecipitationExtent);
		bool dryWeatherMorning = is_dry_weather(theParameters,
												morningPrecipitation->thePrecipitationForm,
												morningPrecipitation->thePrecipitationIntensity,
												morningPrecipitation->thePrecipitationExtent);
		bool dryWeatherAfternoon = is_dry_weather(theParameters,
												  afternoonPrecipitation->thePrecipitationForm,
												  afternoonPrecipitation->thePrecipitationIntensity,
												  afternoonPrecipitation->thePrecipitationExtent);
				
		if(!dryWeatherNight && morningPrecipitation->theTrendId == POUTAANTUU && dryWeatherAfternoon)
		  {// compare morning to the previous night
			  sentence << AAMUPAIVALLA_WORD << SAA_POUTAANTUU_PHRASE;
		  }
		else if(dryWeatherNight && morningPrecipitation->theTrendId == SADE_ALKAA && !dryWeatherAfternoon)
		  { // compare morning to the previous night
			sentence << SADE_WORD << ALKAA_WORD << AAMUPAIVALLA_WORD;
		  }
		else if(!dryWeatherMorning && afternoonPrecipitation->theTrendId == POUTAANTUU)
		  {// compare afternoon to the morning
			  sentence << ILTAPAIVALLA_WORD << SAA_POUTAANTUU_PHRASE;
		  }
		else if(dryWeatherMorning && afternoonPrecipitation->theTrendId == SADE_ALKAA)
		  {// compare afternoon to the morning
			sentence << SADE_WORD << ALKAA_WORD  << ILTAPAIVALLA_WORD;
		  }
	  }
	else
	  {
		PrecipitationDataItem* nightPrecipitation = ((*theCurrentResultData)[YO]);
		PrecipitationDataItem* afternoonPrecipitation = ((*thePreviousResultData)[ILTAPAIVA]);

		//compare night to the previous afternoon
		bool dryWeatherAfternoon = is_dry_weather(theParameters,
												  afternoonPrecipitation->thePrecipitationForm,
												  afternoonPrecipitation->thePrecipitationIntensity,
												  afternoonPrecipitation->thePrecipitationExtent);

		if(!dryWeatherAfternoon && nightPrecipitation->theTrendId == POUTAANTUU)
			  sentence << YOLLA_WORD << SAA_POUTAANTUU_PHRASE;
	  }
	*/


	Sentence precipitationSentence(wf_story_params& theParameters, const WeatherPeriod& thePeriod) const
	{
	  Sentence sentence;
	  
	  const precipitation_data_vector* dataVector = 0;

	  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
		{
		  if(separateCoastInlandPrecipitation(thePeriod))
			{
			}
		  else
			{
			  dataVector = &theFullData;
			}
		}
	  else if(theParameters.theForecastArea & INLAND_AREA)
		{
		  dataVector = &theInlandData;
		}
	  else if(theParameters.theForecastArea & COASTAL_AREA)
		{
		  dataVector = &theCoastalData;
		}

	  if(dataVector)
		{
		  float precipitationIntensity = getMean(*dataVector, PRECIPITATION_DATA, thePeriod);
		  float precipitationExtent = getMean(*dataVector, PRECIPITATION_EXTENT_DATA, thePeriod);
		  float precipitationFormWater = getMean(*dataVector, PRECIPITATION_FORM_WATER_DATA, thePeriod);
		  float precipitationFormDrizzle = getMean(*dataVector, PRECIPITATION_FORM_DRIZZLE_DATA, thePeriod);
		  float precipitationFormSleet = getMean(*dataVector, PRECIPITATION_FORM_SLEET_DATA, thePeriod);
		  float precipitationFormSnow = getMean(*dataVector, PRECIPITATION_FORM_SNOW_DATA, thePeriod);
		  float precipitationFormFreezing = getMean(*dataVector, PRECIPITATION_FORM_FREEZING_DATA, thePeriod);
		  float precipitationTypeShowers = getMean(*dataVector, PRECIPITATION_TYPE_DATA, thePeriod);

		  unsigned int precipitationForm = get_complete_precipitation_form(theParameters.theVariable,
																		   precipitationFormWater,
																		   precipitationFormDrizzle,
																		   precipitationFormSleet,
																		   precipitationFormSnow,
																		   precipitationFormFreezing);
		  float dryWeatherLimit;
		  float weakPrecipitationLimit;

		  get_dry_and_weak_limit(theParameters, precipitationForm, dryWeatherLimit, weakPrecipitationLimit);

		  sentence << precipitation_sentence(theParameters,
											 precipitationForm,
											 precipitationIntensity,
											 precipitationExtent,
											 precipitationFormWater,
											 precipitationFormDrizzle,
											 precipitationFormSleet,
											 precipitationFormSnow,
											 precipitationFormFreezing,
											 precipitationTypeShowers);
		}

	  /*

	float precipitation_intesity = 0.0;
	float precipitation_extent = 0.0;
	float precipitation_form_water = 0.0;
	float precipitation_form_drizzle = 0.0;
	float precipitation_form_sleet = 0.0;
	float precipitation_form_snow = 0.0;
	float precipitation_form_freezing = 0.0;
	float precipitation_type_showers = 0.0;
	float pearson_coefficient = 0.0;
	trend_id precipitation_trend(NO_TREND);


	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		precipitation_intesity = get_mean(thePrecipitationFull);
		precipitation_extent = get_mean(thePrecipitationExtentFull);
		thePrecipitationTrendInputData = thePrecipitationFull;
		if(precipitation_extent > 0.0 && precipitation_intesity > ignore_drizzle_limit)
		  {
			precipitation_form_water = get_mean(thePrecipitationFromWaterFull);
			precipitation_form_drizzle = get_mean(thePrecipitationFromDrizzleFull);
			precipitation_form_sleet = get_mean(thePrecipitationFromSleetFull);
			precipitation_form_snow = get_mean(thePrecipitationFromSnowFull);
			precipitation_type_showers = get_mean(thePrecipitationTypeFull);
			float min_freezing = 0.0;
			float max_freezing = 0.0;
			get_min_max(thePrecipitationFromFreezingFull, min_freezing, max_freezing);
			precipitation_form_freezing = max_freezing;
			pearson_coefficient = get_pearson_coefficient(thePrecipitationFull);
 		  }
	  }


	const unsigned int precipitation_form = get_complete_precipitation_form(theParameters.theVariable,
														  precipitation_form_water,
														  precipitation_form_drizzle,
														  precipitation_form_sleet,
														  precipitation_form_snow,
														  precipitation_form_freezing);

	// TODO: if its raining save the location (north, east,...)

	// examine the trend
	float dryWeatherLimit;
	float weakPrecipitationLimit;
	float precipitationAtStart = thePrecipitation.at(0)->theResult.value();
	float precipitationAtEnd = thePrecipitation.at(thePrecipitation.size()-1)->theResult.value();

	get_dry_and_weak_limit(theParameters, precipitation_form, dryWeatherLimit, weakPrecipitationLimit);




  Sentence precipitation_sentence(wf_story_params& theParameters,
								  const unsigned int& thePrecipitationForm,
								  const float thePrecipitationIntensity,
								  const float thePrecipitationExtent,
								  const float thePrecipitationFormWater,
								  const float thePrecipitationFormDrizzle,
								  const float thePrecipitationFormSleet,
								  const float thePrecipitationFormSnow,
								  const float thePrecipitationFormFreezing,
								  const float thePrecipitationTypeShowers)

	  */

	  /*
	  if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
		{

		cloudiness_id coastalCloudinessId = getCloudinessId(thePeriod, theCoastalData);
		cloudiness_id inlandCloudinessId = getCloudinessId(thePeriod, theInlandData);
		cloudiness_id fullAreaCloudinessId = getCloudinessId(thePeriod, theFullData);
		
		if(separateCoastInlandCloudiness(thePeriod))
		  {
			sentence << COAST_PHRASE
					 << cloudiness_sentence(coastalCloudinessId);
			sentence << Delimiter(",");
			sentence << INLAND_PHRASE;
			sentence << cloudiness_sentence(inlandCloudinessId, true);
		  }
		else
		  {
			sentence << cloudiness_sentence(fullAreaCloudinessId);
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		cloudiness_id inlandCloudinessId = getCloudinessId(thePeriod, theInlandData);

		sentence << cloudiness_sentence(inlandCloudinessId);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		cloudiness_id coastalCloudinessId = getCloudinessId(thePeriod, theCoastalData);

		sentence << cloudiness_sentence(coastalCloudinessId);
	  }

	  */
  
	return sentence;
  }



	PrecipitationDataView(wf_story_params& parameters) :theParameters(parameters)
	{
	  gatherPrecipitationData(theParameters);
	  findOutPrecipitationPeriods();
	  findOutPrecipitationTrends();
	}

	~PrecipitationDataView()
	{
	  theCoastalData.clear();
	  theInlandData.clear();
	  theFullData.clear();
	}
  private:
	/*
	const weather_result_data_item_vector* theCoastalData;
	const weather_result_data_item_vector* theInlandData;
	const weather_result_data_item_vector* theFullData;
	*/

	precipitation_data_vector theCoastalData;
    precipitation_data_vector theInlandData;
    precipitation_data_vector theFullData;

	trend_id_vector thePrecipitationTrendsCoastal;
	trend_id_vector thePrecipitationTrendsInland;
	trend_id_vector thePrecipitationTrendsFull;

	vector<WeatherPeriod> thePrecipitationPeriodsCoastal;
	vector<WeatherPeriod> thePrecipitationPeriodsInland;
	vector<WeatherPeriod> thePrecipitationPeriodsFull;

	wf_story_params& theParameters;

  };



















  typedef std::pair<WeatherPeriod, cloudiness_id> weather_period_cloudiness_id_pair;
  typedef vector<weather_period_cloudiness_id_pair> cloudiness_period_vector;

  class CloudinessDataView
  {
  public:

	CloudinessDataView() :theCoastalData(0),
						  theInlandData(0),
						  theFullData(0)
	{}

	CloudinessDataView(const weather_result_data_item_vector* coastalData,
					   const weather_result_data_item_vector* inlandData,
					   const weather_result_data_item_vector* fullData) :
	  theCoastalData(coastalData),
	  theInlandData(inlandData),
	  theFullData(fullData)
	{ 
	  findOutCloudinessPeriods(); 
	  joinPeriods();
	  findOutCloudinessTrends();
	}

	~CloudinessDataView(){}
	
	void setCoastalData(const weather_result_data_item_vector* coastalData) 
	{ 
	  theCoastalData = coastalData;
	  findOutCloudinessPeriods(theCoastalData, theCloudinessPeriodsCoastal);
	  joinPeriods(theCoastalData, theCloudinessPeriodsCoastal, theCloudinessPeriodsCoastalJoined);
	  findOutCloudinessTrends(theCoastalData, theCloudinessTrendsCoastal);
	}
	void setInlandData(const weather_result_data_item_vector* inlandData) 
	{ 
	  theInlandData = inlandData; 
	  findOutCloudinessPeriods(theInlandData, theCloudinessPeriodsInland);
	  joinPeriods(theInlandData, theCloudinessPeriodsInland, theCloudinessPeriodsInlandJoined);
	  findOutCloudinessTrends(theInlandData, theCloudinessTrendsInland);
	}
	void setFullData(const weather_result_data_item_vector* fullData) 
	{ 
	  theFullData = fullData; 
	  findOutCloudinessPeriods(theFullData, theCloudinessPeriodsFull);
	  joinPeriods(theFullData, theCloudinessPeriodsFull, theCloudinessPeriodsFullJoined);	
	  findOutCloudinessTrends(theFullData, theCloudinessTrendsFull);
	}

	const weather_result_data_item_vector* getCoastalData() const { return theCoastalData; }
	const weather_result_data_item_vector* getInlandData() const { return theInlandData; }
	const weather_result_data_item_vector* getFullData() const { return theFullData; }

	Sentence cloudinessSentence(wf_story_params& theParameters, const WeatherPeriod& thePeriod) const;
	Sentence cloudinessSentence(const WeatherPeriod& thePeriod, 
								const weather_result_data_item_vector& theCloudinessData) const;
	Sentence cloudinessChangeSentence(wf_story_params& theParameters, 
									  const WeatherPeriod& theWeatherPeriod) const;

	Sentence cloudinessSentence(const WeatherPeriod& thePeriod) const;//, const unsigned int& thePeriodNumber);
	Sentence cloudinessSentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber);
	cloudiness_id getCloudinessId(const WeatherPeriod& thePeriod,
								  const weather_result_data_item_vector* theCloudinessData) const;

	void printOutCloudinessTrends(std::ostream& theOutput) const;
	void printOutCloudinessPeriods(std::ostream & theOutput) const;

  private:
	
	bool separateCoastInlandCloudiness(const WeatherPeriod& theWeatherPeriod) const;
	// e.g. separate morning afternoon
	bool separateWeatherPeriodCloudiness(const WeatherPeriod& theWeatherPeriod1, 
										 const WeatherPeriod& theWeatherPeriod2,
										 const weather_result_data_item_vector& theCloudinessData) const;

	void getWeatherPeriodCloudiness(const WeatherPeriod& thePeriod,
									const cloudiness_period_vector& theSourceCloudinessPeriods,
									cloudiness_period_vector& theWeatherPeriodCloudiness) const;
	  
	void printOutCloudinessTrends(std::ostream& theOutput, const trend_id_vector& theCloudinessTrends) const;

	void printOutCloudinessPeriods(std::ostream& theOutput, const cloudiness_period_vector& theCloudinessPeriods) const;

	void findOutCloudinessPeriods(const weather_result_data_item_vector* theData, 
								 cloudiness_period_vector& theCloudinessPeriods);
	void findOutCloudinessPeriods();
	void findOutCloudinessTrends(const weather_result_data_item_vector* theData,
								 trend_id_vector& theCloudinessTrends);
	void findOutCloudinessTrends();

	void joinPeriods(const weather_result_data_item_vector* theDataSource,
					 const cloudiness_period_vector& theCloudinessPeriodsSource,
					 cloudiness_period_vector& theCloudinessPeriodsDestination);
	void joinPeriods();

	const weather_result_data_item_vector* theCoastalData;
	const weather_result_data_item_vector* theInlandData;
	const weather_result_data_item_vector* theFullData;

	cloudiness_period_vector theCloudinessPeriodsCoastal;
	cloudiness_period_vector theCloudinessPeriodsInland;
	cloudiness_period_vector theCloudinessPeriodsFull;

	cloudiness_period_vector theCloudinessPeriodsCoastalJoined;
	cloudiness_period_vector theCloudinessPeriodsInlandJoined;
	cloudiness_period_vector theCloudinessPeriodsFullJoined;

	trend_id_vector theCloudinessTrendsCoastal;
	trend_id_vector theCloudinessTrendsInland;
	trend_id_vector theCloudinessTrendsFull;

	bool theSeparateCoastInlandMorning;
	bool theSeparateCoastInlandAfternoon;
	bool theSeparateMorningAfternoon; // full area

  };


  Sentence CloudinessDataView::cloudinessChangeSentence(wf_story_params& theParameters, 
														const WeatherPeriod& theWeatherPeriod) const
  {
	Sentence sentence;

	for(unsigned int i = 0; i < theCloudinessTrendsFull.size(); i++)
	  {
		WeatherPeriod period(theCloudinessTrendsFull.at(i).first.localStartTime(),
							 theCloudinessTrendsFull.at(i).first.localEndTime());
		trend_id trid(theCloudinessTrendsFull.at(i).second);
		
		if(trid == PILVISTYY)
		  sentence << PILVISTYVAA_WORD;
		else
		  sentence << SELKENEVAA_WORD;

		sentence << get_time_phrase(period, true);
	  }

	return sentence;
  }


  bool CloudinessDataView::separateCoastInlandCloudiness(const WeatherPeriod& theWeatherPeriod) const
  {
	weather_result_data_item_vector theInterestingDataCoastal;
	weather_result_data_item_vector theInterestingDataInland;
	
	get_sub_time_series(theWeatherPeriod,
						*theCoastalData,
						theInterestingDataCoastal);

	float mean_coastal = get_mean(theInterestingDataCoastal);

	get_sub_time_series(theWeatherPeriod,
						*theInlandData,
						theInterestingDataInland);

	float mean_inland = get_mean(theInterestingDataInland);

	bool retval  = (mean_inland <= 5.0 && mean_coastal >= 70.0 ||
					mean_inland >= 70.0 && mean_coastal <= 5.0);

	return retval;
  }

  // TODO: onko OK
  bool CloudinessDataView::separateWeatherPeriodCloudiness(const WeatherPeriod& theWeatherPeriod1, 
														   const WeatherPeriod& theWeatherPeriod2,
														   const weather_result_data_item_vector& theCloudinessData) const
  {
	weather_result_data_item_vector theInterestingDataPeriod1;
	weather_result_data_item_vector theInterestingDataPeriod2;

	//		bool separateMorningAfternoon = (abs(theCloudinessIdFullMorning - theCloudinessIdFullAfternoon) >= 2);

	get_sub_time_series(theWeatherPeriod1,
						theCloudinessData,
						theInterestingDataPeriod1);

	cloudiness_id cloudinessId1 = get_cloudiness_id(get_mean(theInterestingDataPeriod1));
	
	//	float mean_period1 = get_mean(theInterestingDataPeriod1);
	
	get_sub_time_series(theWeatherPeriod2,
						theCloudinessData,
						theInterestingDataPeriod2);
	
	cloudiness_id cloudinessId2 = get_cloudiness_id(get_mean(theInterestingDataPeriod2));
	//	float mean_period2 = get_mean(theInterestingDataPeriod2);

	return (abs(cloudinessId1 - cloudinessId2) >= 2);

	/*													
	bool retval  = (mean_period1 <= 5.0 && mean_period2 >= 70.0 ||
					mean_period1 >= 70.0 && mean_period2 <= 5.0);
	
	return retval;
	*/
  }  
  
  void CloudinessDataView::findOutCloudinessTrends(const weather_result_data_item_vector* theData,
												   trend_id_vector& theCloudinessTrends)
  {
	// check 4-hour periods if there is trend
	const unsigned int periodLength = 4;
	unsigned int startIndex = 0;
	unsigned int endIndex = periodLength - 1;
	while(startIndex < theData->size() && endIndex < theData->size())
	  {
		WeatherPeriod period(theData->at(startIndex)->thePeriod.localStartTime(),
							 theData->at(endIndex)->thePeriod.localEndTime());
		weather_result_data_item_vector theInterestingData;
		
		get_sub_time_series(period,
							*theData,
							theInterestingData);
		
		/*
		float min, max;
		float mean = get_mean(theInterestingData);
		get_min_max(theInterestingData, min, max);
		float standard_deviation = get_standard_deviation(theInterestingData);	
		theCloudinessId = get_cloudiness_id(min, mean, max, standard_deviation);
		*/
		float pearson_coefficient = get_pearson_coefficient(theInterestingData);
		cloudiness_id cloudinessAtStart = 
		  get_cloudiness_id(theInterestingData[0]->theResult.value());
		cloudiness_id cloudinessAtEnd = 
		  get_cloudiness_id(theInterestingData[theInterestingData.size()-1]->theResult.value());

		bool dataOk = (cloudinessAtStart != kFloatMissing && cloudinessAtEnd != kFloatMissing);
		
		trend_id trendId = NO_TREND;

		if(dataOk && cloudinessAtStart >= VERRATTAIN_PILVISTA && 
		   cloudinessAtEnd == SELKEAA && pearson_coefficient < -0.65)
		  {
			trendId = SELKENEE;
		  }
		else if(dataOk && cloudinessAtStart <= MELKO_SELKEAA && 
				cloudinessAtEnd >= VERRATTAIN_PILVISTA && pearson_coefficient >= 0.65)
		  {
			trendId = PILVISTYY;
		  }

		if(trendId != NO_TREND)
		  {
			theCloudinessTrends.push_back(make_pair(period, trendId));			
		  }

		startIndex += (trendId == NO_TREND ? 1 : periodLength);
		endIndex += (trendId == NO_TREND ? 1 : periodLength);
	  }
 }

  void CloudinessDataView::findOutCloudinessTrends()
  {
	if(theCoastalData)
	  findOutCloudinessTrends(theCoastalData, theCloudinessTrendsCoastal);
	if(theInlandData)
	  findOutCloudinessTrends(theInlandData, theCloudinessTrendsInland);
	if(theFullData)
	  findOutCloudinessTrends(theFullData, theCloudinessTrendsFull);
  }

  void CloudinessDataView::findOutCloudinessPeriods(const weather_result_data_item_vector* theData, 
													cloudiness_period_vector& theCloudinessPeriods)
  {
	if(theData)
	  {
		NFmiTime previousStartTime;
		NFmiTime previousEndTime;
		cloudiness_id previousCloudinessId;
		for(unsigned int i = 0; i < theData->size(); i++)
		  {
			if(i== 0)
			  {
				previousStartTime = theData->at(i)->thePeriod.localStartTime();
				previousEndTime = theData->at(i)->thePeriod.localEndTime();
				previousCloudinessId = get_cloudiness_id(theData->at(i)->theResult.value());
			  }
			else
			  {
				if(previousCloudinessId != get_cloudiness_id(theData->at(i)->theResult.value()))
				  {
					pair<WeatherPeriod, cloudiness_id> item = 
					  make_pair(WeatherPeriod(previousStartTime, previousEndTime), previousCloudinessId);
					theCloudinessPeriods.push_back(item);
					previousStartTime = theData->at(i)->thePeriod.localStartTime();
					previousEndTime = theData->at(i)->thePeriod.localEndTime();
					previousCloudinessId = get_cloudiness_id(theData->at(i)->theResult.value());

				  }
				else if(i == theData->size() - 1)
				  {
					pair<WeatherPeriod, cloudiness_id> item = 
					  make_pair(WeatherPeriod(previousStartTime, theData->at(i)->thePeriod.localEndTime()), previousCloudinessId );
					theCloudinessPeriods.push_back(item);

				  }
				else
				  {
					previousEndTime = theData->at(i)->thePeriod.localEndTime();
				  }
			  }
		  }
	  }
  }

  void CloudinessDataView::findOutCloudinessPeriods()
  {
	findOutCloudinessPeriods(theCoastalData, theCloudinessPeriodsCoastal);
	findOutCloudinessPeriods(theInlandData, theCloudinessPeriodsInland);
	findOutCloudinessPeriods(theFullData, theCloudinessPeriodsFull);
  }

  void joinPuolipilvisestaPilviseen(const weather_result_data_item_vector* theData,
									vector<int>& theCloudinessPuolipilvisestaPilviseen)
  {
	if(!theData)
	  return;

	int index = -1;
	for(unsigned int i = 0; i < theData->size(); i++)
	  {
		if(get_cloudiness_id(theData->at(i)->theResult.value()) >= PUOLIPILVISTA && 
		   get_cloudiness_id(theData->at(i)->theResult.value()) <= PILVISTA &&
		   i != theData->size() - 1)
		  {
			if(index == -1)
			  index = i;
		  }
		else if(get_cloudiness_id(theData->at(i)->theResult.value()) == SELKEAA || 
				get_cloudiness_id(theData->at(i)->theResult.value()) == MELKO_SELKEAA ||
				get_cloudiness_id(theData->at(i)->theResult.value()) == PILVISTA ||
				i == theData->size() - 1)
		  {
			if(index != -1 && i-1-index > 1)
			  {
				for(int k = index; k < static_cast<int>(i); k++)
				  {
					WeatherPeriod period(theData->at(k)->thePeriod.localStartTime(),
										 theData->at(k)->thePeriod.localEndTime());

					theCloudinessPuolipilvisestaPilviseen.push_back(k);
					//	cout << "XXX puolipilvisestä pilviseen period: " << k << "  " << period.localStartTime() << "..." << period.localEndTime() << endl;


					/*
		WeatherPeriod period(theCloudinessPeriods.at(i).first.localStartTime(),
							 theCloudinessPeriods.at(i).first.localEndTime());
		cloudiness_id clid(theCloudinessPeriods.at(i).second);
		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
				  << ": "
				  << cloudiness_string(clid)
				  << endl;

					 */

				  }
				// join
				/*
				WeatherPeriod period(theData->at(index).thePeriod.localStartTime(),
									 theData->at(i-1).thePeriod.localEndTime());
				weather_result_data_item_vector theInterestingData;

				get_sub_time_series(period,
									theData,
									theInterestingData);
				
				float min, max;
				float mean = get_mean(theInterestingData);
				get_min_max(theInterestingData, min, max);
				float standard_deviation = get_standard_deviation(mean, theInterestingData);	
				theCloudinessId = get_cloudiness_id(min, mean, max, standard_deviation);
		pearson_coefficient = get_pearson_coefficient(theInterestingData);
				*/
				
			  }
			index = -1;
		  }
	  }
  }
	/*
  enum cloudiness_id
	{
	  SELKEAA,
	  MELKO_SELKEAA,
	  PUOLIPILVISTA,
	  VERRATTAIN_PILVISTA,
	  PILVISTA,
	  PUOLIPILVISTA_JA_PILVISTA,
	  MISSING_CLOUDINESS_ID
	};

	 */
  void CloudinessDataView::joinPeriods(const weather_result_data_item_vector* theCloudinessDataSource,
									   const cloudiness_period_vector& theCloudinessPeriodsSource,
									   cloudiness_period_vector& theCloudinessPeriodsDestination)
  {
	if(theCloudinessPeriodsSource.size() == 0)
	  return;

	vector<int> theCloudinessPuolipilvisestaPilviseen;


	int periodStartIndex = 0;
	cloudiness_id clidPrevious(theCloudinessPeriodsSource.at(0).second);
	cloudiness_id clidCurrent(clidPrevious);

	for(unsigned int i = 1; i < theCloudinessPeriodsSource.size(); i++)
	  {
		bool lastPeriod = (i == theCloudinessPeriodsSource.size() - 1);
		clidCurrent = theCloudinessPeriodsSource.at(i).second;

		if(abs(clidPrevious - clidCurrent) >= 2 || lastPeriod)
		  {
			// check if "puolipilvisestä pilviseen"
			if(clidPrevious == PUOLIPILVISTA && clidCurrent == PILVISTA || 
			   clidPrevious == PILVISTA && clidCurrent == PUOLIPILVISTA)
			  {
			  }

			NFmiTime startTime(theCloudinessPeriodsSource.at(periodStartIndex).first.localStartTime());
			NFmiTime endTime(theCloudinessPeriodsSource.at(lastPeriod ? i : i-1).first.localEndTime());

			weather_result_data_item_vector thePeriodCloudiness;

			get_sub_time_series(WeatherPeriod(startTime, endTime),
								*theCloudinessDataSource,
								thePeriodCloudiness);

			float min, max, mean, stddev;

			get_min_max(thePeriodCloudiness, min, max);
			mean = get_mean(thePeriodCloudiness);
			stddev = get_standard_deviation(thePeriodCloudiness);

			
			cloudiness_id actual_clid = get_cloudiness_id(min, mean, max, stddev);

			  
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), actual_clid);
//lidPrevious);
			//	cout << "period: " << startTime << ", " << endTime  << endl;
			//cout << "cloudiness_id: " << static_cast<int>(clidPrevious) << endl;

			theCloudinessPeriodsDestination.push_back(item);

			clidPrevious = clidCurrent;
			periodStartIndex = i;
		  }
	  }	
  }

  void CloudinessDataView::joinPeriods()
  {
	joinPeriods(theCoastalData, theCloudinessPeriodsCoastal, theCloudinessPeriodsCoastalJoined);
	joinPeriods(theInlandData, theCloudinessPeriodsInland, theCloudinessPeriodsInlandJoined);
	joinPeriods(theFullData, theCloudinessPeriodsFull, theCloudinessPeriodsFullJoined);	
  }

  void CloudinessDataView::printOutCloudinessPeriods(std::ostream& theOutput, 
													 const cloudiness_period_vector& theCloudinessPeriods) const
  {
	for(unsigned int i = 0; i < theCloudinessPeriods.size(); i++)
	  {
		WeatherPeriod period(theCloudinessPeriods.at(i).first.localStartTime(),
							 theCloudinessPeriods.at(i).first.localEndTime());
		cloudiness_id clid(theCloudinessPeriods.at(i).second);
		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
				  << ": "
				  << cloudiness_string(clid)
				  << endl;
	  }
  }
  
  void CloudinessDataView::printOutCloudinessTrends(std::ostream& theOutput, 
													const trend_id_vector& theCloudinessTrends) const
  {
	for(unsigned int i = 0; i < theCloudinessTrends.size(); i++)
	  {
		WeatherPeriod period(theCloudinessTrends.at(i).first.localStartTime(),
							 theCloudinessTrends.at(i).first.localEndTime());
		trend_id trid(theCloudinessTrends.at(i).second);
		theOutput << period.localStartTime()
				  << "..."
				  << period.localEndTime()
				  << ": "
				  << trend_string(trid)
				  << endl;
	  }
  }

  void CloudinessDataView::printOutCloudinessTrends(std::ostream& theOutput) const
  {
	theOutput << "Cloudiness trends: " << endl; 
	bool isTrends = false;
	if(theCloudinessTrendsCoastal.size() > 0)
	  {
		theOutput << "Coastal trends: " << endl; 
		printOutCloudinessTrends(theOutput, theCloudinessTrendsCoastal);
		isTrends = true;
	  }
	if(theCloudinessTrendsInland.size() > 0)
	  {
		theOutput << "Inland trends: " << endl; 
		printOutCloudinessTrends(theOutput, theCloudinessTrendsInland);
		isTrends = true;
	  }
	if(theCloudinessTrendsFull.size() > 0)
	  {
		theOutput << "Full area trends: " << endl; 
		printOutCloudinessTrends(theOutput, theCloudinessTrendsFull);
		isTrends = true;
	  }

	if(!isTrends)
		theOutput << "No trends!" << endl; 
  }

  void CloudinessDataView::printOutCloudinessPeriods(std::ostream& theOutput) const
  {
	if(theCoastalData)
	  {
	  theOutput << "Coastal cloudiness: " << endl; 
	  printOutCloudinessPeriods(theOutput, theCloudinessPeriodsCoastal);
	  theOutput << "Coastal cloudiness joined: " << endl; 
	  printOutCloudinessPeriods(theOutput, theCloudinessPeriodsCoastalJoined);
	  vector<int> theCloudinessPuolipilvisestaPilviseen;
	  joinPuolipilvisestaPilviseen(theCoastalData, theCloudinessPuolipilvisestaPilviseen);
	  }
	if(theInlandData)
	  {
		theOutput << "Inland cloudiness: " << endl; 
		printOutCloudinessPeriods(theOutput, theCloudinessPeriodsInland);
		theOutput << "Inland cloudiness joined: " << endl; 
		printOutCloudinessPeriods(theOutput, theCloudinessPeriodsInlandJoined);
		vector<int> theCloudinessPuolipilvisestaPilviseen;
		joinPuolipilvisestaPilviseen(theInlandData, theCloudinessPuolipilvisestaPilviseen);
	  }
	if(theFullData)
	  {
		theOutput << "Full area cloudiness: " << endl; 
		printOutCloudinessPeriods(theOutput, theCloudinessPeriodsFull);
		theOutput << "Full area cloudiness joined: " << endl; 
		printOutCloudinessPeriods(theOutput, theCloudinessPeriodsFullJoined);
		vector<int> theCloudinessPuolipilvisestaPilviseen;
		joinPuolipilvisestaPilviseen(theFullData, theCloudinessPuolipilvisestaPilviseen);
	  }
  }

  void CloudinessDataView::getWeatherPeriodCloudiness(const WeatherPeriod& thePeriod,
													  const cloudiness_period_vector& theSourceCloudinessPeriods,
													  cloudiness_period_vector& theWeatherPeriodCloudiness) const
  {
	for(unsigned int i = 0; i < theSourceCloudinessPeriods.size(); i++)
	  {
		if(thePeriod.localStartTime() >= theSourceCloudinessPeriods.at(i).first.localStartTime() &&
		   thePeriod.localEndTime() <= theSourceCloudinessPeriods.at(i).first.localEndTime())
		  {
			NFmiTime startTime(thePeriod.localStartTime());
			NFmiTime endTime(thePeriod.localEndTime());
			cloudiness_id clid(theSourceCloudinessPeriods.at(i).second);
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
			theWeatherPeriodCloudiness.push_back(item);
		  }
		else if(thePeriod.localStartTime() >= theSourceCloudinessPeriods.at(i).first.localStartTime() &&
				thePeriod.localStartTime() < theSourceCloudinessPeriods.at(i).first.localEndTime() &&
				thePeriod.localEndTime() > theSourceCloudinessPeriods.at(i).first.localEndTime())
		  {
			NFmiTime startTime(thePeriod.localStartTime());
			NFmiTime endTime(theSourceCloudinessPeriods.at(i).first.localEndTime());
			cloudiness_id clid(theSourceCloudinessPeriods.at(i).second);
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
			theWeatherPeriodCloudiness.push_back(item);
		  }
		else if(thePeriod.localStartTime() < theSourceCloudinessPeriods.at(i).first.localStartTime() &&
				thePeriod.localEndTime() > theSourceCloudinessPeriods.at(i).first.localStartTime() &&
				thePeriod.localEndTime() <= theSourceCloudinessPeriods.at(i).first.localEndTime())
		  {
			NFmiTime startTime(theSourceCloudinessPeriods.at(i).first.localStartTime());
			NFmiTime endTime(thePeriod.localEndTime());
			cloudiness_id clid(theSourceCloudinessPeriods.at(i).second);
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
			theWeatherPeriodCloudiness.push_back(item);				
		  }
		else if(thePeriod.localStartTime() < theSourceCloudinessPeriods.at(i).first.localStartTime() &&
				thePeriod.localEndTime() > theSourceCloudinessPeriods.at(i).first.localEndTime())
		  {
			NFmiTime startTime(theSourceCloudinessPeriods.at(i).first.localStartTime());
			NFmiTime endTime( theSourceCloudinessPeriods.at(i).first.localEndTime());
			cloudiness_id clid(theSourceCloudinessPeriods.at(i).second);
			pair<WeatherPeriod, cloudiness_id> item = make_pair(WeatherPeriod(startTime, endTime), clid);
			theWeatherPeriodCloudiness.push_back(item);				
		  }
	  }
  }

  Sentence CloudinessDataView::cloudinessSentence(wf_story_params& theParameters, const WeatherPeriod& thePeriod) const
  {
	Sentence sentence;

	if(theParameters.theForecastArea & INLAND_AREA && theParameters.theForecastArea & COASTAL_AREA)
	  {
		cloudiness_id coastalCloudinessId = getCloudinessId(thePeriod, theCoastalData);
		cloudiness_id inlandCloudinessId = getCloudinessId(thePeriod, theInlandData);
		cloudiness_id fullAreaCloudinessId = getCloudinessId(thePeriod, theFullData);
		
		if(separateCoastInlandCloudiness(thePeriod))
		  {
			sentence << COAST_PHRASE
					 << cloudiness_sentence(coastalCloudinessId);
			sentence << Delimiter(",");
			sentence << INLAND_PHRASE;
			sentence << cloudiness_sentence(inlandCloudinessId, true);
		  }
		else
		  {
			sentence << cloudiness_sentence(fullAreaCloudinessId);
		  }
	  }
	else if(theParameters.theForecastArea & INLAND_AREA)
	  {
		cloudiness_id inlandCloudinessId = getCloudinessId(thePeriod, theInlandData);

		sentence << cloudiness_sentence(inlandCloudinessId);
	  }
	else if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		cloudiness_id coastalCloudinessId = getCloudinessId(thePeriod, theCoastalData);

		sentence << cloudiness_sentence(coastalCloudinessId);
	  }
  
	return sentence;
  }

  Sentence CloudinessDataView::cloudinessSentence(wf_story_params& theParameters, const unsigned int& thePeriodNumber)
  {
	Sentence sentence;

 	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	WeatherPeriod thePeriod(generator.period(thePeriodNumber));

	if(generator.size() < thePeriodNumber)
	   return sentence;

	if(generator.isday(thePeriodNumber))
	  {
		WeatherPeriod morningWeatherPeriod(thePeriod.localStartTime(),
										   thePeriod.localEndTime());
		WeatherPeriod afternoonWeatherPeriod(thePeriod.localStartTime(),
										   thePeriod.localEndTime());
		get_part_of_the_day(thePeriod, AAMUPAIVA, morningWeatherPeriod);
		get_part_of_the_day(thePeriod, ILTAPAIVA, afternoonWeatherPeriod);

		bool inlandAndCoastalIncluded = (theParameters.theForecastArea & INLAND_AREA && 
										 theParameters.theForecastArea & COASTAL_AREA);
		bool inlandIncluded = theParameters.theForecastArea & INLAND_AREA ;
		bool coastalIncluded = theParameters.theForecastArea & COASTAL_AREA;


		cloudiness_id cloudinessIdFullMorning(getCloudinessId(morningWeatherPeriod, theFullData));
		cloudiness_id cloudinessIdCoastalMorning(getCloudinessId(morningWeatherPeriod, theCoastalData));
		cloudiness_id cloudinessIdInlandMorning(getCloudinessId(morningWeatherPeriod, theInlandData));
		cloudiness_id cloudinessIdFullAfternoon(getCloudinessId(afternoonWeatherPeriod, theFullData));
		cloudiness_id cloudinessIdCoastalAfternoon(getCloudinessId(afternoonWeatherPeriod, theCoastalData));
		cloudiness_id cloudinessIdInlandAfternoon(getCloudinessId(afternoonWeatherPeriod, theInlandData));

		if(inlandAndCoastalIncluded)
		  {
			bool separateCoastInlandMorning = separateCoastInlandCloudiness(morningWeatherPeriod);
			bool separateCoastInlandAfternoon = separateCoastInlandCloudiness(afternoonWeatherPeriod);
			bool separateMorningAfternoon = separateWeatherPeriodCloudiness(morningWeatherPeriod, 
																			afternoonWeatherPeriod,
																			*theFullData);
			if(!separateCoastInlandMorning && !separateCoastInlandAfternoon && !separateMorningAfternoon)
			  {
				sentence << cloudiness_sentence(cloudinessIdFullMorning);
			  }
			else
			  {
				if(separateCoastInlandMorning)
				  {
					sentence << AAMUPAIVALLA_WORD << COAST_PHRASE
							 << cloudiness_sentence(cloudinessIdCoastalMorning);
					sentence << Delimiter(",");
					sentence << INLAND_PHRASE << cloudiness_sentence(cloudinessIdInlandMorning, true);
				  }
				else
				  {
					sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(cloudinessIdFullMorning);
				  }
				sentence << Delimiter(",");
				if(separateCoastInlandAfternoon)
				  {
					sentence << ILTAPAIVALLA_WORD << COAST_PHRASE
							 << cloudiness_sentence(cloudinessIdCoastalAfternoon);
					sentence << Delimiter(",");
					sentence << INLAND_PHRASE << cloudiness_sentence(cloudinessIdInlandAfternoon, true);
				  }
				else
				  {
					sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(cloudinessIdFullAfternoon);
				  }
			  }
		  }
		else if(inlandIncluded)
		  {
			if(puolipilvisesta_pilviseen(cloudinessIdInlandMorning,
										 cloudinessIdInlandAfternoon))
			  {
				sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
			  }
			else if(cloudinessIdInlandMorning != MISSING_CLOUDINESS_ID &&
					cloudinessIdInlandMorning != PUOLIPILVISTA_JA_PILVISTA && 
					cloudinessIdInlandAfternoon != PUOLIPILVISTA_JA_PILVISTA &&
					abs(cloudinessIdInlandMorning - cloudinessIdInlandAfternoon) < 2)
			  {
				sentence << cloudiness_sentence(cloudinessIdInlandMorning);
			  }
			else
			  {
				if(cloudinessIdInlandMorning != MISSING_CLOUDINESS_ID)
				  {
					sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(cloudinessIdInlandMorning);
					sentence << Delimiter(",");
					sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(cloudinessIdInlandAfternoon, true);
				  }
				else
				  {
					sentence << cloudiness_sentence(cloudinessIdInlandAfternoon);
				  }
			  }
		  }
		else if(coastalIncluded)
		  {
			if(puolipilvisesta_pilviseen(cloudinessIdCoastalMorning,
										 cloudinessIdCoastalAfternoon))
			  {
				sentence << SAA_VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE;
			  }
			else if(cloudinessIdCoastalMorning != MISSING_CLOUDINESS_ID &&
					cloudinessIdCoastalMorning != PUOLIPILVISTA_JA_PILVISTA && 
					cloudinessIdCoastalAfternoon != PUOLIPILVISTA_JA_PILVISTA &&
					abs(cloudinessIdCoastalMorning - cloudinessIdCoastalAfternoon) < 2)
			  {
				sentence << cloudiness_sentence(cloudinessIdCoastalMorning);
			  }
			else
			  {
				if(cloudinessIdCoastalMorning != MISSING_CLOUDINESS_ID)
				  {
					sentence << AAMUPAIVALLA_WORD << cloudiness_sentence(cloudinessIdCoastalMorning);
					sentence << Delimiter(",");
					sentence << ILTAPAIVALLA_WORD << cloudiness_sentence(cloudinessIdCoastalAfternoon, true);
				  }
				else
				  {
					sentence << cloudiness_sentence(cloudinessIdCoastalAfternoon);
				  }
			  }
		  }
	  }

	return sentence;
  }

  cloudiness_id CloudinessDataView::getCloudinessId(const WeatherPeriod& thePeriod,
													const weather_result_data_item_vector* theCloudinessData) const
  {
	if(!theCloudinessData)
	  return MISSING_CLOUDINESS_ID;

	weather_result_data_item_vector thePeriodCloudiness;
	
	get_sub_time_series(thePeriod,
						*theCloudinessData,
						thePeriodCloudiness);

	float min, max, mean, stddev;
	get_min_max(thePeriodCloudiness, min, max);
	mean = get_mean(thePeriodCloudiness);
	stddev = get_standard_deviation(thePeriodCloudiness);
	cloudiness_id clid = get_cloudiness_id(min, mean, max, stddev);

	return clid;
  }

  Sentence CloudinessDataView::cloudinessSentence(const WeatherPeriod& thePeriod,
												  const weather_result_data_item_vector& theCloudinessData) const
  {
	Sentence sentence;

	cloudiness_id clid(getCloudinessId(thePeriod, &theCloudinessData));

	sentence << cloudiness_string(clid);

	return sentence;
  }

  Sentence CloudinessDataView::cloudinessSentence(const WeatherPeriod& thePeriod) const
  {

	Sentence sentence;

	//	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	//if(generator.size() < thePeriodNumber)
	//return sentence;

	//	bool isDay = generator.isday(thePeriodNumber);


	//	const cloudiness_period_vector* cloudinessPeriods = 0;

	cloudiness_period_vector weatherPeriodCloudinessFull;
	cloudiness_period_vector weatherPeriodCloudinessInland;
	cloudiness_period_vector weatherPeriodCloudinessCoastal;

	if(theCoastalData)
	  {
		cout << "cloudiness sentence COASTAL: " << endl;
		getWeatherPeriodCloudiness(thePeriod, theCloudinessPeriodsCoastalJoined, weatherPeriodCloudinessCoastal);
		printOutCloudinessPeriods(cout, weatherPeriodCloudinessCoastal);
	  }
	if(theInlandData)
	  {
		cout << "cloudiness sentence INLAND: " << endl;
		getWeatherPeriodCloudiness(thePeriod, theCloudinessPeriodsInlandJoined, weatherPeriodCloudinessInland);
		printOutCloudinessPeriods(cout, weatherPeriodCloudinessInland);
	  }
	if(theFullData)
	  {
		cout << "cloudiness sentence FULL: " << endl;
		getWeatherPeriodCloudiness(thePeriod, theCloudinessPeriodsFullJoined, weatherPeriodCloudinessFull);
		printOutCloudinessPeriods(cout, weatherPeriodCloudinessFull);
	  }



	return sentence;
  }

  /*
  Sentence cloudiness_sentence(wf_story_params& theParameters, 
							   const unsigned int& thePeriodNumber, 
							   const CloudinessDataView& theCloudinessDataView)
  {
	Sentence sentence;

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	if(generator.size() < thePeriodNumber)
	  return sentence;

	Sentence cloudinessSentence;

	cloudiness_data_item_container* theResultData = theParameters.theCloudinessData[thePeriodNumber - 1];

	if(theResultData)
	  {
		if(generator.isnight(thePeriodNumber))
		  {
			if(theResultData->find(YO) != theResultData->end())
			  sentence << cloudiness_night_sentence(theParameters, *((*theResultData)[YO]));
		  }
		else
		  {
			CloudinessDataItem* morningItem = 0;
			CloudinessDataItem* afternoonItem = 0;
			if(theResultData->find(AAMUPAIVA) != theResultData->end())
			  morningItem = (*theResultData)[AAMUPAIVA];
			if(theResultData->find(ILTAPAIVA) != theResultData->end())
			  afternoonItem = (*theResultData)[ILTAPAIVA];
			sentence << cloudiness_day_sentence(theParameters, *morningItem, *afternoonItem);
		  }
	  }

	return sentence;
  }
  */







// ----------------------------------------------------------------------
  /*!populate_temperature_time_series
   * \brief Generate overview on weather
   *
   * \return The story
   *
   * \see page_weather_forecast
   *
   * \todo Much missing
   */
  // ----------------------------------------------------------------------

  const Paragraph WeatherStory::forecast() const
  {
	MessageLogger log("WeatherStory::forecast");

	using namespace PrecipitationPeriodTools;

	Paragraph paragraph;

	// Period generator
	NightAndDayPeriodGenerator generator00(itsPeriod, itsVar);

	if(generator00.size() == 0)
	  {
		log << "No weather periods available!" << endl;
		log << paragraph;
		return paragraph;
	  }

	/*
	RangeAcceptor waterfilter;
	waterfilter.lowerLimit(0);	// 0 = drizzle
	waterfilter.upperLimit(1);	// 1 = water
	//RangeAcceptor percentagelimits;
	//percentagelimits.lowerLimit(maxprecipitationlimit);
	ValueAcceptor drizzlefilter;
	drizzlefilter.value(0);	// 0 = drizzle
	ValueAcceptor sleetfilter;
	sleetfilter.value(2);	// 2 = sleet
	ValueAcceptor snowfilter;
	snowfilter.value(3);	// 3 = snow
	ValueAcceptor freezingfilter;
	freezingfilter.value(4);	// 4 = freezing
	ValueAcceptor showerfilter;
	showerfilter.value(2);	// 1=large scale, 2=showers

	for(unsigned int i = 0; i < precipitationHourly->size(); i++)
	  {
        (*precipitationHourly)[i]->theResult =
		  theForecaster.analyze(theVariable,
								theSources,
								Precipitation,
								Mean,
								Sum,
								theArea,
								(*precipitationHourly)[i]->thePeriod,
								DefaultAcceptor(),
								precipitationlimits);

	*/
		
	Rect areaRect(itsArea);		
	if(itsArea.isNamed())
	  {
		std::string name(itsArea.name());
		log << "** " << name  << " **" << endl;
		cout << "** " << name  << " **" << endl;

		Rect finlandRect(NFmiPoint(20.5558, 70.0925), NFmiPoint(31.5716, 59.8047));
		direction_id dirId(AreaTools::getDirection(finlandRect, areaRect));
		std::string dirStr(AreaTools::getDirectionString(dirId));
		//std::string areaRectStr(areaRect);
		cout << static_cast<std::string>(areaRect) << endl;//areaRectStr;
		//double xMin, yMin, xMax, yMax;
		//NFmiSvgTools::BoundingBox(itsArea.path(), xMin, yMin, xMax, yMax);
		//		Rect rect2(xMin, yMax, xMax, yMin);
		//	cout << "compare rect: " << static_cast<std::string>(rect2) << endl;//areaRectStr;

		cout << "Where: " << name << " is in the " << dirStr << endl;
		cout << "size: " << finlandRect.intersectionPercentage(areaRect) << " of Finland " << endl;



		/*
		NFmiSvgPath svgPath = itsArea.path();
		NFmiSvgPath::iterator iterator = svgPath.begin();
		double lon_min = 180.0;
		double lat_min = 90.0;
		double lon_max = 0.0;
		double lat_max = 0.0;
		while(iterator != svgPath.end())
		  {
			if(lon_min > iterator->itsX)
			  lon_min = iterator->itsX;
			if(lon_max < iterator->itsX)
			  lon_max = iterator->itsX;
			if(lat_min > iterator->itsY)
			  lat_min = iterator->itsY;
			if(lat_max < iterator->itsY)
			  lat_max = iterator->itsY;

			//			WeatherResult res(iterator->itsX, iterator->itsY);
			//			double x = static_cast<double>(iterator->itsX);
			iterator++;
		  }
		WeatherResult res1(lon_min, lat_max);
		WeatherResult res2(lon_max, lat_min);
		cout << "lon_min, lat_max: " << res1.value() << "," << res1.error() << "; ";
		cout << "lon_max, lat_min: " << res2.value() << "," << res2.error() << endl;
		*/
	  }

	

	NFmiTime periodStartTime(itsPeriod.localStartTime());
	NFmiTime periodEndTime(itsPeriod.localEndTime());

	const int day_starthour = optional_hour(itsVar+"::day::starthour", 6);
	const int day_maxstarthour = optional_hour(itsVar+"::day::maxstarthour", 12);


	if(periodStartTime.GetHour() >= day_starthour)
	  {
		periodStartTime.ChangeByHours(-1*(periodStartTime.GetHour() - day_starthour));
		if(itsForecastTime.GetHour() < day_maxstarthour)
		  periodStartTime.ChangeByHours(-24);
	  }
	else if(periodStartTime.GetHour() < day_starthour)
	  {
		periodStartTime.ChangeByHours(day_starthour - periodStartTime.GetHour());
		periodStartTime.ChangeByHours(-24);
	  }
	if(generator00.size() == 0)
	  periodEndTime.ChangeByHours(24);

	WeatherPeriod theExtendedPeriod(periodStartTime, periodEndTime);

#ifdef LATER
	location_coordinate_vector locationCoordinates;

	RangeAcceptor precipitationlimits;
	precipitationlimits.lowerLimit(0.1);
	//	NFmiIndexMask indexMask;
  	double precipitationAmount = GetLocationCoordinates(itsSources,
											   Precipitation,
											   itsArea,
											   theExtendedPeriod,
											   precipitationlimits,
											   locationCoordinates);

	if(locationCoordinates.size() > 0)
	  {
		Rect precipitationRect(locationCoordinates);
		cout << "Precipitation at area " << static_cast<std::string>(precipitationRect) << "sum: " << precipitationAmount << endl;

		direction_id directionId(AreaTools::getDirection(areaRect, precipitationRect));
		std::string directionStr(AreaTools::getDirectionString(directionId));

		cout << "Precipitation area is in the " << directionStr << " in " << itsArea.name() << endl;

		map<direction_id, double> precipitationDistribution;

		AreaTools::getDirectionDistribution(locationCoordinates, precipitationDistribution);
		cout << "Precipitation distribution: " << endl;
		cout << "NORTH: " << precipitationDistribution[NORTH] << endl;
		cout << "SOUTH: " << precipitationDistribution[SOUTH] << endl;
		cout << "EAST: " << precipitationDistribution[EAST] << endl;
		cout << "WEST: " << precipitationDistribution[WEST] << endl;
		cout << "NORTHEAST: " << precipitationDistribution[NORTHEAST] << endl;
		cout << "SOUTHEAST: " << precipitationDistribution[SOUTHEAST] << endl;
		cout << "SOUTHWEST: " << precipitationDistribution[SOUTHWEST] << endl;
		cout << "NORTHWEST: " << precipitationDistribution[NORTHWEST] << endl;
	  }
	else
	  {
		cout << "No precipitation" << endl;
	  }
#endif

	/*
	unsigned int maskSize = ExtractMask(itsSources,
										Precipitation,
										itsArea,
										theExtendedPeriod,
										precipitationlimits,
										indexMask);
	cout << "mask_size: " << maskSize << endl;
	*/
	//log << indexMask;
	//	PrintLatLon(itsSources, Precipitation, indexMask);






	wf_story_params theParameters(itsVar,
								  itsArea,
								  theExtendedPeriod,
								  itsSources,
								  log);

	init_parameters(theParameters);

	if(theParameters.theForecastArea == NO_AREA)
	  return paragraph;

	populate_temperature_time_series(theParameters);
	populate_cloudiness_time_series(theParameters);
	populate_thunderprobability_time_series(theParameters);
	populate_precipitation_time_series(theParameters);

	analyze_cloudiness_data(theParameters);
	analyze_precipitation_data(theParameters);
	analyze_thunder_data(theParameters);

	NightAndDayPeriodGenerator generator(theParameters.thePeriod, theParameters.theVariable);

	int thePeriodNumber = 3;

	Sentence cloudinessSentence;
	Sentence cloudinessChangeSentence;
	Sentence precipitationSentence;
	Sentence precipitationChangeSentence;

	cloudinessSentence << cloudiness_sentence(theParameters, thePeriodNumber);
	cloudinessChangeSentence << cloudiness_change_sentence(theParameters, thePeriodNumber);
	
	precipitationSentence << precipitation_sentence(theParameters, thePeriodNumber);
	precipitationChangeSentence << precipitation_change_sentence(theParameters, thePeriodNumber);

	if(!cloudinessChangeSentence.empty())
	  paragraph << cloudinessChangeSentence;
	else
	  paragraph << cloudinessSentence;

	if(!precipitationChangeSentence.empty())
	  paragraph << precipitationChangeSentence;
	else
	  paragraph << precipitationSentence;

	paragraph << thunder_sentence(theParameters, thePeriodNumber);

	log_weather_result_data(theParameters);


	CloudinessDataView theCloudinessView;
	PrecipitationDataView thePrecipitationView(theParameters);
	//	weather_result_data_item_vector* theInterestingData;

	if(theParameters.theForecastArea & INLAND_AREA)
	  {
		theCloudinessView.setInlandData((*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA]);
		/*
		get_sub_time_series(theParameters,
							theExtendedPeriod,
							*(*theParameters.theCompleteData[INLAND_AREA])[CLOUDINESS_DATA],
							theInterestingData);
		*/
	  }
	if(theParameters.theForecastArea & COASTAL_AREA)
	  {
		theCloudinessView.setCoastalData((*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA]);
		/*
		get_sub_time_series(theParameters,
							theExtendedPeriod,
							*(*theParameters.theCompleteData[COASTAL_AREA])[CLOUDINESS_DATA],
							theInterestingData);
		*/
	  }
	if(theParameters.theForecastArea & FULL_AREA)
		theCloudinessView.setFullData((*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA]);
	/*
	  get_sub_time_series(theParameters,
						  theExtendedPeriod,
						*(*theParameters.theCompleteData[FULL_AREA])[CLOUDINESS_DATA],
						  theInterestingData);
	*/

	/*
	theCloudinessView.printOutCloudinessPeriods(cout);
	theCloudinessView.printOutCloudinessTrends(cout);

		  theCloudinessView.cloudinessSentence(generator.period(thePeriodNumber));
	*/


	/*
	if(generator.isday(thePeriodNumber))
	  {
		WeatherPeriod ap(theExtendedPeriod);
		WeatherPeriod ip(theExtendedPeriod);
		get_part_of_the_day(generator.period(thePeriodNumber),
						   AAMUPAIVA, 
							ap);
		get_part_of_the_day(generator.period(thePeriodNumber),
						   ILTAPAIVA, 
							ip);
		log << "ANSSI:koko: ";
		log << thePrecipitationView.precipitationSentence(theParameters, generator.period(thePeriodNumber));
		log << "ANSSI:aamupäivä: ";
		log << thePrecipitationView.precipitationSentence(theParameters, ap);
		log << "ANSSI:iltapäivä: ";
		log << thePrecipitationView.precipitationSentence(theParameters, ip);

		log << "ANSSI:koko: ";
		log << theCloudinessView.cloudinessSentence(theParameters, generator.period(thePeriodNumber));
		log << "ANSSI:aamupäivä: ";
		log << theCloudinessView.cloudinessSentence(theParameters, ap);
		log << "ANSSI:iltapäivä: ";
		log << theCloudinessView.cloudinessSentence(theParameters, ip);
	  }
*/


	//	thePrecipitationView.printOutPrecipitationData(cout);
	//	thePrecipitationView.printOutPrecipitationTrends(cout);
	//thePrecipitationView.printOutPrecipitationLocation(cout);



	/*
	if(thePrecipitationView.separateCoastInlandPrecipitation(generator.period(thePeriodNumber)))
	  cout << "ANSSI: separate INLAND and COASTAL" << endl;
	else
	  cout << "ANSSI: DO NOT separate INLAND and COASTAL" << endl;
	*/
	theCloudinessView.printOutCloudinessTrends(cout);
	thePrecipitationView.printOutPrecipitationTrends(cout);
	thePrecipitationView.printOutPrecipitationPeriods(cout);

	Paragraph para;
	para <<  theCloudinessView.cloudinessSentence(theParameters, thePeriodNumber);
	para << theCloudinessView.cloudinessChangeSentence(theParameters, generator.period(thePeriodNumber));
	para << thePrecipitationView.precipitationSentence(theParameters, generator.period(thePeriodNumber));
	para << thePrecipitationView.precipitationChangeSentence(theParameters, generator.period(thePeriodNumber));

	log << "ANSSI: " << itsArea.name() << ": " << endl;
	log << para;
	/*
	log << theCloudinessView.cloudinessSentence(theParameters, thePeriodNumber);
	log << thePrecipitationView.precipitationSentence(theParameters, generator.period(thePeriodNumber));
	*/
	/*
	for(unsigned i = 0; i < generator.size(); i++)
	  {
		if(itsForecastTime >= generator.period(i+1).localStartTime() && 
		   itsForecastTime <= generator.period(i+1).localEndTime())
		  theCloudinessView.cloudinessSentence(generator.period(i+1));
	  }
	*/

#ifdef LATER
	const weather_result_data_item_vector* theInterestingData = theCloudinessView.getFullData() != 0 ? 
	  theCloudinessView.getFullData() : 
	  (theCloudinessView.getInlandData() != 0 ? 
	   theCloudinessView.getInlandData() : theCloudinessView.getCoastalData());

	
	for(unsigned int i = 0; i < theInterestingData->size(); i++)
	  {
		cloudiness_id clid = get_cloudiness_id(theInterestingData->at(i)->theResult.value());
		cout << cloudiness_string(clid) << endl;
	  }
#endif



	delete_data_structures(theParameters);

	log << paragraph;

	return paragraph;
  }


} // namespace TextGen

// ======================================================================
