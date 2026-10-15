#pragma once

#include "AreaTools.h"
#include "MessageLogger.h"
#include <calculator/WeatherPeriod.h>
#include <memory>
#include <string>

namespace TextGen
{
// An empty period at the current time, for members that have no meaningful value yet. It is
// built from one time value: two separate TextGenPosixTime() calls may straddle a second
// boundary, and since the compiler may evaluate the end argument first, the end could then be
// before the start, which WeatherPeriod rejects.
inline WeatherPeriod empty_period()
{
  TextGenPosixTime now;
  return {now, now};
}

class PrecipitationForecast;
class CloudinessForecast;
class FogForecast;
class ThunderForecast;
class Sentence;
class GlyphContainer;

constexpr const char* EMPTY_STRING = "empty_string";
constexpr const char* SPACE_STRING = " ";
constexpr const char* COMMA_PUNCTUATION_MARK = ",";
constexpr const char* COLON_PUNCTUATION_MARK = ":";
constexpr const char* INTENSITY_PARAMETER = "intensity";
constexpr const char* PRECIPITATION_PARAMETER = "precipitation";
constexpr const char* IN_PLACES_PARAMETER = "paikoin";
constexpr const char* PLAIN_PRECIPITATION_PHRASE = "precipitation phrase";

constexpr const char* INLAND_PHRASE = "sisamaassa";
constexpr const char* COAST_PHRASE = "rannikolla";
constexpr const char* SADE_ALKAA_PHRASE = "sade alkaa";
constexpr const char* AAMULLA_WORD = "aamulla";
constexpr const char* AAMUPAIVALLA_WORD = "aamupaivalla";
constexpr const char* ILTAPAIVALLA_WORD = "iltapaivalla";
constexpr const char* PAIVALLA_WORD = "paivalla";
constexpr const char* ILLALLA_WORD = "illalla";
constexpr const char* ILTAYOLLA_WORD = "iltayolla";
constexpr const char* KESKIYOLLA_WORD = "keskiyolla";
constexpr const char* KESKIPAIVALLA_WORD = "keskipaivalla";
constexpr const char* AAMUYOLLA_WORD = "aamuyolla";
constexpr const char* YOLLA_WORD = "yolla";
constexpr const char* TANAAN_WORD = "tanaan";
constexpr const char* HUOMENNA_WORD = "huomenna";
constexpr const char* ALUKSI_WORD = "aluksi";
constexpr const char* MYOHEMMIN_WORD = "myohemmin";
constexpr const char* TILAPAISESTI_WORD = "tilapaisesti";

constexpr const char* SAA_ON_ENIMMAKSEEN_POUTAISTA_PHRASE = "saa on enimmakseen poutaista";
constexpr const char* SAA_ON_POUTAINEN_PHRASE = "saa on poutainen";
constexpr const char* HEIKKOJA_WORD = "heikkoja";
constexpr const char* SAKEITA_WORD = "sakeita";
constexpr const char* RUNSAITA_WORD = "runsaita";
constexpr const char* ENIMMAKSEEN_WORD = "enimmakseen";
constexpr const char* SAA_WORD = "saa";
constexpr const char* ON_WORD = "on";
constexpr const char* SAADAAN_WORD = "saadaan";
constexpr const char* HEIKKOA_WORD = "heikkoa";
constexpr const char* SAKEAA_WORD = "sakeaa";
constexpr const char* KOVAA_WORD = "kovaa";
constexpr const char* SADETTA_WORD = "sadetta";
constexpr const char* RUNSASTA_VESISADETTA_PHRASE = "runsasta vesisadetta";
constexpr const char* RUNSASTA_SADETTA_PHRASE = "runsasta sadetta";
constexpr const char* VAIHTELEE_PUOLIPILVISESTA_PILVISEEN_PHRASE = "vaihtelee puolipilvisesta pilviseen";
constexpr const char* VERRATTAIN_PILVINEN_PHRASE = "verrattain pilvinen";
constexpr const char* SELKEA_WORD = "selkea";
constexpr const char* PILVINEN_WORD = "pilvinen";
constexpr const char* MELKO_SELKEA_PHRASE = "melko selkea";
constexpr const char* PUOLIPILVINEN_WORD = "puolipilvinen";
constexpr const char* HEIKKOA_SADETTA_PHRASE = "heikkoa sadetta";
constexpr const char* HEIKKOA_VESISADETTA_PHRASE = "heikkoa vesisadetta";
constexpr const char* HEIKKOJA_VESIKUUROJA_PHRASE = "heikkoja vesikuuroja";
constexpr const char* VOIMAKKAITA_VESIKUUROJA_PHRASE = "voimakkaita vesikuuroja";
constexpr const char* PAIKOIN_WORD = "paikoin";
constexpr const char* MONIN_PAIKOIN_WORD = "monin paikoin";
constexpr const char* PILVISTYVAA_WORD = "pilvistyvaa";
constexpr const char* SELKENEVAA_WORD = "selkenevaa";
constexpr const char* SAA_POUTAANTUU_PHRASE = "saa poutaantuu";
constexpr const char* MAHDOLLISESTI_UKKOSTA_PHRASE = "mahdollisesti ukkosta";
constexpr const char* PAIKOIN_UKKOSTA_PHRASE = "paikoin ukkosta";
constexpr const char* UKKOSTA_WORD = "ukkosta";

constexpr const char* HEIKKOJA_SADEKUUROJA_PHRASE = "heikkoja sadekuuroja";
constexpr const char* VOIMAKKAITA_SADEKUUROJA_PHRASE = "voimakkaita sadekuuroja";
constexpr const char* SADEKUUROJA_WORD = "sadekuuroja";
constexpr const char* LUMIKUUROJA_WORD = "lumikuuroja";
constexpr const char* RANTAKUUROJA_WORD = "rantakuuroja";
constexpr const char* VESIKUUROJA_WORD = "vesikuuroja";
constexpr const char* RANTASADETTA_WORD = "rantasadetta";
constexpr const char* VESISADETTA_WORD = "vesisadetta";
constexpr const char* LUMISADETTA_WORD = "lumisadetta";
constexpr const char* TIHKUSADETTA_WORD = "tihkusadetta";

constexpr const char* RANTA_TAI_LUMISADETTA_PHRASE = "ranta- tai lumisadetta";
constexpr const char* LUMI_TAI_RANTASADETTA_PHRASE = "lumi- tai rantasadetta";
constexpr const char* LUMI_TAI_VESISADETTA_PHRASE = "lumi- tai vesisadetta";
constexpr const char* VESI_TAI_LUMISADETTA_PHRASE = "vesi- tai lumisadetta";
constexpr const char* RANTA_TAI_VESISADETTA_PHRASE = "ranta- tai vesisadetta";
constexpr const char* VESI_TAI_RANTASADETTA_PHRASE = "vesi- tai rantasadetta";
constexpr const char* RANTA_TAI_VESIKUUROJA_PHRASE = "ranta- tai vesikuuroja";
constexpr const char* VESI_TAI_RANTAKUUROJA_PHRASE = "vesi- tai rantakuuroja";
constexpr const char* VESI_TAI_LUMIKUUROJA_PHRASE = "vesi- tai lumikuuroja";
constexpr const char* LUMI_TAI_VESIKUUROJA_PHRASE = "lumi- tai vesikuuroja";
constexpr const char* LUMI_TAI_TIHKUSADETTA_PHRASE = "lumi- tai tihkusadetta";
constexpr const char* VESI_TAI_TIHKUSADETTA_PHRASE = "vesi- tai tihkusadetta";
constexpr const char* RANTA_TAI_TIHKUSADETTA_PHRASE = "ranta- tai tihkusadetta";

constexpr const char* JA_WORD = "ja";
constexpr const char* JAATAVAA_VESISADETTA_PHRASE = "jaatavaa vesisadetta";
constexpr const char* JAATAVAA_TIHKUSADETTA_PHRASE = "jaatavaa tihkusadetta";
constexpr const char* JOKA_VOI_OLLA_JAATAVAA_PHRASE = "joka voi olla jaatavaa";
constexpr const char* JOTKA_VOIVAT_OLLA_JAATAVIA_PHRASE = "jotka voivat olla jaatavia";
constexpr const char* YKSITTAISET_SADEKUUROT_MAHDOLLISIA_PHRASE = "yksittaiset sadekuurot mahdollisia";
constexpr const char* YKSITTAISET_VESIKUUROT_MAHDOLLISIA_PHRASE = "yksittaiset vesikuurot mahdollisia";
constexpr const char* YKSITTAISET_RANTAKUUROT_MAHDOLLISIA_PHRASE = "yksittaiset rantakuurot mahdollisia";
constexpr const char* YKSITTAISET_LUMIKUUROT_MAHDOLLISIA_PHRASE = "yksittaiset lumikuurot mahdollisia";
constexpr const char* YKSITTAISET_VESI_RANTA_KUUROT_MAHDOLLISIA_PHRASE =
  "yksittaiset vesi- tai rantakuurot mahdollisia";
constexpr const char* YKSITTAISET_RANTA_VESI_KUUROT_MAHDOLLISIA_PHRASE =
  "yksittaiset ranta- tai vesikuurot mahdollisia";
constexpr const char* YKSITTAISET_LUMI_RANTA_KUUROT_MAHDOLLISIA_PHRASE =
  "yksittaiset lumi- tai rantakuurot mahdollisia";
constexpr const char* YKSITTAISET_VESI_LUMI_KUUROT_MAHDOLLISIA_PHRASE =
  "yksittaiset vesi- tai lumikuurot mahdollisia";
constexpr const char* JOKA_MUUTTUU_LUMISATEEKSI_PHRASE = "joka muuttuu lumisateeksi";
constexpr const char* JOKA_MUUTTUU_VESISATEEKSI_PHRASE = "joka muuttuu vesisateeksi";
constexpr const char* JOKA_MUUTTUU_TIHKUSATEEKSI_PHRASE = "joka muuttuu tihkusateeksi";
constexpr const char* JOKA_MUUTTUU_RANTASATEEKSI_PHRASE = "joka muuttuu rantasateeksi";
constexpr const char* JOTKA_MUUTTUVAT_RANTAKUUROIKSI_PHRASE = "jotka muuttuvat rantakuuroiksi";
constexpr const char* JOTKA_MUUTTUVAT_LUMIKUUROIKSI_PHRASE = "jotka muuttuvat lumikuuroiksi";

constexpr const char* PIKKUPAKKASTA_PHRASE = "pikkupakkasta";
constexpr const char* SUMUA_WORD = "sumua";
constexpr const char* JOKA_VOI_OLLA_SAKEAA_PHRASE = "joka voi olla sakeaa";

constexpr const char* ALUEEN_POHJOISOSASSA_PHRASE = "alueen pohjoisosassa";
constexpr const char* ALUEEN_ETELAOSASSA_PHRASE = "alueen etelaosassa";
constexpr const char* ALUEEN_ITAOSASSA_PHRASE = "alueen itaosassa";
constexpr const char* ALUEEN_LANSIOSASSA_PHRASE = "alueen lansiosassa";
constexpr const char* ALUEEN_KOILLISOSASSA_PHRASE = "alueen koillisosassa";
constexpr const char* ALUEEN_KAAKKOISOSASSA_PHRASE = "alueen kaakkoisosassa";
constexpr const char* ALUEEN_LOUNAISOSASSA_PHRASE = "alueen lounaisosassa";
constexpr const char* ALUEEN_LUOTEISOSASSA_PHRASE = "alueen luoteisosassa";

constexpr const char* AAMUSTA_ALKAEN_PHRASE = "aamusta alkaen";
constexpr const char* AAMUPAIVASTA_ALKAEN_PHRASE = "aamupaivasta alkaen";
constexpr const char* ILTAPAIVASTA_ALKAEN_PHRASE = "iltapaivasta alkaen";
constexpr const char* ILLASTA_ALKAEN_PHRASE = "illasta alkaen";
constexpr const char* ILTAYOSTA_ALKAEN_PHRASE = "iltayosta alkaen";
constexpr const char* KESKIYOSTA_ALKAEN_PHRASE = "keskiyosta alkaen";
constexpr const char* KESKIPAIVASTA_ALKAEN_PHRASE = "keskipaivasta alkaen";
constexpr const char* AAMUYOSTA_ALKAEN_PHRASE = "aamuyosta alkaen";
constexpr const char* AAMUUN_ASTI_PHRASE = "aamuun asti";
constexpr const char* AAMUPAIVAAN_ASTI_PHRASE = "aamupaivaan asti";
constexpr const char* ILTAPAIVAAN_ASTI_PHRASE = "iltapaivaan asti";
constexpr const char* ILTAAN_ASTI_PHRASE = "iltaan asti";
constexpr const char* ILTAYOHON_ASTI_PHRASE = "iltayohon asti";
constexpr const char* KESKIYOHON_ASTI_PHRASE = "keskiyohon asti";
constexpr const char* KESKIPAIVAAN_ASTI_PHRASE = "keskipaivaan asti";
constexpr const char* AAMUYOHON_ASTI_PHRASE = "aamuyohon asti";

constexpr const char* AAMULLA_JA_AAMUPAIVALLA_PHRASE = "aamulla ja aamupaivalla";
constexpr const char* ILTAPAIVALLA_JA_ILLALLA_PHRASE = "iltapaivalla ja illalla";
constexpr const char* ILLALLA_JA_ILTAYOLLA_PHRASE = "illalla ja iltayolla";
constexpr const char* ILTAYOLLA_JA_KESKIYOLLA_PHRASE = "iltayolla ja keskiyolla";
constexpr const char* KESKIYOLLA_JA_AAMUYOLLA_PHRASE = "keskiyolla ja aamuyolla";
constexpr const char* AAMUYOLLA_JA_AAMULLA_PHRASE = "aamuyolla ja aamulla";
constexpr const char* YOLLA_JA_AAMULLA_PHRASE = "yolla ja aamulla";
constexpr const char* AAMULLA_JA_PAIVALLA_PHRASE = "aamulla ja paivalla";
constexpr const char* PAIVALLA_JA_ILLALLA_PHRASE = "paivalla ja illalla";
constexpr const char* ILLALLA_JA_YOLLA_PHRASE = "illalla ja yolla";

constexpr const char* POHJOISESTA_ALKAEN_PHRASE = "pohjoisesta alkaen";
constexpr const char* ETELASTA_ALKAEN_PHRASE = "etelasta alkaen";
constexpr const char* IDASTA_ALKAEN_PHRASE = "idasta alkaen";
constexpr const char* LANNESTA_ALKAEN_PHRASE = "lannesta alkaen";
constexpr const char* KOILLISESTA_ALKAEN_PHRASE = "koillisesta alkaen";
constexpr const char* KAAKOSTA_ALKAEN_PHRASE = "kaakosta alkaen";
constexpr const char* LOUNAASTA_ALKAEN_PHRASE = "lounaasta alkaen";
constexpr const char* LUOTEESTA_ALKAEN_PHRASE = "luoteesta alkaen";
constexpr const char* HYVA_NAKYVYYS_PHRASE = "hyva nakyvyys";
constexpr const char* ENIMMAKSEEN_HYVA_NAKYVYYS_PHRASE = "enimmakseen hyva nakyvyys";
constexpr const char* MUUTEN_HYVA_NAKYVYYS_PHRASE = "muuten hyva nakyvyys";
constexpr const char* JA_HUONO_NAKYVYYS_PHRASE = "ja huono nakyvyys";
constexpr const char* HUONO_NAKYVYYS_PHRASE = "huono nakyvyys";

constexpr const char* SADEALUE_WORD = "sadealue";
constexpr const char* SAAPUU_WORD = "saapuu";
constexpr const char* POHJOISESTA_WORD = "pohjoisesta";
constexpr const char* ETELASTA_WORD = "etelasta";
constexpr const char* IDASTA_WORD = "idasta";
constexpr const char* LANNESTA_WORD = "lannesta";
constexpr const char* KOILLISESTA_WORD = "koillisesta";
constexpr const char* KAAKOSTA_WORD = "kaakosta";
constexpr const char* LOUNAASTA_WORD = "lounaasta";
constexpr const char* LUOTEESTA_WORD = "luoteesta";

constexpr const char* HORIZONTAL_SPLIT_KEY = "horizontal";
constexpr const char* VERTICAL_SPLIT_KEY = "vertical";

constexpr double SELKEA_UPPER_LIMIT = 19.9;
constexpr double MELKEIN_SELKEA_LOWER_LIMIT = 19.9;
constexpr double MELKEIN_SELKEA_UPPER_LIMIT = 33.0;
constexpr double PUOLIPILVISTA_LOWER_LIMIT = 33.0;
constexpr double PUOLIPILVISTA_UPPER_LIMIT = 72.0;
constexpr double VERRATTAIN_PILVISTA_LOWER_LIMIT = 72.0;
constexpr double VERRATTAIN_PILVISTA_UPPER_LIMIT = 93.0;
constexpr double PILVISTA_LOWER_LIMIT = 93.0;

constexpr double PILVISTYVAA_LOWER_LIMIT = 20.0;
constexpr double PILVISTYVAA_UPPER_LIMIT = 72.0;

constexpr double TAYSIN_PILVETON_LIMIT = 5.0;
constexpr double HYVIN_PILVINEN_LIMIT = 70.0;

constexpr double DRY_WEATHER_LIMIT_WATER = 0.04;
constexpr double DRY_WEATHER_LIMIT_DRIZZLE = 0.02;
constexpr double DRY_WEATHER_LIMIT_SLEET = 0.02;
constexpr double DRY_WEATHER_LIMIT_SNOW = 0.02;
constexpr double WEAK_PRECIPITATION_LIMIT_WATER = 0.4;
constexpr double WEAK_PRECIPITATION_LIMIT_SLEET = 0.4;
constexpr double WEAK_PRECIPITATION_LIMIT_SNOW = 0.4;
constexpr double HEAVY_PRECIPITATION_LIMIT_WATER = 4.0;
constexpr double HEAVY_PRECIPITATION_LIMIT_SLEET = 1.7;
constexpr double HEAVY_PRECIPITATION_LIMIT_SNOW = 1.5;
constexpr double RAINSTORM_LIMIT = 7.0;
constexpr double MOSTLY_DRY_WEATHER_LIMIT = 10.0;
constexpr double IN_SOME_PLACES_LOWER_LIMIT_FOG = 20.0;
constexpr double IN_SOME_PLACES_LOWER_LIMIT = 10.0;
constexpr double IN_SOME_PLACES_UPPER_LIMIT = 50.0;
constexpr double IN_MANY_PLACES_LOWER_LIMIT = 50.0;
constexpr double IN_MANY_PLACES_UPPER_LIMIT = 90.0;
constexpr int FREEZING_RAIN_LIMIT = 10;
constexpr double PRECIPITATION_FORM_REPORTING_LIMIT = 10.0;  // at least 10% must be of the type to be mentioned

constexpr double SMALL_PROBABILITY_FOR_THUNDER_LOWER_LIMIT = 0.1;
constexpr double SMALL_PROBABILITY_FOR_THUNDER_UPPER_LIMIT = 15.0;
constexpr double POSSIBLY_THUNDER_LOWER_LIMIT = 15.0;
constexpr double POSSIBLY_THUNDER_UPPER_LIMIT = 35.0;
constexpr double OCCASIONALLY_THUNDER_LOWER_LIMIT = 35.0;
constexpr double OCCASIONALLY_THUNDER_UPPER_LIMIT = 110.0;
constexpr double MAJORITY_LIMIT = 50.0;
constexpr double SEPARATE_COASTAL_AREA_PERCENTAGE = 5.0;

constexpr double TREND_CHANGE_COEFFICIENT_TRESHOLD = 0.65;  // pearson coefficient
constexpr double PEARSON_CO_FORM_TRANSFORM = 0.65;

constexpr int KESKIYO_START = 0;
constexpr int KESKIYO_END = 3;
constexpr int AAMUYO_START = 3;
constexpr int AAMUYO_END = 6;
constexpr int AAMU_START = 6;
constexpr int AAMU_END = 9;
constexpr int AAMUPAIVA_START = 9;
constexpr int AAMUPAIVA_END = 12;
constexpr int PAIVA_START = 10;
constexpr int PAIVA_END = 18;
constexpr int ILTAPAIVA_START = 12;
constexpr int ILTAPAIVA_END = 18;
constexpr int ILTA_START = 17;
constexpr int ILTA_END = 22;
constexpr int ILTAYO_START = 21;
constexpr int ILTAYO_END = 23;
constexpr int YO_START = 22;
constexpr int YO_END = 6;

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
  PRECIPITATION_FORM_FREEZING_DRIZZLE_DATA,
  PRECIPITATION_FORM_FREEZING_RAIN_DATA,
  PRECIPITATION_NORTHEAST_SHARE_DATA,
  PRECIPITATION_SOUTHEAST_SHARE_DATA,
  PRECIPITATION_SOUTHWEST_SHARE_DATA,
  PRECIPITATION_NORTHWEST_SHARE_DATA,
  PRECIPITATION_POINT_DATA,
  THUNDER_PROBABILITY_DATA,
  THUNDER_EXTENT_DATA,
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
  SELKEA,
  MELKO_SELKEA,
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
  AAMU,               // 06-09
  AAMUPAIVA,          // 09-12
  KESKIPAIVA,         // 12
  ILTAPAIVA,          // 12-18
  ILTA,               // 17-22
  ILTAYO,             // 21-24
  KESKIYO,            // 00-03
  AAMUYO,             // 02-06
  PAIVA,              // 10-18
  YO,                 // 22-06
  AAMU_JA_AAMUPAIVA,  // 06-12
  ILTAPAIVA_JA_ILTA,  // 13-21
  ILTA_JA_ILTAYO,     // 18-00
  ILTAYO_JA_KESKIYO,  // 22-03
  KESKIYO_JA_AAMUYO,  // 00-06
  AAMUYO_JA_AAMU,     // 03-09
  MISSING_PART_OF_THE_DAY_ID
};

