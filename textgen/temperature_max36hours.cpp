// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::TemperatureStory::max36hours
 */
// ======================================================================

#include "AreaTools.h"
#include "ClimatologyTools.h"
#include "DebugTextFormatter.h"
#include "Delimiter.h"
#include "FrostStory.h"
#include "GridClimatology.h"
#include "Integer.h"
#include "MessageLogger.h"
#include "NightAndDayPeriodGenerator.h"
#include "NorthernMaskSource.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "SeasonTools.h"
#include "Sentence.h"
#include "SouthernMaskSource.h"
#include "TemperatureStory.h"
#include "TemperatureStoryTools.h"
#include "UnitFactory.h"
#include "WeatherForecast.h"
#include "WeekdayTools.h"
#include <boost/lexical_cast.hpp>
#include <calculator/DefaultAcceptor.h>
#include <calculator/GridForecaster.h>
#include <calculator/HourPeriodGenerator.h>
#include <calculator/MathTools.h>
#include <calculator/Settings.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherPeriodTools.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <calculator/WeatherSource.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiGlobals.h>
#include <newbase/NFmiGrid.h>
#include <newbase/NFmiIndexMask.h>
#include <newbase/NFmiIndexMaskSource.h>
#include <newbase/NFmiIndexMaskTools.h>
#include <newbase/NFmiQueryData.h>
#include <newbase/NFmiStringTools.h>
#include <cstdio>
#include <map>
#include <vector>

using namespace std;

namespace TextGen
{
// std::string as_string(const GlyphContainer& gc);

namespace TemperatureMax36Hours
{
using namespace TextGen::TemperatureStoryTools;

using namespace Settings;
using namespace AreaTools;
using namespace SeasonTools;
using Settings::optional_bool;
using Settings::optional_int;
using Settings::optional_string;

#define HUOMENNA_SISAMAASSA_PAKKASTA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] pakkasta on [M...N] [astetta]"

#define HUOMENNA_SISAMAASSA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] pakkasta on noin [N] [astetta]"
#define HUOMENNA_SISAMAASSA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] pakkanen on [N] [asteen] tienoilla"
#define HUOMENNA_SISAMAASSA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] pakkanen on [N] [asteen] tuntumassa"

#define HUOMENNA_SISAMAASSA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] pakkanen on lahella [N] [astetta]"
#define HUOMENNA_SISAMAASSA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] pakkasta on vajaat [N] [astetta]"
#define HUOMENNA_SISAMAASSA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] pakkasta on vahan yli [N] [astetta]"

#define HUOMENNA_PAKKASTA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] pakkasta on [M...N] [astetta]"
#define HUOMENNA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] pakkasta on noin [N] [astetta]"
#define HUOMENNA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] pakkanen on [N] [asteen] tienoilla"
#define HUOMENNA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] pakkanen on [N] [asteen] tuntumassa"

#define HUOMENNA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] pakkanen on lahella [N] [astetta]"
#define HUOMENNA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] pakkasta on vajaat [N] [astetta]"
#define HUOMENNA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] pakkasta on vahan yli [N] [astetta]"
#define SISAMAASSA_PAKKASTA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] pakkasta on [M...N] [astetta]"
#define SISAMAASSA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] pakkasta on noin [N] [astetta]"
#define SISAMAASSA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] pakkanen on [N] [asteen] tienoilla"
#define SISAMAASSA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] pakkanen on [N] [asteen] tuntumassa"

#define SISAMAASSA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] pakkanen on lahella [N] [astetta]"
#define SISAMAASSA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] pakkasta on vajaat [N] [astetta]"
#define SISAMAASSA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] pakkasta on vahan yli [N] [astetta]"
#define PAKKASTA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE "pakkasta on [M...N] [astetta]"
#define PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE "pakkasta on noin [N] [astetta]"
#define PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE "pakkanen on [N] [asteen] tienoilla"
#define PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE "pakkanen on [N] [asteen] tuntumassa"
#define PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE "pakkanen on lahella [N] [astetta]"
#define PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE "pakkasta on vajaat [N] [astetta]"
#define PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE "pakkasta on vahan yli [N] [astetta]"

#define LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE "[lampotila] on [M...N] [astetta]"
#define LAMPOTILA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE "[lampotila] [M...N] [astetta]"
#define LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE "[lampotila] on noin [N] [astetta]"
#define LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE "[lampotila] on [N] [asteen] tienoilla"
#define LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE "[lampotila] on [N] [asteen] tuntumassa"
#define LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE "[lampotila] on lahella [N] [astetta]"
#define LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE "[lampotila] on vajaat [N] [astetta]"
#define LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE "[lampotila] on vahan yli [N] [astetta]"
#define LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE \
  "[lampotila] on vahan nollan alapuolella"
#define HUOMENNA_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE "[huomenna] vahan nollan alapuolella"
#define HUOMENNA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on vahan nollan alapuolella"
#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on vahan nollan alapuolella"

#define LAMPOTILA_NOIN_ASTETTA_COMPOSITE_PHRASE "[lampotila] noin [N] [astetta]"
#define LAMPOTILA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE "[lampotila] [N] [asteen] tienoilla"
#define LAMPOTILA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE "[lampotila] [N] [asteen] tuntumassa"
#define LAMPOTILA_LAHELLA_ASTETTA_COMPOSITE_PHRASE "[lampotila] lahella [N] [astetta]"
#define LAMPOTILA_VAJAAT_ASTETTA_COMPOSITE_PHRASE "[lampotila] vajaat [N] [astetta]"
#define LAMPOTILA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE "[lampotila] vahan yli [N] [astetta]"
#define LAMPOTILA_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE "[lampotila] vahan nollan alapuolella"

#define LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE "[lampotila] on [suunnilleen sama]"
#define LAMPOTILA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE "[lampotila] [suunnilleen sama]"

#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on [M...N] [astetta]"
#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on noin [N] [astetta]"
#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on [N] [asteen] tienoilla"
#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on [N] [asteen] tuntumassa"
#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on lahella [N] [astetta]"
#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on vajaat [N] [astetta]"
#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on vahan yli [N] [astetta]"
#define HUOMENNA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on [M...N] [astetta]"
#define HUOMENNA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on noin [N] [astetta]"
#define HUOMENNA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on [N] [asteen] tienoilla"
#define HUOMENNA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on [N] [asteen] tuntumassa"
#define HUOMENNA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on lahella [N] [astetta]"
#define HUOMENNA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on vajaat [N] [astetta]"
#define HUOMENNA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on vahan yli [N] [astetta]"
#define SISAMAASSA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on [M...N] [astetta]"
#define SISAMAASSA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on noin [N] [astetta]"
#define SISAMAASSA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on [N] [asteen] tienoilla"
#define SISAMAASSA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on [N] [asteen] tuntumassa"
#define SISAMAASSA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on lahella [N] [astetta]"
#define SISAMAASSA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on vajaat [N] [astetta]"
#define SISAMAASSA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on vahan yli [N] [astetta]"
#define SISAMAASSA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on vahan nollan alapuolella"

#define HUOMENNA_RANNIKOLLA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE \
  "[huomenna] [rannikolla] on pikkupakkasta"
#define HUOMENNA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE "[huomenna] on pikkupakkasta"
#define RANNIKOLLA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE "[rannikolla] on pikkupakkasta"

#define HUOMENNA_SISAMAASSA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] pakkanen on [hieman kireampaa]"
#define HUOMENNA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE \
  "[huomenna] pakkanen on [hieman kireampaa]"
#define SISAMAASSA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE \
  "[sisamaassa] pakkanen on [hieman kireampaa]"

#define LAMPOTILA_NOUSEE_COMPOSITE_PHRASE "[yolla] lampotila nousee"

#define HUOMENNA_SISAMAASSA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [lampotila] on [suunnilleen sama]"
#define HUOMENNA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE \
  "[huomenna] [lampotila] on [suunnilleen sama]"
#define SISAMAASSA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE \
  "[sisamaassa] [lampotila] on [suunnilleen sama]"

#define RANNIKOLLA_LAHELLA_ASTETTA_COMPOSITE_PHRASE "rannikolla lahella [N] [astetta]"
#define RANNIKOLLA_NOIN_ASTETTA_COMPOSITE_PHRASE "rannikolla noin [N] [astetta]"
#define RANNIKOLLA_VAJAAT_ASTETTA_COMPOSITE_PHRASE "rannikolla vajaat [N] [astetta]"
#define RANNIKOLLA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE "rannikolla vahan yli [N] [astetta]"
#define RANNIKOLLA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE "rannikolla [M...N] [astetta]"
#define RANNIKOLLA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE "rannikolla [N] [asteen] tienoilla"
#define RANNIKOLLA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE "rannikolla [N] [asteen] tuntumassa"
#define RANNIKOLLA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE "rannikolla [suunnilleen sama]"
#define RANNIKOLLA_VAHAN_NOLLAN_ALAPUOLELLA_PHRASE "rannikolla vahan nollan alapuolella"

#define HUOMENNA_SISAMAASSA_LAHELLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] lahella [N] [astetta]"
#define HUOMENNA_SISAMAASSA_NOIN_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] noin [N] [astetta]"
#define HUOMENNA_SISAMAASSA_VAJAAT_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] vajaat [N] [astetta]"
#define HUOMENNA_SISAMAASSA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] vahan yli [N] [astetta]"
#define HUOMENNA_SISAMAASSA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [M...N] [astetta]"
#define HUOMENNA_SISAMAASSA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [N] [asteen] tienoilla"
#define HUOMENNA_SISAMAASSA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [N] [asteen] tuntumassa"
#define HUOMENNA_SISAMAASSA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] [suunnilleen sama]"
#define HUOMENNA_SISAMAASSA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE \
  "[huomenna] [sisamaassa] on [hieman lauhempaa]"
#define HUOMENNA_LAHELLA_ASTETTA_COMPOSITE_PHRASE "[huomenna] lahella [N] [astetta]"
#define HUOMENNA_NOIN_ASTETTA_COMPOSITE_PHRASE "[huomenna] noin [N] [astetta]"
#define HUOMENNA_VAJAAT_ASTETTA_COMPOSITE_PHRASE "[huomenna] vajaat [N] [astetta]"
#define HUOMENNA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE "[huomenna] vahan yli [N] [astetta]"
#define HUOMENNA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE "[huomenna] [M...N] [astetta]"
#define HUOMENNA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE "[huomenna] [N] [asteen] tienoilla"
#define HUOMENNA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE "[huomenna] [N] [asteen] tuntumassa"
#define HUOMENNA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE "[huomenna] [suunnilleen sama]"
#define HUOMENNA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE "[huomenna] on [hieman lauhempaa]"
#define SISAMAASSA_LAHELLA_ASTETTA_COMPOSITE_PHRASE "[sisamaassa] lahella [N] [astetta]"
#define SISAMAASSA_NOIN_ASTETTA_COMPOSITE_PHRASE "[sisamaassa] noin [N] [astetta]"
#define SISAMAASSA_VAJAAT_ASTETTA_COMPOSITE_PHRASE "[sisamaassa] vajaat [N] [astetta]"
#define SISAMAASSA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE "[sisamaassa] vahan yli [N] [astetta]"
#define SISAMAASSA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE "[sisamaassa] [M...N] [astetta]"
#define SISAMAASSA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE "[sisamaassa] [N] [asteen] tienoilla"
#define SISAMAASSA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE "[sisamaassa] [N] [asteen] tuntumassa"
#define SISAMAASSA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE "[sisamaassa] [suunnilleen sama]"
#define SISAMAASSA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE "[sisamaassa] on [hieman lauhempaa]"

#define COASTAL_AREA_IGNORE_LIMIT (-15.0)
#define DAY_NIGHT_SEPARATION_LIMIT 3.0
#define MORNING_AFTERNOON_SEPARATION_LIMIT 3.0
#define ABOUT_THE_SAME_UPPER_LIMIT 2.0
#define AROUND_ZERO_UPPER_LIMIT 2.5
#define SMALL_CHANGE_UPPER_LIMIT 4.5

#define NOIN_ASTETTA_LOW_TEMP_LIMIT 2.5
#define NOIN_ASTETTA_HIGH_TEMP_LIMIT 1.5
#define TIENOILLA_ASTETTA_LOW_TEMP_LIMIT 3.0
#define TIENOILLA_ASTETTA_HIGH_TEMP_LIMIT 2.0
#define LAHELLA_ASTETTA_ALI 2.0
#define LAHELLA_ASTETTA_YLI 1.0
#define TUNTUMASSA_ASTETTA_ALI 1.5
#define TUNTUMASSA_ASTETTA_YLI 0.5
#define VAJAAT_ASTETTA_LIMIT 2.0
#define VAHAN_YLI_ASTETTA_LOW_TEMP_LIMIT 3.0
#define VAHAN_YLI_ASTETTA_HIGH_TEMP_LIMIT 2.0
#define PAKKASRAJA_TEMPERATURE (-20.0)

#define SUUNNILLEEN_SAMA_PHRASE "suunnilleen sama"
#define HIEMAN_KORKEAMPI_PHRASE "hieman korkeampi"
#define HIEMAN_ALEMPI_PHRASE "hieman alempi"
#define HIEMAN_LAUHEMPAA_PHRASE "hieman lauhempaa"
#define HIEMAN_KYLMEMPAA_PHRASE "hieman kylmempaa"
#define HIEMAN_HEIKOMPAA_PHRASE "hieman heikompaa"
#define HIEMAN_KIREAMPAA_PHRASE "hieman kireampaa"

#define LAMPOTILA_NOUSEE_PHRASE "lampotila nousee"
#define NOLLAN_TIENOILLA_PHRASE "nollan tienoilla"
#define VAHAN_PLUSSAN_PUOLELLA_PHRASE "vahan plussan puolella"
#define PIKKUPAKKASTA_PHRASE "pikkupakkasta"
#define SISAMAASSA_PHRASE "sisamaassa"
#define RANNIKOLLA_PHRASE "rannikolla"
#define AAMULLA_PHRASE "aamulla"
#define ILTAPAIVALLA_PHRASE "iltapaivalla"
#define PAIVALLA_PHRASE "paivalla"
#define YOLLA_PHRASE "yolla"
#define PAIVAN_YLIN_LAMPOTILA_LONG_PHRASE "paivan ylin lampotila"
#define YON_ALIN_LAMPOTILA_LONG_PHRASE "yon alin lampotila"
#define PAIVAN_YLIN_LAMPOTILA_SHORT_PHRASE "paivan ylin"
#define YON_ALIN_LAMPOTILA_SHORT_PHRASE "yon alin"

#define YOLAMPOTILA_PHRASE "yolampotila"
#define PAIVALAMPOTILA_PHRASE "paivalampotila"
#define LAMPOTILA_WORD "lampotila"
#define PAKKASTA_WORD "pakkasta"
#define PAKKANEN_WORD "pakkanen"
#define ON_WORD "on"
#define NOIN_PHRASE "noin"
#define TIENOILLA_PHRASE "[1] asteen tienoilla"
#define LAHELLA_PHRASE "lahella [1] astetta"
#define TUNTUMASSA_PHRASE "[1] asteen tuntumassa"

#define VAHAN_NOLLAN_ALAPUOLELLA_PHRASE "vahan nolla alapuolella"
#define VAJAAT_PHRASE "vajaat"
#define VAHAN_PHRASE "vahan"
#define YLI_PHRASE "yli"
#define TAI_PHRASE "tai"
#define NOLLA_WORD "nolla"

#define NOLLA_ASTETTA_PHRASE "0 astetta"

enum proximity_id
{
  NOIN_ASTETTA,
  TIENOILLA_ASTETTA,
  LAHELLA_ASTETTA,
  TUNTUMASSA_ASTETTA,
  VAJAAT_ASTETTA,
  VAHAN_YLI_ASTETTA,
  NO_PROXIMITY
};

enum temperature_phrase_id
{
  VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID,
  NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID,
  SUUNNILLEEN_SAMA_PHRASE_ID,
  HIEMAN_KORKEAMPI_PHRASE_ID,
  HIEMAN_ALEMPI_PHRASE_ID,
  HIEMAN_LAUHEMPAA_PHRASE_ID,
  HIEMAN_KYLMEMPAA_PHRASE_ID,
  HIEMAN_HEIKOMPAA_PHRASE_ID,
  HIEMAN_KIREAMPAA_PHRASE_ID,
  LAMPOTILA_NOUSEE_PHRASE_ID,
  PIKKUPAKKASTA_PHRASE_ID,
  NOLLAN_TIENOILLA_PHRASE_ID,
  VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID,
  NOIN_ASTETTA_PHRASE_ID,
  TIENOILLA_ASTETTA_PHRASE_ID,
  LAHELLA_ASTETTA_PHRASE_ID,
  TUNTUMASSA_ASTETTA_PHRASE_ID,
  VAJAAT_ASTETTA_PHRASE_ID,
  VAHAN_YLI_ASTETTA_PHRASE_ID,
  LAMPOTILA_VALILLA_PHRASE_ID,
  NO_PHRASE_ID
};

enum forecast_period_id
{
  DAY1_PERIOD = 0x1,
  NIGHT_PERIOD = 0x2,
  DAY2_PERIOD = 0x4,
  NO_PERIOD = 0x0
};

enum forecast_subperiod_id
{
  DAY1_MORNING_PERIOD = 0x1,
  DAY1_AFTERNOON_PERIOD = 0x2,
  DAY2_MORNING_PERIOD = 0x4,
  DAY2_AFTERNOON_PERIOD = 0x8,
  UNDEFINED_SUBPERIOD = 0x0
};

enum processing_order
{
  DAY1,
  DAY1_DAY2_NIGHT,
  DAY1_NIGHT,
  NIGHT_DAY2,
  NIGHT,
  UNDEFINED_PROCESSING_ORDER
};

enum sentence_part_id
{
  DAY1_INLAND,
  DAY1_COASTAL,
  DAY1_FULL,
  NIGHT_INLAND,
  NIGHT_COASTAL,
  NIGHT_FULL,
  DAY2_INLAND,
  DAY2_COASTAL,
  DAY2_FULL,
  DELIMITER_COMMA,
  DELIMITER_DOT
};

enum weather_result_id
{
  AREA_MIN_DAY1 = 0x1,
  AREA_MAX_DAY1,
  AREA_MEAN_DAY1,
  AREA_MIN_NIGHT,
  AREA_MAX_NIGHT,
  AREA_MEAN_NIGHT,
  AREA_MIN_DAY2,
  AREA_MAX_DAY2,
  AREA_MEAN_DAY2,
  AREA_MIN_DAY1_MORNING,
  AREA_MAX_DAY1_MORNING,
  AREA_MEAN_DAY1_MORNING,
  AREA_MIN_DAY2_MORNING,
  AREA_MAX_DAY2_MORNING,
  AREA_MEAN_DAY2_MORNING,
  AREA_MIN_DAY1_AFTERNOON,
  AREA_MAX_DAY1_AFTERNOON,
  AREA_MEAN_DAY1_AFTERNOON,
  AREA_MIN_DAY2_AFTERNOON,
  AREA_MAX_DAY2_AFTERNOON,
  AREA_MEAN_DAY2_AFTERNOON,
  INLAND_MIN_DAY1,
  INLAND_MAX_DAY1,
  INLAND_MEAN_DAY1,
  INLAND_MIN_NIGHT,
  INLAND_MAX_NIGHT,
  INLAND_MEAN_NIGHT,
  INLAND_MIN_DAY2,
  INLAND_MAX_DAY2,
  INLAND_MEAN_DAY2,
  INLAND_MIN_DAY1_MORNING,
  INLAND_MAX_DAY1_MORNING,
  INLAND_MEAN_DAY1_MORNING,
  INLAND_MIN_DAY2_MORNING,
  INLAND_MAX_DAY2_MORNING,
  INLAND_MEAN_DAY2_MORNING,
  INLAND_MIN_DAY1_AFTERNOON,
  INLAND_MAX_DAY1_AFTERNOON,
  INLAND_MEAN_DAY1_AFTERNOON,
  INLAND_MIN_DAY2_AFTERNOON,
  INLAND_MAX_DAY2_AFTERNOON,
  INLAND_MEAN_DAY2_AFTERNOON,
  COAST_MIN_DAY1,
  COAST_MAX_DAY1,
  COAST_MEAN_DAY1,
  COAST_MIN_NIGHT,
  COAST_MAX_NIGHT,
  COAST_MEAN_NIGHT,
  COAST_MIN_DAY2,
  COAST_MAX_DAY2,
  COAST_MEAN_DAY2,
  COAST_MIN_DAY1_MORNING,
  COAST_MAX_DAY1_MORNING,
  COAST_MEAN_DAY1_MORNING,
  COAST_MIN_DAY2_MORNING,
  COAST_MAX_DAY2_MORNING,
  COAST_MEAN_DAY2_MORNING,
  COAST_MIN_DAY1_AFTERNOON,
  COAST_MAX_DAY1_AFTERNOON,
  COAST_MEAN_DAY1_AFTERNOON,
  COAST_MIN_DAY2_AFTERNOON,
  COAST_MAX_DAY2_AFTERNOON,
  COAST_MEAN_DAY2_AFTERNOON,
  UNDEFINED_WEATHER_RESULT_ID
};

namespace
{

std::string weather_result_string(const std::string& areaName, weather_result_id id, bool isWinter)
{
  std::string timeFunDay = (isWinter ? "mean" : "maximum");
  std::string timeFunNight = (isWinter ? "mean" : "minimum");

  switch (id)
  {
    case AREA_MIN_DAY1:
      return areaName + " - area day1 " + timeFunDay + "(minimum): ";
    case AREA_MAX_DAY1:
      return areaName + " - area day1 " + timeFunDay + "(maximum): ";
    case AREA_MEAN_DAY1:
      return areaName + " - area day1 " + timeFunDay + "(mean): ";
    case AREA_MIN_NIGHT:
      return areaName + " - area night " + timeFunNight + "(minimum): ";
    case AREA_MAX_NIGHT:
      return areaName + " - area night " + timeFunNight + "(maximum): ";
    case AREA_MEAN_NIGHT:
      return areaName + " - area night " + timeFunNight + "(mean): ";
    case AREA_MIN_DAY2:
      return areaName + " - area day2 " + timeFunDay + "(minimum): ";
    case AREA_MAX_DAY2:
      return areaName + " - area day2 " + timeFunDay + "(maximum): ";
    case AREA_MEAN_DAY2:
      return areaName + " - area day2 " + timeFunDay + "(mean): ";
    case AREA_MIN_DAY1_MORNING:
      return areaName + " - area day1 morning " + timeFunDay + "(minimum): ";
    case AREA_MAX_DAY1_MORNING:
      return areaName + " - area day1 morning " + timeFunDay + "(maximum): ";
    case AREA_MEAN_DAY1_MORNING:
      return areaName + " - area day1  morning " + timeFunDay + "(mean): ";
    case AREA_MIN_DAY2_MORNING:
      return areaName + " - area day2  morning " + timeFunDay + "(minimum): ";
    case AREA_MAX_DAY2_MORNING:
      return areaName + " - area day2 morning " + timeFunDay + "(maximum): ";
    case AREA_MEAN_DAY2_MORNING:
      return areaName + " - area day2 morning " + timeFunDay + "(mean): ";
    case AREA_MIN_DAY1_AFTERNOON:
      return areaName + " - area day1 afternoon " + timeFunDay + "(minimum): ";
    case AREA_MAX_DAY1_AFTERNOON:
      return areaName + " - area day1 afternoon " + timeFunDay + "(maximum): ";
    case AREA_MEAN_DAY1_AFTERNOON:
      return areaName + " - area day1 afternoon " + timeFunDay + "(mean): ";
    case AREA_MIN_DAY2_AFTERNOON:
      return areaName + " - area day2 afternoon " + timeFunDay + "(minimum): ";
    case AREA_MAX_DAY2_AFTERNOON:
      return areaName + " - area day2 afternoon " + timeFunDay + "(maximum): ";
    case AREA_MEAN_DAY2_AFTERNOON:
      return areaName + " - area day2 afternoon " + timeFunDay + "(mean): ";
    case INLAND_MIN_DAY1:
      return areaName + " - inland day1 " + timeFunDay + "(minimum): ";
    case INLAND_MAX_DAY1:
      return areaName + " - inland day1 " + timeFunDay + "(maximum): ";
    case INLAND_MEAN_DAY1:
      return areaName + " - inland day1 " + timeFunDay + "(mean): ";
    case INLAND_MIN_NIGHT:
      return areaName + " - inland night " + timeFunNight + "(minimum): ";
    case INLAND_MAX_NIGHT:
      return areaName + " - inland night " + timeFunNight + "(maximum): ";
    case INLAND_MEAN_NIGHT:
      return areaName + " - inland night " + timeFunNight + "(mean): ";
    case INLAND_MIN_DAY2:
      return areaName + " - inland day2 " + timeFunDay + "(minimum): ";
    case INLAND_MAX_DAY2:
      return areaName + " - inland day2 " + timeFunDay + "(maximum): ";
    case INLAND_MEAN_DAY2:
      return areaName + " - inland day2 " + timeFunDay + "(mean): ";
    case INLAND_MIN_DAY1_MORNING:
      return areaName + " - inland day1 morning " + timeFunDay + "(minimum): ";
    case INLAND_MAX_DAY1_MORNING:
      return areaName + " - inland day1 morning " + timeFunDay + "(maximum): ";
    case INLAND_MEAN_DAY1_MORNING:
      return areaName + " - inland day1  morning " + timeFunDay + "(mean): ";
    case INLAND_MIN_DAY2_MORNING:
      return areaName + " - inland day2  morning " + timeFunDay + "(minimum): ";
    case INLAND_MAX_DAY2_MORNING:
      return areaName + " - inland day2 morning " + timeFunDay + "(maximum): ";
    case INLAND_MEAN_DAY2_MORNING:
      return areaName + " - inland day2 morning " + timeFunDay + "(mean): ";
    case INLAND_MIN_DAY1_AFTERNOON:
      return areaName + " - inland day1 afternoon " + timeFunDay + "(minimum): ";
    case INLAND_MAX_DAY1_AFTERNOON:
      return areaName + " - inland day1 afternoon " + timeFunDay + "(maximum): ";
    case INLAND_MEAN_DAY1_AFTERNOON:
      return areaName + " - inland day1 afternoon " + timeFunDay + "(mean): ";
    case INLAND_MIN_DAY2_AFTERNOON:
      return areaName + " - inland day2 afternoon " + timeFunDay + "(minimum): ";
    case INLAND_MAX_DAY2_AFTERNOON:
      return areaName + " - inland day2 afternoon " + timeFunDay + "(maximum): ";
    case INLAND_MEAN_DAY2_AFTERNOON:
      return areaName + " - inland day2 afternoon " + timeFunDay + "(mean): ";
    case COAST_MIN_DAY1:
      return areaName + " - coast day1 " + timeFunDay + "(minimum): ";
    case COAST_MAX_DAY1:
      return areaName + " - coast day1 " + timeFunDay + "(maximum): ";
    case COAST_MEAN_DAY1:
      return areaName + " - coast day1 " + timeFunDay + "(mean): ";
    case COAST_MIN_NIGHT:
      return areaName + " - coast night " + timeFunNight + "(minimum): ";
    case COAST_MAX_NIGHT:
      return areaName + " - coast night " + timeFunNight + "(maximum): ";
    case COAST_MEAN_NIGHT:
      return areaName + " - coast night " + timeFunNight + "(mean): ";
    case COAST_MIN_DAY2:
      return areaName + " - coast day2 " + timeFunDay + "(minimum): ";
    case COAST_MAX_DAY2:
      return areaName + " - coast day2 " + timeFunDay + "(maximum): ";
    case COAST_MEAN_DAY2:
      return areaName + " - coast day2 " + timeFunDay + "(mean): ";
    case COAST_MIN_DAY1_MORNING:
      return areaName + " - coast day1 morning " + timeFunDay + "(minimum): ";
    case COAST_MAX_DAY1_MORNING:
      return areaName + " - coast day1 morning " + timeFunDay + "(maximum): ";
    case COAST_MEAN_DAY1_MORNING:
      return areaName + " - coast day1  morning " + timeFunDay + "(mean): ";
    case COAST_MIN_DAY2_MORNING:
      return areaName + " - coast day2  morning " + timeFunDay + "(minimum): ";
    case COAST_MAX_DAY2_MORNING:
      return areaName + " - coast day2 morning " + timeFunDay + "(maximum): ";
    case COAST_MEAN_DAY2_MORNING:
      return areaName + " - coast day2 morning " + timeFunDay + "(mean): ";
    case COAST_MIN_DAY1_AFTERNOON:
      return areaName + " - coast day1 afternoon " + timeFunDay + "(minimum): ";
    case COAST_MAX_DAY1_AFTERNOON:
      return areaName + " - coast day1 afternoon " + timeFunDay + "(maximum): ";
    case COAST_MEAN_DAY1_AFTERNOON:
      return areaName + " - coast day1 afternoon " + timeFunDay + "(mean): ";
    case COAST_MIN_DAY2_AFTERNOON:
      return areaName + " - coast day2 afternoon " + timeFunDay + "(minimum): ";
    case COAST_MAX_DAY2_AFTERNOON:
      return areaName + " - coast day2 afternoon " + timeFunDay + "(maximum): ";
    case COAST_MEAN_DAY2_AFTERNOON:
      return areaName + " - coast day2 afternoon " + timeFunDay + "(mean): ";
    case UNDEFINED_WEATHER_RESULT_ID:
    default:
      return {};
  }
}
}  // namespace

using weather_result_container_type = map<int, WeatherResult*>;
using value_type = weather_result_container_type::value_type;

struct t36hparams
{
  t36hparams(const string& variable,
             MessageLogger& log,
             const NightAndDayPeriodGenerator& generator,
             forecast_season_id& seasonId,
             unsigned short forecastArea,
             unsigned short forecastPeriod,
             const TextGenPosixTime& forecastTime,
             const WeatherPeriod& fullPeriod,
             WeatherPeriod& weatherPeriod,
             const WeatherArea& weatherArea,
             const AnalysisSources& analysisSources,
             weather_result_container_type& weatherResults)
      : theVariable(variable),
        theLog(log),
        theGenerator(generator),
        theSeasonId(seasonId),
        theForecastArea(forecastArea),
        theForecastPeriod(forecastPeriod),
        theForecastTime(forecastTime),
        theFullPeriod(fullPeriod),
        theWeatherPeriod(weatherPeriod),
        theWeatherArea(weatherArea),
        theAnalysisSources(analysisSources),
        theWeatherResults(weatherResults)
  {
  }

