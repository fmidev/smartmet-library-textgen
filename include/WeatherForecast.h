#ifndef TEXTGEN_WEATHER_FORECAST_H
#define TEXTGEN_WEATHER_FORECAST_H

#include "AreaTools.h"
#include "WeatherPeriod.h"

namespace TextGen
{

  class PrecipitationForecast;
  class CloudinessForecast;
  class FogForecast;
  class ThunderForecast;

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
#define ILTAYOSTA_WORD "iltayöllä"
#define KESKIYOLLA_WORD "keskiyöllä"
#define AAMUYOLLA_WORD "aamuyöllä"
#define YOLLA_WORD "yöllä"
#define TANAAN_WORD "tänään"
#define HUOMENNA_WORD "huomenna"

#define HEIKKOJA_WORD "heikkoja"
#define SAKEITA_WORD "sakeita"
#define RUNSAITA_WORD "runsaita"
#define MAHDOLLISESTI_WORD "mahdollisesti"
#define ENIMMAKSEEN_WORD "enimmäkseen"
#define ALUKSI_WORD "aluksi"
#define SAA_WORD "sää"
#define ON_WORD "on"
#define TULEE_WORD "tulee"
#define SAADAAN_WORD "saadaan"
#define HEIKKOA_WORD "heikkoa"
#define SAKEAA_WORD "sakeaa"
#define KOHTALAISTA_WORD "kohtalaista"
#define KOVAA_WORD "kovaa"
#define SADETTA_WORD "sadetta"
#define POUTAA_WORD "poutaa"
#define JONKA_JALKEEN_PHRASE "jonka jälkeen"
#define RUNSASTA_VESISADETTA_PHRASE "runsasta vesisadetta"
#define RUNSASTA_SADETTA_PHRASE "runsasta sadetta"
#define VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE "vaihtelee puolipilvisestä pilviseen"
#define VERRATTAIN_SELKEAA_PHRASE "verrattain selkeää"
#define VERRATTAIN_PILVINEN_PHRASE "verrattain pilvinen"
#define SELKEAA_WORD "selkeää"
#define PILVINEN_WORD "pilvinen"
#define SADETTA_WORD "sadetta"
#define MELKO_SELKEAA_PHRASE "melko selkeää"
#define PUOLIPILVINEN_WORD "puolipilvinen"
#define POUTAINEN_WORD "poutainen"
#define HEIKKOA_SADETTA_PHRASE "heikkoa sadetta"
#define HEIKKOA_VESISADETTA_PHRASE "heikkoa vesisadetta"
#define KOHTALAISTA_SADETTA_PHRASE "kohtalaista sadetta"
#define KOHTALAISTA_VESISADETTA_PHRASE "kohtalaista vesisadetta"
  //#define ENIMMAKSEEN_POUTAINEN_PHRASE "enimmäkseen poutainen"
#define HEIKKOJA_VESIKUUROJA_PHRASE "heikkoja vesikuuroja"
#define VOIMAKKAITA_VESIKUUROJA_PHRASE "voimakkaita vesikuuroja"
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
#define VESI_TAVUVIIVA_WORD "vesi-"
#define RANTA_TAVUVIIVA_WORD "räntä-"
#define LUMI_TAVUVIIVA_WORD "lumi-"
#define TAI_WORD "tai"
#define JA_WORD "ja"
#define JOKA_VOI_OLLA_JAATAVAA_PHRASE "joka voi olla jäätävää"
#define JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE "jotka voivat olla jäätäviä"
#define YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE "yksittäiset sadekuurot mahdollisia"
#define YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE "yksittäiset vesikuurot mahdollisia"
#define YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_PHRASE "yksittäiset räntäkuurot mahdollisia"
#define YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_PHRASE "yksittäiset lumikuurot mahdollisia"
#define YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA_PHRASE "yksittäiset vesi- tai räntäkuurot mahdollisia"
#define YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA_PHRASE "yksittäiset räntä- tai vesikuurot mahdollisia"
#define YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA_PHRASE "yksittäiset lumi- tai räntäkuurot mahdollisia"
#define YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA_PHRASE "yksittäiset vesi- tai lumikuurot mahdollisia"
#define JOKA_MUUTTUU_LUMISATEEKSI_PHRASE "joka muuttuu lumisateeksi"
#define JOKA_MUUTTUU_VESISATEEKSI_PHRASE "joka muuttuu vesisateeksi"
#define JOKA_MUUTTUU_TIHKUSATEEKSI_PHRASE "joka muuttuu tihkusateeksi"
#define JOKA_MUUTTUU_RANTASATEEKSI_PHRASE "joka muuttuu räntäsateeksi"

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

#define POHJOISESTA_ALKAEN_PHRASE "pohjoisesta alkaen"
#define ETELASTA_ALKAEN_PHRASE "etelästä alkaen"
#define IDASTA_ALKAEN_PHRASE "idästä alkaen"
#define LANNESTA_ALKAEN_PHRASE "lännestä alkaen"
#define KOILLISESTA_ALKAEN_PHRASE "koillisesta alkaen"
#define KAAKOSTA_ALKAEN_PHRASE "kaakosta alkaen"
#define LOUNAASTA_ALKAEN_PHRASE "lounaasta alkaen"
#define LUOTEESTA_ALKAEN_PHRASE "luoteesta alkaen"

#define SADEALUE_WORD "sadealue"
#define SAAPUU_WORD "saapuu"
#define POHJOISESTA_WORD "pohjoisesta"
#define ETELASTA_WORD "etelästä"
#define IDASTA_WORD "idästä"
#define LANNESTA_WORD "lännestä"
#define KOILLISESTA_WORD "koillisesta"
#define KAAKOSTA_WORD "kaakosta"
#define LOUNAASTA_WORD "lounaasta"
#define LUOTEESTA_WORD "luoteesta"

#define SELKEAA_UPPER_LIMIT 9.9
#define MELKEIN_SELKEAA_LOWER_LIMIT 9.9
#define MELKEIN_SELKEAA_UPPER_LIMIT 35
#define PUOLIPILVISTA_LOWER_LIMIT 35.0
#define PUOLIPILVISTA_UPPER_LIMIT 65.0
#define VERRATTAIN_PILVISTA_LOWER_LIMIT 65.0
#define VERRATTAIN_PILVISTA_UPPER_LIMIT 85.0
#define PILVISTA_LOWER_LIMIT 85.0
#define PILVISYVAA_LOWER_LIMIT 20.0
#define PILVISYVAA_UPPER_LIMIT 65.0

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

#define SMALL_PROBABILITY_FOR_THUNDER_LOWER_LIMIT 0.1
#define SMALL_PROBABILITY_FOR_THUNDER_UPPER_LIMIT 15.0
#define POSSIBLY_THUNDER_LOWER_LIMIT 15.0
#define POSSIBLY_THUNDER_UPPER_LIMIT 35.0
#define OCCASIONALLY_THUNDER_LOWER_LIMIT 35.0
#define OCCASIONALLY_THUNDER_UPPER_LIMIT 110.0
#define MAJORITY_LIMIT 50.0

#define TREND_CHANGE_COEFFICIENT_TRESHOLD 0.65 // pearson coefficient
#define PEARSON_CO_FORM_TRANSFORM 0.65