enum weather_event_id
{
  PILVISTYY,
  SELKENEE,
  POUTAANTUU,                    // >= 6h
  POUTAANTUU_WHEN_EXTENT_SMALL,  // >= 6h, but extent is not large
  SADE_ALKAA,                    // >= 6h
  // TYPE_CHANGES,
  MISSING_WEATHER_EVENT
};

enum story_part_id
{
  PRECIPITATION_STORY_PART = 0x1,
  CLOUDINESS_STORY_PART = 0x2,
  GETTING_CLOUDY_STORY_PART = 0x4,
  CLEARING_UP_STORY_PART = 0x8,
  PRECIPITATION_TYPE_CHANGE_STORY_PART = 0x10,
  MISSING_STORY_PART = 0x0
};

enum stat_func_id
{
  MIN,
  MAX,
  MEAN
};

enum area_specific_sentence_id
{
  ALUEEN_POHJOISOSASSA,
  ALUEEN_ETELAOSASSA,
  ALUEEN_ITAOSASSA,
  ALUEEN_LANSIOSASSA,
  ENIMMAKSEEN_ALUEEN_POHJOISOSASSA,
  ENIMMAKSEEN_ALUEEN_ETELAOSASSA,
  ENIMMAKSEEN_ALUEEN_ITAOSASSA,
  ENIMMAKSEEN_ALUEEN_LANSIOSASSA,
  MISSING_AREA_SPECIFIC_SENTENCE_ID
};