  const string& theVariable;
  MessageLogger& theLog;
  const NightAndDayPeriodGenerator& theGenerator;
  forecast_season_id& theSeasonId;
  unsigned short theForecastArea = 0;
  unsigned short theForecastPeriod = 0;
  const TextGenPosixTime& theForecastTime;
  const WeatherPeriod& theFullPeriod;
  WeatherPeriod& theWeatherPeriod;
  const WeatherArea& theWeatherArea;
  const AnalysisSources& theAnalysisSources;
  weather_result_container_type& theWeatherResults;
  bool theCoastalAndInlandTogetherFlag = false;
  forecast_area_id theForecastAreaId = NO_AREA;
  forecast_period_id theForecastPeriodId = NO_PERIOD;
  forecast_subperiod_id theSubPeriodId = UNDEFINED_SUBPERIOD;
  unsigned short theForecastAreaDay1 = 0;
  unsigned short theForecastAreaNight = 0;
  unsigned short theForecastAreaDay2 = 0;
  unsigned short theForecastSubPeriod = 0;
  double theMaxTemperatureDay1 = kFloatMissing;
  double theMeanTemperatureDay1 = kFloatMissing;
  double theMinimum = kFloatMissing;
  double theMaximum = kFloatMissing;
  double theMean = kFloatMissing;
  bool theNightPeriodTautologyFlag = false;
  bool theDayPeriodTautologyFlag = false;
  bool theTomorrowTautologyFlag = false;
  bool theOnCoastalAreaTautologyFlag = false;
  bool theOnInlandAreaTautologyFlag = false;
  bool theFrostExistsTautologyFlag = false;
  string theRangeSeparator = "...";
  int theMinInterval = 2;
  bool theZeroIntervalFlag = false;
  temperature_phrase_id theTemperaturePhraseId = NO_PHRASE_ID;
  bool theDayAndNightSeparationFlag = true;
  Paragraph theOptionalFrostParagraph;
  bool theUseFrostExistsPhrase = false;
  bool theFullDayFlag = true;
  bool theUseLongPhrase = true;
  bool theAddCommaDelimiterFlag = false;
  Sentence theSentenceUnderConstruction;

  bool morningAndAfternoonSeparated(forecast_period_id forecastPeriodId = NO_PERIOD) const
  {
    if (forecastPeriodId == NO_PERIOD)
    {
      if (theForecastPeriodId == DAY1_PERIOD)
        return theForecastSubPeriod & DAY1_MORNING_PERIOD;
      if (theForecastPeriodId == NIGHT_PERIOD)
        return false;
      if (theForecastPeriodId == DAY2_PERIOD)
        return theForecastSubPeriod & DAY2_MORNING_PERIOD;
    }
    else if (forecastPeriodId == DAY1_PERIOD)
    {
      return theForecastSubPeriod & DAY1_MORNING_PERIOD;
    }
    else if (forecastPeriodId == NIGHT_PERIOD)
    {
      return false;
    }
    else if (forecastPeriodId == DAY2_PERIOD)
    {
      return theForecastSubPeriod & DAY2_MORNING_PERIOD;
    }

    return false;
  }

  bool inlandAndCoastSeparated(forecast_period_id forecastPeriodId = NO_PERIOD) const
  {
    if (forecastPeriodId == NO_PERIOD)
    {
      if (theForecastPeriodId == DAY1_PERIOD)
        return (theForecastAreaDay1 & COASTAL_AREA) && (theForecastAreaDay1 & INLAND_AREA);
      if (theForecastPeriodId == NIGHT_PERIOD)
        return (theForecastAreaNight & COASTAL_AREA) && (theForecastAreaNight & INLAND_AREA);
      if (theForecastPeriodId == DAY2_PERIOD)
        return (theForecastAreaDay2 & COASTAL_AREA) && (theForecastAreaDay2 & INLAND_AREA);
    }
    else if (forecastPeriodId == DAY1_PERIOD)
    {
      return (theForecastAreaDay1 & COASTAL_AREA) && (theForecastAreaDay1 & INLAND_AREA);
    }
    else if (forecastPeriodId == NIGHT_PERIOD)
    {
      return (theForecastAreaNight & COASTAL_AREA) && (theForecastAreaNight & INLAND_AREA);
    }
    else if (forecastPeriodId == DAY2_PERIOD)
    {
      return (theForecastAreaDay2 & COASTAL_AREA) && (theForecastAreaDay2 & INLAND_AREA);
    }

    return false;
  }

