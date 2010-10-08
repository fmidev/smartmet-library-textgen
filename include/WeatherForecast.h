#ifndef TEXTGEN_WEATHER_FORECAST_H
#define TEXTGEN_WEATHER_FORECAST_H

#include "AreaTools.h"
#include "WeatherPeriod.h"

namespace TextGen
{

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
#define SAA_JATKUU_POUTAISENA_PHRASE "sää jatkuu poutaisena"
#define SAA_JATKUU_PILVISENA_PHRASE "sää jatkuu pilvisenä"

#define HEIKKOJA_SADEKUUROJA_PHRASE "heikkoja sadekuuroja"
#define VOIMAKKAITA_SADEKUUROJA_PHRASE "voimakkaita sadekuuroja"
#define SADEKUUROJA_WORD "sadekuuroja"
#define LUMIKUUROJA_WORD "lumikuuroja"
#define SADEKUUROJA_WORD "sadekuuroja"
#define RANTAKUUROJA_WORD "räntäkuuroja"
#define VESIKUUROJA_WORD "vesikuuroja"
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
#define JA_WORD "ja"
#define JOKA_VOI_OLLA_JAATAVAA_PHRASE "joka voi olla jäätävää"
#define JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE "jotka voivat olla jäätäviä"
#define YKSITTAISET_SADEKUUROT_MAHDOLLISIA "yksittäiset sadekuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_RANTAKUUROT_MAHDOLLISIA "yksittäiset räntäkuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_LUMIKUUROT_MAHDOLLISIA "yksittäiset lumikuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA "yksittäiset vesi- tai räntäkuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA "yksittäiset räntä- tai vesikuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA "yksittäiset lumi- tai räntäkuurot ovat kuitenkin mahdollisia"
#define YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA "yksittäiset vesi- tai lumikuurot ovat kuitenkin mahdollisia"

#define SUMUA_WORD "sumua"
#define JOKA_VOI_OLLA_SAKEAA_PHRASE "joka voi olla sakeaa"

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

#define SELKEAA_UPPER_LIMIT 9.9
#define MELKEIN_SELKEAA_LOWER_LIMIT 9.9
#define MELKEIN_SELKEAA_UPPER_LIMIT 35
#define PUOLIPILVISTA_LOWER_LIMIT 35.0
#define PUOLIPILVISTA_UPPER_LIMIT 65.0
#define VERRATTAIN_PILVISTA_LOWER_LIMIT 65.0
#define VERRATTAIN_PILVISTA_UPPER_LIMIT 85.0
#define PILVISTA_LOWER_LIMIT 85.0

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
	  FOG_INTENSITY_MODERATE_DATA,
	  FOG_INTENSITY_DENSE_DATA,
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
	  //	  PITKA_AAMUPAIVA, // 09-12
	  KESKIPAIVA, // 11-13
	  //	  PITKA_KESKIPAIVA, // 09-15
	  ILTAPAIVA, // 13-18
	  //	  PITKA_ILTAPAIVA, // 12-18
	  ILTA, // 18-22
	  ILTAYO, // 22-00
	  KESKIYO, // 00-03
	  AAMUYO, // 03-06
	  PAIVA, // 09-18
	  //	  PITKA_PAIVA, // 06-18
	  YO, // 00-06
	  //	  PITKA_YO, // 22-06
	  YOPUOLI, // 18-06
	  PAIVAPUOLI, // 06-18