enum precipitation_type
{
  CONTINUOUS = 1,
  SHOWERS,
  MISSING_PRECIPITATION_TYPE
};

enum split_method
{
  HORIZONTAL,
  VERTICAL,
  NO_SPLITTING
};

enum time_phrase_format
{
  FROM_FORMAT,
  TILL_FORMAT,
  AT_FORMAT
};

struct WeatherResultDataItem;
struct PrecipitationDataItemData;
struct FogIntensityDataItem;
struct PrecipitationDataItem;
struct CloudinessDataItem;
struct ThunderDataItem;

using weather_result_data_item_vector = std::vector<std::shared_ptr<WeatherResultDataItem> >;
using timestamp_weather_event_id_pair = std::pair<TextGenPosixTime, weather_event_id>;
using weather_event_id_vector = std::vector<timestamp_weather_event_id_pair>;
using precipitation_data_vector = std::vector<std::shared_ptr<PrecipitationDataItemData> >;
using precipitation_data_item_container = std::map<int, std::shared_ptr<PrecipitationDataItem> >;
using weather_forecast_result_container =
    std::map<int, std::shared_ptr<weather_result_data_item_vector> >;
using weather_forecast_data_container =
    std::map<int, std::shared_ptr<weather_forecast_result_container> >;
using cloudiness_data_item_container = std::map<int, std::shared_ptr<CloudinessDataItem> >;
using cloudiness_data_container = std::vector<std::shared_ptr<cloudiness_data_item_container> >;
using fog_data_item_container = std::map<int, std::shared_ptr<FogIntensityDataItem> >;
using fog_data_container = std::vector<std::shared_ptr<fog_data_item_container> >;
using weather_period_story_part_id_pair = std::pair<WeatherPeriod, unsigned int>;
using story_part_vector = std::vector<weather_period_story_part_id_pair>;