  enum weather_result_data_id
	{
	  CLOUDINESS_DATA = 0x1,
	  CLOUDINESS_NORTHEAST_SHARE_DATA,
	  CLOUDINESS_SOUTHEAST_SHARE_DATA,
	  CLOUDINESS_SOUTHWEST_SHARE_DATA,
	  CLOUDINESS_NORTHWEST_SHARE_DATA,
	  PRECIPITATION_MAX_DATA,
	  PRECIPITATION_MEAN_DATA,
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
	  PRECIPITATION_POINT_DATA,
	  THUNDER_PROBABILITY_DATA,
	  THUNDER_NORTHEAST_SHARE_DATA,
	  THUNDER_SOUTHEAST_SHARE_DATA,
	  THUNDER_SOUTHWEST_SHARE_DATA,
	  THUNDER_NORTHWEST_SHARE_DATA,
	  FOG_INTENSITY_MODERATE_DATA,
	  FOG_INTENSITY_DENSE_DATA,
	  FOG_NORTHEAST_SHARE_DATA,
	  FOG_SOUTHEAST_SHARE_DATA,
	  FOG_SOUTHWEST_SHARE_DATA,
	  FOG_NORTHWEST_SHARE_DATA,
	  UNDEFINED_DATA_ID
	};

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

  enum precipitation_form_transformation_id
	{
	  WATER_TO_SNOW,
	  WATER_TO_DRIZZLE,
	  WATER_TO_SLEET,
	  SNOW_TO_WATER,
	  SNOW_TO_DRIZZLE,
	  SNOW_TO_SLEET,
	  DRIZZLE_TO_WATER,
	  DRIZZLE_TO_SNOW,
	  DRIZZLE_TO_SLEET,
	  SLEET_TO_WATER,
	  SLEET_TO_DRIZZLE,
	  SLEET_TO_SNOW,
	  NO_FORM_TRANSFORMATION
	};