  unsigned int numberOfPeriods() const
  {
    unsigned int retval(0);

    retval += (theForecastPeriod & DAY1_PERIOD ? 1 : 0);
    retval += (theForecastPeriod & NIGHT_PERIOD ? 1 : 0);
    retval += (theForecastPeriod & DAY2_PERIOD ? 1 : 0);

    return retval;
  }
};

void construct_optional_frost_story(t36hparams& theParameters)
{
  if (Settings::isset(theParameters.theVariable + "::frost_story"))
  {
    const string frost_function = require_string(theParameters.theVariable + "::frost_story");

    std::string theVariable = theParameters.theVariable.substr(
        0, theParameters.theVariable.find("temperature_max36hours"));
    theVariable.append(frost_function);

    FrostStory story(theParameters.theForecastTime,
                     theParameters.theAnalysisSources,
                     theParameters.theWeatherArea,
                     theParameters.theWeatherPeriod,
                     theVariable);

    theParameters.theOptionalFrostParagraph = story.makeStory("frost_onenight");
  }
}

// Classify whether min/max values sit below, between, or above the nearest multiple of 5.
// Returns the case and adjusts theNumberDivisibleByFive for ABOVE case.
enum proximity_case_t
{
  PROX_BELOW,
  PROX_BETWEEN,
  PROX_ABOVE,
  PROX_NONE
};

proximity_case_t get_proximity_case(int iMinModFive,
                                    int iMaxModFive,
                                    int iDivFiveMin,
                                    int iDivFiveMax,
                                    float theMinimumCalc,
                                    float theMaximumCalc,
                                    float theMaximum,
                                    int& theNumberDivisibleByFive)
{
  bool straddles =
      (theNumberDivisibleByFive < theMaximumCalc && theMinimumCalc < theNumberDivisibleByFive &&
       theNumberDivisibleByFive - theMinimumCalc < TIENOILLA_ASTETTA_LOW_TEMP_LIMIT &&
       theMaximumCalc - theNumberDivisibleByFive < TIENOILLA_ASTETTA_LOW_TEMP_LIMIT);
  bool exactMatch =
      (theMaximum == theMinimumCalc && theMaximum == static_cast<float>(theNumberDivisibleByFive));
  if (straddles || exactMatch)
    return PROX_BETWEEN;

  bool lowMod = (iMinModFive <= 2) && (iMaxModFive <= 2) && (iDivFiveMin == iDivFiveMax);
  if (lowMod)
  {
    theNumberDivisibleByFive -= 5;
    return PROX_ABOVE;
  }

  bool highMod = (iMinModFive >= 3) && (iMaxModFive >= 3) && (iDivFiveMin == iDivFiveMax);
  if (highMod)
    return PROX_BELOW;

  return PROX_NONE;
}

proximity_id get_proximity_above(float theMinimumCalc,
                                 float theMaximumCalc,
                                 bool bBelowZeroDegrees,
                                 int theNumberDivisibleByFive,
                                 int& outProximityNumber)
{
  float theMinDiff = theMinimumCalc - theNumberDivisibleByFive;
  float theMaxDiff = theMaximumCalc - theNumberDivisibleByFive;

  float vahanYliAstettaLimit = (theMaximumCalc * (bBelowZeroDegrees ? -1.0f : +1.0f)) < -10.0
                                   ? VAHAN_YLI_ASTETTA_LOW_TEMP_LIMIT
                                   : VAHAN_YLI_ASTETTA_HIGH_TEMP_LIMIT;

  if (theMinDiff < vahanYliAstettaLimit && theMaxDiff < vahanYliAstettaLimit)
  {
    outProximityNumber = theNumberDivisibleByFive;
    return VAHAN_YLI_ASTETTA;
  }
  return NO_PROXIMITY;
}

proximity_id get_proximity_below(float theMinimumCalc,
                                 float theMaximumCalc,
                                 int theNumberDivisibleByFive,
                                 int& outProximityNumber)
{
  float theMinDiff = theNumberDivisibleByFive - theMinimumCalc;
  float theMaxDiff = theNumberDivisibleByFive - theMaximumCalc;

  proximity_id retval = NO_PROXIMITY;
  if (theMinDiff < TUNTUMASSA_ASTETTA_ALI && theMaxDiff < TUNTUMASSA_ASTETTA_ALI)
    retval = TUNTUMASSA_ASTETTA;
  else if (theMinDiff < VAJAAT_ASTETTA_LIMIT && theMaxDiff < VAJAAT_ASTETTA_LIMIT)
    retval = VAJAAT_ASTETTA;

  if (retval != NO_PROXIMITY)
    outProximityNumber = theNumberDivisibleByFive;
  return retval;
}

proximity_id get_proximity_between(float theMinimumCalc,
                                   float theMaximumCalc,
                                   float theMean,
                                   bool bBelowZeroDegrees,
                                   int theNumberDivisibleByFive,
                                   int& outProximityNumber)
{
  float theMinDiff = theNumberDivisibleByFive - theMinimumCalc;
  float theMaxDiff = theMaximumCalc - theNumberDivisibleByFive;
  proximity_id retval = NO_PROXIMITY;

  if (theMinDiff < TUNTUMASSA_ASTETTA_ALI && theMaxDiff < TUNTUMASSA_ASTETTA_YLI &&
      theMean < theNumberDivisibleByFive && theMinDiff > theMaxDiff)
  {
    retval = TUNTUMASSA_ASTETTA;
  }
  else if (theMinDiff < LAHELLA_ASTETTA_ALI && theMaxDiff < LAHELLA_ASTETTA_YLI &&
           theMean < theNumberDivisibleByFive && theMinDiff > theMaxDiff)
  {
    retval = LAHELLA_ASTETTA;
  }
  else
  {
    float signedMax = theMaximumCalc * (bBelowZeroDegrees ? -1.0f : +1.0f);
    float noinAstettaLimit =
        signedMax < -10.0 ? NOIN_ASTETTA_LOW_TEMP_LIMIT : NOIN_ASTETTA_HIGH_TEMP_LIMIT;
    float tienoillaAstettaLimit =
        signedMax < -10.0 ? TIENOILLA_ASTETTA_LOW_TEMP_LIMIT : TIENOILLA_ASTETTA_HIGH_TEMP_LIMIT;

    if (theMinDiff < noinAstettaLimit && theMaxDiff < noinAstettaLimit)
      retval = NOIN_ASTETTA;
    else if (theMinDiff < tienoillaAstettaLimit && theMaxDiff < tienoillaAstettaLimit)
      retval = TIENOILLA_ASTETTA;
  }

  if (retval != NO_PROXIMITY)
    outProximityNumber = theNumberDivisibleByFive;
  return retval;
}

proximity_id get_proximity_id(float theMinimum,
                              float theMean,
                              float theMaximum,
                              int& theProximityNumber)
{
  float range = abs(theMaximum - theMinimum);

  // if minimum and maximum differ more than 2.0 degrees we are not in the proximity of
  // a number divisible by five
  if (range >= 6.0 || (theMinimum == kFloatMissing || theMaximum == kFloatMissing))
    return NO_PROXIMITY;

  float theMinimumCalc = theMinimum;
  float theMaximumCalc = theMaximum;

  proximity_id retval = NO_PROXIMITY;
  bool bBelowZeroDegrees = (theMaximum < 0);

  if (bBelowZeroDegrees)
  {
    theMaximumCalc = abs(theMinimum);
    theMinimumCalc = abs(theMaximum);
  }

  int iMin = static_cast<int>(floor(theMinimumCalc));
  int iMax = static_cast<int>(floor(theMaximumCalc));
  int iDivFiveMin = iMin / 5;
  int iDivFiveMax = iMax / 5;
  int iMinModFive = iMin % 5;
  int iMaxModFive = iMax % 5;

  int theNumberDivisibleByFive = iMin;
  while (theNumberDivisibleByFive % 5 != 0)
    theNumberDivisibleByFive += 1;

  proximity_case_t theCase = get_proximity_case(iMinModFive,
                                                iMaxModFive,
                                                iDivFiveMin,
                                                iDivFiveMax,
                                                theMinimumCalc,
                                                theMaximumCalc,
                                                theMaximum,
                                                theNumberDivisibleByFive);

  if (theCase == PROX_BETWEEN)
    retval = get_proximity_between(theMinimumCalc,
                                   theMaximumCalc,
                                   theMean,
                                   bBelowZeroDegrees,
                                   theNumberDivisibleByFive,
                                   theProximityNumber);
  else if (theCase == PROX_ABOVE)
    retval = get_proximity_above(theMinimumCalc,
                                 theMaximumCalc,
                                 bBelowZeroDegrees,
                                 theNumberDivisibleByFive,
                                 theProximityNumber);
  else if (theCase == PROX_BELOW)
    retval = get_proximity_below(
        theMinimumCalc, theMaximumCalc, theNumberDivisibleByFive, theProximityNumber);

  if (bBelowZeroDegrees && retval != NO_PROXIMITY)
    theProximityNumber = theProximityNumber * -1;

  return retval;
}

WeatherResult do_calculation(const string& theVar,
                             const AnalysisSources& theSources,
                             const WeatherFunction& theAreaFunction,
                             const WeatherFunction& theTimeFunction,
                             const WeatherArea& theArea,
                             const WeatherPeriod& thePeriod)
{
  GridForecaster theForecaster;

  return theForecaster.analyze(
      theVar, theSources, Temperature, theAreaFunction, theTimeFunction, theArea, thePeriod);
}

// Lookup tables for weather result IDs per (period, area) combination
struct period_area_ids
{
  weather_result_id min_full, max_full, mean_full;
  weather_result_id min_morning, max_morning, mean_morning;
  weather_result_id min_afternoon, max_afternoon, mean_afternoon;
  std::string fakeVarSuffix;
  std::string fakeMorningSuffix;
  std::string fakeAfternoonSuffix;
};

period_area_ids get_period_area_ids(forecast_period_id thePeriodId, forecast_area_id theAreaId)
{
  // Helper: pick correct ID based on area
  auto pick = [theAreaId](weather_result_id inland, weather_result_id coast, weather_result_id area)
  { return theAreaId == INLAND_AREA ? inland : (theAreaId == COASTAL_AREA ? coast : area); };

  period_area_ids ids{};
  ids.min_morning = UNDEFINED_WEATHER_RESULT_ID;
  ids.max_morning = UNDEFINED_WEATHER_RESULT_ID;
  ids.mean_morning = UNDEFINED_WEATHER_RESULT_ID;
  ids.min_afternoon = UNDEFINED_WEATHER_RESULT_ID;
  ids.max_afternoon = UNDEFINED_WEATHER_RESULT_ID;
  ids.mean_afternoon = UNDEFINED_WEATHER_RESULT_ID;

  if (thePeriodId == DAY1_PERIOD)
  {
    ids.min_full = pick(INLAND_MIN_DAY1, COAST_MIN_DAY1, AREA_MIN_DAY1);
    ids.max_full = pick(INLAND_MAX_DAY1, COAST_MAX_DAY1, AREA_MAX_DAY1);
    ids.mean_full = pick(INLAND_MEAN_DAY1, COAST_MEAN_DAY1, AREA_MEAN_DAY1);
    ids.min_morning = pick(INLAND_MIN_DAY1_MORNING, COAST_MIN_DAY1_MORNING, AREA_MIN_DAY1_MORNING);
    ids.max_morning = pick(INLAND_MAX_DAY1_MORNING, COAST_MAX_DAY1_MORNING, AREA_MAX_DAY1_MORNING);
    ids.mean_morning =
        pick(INLAND_MEAN_DAY1_MORNING, COAST_MEAN_DAY1_MORNING, AREA_MEAN_DAY1_MORNING);
    ids.min_afternoon =
        pick(INLAND_MIN_DAY1_AFTERNOON, COAST_MIN_DAY1_AFTERNOON, AREA_MIN_DAY1_AFTERNOON);
    ids.max_afternoon =
        pick(INLAND_MAX_DAY1_AFTERNOON, COAST_MAX_DAY1_AFTERNOON, AREA_MAX_DAY1_AFTERNOON);
    ids.mean_afternoon =
        pick(INLAND_MEAN_DAY1_AFTERNOON, COAST_MEAN_DAY1_AFTERNOON, AREA_MEAN_DAY1_AFTERNOON);
    ids.fakeVarSuffix = "::day1";
    ids.fakeMorningSuffix = "::day1::morning";
    ids.fakeAfternoonSuffix = "::day1::afternoon";
  }
  else if (thePeriodId == NIGHT_PERIOD)
  {
    ids.min_full = pick(INLAND_MIN_NIGHT, COAST_MIN_NIGHT, AREA_MIN_NIGHT);
    ids.max_full = pick(INLAND_MAX_NIGHT, COAST_MAX_NIGHT, AREA_MAX_NIGHT);
    ids.mean_full = pick(INLAND_MEAN_NIGHT, COAST_MEAN_NIGHT, AREA_MEAN_NIGHT);
    ids.fakeVarSuffix = "::night";
    ids.fakeMorningSuffix = "::night";
    ids.fakeAfternoonSuffix = "::night";
  }
  else  // DAY2_PERIOD
  {
    ids.min_full = pick(INLAND_MIN_DAY2, COAST_MIN_DAY2, AREA_MIN_DAY2);
    ids.max_full = pick(INLAND_MAX_DAY2, COAST_MAX_DAY2, AREA_MAX_DAY2);
    ids.mean_full = pick(INLAND_MEAN_DAY2, COAST_MEAN_DAY2, AREA_MEAN_DAY2);
    ids.min_morning = pick(INLAND_MIN_DAY2_MORNING, COAST_MIN_DAY2_MORNING, AREA_MIN_DAY2_MORNING);
    ids.max_morning = pick(INLAND_MAX_DAY2_MORNING, COAST_MAX_DAY2_MORNING, AREA_MAX_DAY2_MORNING);
    ids.mean_morning =
        pick(INLAND_MEAN_DAY2_MORNING, COAST_MEAN_DAY2_MORNING, AREA_MEAN_DAY2_MORNING);
    ids.min_afternoon =
        pick(INLAND_MIN_DAY2_AFTERNOON, COAST_MIN_DAY2_AFTERNOON, AREA_MIN_DAY2_AFTERNOON);
    ids.max_afternoon =
        pick(INLAND_MAX_DAY2_AFTERNOON, COAST_MAX_DAY2_AFTERNOON, AREA_MAX_DAY2_AFTERNOON);
    ids.mean_afternoon =
        pick(INLAND_MEAN_DAY2_AFTERNOON, COAST_MEAN_DAY2_AFTERNOON, AREA_MEAN_DAY2_AFTERNOON);
    ids.fakeVarSuffix = "::day2";
    ids.fakeMorningSuffix = "::day2::morning";
    ids.fakeAfternoonSuffix = "::day2::afternoon";
  }
  return ids;
}

void calculate_night_period(const string& theVar,
                            const AnalysisSources& theSources,
                            const WeatherArea& theActualArea,
                            const WeatherPeriod& thePeriod,
                            WeatherFunction timeFunction,
                            const string& fakeVarFull,
                            WeatherResult& minResultFull,
                            WeatherResult& maxResultFull,
                            WeatherResult& meanResultFull)
{
  minResultFull = do_calculation(
      theVar + fakeVarFull + "::min", theSources, Minimum, timeFunction, theActualArea, thePeriod);
  maxResultFull = do_calculation(
      theVar + fakeVarFull + "::max", theSources, Maximum, timeFunction, theActualArea, thePeriod);
  meanResultFull = do_calculation(
      theVar + fakeVarFull + "::mean", theSources, Mean, timeFunction, theActualArea, thePeriod);

  if (theActualArea.type() == WeatherArea::Full)
    WeatherResultTools::checkMissingValue(
        "temperature_max36hours", Temperature, {minResultFull, maxResultFull, meanResultFull});
}

void calculate_day_period(const string& theVar,
                          const AnalysisSources& theSources,
                          const WeatherArea& theActualArea,
                          const WeatherPeriod& thePeriod,
                          const string& fakeVarMorning,
                          const string& fakeVarAfternoon,
                          const string& fakeVarFull,
                          WeatherResult& minResultMorning,
                          WeatherResult& maxResultMorning,
                          WeatherResult& meanResultMorning,
                          WeatherResult& minResultAfternoon,
                          WeatherResult& maxResultAfternoon,
                          WeatherResult& meanResultAfternoon,
                          WeatherResult& minResultFull,
                          WeatherResult& maxResultFull,
                          WeatherResult& meanResultFull)
{
  morning_temperature(theVar + fakeVarMorning,
                      theSources,
                      theActualArea,
                      thePeriod,
                      minResultMorning,
                      maxResultMorning,
                      meanResultMorning);
  afternoon_temperature(theVar + fakeVarAfternoon,
                        theSources,
                        theActualArea,
                        thePeriod,
                        minResultAfternoon,
                        maxResultAfternoon,
                        meanResultAfternoon);
  afternoon_temperature(theVar + fakeVarFull,
                        theSources,
                        theActualArea,
                        thePeriod,
                        minResultFull,
                        maxResultFull,
                        meanResultFull);

  if (theActualArea.type() == WeatherArea::Full)
    WeatherResultTools::checkMissingValue(
        "temperature_max36hours", Temperature, {minResultFull, maxResultFull, meanResultFull});
}

void calculate_results(MessageLogger& /*theLog*/,
                       const string& theVar,
                       const AnalysisSources& theSources,
                       const WeatherArea& theArea,
                       const WeatherPeriod& thePeriod,
                       forecast_period_id thePeriodId,
                       forecast_season_id theSeasonId,
                       forecast_area_id theAreaId,
                       weather_result_container_type& theWeatherResults)
{
  if (thePeriodId != DAY1_PERIOD && thePeriodId != NIGHT_PERIOD && thePeriodId != DAY2_PERIOD)
    return;

  period_area_ids ids = get_period_area_ids(thePeriodId, theAreaId);

  // Determine area suffix and actual area type
  WeatherArea theActualArea = theArea;
  std::string areaSuffix;
  if (theAreaId == INLAND_AREA)
  {
    if (theArea.type() == WeatherArea::Full)
      theActualArea.type(WeatherArea::Inland);
    areaSuffix = "::inland";
  }
  else if (theAreaId == COASTAL_AREA)
  {
    if (theArea.type() == WeatherArea::Full)
      theActualArea.type(WeatherArea::Coast);
    areaSuffix = "::coast";
  }
  else
  {
    if (theArea.type() == WeatherArea::Full)
      theActualArea.type(WeatherArea::Full);
    areaSuffix = "::area";
  }

  std::string fakeVarFull = "::fake" + ids.fakeVarSuffix + areaSuffix;
  std::string fakeVarMorning = "::fake" + ids.fakeMorningSuffix + areaSuffix;
  std::string fakeVarAfternoon = "::fake" + ids.fakeAfternoonSuffix + areaSuffix;

  // Use UNDEFINED_WEATHER_RESULT_ID sentinels for night morning/afternoon (unused)
  WeatherResult dummyResult(kFloatMissing, 0);
  WeatherResult& minResultFull = (ids.min_full != UNDEFINED_WEATHER_RESULT_ID)
                                     ? *theWeatherResults[ids.min_full]
                                     : dummyResult;
  WeatherResult& maxResultFull = (ids.max_full != UNDEFINED_WEATHER_RESULT_ID)
                                     ? *theWeatherResults[ids.max_full]
                                     : dummyResult;
  WeatherResult& meanResultFull = (ids.mean_full != UNDEFINED_WEATHER_RESULT_ID)
                                      ? *theWeatherResults[ids.mean_full]
                                      : dummyResult;
  WeatherResult& minResultMorning = (ids.min_morning != UNDEFINED_WEATHER_RESULT_ID)
                                        ? *theWeatherResults[ids.min_morning]
                                        : dummyResult;
  WeatherResult& maxResultMorning = (ids.max_morning != UNDEFINED_WEATHER_RESULT_ID)
                                        ? *theWeatherResults[ids.max_morning]
                                        : dummyResult;
  WeatherResult& meanResultMorning = (ids.mean_morning != UNDEFINED_WEATHER_RESULT_ID)
                                         ? *theWeatherResults[ids.mean_morning]
                                         : dummyResult;
  WeatherResult& minResultAfternoon = (ids.min_afternoon != UNDEFINED_WEATHER_RESULT_ID)
                                          ? *theWeatherResults[ids.min_afternoon]
                                          : dummyResult;
  WeatherResult& maxResultAfternoon = (ids.max_afternoon != UNDEFINED_WEATHER_RESULT_ID)
                                          ? *theWeatherResults[ids.max_afternoon]
                                          : dummyResult;
  WeatherResult& meanResultAfternoon = (ids.mean_afternoon != UNDEFINED_WEATHER_RESULT_ID)
                                           ? *theWeatherResults[ids.mean_afternoon]
                                           : dummyResult;

  if (thePeriodId == NIGHT_PERIOD)
  {
    // In summertime use Minimum time function for night, in wintertime use Mean
    WeatherFunction timeFunction = (theSeasonId == SUMMER_SEASON) ? Minimum : Mean;
    calculate_night_period(theVar,
                           theSources,
                           theActualArea,
                           thePeriod,
                           timeFunction,
                           fakeVarFull,
                           minResultFull,
                           maxResultFull,
                           meanResultFull);
    return;
  }

  // Day periods: morning + afternoon calculations are the same for summer and winter
  calculate_day_period(theVar,
                       theSources,
                       theActualArea,
                       thePeriod,
                       fakeVarMorning,
                       fakeVarAfternoon,
                       fakeVarFull,
                       minResultMorning,
                       maxResultMorning,
                       meanResultMorning,
                       minResultAfternoon,
                       maxResultAfternoon,
                       meanResultAfternoon,
                       minResultFull,
                       maxResultFull,
                       meanResultFull);
}

void log_start_time_and_end_time(MessageLogger& theLog,
                                 const std::string& theLogMessage,
                                 const WeatherPeriod& thePeriod)
{
  theLog << NFmiStringTools::Convert(theLogMessage) << thePeriod.localStartTime() << " ... "
         << thePeriod.localEndTime() << '\n';
}

void log_weather_results(const t36hparams& theParameters)
{
  theParameters.theLog << "Weather results: \n";

  // Iterate and print out the WeatherResult variables
  for (int i = AREA_MIN_DAY1; i < UNDEFINED_WEATHER_RESULT_ID; i++)
  {
    const WeatherResult& theWeatherResult = *(theParameters.theWeatherResults[i]);

    if (theWeatherResult.value() != kFloatMissing)
    {
      theParameters.theLog << weather_result_string(
          theParameters.theWeatherArea.isNamed() ? theParameters.theWeatherArea.name() : "",
          static_cast<weather_result_id>(i),
          theParameters.theSeasonId == WINTER_SEASON);
      theParameters.theLog << theWeatherResult << '\n';
    }
  }
}

temperature_phrase_id around_zero_phrase(float theMinimum,
                                         float /*theMean*/,
                                         float theMaximum,
                                         bool theZeroIntervalFlag)
{
  temperature_phrase_id retval = NO_PHRASE_ID;

  if (theMinimum > 0.0 && theMaximum < 2.50 && !theZeroIntervalFlag)  // [+0,1...+2,49]
  {
    retval = VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID;
  }
  else if (theMinimum < 0 && theMinimum > -0.50 && theMaximum >= 0 && theMaximum < 2.50 &&
           !theZeroIntervalFlag)  // [-0,49...+2,49]
  {
    retval = NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID;
  }
  else if (theMinimum < 0 && theMinimum >= -2.0 && theMaximum >= 0 && theMaximum <= 2.0 &&
           !theZeroIntervalFlag)  // [-2,0...+2,0]
  {
    retval = NOLLAN_TIENOILLA_PHRASE_ID;
  }
  else if (theMinimum > -4.50 && theMaximum < 0.0 && !theZeroIntervalFlag)  // [-4,49...-0,1]
  {
    retval = PIKKUPAKKASTA_PHRASE_ID;
  }

  return retval;
}

temperature_phrase_id around_zero_phrase(const t36hparams& theParameters)
{
  temperature_phrase_id retval = NO_PHRASE_ID;

  if (theParameters.theMinimum > 0.0 && theParameters.theMaximum < 2.50 &&
      !theParameters.theZeroIntervalFlag &&
      theParameters.theTemperaturePhraseId != VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID)  // [+0,1...+2,49]
  {
    retval = VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID;
  }
  else if (theParameters.theMinimum < 0 && theParameters.theMinimum > -0.50 &&
           theParameters.theMaximum >= 0 && theParameters.theMaximum < 2.50 &&
           !theParameters.theZeroIntervalFlag &&
           theParameters.theTemperaturePhraseId !=
               NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID)  // [-0,49...+2,49]
  {
    retval = NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID;
  }
  else if (theParameters.theMinimum < 0 && theParameters.theMinimum >= -2.0 &&
           theParameters.theMaximum >= 0 && theParameters.theMaximum <= 2.0 &&
           !theParameters.theZeroIntervalFlag &&
           theParameters.theTemperaturePhraseId != NOLLAN_TIENOILLA_PHRASE_ID)  // [-2,0...+2,0]
  {
    retval = NOLLAN_TIENOILLA_PHRASE_ID;
  }
  else if (theParameters.theMinimum > -4.50 && theParameters.theMaximum < 0.0 &&
           !theParameters.theZeroIntervalFlag &&
           theParameters.theTemperaturePhraseId != PIKKUPAKKASTA_PHRASE_ID)  // [-4,49...-0,1]
  {
    retval = PIKKUPAKKASTA_PHRASE_ID;
  }

  return retval;
}

// Check if day and night should be separated based on temperature difference for a given area pair.
// Returns updated separateDayAndNight. Uses the day period (DAY1/DAY2) and night ID sets.
bool check_day_night_temp_separation(const t36hparams& p,
                                     forecast_period_id dayPeriod,
                                     weather_result_id dayMin,
                                     weather_result_id dayMean,
                                     weather_result_id dayMax,
                                     weather_result_id nightMin,
                                     weather_result_id nightMean,
                                     weather_result_id nightMax)
{
  if (!(p.theForecastPeriod & dayPeriod) || !(p.theForecastPeriod & NIGHT_PERIOD))
    return true;  // no applicable periods - keep separateDayAndNight = true

  if (around_zero_phrase(p.theWeatherResults[dayMin]->value(),
                         p.theWeatherResults[dayMean]->value(),
                         p.theWeatherResults[dayMax]->value(),
                         p.theZeroIntervalFlag) !=
      around_zero_phrase(p.theWeatherResults[nightMin]->value(),
                         p.theWeatherResults[nightMean]->value(),
                         p.theWeatherResults[nightMax]->value(),
                         p.theZeroIntervalFlag))
  {
    return true;  // different around-zero phrase => separate
  }

  float dayTemperature = p.theWeatherResults[dayMean]->value();
  float nightTemperature = p.theWeatherResults[nightMean]->value();
  if (abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT)
    return true;

  forecast_period_id otherDay = (dayPeriod == DAY1_PERIOD) ? DAY1_PERIOD : DAY2_PERIOD;
  return p.inlandAndCoastSeparated(otherDay) || p.inlandAndCoastSeparated(NIGHT_PERIOD);
}

bool separate_day_and_night_winter(const t36hparams& p, forecast_area_id theForecastAreaId)
{
  bool separateDayAndNight = true;

  // Pick the primary area result IDs based on forecastAreaId
  if (theForecastAreaId == FULL_AREA)
  {
    if (p.theForecastPeriod & DAY1_PERIOD)
      separateDayAndNight = check_day_night_temp_separation(p,
                                                            DAY1_PERIOD,
                                                            AREA_MIN_DAY1,
                                                            AREA_MEAN_DAY1,
                                                            AREA_MAX_DAY1,
                                                            AREA_MIN_NIGHT,
                                                            AREA_MEAN_NIGHT,
                                                            AREA_MAX_NIGHT);
    else
      separateDayAndNight = check_day_night_temp_separation(p,
                                                            DAY2_PERIOD,
                                                            AREA_MIN_DAY2,
                                                            AREA_MEAN_DAY2,
                                                            AREA_MAX_DAY2,
                                                            AREA_MIN_NIGHT,
                                                            AREA_MEAN_NIGHT,
                                                            AREA_MAX_NIGHT);
    return separateDayAndNight;
  }

  // For INLAND_AREA and COASTAL_AREA, first check the primary area, then check the secondary
  weather_result_id primaryMinDay1, primaryMeanDay1, primaryMaxDay1;
  weather_result_id primaryMinDay2, primaryMeanDay2, primaryMaxDay2;
  unsigned short secondaryAreaMask;

  if (theForecastAreaId == INLAND_AREA)
  {
    primaryMinDay1 = INLAND_MIN_DAY1;
    primaryMeanDay1 = INLAND_MEAN_DAY1;
    primaryMaxDay1 = INLAND_MAX_DAY1;
    primaryMinDay2 = INLAND_MIN_DAY2;
    primaryMeanDay2 = INLAND_MEAN_DAY2;
    primaryMaxDay2 = INLAND_MAX_DAY2;
    secondaryAreaMask = COASTAL_AREA;
  }
  else  // COASTAL_AREA
  {
    primaryMinDay1 = COAST_MIN_DAY1;
    primaryMeanDay1 = COAST_MEAN_DAY1;
    primaryMaxDay1 = COAST_MAX_DAY1;
    primaryMinDay2 = COAST_MIN_DAY2;
    primaryMeanDay2 = COAST_MEAN_DAY2;
    primaryMaxDay2 = COAST_MAX_DAY2;
    secondaryAreaMask = INLAND_AREA;
  }

  if (p.theForecastPeriod & DAY1_PERIOD)
    separateDayAndNight = check_day_night_temp_separation(p,
                                                          DAY1_PERIOD,
                                                          primaryMinDay1,
                                                          primaryMeanDay1,
                                                          primaryMaxDay1,
                                                          INLAND_MIN_NIGHT,
                                                          INLAND_MEAN_NIGHT,
                                                          INLAND_MAX_NIGHT);
  else
    separateDayAndNight = check_day_night_temp_separation(p,
                                                          DAY2_PERIOD,
                                                          primaryMinDay2,
                                                          primaryMeanDay2,
                                                          primaryMaxDay2,
                                                          INLAND_MIN_NIGHT,
                                                          INLAND_MEAN_NIGHT,
                                                          INLAND_MAX_NIGHT);

  // If not separate, check secondary area too
  if (!separateDayAndNight && (p.theForecastArea & secondaryAreaMask))
  {
    weather_result_id secMinDay1 =
        (secondaryAreaMask == COASTAL_AREA) ? COAST_MIN_DAY1 : INLAND_MIN_DAY1;
    weather_result_id secMeanDay1 =
        (secondaryAreaMask == COASTAL_AREA) ? COAST_MEAN_DAY1 : INLAND_MEAN_DAY1;
    weather_result_id secMaxDay1 =
        (secondaryAreaMask == COASTAL_AREA) ? COAST_MAX_DAY1 : INLAND_MAX_DAY1;
    weather_result_id secMinDay2 =
        (secondaryAreaMask == COASTAL_AREA) ? COAST_MIN_DAY2 : INLAND_MIN_DAY2;
    weather_result_id secMeanDay2 =
        (secondaryAreaMask == COASTAL_AREA) ? COAST_MEAN_DAY2 : INLAND_MEAN_DAY2;
    weather_result_id secMaxDay2 =
        (secondaryAreaMask == COASTAL_AREA) ? COAST_MAX_DAY2 : INLAND_MAX_DAY2;

    if (p.theForecastPeriod & DAY1_PERIOD)
      separateDayAndNight = check_day_night_temp_separation(p,
                                                            DAY1_PERIOD,
                                                            secMinDay1,
                                                            secMeanDay1,
                                                            secMaxDay1,
                                                            COAST_MIN_NIGHT,
                                                            COAST_MEAN_NIGHT,
                                                            COAST_MAX_NIGHT);
    else
      separateDayAndNight = check_day_night_temp_separation(p,
                                                            DAY2_PERIOD,
                                                            secMinDay2,
                                                            secMeanDay2,
                                                            secMaxDay2,
                                                            COAST_MIN_NIGHT,
                                                            COAST_MEAN_NIGHT,
                                                            COAST_MAX_NIGHT);
  }

  return separateDayAndNight;
}

bool separate_day_and_night(const t36hparams& theParameters, forecast_area_id theForecastAreaId)
{
  // Shorthand alias to make code more readable
  const auto& p = theParameters;

  // if only one day or one night is included ==> nothing to separate
  if (p.theForecastPeriod == DAY1_PERIOD || p.theForecastPeriod == NIGHT_PERIOD ||
      p.theForecastPeriod == DAY2_PERIOD)
    return false;

  if (p.theSeasonId != WINTER_SEASON)
    return true;

  return separate_day_and_night_winter(p, theForecastAreaId);
}

namespace
{
// Helper: build sentence using plain temperature_sentence2 (not proximity-based)
Sentence build_numeric_temperature_sentence(t36hparams& p,
                                            int& intervalStart,
                                            int& intervalEnd,
                                            bool allowInterval)
{
  int theMinimumInt = static_cast<int>(round(p.theMinimum));
  int theMeanInt = static_cast<int>(round(p.theMean));
  int theMaximumInt = static_cast<int>(round(p.theMaximum));
  bool intervalUsed = false;

  clamp_temperature(p.theVariable,
                    p.theSeasonId == WINTER_SEASON,
                    p.theForecastPeriodId != NIGHT_PERIOD,
                    theMinimumInt < theMaximumInt ? theMinimumInt : theMaximumInt,
                    theMaximumInt > theMinimumInt ? theMaximumInt : theMinimumInt);

  Sentence s = TemperatureStoryTools::temperature_sentence2(theMinimumInt,
                                                            theMeanInt,
                                                            theMaximumInt,
                                                            p.theMinInterval,
                                                            p.theZeroIntervalFlag,
                                                            intervalUsed,
                                                            intervalStart,
                                                            intervalEnd,
                                                            p.theRangeSeparator,
                                                            allowInterval);
  p.theTemperaturePhraseId = intervalUsed ? LAMPOTILA_VALILLA_PHRASE_ID : NOIN_ASTETTA_PHRASE_ID;
  return s;
}

// Helper: handle around-zero phrase (non-NO_PHRASE_ID cases)
Sentence handle_around_zero_phrase(t36hparams& p,
                                   temperature_phrase_id phrase_id,
                                   int& intervalStart,
                                   int& intervalEnd)
{
  Sentence sentence;
  p.theTemperaturePhraseId = phrase_id;

  if (phrase_id == VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID)
  {
    sentence << VAHAN_PLUSSAN_PUOLELLA_PHRASE;
    return sentence;
  }
  if (phrase_id == NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID)
  {
    sentence << NOLLAN_TIENOILLA_PHRASE << TAI_PHRASE << VAHAN_PLUSSAN_PUOLELLA_PHRASE;
    return sentence;
  }
  if (phrase_id == NOLLAN_TIENOILLA_PHRASE_ID)
  {
    sentence << NOLLAN_TIENOILLA_PHRASE;
    return sentence;
  }
  if (phrase_id == PIKKUPAKKASTA_PHRASE_ID)
  {
    TextGenPosixTime startTime(p.theGenerator.period(1).localStartTime());
    if (SeasonTools::isSpring(startTime, p.theVariable) ||
        abs(p.theMaximum - p.theMinimum) < AROUND_ZERO_UPPER_LIMIT)
    {
      if (p.theMinimum >= -1.0)
      {
        sentence << VAHAN_NOLLAN_ALAPUOLELLA_PHRASE;
        intervalStart = 0;
        p.theTemperaturePhraseId = VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID;
      }
      else
      {
        sentence = build_numeric_temperature_sentence(p, intervalStart, intervalEnd, false);
      }
    }
    else
    {
      p.theTemperaturePhraseId = PIKKUPAKKASTA_PHRASE_ID;
      sentence << PIKKUPAKKASTA_PHRASE;
    }
  }
  return sentence;
}

// Helper: handle proximity-based phrase
Sentence handle_proximity_phrase(t36hparams& p,
                                 proximity_id proxim_id,
                                 int theProximityNumber,
                                 int& intervalStart)
{
  Sentence sentence;
  char proximityNumberBuff[32];
  char tempBuff[128];

  sprintf(
      tempBuff, "Minimum: %.02f;Mean: %.02f;Maximum: %.02f", p.theMinimum, p.theMean, p.theMaximum);
  sprintf(proximityNumberBuff, "%i", theProximityNumber);

  switch (proxim_id)
  {
    case NOIN_ASTETTA:
      sentence << NOIN_PHRASE << Integer(theProximityNumber)
               << *UnitFactory::create_unit(DegreesCelsius, theProximityNumber);
      p.theTemperaturePhraseId = NOIN_ASTETTA_PHRASE_ID;
      intervalStart = theProximityNumber;
      p.theLog << "PROXIMITY: Noin " << proximityNumberBuff << " astetta :: " << tempBuff << '\n';
      break;
    case TIENOILLA_ASTETTA:
      sentence << TIENOILLA_PHRASE << Integer(theProximityNumber);
      p.theTemperaturePhraseId = TIENOILLA_ASTETTA_PHRASE_ID;
      intervalStart = theProximityNumber;
      p.theLog << "PROXIMITY: " << proximityNumberBuff << " asteen tienoilla :: " << tempBuff
               << '\n';
      break;
    case LAHELLA_ASTETTA:
      sentence << LAHELLA_PHRASE << Integer(theProximityNumber);
      p.theTemperaturePhraseId = LAHELLA_ASTETTA_PHRASE_ID;
      intervalStart = theProximityNumber;
      p.theLog << "PROXIMITY: Lahella " << proximityNumberBuff << " astetta :: " << tempBuff
               << '\n';
      break;
    case TUNTUMASSA_ASTETTA:
      sentence << TUNTUMASSA_PHRASE << Integer(theProximityNumber);
      p.theTemperaturePhraseId = TUNTUMASSA_ASTETTA_PHRASE_ID;
      intervalStart = theProximityNumber;
      p.theLog << "PROXIMITY: " << proximityNumberBuff << " asteen tuntumassa :: " << tempBuff
               << '\n';
      break;
    case VAJAAT_ASTETTA:
      if (theProximityNumber == 0)
      {
        sentence << VAHAN_NOLLAN_ALAPUOLELLA_PHRASE;
        p.theTemperaturePhraseId = VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID;
        intervalStart = 0;
        p.theLog << "PROXIMITY: Vähän nollan alapuolella \n";
      }
      else
      {
        sentence << VAJAAT_PHRASE << Integer(theProximityNumber)
                 << *UnitFactory::create_unit(DegreesCelsius, theProximityNumber);
        p.theTemperaturePhraseId = VAJAAT_ASTETTA_PHRASE_ID;
        intervalStart = theProximityNumber;
        if (p.theMaximum < 0)
          p.theUseFrostExistsPhrase = true;
        p.theLog << "PROXIMITY: Vajaat " << proximityNumberBuff << " astetta :: " << tempBuff
                 << '\n';
      }
      break;
    case VAHAN_YLI_ASTETTA:
      sentence << VAHAN_PHRASE << YLI_PHRASE << Integer(theProximityNumber)
               << *UnitFactory::create_unit(DegreesCelsius, theProximityNumber);
      p.theTemperaturePhraseId = VAHAN_YLI_ASTETTA_PHRASE_ID;
      intervalStart = theProximityNumber;
      p.theLog << "PROXIMITY: Vahan yli " << proximityNumberBuff << " astetta :: " << tempBuff
               << '\n';
      if (p.theMaximum < 0)
        p.theUseFrostExistsPhrase = true;
      break;
    case NO_PROXIMITY:
      p.theLog << "NO PROXIMITY: " << tempBuff << '\n';
      break;
  }
  return sentence;
}
}  // namespace

Sentence temperature_sentence(t36hparams& theParameters, int& intervalStart, int& intervalEnd)
{
  auto& p = theParameters;

  temperature_phrase_id phrase_id = around_zero_phrase(p);

  if (phrase_id != NO_PHRASE_ID)
    return handle_around_zero_phrase(p, phrase_id, intervalStart, intervalEnd);

  int theProximityNumber = 0;
  proximity_id proxim_id =
      get_proximity_id(p.theMinimum, p.theMean, p.theMaximum, theProximityNumber);

  if (proxim_id != NO_PROXIMITY)
    return handle_proximity_phrase(p, proxim_id, theProximityNumber, intervalStart);

  return build_numeric_temperature_sentence(p, intervalStart, intervalEnd, true);
}

namespace
{
void fill_night_temperature_phrase(t36hparams& p, Sentence& theTemperaturePhrase)
{
  if (p.theSeasonId == SUMMER_SEASON)
  {
    theTemperaturePhrase << (p.theUseLongPhrase ? YON_ALIN_LAMPOTILA_LONG_PHRASE
                                                : YON_ALIN_LAMPOTILA_SHORT_PHRASE);
  }
  else
  {
    theTemperaturePhrase << YOLAMPOTILA_PHRASE;
  }
}

void fill_day_temperature_phrase(t36hparams& p, Sentence& theTemperaturePhrase)
{
  if (p.theSeasonId == SUMMER_SEASON)
  {
    theTemperaturePhrase << (p.theUseLongPhrase ? PAIVAN_YLIN_LAMPOTILA_LONG_PHRASE
                                                : PAIVAN_YLIN_LAMPOTILA_SHORT_PHRASE);
  }
  else
  {
    theTemperaturePhrase << PAIVALAMPOTILA_PHRASE;
  }
}
}  // namespace

void pakkasta_on(t36hparams& theParameters,
                 Sentence& theDayPhasePhrase,
                 Sentence& theTemperaturePhrase)
{
  auto& p = theParameters;

  if (p.theUseFrostExistsPhrase)
  {
    // if only one period exists, dont use word 'yolla'/'paivalla'
    if (p.numberOfPeriods() > 1 && p.theDayAndNightSeparationFlag && p.theFullDayFlag)
    {
      theDayPhasePhrase << (p.theForecastPeriodId == NIGHT_PERIOD ? YOLLA_PHRASE : PAIVALLA_PHRASE);
    }
    theTemperaturePhrase << PAKKASTA_WORD;
    p.theUseFrostExistsPhrase = false;
  }

  if (theTemperaturePhrase.empty())
  {
    if (p.theFullDayFlag && p.numberOfPeriods() > 1)
    {
      if (p.theForecastPeriodId == NIGHT_PERIOD)
        fill_night_temperature_phrase(p, theTemperaturePhrase);
      else
        fill_day_temperature_phrase(p, theTemperaturePhrase);
    }
    else
    {
      theTemperaturePhrase << LAMPOTILA_WORD;
    }
  }
}

namespace
{
bool is_yolla_phrase_id(temperature_phrase_id pid)
{
  return (pid == PIKKUPAKKASTA_PHRASE_ID || pid == HIEMAN_LAUHEMPAA_PHRASE_ID ||
          pid == HIEMAN_KYLMEMPAA_PHRASE_ID || pid == HIEMAN_HEIKOMPAA_PHRASE_ID ||
          pid == HIEMAN_KIREAMPAA_PHRASE_ID || pid == LAMPOTILA_NOUSEE_PHRASE_ID);
}

void handle_night_period_phrase(t36hparams& p,
                                temperature_phrase_id phrase_id,
                                forecast_season_id season_id,
                                Sentence& theDayPhasePhrase,
                                Sentence& theTemperaturePhrase)
{
  if (p.theUseFrostExistsPhrase)
  {
    pakkasta_on(p, theDayPhasePhrase, theTemperaturePhrase);
    return;
  }
  if (is_yolla_phrase_id(phrase_id))
  {
    theDayPhasePhrase << YOLLA_PHRASE;
    return;
  }
  if (season_id == SUMMER_SEASON)
  {
    theTemperaturePhrase << (p.theUseLongPhrase ? YON_ALIN_LAMPOTILA_LONG_PHRASE
                                                : YON_ALIN_LAMPOTILA_SHORT_PHRASE);
  }
  else
  {
    theTemperaturePhrase << (p.numberOfPeriods() > 1 ? YOLAMPOTILA_PHRASE : LAMPOTILA_WORD);
  }
}

void handle_day_undefined_subperiod(t36hparams& p,
                                    temperature_phrase_id phrase_id,
                                    forecast_season_id season_id,
                                    Sentence& theDayPhasePhrase,
                                    Sentence& theTemperaturePhrase)
{
  if (p.theUseFrostExistsPhrase)
  {
    pakkasta_on(p, theDayPhasePhrase, theTemperaturePhrase);
    return;
  }
  if (phrase_id == PIKKUPAKKASTA_PHRASE_ID)
  {
    if (p.theFullDayFlag)
    {
      if (p.theForecastPeriodId == DAY2_PERIOD)
        theDayPhasePhrase << HUOMENNA_WORD;
      else
        theDayPhasePhrase << PAIVALLA_PHRASE;
    }
    return;
  }
  if (!p.theFullDayFlag)
  {
    theTemperaturePhrase << LAMPOTILA_WORD;
    return;
  }
  if (season_id == SUMMER_SEASON)
  {
    theTemperaturePhrase << (p.theUseLongPhrase ? PAIVAN_YLIN_LAMPOTILA_LONG_PHRASE
                                                : PAIVAN_YLIN_LAMPOTILA_SHORT_PHRASE);
  }
  else
  {
    theTemperaturePhrase << (p.theDayAndNightSeparationFlag ? PAIVALAMPOTILA_PHRASE
                                                            : LAMPOTILA_WORD);
  }
}
}  // namespace

void temperature_phrase(t36hparams& theParameters,
                        Sentence& theDayPhasePhrase,
                        Sentence& theTemperaturePhrase,
                        Sentence& theAreaPhrase)
{
  auto& p = theParameters;
  temperature_phrase_id phrase_id(p.theTemperaturePhraseId);
  forecast_season_id season_id(p.theSeasonId);

  if (p.theForecastPeriodId == NIGHT_PERIOD)
  {
    handle_night_period_phrase(p, phrase_id, season_id, theDayPhasePhrase, theTemperaturePhrase);
  }
  else
  {
    forecast_subperiod_id subperiod_id(p.theSubPeriodId);
    if (subperiod_id == UNDEFINED_SUBPERIOD)
    {
      handle_day_undefined_subperiod(
          p, phrase_id, season_id, theDayPhasePhrase, theTemperaturePhrase);
    }
    else if (season_id == SUMMER_SEASON)
    {
      if (subperiod_id == DAY1_MORNING_PERIOD || subperiod_id == DAY2_MORNING_PERIOD)
      {
        theDayPhasePhrase << AAMULLA_PHRASE;
        if (phrase_id != PIKKUPAKKASTA_PHRASE_ID)
          theTemperaturePhrase << LAMPOTILA_WORD;
      }
      else
      {
        theDayPhasePhrase << ILTAPAIVALLA_PHRASE;
      }
    }
  }

  if (p.inlandAndCoastSeparated())
  {
    if (p.theForecastAreaId == COASTAL_AREA)
      theAreaPhrase << RANNIKOLLA_PHRASE;
    else if (p.theForecastAreaId == INLAND_AREA)
      theAreaPhrase << SISAMAASSA_PHRASE;
  }
}

namespace
{
// Shared night period handler for both summer and non-summer: sets dayPhaseString and fills
// theDayPhasePhrase Returns true if a "yolla" type phrase was used (no tautology reset needed),
// false otherwise
void handle_night_tphrase_summer(t36hparams& p,
                                 Sentence& theDayPhasePhrase,
                                 std::string& dayPhaseString)
{
  // bugfix 13.4.2011: "Yon alin lampotila on hieman kylmempaa" => "Yolla on hieman kylmempaa"
  temperature_phrase_id pid = p.theTemperaturePhraseId;
  if (pid == PIKKUPAKKASTA_PHRASE_ID || pid == HIEMAN_HEIKOMPAA_PHRASE_ID ||
      pid == HIEMAN_LAUHEMPAA_PHRASE_ID || pid == HIEMAN_KIREAMPAA_PHRASE_ID ||
      pid == HIEMAN_KYLMEMPAA_PHRASE_ID)
  {
    dayPhaseString = YOLLA_PHRASE;
    if (pid == HIEMAN_KIREAMPAA_PHRASE_ID || pid == HIEMAN_HEIKOMPAA_PHRASE_ID)
      theDayPhasePhrase << YOLLA_PHRASE << PAKKANEN_WORD << ON_WORD;
    else
      theDayPhasePhrase << YOLLA_PHRASE << ON_WORD;
  }
  else if (pid == LAMPOTILA_NOUSEE_PHRASE_ID)
  {
    dayPhaseString = YOLLA_PHRASE;
    theDayPhasePhrase << YOLLA_PHRASE;
  }
  else
  {
    theDayPhasePhrase << (p.theUseLongPhrase ? YON_ALIN_LAMPOTILA_LONG_PHRASE
                                             : YON_ALIN_LAMPOTILA_SHORT_PHRASE);
    if (p.theUseLongPhrase)
      theDayPhasePhrase << ON_WORD;
    p.theNightPeriodTautologyFlag = true;
  }
  p.theDayPeriodTautologyFlag = false;
}

void handle_night_tphrase_winter(t36hparams& p,
                                 Sentence& theDayPhasePhrase,
                                 std::string& dayPhaseString)
{
  temperature_phrase_id pid = p.theTemperaturePhraseId;
  if (pid == PIKKUPAKKASTA_PHRASE_ID || pid == HIEMAN_HEIKOMPAA_PHRASE_ID ||
      pid == HIEMAN_LAUHEMPAA_PHRASE_ID || pid == HIEMAN_KIREAMPAA_PHRASE_ID ||
      pid == HIEMAN_KYLMEMPAA_PHRASE_ID)
  {
    dayPhaseString = YOLLA_PHRASE;
    if (pid == HIEMAN_KIREAMPAA_PHRASE_ID || pid == HIEMAN_HEIKOMPAA_PHRASE_ID)
      theDayPhasePhrase << YOLLA_PHRASE << PAKKANEN_WORD << ON_WORD;
    else
      theDayPhasePhrase << YOLLA_PHRASE << ON_WORD;
  }
  else if (pid == LAMPOTILA_NOUSEE_PHRASE_ID)
  {
    dayPhaseString = YOLLA_PHRASE;
    theDayPhasePhrase << YOLLA_PHRASE;
  }
  else
  {
    if (p.numberOfPeriods() > 1)
      theDayPhasePhrase << YOLAMPOTILA_PHRASE << ON_WORD;
    else
      theDayPhasePhrase << LAMPOTILA_WORD << ON_WORD;
    p.theNightPeriodTautologyFlag = true;
  }
  p.theDayPeriodTautologyFlag = false;
}

bool handle_summer_day_use_day_phrase(t36hparams& p, Sentence& theDayPhasePhrase)
{
  if (p.theTemperaturePhraseId == PIKKUPAKKASTA_PHRASE_ID)
  {
    theDayPhasePhrase << ON_WORD;
    return true;  // plainIsVerbUsed
  }
  if (p.theFullDayFlag)
  {
    if (p.theUseLongPhrase)
      theDayPhasePhrase << PAIVAN_YLIN_LAMPOTILA_LONG_PHRASE << ON_WORD;
    else
      theDayPhasePhrase << PAIVAN_YLIN_LAMPOTILA_SHORT_PHRASE;
  }
  else
  {
    theDayPhasePhrase << LAMPOTILA_WORD << ON_WORD;
  }
  p.theDayPeriodTautologyFlag = true;
  return false;
}

bool handle_summer_day_phrase(t36hparams& p,
                              bool useDayTemperaturePhrase,
                              Sentence& theDayPhasePhrase,
                              std::string& dayPhaseString)
{
  bool useDayPhrase =
      (p.theSubPeriodId == UNDEFINED_SUBPERIOD && useDayTemperaturePhrase &&
       !p.theDayPeriodTautologyFlag) ||
      (p.theForecastPeriodId == DAY2_PERIOD &&
       (p.theForecastAreaId == INLAND_AREA || p.theForecastAreaId == FULL_AREA) &&
       ((p.inlandAndCoastSeparated(DAY2_PERIOD) && p.theForecastAreaId == INLAND_AREA) ||
        p.morningAndAfternoonSeparated(DAY1_PERIOD)));

  if (useDayPhrase)
    return handle_summer_day_use_day_phrase(p, theDayPhasePhrase);

  p.theDayPeriodTautologyFlag = false;
  bool isPikkupakkasta = (p.theTemperaturePhraseId == PIKKUPAKKASTA_PHRASE_ID);

  if (p.theSubPeriodId == DAY1_MORNING_PERIOD || p.theSubPeriodId == DAY2_MORNING_PERIOD)
  {
    if (isPikkupakkasta)
    {
      dayPhaseString = AAMULLA_PHRASE;
      theDayPhasePhrase << AAMULLA_PHRASE << ON_WORD;
    }
    else
    {
      theDayPhasePhrase << LAMPOTILA_WORD << ON_WORD << AAMULLA_PHRASE;
    }
  }
  else if (p.theSubPeriodId == DAY1_AFTERNOON_PERIOD || p.theSubPeriodId == DAY2_AFTERNOON_PERIOD)
  {
    dayPhaseString = ILTAPAIVALLA_PHRASE;
    theDayPhasePhrase << ILTAPAIVALLA_PHRASE;
    if (isPikkupakkasta)
      theDayPhasePhrase << ON_WORD;
  }
  return false;
}

bool handle_winter_day_phrase(t36hparams& p,
                              Sentence& theDayPhasePhrase,
                              std::string& dayPhaseString)
{
  bool plainIsVerbUsed = false;
  bool useDayPhrase = p.theSubPeriodId == UNDEFINED_SUBPERIOD &&
                      (!p.theDayPeriodTautologyFlag || (p.inlandAndCoastSeparated(DAY2_PERIOD) &&
                                                        p.theForecastPeriodId == DAY2_PERIOD &&
                                                        p.theForecastAreaId == INLAND_AREA));

  if (useDayPhrase)
  {
    if (p.theTemperaturePhraseId == PIKKUPAKKASTA_PHRASE_ID)
    {
      dayPhaseString = PAIVALLA_PHRASE;
      theDayPhasePhrase << PAIVALLA_PHRASE << ON_WORD;
      plainIsVerbUsed = true;
    }
    else
    {
      if (p.theDayAndNightSeparationFlag)
        theDayPhasePhrase << PAIVALAMPOTILA_PHRASE << ON_WORD;
      else
        theDayPhasePhrase << LAMPOTILA_WORD << ON_WORD;
      p.theDayPeriodTautologyFlag = true;
    }
  }
  return plainIsVerbUsed;
}

void append_frost_or_day_phrase(t36hparams& p,
                                Sentence& sentence,
                                const Sentence& theDayPhasePhrase,
                                bool coastal)
{
  if (p.theUseFrostExistsPhrase)
  {
    // if only one period exists, dont use word 'yolla'/'paivalla'
    if (p.numberOfPeriods() > 1 && p.theDayAndNightSeparationFlag)
    {
      sentence << (p.theForecastPeriodId == NIGHT_PERIOD ? YOLLA_PHRASE : PAIVALLA_PHRASE);
    }
    if (!p.theFrostExistsTautologyFlag)
    {
      sentence << PAKKASTA_WORD << ON_WORD;
      p.theFrostExistsTautologyFlag = true;
    }
    else
    {
      sentence << LAMPOTILA_WORD << ON_WORD;
      p.theFrostExistsTautologyFlag = false;
    }
    if (!coastal)
      p.theUseFrostExistsPhrase = false;
  }
  else
  {
    sentence << theDayPhasePhrase;
  }
}
}  // namespace

Sentence temperature_phrase(t36hparams& theParameters)
{
  Sentence sentence;
  auto& p = theParameters;

  bool useDayTemperaturePhrase =
      (p.theForecastPeriodId == DAY1_PERIOD ||
       ((p.theForecastPeriodId == DAY2_PERIOD && !(p.theForecastPeriod & DAY1_PERIOD)) ||
        p.inlandAndCoastSeparated(DAY1_PERIOD)));

  std::string dayPhaseString(EMPTY_STRING);
  Sentence theDayPhasePhrase;
  Sentence theAreaPhrase;
  bool plainIsVerbUsed = false;

  if (p.theSeasonId == SUMMER_SEASON)
  {
    if (p.theForecastPeriodId == NIGHT_PERIOD && !p.theNightPeriodTautologyFlag)
    {
      handle_night_tphrase_summer(p, theDayPhasePhrase, dayPhaseString);
    }
    else if (p.theForecastPeriodId != NIGHT_PERIOD)
    {
      p.theNightPeriodTautologyFlag = false;
      plainIsVerbUsed =
          handle_summer_day_phrase(p, useDayTemperaturePhrase, theDayPhasePhrase, dayPhaseString);
    }
  }
  else
  {
    if (p.theForecastPeriodId == NIGHT_PERIOD && !p.theNightPeriodTautologyFlag)
    {
      handle_night_tphrase_winter(p, theDayPhasePhrase, dayPhaseString);
    }
    else if (p.theForecastPeriodId != NIGHT_PERIOD)
    {
      p.theNightPeriodTautologyFlag = false;
      plainIsVerbUsed = handle_winter_day_phrase(p, theDayPhasePhrase, dayPhaseString);
    }
  }

  if (dayPhaseString.empty())
    dayPhaseString = EMPTY_STRING;

  if (p.theForecastAreaId == COASTAL_AREA && p.inlandAndCoastSeparated() &&
      !p.theOnCoastalAreaTautologyFlag)
  {
    theAreaPhrase << RANNIKOLLA_PHRASE;
    p.theOnCoastalAreaTautologyFlag = true;
    p.theOnInlandAreaTautologyFlag = false;
  }
  else if (p.theForecastAreaId == INLAND_AREA && p.inlandAndCoastSeparated() &&
           !p.theOnInlandAreaTautologyFlag)
  {
    theAreaPhrase << SISAMAASSA_PHRASE;
    p.theOnInlandAreaTautologyFlag = true;
    p.theOnCoastalAreaTautologyFlag = false;
  }

  if (p.theForecastAreaId == COASTAL_AREA || plainIsVerbUsed)
  {
    sentence << theAreaPhrase;
    append_frost_or_day_phrase(p, sentence, theDayPhasePhrase, true);
  }
  else
  {
    append_frost_or_day_phrase(p, sentence, theDayPhasePhrase, false);
    sentence << theAreaPhrase;
  }

  return sentence;
}

namespace
{
string degrees_string_for_phrase(int degrees, temperature_phrase_id phrase_id)
{
  bool tienoilla = (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID);
  bool tienoilla_or_tuntumassa =
      (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID || phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID);
  bool mod10_1 = (abs(degrees) % 10 == 1 && abs(degrees) != 11);

  if (mod10_1)
  {
    if (tienoilla_or_tuntumassa)
      return tienoilla ? "asteen(tienoilla (mod 10=1))" : "asteen(tuntumassa (mod 10=1))";
    if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
      return "astetta(noin (mod 10=1))";
    return {};
  }
  if (tienoilla_or_tuntumassa)
    return tienoilla ? "asteen(tienoilla)" : "asteen(tuntumassa)";
  if (abs(degrees) > 4)
    return "astetta(noin n)";
  return "astetta(noin " + std::to_string(abs(degrees)) + ")";
}
}  // namespace

Sentence tienoilla_and_tuntumassa_astetta(int degrees, temperature_phrase_id phrase_id)
{
  Sentence sentence;

  const string var = "textgen::units::celsius::format";
  const string opt = Settings::optional_string(var, "SI");

  if (opt == "SI")
  {
    sentence << Integer(degrees) << *UnitFactory::create_unit(DegreesCelsius, degrees);
  }
  else if (opt == "phrase")
  {
    sentence << Integer(degrees) << degrees_string_for_phrase(degrees, phrase_id);
  }
  else if (opt == "none")
    ;
  else
    throw Fmi::Exception(BCP, "Unknown format " + opt + " in variable " + var);

  return sentence;
}

namespace
{
// Lookup table helpers for construct_final_sentence

// Returns the "coastal" composite phrase string for a given phrase_id
const char* coastal_composite_phrase(temperature_phrase_id pid)
{
  switch (pid)
  {
    case NOIN_ASTETTA_PHRASE_ID:
      return RANNIKOLLA_NOIN_ASTETTA_COMPOSITE_PHRASE;
    case TIENOILLA_ASTETTA_PHRASE_ID:
      return RANNIKOLLA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
    case TUNTUMASSA_ASTETTA_PHRASE_ID:
      return RANNIKOLLA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_YLI_ASTETTA_PHRASE_ID:
      return RANNIKOLLA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
    case VAJAAT_ASTETTA_PHRASE_ID:
      return RANNIKOLLA_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
    case LAHELLA_ASTETTA_PHRASE_ID:
      return RANNIKOLLA_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID:
      return RANNIKOLLA_VAHAN_NOLLAN_ALAPUOLELLA_PHRASE;
    default:
      return "";
  }
}

// Select frost composite phrase for (dayEmpty, areaEmpty) combos
const char* frost_composite_phrase(temperature_phrase_id pid, bool dayEmpty, bool areaEmpty)
{
  if (dayEmpty && areaEmpty)
  {
    switch (pid)
    {
      case NOIN_ASTETTA_PHRASE_ID:
        return PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
      case TIENOILLA_ASTETTA_PHRASE_ID:
        return PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
      case TUNTUMASSA_ASTETTA_PHRASE_ID:
        return PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
      case VAHAN_YLI_ASTETTA_PHRASE_ID:
        return PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
      case VAJAAT_ASTETTA_PHRASE_ID:
        return PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
      case LAHELLA_ASTETTA_PHRASE_ID:
        return PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
      default:
        return "";
    }
  }
  if (!dayEmpty && areaEmpty)
  {
    switch (pid)
    {
      case NOIN_ASTETTA_PHRASE_ID:
        return HUOMENNA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
      case TIENOILLA_ASTETTA_PHRASE_ID:
        return HUOMENNA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
      case TUNTUMASSA_ASTETTA_PHRASE_ID:
        return HUOMENNA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
      case VAHAN_YLI_ASTETTA_PHRASE_ID:
        return HUOMENNA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
      case VAJAAT_ASTETTA_PHRASE_ID:
        return HUOMENNA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
      case LAHELLA_ASTETTA_PHRASE_ID:
        return HUOMENNA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
      default:
        return "";
    }
  }
  if (dayEmpty && !areaEmpty)
  {
    switch (pid)
    {
      case NOIN_ASTETTA_PHRASE_ID:
        return SISAMAASSA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
      case TIENOILLA_ASTETTA_PHRASE_ID:
        return SISAMAASSA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
      case TUNTUMASSA_ASTETTA_PHRASE_ID:
        return SISAMAASSA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
      case VAHAN_YLI_ASTETTA_PHRASE_ID:
        return SISAMAASSA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
      case VAJAAT_ASTETTA_PHRASE_ID:
        return SISAMAASSA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
      case LAHELLA_ASTETTA_PHRASE_ID:
        return SISAMAASSA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
      default:
        return "";
    }
  }
  // both present
  switch (pid)
  {
    case NOIN_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
    case TIENOILLA_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
    case TUNTUMASSA_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_YLI_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
    case VAJAAT_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
    case LAHELLA_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
    default:
      return "";
  }
}

// Returns the "normal" (non-frost, non-coastal) composite phrase
// useDay2Phrase=true: "huomenna X" phrases; otherwise "lampotila X" phrases
const char* normal_composite_phrase_day_empty_area_empty(temperature_phrase_id pid,
                                                         bool useLongPhrase)
{
  switch (pid)
  {
    case NOIN_ASTETTA_PHRASE_ID:
      return useLongPhrase ? LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE
                           : LAMPOTILA_NOIN_ASTETTA_COMPOSITE_PHRASE;
    case TIENOILLA_ASTETTA_PHRASE_ID:
      return useLongPhrase ? LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE
                           : LAMPOTILA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
    case TUNTUMASSA_ASTETTA_PHRASE_ID:
      return useLongPhrase ? LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE
                           : LAMPOTILA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_YLI_ASTETTA_PHRASE_ID:
      return useLongPhrase ? LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE
                           : LAMPOTILA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
    case VAJAAT_ASTETTA_PHRASE_ID:
      return useLongPhrase ? LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE
                           : LAMPOTILA_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
    case LAHELLA_ASTETTA_PHRASE_ID:
      return useLongPhrase ? LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE
                           : LAMPOTILA_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID:
      return useLongPhrase ? LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE
                           : LAMPOTILA_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE;
    default:
      return "";
  }
}

const char* normal_composite_phrase_day_present_area_empty(temperature_phrase_id pid,
                                                           bool useDay2Phrase)
{
  if (useDay2Phrase)
  {
    switch (pid)
    {
      case NOIN_ASTETTA_PHRASE_ID:
        return HUOMENNA_NOIN_ASTETTA_COMPOSITE_PHRASE;
      case TIENOILLA_ASTETTA_PHRASE_ID:
        return HUOMENNA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
      case TUNTUMASSA_ASTETTA_PHRASE_ID:
        return HUOMENNA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
      case VAHAN_YLI_ASTETTA_PHRASE_ID:
        return HUOMENNA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
      case VAJAAT_ASTETTA_PHRASE_ID:
        return HUOMENNA_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
      case LAHELLA_ASTETTA_PHRASE_ID:
        return HUOMENNA_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
      case VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID:
        return HUOMENNA_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE;
      default:
        return "";
    }
  }
  switch (pid)
  {
    case NOIN_ASTETTA_PHRASE_ID:
      return HUOMENNA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
    case TIENOILLA_ASTETTA_PHRASE_ID:
      return HUOMENNA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
    case TUNTUMASSA_ASTETTA_PHRASE_ID:
      return HUOMENNA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_YLI_ASTETTA_PHRASE_ID:
      return HUOMENNA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
    case VAJAAT_ASTETTA_PHRASE_ID:
      return HUOMENNA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
    case LAHELLA_ASTETTA_PHRASE_ID:
      return HUOMENNA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID:
      return HUOMENNA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE;
    default:
      return "";
  }
}

const char* normal_composite_phrase(temperature_phrase_id pid,
                                    bool dayEmpty,
                                    bool areaEmpty,
                                    bool useDay2Phrase,
                                    bool useLongPhrase)
{
  if (dayEmpty && areaEmpty)
    return normal_composite_phrase_day_empty_area_empty(pid, useLongPhrase);
  if (!dayEmpty && areaEmpty)
    return normal_composite_phrase_day_present_area_empty(pid, useDay2Phrase);
  if (dayEmpty && !areaEmpty)
  {
    switch (pid)
    {
      case NOIN_ASTETTA_PHRASE_ID:
        return SISAMAASSA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
      case TIENOILLA_ASTETTA_PHRASE_ID:
        return SISAMAASSA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
      case TUNTUMASSA_ASTETTA_PHRASE_ID:
        return SISAMAASSA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
      case VAHAN_YLI_ASTETTA_PHRASE_ID:
        return SISAMAASSA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
      case VAJAAT_ASTETTA_PHRASE_ID:
        return SISAMAASSA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
      case LAHELLA_ASTETTA_PHRASE_ID:
        return SISAMAASSA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
      case VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID:
        return SISAMAASSA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE;
      default:
        return "";
    }
  }
  // both present
  switch (pid)
  {
    case NOIN_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
    case TIENOILLA_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
    case TUNTUMASSA_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_YLI_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
    case VAJAAT_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
    case LAHELLA_ASTETTA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
    case VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID:
      return HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE;
    default:
      return "";
  }
}

bool is_tienoilla_type(temperature_phrase_id pid)
{
  return (pid == TIENOILLA_ASTETTA_PHRASE_ID || pid == TUNTUMASSA_ASTETTA_PHRASE_ID ||
          pid == NOIN_ASTETTA_PHRASE_ID);
}

// Build sentence for the coastal case (NOIN/TIENOILLA/... + coastal area)
void build_coastal_numeric_sentence(Sentence& sentence,
                                    temperature_phrase_id phrase_id,
                                    int intervalStart,
                                    const Sentence& degreesSentence)
{
  sentence << coastal_composite_phrase(phrase_id);
  if (is_tienoilla_type(phrase_id))
    sentence << tienoilla_and_tuntumassa_astetta(intervalStart, phrase_id);
  else
    sentence << degreesSentence;
}

// Build sentence for the pakkanen (frost) case
void build_frost_numeric_sentence(Sentence& sentence,
                                  temperature_phrase_id phrase_id,
                                  bool dayEmpty,
                                  bool areaEmpty,
                                  int pakkanenDegrees,
                                  const Sentence& pakkanenDegreesSentence,
                                  const Sentence& theDayPhasePhrase,
                                  const Sentence& theAreaPhrase)
{
  sentence << frost_composite_phrase(phrase_id, dayEmpty, areaEmpty);
  Sentence degreesForSentence = is_tienoilla_type(phrase_id)
                                    ? tienoilla_and_tuntumassa_astetta(pakkanenDegrees, phrase_id)
                                    : pakkanenDegreesSentence;
  if (dayEmpty && areaEmpty)
    sentence << degreesForSentence;
  else if (!dayEmpty && areaEmpty)
    sentence << theDayPhasePhrase << degreesForSentence;
  else if (dayEmpty && !areaEmpty)
    sentence << theAreaPhrase << degreesForSentence;
  else
    sentence << theDayPhasePhrase << theAreaPhrase << degreesForSentence;
}

// Build sentence for normal (non-frost, non-coastal) numeric case
// Returns false if a "break" equivalent is needed (VAHAN_NOLLAN_ALAPUOLELLA special case)
bool build_normal_numeric_sentence(Sentence& sentence,
                                   temperature_phrase_id phrase_id,
                                   bool dayEmpty,
                                   bool areaEmpty,
                                   bool useDay2Phrase,
                                   bool useLongPhrase,
                                   int intervalStart,
                                   const Sentence& degreesSentence,
                                   const Sentence& theDayPhasePhrase,
                                   const Sentence& theAreaPhrase,
                                   const Sentence& theTemperaturePhrase)
{
  const char* composite =
      normal_composite_phrase(phrase_id, dayEmpty, areaEmpty, useDay2Phrase, useLongPhrase);
  sentence << composite;

  // Special case: VAHAN_NOLLAN_ALAPUOLELLA - append different trailing sentences and signal break
  if (phrase_id == VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID)
  {
    if (dayEmpty && areaEmpty)
      sentence << theTemperaturePhrase;
    else if (!dayEmpty && areaEmpty)
    {
      if (useDay2Phrase)
        sentence << theDayPhasePhrase;
      else
        sentence << theDayPhasePhrase << theTemperaturePhrase;
    }
    else if (dayEmpty && !areaEmpty)
      sentence << theAreaPhrase << theTemperaturePhrase;
    else
      sentence << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase;
    return false;  // signals "break" needed
  }

  // Normal trailing
  Sentence degreesOrTienoilla = is_tienoilla_type(phrase_id)
                                    ? tienoilla_and_tuntumassa_astetta(intervalStart, phrase_id)
                                    : degreesSentence;

  if (dayEmpty && areaEmpty)
  {
    sentence << theTemperaturePhrase << degreesOrTienoilla;
  }
  else if (!dayEmpty && areaEmpty)
  {
    if (useDay2Phrase)
      sentence << theDayPhasePhrase << degreesOrTienoilla;
    else
      sentence << theDayPhasePhrase << theTemperaturePhrase << degreesOrTienoilla;
  }
  else if (dayEmpty && !areaEmpty)
  {
    sentence << theAreaPhrase << theTemperaturePhrase << degreesOrTienoilla;
  }
  else
  {
    sentence << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase << degreesOrTienoilla;
  }
  return true;
}

Sentence build_hieman_korkeampi_alempi_sentence(t36hparams& p,
                                                temperature_phrase_id phrase_id,
                                                bool useDay2Phrase,
                                                bool dayEmpty,
                                                bool areaEmpty,
                                                const Sentence& theDayPhasePhrase,
                                                const Sentence& theAreaPhrase,
                                                const Sentence& theTemperaturePhrase)
{
  Sentence sentence;
  if (useDay2Phrase)
  {
    if (dayEmpty && areaEmpty)
    {
      sentence << (p.theUseLongPhrase ? LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE
                                      : LAMPOTILA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE);
      sentence << theTemperaturePhrase;
    }
    else if (!dayEmpty && areaEmpty)
      sentence << SISAMAASSA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase;
    else if (dayEmpty && !areaEmpty)
      sentence << HUOMENNA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theAreaPhrase;
    else
      sentence << HUOMENNA_SISAMAASSA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase
               << theAreaPhrase;
  }
  else
  {
    if (dayEmpty && areaEmpty)
    {
      sentence << (p.theUseLongPhrase ? LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE
                                      : LAMPOTILA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE);
      sentence << theTemperaturePhrase;
    }
    else if (!dayEmpty && areaEmpty)
    {
      sentence << HUOMENNA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase
               << theTemperaturePhrase;
    }
    else if (dayEmpty && !areaEmpty)
    {
      sentence << SISAMAASSA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theAreaPhrase
               << theTemperaturePhrase;
    }
    else
    {
      sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE
               << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase;
    }
  }
  sentence << (phrase_id == HIEMAN_KORKEAMPI_PHRASE_ID ? HIEMAN_KORKEAMPI_PHRASE
                                                       : HIEMAN_ALEMPI_PHRASE);
  return sentence;
}

Sentence build_intervalli_sentence(t36hparams& p,
                                   bool useDay2Phrase,
                                   bool dayEmpty,
                                   bool areaEmpty,
                                   int intervalStart,
                                   int intervalEnd,
                                   const Sentence& temperatureSentence,
                                   const Sentence& theDayPhasePhrase,
                                   const Sentence& theAreaPhrase,
                                   const Sentence& theTemperaturePhrase)
{
  Sentence sentence;
  if (p.theForecastAreaId == COASTAL_AREA && p.inlandAndCoastSeparated())
  {
    Sentence temperatureRangeSentence;
    int actualIntervalStart(intervalStart);
    int actualIntervalEnd(intervalEnd);
    temperatureRangeSentence << temperature_range(
        intervalStart, intervalEnd, p.theRangeSeparator, actualIntervalStart, actualIntervalEnd);
    sentence << RANNIKOLLA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE << temperatureRangeSentence
             << *UnitFactory::create_unit(DegreesCelsius, actualIntervalEnd, true);
    return sentence;
  }
  if (useDay2Phrase)
  {
    sentence << HUOMENNA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE << theDayPhasePhrase
             << temperatureSentence;
    return sentence;
  }
  if (dayEmpty && areaEmpty)
  {
    sentence << (p.theUseLongPhrase ? LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE
                                    : LAMPOTILA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE);
    sentence << theTemperaturePhrase << temperatureSentence;
  }
  else if (!dayEmpty && areaEmpty)
  {
    sentence << HUOMENNA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE << theDayPhasePhrase
             << theTemperaturePhrase << temperatureSentence;
  }
  else if (dayEmpty && !areaEmpty)
  {
    sentence << SISAMAASSA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE << theAreaPhrase
             << theTemperaturePhrase << temperatureSentence;
  }
  else
  {
    sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE
             << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase << temperatureSentence;
  }
  return sentence;
}

Sentence build_suunnilleen_sama_sentence(t36hparams& p,
                                         bool useDay2Phrase,
                                         bool dayEmpty,
                                         bool areaEmpty,
                                         const Sentence& temperatureSentence,
                                         const Sentence& theDayPhasePhrase,
                                         const Sentence& theAreaPhrase,
                                         const Sentence& theTemperaturePhrase)
{
  Sentence sentence;
  if (p.theForecastAreaId == COASTAL_AREA && p.inlandAndCoastSeparated())
  {
    sentence << RANNIKOLLA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << temperatureSentence;
    return sentence;
  }
  if (useDay2Phrase)
  {
    sentence << HUOMENNA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase << theAreaPhrase
             << temperatureSentence;
    return sentence;
  }
  if (dayEmpty && areaEmpty)
  {
    sentence << (p.theUseLongPhrase ? LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE
                                    : LAMPOTILA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE);
    sentence << theTemperaturePhrase << temperatureSentence;
  }
  else if (!dayEmpty && areaEmpty)
  {
    sentence << HUOMENNA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase
             << theTemperaturePhrase << temperatureSentence;
  }
  else if (dayEmpty && !areaEmpty)
  {
    sentence << SISAMAASSA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theAreaPhrase
             << theTemperaturePhrase << temperatureSentence;
  }
  else
  {
    sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE
             << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase << temperatureSentence;
  }
  return sentence;
}
}  // namespace

namespace
{
Sentence build_pikkupakkasta_sentence(bool dayEmpty,
                                      bool areaEmpty,
                                      const Sentence& theDayPhasePhrase,
                                      const Sentence& theAreaPhrase)
{
  Sentence s;
  if (dayEmpty && areaEmpty)
    s << PIKKUPAKKASTA_PHRASE;
  else if (!dayEmpty && areaEmpty)
    s << HUOMENNA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE << theDayPhasePhrase;
  else if (dayEmpty && !areaEmpty)
    s << RANNIKOLLA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE << theAreaPhrase;
  else
    s << HUOMENNA_RANNIKOLLA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE << theDayPhasePhrase
      << theAreaPhrase;
  return s;
}

Sentence build_hieman_lauhempaa_sentence(temperature_phrase_id pid,
                                         bool dayEmpty,
                                         bool areaEmpty,
                                         const Sentence& theDayPhasePhrase,
                                         const Sentence& theAreaPhrase)
{
  Sentence s;
  if (!dayEmpty && areaEmpty)
    s << HUOMENNA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE << theDayPhasePhrase;
  else if (dayEmpty && !areaEmpty)
    s << SISAMAASSA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE << theAreaPhrase;
  else
    s << HUOMENNA_SISAMAASSA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE << theDayPhasePhrase
      << theAreaPhrase;
  s << (pid == HIEMAN_LAUHEMPAA_PHRASE_ID ? HIEMAN_LAUHEMPAA_PHRASE : HIEMAN_KYLMEMPAA_PHRASE);
  return s;
}

Sentence build_hieman_heikompaa_sentence(temperature_phrase_id pid,
                                         bool dayEmpty,
                                         bool areaEmpty,
                                         const Sentence& theDayPhasePhrase,
                                         const Sentence& theAreaPhrase)
{
  Sentence s;
  if (!dayEmpty && areaEmpty)
    s << HUOMENNA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE << theDayPhasePhrase;
  else if (dayEmpty && !areaEmpty)
    s << SISAMAASSA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE << theAreaPhrase;
  else
    s << HUOMENNA_SISAMAASSA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE << theDayPhasePhrase
      << theAreaPhrase;
  s << (pid == HIEMAN_HEIKOMPAA_PHRASE_ID ? HIEMAN_HEIKOMPAA_PHRASE : HIEMAN_KIREAMPAA_PHRASE);
  return s;
}

void fill_area_phrase_from_weather_area(t36hparams& p, Sentence& theAreaPhrase)
{
  const auto type = p.theWeatherArea.type();
  if (type == WeatherArea::Northern)
    theAreaPhrase << ALUEEN_POHJOISOSASSA_PHRASE;
  else if (type == WeatherArea::Southern)
    theAreaPhrase << ALUEEN_ETELAOSASSA_PHRASE;
  else if (type == WeatherArea::Eastern)
    theAreaPhrase << ALUEEN_ITAOSASSA_PHRASE;
  else if (type == WeatherArea::Western)
    theAreaPhrase << ALUEEN_LANSIOSASSA_PHRASE;
}
}  // namespace

Sentence construct_final_sentence(t36hparams& theParameters,
                                  const Sentence& temperatureSentence,
                                  const Sentence& daySentence,
                                  int intervalStart,
                                  int intervalEnd)
{
  Sentence sentence;

  Sentence theDayPhasePhrase;
  Sentence theTemperaturePhrase;
  Sentence theAreaPhrase;
  Sentence degreesSentence;
  bool dayPhasePhraseEmpty(false);
  bool areaPhraseEmpty(false);

  bool useDay2Phrase = (theParameters.theForecastPeriodId == DAY2_PERIOD &&
                        (theParameters.theForecastPeriod & DAY1_PERIOD &&
                         theParameters.theForecastPeriod & DAY2_PERIOD) &&
                        !theParameters.inlandAndCoastSeparated(DAY1_PERIOD));

  fill_area_phrase_from_weather_area(theParameters, theAreaPhrase);

  temperature_phrase_id phrase_id(theParameters.theTemperaturePhraseId);

  temperature_phrase(theParameters, theDayPhasePhrase, theTemperaturePhrase, theAreaPhrase);

  bool pakkastaOn = (as_string(theTemperaturePhrase).compare(0, 8, "Pakkasta") == 0);

  if (theDayPhasePhrase.empty())
  {
    if (!daySentence.empty())
      theDayPhasePhrase << daySentence;
    else
    {
      theDayPhasePhrase << EMPTY_STRING;
      dayPhasePhraseEmpty = true;
    }
  }

  if (theTemperaturePhrase.empty())
    theTemperaturePhrase << EMPTY_STRING;

  if (theAreaPhrase.empty())
  {
    theAreaPhrase << EMPTY_STRING;
    areaPhraseEmpty = true;
  }

  if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID || phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
    degreesSentence << intervalStart;
  else if (intervalStart == 0)
    degreesSentence << NOLLA_WORD << *UnitFactory::create_unit(DegreesCelsius, intervalStart);
  else
    degreesSentence << intervalStart << *UnitFactory::create_unit(DegreesCelsius, intervalStart);

  switch (phrase_id)
  {
    case PIKKUPAKKASTA_PHRASE_ID:
      sentence << build_pikkupakkasta_sentence(
          dayPhasePhraseEmpty, areaPhraseEmpty, theDayPhasePhrase, theAreaPhrase);
      break;

    case HIEMAN_LAUHEMPAA_PHRASE_ID:
    case HIEMAN_KYLMEMPAA_PHRASE_ID:
      sentence << build_hieman_lauhempaa_sentence(
          phrase_id, dayPhasePhraseEmpty, areaPhraseEmpty, theDayPhasePhrase, theAreaPhrase);
      break;

    case HIEMAN_HEIKOMPAA_PHRASE_ID:
    case HIEMAN_KIREAMPAA_PHRASE_ID:
      sentence << build_hieman_heikompaa_sentence(
          phrase_id, dayPhasePhraseEmpty, areaPhraseEmpty, theDayPhasePhrase, theAreaPhrase);
      break;

    case HIEMAN_KORKEAMPI_PHRASE_ID:
    case HIEMAN_ALEMPI_PHRASE_ID:
      sentence << build_hieman_korkeampi_alempi_sentence(theParameters,
                                                         phrase_id,
                                                         useDay2Phrase,
                                                         dayPhasePhraseEmpty,
                                                         areaPhraseEmpty,
                                                         theDayPhasePhrase,
                                                         theAreaPhrase,
                                                         theTemperaturePhrase);
      break;

    case LAMPOTILA_NOUSEE_PHRASE_ID:
      sentence << LAMPOTILA_NOUSEE_COMPOSITE_PHRASE << theDayPhasePhrase;
      break;

    case NOIN_ASTETTA_PHRASE_ID:
    case TIENOILLA_ASTETTA_PHRASE_ID:
    case TUNTUMASSA_ASTETTA_PHRASE_ID:
    case VAHAN_YLI_ASTETTA_PHRASE_ID:
    case VAJAAT_ASTETTA_PHRASE_ID:
    case LAHELLA_ASTETTA_PHRASE_ID:
    case VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID:
    {
      if (theParameters.theForecastAreaId == COASTAL_AREA &&
          theParameters.inlandAndCoastSeparated())
      {
        build_coastal_numeric_sentence(sentence, phrase_id, intervalStart, degreesSentence);
      }
      else if (pakkastaOn)
      {
        int pakkanenDegrees = abs(intervalStart);
        Sentence pakkanenDegreesSentence;
        pakkanenDegreesSentence << pakkanenDegrees
                                << *UnitFactory::create_unit(DegreesCelsius, pakkanenDegrees);
        build_frost_numeric_sentence(sentence,
                                     phrase_id,
                                     dayPhasePhraseEmpty,
                                     areaPhraseEmpty,
                                     pakkanenDegrees,
                                     pakkanenDegreesSentence,
                                     theDayPhasePhrase,
                                     theAreaPhrase);
      }
      else
      {
        bool cont = build_normal_numeric_sentence(sentence,
                                                  phrase_id,
                                                  dayPhasePhraseEmpty,
                                                  areaPhraseEmpty,
                                                  useDay2Phrase,
                                                  theParameters.theUseLongPhrase,
                                                  intervalStart,
                                                  degreesSentence,
                                                  theDayPhasePhrase,
                                                  theAreaPhrase,
                                                  theTemperaturePhrase);
        if (!cont)
          break;
      }
    }
    break;

    case LAMPOTILA_VALILLA_PHRASE_ID:
      sentence << build_intervalli_sentence(theParameters,
                                            useDay2Phrase,
                                            dayPhasePhraseEmpty,
                                            areaPhraseEmpty,
                                            intervalStart,
                                            intervalEnd,
                                            temperatureSentence,
                                            theDayPhasePhrase,
                                            theAreaPhrase,
                                            theTemperaturePhrase);
      break;

    case VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID:
    case NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID:
    case NOLLAN_TIENOILLA_PHRASE_ID:
    case SUUNNILLEEN_SAMA_PHRASE_ID:
      sentence << build_suunnilleen_sama_sentence(theParameters,
                                                  useDay2Phrase,
                                                  dayPhasePhraseEmpty,
                                                  areaPhraseEmpty,
                                                  temperatureSentence,
                                                  theDayPhasePhrase,
                                                  theAreaPhrase,
                                                  theTemperaturePhrase);
      break;

    case NO_PHRASE_ID:
      break;
  }

  if (theParameters.theAddCommaDelimiterFlag && !sentence.empty())
    theParameters.theSentenceUnderConstruction << Delimiter(COMMA_PUNCTUATION_MARK) << sentence;
  else
    theParameters.theSentenceUnderConstruction << sentence;

  return sentence;
}

namespace
{
// Shared helper: compute the "moderate change" temperature phrase based on temperature difference
// and mean Fills temperatureSentence and sets theTemperaturePhraseId
void apply_moderate_change_phrase(t36hparams& p,
                                  double temperatureDifference,
                                  Sentence& temperatureSentence)
{
  if (temperatureDifference > 0)
  {
    if (p.theMean <= PAKKASRAJA_TEMPERATURE)
    {
      temperatureSentence << HIEMAN_HEIKOMPAA_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_HEIKOMPAA_PHRASE_ID;
    }
    else if (p.theMean < 0)
    {
      temperatureSentence << HIEMAN_LAUHEMPAA_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_LAUHEMPAA_PHRASE_ID;
    }
    else
    {
      temperatureSentence << HIEMAN_KORKEAMPI_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_KORKEAMPI_PHRASE_ID;
    }
  }
  else
  {
    if (p.theMean <= PAKKASRAJA_TEMPERATURE)
    {
      temperatureSentence << HIEMAN_KIREAMPAA_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_KIREAMPAA_PHRASE_ID;
    }
    else if (p.theMean < 0)
    {
      temperatureSentence << HIEMAN_KYLMEMPAA_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_KYLMEMPAA_PHRASE_ID;
    }
    else
    {
      temperatureSentence << HIEMAN_ALEMPI_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_ALEMPI_PHRASE_ID;
    }
  }
}
}  // namespace

Sentence night_sentence(t36hparams& theParameters)
{
  Sentence sentence;
  auto& p = theParameters;

  double temperatureDifference = 100.0;
  int intervalStart = static_cast<int>(kFloatMissing);
  int intervalEnd = static_cast<int>(kFloatMissing);
  bool day1PeriodIncluded = p.theMaxTemperatureDay1 != kFloatMissing;

  if (day1PeriodIncluded)
    temperatureDifference = p.theMean - p.theMeanTemperatureDay1;

  // If day2 is included, we have to use numbers to describe temperature, since
  // day2 story is told before night story
  bool noDay2 = !(p.theForecastPeriod & DAY2_PERIOD);
  bool nightlyMinHigherThanDailyMax = noDay2 && (p.theMaxTemperatureDay1 - p.theMinimum < 0);
  bool smallChangeBetweenDay1AndNight =
      noDay2 && day1PeriodIncluded && abs(temperatureDifference) <= ABOUT_THE_SAME_UPPER_LIMIT;
  bool moderateChangeBetweenDay1AndNight =
      noDay2 && day1PeriodIncluded && abs(temperatureDifference) > ABOUT_THE_SAME_UPPER_LIMIT &&
      abs(temperatureDifference) <= SMALL_CHANGE_UPPER_LIMIT;

  p.theForecastPeriodId = NIGHT_PERIOD;
  p.theSubPeriodId = UNDEFINED_SUBPERIOD;

  Sentence temperatureSentence;

  bool inlandAndCoastSeparately = false;
  if (p.theForecastPeriod & DAY1_PERIOD)
    inlandAndCoastSeparately =
        p.inlandAndCoastSeparated(DAY1_PERIOD) || p.morningAndAfternoonSeparated(DAY1_PERIOD);
  if (!inlandAndCoastSeparately)
    inlandAndCoastSeparately = p.inlandAndCoastSeparated(NIGHT_PERIOD);

  if (around_zero_phrase(p) == NO_PHRASE_ID && !inlandAndCoastSeparately)
  {
    if (smallChangeBetweenDay1AndNight)
    {
      temperatureSentence << SUUNNILLEEN_SAMA_PHRASE;
      p.theTemperaturePhraseId = SUUNNILLEEN_SAMA_PHRASE_ID;
    }
    else if (moderateChangeBetweenDay1AndNight &&
             p.theTemperaturePhraseId != NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID)
    {
      apply_moderate_change_phrase(p, temperatureDifference, temperatureSentence);
    }
    else if (nightlyMinHigherThanDailyMax)
    {
      temperatureSentence << LAMPOTILA_NOUSEE_PHRASE;
      p.theTemperaturePhraseId = LAMPOTILA_NOUSEE_PHRASE_ID;
    }
    else
    {
      temperatureSentence << temperature_sentence(p, intervalStart, intervalEnd);
    }
  }
  else
  {
    temperatureSentence << temperature_sentence(p, intervalStart, intervalEnd);
  }

  Sentence tonightSentence;
  if (p.theDayAndNightSeparationFlag &&
      p.theFullPeriod.localEndTime().DifferenceInHours(p.theFullPeriod.localStartTime()) > 24)
  {
    tonightSentence << PeriodPhraseFactory::create(
        "tonight", p.theVariable, p.theForecastTime, p.theWeatherPeriod, p.theWeatherArea);
  }

  sentence << construct_final_sentence(
      p, temperatureSentence, tonightSentence, intervalStart, intervalEnd);
  p.theTomorrowTautologyFlag = false;
  return sentence;
}

namespace
{
// Day2 variant: positive moderate change uses ON_WORD prefix for "lauhempaa" phrase
void apply_moderate_change_phrase_day2(t36hparams& p,
                                       double temperatureDifference,
                                       Sentence& temperatureSentence)
{
  if (temperatureDifference > 0)
  {
    if (p.theMean <= PAKKASRAJA_TEMPERATURE)
    {
      temperatureSentence << HIEMAN_HEIKOMPAA_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_HEIKOMPAA_PHRASE_ID;
    }
    else if (p.theMean < 0)
    {
      temperatureSentence << ON_WORD << HIEMAN_LAUHEMPAA_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_LAUHEMPAA_PHRASE_ID;
    }
    else
    {
      temperatureSentence << HIEMAN_KORKEAMPI_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_KORKEAMPI_PHRASE_ID;
    }
  }
  else
  {
    if (p.theMean <= PAKKASRAJA_TEMPERATURE)
    {
      temperatureSentence << HIEMAN_KIREAMPAA_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_KIREAMPAA_PHRASE_ID;
    }
    else if (p.theMean < 0)
    {
      temperatureSentence << HIEMAN_KYLMEMPAA_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_KYLMEMPAA_PHRASE_ID;
    }
    else
    {
      temperatureSentence << HIEMAN_ALEMPI_PHRASE;
      p.theTemperaturePhraseId = HIEMAN_ALEMPI_PHRASE_ID;
    }
  }
}
}  // namespace

Sentence day2_sentence(t36hparams& theParameters)
{
  Sentence sentence;
  auto& p = theParameters;

  int intervalStart = static_cast<int>(kFloatMissing);
  int intervalEnd = static_cast<int>(kFloatMissing);

  double temperatureDifference = 100.0;
  bool day1PeriodIncluded = p.theForecastPeriod & DAY1_PERIOD;

  if (day1PeriodIncluded)
    temperatureDifference = p.theMean - p.theMeanTemperatureDay1;

  bool notSubperiod =
      (p.theSubPeriodId != DAY2_MORNING_PERIOD && p.theSubPeriodId != DAY2_AFTERNOON_PERIOD);
  bool smallChangeBetweenDay1AndDay2 =
      day1PeriodIncluded && abs(temperatureDifference) <= ABOUT_THE_SAME_UPPER_LIMIT;
  bool moderateChangeBetweenDay1AndDay2 = day1PeriodIncluded &&
                                          abs(temperatureDifference) > ABOUT_THE_SAME_UPPER_LIMIT &&
                                          abs(temperatureDifference) <= SMALL_CHANGE_UPPER_LIMIT;

  p.theForecastPeriodId = DAY2_PERIOD;

  Sentence temperatureSentence;

  if (around_zero_phrase(p) == NO_PHRASE_ID && !p.inlandAndCoastSeparated(DAY1_PERIOD) &&
      !p.inlandAndCoastSeparated(DAY2_PERIOD) && !p.morningAndAfternoonSeparated(DAY1_PERIOD))
  {
    if (smallChangeBetweenDay1AndDay2 && notSubperiod)
    {
      temperatureSentence << SUUNNILLEEN_SAMA_PHRASE;
      p.theTemperaturePhraseId = SUUNNILLEEN_SAMA_PHRASE_ID;
    }
    else if (moderateChangeBetweenDay1AndDay2 && notSubperiod &&
             p.theTemperaturePhraseId != NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID)
    {
      apply_moderate_change_phrase_day2(p, temperatureDifference, temperatureSentence);
    }
    else
    {
      temperatureSentence << temperature_sentence(p, intervalStart, intervalEnd);
    }
  }
  else
  {
    temperatureSentence << temperature_sentence(p, intervalStart, intervalEnd);
  }

  Sentence nextDaySentence;

  // exception: inland and coastal separated and morning and afternoon separated and now we are
  // processing
  // morning on the coastal area
  if (p.theFullPeriod.localEndTime().DifferenceInHours(p.theFullPeriod.localStartTime()) > 24)
  {
    // ARE 07.02.2011: if day1 and day2 are included
    if (p.theForecastPeriod & DAY1_PERIOD)
      const_cast<WeatherHistory&>(p.theWeatherArea.history())
          .updateTimePhrase("", "", TextGenPosixTime(1970, 1, 1));

    nextDaySentence << PeriodPhraseFactory::create(
        "next_day", p.theVariable, p.theForecastTime, p.theWeatherPeriod, p.theWeatherArea);
  }

  if (!nextDaySentence.empty())
    p.theTomorrowTautologyFlag = true;

  sentence << construct_final_sentence(
      p, temperatureSentence, nextDaySentence, intervalStart, intervalEnd);

  return sentence;
}

Sentence day1_sentence(t36hparams& theParameters)
{
  Sentence sentence;

  theParameters.theForecastPeriodId = DAY1_PERIOD;

  Sentence temperatureSentence;
  Sentence theSpecifiedDay;

  int intervalStart = static_cast<int>(kFloatMissing);
  int intervalEnd = static_cast<int>(kFloatMissing);

  temperatureSentence << temperature_sentence(theParameters, intervalStart, intervalEnd);

  if (theParameters.theDayAndNightSeparationFlag &&
      theParameters.theFullPeriod.localEndTime().DifferenceInHours(
          theParameters.theFullPeriod.localStartTime()) > 24)
  {
    theSpecifiedDay << PeriodPhraseFactory::create("today",
                                                   theParameters.theVariable,
                                                   theParameters.theForecastTime,
                                                   theParameters.theWeatherPeriod,
                                                   theParameters.theWeatherArea);
  }

  sentence << construct_final_sentence(
      theParameters, temperatureSentence, theSpecifiedDay, intervalStart, intervalEnd);

  theParameters.theTomorrowTautologyFlag = false;

  return sentence;
}

namespace
{
// IDs for the three area types: FULL_AREA, INLAND_AREA, COASTAL_AREA
struct AreaWeatherIds
{
  forecast_area_id areaId;
  int maxDay1;
  int meanDay1;
  int minNight;
  int maxNight;
  int meanNight;
  int minDay1Morning;
  int maxDay1Morning;
  int meanDay1Morning;
  int minDay1Afternoon;
  int maxDay1Afternoon;
  int meanDay1Afternoon;
  int minDay2Morning;
  int maxDay2Morning;
  int meanDay2Morning;
  int minDay2Afternoon;
  int maxDay2Afternoon;
  int meanDay2Afternoon;
};

Sentence construct_day1_sentence_for_area(t36hparams& p, const AreaWeatherIds& ids)
{
  Sentence sentence;
  p.theForecastAreaId = ids.areaId;
  if (p.theSeasonId == SUMMER_SEASON &&
      p.theWeatherResults[ids.maxDay1Morning]->value() != kFloatMissing &&
      p.theWeatherResults[ids.maxDay1Afternoon]->value() != kFloatMissing &&
      p.theWeatherResults[ids.maxDay1Morning]->value() >
          p.theWeatherResults[ids.maxDay1Afternoon]->value() + MORNING_AFTERNOON_SEPARATION_LIMIT)
  {
    p.theSubPeriodId = DAY1_MORNING_PERIOD;
    p.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
    p.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
    p.theMinimum = p.theWeatherResults[ids.minDay1Morning]->value();
    p.theMaximum = p.theWeatherResults[ids.maxDay1Morning]->value();
    p.theMean = p.theWeatherResults[ids.meanDay1Morning]->value();
    sentence << day1_sentence(p);
    sentence << Delimiter(COMMA_PUNCTUATION_MARK);
    p.theForecastAreaId = ids.areaId;
    p.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
    p.theMinimum = p.theWeatherResults[ids.minDay1Afternoon]->value();
    p.theMaximum = p.theWeatherResults[ids.maxDay1Afternoon]->value();
    p.theMean = p.theWeatherResults[ids.meanDay1Afternoon]->value();
    sentence << day1_sentence(p);
  }
  else
  {
    p.theSubPeriodId = UNDEFINED_SUBPERIOD;
    p.theMinimum = p.theWeatherResults[ids.minDay1Afternoon]->value();
    p.theMaximum = p.theWeatherResults[ids.maxDay1Afternoon]->value();
    p.theMean = p.theWeatherResults[ids.meanDay1Afternoon]->value();
    sentence << day1_sentence(p);
  }
  return sentence;
}

Sentence construct_night_sentence_for_area(t36hparams& p, const AreaWeatherIds& ids)
{
  Sentence sentence;
  p.theForecastAreaId = ids.areaId;
  p.theMaxTemperatureDay1 = p.theWeatherResults[ids.maxDay1]->value();
  p.theMeanTemperatureDay1 = p.theWeatherResults[ids.meanDay1]->value();
  p.theMinimum = p.theWeatherResults[ids.minNight]->value();
  p.theMaximum = p.theWeatherResults[ids.maxNight]->value();
  p.theMean = p.theWeatherResults[ids.meanNight]->value();
  if (p.theDayAndNightSeparationFlag || p.theForecastPeriod == NIGHT_PERIOD)
    sentence << night_sentence(p);
  construct_optional_frost_story(p);
  return sentence;
}

Sentence construct_day2_sentence_for_area(t36hparams& p, const AreaWeatherIds& ids)
{
  Sentence sentence;
  p.theForecastAreaId = ids.areaId;
  if (p.theSeasonId == SUMMER_SEASON &&
      p.theWeatherResults[ids.maxDay2Morning]->value() != kFloatMissing &&
      p.theWeatherResults[ids.maxDay2Afternoon]->value() != kFloatMissing &&
      p.theWeatherResults[ids.maxDay2Morning]->value() >
          p.theWeatherResults[ids.maxDay2Afternoon]->value() + MORNING_AFTERNOON_SEPARATION_LIMIT)
  {
    p.theSubPeriodId = DAY2_MORNING_PERIOD;
    p.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
    p.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
    p.theMeanTemperatureDay1 = p.theWeatherResults[ids.meanDay1]->value();
    p.theMinimum = p.theWeatherResults[ids.minDay2Morning]->value();
    p.theMaximum = p.theWeatherResults[ids.maxDay2Morning]->value();
    p.theMean = p.theWeatherResults[ids.meanDay2Morning]->value();
    sentence << day2_sentence(p);
    sentence << Delimiter(COMMA_PUNCTUATION_MARK);
    p.theForecastAreaId = ids.areaId;
    p.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
    p.theMeanTemperatureDay1 = p.theWeatherResults[ids.meanDay1]->value();
    p.theMinimum = p.theWeatherResults[ids.minDay2Afternoon]->value();
    p.theMaximum = p.theWeatherResults[ids.maxDay2Afternoon]->value();
    p.theMean = p.theWeatherResults[ids.meanDay2Afternoon]->value();
    sentence << day2_sentence(p);
  }
  else
  {
    p.theSubPeriodId = UNDEFINED_SUBPERIOD;
    p.theMeanTemperatureDay1 = p.theWeatherResults[ids.meanDay1]->value();
    p.theMinimum = p.theWeatherResults[ids.minDay2Afternoon]->value();
    p.theMaximum = p.theWeatherResults[ids.maxDay2Afternoon]->value();
    p.theMean = p.theWeatherResults[ids.meanDay2Afternoon]->value();
    sentence << day2_sentence(p);
  }
  return sentence;
}

AreaWeatherIds make_area_ids_full()
{
  return {FULL_AREA,
          AREA_MAX_DAY1,
          AREA_MEAN_DAY1,
          AREA_MIN_NIGHT,
          AREA_MAX_NIGHT,
          AREA_MEAN_NIGHT,
          AREA_MIN_DAY1_MORNING,
          AREA_MAX_DAY1_MORNING,
          AREA_MEAN_DAY1_MORNING,
          AREA_MIN_DAY1_AFTERNOON,
          AREA_MAX_DAY1_AFTERNOON,
          AREA_MEAN_DAY1_AFTERNOON,
          AREA_MIN_DAY2_MORNING,
          AREA_MAX_DAY2_MORNING,
          AREA_MEAN_DAY2_MORNING,
          AREA_MIN_DAY2_AFTERNOON,
          AREA_MAX_DAY2_AFTERNOON,
          AREA_MEAN_DAY2_AFTERNOON};
}

AreaWeatherIds make_area_ids_inland()
{
  return {INLAND_AREA,
          INLAND_MAX_DAY1,
          INLAND_MEAN_DAY1,
          INLAND_MIN_NIGHT,
          INLAND_MAX_NIGHT,
          INLAND_MEAN_NIGHT,
          INLAND_MIN_DAY1_MORNING,
          INLAND_MAX_DAY1_MORNING,
          INLAND_MEAN_DAY1_MORNING,
          INLAND_MIN_DAY1_AFTERNOON,
          INLAND_MAX_DAY1_AFTERNOON,
          INLAND_MEAN_DAY1_AFTERNOON,
          INLAND_MIN_DAY2_MORNING,
          INLAND_MAX_DAY2_MORNING,
          INLAND_MEAN_DAY2_MORNING,
          INLAND_MIN_DAY2_AFTERNOON,
          INLAND_MAX_DAY2_AFTERNOON,
          INLAND_MEAN_DAY2_AFTERNOON};
}

AreaWeatherIds make_area_ids_coastal()
{
  return {COASTAL_AREA,
          COAST_MAX_DAY1,
          COAST_MEAN_DAY1,
          COAST_MIN_NIGHT,
          COAST_MAX_NIGHT,
          COAST_MEAN_NIGHT,
          COAST_MIN_DAY1_MORNING,
          COAST_MAX_DAY1_MORNING,
          COAST_MEAN_DAY1_MORNING,
          COAST_MIN_DAY1_AFTERNOON,
          COAST_MAX_DAY1_AFTERNOON,
          COAST_MEAN_DAY1_AFTERNOON,
          COAST_MIN_DAY2_MORNING,
          COAST_MAX_DAY2_MORNING,
          COAST_MEAN_DAY2_MORNING,
          COAST_MIN_DAY2_AFTERNOON,
          COAST_MAX_DAY2_AFTERNOON,
          COAST_MEAN_DAY2_AFTERNOON};
}
}  // namespace

Sentence construct_sentence(t36hparams& theParameters)
{
  auto& p = theParameters;

  AreaWeatherIds ids;
  if (p.theForecastAreaId == FULL_AREA)
    ids = make_area_ids_full();
  else if (p.theForecastAreaId == INLAND_AREA)
    ids = make_area_ids_inland();
  else if (p.theForecastAreaId == COASTAL_AREA)
    ids = make_area_ids_coastal();
  else
    return Sentence{};

  if (p.theForecastPeriodId == DAY1_PERIOD)
    return construct_day1_sentence_for_area(p, ids);
  if (p.theForecastPeriodId == NIGHT_PERIOD)
    return construct_night_sentence_for_area(p, ids);
  if (p.theForecastPeriodId == DAY2_PERIOD)
    return construct_day2_sentence_for_area(p, ids);
  return Sentence{};
}

void get_interval_details(t36hparams& theParameters,
                          float minValue,
                          float meanValue,
                          float maxValue,
                          bool& intervalUsed,
                          int& intervalStart,
                          int& intervalEnd)
{
  int theMinimumInt = static_cast<int>(round(minValue));
  int theMeanInt = static_cast<int>(round(meanValue));
  int theMaximumInt = static_cast<int>(round(maxValue));

  clamp_temperature(theParameters.theVariable,
                    theParameters.theSeasonId == WINTER_SEASON,
                    theParameters.theForecastPeriodId != NIGHT_PERIOD,
                    theMinimumInt < theMaximumInt ? theMinimumInt : theMaximumInt,
                    theMaximumInt > theMinimumInt ? theMaximumInt : theMinimumInt);

  TemperatureStoryTools::temperature_sentence2(theMinimumInt,
                                               theMeanInt,
                                               theMaximumInt,
                                               theParameters.theMinInterval,
                                               theParameters.theZeroIntervalFlag,
                                               intervalUsed,
                                               intervalStart,
                                               intervalEnd,
                                               theParameters.theRangeSeparator,
                                               true);
}

namespace
{
processing_order determine_processing_order(const t36hparams& p)
{
  if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD &&
      p.theForecastPeriod & DAY2_PERIOD)
    return DAY1_DAY2_NIGHT;
  if (p.theForecastPeriod & NIGHT_PERIOD && p.theForecastPeriod & DAY2_PERIOD)
    return NIGHT_DAY2;
  if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
    return DAY1_NIGHT;
  if (p.theForecastPeriod == DAY1_PERIOD)
    return DAY1;
  if (p.theForecastPeriod == NIGHT_PERIOD)
    return NIGHT;
  return UNDEFINED_PROCESSING_ORDER;
}

bool should_separate_coast_inland(t36hparams& p,
                                  int inlandMinId,
                                  int inlandMeanId,
                                  int inlandMaxId,
                                  int coastMinId,
                                  int coastMeanId,
                                  int coastMaxId,
                                  int limit)
{
  if (p.theWeatherResults[inlandMeanId]->value() == kFloatMissing ||
      p.theWeatherResults[coastMeanId]->value() == kFloatMissing)
    return false;
  if (p.theSeasonId == WINTER_SEASON &&
      p.theWeatherResults[coastMeanId]->value() <= COASTAL_AREA_IGNORE_LIMIT)
    return false;

  bool intervalUsedInland = false;
  bool intervalUsedCoast = false;
  int intervalStartInland = 0;
  int intervalEndInland = 0;
  int intervalStartCoast = 0;
  int intervalEndCoast = 0;

  get_interval_details(p,
                       p.theWeatherResults[inlandMinId]->value(),
                       p.theWeatherResults[inlandMeanId]->value(),
                       p.theWeatherResults[inlandMaxId]->value(),
                       intervalUsedInland,
                       intervalStartInland,
                       intervalEndInland);
  get_interval_details(p,
                       p.theWeatherResults[coastMinId]->value(),
                       p.theWeatherResults[coastMeanId]->value(),
                       p.theWeatherResults[coastMaxId]->value(),
                       intervalUsedCoast,
                       intervalStartCoast,
                       intervalEndCoast);

  float temp_diff =
      abs(p.theWeatherResults[inlandMeanId]->value() - p.theWeatherResults[coastMeanId]->value());
  if (temp_diff < limit || p.theCoastalAndInlandTogetherFlag)
    return false;

  if (intervalUsedInland && intervalUsedCoast)
    return (abs(round(intervalStartCoast - intervalStartInland)) +
            abs(round(intervalEndCoast - intervalEndInland))) >= limit;
  return true;
}

void push_period_with_sep(vector<int>& v, bool separate, int inland, int coastal, int full)
{
  if (separate)
  {
    v.push_back(inland);
    v.push_back(DELIMITER_COMMA);
    v.push_back(coastal);
  }
  else
  {
    v.push_back(full);
  }
}

vector<int> build_period_areas(processing_order order, bool sepDay1, bool sepDay2, bool sepNight)
{
  vector<int> v;
  if (order == DAY1_DAY2_NIGHT)
  {
    push_period_with_sep(v, sepDay1, DAY1_INLAND, DAY1_COASTAL, DAY1_FULL);
    v.push_back(DELIMITER_DOT);
    push_period_with_sep(v, sepDay2, DAY2_INLAND, DAY2_COASTAL, DAY2_FULL);
    v.push_back(DELIMITER_DOT);
    push_period_with_sep(v, sepNight, NIGHT_INLAND, NIGHT_COASTAL, NIGHT_FULL);
  }
  else if (order == DAY1_NIGHT)
  {
    push_period_with_sep(v, sepDay1, DAY1_INLAND, DAY1_COASTAL, DAY1_FULL);
    v.push_back(DELIMITER_DOT);
    push_period_with_sep(v, sepNight, NIGHT_INLAND, NIGHT_COASTAL, NIGHT_FULL);
  }
  else if (order == NIGHT_DAY2)
  {
    push_period_with_sep(v, sepNight, NIGHT_INLAND, NIGHT_COASTAL, NIGHT_FULL);
    v.push_back(DELIMITER_DOT);
    push_period_with_sep(v, sepDay2, DAY2_INLAND, DAY2_COASTAL, DAY2_FULL);
  }
  else if (order == DAY1)
  {
    push_period_with_sep(v, sepDay1, DAY1_INLAND, DAY1_COASTAL, DAY1_FULL);
  }
  else if (order == NIGHT)
  {
    push_period_with_sep(v, sepNight, NIGHT_INLAND, NIGHT_COASTAL, NIGHT_FULL);
  }
  return v;
}

void resolve_area_id_for_period(t36hparams& p,
                                int periodArea,
                                bool separate,
                                int inlandCode,
                                int coastalCode,
                                forecast_area_id& area_id,
                                unsigned short& forecastAreaRef)
{
  if (p.theForecastArea & INLAND_AREA && p.theForecastArea & COASTAL_AREA)
  {
    if (separate)
    {
      area_id = (periodArea == inlandCode) ? INLAND_AREA : COASTAL_AREA;
      forecastAreaRef |= COASTAL_AREA;
      forecastAreaRef |= INLAND_AREA;
    }
    else
    {
      area_id = FULL_AREA;
      forecastAreaRef |= FULL_AREA;
    }
  }
  else if (p.theForecastArea & INLAND_AREA)
  {
    area_id = INLAND_AREA;
    forecastAreaRef |= INLAND_AREA;
  }
  else if (p.theForecastArea & COASTAL_AREA)
  {
    area_id = COASTAL_AREA;
    forecastAreaRef |= COASTAL_AREA;
  }
  else if (p.theForecastArea & FULL_AREA)
  {
    area_id = FULL_AREA;
    forecastAreaRef |= FULL_AREA;
  }
}

void handle_dot_delimiter(t36hparams& p,
                          Paragraph& paragraph,
                          processing_order processingOrder,
                          const vector<int>& periodAreas,
                          unsigned int i,
                          bool sep_day1,
                          bool sep_day2,
                          bool sep_night)
{
  p.theUseLongPhrase = true;
  if (p.theSentenceUnderConstruction.empty())
    return;

  if (processingOrder == DAY1_DAY2_NIGHT && i > 0 && periodAreas[i - 1] == DAY1_FULL &&
      periodAreas[i + 1] == DAY2_FULL && !p.morningAndAfternoonSeparated(DAY1_PERIOD) &&
      !p.morningAndAfternoonSeparated(DAY2_PERIOD))
  {
    p.theAddCommaDelimiterFlag = true;
    return;
  }

  bool use_comma =
      ((processingOrder == DAY1_NIGHT && periodAreas[i - 1] == DAY1_FULL && !sep_day1) ||
       (processingOrder == NIGHT_DAY2 && periodAreas[i - 1] == NIGHT_FULL && !sep_day2)) &&
      !sep_night;
  if (use_comma)
  {
    p.theAddCommaDelimiterFlag = true;
    p.theUseLongPhrase = false;
  }
  else
  {
    paragraph << p.theSentenceUnderConstruction;
    if (!p.theOptionalFrostParagraph.empty())
    {
      paragraph << p.theOptionalFrostParagraph;
      p.theOptionalFrostParagraph.clear();
    }
    p.theSentenceUnderConstruction.clear();
  }
}

void process_period_area_item(t36hparams& p,
                              const vector<int>& periodAreas,
                              unsigned int i,
                              bool sep_day1,
                              bool sep_day2,
                              bool sep_night)
{
  int periodArea = periodAreas[i];
  forecast_period_id period_id(NO_PERIOD);
  forecast_area_id area_id(NO_AREA);

  if (periodArea == DAY1_INLAND || periodArea == DAY1_COASTAL || periodArea == DAY1_FULL)
  {
    period_id = DAY1_PERIOD;
    p.theWeatherPeriod = p.theGenerator.period(1);
    resolve_area_id_for_period(
        p, periodArea, sep_day1, DAY1_INLAND, DAY1_COASTAL, area_id, p.theForecastAreaDay1);
  }
  else if (periodArea == NIGHT_INLAND || periodArea == NIGHT_COASTAL || periodArea == NIGHT_FULL)
  {
    period_id = NIGHT_PERIOD;
    p.theWeatherPeriod = p.theGenerator.period(p.theForecastPeriod & DAY1_PERIOD ? 2 : 1);
    resolve_area_id_for_period(
        p, periodArea, sep_night, NIGHT_INLAND, NIGHT_COASTAL, area_id, p.theForecastAreaNight);
  }
  else if (periodArea == DAY2_INLAND || periodArea == DAY2_COASTAL || periodArea == DAY2_FULL)
  {
    period_id = DAY2_PERIOD;
    int periodNum =
        (p.theForecastPeriod & DAY1_PERIOD) ? 3 : (p.theForecastPeriod & NIGHT_PERIOD ? 2 : 1);
    p.theWeatherPeriod = p.theGenerator.period(periodNum);
    resolve_area_id_for_period(
        p, periodArea, sep_day2, DAY2_INLAND, DAY2_COASTAL, area_id, p.theForecastAreaDay2);
  }

  p.theForecastAreaId = area_id;
  p.theForecastPeriodId = period_id;
  p.theDayAndNightSeparationFlag = separate_day_and_night(p, p.theForecastAreaId);
  construct_sentence(p);
  p.theAddCommaDelimiterFlag = false;
}
}  // namespace

Paragraph temperature_max36hours_sentence(t36hparams& theParameters)
{
  auto& p = theParameters;

  Paragraph paragraph;

  if (p.theForecastArea == NO_AREA)
    return paragraph;

  const int temperature_limit_coast_inland =
      optional_int(p.theVariable + "::temperature_limit_coast_inland", 3);

  processing_order processingOrder = determine_processing_order(p);

  bool sep_day1 = should_separate_coast_inland(p,
                                               INLAND_MIN_DAY1,
                                               INLAND_MEAN_DAY1,
                                               INLAND_MAX_DAY1,
                                               COAST_MIN_DAY1,
                                               COAST_MEAN_DAY1,
                                               COAST_MAX_DAY1,
                                               temperature_limit_coast_inland);
  bool sep_day2 = should_separate_coast_inland(p,
                                               INLAND_MIN_DAY2,
                                               INLAND_MEAN_DAY2,
                                               INLAND_MAX_DAY2,
                                               COAST_MIN_DAY2,
                                               COAST_MEAN_DAY2,
                                               COAST_MAX_DAY2,
                                               temperature_limit_coast_inland);
  bool sep_night = should_separate_coast_inland(p,
                                                INLAND_MIN_NIGHT,
                                                INLAND_MEAN_NIGHT,
                                                INLAND_MAX_NIGHT,
                                                COAST_MIN_NIGHT,
                                                COAST_MEAN_NIGHT,
                                                COAST_MAX_NIGHT,
                                                temperature_limit_coast_inland);

  vector<int> periodAreas = build_period_areas(processingOrder, sep_day1, sep_day2, sep_night);

  p.theUseLongPhrase = true;
  for (unsigned int i = 0; i < periodAreas.size(); i++)
  {
    int periodArea = periodAreas[i];

    if (periodArea == DELIMITER_DOT)
    {
      handle_dot_delimiter(
          p, paragraph, processingOrder, periodAreas, i, sep_day1, sep_day2, sep_night);
      continue;
    }
    if (periodArea == DELIMITER_COMMA)
    {
      if (!p.theSentenceUnderConstruction.empty())
        p.theAddCommaDelimiterFlag = true;
      continue;
    }

    process_period_area_item(p, periodAreas, i, sep_day1, sep_day2, sep_night);
  }  // for-loop

  if (!p.theSentenceUnderConstruction.empty())
    paragraph << p.theSentenceUnderConstruction;

  if (!p.theOptionalFrostParagraph.empty())
  {
    paragraph << p.theOptionalFrostParagraph;
    p.theOptionalFrostParagraph.clear();
  }

  return paragraph;
}

bool valid_value_period_check(float value, unsigned short& forecast_period, unsigned short mask)
{
  bool retval = (value != kFloatMissing);

  // clear the bit
  if (!retval)
    forecast_period &= (~mask);

  return retval;
}

namespace
{
void log_period_description(MessageLogger& theLog, const NightAndDayPeriodGenerator& gen)
{
  theLog << "period contains ";
  if (gen.isday(1))
  {
    if (gen.size() > 2)
      theLog << "today, night and tomorrow\n";
    else if (gen.size() == 2)
      theLog << "today and night\n";
    else
      theLog << "today\n";
  }
  else
  {
    theLog << (gen.size() == 1 ? "one night\n" : "night and tomorrow\n");
  }
}

unsigned short determine_forecast_period(const NightAndDayPeriodGenerator& gen)
{
  unsigned short fp = 0x0;
  if (gen.isday(1))
  {
    fp |= DAY1_PERIOD;
    fp |= (gen.size() > 1 ? NIGHT_PERIOD : 0x0);
    fp |= (gen.size() > 2 ? DAY2_PERIOD : 0x0);
  }
  else
  {
    fp |= NIGHT_PERIOD;
    fp |= (gen.size() > 1 ? DAY2_PERIOD : 0x0);
  }
  return fp;
}

unsigned short calculate_results_for_period(MessageLogger& theLog,
                                            const std::string& itsVar,
                                            const TextGen::AnalysisSources& itsSources,
                                            const TextGen::WeatherArea& itsArea,
                                            const WeatherPeriod& period,
                                            forecast_period_id periodId,
                                            forecast_season_id forecast_season,
                                            unsigned short forecast_area,
                                            unsigned short& forecast_period,
                                            weather_result_container_type& weatherResults,
                                            int coastMinId,
                                            int inlandMinId,
                                            int areaMinId,
                                            unsigned short periodMask)
{
  if (forecast_area & INLAND_AREA)
  {
    calculate_results(theLog,
                      itsVar,
                      itsSources,
                      itsArea,
                      period,
                      periodId,
                      forecast_season,
                      INLAND_AREA,
                      weatherResults);
    valid_value_period_check(weatherResults[inlandMinId]->value(), forecast_period, periodMask);
  }
  if (forecast_area & COASTAL_AREA && (forecast_period & periodMask))
  {
    calculate_results(theLog,
                      itsVar,
                      itsSources,
                      itsArea,
                      period,
                      periodId,
                      forecast_season,
                      COASTAL_AREA,
                      weatherResults);
    valid_value_period_check(weatherResults[coastMinId]->value(), forecast_period, periodMask);
  }
  if (forecast_area & FULL_AREA && (forecast_period & periodMask))
  {
    calculate_results(theLog,
                      itsVar,
                      itsSources,
                      itsArea,
                      period,
                      periodId,
                      forecast_season,
                      FULL_AREA,
                      weatherResults);
    valid_value_period_check(weatherResults[areaMinId]->value(), forecast_period, periodMask);
  }
  forecast_area |= (weatherResults[coastMinId]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
  forecast_area |= (weatherResults[inlandMinId]->value() != kFloatMissing ? INLAND_AREA : 0x0);
  forecast_area |= (weatherResults[areaMinId]->value() != kFloatMissing ? FULL_AREA : 0x0);
  return forecast_area;
}

void calculate_day2_after_night(MessageLogger& theLog,
                                const std::string& itsVar,
                                const TextGen::AnalysisSources& itsSources,
                                const TextGen::WeatherArea& itsArea,
                                const WeatherPeriod& period,
                                forecast_season_id forecast_season,
                                unsigned short forecast_area,
                                unsigned short& forecast_period,
                                weather_result_container_type& weatherResults)
{
  if (forecast_area & INLAND_AREA)
  {
    calculate_results(theLog,
                      itsVar,
                      itsSources,
                      itsArea,
                      period,
                      DAY2_PERIOD,
                      forecast_season,
                      INLAND_AREA,
                      weatherResults);
    valid_value_period_check(
        weatherResults[INLAND_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
  }
  if (forecast_area & COASTAL_AREA && (forecast_period & DAY2_PERIOD))
  {
    calculate_results(theLog,
                      itsVar,
                      itsSources,
                      itsArea,
                      period,
                      DAY2_PERIOD,
                      forecast_season,
                      COASTAL_AREA,
                      weatherResults);
    valid_value_period_check(weatherResults[COAST_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
  }
  if (forecast_area & INLAND_AREA && forecast_area & COASTAL_AREA &&
      (forecast_period & DAY2_PERIOD))
    calculate_results(theLog,
                      itsVar,
                      itsSources,
                      itsArea,
                      period,
                      DAY2_PERIOD,
                      forecast_season,
                      FULL_AREA,
                      weatherResults);
}

// Calculate all weather results when the first period is DAY1.
// Updates forecast_area and forecast_period in-place, period is set to last calculated period.
unsigned short calculate_day1_first_results(MessageLogger& theLog,
                                            const std::string& itsVar,
                                            const TextGen::AnalysisSources& itsSources,
                                            const TextGen::WeatherArea& itsArea,
                                            NightAndDayPeriodGenerator& generator,
                                            forecast_season_id forecast_season,
                                            unsigned short& forecast_period,
                                            weather_result_container_type& weatherResults,
                                            WeatherPeriod& period)
{
  log_start_time_and_end_time(theLog, "Day1: ", period);
  calculate_results(theLog,
                    itsVar,
                    itsSources,
                    itsArea,
                    period,
                    DAY1_PERIOD,
                    forecast_season,
                    INLAND_AREA,
                    weatherResults);
  calculate_results(theLog,
                    itsVar,
                    itsSources,
                    itsArea,
                    period,
                    DAY1_PERIOD,
                    forecast_season,
                    COASTAL_AREA,
                    weatherResults);
  calculate_results(theLog,
                    itsVar,
                    itsSources,
                    itsArea,
                    period,
                    DAY1_PERIOD,
                    forecast_season,
                    FULL_AREA,
                    weatherResults);
  valid_value_period_check(weatherResults[AREA_MIN_DAY1]->value(), forecast_period, DAY1_PERIOD);

  unsigned short forecast_area = 0x0;
  forecast_area |= (weatherResults[COAST_MIN_DAY1]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
  forecast_area |= (weatherResults[INLAND_MIN_DAY1]->value() != kFloatMissing ? INLAND_AREA : 0x0);
  forecast_area |= (weatherResults[AREA_MIN_DAY1]->value() != kFloatMissing ? FULL_AREA : 0x0);

  if (forecast_area == NO_AREA)
  {
    theLog << "Something wrong, NO Coastal area NOR Inland area is included!\n";
    return forecast_area;
  }

  if (forecast_period & NIGHT_PERIOD)
  {
    period = generator.period(2);
    log_start_time_and_end_time(theLog, "Night: ", period);
    forecast_area = calculate_results_for_period(theLog,
                                                 itsVar,
                                                 itsSources,
                                                 itsArea,
                                                 period,
                                                 NIGHT_PERIOD,
                                                 forecast_season,
                                                 forecast_area,
                                                 forecast_period,
                                                 weatherResults,
                                                 COAST_MIN_NIGHT,
                                                 INLAND_MIN_NIGHT,
                                                 AREA_MIN_NIGHT,
                                                 NIGHT_PERIOD);
  }
  if (forecast_period & DAY2_PERIOD)
  {
    period = generator.period(3);
    log_start_time_and_end_time(theLog, "Day2: ", period);
    forecast_area = calculate_results_for_period(theLog,
                                                 itsVar,
                                                 itsSources,
                                                 itsArea,
                                                 period,
                                                 DAY2_PERIOD,
                                                 forecast_season,
                                                 forecast_area,
                                                 forecast_period,
                                                 weatherResults,
                                                 COAST_MIN_DAY2,
                                                 INLAND_MIN_DAY2,
                                                 AREA_MIN_DAY2,
                                                 DAY2_PERIOD);
  }
  return forecast_area;
}

// Calculate all weather results when the first period is NIGHT.
// Updates forecast_area and forecast_period in-place, period is set to last calculated period.
unsigned short calculate_night_first_results(MessageLogger& theLog,
                                             const std::string& itsVar,
                                             const TextGen::AnalysisSources& itsSources,
                                             const TextGen::WeatherArea& itsArea,
                                             NightAndDayPeriodGenerator& generator,
                                             forecast_season_id forecast_season,
                                             unsigned short& forecast_period,
                                             weather_result_container_type& weatherResults,
                                             WeatherPeriod& period)
{
  log_start_time_and_end_time(theLog, "Night: ", period);
  calculate_results(theLog,
                    itsVar,
                    itsSources,
                    itsArea,
                    period,
                    NIGHT_PERIOD,
                    forecast_season,
                    INLAND_AREA,
                    weatherResults);
  calculate_results(theLog,
                    itsVar,
                    itsSources,
                    itsArea,
                    period,
                    NIGHT_PERIOD,
                    forecast_season,
                    COASTAL_AREA,
                    weatherResults);
  calculate_results(theLog,
                    itsVar,
                    itsSources,
                    itsArea,
                    period,
                    NIGHT_PERIOD,
                    forecast_season,
                    FULL_AREA,
                    weatherResults);

  unsigned short forecast_area = 0x0;
  forecast_area |= (weatherResults[COAST_MIN_NIGHT]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
  forecast_area |= (weatherResults[INLAND_MIN_NIGHT]->value() != kFloatMissing ? INLAND_AREA : 0x0);
  forecast_area |= (weatherResults[AREA_MIN_NIGHT]->value() != kFloatMissing ? FULL_AREA : 0x0);

  if (forecast_area == NO_AREA)
  {
    valid_value_period_check(kFloatMissing, forecast_period, NIGHT_PERIOD);
    theLog << "Something wrong, NO Coastal area NOR Inland area is included! \n";
    return forecast_area;
  }

  if (forecast_period & DAY2_PERIOD)
  {
    period = generator.period(2);
    log_start_time_and_end_time(theLog, "Day2: ", period);
    calculate_day2_after_night(theLog,
                               itsVar,
                               itsSources,
                               itsArea,
                               period,
                               forecast_season,
                               forecast_area,
                               forecast_period,
                               weatherResults);
  }
  return forecast_area;
}
}  // namespace

Paragraph max36hours(const TextGen::WeatherArea& itsArea,
                     const TextGen::WeatherPeriod& itsPeriod,
                     const TextGen::AnalysisSources& itsSources,
                     const TextGenPosixTime& itsForecastTime,
                     const std::string& itsVar,
                     MessageLogger& theLog)

{
  using namespace TemperatureMax36Hours;

  Paragraph paragraph;

  log_start_time_and_end_time(theLog, "Whole period: ", itsPeriod);

  const TextGenPosixTime& periodStartTime = itsPeriod.localStartTime();
  const TextGenPosixTime& periodEndTime = itsPeriod.localEndTime();

  // Period generator
  NightAndDayPeriodGenerator generator00(itsPeriod, itsVar);

  if (generator00.size() == 0)
  {
    theLog << "No weather periods available!\n";
    theLog << paragraph;
    return paragraph;
  }

  log_period_description(theLog, generator00);

  WeatherPeriod fullPeriod(periodStartTime, periodEndTime);
  NightAndDayPeriodGenerator generator(fullPeriod, itsVar);

  unsigned short forecast_area = 0x0;
  forecast_season_id forecast_season =
      get_forecast_season(itsArea, itsSources, generator.period(1), itsVar);
  unsigned short forecast_period = determine_forecast_period(generator);

  weather_result_container_type weatherResults;
  for (int i = AREA_MIN_DAY1; i < UNDEFINED_WEATHER_RESULT_ID; i++)
    weatherResults.insert(make_pair(i, new WeatherResult(kFloatMissing, 0)));

  WeatherPeriod period = generator.period(1);

  if (forecast_period & DAY1_PERIOD)
    forecast_area = calculate_day1_first_results(theLog,
                                                 itsVar,
                                                 itsSources,
                                                 itsArea,
                                                 generator,
                                                 forecast_season,
                                                 forecast_period,
                                                 weatherResults,
                                                 period);
  else
    forecast_area = calculate_night_first_results(theLog,
                                                  itsVar,
                                                  itsSources,
                                                  itsArea,
                                                  generator,
                                                  forecast_season,
                                                  forecast_period,
                                                  weatherResults,
                                                  period);

  const string range_separator = optional_string(itsVar + "::rangeseparator", "...");
  const int mininterval = optional_int(itsVar + "::mininterval", 2);
  const bool interval_zero = optional_bool(itsVar + "::always_interval_zero", false);

  t36hparams parameters(itsVar,
                        theLog,
                        generator,
                        forecast_season,
                        forecast_area,
                        forecast_period,
                        itsForecastTime,
                        fullPeriod,
                        period,
                        itsArea,
                        itsSources,
                        weatherResults);

  parameters.theFullDayFlag = Settings::optional_bool(itsVar + "::specify_part_of_the_day", true);

  float coastalPercentage = get_area_percentage(
      itsVar + "::fake::area_percentage", itsArea, WeatherArea::Coast, itsSources, itsPeriod);

  float separate_coastal_area_percentage = Settings::optional_double(
      itsVar + "::separate_coastal_area_percentage", SEPARATE_COASTAL_AREA_PERCENTAGE);

  parameters.theCoastalAndInlandTogetherFlag =
      coastalPercentage > 0 && coastalPercentage < separate_coastal_area_percentage;

  if (coastalPercentage > 0)
  {
    if (parameters.theCoastalAndInlandTogetherFlag)
      theLog << "Coastal proportion: " << coastalPercentage << '\n';
    else
      theLog << "Coastal proportion: " << coastalPercentage
             << " (the areas will be separated if mean temperature is different enough)\n";
  }

  parameters.theRangeSeparator = range_separator;
  parameters.theMinInterval = mininterval;
  parameters.theZeroIntervalFlag = interval_zero;

  if (itsArea.isMarine() || itsArea.isIsland())
    parameters.theCoastalAndInlandTogetherFlag = true;

  if (forecast_area != NO_AREA)
    paragraph << temperature_max36hours_sentence(parameters);

  log_weather_results(parameters);

  for (int i = AREA_MIN_DAY1; i < UNDEFINED_WEATHER_RESULT_ID; i++)
  {
    delete weatherResults[i];
  }

  theLog << paragraph;

  return paragraph;
}

}  // namespace TemperatureMax36Hours

// ----------------------------------------------------------------------
/*!
 * \brief Generate story on temperature for the day and night
 *
 * \return The generated paragraph
 */
// ----------------------------------------------------------------------

Paragraph TemperatureStory::max36hours() const
{
  MessageLogger log("TemperatureStory::max36h");

  using namespace TemperatureMax36Hours;

  Paragraph paragraph;

  std::string areaName;

  if (itsArea.isNamed())
  {
    areaName = itsArea.name();
  }

  WeatherArea areaOne(itsArea);
  WeatherArea areaTwo(itsArea);
  split_method splitMethod =
      check_area_splitting(itsVar, itsArea, itsPeriod, itsSources, areaOne, areaTwo, log);

  if (NO_SPLITTING != splitMethod)
  {
    Paragraph paragraphAreaOne;
    Paragraph paragraphAreaTwo;

    log << areaName + (splitMethod == HORIZONTAL ? " - southern part" : " - western part") << '\n';

    paragraphAreaOne << TemperatureMax36Hours::max36hours(
        areaOne, itsPeriod, itsSources, itsForecastTime, itsVar, log);

    log << areaName + (splitMethod == HORIZONTAL ? " - northern part" : " - eastern part") << '\n';

    paragraphAreaTwo << TemperatureMax36Hours::max36hours(
        areaTwo, itsPeriod, itsSources, itsForecastTime, itsVar, log);

    paragraph << paragraphAreaOne << paragraphAreaTwo;
  }
  else
  {
    log << areaName << '\n';

    paragraph << TemperatureMax36Hours::max36hours(
        itsArea, itsPeriod, itsSources, itsForecastTime, itsVar, log);
  }

  return paragraph;
}

}  // namespace TextGen

// ======================================================================