struct wf_story_params
{
  wf_story_params(const std::string& variable,
                  const WeatherArea& weatherArea,
                  WeatherPeriod dataPeriod,
                  WeatherPeriod forecastPeriod,
                  const TextGenPosixTime& forecastTime,
                  const AnalysisSources& analysisSources,
                  MessageLogger& log)
      : theVariable(variable),
        theArea(weatherArea),
        theDataPeriod(std::move(dataPeriod)),
        theForecastPeriod(std::move(forecastPeriod)),
        theForecastTime(forecastTime),
        theSources(analysisSources),
        theLog(log),
        theForecastArea(TextGen::AreaTools::NO_AREA)
  {
  }

  const std::string& theVariable;
  const WeatherArea& theArea;
  const WeatherPeriod theDataPeriod;
  const WeatherPeriod theForecastPeriod;
  const TextGenPosixTime theForecastTime;
  const AnalysisSources& theSources;
  MessageLogger& theLog;
  bool theCoastalAndInlandTogetherFlag = false;
  bool theShortTextModeFlag = false;
  PrecipitationForecast* thePrecipitationForecast = nullptr;
  CloudinessForecast* theCloudinessForecast = nullptr;
  FogForecast* theFogForecast = nullptr;
  ThunderForecast* theThunderForecast = nullptr;
  unsigned int theHourPeriodCount = 0;
  unsigned int theOriginalPeriodCount = 0;
  unsigned short theForecastArea = 0;
  float theDryWeatherLimitWater = 0;
  float theDryWeatherLimitDrizzle = 0;
  float theDryWeatherLimitSleet = 0;
  float theDryWeatherLimitSnow = 0;
  float theWeakPrecipitationLimitWater = 0;
  float theWeakPrecipitationLimitSleet = 0;
  float theWeakPrecipitationLimitSnow = 0;
  float theHeavyPrecipitationLimitWater = 0;
  float theHeavyPrecipitationLimitSleet = 0;
  float theHeavyPrecipitationLimitSnow = 0;
  float thePrecipitationStormLimit = 0;
  float theMostlyDryWeatherLimit = 0;
  float theInSomePlacesLowerLimit = 0;
  float theInSomePlacesUpperLimit = 0;
  float theInManyPlacesLowerLimit = 0;
  float theInManyPlacesUpperLimit = 0;
  float theFreezingPrecipitationLimit = 0;
  float theClearSkyUpperLimit = 0;
  float theAlmostClearSkyUpperLimit = 0;
  float thePartlyCloudySkyUpperLimit = 0;
  float theMostlyCloudySkyUpperLimit = 0;
  float theThuderNormalExtentMin = 0;
  float theThuderNormalExtentMax = 0;
  float theThunderProbabilityMin = 0;
  float theThunderProbabilityThreshold = 0;
  weather_forecast_data_container theCompleteData;
  cloudiness_data_container theCloudinessData;
  fog_data_container theFogData;
};