  enum precipitation_intesity_id
	{
	  DRY_WEATHER,
	  WEAK_PRECIPITATION,
	  MODERATE_PRECIPITATION,
	  HEAVY_PRECIPITATION,
	  MISSING_INTENSITY_ID
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

  enum precipitation_traverse_id
	{
	  FROM_SOUTH_TO_NORTH,
	  FROM_NORTH_TO_SOUTH,
	  FROM_EAST_TO_WEST,
	  FROM_WEST_TO_EAST,
	  FROM_NORTHEAST_TO_SOUTHWEST,
	  FROM_SOUTHWEST_TO_NORTHEAST,
	  FROM_NORTHWEST_TO_SOUTHEAST,
	  FROM_SOUTHEAST_TO_NORTHWEST,
	  MISSING_TRAVERSE_ID
	};

  enum cloudiness_id
	{
	  SELKEAA,
	  MELKO_SELKEAA,
	  PUOLIPILVINEN,
	  VERRATTAIN_PILVINEN,
	  PILVINEN,
	  PUOLIPILVINEN_JA_PILVINEN,
	  MISSING_CLOUDINESS_ID
	};

  enum thunder_probability_id
	{
	  SMALL_PROBABILITY_FOR_THUNDER,
	  POSSIBLY_THUNDER,
	  OCCASIONALLY_THUNDER,
	  NO_THUNDER
	};

  enum part_of_the_day_id
	{
	  AAMU, // 06-09
	  AAMUPAIVA, // 09-11
	  KESKIPAIVA, // 11-13
	  ILTAPAIVA, // 13-18
	  ILTA, // 18-22
	  ILTAYO, // 22-00
	  KESKIYO, // 00-03
	  AAMUYO, // 03-06
	  PAIVA, // 09-18
	  YO, // 00-06
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
	  MISSING_PART_OF_THE_DAY_ID
	};

  enum weather_event_id
	{
	  PILVISTYY,
	  SELKENEE,
	  POUTAANTUU, // >= 6h
	  POUTAANTUU_WHEN_EXTENT_SMALL, // >= 6h, but extent is not large
	  SADE_ALKAA, // >= 6h
	  //TYPE_CHANGES,
	  MISSING_WEATHER_EVENT
	};

  enum story_part_id2
	{
	  PRECIPITATION_STORY_PART = 0x1,
	  CLOUDINESS_STORY_PART = 0x2,
	  GETTING_CLOUDY_STORY_PART = 0x4,
	  CLEARING_UP_STORY_PART = 0x8,
	  PRECIPITATION_TYPE_CHANGE_STORY_PART = 0x10,
	  MISSING_STORY_PART = 0x0
	};