	  AAMU_JA_AAMUPAIVA, // 06-12
	  AAMUPAIVA_JA_KESKIPAIVA, // 09-13
	  KESKIPAIVA_JA_ILTAPAIVA, // 11-18
	  ILTAPAIVA_JA_ILTA, // 13-21
	  ILTA_JA_ILTAYO, //18-00
	  ILTAYO_JA_KESKIYO, // 22-03
	  KESKIYO_JA_AAMUYO, // 00-06
	  AAMUYO_JA_AAMU, // 03-09
	};

  enum trend_id
	{
	  PILVISTYY,
	  SELKENEE,
	  POUTAANTUU,
	  SADE_ALKAA,
	  NO_TREND
	};

  enum stat_func_id
	{
	  MIN,
	  MAX,
	  MEAN
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

  typedef vector<WeatherResultDataItem*> weather_result_data_item_vector;

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

  struct FogIntensityDataItem
  {
	FogIntensityDataItem(const float& moderateFogExtent,
						 const float& denseFogExtent):
	  theModerateFogExtent(moderateFogExtent),
	  theDenseFogExtent(denseFogExtent)
	{}

	float theModerateFogExtent;
	float theDenseFogExtent;
  };

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

	float precipitationPercentageNorth() const
	{ return thePrecipitationPercentageNorthEast + thePrecipitationPercentageNorthWest; }
	float precipitationPercentageSouth() const
	{ return thePrecipitationPercentageSouthEast + thePrecipitationPercentageSouthWest; }
	float precipitationPercentageEast() const
	{ return thePrecipitationPercentageNorthEast + thePrecipitationPercentageSouthEast; }
	float precipitationPercentageWest() const
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
  */
  typedef std::pair<WeatherPeriod, trend_id> weather_period_trend_id_pair;
  typedef vector<weather_period_trend_id_pair> trend_id_vector;
  typedef vector<PrecipitationDataItemData*> precipitation_data_vector;
  typedef vector<FogIntensityDataItem*> fog_data_vector;
  typedef map<int, PrecipitationDataItem*> precipitation_data_item_container;
  typedef vector<precipitation_data_item_container*> precipitation_data_container;
  typedef map<int, weather_result_data_item_vector*> weather_forecast_result_container;
  typedef map<int, weather_forecast_result_container*> weather_forecast_data_container;
  typedef map<int, CloudinessDataItem*> cloudiness_data_item_container;
  typedef vector<cloudiness_data_item_container*> cloudiness_data_container;
  typedef map<int, ThunderDataItem*> thunder_data_item_container;
  typedef vector<thunder_data_item_container*> thunder_data_container;
  typedef map<int, FogIntensityDataItem*> fog_data_item_container;
  typedef vector<fog_data_item_container*> fog_data_container;
  typedef vector<NFmiPoint*> location_coordinate_vector;




	struct wf_story_params
	{								 
	  wf_story_params(const string& variable,
					  const WeatherArea& weatherArea,
					  const WeatherPeriod weatherPeriod,
					  const NFmiTime forecastTime,
					  const AnalysisSources& analysisSources,
					  MessageLogger& log) :
		theVariable(variable),
		theArea(weatherArea),
		thePeriod(weatherPeriod),
		theForecastTime(forecastTime),
		theSources(analysisSources),
		theLog(log),
		theHourPeriodCount(0),
		theOriginalPeriodCount(0),
		theForecastArea(TextGen::AreaTools::NO_AREA)
	  {
	  }

	  const string& theVariable;
	  const WeatherArea& theArea;
	  const WeatherPeriod thePeriod;
	  const NFmiTime theForecastTime;
	  const AnalysisSources& theSources;
	  MessageLogger& theLog;
	  unsigned int theHourPeriodCount;
	  unsigned int theOriginalPeriodCount;
	  unsigned short theForecastArea;
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
	  fog_data_container theFogData;
	};


  void get_dry_and_weak_precipitation_limit(const wf_story_params& theParameters,
											const unsigned int& thePrecipitationForm,
											float& theDryWeatherLimit, 
											float& theWeakPrecipitationLimit);
  const char* trend_string(const trend_id& theTrendId);
  const char* precipitation_form_string(const precipitation_form_id& thePrecipitationForm);

  void get_part_of_the_day(const part_of_the_day_id& thePartOfTheDayId, int& theStartHour, int& theEndHour);
  part_of_the_day_id get_part_of_the_day_id(const WeatherPeriod& thePeriod);
  bool get_part_of_the_day(const WeatherPeriod& theSourcePeriod, 
						   const part_of_the_day_id& thePartOfTheDayId, 
						   WeatherPeriod& theDestinationPeriod);
  bool is_inside(const NFmiTime& theTimeStamp, 
				 const part_of_the_day_id& thePartOfTheDayId);
  bool is_inside(const NFmiTime& theTimeStamp, 
				 const WeatherPeriod& theWeatherPeriod);
  Sentence get_time_phrase_large(const WeatherPeriod& theWeatherPeriod);
  Sentence get_time_phrase(const WeatherPeriod& theWeatherPeriod, bool theAlkaenPhrase = false);
  unsigned int get_complete_precipitation_form(const string& theVariable,
											   const float thePrecipitationFormWater,
											   const float thePrecipitationFormDrizzle,
											   const float thePrecipitationFormSleet,
											   const float thePrecipitationFormSnow,
											   const float thePrecipitationFormFreezing); 
  void get_sub_time_series(const WeatherPeriod& thePeriod,
						   const weather_result_data_item_vector& theSourceVector,						   
						   weather_result_data_item_vector& theDestinationVector);
  void get_sub_time_series(const part_of_the_day_id& thePartOfTheDay,
						   const weather_result_data_item_vector& theSourceVector,
						   weather_result_data_item_vector& theDestinationVector);
  float get_mean(const weather_result_data_item_vector& theTimeSeries, 
				 const int& theStartIndex = 0, 
				 const int& theEndIndex = 0);
  float get_standard_deviation(const weather_result_data_item_vector& theTimeSeries);
  void get_min_max(const weather_result_data_item_vector& theTimeSeries, float& theMin, float& theMax);
  double get_pearson_coefficient(const weather_result_data_item_vector& theTimeSeries);
  void print_out_trend_vector(std::ostream& theOutput, const trend_id_vector& theTrendVector);

}

#endif // TEXTGEN_WEATHER_FORECAST_H