Sentence get_large_time_phrase(const WeatherPeriod& theWeatherPeriod,
                               bool theSpecifyDayFlag,
                               std::string& thePhraseString,
                               part_of_the_day_id& thePartOfTheDay);
std::string get_narrow_time_phrase(const WeatherPeriod& theWeatherPeriod,
                                   const std::string& theVar,
                                   part_of_the_day_id& thePartOfTheDay,
                                   bool theAlkaenPhrase = false);
void get_precipitation_limit_value(const wf_story_params& theParameters,
                                   unsigned int thePrecipitationForm,
                                   precipitation_intesity_id theIntensityId,
                                   float& theLowerLimit,
                                   float& theUpperLimit);
void get_dry_and_weak_precipitation_limit(const wf_story_params& theParameters,
                                          unsigned int thePrecipitationForm,
                                          float& theDryWeatherLimit,
                                          float& theWeakPrecipitationLimit);
precipitation_intesity_id get_precipitation_intensity_id(unsigned int thePrecipitationForm,
                                                         float thePrecipitationIntensity,
                                                         const wf_story_params& theParameters);
const char* weather_event_string(weather_event_id theWeatherEventId);
const char* precipitation_form_string(precipitation_form_id thePrecipitationForm);
const char* precipitation_type_string(precipitation_type thePrecipitationType);
const char* precipitation_traverse_string(precipitation_traverse_id thePrecipitationTraverseId);
const char* part_of_the_day_string(part_of_the_day_id thePartOfTheDayId);
const char* story_part_id_string(story_part_id theStoryPartId);