  enum story_part_id
	{
	  PILVISTYY_STORY_PART = 0x1,
	  SELKENEE_STORY_PART = 0x2,
	  POUTAANTUU_STORY_PART = 0x4,
	  POUTAANTUU_AFTER_SMALL_EXTENT_STORY_PART = 0x8,
	  SADE_ALKAA_STORY_PART = 0x10,
	  PILVISYYS_STORY_PART = 0x20,
	  SADE_STORY_PART = 0x40,
	  SHORT_PRECIPITATION_STORY_PART = 0x80,
	  MISSING_STORY_PART_ID = 0x0
	};

  enum stat_func_id
	{
	  MIN,
	  MAX,
	  MEAN
	};

  enum area_specific_sentence_id
	{
	  ALUEEN_POHJOISOSISSA,
	  ALUEEN_ETELAOSISSA,
	  ALUEEN_ITAOSISSA,
	  ALUEEN_LANSIOSISSA,
	  ENIMMAKSEEN_ALUEEN_POHJOISOSISSA,
	  ENIMMAKSEEN_ALUEEN_ETELAOSISSA,
	  ENIMMAKSEEN_ALUEEN_ITAOSISSA,
	  ENIMMAKSEEN_ALUEEN_LANSIOSISSA,
	  MISSING_AREA_SPECIFIC_SENTENCE_ID
	};

  enum precipitation_type
	{
	  SHOWERS,
	  CONTINUOUS,
	  MISSING_PRECIPITATION_TYPE
	};

  class WeatherResultDataItem;
  class PrecipitationDataItemData;
  class FogIntensityDataItem;
  class PrecipitationDataItem;
  class CloudinessDataItem;
  class ThunderDataItem;
  class FogIntensityDataItem;

  typedef vector<WeatherResultDataItem*> weather_result_data_item_vector;
  typedef std::pair<NFmiTime, weather_event_id> timestamp_weather_event_id_pair;
  typedef vector<timestamp_weather_event_id_pair> weather_event_id_vector;
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
  typedef std::pair<WeatherPeriod, unsigned int> weather_period_story_part_id_pair;
  typedef vector<weather_period_story_part_id_pair> story_part_vector;


  struct wf_story_params
  {								 
	wf_story_params(const string& variable,
					const WeatherArea& weatherArea,
					const WeatherPeriod dataPeriod,
					const WeatherPeriod forecastPeriod,
					const NFmiTime forecastTime,
					const AnalysisSources& analysisSources,
					MessageLogger& log) :
	  theVariable(variable),
	  theArea(weatherArea),
	  theDataPeriod(dataPeriod),
	  theForecastPeriod(forecastPeriod),
	  theForecastTime(forecastTime),
	  theSources(analysisSources),
	  theLog(log),
	  thePrecipitationForecast(0),
	  theCloudinessForecast(0),
	  theFogForecast(0),
	  theThunderForecast(0),
	  theHourPeriodCount(0),
	  theOriginalPeriodCount(0),
	  theForecastArea(TextGen::AreaTools::NO_AREA)
	{
	}

	const string& theVariable;
	const WeatherArea& theArea;
	const WeatherPeriod theDataPeriod;
	const WeatherPeriod theForecastPeriod;
	const NFmiTime theForecastTime;
	const AnalysisSources& theSources;
	MessageLogger& theLog;
	PrecipitationForecast* thePrecipitationForecast;
	CloudinessForecast* theCloudinessForecast;
	FogForecast* theFogForecast;
	ThunderForecast* theThunderForecast;
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


  void get_precipitation_limit_value(const wf_story_params& theParameters,
									 const unsigned int& thePrecipitationForm,
									 const precipitation_intesity_id& theIntensityId,
									 float& theLowerLimit,
									 float& theUpperLimit);
  void get_dry_and_weak_precipitation_limit(const wf_story_params& theParameters,
											const unsigned int& thePrecipitationForm,
											float& theDryWeatherLimit, 
											float& theWeakPrecipitationLimit);
  const char* weather_event_string(const weather_event_id& theWeatherEventId);
  const char* precipitation_form_string(const precipitation_form_id& thePrecipitationForm);
  const char* precipitation_traverse_string(const precipitation_traverse_id& thePrecipitationTraverseId);