bool is_same_part_of_day(const WeatherPeriod& thePeriod1, const WeatherPeriod& thePeriod2);
void get_part_of_the_day(part_of_the_day_id thePartOfTheDayId, int& theStartHour, int& theEndHour);
part_of_the_day_id get_part_of_the_day_id_narrow(const WeatherPeriod& thePeriod,
                                                 bool ignoreKeskiyo = false);
part_of_the_day_id get_part_of_the_day_id_large(const WeatherPeriod& thePeriod);
part_of_the_day_id get_most_relevant_part_of_the_day_id_narrow(const WeatherPeriod& thePeriod);
part_of_the_day_id get_adjusted_part_of_the_day_id(const WeatherPeriod& theWeatherPeriod,
                                                   bool theAlkaenPhrase = false);
part_of_the_day_id get_part_of_the_day_id(const TextGenPosixTime& theTimestamp,
                                          bool ignoreKeskiyo = false);
bool is_inside(const WeatherPeriod& theWeatherPeriod1, const WeatherPeriod& theWeatherPeriod2);
bool is_inside(const WeatherPeriod& theWeatherPeriod, part_of_the_day_id thePartOfTheDayId);
bool is_inside(const TextGenPosixTime& theTimeStamp, part_of_the_day_id thePartOfTheDayId);
bool is_inside(const TextGenPosixTime& theTimeStamp, const WeatherPeriod& theWeatherPeriod);
bool same_period(const WeatherPeriod& theWeatherPeriod1, const WeatherPeriod& theWeatherPeriod2);
WeatherPeriod intersecting_period(const WeatherPeriod& theWeatherPeriod1,
                                  const WeatherPeriod& theWeatherPeriod2);

Sentence get_direction_phrase(AreaTools::direction_id theDirectionId,
                              bool theAlkaenPhrase = false);
Sentence get_time_phrase_large(const WeatherPeriod& theWeatherPeriod,
                               bool theSpecifyDayFlag,
                               const std::string& theVar,
                               std::string& thePhraseString,
                               bool theAlkaenPhrase,
                               part_of_the_day_id& thePartOfTheDay);
std::string get_time_phrase(const TextGenPosixTime& theTimestamp,
                            const std::string& theVar,
                            bool theAlkaenPhrase = false);
std::string get_time_phrase_from_id(part_of_the_day_id thePartOfTheDayId,
                                    const std::string& theVar,
                                    bool theAlkaenPhrase);
Sentence get_today_phrase(const TextGenPosixTime& theEventTimestamp,
                          const std::string& theVariable,
                          const WeatherArea& theArea,
                          const WeatherPeriod& thePeriod,
                          const TextGenPosixTime& theForecastTime);
std::string parse_time_phrase(short theWeekday,
                              bool theSpecifyDayFlag,
                              const std::string& theTimePhrase);
precipitation_form_id get_complete_precipitation_form(const std::string& theVariable,
                                                      float thePrecipitationFormWater,
                                                      float thePrecipitationFormDrizzle,
                                                      float thePrecipitationFormSleet,
                                                      float thePrecipitationFormSnow,
                                                      float thePrecipitationFormFreezingRain,
                                                      float thePrecipitationFormFreezingDrizzle);
void get_sub_time_series(const WeatherPeriod& thePeriod,
                         const weather_result_data_item_vector& theSourceVector,
                         weather_result_data_item_vector& theDestinationVector);
void get_sub_time_series(part_of_the_day_id thePartOfTheDay,
                         const weather_result_data_item_vector& theSourceVector,
                         weather_result_data_item_vector& theDestinationVector);
float get_mean(const weather_result_data_item_vector& theTimeSeries,
               int theStartIndex = 0,
               int theEndIndex = 0);
float get_standard_deviation(const weather_result_data_item_vector& theTimeSeries);
void get_min_max(const weather_result_data_item_vector& theTimeSeries,
                 float& theMin,
                 float& theMax);
double get_pearson_coefficient(const weather_result_data_item_vector& theTimeSeries,
                               unsigned int theStartIndex,
                               unsigned int theEndIndex,
                               bool theUseErrorValueFlag = false);