  void get_part_of_the_day(const part_of_the_day_id& thePartOfTheDayId, int& theStartHour, int& theEndHour);
  part_of_the_day_id get_part_of_the_day_id(const WeatherPeriod& thePeriod);
  /*  bool get_part_of_the_day(const WeatherPeriod& theSourcePeriod, 
						   const part_of_the_day_id& thePartOfTheDayId, 
						   WeatherPeriod& theDestinationPeriod);*/
  part_of_the_day_id get_part_of_the_day_id(const NFmiTime& theTimestamp);
  bool is_inside(const WeatherPeriod& theWeatherPeriod,
				 const part_of_the_day_id& thePartOfTheDayId);
  bool is_inside(const NFmiTime& theTimeStamp, 
				 const part_of_the_day_id& thePartOfTheDayId);
  bool is_inside(const NFmiTime& theTimeStamp, 
				 const WeatherPeriod& theWeatherPeriod);
  Sentence get_direction_phrase(const AreaTools::direction_id& theDirectionId, bool theAlkaenPhrase = false);
  Sentence get_time_phrase_large(const WeatherPeriod& theWeatherPeriod,
								 bool theAlkaenPhrase = false,
								 vector<std::string>* theStringVector = 0);
  Sentence get_time_phrase(const NFmiTime& theTimestamp, 
						   bool theAlkaenPhrase = false,
						   vector<std::string>* theStringVector = 0);
  Sentence get_today_phrase(const NFmiTime& theEventTimestamp,
							const string& theVariable,
							const WeatherArea& theArea,
							const WeatherPeriod thePeriod,
							const NFmiTime& theForecastTime);
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
  double get_pearson_coefficient(const weather_result_data_item_vector& theTimeSeries,
								 const unsigned int& theStartIndex,
								 const unsigned int& theEndIndex,
								 const bool& theUseErrorValueFlag = false);
  void print_out_weather_event_vector(std::ostream& theOutput, const weather_event_id_vector& theWeatherEventVector);
  Sentence area_specific_sentence(const float& north,
								  const float& south,
								  const float& east,
								  const float& west,
								  const float& northEast,
								  const float& southEast,
								  const float& southWest,
								  const float& northWest,
								  const bool& mostlyFlag = true);

  int get_today_vector(const string& theVariable,
					   const WeatherArea& theArea,
					   const WeatherPeriod& thePeriod,
					   const NFmiTime& theForecastTime,
					   vector<Sentence*>& theTodayVector);
 
  area_specific_sentence_id get_area_specific_sentence_id(const float& north,
														  const float& south,
														  const float& east,
														  const float& west,
														  const float& northEast,
														  const float& southEast,
														  const float& southWest,
														  const float& northWest,
														  const bool& mostlyFlag = true);
  int get_period_length(const WeatherPeriod& thePeriod);


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

  struct CloudinessDataItemData
  {
	CloudinessDataItemData(const cloudiness_id& id,
						   const float& min,
						   const float& mean,
						   const float& max,
						   const float& standardDeviation,
						   const weather_event_id& weatherEventId,
						   const float& pearsonCoefficient) :
	  theId(id),
	  theMin(min),
	  theMean(mean),
	  theMax(max),
	  theStandardDeviation(standardDeviation),
	  theWeatherEventId(weatherEventId),
	  thePearsonCoefficient(pearsonCoefficient)
	{
	}