void print_out_weather_event_vector(std::ostream& theOutput,
                                    const weather_event_id_vector& theWeatherEventVector);
Sentence area_specific_sentence(float north,
                                float south,
                                float east,
                                float west,
                                float northEast,
                                float southEast,
                                float southWest,
                                float northWest,
                                bool mostlyFlag = true);
int get_today_vector(const std::string& theVariable,
                     const WeatherArea& theArea,
                     const WeatherPeriod& thePeriod,
                     const TextGenPosixTime& theForecastTime,
                     std::vector<Sentence*>& theTodayVector);
area_specific_sentence_id get_area_specific_sentence_id(float north,
                                                        float south,
                                                        float east,
                                                        float west,
                                                        float northEast,
                                                        float southEast,
                                                        float southWest,
                                                        float northWest,
                                                        bool mostlyFlag = true);
int get_period_length(const WeatherPeriod& thePeriod);
float get_area_percentage(const std::string& theVar,
                          const WeatherArea& theArea,
                          const TextGen::WeatherArea::Type& theType,
                          const AnalysisSources& theSources,
                          const WeatherPeriod& thePeriod);
std::string parse_weekday_phrase(short weekday, const std::string& part_of_the_day);
Sentence parse_weekday_phrase(short weekday, const Sentence& part_of_the_day);
WeatherPeriod get_intersection_period(const WeatherPeriod& thePeriod1,
                                      const WeatherPeriod& thePeriod2,
                                      bool& theIntersectionPeriodFound);

split_method split_the_area(const std::string& theVar,
                            const TextGen::WeatherArea& theArea,
                            const TextGen::WeatherPeriod& thePeriod,
                            const TextGen::AnalysisSources& theSources,
                            double& theDivisionLine,
                            MessageLogger& theLog);

bool test_temperature_split_criterion(const std::string& theVar,
                                      bool morningTemperature,
                                      const TextGen::WeatherArea& theAreaOne,
                                      const TextGen::WeatherArea& theAreaTwo,
                                      const TextGen::WeatherPeriod& thePeriod,
                                      const TextGen::AnalysisSources& theSources,
                                      MessageLogger& theLog);
split_method check_area_splitting(const std::string& theVar,
                                  const TextGen::WeatherArea& theArea,
                                  const TextGen::WeatherPeriod& thePeriod,
                                  const TextGen::AnalysisSources& theSources,
                                  TextGen::WeatherArea& theFirstArea,
                                  TextGen::WeatherArea& theSecondArea,
                                  MessageLogger& theLog);
bool fit_into_narrow_day_part(const WeatherPeriod& thePeriod);
bool fit_into_large_day_part(const WeatherPeriod& thePeriod);
// std::ostream& operator<<(std::ostream& theOutput, const WeatherPeriod& period);
std::string as_string(const WeatherPeriod& period);
std::string as_string(const GlyphContainer& gc);
std::string as_string(const WeatherResult& wr);

struct WeatherResultDataItem
{
  WeatherResultDataItem(WeatherPeriod period,
                        const WeatherResult& result,
                        part_of_the_day_id partOfTheDay)
      : thePeriod(std::move(period)), theResult(result), thePartOfTheDay(partOfTheDay)
  {
  }

  WeatherPeriod thePeriod;
  WeatherResult theResult;
  part_of_the_day_id thePartOfTheDay;
};

struct CloudinessDataItemData
{
  CloudinessDataItemData(cloudiness_id id,
                         float min,
                         float mean,
                         float max,
                         float standardDeviation,
                         weather_event_id weatherEventId,
                         float pearsonCoefficient)
      : theId(id),
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
                     const CloudinessDataItemData* fullData)
      : theCoastalData(coastalData), theInlandData(inlandData), theFullData(fullData)
  {
  }

  const CloudinessDataItemData* theCoastalData;
  const CloudinessDataItemData* theInlandData;
  const CloudinessDataItemData* theFullData;

  ~CloudinessDataItem()
  {
    delete theCoastalData;

    delete theInlandData;

    delete theFullData;
  }
  CloudinessDataItem(const CloudinessDataItem&) = delete;
  CloudinessDataItem& operator=(const CloudinessDataItem&) = delete;
};

struct ThunderDataItem
{
  ThunderDataItem(float minProbability,
                  float meanProbability,
                  float maxProbability,
                  float standardDeviationProbability)
      : theMinProbability(minProbability),
        theMeanProbability(meanProbability),
        theMaxProbability(maxProbability),
        theStandardDeviationProbability(standardDeviationProbability)
  {
  }

  float theMinProbability;
  float theMeanProbability;
  float theMaxProbability;
  float theStandardDeviationProbability;
};

struct FogIntensityDataItem
{
  FogIntensityDataItem(float moderateFogExtent, float denseFogExtent)
      : theModerateFogExtent(moderateFogExtent), theDenseFogExtent(denseFogExtent)
  {
  }

  float theModerateFogExtent;
  float theDenseFogExtent;
};