	cloudiness_id theId;
	float theMin;
	float theMean;
	float theMax;
	float theStandardDeviation;
	weather_event_id theWeatherEventId;
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
	PrecipitationDataItemData(const wf_story_params& theParameters,
							  const unsigned int& precipitationForm,
							  const float& precipitationIntensity,
							  const float& precipitationExtent,
							  const float& precipitationFormWater,
							  const float& precipitationFormDrizzle,
							  const float& precipitationFormSleet,
							  const float& precipitationFormSnow,
							  const float& precipitationFormFreezing,
							  const float& precipitationTypeShowers,
							  const weather_event_id weatherEventId,
							  const float& pearsonCoefficient,
							  const NFmiTime& observationTime) :
	  thePrecipitationForm(precipitationForm),
	  thePrecipitationIntensity(precipitationIntensity),
	  thePrecipitationExtent(precipitationExtent),
	  thePrecipitationFormWater(precipitationFormWater),
	  thePrecipitationFormDrizzle(precipitationFormDrizzle),
	  thePrecipitationFormSleet(precipitationFormSleet),
	  thePrecipitationFormSnow(precipitationFormSnow),
	  thePrecipitationFormFreezing(precipitationFormFreezing),
	  thePrecipitationTypeShowers(precipitationTypeShowers),
	  theWeatherEventId(weatherEventId),
	  thePearsonCoefficient(pearsonCoefficient),
	  theObservationTime(observationTime),
	  thePrecipitationPercentageNorthEast(0.0),
	  thePrecipitationPercentageSouthEast(0.0),
	  thePrecipitationPercentageSouthWest(0.0),
	  thePrecipitationPercentageNorthWest(0.0),
	  thePrecipitationPoint(0.0, 0.0),
	  thePrecipitationType(MISSING_PRECIPITATION_TYPE),
	  thePrecipitationIntensityId(MISSING_INTENSITY_ID)
	{
	  if(precipitationTypeShowers != kFloatMissing)
		thePrecipitationType = precipitationTypeShowers >= MAJORITY_LIMIT ? SHOWERS : CONTINUOUS;
	  
	  if(precipitationIntensity == kFloatMissing)
		{
		  thePrecipitationIntensityId = DRY_WEATHER;
		}
	  else
		{
		  float lowerLimit;
		  float dryUpperLimit;
		  float weakUpperLimit;
		  float moderateUpperLimit;
		  get_precipitation_limit_value(theParameters,
										precipitationForm,
										DRY_WEATHER,
										lowerLimit,
										dryUpperLimit);
		  get_precipitation_limit_value(theParameters,
										precipitationForm,
										WEAK_PRECIPITATION,
										lowerLimit,
										weakUpperLimit);
		  get_precipitation_limit_value(theParameters,
										precipitationForm,
										MODERATE_PRECIPITATION,
										lowerLimit,
										moderateUpperLimit);

		  if(precipitationIntensity < dryUpperLimit)
			thePrecipitationIntensityId = DRY_WEATHER;
		  else if(precipitationIntensity < weakUpperLimit)
			thePrecipitationIntensityId = WEAK_PRECIPITATION;
		  else if(precipitationIntensity < moderateUpperLimit)
			thePrecipitationIntensityId = MODERATE_PRECIPITATION;
		  else
			thePrecipitationIntensityId = HEAVY_PRECIPITATION;
		}
	}

	unsigned int thePrecipitationForm;
	float thePrecipitationIntensity;
	float thePrecipitationExtent;
	float thePrecipitationFormWater;
	float thePrecipitationFormDrizzle;
	float thePrecipitationFormSleet;
	float thePrecipitationFormSnow;
	float thePrecipitationFormFreezing;
	float thePrecipitationTypeShowers;
	weather_event_id theWeatherEventId;
	float thePearsonCoefficient;
	NFmiTime theObservationTime;
	float thePrecipitationPercentageNorthEast;
	float thePrecipitationPercentageSouthEast;
	float thePrecipitationPercentageSouthWest;
	float thePrecipitationPercentageNorthWest;
	weather_result_data_item_vector thePrecipitationShareNorthEast;
	weather_result_data_item_vector thePrecipitationShareSouthEast;
	weather_result_data_item_vector thePrecipitationShareSouthWest;
	weather_result_data_item_vector thePrecipitationShareNorthWest;
	NFmiPoint thePrecipitationPoint;
	precipitation_type thePrecipitationType;
	precipitation_intesity_id thePrecipitationIntensityId;

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
			 theWeatherEventId == theItem.theWeatherEventId &&
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

}

#endif // TEXTGEN_WEATHER_FORECAST_H