struct PrecipitationDataItemData
{
  PrecipitationDataItemData(const wf_story_params& theParameters,
                            precipitation_form_id precipitationForm,
                            float precipitationIntensity,
                            float precipitationMaxIntensity,
                            float precipitationExtent,
                            float precipitationFormWater,
                            float precipitationFormDrizzle,
                            float precipitationFormSleet,
                            float precipitationFormSnow,
                            float precipitationFormFreezingRain,
                            float precipitationFormFreezingDrizzle,
                            float precipitationTypeShowers,
                            weather_event_id weatherEventId,
                            float pearsonCoefficient,
                            const TextGenPosixTime& observationTime)
      : thePrecipitationForm(precipitationForm),
        thePrecipitationIntensity(precipitationIntensity),
        thePrecipitationMaxIntensity(precipitationMaxIntensity),
        thePrecipitationExtent(precipitationExtent),
        thePrecipitationFormWater(precipitationFormWater),
        thePrecipitationFormDrizzle(precipitationFormDrizzle),
        thePrecipitationFormSleet(precipitationFormSleet),
        thePrecipitationFormSnow(precipitationFormSnow),
        thePrecipitationFormFreezingRain(precipitationFormFreezingRain),
        thePrecipitationFormFreezingDrizzle(precipitationFormFreezingDrizzle),
        thePrecipitationTypeShowers(precipitationTypeShowers),
        theWeatherEventId(weatherEventId),
        thePearsonCoefficient(pearsonCoefficient),
        theObservationTime(observationTime)
  {
    if (precipitationTypeShowers != kFloatMissing)
      thePrecipitationType = precipitationTypeShowers >= MAJORITY_LIMIT ? SHOWERS : CONTINUOUS;

    if (precipitationIntensity == kFloatMissing)
    {
      thePrecipitationIntensityId = DRY_WEATHER;
    }
    else
    {
      float lowerLimit(0.0);
      float dryUpperLimit(0.0);
      float weakUpperLimit(0.0);
      float moderateUpperLimit(0.0);
      get_precipitation_limit_value(
          theParameters, precipitationForm, DRY_WEATHER, lowerLimit, dryUpperLimit);
      get_precipitation_limit_value(
          theParameters, precipitationForm, WEAK_PRECIPITATION, lowerLimit, weakUpperLimit);
      get_precipitation_limit_value(
          theParameters, precipitationForm, MODERATE_PRECIPITATION, lowerLimit, moderateUpperLimit);

      if (precipitationIntensity < dryUpperLimit)
        thePrecipitationIntensityId = DRY_WEATHER;
      else if (precipitationIntensity < weakUpperLimit)
        thePrecipitationIntensityId = WEAK_PRECIPITATION;
      else if (precipitationIntensity < moderateUpperLimit)
        thePrecipitationIntensityId = MODERATE_PRECIPITATION;
      else
        thePrecipitationIntensityId = HEAVY_PRECIPITATION;
    }
  }

  precipitation_form_id thePrecipitationForm;
  float thePrecipitationIntensity = 0;
  float thePrecipitationMaxIntensity = 0;
  float thePrecipitationExtent = 0;
  float thePrecipitationFormWater = 0;
  float thePrecipitationFormDrizzle = 0;
  float thePrecipitationFormSleet = 0;
  float thePrecipitationFormSnow = 0;
  float thePrecipitationFormFreezingRain = 0;
  float thePrecipitationFormFreezingDrizzle = 0;
  float thePrecipitationTypeShowers = 0;
  weather_event_id theWeatherEventId;
  float thePearsonCoefficient = 0;
  TextGenPosixTime theObservationTime;
  float thePrecipitationPercentageNorthEast = 0;
  float thePrecipitationPercentageSouthEast = 0;
  float thePrecipitationPercentageSouthWest = 0;
  float thePrecipitationPercentageNorthWest = 0;
  weather_result_data_item_vector thePrecipitationShareNorthEast;
  weather_result_data_item_vector thePrecipitationShareSouthEast;
  weather_result_data_item_vector thePrecipitationShareSouthWest;
  weather_result_data_item_vector thePrecipitationShareNorthWest;
  NFmiPoint thePrecipitationPoint{0, 0};
  precipitation_type thePrecipitationType = MISSING_PRECIPITATION_TYPE;
  precipitation_intesity_id thePrecipitationIntensityId = MISSING_INTENSITY_ID;

  float precipitationPercentageNorth() const
  {
    return thePrecipitationPercentageNorthEast + thePrecipitationPercentageNorthWest;
  }
  float precipitationPercentageSouth() const
  {
    return thePrecipitationPercentageSouthEast + thePrecipitationPercentageSouthWest;
  }
  float precipitationPercentageEast() const
  {
    return thePrecipitationPercentageNorthEast + thePrecipitationPercentageSouthEast;
  }
  float precipitationPercentageWest() const
  {
    return thePrecipitationPercentageNorthWest + thePrecipitationPercentageSouthWest;
  }

  bool operator==(const PrecipitationDataItemData& theItem) const
  {
    return (thePrecipitationForm == theItem.thePrecipitationForm &&
            thePrecipitationIntensity == theItem.thePrecipitationIntensity &&
            thePrecipitationExtent == theItem.thePrecipitationExtent &&
            thePrecipitationFormWater == theItem.thePrecipitationFormWater &&
            thePrecipitationFormDrizzle == theItem.thePrecipitationFormDrizzle &&
            thePrecipitationFormSleet == theItem.thePrecipitationFormSleet &&
            thePrecipitationFormSnow == theItem.thePrecipitationFormSnow &&
            thePrecipitationFormFreezingRain == theItem.thePrecipitationFormFreezingRain &&
            thePrecipitationFormFreezingDrizzle == theItem.thePrecipitationFormFreezingDrizzle &&
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
                        const PrecipitationDataItemData* fullData)
      : theCoastalData(coastalData), theInlandData(inlandData), theFullData(fullData)
  {
  }

  const PrecipitationDataItemData* theCoastalData;
  const PrecipitationDataItemData* theInlandData;
  const PrecipitationDataItemData* theFullData;

  ~PrecipitationDataItem()
  {
    delete theCoastalData;

    delete theInlandData;

    delete theFullData;
  }
  PrecipitationDataItem(const PrecipitationDataItem&) = delete;
  PrecipitationDataItem& operator=(const PrecipitationDataItem&) = delete;
};

}  // namespace TextGen
