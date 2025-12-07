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
  //	  float theMeanCalc = theMean;
  float theMaximumCalc = theMaximum;

  proximity_id retval = NO_PROXIMITY;
  bool bBelowZeroDegrees = (theMaximum < 0);

  if (bBelowZeroDegrees)
  {
    theMaximumCalc = abs(theMinimum);
    //		  theMeanCalc = abs(theMean);
    theMinimumCalc = abs(theMaximum);
  }

  int iMin = static_cast<int>(floor(theMinimumCalc));
  int iMax = static_cast<int>(floor(theMaximumCalc));
  int iDivFiveMin = iMin / 5;
  int iDivFiveMax = iMax / 5;
  int iMinModFive = iMin % 5;
  int iMaxModFive = iMax % 5;
  enum proximity_type
  {
    BELOW,
    BETWEEN,
    ABOVE,
    NONE
  };
  enum proximity_type theCase = NONE;
  int theNumberDivisibleByFive = iMin;
  while (theNumberDivisibleByFive % 5 != 0)
    theNumberDivisibleByFive += 1;

  if ((theNumberDivisibleByFive < theMaximumCalc && theMinimumCalc < theNumberDivisibleByFive &&
       theNumberDivisibleByFive - theMinimumCalc < TIENOILLA_ASTETTA_LOW_TEMP_LIMIT &&
       theMaximumCalc - theNumberDivisibleByFive < TIENOILLA_ASTETTA_LOW_TEMP_LIMIT) ||
      (theMaximum == theMinimum && theMaximum == theNumberDivisibleByFive))
  {
    theCase = BETWEEN;
  }
  else
  {
    if ((iMinModFive == 0 || iMinModFive == 1 || iMinModFive == 2) &&
        (iMaxModFive == 0 || iMaxModFive == 1 || iMaxModFive == 2) && iDivFiveMin == iDivFiveMax)
    {
      theCase = ABOVE;
      theNumberDivisibleByFive -= 5;
    }
    else if ((iMinModFive == 3 || iMinModFive == 4) && (iMaxModFive == 3 || iMaxModFive == 4) &&
             iDivFiveMin == iDivFiveMax)
    {
      theCase = BELOW;
    }
  }

  if (theCase == BETWEEN)
  {
    float theMinDiff = theNumberDivisibleByFive - theMinimumCalc;
    float theMaxDiff = theMaximumCalc - theNumberDivisibleByFive;

    if (theMinDiff < TUNTUMASSA_ASTETTA_ALI && theMaxDiff < TUNTUMASSA_ASTETTA_YLI &&
        theMean < theNumberDivisibleByFive && theMinDiff > theMaxDiff)  // tuntumassa
    {
      retval = TUNTUMASSA_ASTETTA;
    }
    else if (theMinDiff < LAHELLA_ASTETTA_ALI && theMaxDiff < LAHELLA_ASTETTA_YLI &&
             theMean < theNumberDivisibleByFive && theMinDiff > theMaxDiff)  // tuntumassa
    {
      retval = LAHELLA_ASTETTA;
    }
    else
    {
      float noinAstettaLimit = (theMaximumCalc * (bBelowZeroDegrees ? -1 : +1)) < -10.0
                                   ? NOIN_ASTETTA_LOW_TEMP_LIMIT
                                   : NOIN_ASTETTA_HIGH_TEMP_LIMIT;
      float tienoillaAstettaLimit = (theMaximumCalc * (bBelowZeroDegrees ? -1 : +1)) < -10.0
                                        ? TIENOILLA_ASTETTA_LOW_TEMP_LIMIT
                                        : TIENOILLA_ASTETTA_HIGH_TEMP_LIMIT;

      if (theMinDiff < noinAstettaLimit && theMaxDiff < noinAstettaLimit)
      {
        retval = NOIN_ASTETTA;
      }
      else if (theMinDiff < tienoillaAstettaLimit && theMaxDiff < tienoillaAstettaLimit)
      {
        retval = TIENOILLA_ASTETTA;
      }
    }
    if (retval != NO_PROXIMITY)
      theProximityNumber = theNumberDivisibleByFive;
  }
  else if (theCase == ABOVE)
  {
    // minimum and maximum are above the number
    // that is divisible by 5
    float theMinDiff = theMinimumCalc - theNumberDivisibleByFive;
    float theMaxDiff = theMaximumCalc - theNumberDivisibleByFive;

    float vahanYliAstettaLimit = (theMaximumCalc * (bBelowZeroDegrees ? -1 : +1)) < -10.0
                                     ? VAHAN_YLI_ASTETTA_LOW_TEMP_LIMIT
                                     : VAHAN_YLI_ASTETTA_HIGH_TEMP_LIMIT;

    if (theMinDiff < vahanYliAstettaLimit && theMaxDiff < vahanYliAstettaLimit)
      retval = VAHAN_YLI_ASTETTA;

    if (retval != NO_PROXIMITY)
      theProximityNumber = theNumberDivisibleByFive;
  }
  else if (theCase == BELOW)
  {
    // minimum and maximum are below the number
    // that is divisible by 5
    float theMinDiff = theNumberDivisibleByFive - theMinimumCalc;
    float theMaxDiff = theNumberDivisibleByFive - theMaximumCalc;

    if (theMinDiff < TUNTUMASSA_ASTETTA_ALI && theMaxDiff < TUNTUMASSA_ASTETTA_ALI)
      retval = TUNTUMASSA_ASTETTA;
    /*
      // lahella and vajaat is the same if they are both below the number that is divisible by 5
    else if(theMinDiff < LAHELLA_ASTETTA_ALI && theMaxDiff < LAHELLA_ASTETTA_ALI)
      retval = LAHELLA_ASTETTA;
    */
    else if (theMinDiff < VAJAAT_ASTETTA_LIMIT && theMaxDiff < VAJAAT_ASTETTA_LIMIT)
      retval = VAJAAT_ASTETTA;

    if (retval != NO_PROXIMITY)
      theProximityNumber = theNumberDivisibleByFive;
  }

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
  weather_result_id min_id_full(UNDEFINED_WEATHER_RESULT_ID);
  weather_result_id max_id_full(UNDEFINED_WEATHER_RESULT_ID);
  weather_result_id mean_id_full(UNDEFINED_WEATHER_RESULT_ID);
  weather_result_id min_id_morning(UNDEFINED_WEATHER_RESULT_ID);
  weather_result_id max_id_morning(UNDEFINED_WEATHER_RESULT_ID);
  weather_result_id mean_id_morning(UNDEFINED_WEATHER_RESULT_ID);
  weather_result_id min_id_afternoon(UNDEFINED_WEATHER_RESULT_ID);
  weather_result_id max_id_afternoon(UNDEFINED_WEATHER_RESULT_ID);
  weather_result_id mean_id_afternoon(UNDEFINED_WEATHER_RESULT_ID);

  std::string fakeVarFull("::fake");
  std::string fakeVarMorning("::fake");
  std::string fakeVarAfternoon("::fake");

  if (thePeriodId == DAY1_PERIOD)
  {
    min_id_full =
        (theAreaId == INLAND_AREA ? INLAND_MIN_DAY1
                                  : (theAreaId == COASTAL_AREA ? COAST_MIN_DAY1 : AREA_MIN_DAY1));
    max_id_full =
        (theAreaId == INLAND_AREA ? INLAND_MAX_DAY1
                                  : (theAreaId == COASTAL_AREA ? COAST_MAX_DAY1 : AREA_MAX_DAY1));
    mean_id_full =
        (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY1
                                  : (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY1 : AREA_MEAN_DAY1));

    min_id_morning =
        (theAreaId == INLAND_AREA
             ? INLAND_MIN_DAY1_MORNING
             : (theAreaId == COASTAL_AREA ? COAST_MIN_DAY1_MORNING : AREA_MIN_DAY1_MORNING));
    max_id_morning =
        (theAreaId == INLAND_AREA
             ? INLAND_MAX_DAY1_MORNING
             : (theAreaId == COASTAL_AREA ? COAST_MAX_DAY1_MORNING : AREA_MAX_DAY1_MORNING));
    mean_id_morning =
        (theAreaId == INLAND_AREA
             ? INLAND_MEAN_DAY1_MORNING
             : (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY1_MORNING : AREA_MEAN_DAY1_MORNING));
    min_id_afternoon =
        (theAreaId == INLAND_AREA
             ? INLAND_MIN_DAY1_AFTERNOON
             : (theAreaId == COASTAL_AREA ? COAST_MIN_DAY1_AFTERNOON : AREA_MIN_DAY1_AFTERNOON));
    max_id_afternoon =
        (theAreaId == INLAND_AREA
             ? INLAND_MAX_DAY1_AFTERNOON
             : (theAreaId == COASTAL_AREA ? COAST_MAX_DAY1_AFTERNOON : AREA_MAX_DAY1_AFTERNOON));
    mean_id_afternoon =
        (theAreaId == INLAND_AREA
             ? INLAND_MEAN_DAY1_AFTERNOON
             : (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY1_AFTERNOON : AREA_MEAN_DAY1_AFTERNOON));

    fakeVarFull += "::day1";
    fakeVarMorning += "::day1::morning";
    fakeVarAfternoon += "::day1::afternoon";
  }
  else if (thePeriodId == NIGHT_PERIOD)
  {
    min_id_full =
        (theAreaId == INLAND_AREA ? INLAND_MIN_NIGHT
                                  : (theAreaId == COASTAL_AREA ? COAST_MIN_NIGHT : AREA_MIN_NIGHT));
    max_id_full =
        (theAreaId == INLAND_AREA ? INLAND_MAX_NIGHT
                                  : (theAreaId == COASTAL_AREA ? COAST_MAX_NIGHT : AREA_MAX_NIGHT));
    mean_id_full = (theAreaId == INLAND_AREA
                        ? INLAND_MEAN_NIGHT
                        : (theAreaId == COASTAL_AREA ? COAST_MEAN_NIGHT : AREA_MEAN_NIGHT));
    fakeVarFull += "::night";
  }
  else if (thePeriodId == DAY2_PERIOD)
  {
    min_id_full =
        (theAreaId == INLAND_AREA ? INLAND_MIN_DAY2
                                  : (theAreaId == COASTAL_AREA ? COAST_MIN_DAY2 : AREA_MIN_DAY2));
    max_id_full =
        (theAreaId == INLAND_AREA ? INLAND_MAX_DAY2
                                  : (theAreaId == COASTAL_AREA ? COAST_MAX_DAY2 : AREA_MAX_DAY2));
    mean_id_full =
        (theAreaId == INLAND_AREA ? INLAND_MEAN_DAY2
                                  : (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY2 : AREA_MEAN_DAY2));

    min_id_morning =
        (theAreaId == INLAND_AREA
             ? INLAND_MIN_DAY2_MORNING
             : (theAreaId == COASTAL_AREA ? COAST_MIN_DAY2_MORNING : AREA_MIN_DAY2_MORNING));
    max_id_morning =
        (theAreaId == INLAND_AREA
             ? INLAND_MAX_DAY2_MORNING
             : (theAreaId == COASTAL_AREA ? COAST_MAX_DAY2_MORNING : AREA_MAX_DAY2_MORNING));
    mean_id_morning =
        (theAreaId == INLAND_AREA
             ? INLAND_MEAN_DAY2_MORNING
             : (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY2_MORNING : AREA_MEAN_DAY2_MORNING));
    min_id_afternoon =
        (theAreaId == INLAND_AREA
             ? INLAND_MIN_DAY2_AFTERNOON
             : (theAreaId == COASTAL_AREA ? COAST_MIN_DAY2_AFTERNOON : AREA_MIN_DAY2_AFTERNOON));
    max_id_afternoon =
        (theAreaId == INLAND_AREA
             ? INLAND_MAX_DAY2_AFTERNOON
             : (theAreaId == COASTAL_AREA ? COAST_MAX_DAY2_AFTERNOON : AREA_MAX_DAY2_AFTERNOON));
    mean_id_afternoon =
        (theAreaId == INLAND_AREA
             ? INLAND_MEAN_DAY2_AFTERNOON
             : (theAreaId == COASTAL_AREA ? COAST_MEAN_DAY2_AFTERNOON : AREA_MEAN_DAY2_AFTERNOON));

    fakeVarFull += "::day2";
    fakeVarMorning += "::day2::morning";
    fakeVarAfternoon += "::day2::afternoon";
  }
  else
  {
    return;
  }

  WeatherArea theActualArea = theArea;

  if (theAreaId == INLAND_AREA)
  {
    if (theArea.type() == WeatherArea::Full)
      theActualArea.type(WeatherArea::Inland);
    fakeVarFull += "::inland";
    fakeVarMorning += "::inland";
    fakeVarAfternoon += "::inland";
  }
  else if (theAreaId == COASTAL_AREA)
  {
    if (theArea.type() == WeatherArea::Full)
      theActualArea.type(WeatherArea::Coast);
    fakeVarFull += "::coast";
    fakeVarMorning += "::coast";
    fakeVarAfternoon += "::coast";
  }
  else
  {
    if (theArea.type() == WeatherArea::Full)
      theActualArea.type(WeatherArea::Full);
    fakeVarFull += "::area";
    fakeVarMorning += "::area";
    fakeVarAfternoon += "::area";
  }

  WeatherResult& minResultFull = *theWeatherResults[min_id_full];
  WeatherResult& maxResultFull = *theWeatherResults[max_id_full];
  WeatherResult& meanResultFull = *theWeatherResults[mean_id_full];
  WeatherResult& minResultMorning = *theWeatherResults[min_id_morning];
  WeatherResult& maxResultMorning = *theWeatherResults[max_id_morning];
  WeatherResult& meanResultMorning = *theWeatherResults[mean_id_morning];
  WeatherResult& minResultAfternoon = *theWeatherResults[min_id_afternoon];
  WeatherResult& maxResultAfternoon = *theWeatherResults[max_id_afternoon];
  WeatherResult& meanResultAfternoon = *theWeatherResults[mean_id_afternoon];

  if (theSeasonId == SUMMER_SEASON)
  {
    // In summertime we calculate maximum (of area maximums) at daytime and
    // minimum (of area minimums) at nighttime
    WeatherFunction theTimeFunction = (thePeriodId == NIGHT_PERIOD ? Minimum : Maximum);

    if (thePeriodId == NIGHT_PERIOD)
    {
      minResultFull = do_calculation(theVar + fakeVarFull + "::min",
                                     theSources,
                                     Minimum,
                                     theTimeFunction,
                                     theActualArea,
                                     thePeriod);

      maxResultFull = do_calculation(theVar + fakeVarFull + "::max",
                                     theSources,
                                     Maximum,
                                     theTimeFunction,
                                     theActualArea,
                                     thePeriod);

      meanResultFull = do_calculation(theVar + fakeVarFull + "::mean",
                                      theSources,
                                      Mean,
                                      theTimeFunction,
                                      theActualArea,
                                      thePeriod);

      if (theActualArea.type() == WeatherArea::Full)
        WeatherResultTools::checkMissingValue(
            "temperature_max36hours", Temperature, {minResultFull, maxResultFull, meanResultFull});
    }
    else if (thePeriodId == DAY1_PERIOD || thePeriodId == DAY2_PERIOD)
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
  }
  else
  {
    // In wintertime we calculate Mean temperature of areas Maximum, Minimum and Mean temperatures

    if (thePeriodId == NIGHT_PERIOD)
    {
      minResultFull = do_calculation(
          theVar + fakeVarFull + "::min", theSources, Minimum, Mean, theActualArea, thePeriod);

      maxResultFull = do_calculation(
          theVar + fakeVarFull + "::max", theSources, Maximum, Mean, theActualArea, thePeriod);

      meanResultFull = do_calculation(
          theVar + fakeVarFull + "::mean", theSources, Mean, Mean, theActualArea, thePeriod);

      if (theActualArea.type() == WeatherArea::Full)
        WeatherResultTools::checkMissingValue(
            "temperature_max36hours", Temperature, {minResultFull, maxResultFull, meanResultFull});
    }
    else if (thePeriodId == DAY1_PERIOD || thePeriodId == DAY2_PERIOD)
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
  }
}  // namespace TemperatureMax36Hours

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

bool separate_day_and_night(const t36hparams& theParameters, forecast_area_id theForecastAreaId)
{
  // Shorthand alias to make code more readable
  const auto& p = theParameters;

  // if only one day or one night is included ==> nothing to separate
  if (p.theForecastPeriod == DAY1_PERIOD || p.theForecastPeriod == NIGHT_PERIOD ||
      p.theForecastPeriod == DAY2_PERIOD)
    return false;

  bool separateDayAndNight = true;

  if (p.theSeasonId == WINTER_SEASON)
  {
    float dayTemperature = 0;
    float nightTemperature = 0;

    if (theForecastAreaId == FULL_AREA)
    {
      if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
      {
        if (around_zero_phrase(p.theWeatherResults[AREA_MIN_DAY1]->value(),
                               p.theWeatherResults[AREA_MEAN_DAY1]->value(),
                               p.theWeatherResults[AREA_MAX_DAY1]->value(),
                               p.theZeroIntervalFlag) ==
            around_zero_phrase(p.theWeatherResults[AREA_MIN_NIGHT]->value(),
                               p.theWeatherResults[AREA_MEAN_NIGHT]->value(),
                               p.theWeatherResults[AREA_MAX_NIGHT]->value(),
                               p.theZeroIntervalFlag))
        {
          dayTemperature = p.theWeatherResults[AREA_MEAN_DAY1]->value();
          nightTemperature = p.theWeatherResults[AREA_MEAN_NIGHT]->value();
          separateDayAndNight =
              abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
          if (!separateDayAndNight)
            separateDayAndNight =
                (p.inlandAndCoastSeparated(DAY1_PERIOD) || p.inlandAndCoastSeparated(NIGHT_PERIOD));
        }
      }
      else if (p.theForecastPeriod & DAY2_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
      {
        if (around_zero_phrase(p.theWeatherResults[AREA_MIN_DAY2]->value(),
                               p.theWeatherResults[AREA_MEAN_DAY2]->value(),
                               p.theWeatherResults[AREA_MAX_DAY2]->value(),
                               p.theZeroIntervalFlag) ==
            around_zero_phrase(p.theWeatherResults[AREA_MIN_NIGHT]->value(),
                               p.theWeatherResults[AREA_MEAN_NIGHT]->value(),
                               p.theWeatherResults[AREA_MAX_NIGHT]->value(),
                               p.theZeroIntervalFlag))
        {
          dayTemperature = p.theWeatherResults[AREA_MEAN_DAY2]->value();
          nightTemperature = p.theWeatherResults[AREA_MEAN_NIGHT]->value();
          separateDayAndNight =
              abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
          if (!separateDayAndNight)
            separateDayAndNight =
                (p.inlandAndCoastSeparated(DAY2_PERIOD) || p.inlandAndCoastSeparated(NIGHT_PERIOD));
        }
      }
    }
    else if (theForecastAreaId == INLAND_AREA)
    {
      if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
      {
        if (around_zero_phrase(p.theWeatherResults[INLAND_MIN_DAY1]->value(),
                               p.theWeatherResults[INLAND_MEAN_DAY1]->value(),
                               p.theWeatherResults[INLAND_MAX_DAY1]->value(),
                               p.theZeroIntervalFlag) ==
            around_zero_phrase(p.theWeatherResults[INLAND_MIN_NIGHT]->value(),
                               p.theWeatherResults[INLAND_MEAN_NIGHT]->value(),
                               p.theWeatherResults[INLAND_MAX_NIGHT]->value(),
                               p.theZeroIntervalFlag))
        {
          dayTemperature = p.theWeatherResults[INLAND_MEAN_DAY1]->value();
          nightTemperature = p.theWeatherResults[INLAND_MEAN_NIGHT]->value();
          separateDayAndNight =
              abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
          if (!separateDayAndNight)
            separateDayAndNight =
                (p.inlandAndCoastSeparated(DAY1_PERIOD) || p.inlandAndCoastSeparated(NIGHT_PERIOD));
        }
      }
      else if (p.theForecastPeriod & DAY2_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
      {
        if (around_zero_phrase(p.theWeatherResults[INLAND_MIN_DAY2]->value(),
                               p.theWeatherResults[INLAND_MEAN_DAY2]->value(),
                               p.theWeatherResults[INLAND_MAX_DAY2]->value(),
                               p.theZeroIntervalFlag) ==
            around_zero_phrase(p.theWeatherResults[INLAND_MIN_NIGHT]->value(),
                               p.theWeatherResults[INLAND_MEAN_NIGHT]->value(),
                               p.theWeatherResults[INLAND_MAX_NIGHT]->value(),
                               p.theZeroIntervalFlag))
        {
          dayTemperature = p.theWeatherResults[INLAND_MEAN_DAY2]->value();
          nightTemperature = p.theWeatherResults[INLAND_MEAN_NIGHT]->value();
          separateDayAndNight =
              abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
          if (!separateDayAndNight)
            separateDayAndNight =
                (p.inlandAndCoastSeparated(DAY2_PERIOD) || p.inlandAndCoastSeparated(NIGHT_PERIOD));
        }
      }

      if (!separateDayAndNight && p.theForecastArea & COASTAL_AREA)
      {
        if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
        {
          if (around_zero_phrase(p.theWeatherResults[COAST_MIN_DAY1]->value(),
                                 p.theWeatherResults[COAST_MEAN_DAY1]->value(),
                                 p.theWeatherResults[COAST_MAX_DAY1]->value(),
                                 p.theZeroIntervalFlag) ==
              around_zero_phrase(p.theWeatherResults[COAST_MIN_NIGHT]->value(),
                                 p.theWeatherResults[COAST_MEAN_NIGHT]->value(),
                                 p.theWeatherResults[COAST_MAX_NIGHT]->value(),
                                 p.theZeroIntervalFlag))
          {
            dayTemperature = p.theWeatherResults[COAST_MEAN_DAY1]->value();
            nightTemperature = p.theWeatherResults[COAST_MEAN_NIGHT]->value();
            separateDayAndNight =
                abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
            if (!separateDayAndNight)
              separateDayAndNight = (p.inlandAndCoastSeparated(DAY1_PERIOD) ||
                                     p.inlandAndCoastSeparated(NIGHT_PERIOD));
          }
        }
        else if (p.theForecastPeriod & DAY2_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
        {
          if (around_zero_phrase(p.theWeatherResults[COAST_MIN_DAY2]->value(),
                                 p.theWeatherResults[COAST_MEAN_DAY2]->value(),
                                 p.theWeatherResults[COAST_MAX_DAY2]->value(),
                                 p.theZeroIntervalFlag) ==
              around_zero_phrase(p.theWeatherResults[COAST_MIN_NIGHT]->value(),
                                 p.theWeatherResults[COAST_MEAN_NIGHT]->value(),
                                 p.theWeatherResults[COAST_MAX_NIGHT]->value(),
                                 p.theZeroIntervalFlag))
          {
            dayTemperature = p.theWeatherResults[COAST_MEAN_DAY2]->value();
            nightTemperature = p.theWeatherResults[COAST_MEAN_NIGHT]->value();
            separateDayAndNight =
                abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
            if (!separateDayAndNight)
              separateDayAndNight = (p.inlandAndCoastSeparated(DAY2_PERIOD) ||
                                     p.inlandAndCoastSeparated(NIGHT_PERIOD));
          }
        }
      }
    }
    else if (theForecastAreaId == COASTAL_AREA)
    {
      if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
      {
        if (around_zero_phrase(p.theWeatherResults[COAST_MIN_DAY1]->value(),
                               p.theWeatherResults[COAST_MEAN_DAY1]->value(),
                               p.theWeatherResults[COAST_MAX_DAY1]->value(),
                               p.theZeroIntervalFlag) ==
            around_zero_phrase(p.theWeatherResults[COAST_MIN_NIGHT]->value(),
                               p.theWeatherResults[COAST_MEAN_NIGHT]->value(),
                               p.theWeatherResults[COAST_MAX_NIGHT]->value(),
                               p.theZeroIntervalFlag))
        {
          dayTemperature = p.theWeatherResults[COAST_MEAN_DAY1]->value();
          nightTemperature = p.theWeatherResults[COAST_MEAN_NIGHT]->value();
          separateDayAndNight =
              abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
          if (!separateDayAndNight)
            separateDayAndNight =
                (p.inlandAndCoastSeparated(DAY1_PERIOD) || p.inlandAndCoastSeparated(NIGHT_PERIOD));
        }
      }
      else if (p.theForecastPeriod & DAY2_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
      {
        if (around_zero_phrase(p.theWeatherResults[COAST_MIN_DAY2]->value(),
                               p.theWeatherResults[COAST_MEAN_DAY2]->value(),
                               p.theWeatherResults[COAST_MAX_DAY2]->value(),
                               p.theZeroIntervalFlag) ==
            around_zero_phrase(p.theWeatherResults[COAST_MIN_NIGHT]->value(),
                               p.theWeatherResults[COAST_MEAN_NIGHT]->value(),
                               p.theWeatherResults[COAST_MAX_NIGHT]->value(),
                               p.theZeroIntervalFlag))
        {
          dayTemperature = p.theWeatherResults[COAST_MEAN_DAY2]->value();
          nightTemperature = p.theWeatherResults[COAST_MEAN_NIGHT]->value();
          separateDayAndNight =
              abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
          if (!separateDayAndNight)
            separateDayAndNight =
                (p.inlandAndCoastSeparated(DAY2_PERIOD) || p.inlandAndCoastSeparated(NIGHT_PERIOD));
        }
      }

      if (!separateDayAndNight && p.theForecastArea & INLAND_AREA)
      {
        if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
        {
          if (around_zero_phrase(p.theWeatherResults[INLAND_MIN_DAY1]->value(),
                                 p.theWeatherResults[INLAND_MEAN_DAY1]->value(),
                                 p.theWeatherResults[INLAND_MAX_DAY1]->value(),
                                 p.theZeroIntervalFlag) ==
              around_zero_phrase(p.theWeatherResults[INLAND_MIN_NIGHT]->value(),
                                 p.theWeatherResults[INLAND_MEAN_NIGHT]->value(),
                                 p.theWeatherResults[INLAND_MAX_NIGHT]->value(),
                                 p.theZeroIntervalFlag))
          {
            dayTemperature = p.theWeatherResults[INLAND_MEAN_DAY1]->value();
            nightTemperature = p.theWeatherResults[INLAND_MEAN_NIGHT]->value();
            separateDayAndNight =
                abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
            if (!separateDayAndNight)
              separateDayAndNight = (p.inlandAndCoastSeparated(DAY1_PERIOD) ||
                                     p.inlandAndCoastSeparated(NIGHT_PERIOD));
          }
        }
        else if (p.theForecastPeriod & DAY2_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
        {
          if (around_zero_phrase(p.theWeatherResults[INLAND_MIN_DAY2]->value(),
                                 p.theWeatherResults[INLAND_MEAN_DAY2]->value(),
                                 p.theWeatherResults[INLAND_MAX_DAY2]->value(),
                                 p.theZeroIntervalFlag) ==
              around_zero_phrase(p.theWeatherResults[INLAND_MIN_NIGHT]->value(),
                                 p.theWeatherResults[INLAND_MEAN_NIGHT]->value(),
                                 p.theWeatherResults[INLAND_MAX_NIGHT]->value(),
                                 p.theZeroIntervalFlag))
          {
            dayTemperature = p.theWeatherResults[INLAND_MEAN_DAY2]->value();
            nightTemperature = p.theWeatherResults[INLAND_MEAN_NIGHT]->value();
            separateDayAndNight =
                abs(dayTemperature - nightTemperature) >= DAY_NIGHT_SEPARATION_LIMIT;
            if (!separateDayAndNight)
              separateDayAndNight = (p.inlandAndCoastSeparated(DAY2_PERIOD) ||
                                     p.inlandAndCoastSeparated(NIGHT_PERIOD));
          }
        }
      }
    }
  }  // if(p.theSeasonId == WINTER_SEASON)

  return separateDayAndNight;
}

Sentence temperature_sentence(t36hparams& theParameters, int& intervalStart, int& intervalEnd)
{
  Sentence sentence;

  auto& p = theParameters;  // shorthand alias to make code more readable

  temperature_phrase_id phrase_id = around_zero_phrase(p);

  if (phrase_id != NO_PHRASE_ID)
  {
    p.theTemperaturePhraseId = phrase_id;

    switch (phrase_id)
    {
      case VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID:
      {
        p.theTemperaturePhraseId = VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID;
        sentence << VAHAN_PLUSSAN_PUOLELLA_PHRASE;
      }
      break;
      case NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID:
      {
        p.theTemperaturePhraseId = NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID;
        sentence << NOLLAN_TIENOILLA_PHRASE << TAI_PHRASE << VAHAN_PLUSSAN_PUOLELLA_PHRASE;
      }
      break;
      case NOLLAN_TIENOILLA_PHRASE_ID:
      {
        p.theTemperaturePhraseId = NOLLAN_TIENOILLA_PHRASE_ID;
        sentence << NOLLAN_TIENOILLA_PHRASE;
      }
      break;
      case PIKKUPAKKASTA_PHRASE_ID:
      {
        TextGenPosixTime startTime(p.theGenerator.period(1).localStartTime());
        if (SeasonTools::isSpring(startTime, p.theVariable) ||
            abs(p.theMaximum - p.theMinimum) < AROUND_ZERO_UPPER_LIMIT)
        {
          if (p.theMinimum >= -1.0)  // maximum and minimum between [-1,0]
          {
            sentence << VAHAN_NOLLAN_ALAPUOLELLA_PHRASE;
            intervalStart = 0;
            p.theTemperaturePhraseId = VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID;
          }
          else
          {
            int theMinimumInt = static_cast<int>(round(p.theMinimum));
            int theMeanInt = static_cast<int>(round(p.theMean));
            int theMaximumInt = static_cast<int>(round(p.theMaximum));
            bool intervalUsed;

            clamp_temperature(p.theVariable,
                              p.theSeasonId == WINTER_SEASON,
                              p.theForecastPeriodId != NIGHT_PERIOD,
                              theMinimumInt < theMaximumInt ? theMinimumInt : theMaximumInt,
                              theMaximumInt > theMinimumInt ? theMaximumInt : theMinimumInt);

            sentence = TemperatureStoryTools::temperature_sentence2(theMinimumInt,
                                                                    theMeanInt,
                                                                    theMaximumInt,
                                                                    p.theMinInterval,
                                                                    p.theZeroIntervalFlag,
                                                                    intervalUsed,
                                                                    intervalStart,
                                                                    intervalEnd,
                                                                    p.theRangeSeparator,
                                                                    false);

            if (intervalUsed)
              p.theTemperaturePhraseId = LAMPOTILA_VALILLA_PHRASE_ID;
            else
              p.theTemperaturePhraseId = NOIN_ASTETTA_PHRASE_ID;
          }
        }
        else
        {
          p.theTemperaturePhraseId = PIKKUPAKKASTA_PHRASE_ID;
          sentence << PIKKUPAKKASTA_PHRASE;
        }
      }
      break;
      default:
        break;
    }
  }
  else
  {
    int theProximityNumber;

    proximity_id proxim_id =
        get_proximity_id(p.theMinimum, p.theMean, p.theMaximum, theProximityNumber);

    if (proxim_id != NO_PROXIMITY)
    {
      char proximityNumberBuff[32];
      char tempBuff[128];

      sprintf(tempBuff,
              "Minimum: %.02f;Mean: %.02f;Maximum: %.02f",
              p.theMinimum,
              p.theMean,
              p.theMaximum);

      sprintf(proximityNumberBuff, "%i", theProximityNumber);

      switch (proxim_id)
      {
        case NOIN_ASTETTA:
        {
          sentence << NOIN_PHRASE << Integer(theProximityNumber)
                   << *UnitFactory::create_unit(DegreesCelsius, theProximityNumber);

          p.theTemperaturePhraseId = NOIN_ASTETTA_PHRASE_ID;
          intervalStart = theProximityNumber;

          p.theLog << "PROXIMITY: Noin " << proximityNumberBuff << " astetta :: " << tempBuff
                   << '\n';
        }
        break;
        case TIENOILLA_ASTETTA:
        {
          sentence << TIENOILLA_PHRASE << Integer(theProximityNumber);
          p.theTemperaturePhraseId = TIENOILLA_ASTETTA_PHRASE_ID;
          intervalStart = theProximityNumber;

          p.theLog << "PROXIMITY: " << proximityNumberBuff << " asteen tienoilla :: " << tempBuff
                   << '\n';
        }
        break;
        case LAHELLA_ASTETTA:
        {
          sentence << LAHELLA_PHRASE << Integer(theProximityNumber);
          p.theTemperaturePhraseId = LAHELLA_ASTETTA_PHRASE_ID;
          intervalStart = theProximityNumber;

          p.theLog << "PROXIMITY: Lahella " << proximityNumberBuff << " astetta :: " << tempBuff
                   << '\n';
        }
        break;
        case TUNTUMASSA_ASTETTA:
        {
          sentence << TUNTUMASSA_PHRASE << Integer(theProximityNumber);
          p.theTemperaturePhraseId = TUNTUMASSA_ASTETTA_PHRASE_ID;
          intervalStart = theProximityNumber;

          p.theLog << "PROXIMITY: " << proximityNumberBuff << " asteen tuntumassa :: " << tempBuff
                   << '\n';
        }
        break;
        case VAJAAT_ASTETTA:
        {
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
        }
        break;
        case VAHAN_YLI_ASTETTA:
        {
          sentence << VAHAN_PHRASE << YLI_PHRASE << Integer(theProximityNumber)
                   << *UnitFactory::create_unit(DegreesCelsius, theProximityNumber);
          p.theTemperaturePhraseId = VAHAN_YLI_ASTETTA_PHRASE_ID;
          intervalStart = theProximityNumber;

          p.theLog << "PROXIMITY: Vahan yli " << proximityNumberBuff << " astetta :: " << tempBuff
                   << '\n';
          if (p.theMaximum < 0)
            p.theUseFrostExistsPhrase = true;
        }
        break;
        case NO_PROXIMITY:
          p.theLog << "NO PROXIMITY: " << tempBuff << '\n';
          break;
      }
    }
    else
    {
      int theMinimumInt = static_cast<int>(round(p.theMinimum));
      int theMeanInt = static_cast<int>(round(p.theMean));
      int theMaximumInt = static_cast<int>(round(p.theMaximum));
      bool intervalUsed;

      clamp_temperature(p.theVariable,
                        p.theSeasonId == WINTER_SEASON,
                        p.theForecastPeriodId != NIGHT_PERIOD,
                        theMinimumInt < theMaximumInt ? theMinimumInt : theMaximumInt,
                        theMaximumInt > theMinimumInt ? theMaximumInt : theMinimumInt);

      sentence = TemperatureStoryTools::temperature_sentence2(theMinimumInt,
                                                              theMeanInt,
                                                              theMaximumInt,
                                                              p.theMinInterval,
                                                              p.theZeroIntervalFlag,
                                                              intervalUsed,
                                                              intervalStart,
                                                              intervalEnd,
                                                              p.theRangeSeparator,
                                                              true);

      if (intervalUsed)
        p.theTemperaturePhraseId = LAMPOTILA_VALILLA_PHRASE_ID;
      else
        p.theTemperaturePhraseId = NOIN_ASTETTA_PHRASE_ID;
    }
  }

  return sentence;
}

void pakkasta_on(t36hparams& theParameters,
                 Sentence& theDayPhasePhrase,
                 Sentence& theTemperaturePhrase)
{
  auto& p = theParameters;  // shorthand alias

  if (p.theUseFrostExistsPhrase)
  {
    // if only one period exists, dont use word 'yolla'/'paivalla'
    if (p.numberOfPeriods() > 1 && p.theDayAndNightSeparationFlag)
    {
      if (p.theFullDayFlag)
      {
        if (p.theForecastPeriodId == NIGHT_PERIOD)
          theDayPhasePhrase << YOLLA_PHRASE;
        else
          theDayPhasePhrase << PAIVALLA_PHRASE;
      }
    }
    theTemperaturePhrase << PAKKASTA_WORD;
    p.theUseFrostExistsPhrase = false;
  }

  if (theTemperaturePhrase.empty())
  {
    if (p.theFullDayFlag && p.numberOfPeriods() > 1)
    {
      if (p.theForecastPeriodId == NIGHT_PERIOD)
      {
        if (p.theSeasonId == SUMMER_SEASON)
        {
          if (p.theUseLongPhrase)
            theTemperaturePhrase << YON_ALIN_LAMPOTILA_LONG_PHRASE;
          else
            theTemperaturePhrase << YON_ALIN_LAMPOTILA_SHORT_PHRASE;
        }
        else
        {
          theTemperaturePhrase << YOLAMPOTILA_PHRASE;
        }
      }
      else
      {
        if (p.theSeasonId == SUMMER_SEASON)
        {
          if (p.theUseLongPhrase)
            theTemperaturePhrase << PAIVAN_YLIN_LAMPOTILA_LONG_PHRASE;
          else
            theTemperaturePhrase << PAIVAN_YLIN_LAMPOTILA_SHORT_PHRASE;
        }
        else
        {
          theTemperaturePhrase << PAIVALAMPOTILA_PHRASE;
        }
      }
    }
    else
    {
      theTemperaturePhrase << LAMPOTILA_WORD;
    }
  }
}

void temperature_phrase(t36hparams& theParameters,
                        Sentence& theDayPhasePhrase,
                        Sentence& theTemperaturePhrase,
                        Sentence& theAreaPhrase)
{
  Sentence sentence;

  auto& p = theParameters;

  temperature_phrase_id phrase_id(p.theTemperaturePhraseId);
  forecast_season_id season_id(p.theSeasonId);

  if (p.theForecastPeriodId == NIGHT_PERIOD)
  {
    if (p.theUseFrostExistsPhrase)
    {
      pakkasta_on(p, theDayPhasePhrase, theTemperaturePhrase);
    }
    else
    {
      switch (phrase_id)
      {
        case PIKKUPAKKASTA_PHRASE_ID:
        case HIEMAN_LAUHEMPAA_PHRASE_ID:
        case HIEMAN_KYLMEMPAA_PHRASE_ID:
        case HIEMAN_HEIKOMPAA_PHRASE_ID:
        case HIEMAN_KIREAMPAA_PHRASE_ID:
        case LAMPOTILA_NOUSEE_PHRASE_ID:
        {
          theDayPhasePhrase << YOLLA_PHRASE;
          break;
        }
        default:
        {
          if (season_id == SUMMER_SEASON)
          {
            if (p.theUseLongPhrase)
              theTemperaturePhrase << YON_ALIN_LAMPOTILA_LONG_PHRASE;
            else
              theTemperaturePhrase << YON_ALIN_LAMPOTILA_SHORT_PHRASE;
          }
          else
          {
            if (p.numberOfPeriods() > 1)
              theTemperaturePhrase << YOLAMPOTILA_PHRASE;
            else
              theTemperaturePhrase << LAMPOTILA_WORD;
          }
        }
        break;
      }
    }
  }
  else  // day period
  {
    forecast_subperiod_id subperiod_id(p.theSubPeriodId);

    if (subperiod_id == UNDEFINED_SUBPERIOD)
    {
      if (p.theUseFrostExistsPhrase)
      {
        pakkasta_on(p, theDayPhasePhrase, theTemperaturePhrase);
      }
      else
      {
        if (phrase_id == PIKKUPAKKASTA_PHRASE_ID)
        {
          if (p.theFullDayFlag)
          {
            if (p.theForecastPeriodId == NIGHT_PERIOD)
              theDayPhasePhrase << YOLLA_PHRASE;
            else
            {
              if (p.theForecastPeriodId == DAY2_PERIOD)
                theDayPhasePhrase << HUOMENNA_WORD;
              else
                theDayPhasePhrase << PAIVALLA_PHRASE;
            }
          }
        }
        else
        {
          if (p.theFullDayFlag)
          {
            if (season_id == SUMMER_SEASON)
            {
              if (p.theUseLongPhrase)
                theTemperaturePhrase << PAIVAN_YLIN_LAMPOTILA_LONG_PHRASE;
              else
                theTemperaturePhrase << PAIVAN_YLIN_LAMPOTILA_SHORT_PHRASE;
            }
            else
            {
              if (p.theDayAndNightSeparationFlag)
                theTemperaturePhrase << PAIVALAMPOTILA_PHRASE;
              else
                theTemperaturePhrase << LAMPOTILA_WORD;
            }
          }
          else
          {
            theTemperaturePhrase << LAMPOTILA_WORD;
          }
        }
      }
    }
    else
    {
      if (season_id == SUMMER_SEASON)
      {
        if (subperiod_id == DAY1_MORNING_PERIOD || subperiod_id == DAY2_MORNING_PERIOD)
        {
          theDayPhasePhrase << AAMULLA_PHRASE;
          if (phrase_id != PIKKUPAKKASTA_PHRASE_ID)
          {
            theTemperaturePhrase << LAMPOTILA_WORD;
          }
        }
        else
        {
          theDayPhasePhrase << ILTAPAIVALLA_PHRASE;
#if 0          
          if (phrase_id == PIKKUPAKKASTA_PHRASE_ID)
          {
            // theTemperaturePhrase << ON_WORD;
          }
          else
          {
            // no tautology
          }
#endif
        }
      }
    }
  }

  if (p.inlandAndCoastSeparated())
  {
    if (p.theForecastAreaId == COASTAL_AREA)
    {
      theAreaPhrase << RANNIKOLLA_PHRASE;
    }
    else if (p.theForecastAreaId == INLAND_AREA)
    {
      theAreaPhrase << SISAMAASSA_PHRASE;
    }
  }
}

Sentence temperature_phrase(t36hparams& theParameters)
{
  Sentence sentence;

  auto& p = theParameters;  // shorthand alias

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
      // bugfix 13.4.2011: "Yon alin lampotila on hieman kylmempaa" => "Yolla on hieman kylmempaa"
      if (p.theTemperaturePhraseId == PIKKUPAKKASTA_PHRASE_ID ||
          p.theTemperaturePhraseId == HIEMAN_HEIKOMPAA_PHRASE_ID ||
          p.theTemperaturePhraseId == HIEMAN_LAUHEMPAA_PHRASE_ID ||
          p.theTemperaturePhraseId == HIEMAN_KIREAMPAA_PHRASE_ID ||
          p.theTemperaturePhraseId == HIEMAN_KYLMEMPAA_PHRASE_ID)
      {
        dayPhaseString = YOLLA_PHRASE;

        if (p.theTemperaturePhraseId == HIEMAN_KIREAMPAA_PHRASE_ID ||
            p.theTemperaturePhraseId == HIEMAN_HEIKOMPAA_PHRASE_ID)
          theDayPhasePhrase << YOLLA_PHRASE << PAKKANEN_WORD << ON_WORD;
        else
          theDayPhasePhrase << YOLLA_PHRASE << ON_WORD;
      }
      else if (p.theTemperaturePhraseId == LAMPOTILA_NOUSEE_PHRASE_ID)
      {
        dayPhaseString = YOLLA_PHRASE;
        theDayPhasePhrase << YOLLA_PHRASE;
      }
      else
      {
        if (p.theUseLongPhrase)
          theDayPhasePhrase << YON_ALIN_LAMPOTILA_LONG_PHRASE << ON_WORD;
        else
          theDayPhasePhrase << YON_ALIN_LAMPOTILA_SHORT_PHRASE;

        p.theNightPeriodTautologyFlag = true;
      }
      p.theDayPeriodTautologyFlag = false;
    }
    else if (p.theForecastPeriodId != NIGHT_PERIOD)
    {
      p.theNightPeriodTautologyFlag = false;

      if ((p.theSubPeriodId == UNDEFINED_SUBPERIOD && useDayTemperaturePhrase &&
           !p.theDayPeriodTautologyFlag) ||
          (p.theForecastPeriodId == DAY2_PERIOD &&
           (p.theForecastAreaId == INLAND_AREA || p.theForecastAreaId == FULL_AREA) &&
           ((p.inlandAndCoastSeparated(DAY2_PERIOD) && p.theForecastAreaId == INLAND_AREA) ||
            p.morningAndAfternoonSeparated(DAY1_PERIOD))))
      {
        if (p.theTemperaturePhraseId == PIKKUPAKKASTA_PHRASE_ID)
        {
          theDayPhasePhrase << ON_WORD;
          plainIsVerbUsed = true;
        }
        else
        {
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
        }
      }
      else
      {
        p.theDayPeriodTautologyFlag = false;

        if (p.theSubPeriodId == DAY1_MORNING_PERIOD || p.theSubPeriodId == DAY2_MORNING_PERIOD)
        {
          if (p.theTemperaturePhraseId == PIKKUPAKKASTA_PHRASE_ID)
          {
            dayPhaseString = AAMULLA_PHRASE;
            theDayPhasePhrase << AAMULLA_PHRASE << ON_WORD;
          }
          else
          {
            theDayPhasePhrase << LAMPOTILA_WORD << ON_WORD << AAMULLA_PHRASE;
          }
        }
        else if (p.theSubPeriodId == DAY1_AFTERNOON_PERIOD ||
                 p.theSubPeriodId == DAY2_AFTERNOON_PERIOD)
        {
          dayPhaseString = ILTAPAIVALLA_PHRASE;
          if (p.theTemperaturePhraseId == PIKKUPAKKASTA_PHRASE_ID)
          {
            theDayPhasePhrase << ILTAPAIVALLA_PHRASE << ON_WORD;
          }
          else
          {
            theDayPhasePhrase << ILTAPAIVALLA_PHRASE;
          }
        }
      }
    }
  }
  else
  {
    if (p.theForecastPeriodId == NIGHT_PERIOD && !p.theNightPeriodTautologyFlag)
    {
      if (p.theTemperaturePhraseId == PIKKUPAKKASTA_PHRASE_ID ||
          p.theTemperaturePhraseId == HIEMAN_HEIKOMPAA_PHRASE_ID ||
          p.theTemperaturePhraseId == HIEMAN_LAUHEMPAA_PHRASE_ID ||
          p.theTemperaturePhraseId == HIEMAN_KIREAMPAA_PHRASE_ID ||
          p.theTemperaturePhraseId == HIEMAN_KYLMEMPAA_PHRASE_ID)
      {
        dayPhaseString = YOLLA_PHRASE;
        if (p.theTemperaturePhraseId == HIEMAN_KIREAMPAA_PHRASE_ID ||
            p.theTemperaturePhraseId == HIEMAN_HEIKOMPAA_PHRASE_ID)
          theDayPhasePhrase << YOLLA_PHRASE << PAKKANEN_WORD << ON_WORD;
        else
          theDayPhasePhrase << YOLLA_PHRASE << ON_WORD;
      }
      else if (p.theTemperaturePhraseId == LAMPOTILA_NOUSEE_PHRASE_ID)
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
    else if (p.theForecastPeriodId != NIGHT_PERIOD)
    {
      p.theNightPeriodTautologyFlag = false;

      if (p.theSubPeriodId == UNDEFINED_SUBPERIOD &&
          (!p.theDayPeriodTautologyFlag ||
           (p.inlandAndCoastSeparated(DAY2_PERIOD) && p.theForecastPeriodId == DAY2_PERIOD &&
            p.theForecastAreaId == INLAND_AREA)))
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
    if (p.theUseFrostExistsPhrase)
    {
      // if only one period exists, dont use word 'yolla'/'paivalla'
      if (p.numberOfPeriods() > 1 && p.theDayAndNightSeparationFlag)
      {
        if (p.theForecastPeriodId == NIGHT_PERIOD)
          sentence << YOLLA_PHRASE;
        else
          sentence << PAIVALLA_PHRASE;
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
    }
    else
    {
      sentence << theDayPhasePhrase;
    }
  }
  else
  {
    if (p.theUseFrostExistsPhrase)
    {
      // if only one period exists, dont use word 'yolla'/'paivalla'
      if (p.numberOfPeriods() > 1 && p.theDayAndNightSeparationFlag)
      {
        if (p.theForecastPeriodId == NIGHT_PERIOD)
          sentence << YOLLA_PHRASE;
        else
          sentence << PAIVALLA_PHRASE;
      }

      if (!p.theFrostExistsTautologyFlag)
      {
        sentence << PAKKASTA_WORD << ON_WORD;
      }
      else
      {
        sentence << LAMPOTILA_WORD << ON_WORD;
      }

      p.theUseFrostExistsPhrase = false;
    }
    else
    {
      // p.theFrostExistsTautologyFlag = false;
    }
    sentence << theAreaPhrase;
  }

  return sentence;
}

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
    bool tienoilla(phrase_id == TIENOILLA_ASTETTA_PHRASE_ID);

    sentence << Integer(degrees);
    string degrees_string;
    if (abs(degrees) % 10 == 1 && abs(degrees) != 11)
    {
      if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID || phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
        degrees_string =
            (tienoilla ? "asteen(tienoilla (mod 10=1))" : "asteen(tuntumassa (mod 10=1))");
      else if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
        degrees_string = "astetta(noin (mod 10=1))";
    }
    else
    {
      if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID || phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
        degrees_string = (tienoilla ? "asteen(tienoilla)" : "asteen(tuntumassa)");
      else
      {
        if (abs(degrees) > 4)
          degrees_string = "astetta(noin n)";
        else
          degrees_string = string("astetta(noin " + std::to_string(abs(degrees)) + ")");
      }
    }
    sentence << degrees_string;
  }
  else if (opt == "none")
    ;
  else
    throw Fmi::Exception(BCP, "Unknown format " + opt + " in variable " + var);

  return sentence;
}

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

  if (theParameters.theWeatherArea.type() == WeatherArea::Northern)
    theAreaPhrase << ALUEEN_POHJOISOSASSA_PHRASE;
  else if (theParameters.theWeatherArea.type() == WeatherArea::Southern)
    theAreaPhrase << ALUEEN_ETELAOSASSA_PHRASE;
  else if (theParameters.theWeatherArea.type() == WeatherArea::Eastern)
    theAreaPhrase << ALUEEN_ITAOSASSA_PHRASE;
  else if (theParameters.theWeatherArea.type() == WeatherArea::Western)
    theAreaPhrase << ALUEEN_LANSIOSASSA_PHRASE;

  temperature_phrase_id phrase_id(theParameters.theTemperaturePhraseId);

  temperature_phrase(theParameters, theDayPhasePhrase, theTemperaturePhrase, theAreaPhrase);

  bool pakkastaOn = (as_string(theTemperaturePhrase).compare(0, 8, "Pakkasta") == 0);

  if (theDayPhasePhrase.empty())
  {
    if (!daySentence.empty())
    {
      theDayPhasePhrase << daySentence;
    }
    else
    {
      theDayPhasePhrase << EMPTY_STRING;
      dayPhasePhraseEmpty = true;
    }
  }

  if (theTemperaturePhrase.empty())
  {
    theTemperaturePhrase << EMPTY_STRING;
  }

  if (theAreaPhrase.empty())
  {
    theAreaPhrase << EMPTY_STRING;
    areaPhraseEmpty = true;
  }

  if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID || phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
  {
    degreesSentence << intervalStart;
  }
  else
  {
    if (intervalStart == 0)
      degreesSentence << NOLLA_WORD << *UnitFactory::create_unit(DegreesCelsius, intervalStart);
    else
      degreesSentence << intervalStart << *UnitFactory::create_unit(DegreesCelsius, intervalStart);
  }

  switch (phrase_id)
  {
    case PIKKUPAKKASTA_PHRASE_ID:
    {
      if (dayPhasePhraseEmpty && areaPhraseEmpty)
        sentence << PIKKUPAKKASTA_PHRASE;
      else if (!dayPhasePhraseEmpty && areaPhraseEmpty)
        sentence << HUOMENNA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE << theDayPhasePhrase;
      else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
        sentence << RANNIKOLLA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE << theAreaPhrase;
      else
        sentence << HUOMENNA_RANNIKOLLA_ON_PIKKUPAKKASTA_COMPOSITE_PHRASE << theDayPhasePhrase
                 << theAreaPhrase;
    }
    break;

    case HIEMAN_LAUHEMPAA_PHRASE_ID:
    case HIEMAN_KYLMEMPAA_PHRASE_ID:
    {
      if (!dayPhasePhraseEmpty && areaPhraseEmpty)
        sentence << HUOMENNA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE << theDayPhasePhrase;
      else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
        sentence << SISAMAASSA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE << theAreaPhrase;
      else
        sentence << HUOMENNA_SISAMAASSA_ON_HIEMAN_LAUHEMPAA_COMPOSITE_PHRASE << theDayPhasePhrase
                 << theAreaPhrase;

      if (phrase_id == HIEMAN_LAUHEMPAA_PHRASE_ID)
        sentence << HIEMAN_LAUHEMPAA_PHRASE;
      else
        sentence << HIEMAN_KYLMEMPAA_PHRASE;
    }
    break;

    case HIEMAN_HEIKOMPAA_PHRASE_ID:
    case HIEMAN_KIREAMPAA_PHRASE_ID:
    {
      if (!dayPhasePhraseEmpty && areaPhraseEmpty)
        sentence << HUOMENNA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE << theDayPhasePhrase;
      else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
        sentence << SISAMAASSA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE << theAreaPhrase;
      else
        sentence << HUOMENNA_SISAMAASSA_PAKKANEN_ON_HIEMAN_KIREAMPAA_COMPOSITE_PHRASE
                 << theDayPhasePhrase << theAreaPhrase;

      if (phrase_id == HIEMAN_HEIKOMPAA_PHRASE_ID)
        sentence << HIEMAN_HEIKOMPAA_PHRASE;
      else
        sentence << HIEMAN_KIREAMPAA_PHRASE;
    }
    break;

    case HIEMAN_KORKEAMPI_PHRASE_ID:
    case HIEMAN_ALEMPI_PHRASE_ID:
    {
      if (useDay2Phrase)
      {
        if (dayPhasePhraseEmpty && areaPhraseEmpty)
        {
          if (theParameters.theUseLongPhrase)
            sentence << LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE;
          else
            sentence << LAMPOTILA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE;
          sentence << theTemperaturePhrase;
        }
        else if (!dayPhasePhraseEmpty && areaPhraseEmpty)
          sentence << SISAMAASSA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase;
        else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
          sentence << HUOMENNA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theAreaPhrase;
        else
          sentence << HUOMENNA_SISAMAASSA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase
                   << theAreaPhrase;
      }
      else
      {
        if (dayPhasePhraseEmpty && areaPhraseEmpty)
        {
          if (theParameters.theUseLongPhrase)
            sentence << LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE;
          else
            sentence << LAMPOTILA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE;
          sentence << theTemperaturePhrase;
        }
        else if (!dayPhasePhraseEmpty && areaPhraseEmpty)
        {
          sentence << HUOMENNA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase
                   << theTemperaturePhrase;
        }
        else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
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

      if (phrase_id == HIEMAN_KORKEAMPI_PHRASE_ID)
        sentence << HIEMAN_KORKEAMPI_PHRASE;
      else
        sentence << HIEMAN_ALEMPI_PHRASE;
    }
    break;

    case LAMPOTILA_NOUSEE_PHRASE_ID:
    {
      sentence << LAMPOTILA_NOUSEE_COMPOSITE_PHRASE << theDayPhasePhrase;
    }
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
        if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
          sentence << RANNIKOLLA_NOIN_ASTETTA_COMPOSITE_PHRASE;
        else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
          sentence << RANNIKOLLA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
        else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
          sentence << RANNIKOLLA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
        else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
          sentence << RANNIKOLLA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
        else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
          sentence << RANNIKOLLA_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
        else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
          sentence << RANNIKOLLA_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
        else if (phrase_id == VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID)
          sentence << RANNIKOLLA_VAHAN_NOLLAN_ALAPUOLELLA_PHRASE;

        if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID || phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID ||
            phrase_id == NOIN_ASTETTA_PHRASE_ID)
        {
          sentence << tienoilla_and_tuntumassa_astetta(intervalStart, phrase_id);
        }
        else
        {
          sentence << degreesSentence;
        }
      }
      else
      {
        if (pakkastaOn)  // theParameters.theUseFrostExistsPhrase)
        {
          // no minus sign for freezing temperatures
          int pakkanenDegrees = abs(intervalStart);
          Sentence pakkanenDegreesSentence;
          pakkanenDegreesSentence << pakkanenDegrees
                                  << *UnitFactory::create_unit(DegreesCelsius, pakkanenDegrees);

          if (dayPhasePhraseEmpty && areaPhraseEmpty)
          {
            if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
              sentence << PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
              sentence << PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
              sentence << PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
              sentence << PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
              sentence << PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
              sentence << PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;

            if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
            {
              sentence << tienoilla_and_tuntumassa_astetta(pakkanenDegrees, phrase_id);
            }
            else
            {
              sentence << pakkanenDegreesSentence;
            }
          }
          else if (!dayPhasePhraseEmpty && areaPhraseEmpty)
          {
            if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;

            if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
            {
              sentence << theDayPhasePhrase
                       << tienoilla_and_tuntumassa_astetta(pakkanenDegrees, phrase_id);
            }
            else
            {
              sentence << theDayPhasePhrase << pakkanenDegreesSentence;
            }
          }
          else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
          {
            if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;

            if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
            {
              sentence << theAreaPhrase
                       << tienoilla_and_tuntumassa_astetta(pakkanenDegrees, phrase_id);
            }
            else
            {
              sentence << theAreaPhrase << pakkanenDegreesSentence;
            }
          }
          else
          {
            if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_PAKKASTA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_PAKKANEN_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_PAKKANEN_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_PAKKASTA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_PAKKASTA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_PAKKANEN_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;

            if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
            {
              sentence << theDayPhasePhrase << theAreaPhrase
                       << tienoilla_and_tuntumassa_astetta(pakkanenDegrees, phrase_id);
            }
            else
            {
              sentence << theDayPhasePhrase << theAreaPhrase << pakkanenDegreesSentence;
            }
          }
        }  // if (theParameters.theUseFrostExistsPhrase)
        else
        {
          if (dayPhasePhraseEmpty && areaPhraseEmpty)
          {
            if (theParameters.theUseLongPhrase)
            {
              if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID)
              {
                sentence << LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE
                         << theTemperaturePhrase;
                break;
              }
            }
            else
            {
              if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_NOIN_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
                sentence << LAMPOTILA_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID)
              {
                sentence << LAMPOTILA_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE
                         << theTemperaturePhrase;
                break;
              }
            }

            if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
            {
              sentence << theTemperaturePhrase
                       << tienoilla_and_tuntumassa_astetta(intervalStart, phrase_id);
            }
            else
            {
              sentence << theTemperaturePhrase << degreesSentence;
            }
          }
          else if (!dayPhasePhraseEmpty && areaPhraseEmpty)
          {
            if (useDay2Phrase)
            {
              if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_NOIN_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID)
              {
                sentence << HUOMENNA_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE << theDayPhasePhrase;
                break;
              }

              if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                  phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
              {
                sentence << theDayPhasePhrase
                         << tienoilla_and_tuntumassa_astetta(intervalStart, phrase_id);
              }
              else
              {
                sentence << theDayPhasePhrase << degreesSentence;
              }
            }
            else
            {
              if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
                sentence << HUOMENNA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
              else if (phrase_id == VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID)
              {
                sentence << HUOMENNA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE
                         << theDayPhasePhrase << theTemperaturePhrase;
                break;
              }

              if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                  phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
              {
                sentence << theDayPhasePhrase << theTemperaturePhrase
                         << tienoilla_and_tuntumassa_astetta(intervalStart, phrase_id);
              }
              else
              {
                sentence << theDayPhasePhrase << theTemperaturePhrase << degreesSentence;
              }
            }
          }
          else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
          {
            if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
              sentence << SISAMAASSA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID)
            {
              sentence << SISAMAASSA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE
                       << theAreaPhrase << theTemperaturePhrase;
              break;
            }

            if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
            {
              sentence << theAreaPhrase << theTemperaturePhrase
                       << tienoilla_and_tuntumassa_astetta(intervalStart, phrase_id);
            }
            else
            {
              sentence << theAreaPhrase << theTemperaturePhrase << degreesSentence;
            }
          }
          else
          {
            if (phrase_id == NOIN_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_NOIN_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_TIENOILLA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_TUNTUMASSA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAHAN_YLI_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAHAN_YLI_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAJAAT_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAJAAT_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == LAHELLA_ASTETTA_PHRASE_ID)
              sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_LAHELLA_ASTETTA_COMPOSITE_PHRASE;
            else if (phrase_id == VAHAN_NOLLAN_ALAPUOLELLA_PHRASE_ID)
            {
              sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_VAHAN_NOLLAN_ALAPUOLELLA_COMPOSITE_PHRASE
                       << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase;
              break;
            }

            if (phrase_id == TIENOILLA_ASTETTA_PHRASE_ID ||
                phrase_id == TUNTUMASSA_ASTETTA_PHRASE_ID || phrase_id == NOIN_ASTETTA_PHRASE_ID)
            {
              sentence << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase
                       << tienoilla_and_tuntumassa_astetta(intervalStart, phrase_id);
            }
            else
            {
              sentence << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase
                       << degreesSentence;
            }
          }
        }
      }
    }
    break;
    case LAMPOTILA_VALILLA_PHRASE_ID:
    {
      if (theParameters.theForecastAreaId == COASTAL_AREA &&
          theParameters.inlandAndCoastSeparated())
      {
        Sentence temperatureRangeSentence;
        int actualIntervalStart(intervalStart);
        int actualIntervalEnd(intervalEnd);
        temperatureRangeSentence << temperature_range(intervalStart,
                                                      intervalEnd,
                                                      theParameters.theRangeSeparator,
                                                      actualIntervalStart,
                                                      actualIntervalEnd);

        sentence << RANNIKOLLA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE << temperatureRangeSentence
                 << *UnitFactory::create_unit(DegreesCelsius, actualIntervalEnd, true);
      }
      else
      {
        if (useDay2Phrase)
        {
          sentence << HUOMENNA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE << theDayPhasePhrase
                   << temperatureSentence;
        }
        else
        {
          if (dayPhasePhraseEmpty && areaPhraseEmpty)
          {
            if (theParameters.theUseLongPhrase)
              sentence << LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE;
            else
              sentence << LAMPOTILA_INTERVALLI_ASTETTA_COMPOSITE_PHRASE;

            sentence << theTemperaturePhrase << temperatureSentence;
          }
          else if (!dayPhasePhraseEmpty && areaPhraseEmpty)
          {
            sentence << HUOMENNA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE
                     << theDayPhasePhrase << theTemperaturePhrase << temperatureSentence;
          }
          else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
          {
            sentence << SISAMAASSA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE << theAreaPhrase
                     << theTemperaturePhrase << temperatureSentence;
          }
          else
          {
            sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_INTERVALLI_ASTETTA_COMPOSITE_PHRASE
                     << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase
                     << temperatureSentence;
          }
        }
      }
    }
    break;

    case VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID:
    case NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID:
    case NOLLAN_TIENOILLA_PHRASE_ID:
    case SUUNNILLEEN_SAMA_PHRASE_ID:
    {
      if (theParameters.theForecastAreaId == COASTAL_AREA &&
          theParameters.inlandAndCoastSeparated())
      {
        sentence << RANNIKOLLA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << temperatureSentence;
      }
      else
      {
        if (useDay2Phrase)
        {
          sentence << HUOMENNA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase
                   << theAreaPhrase << temperatureSentence;
        }
        else
        {
          if (dayPhasePhraseEmpty && areaPhraseEmpty)
          {
            if (theParameters.theUseLongPhrase)
              sentence << LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE;
            else
              sentence << LAMPOTILA_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE;
            sentence << theTemperaturePhrase << temperatureSentence;
          }
          else if (!dayPhasePhraseEmpty && areaPhraseEmpty)
          {
            sentence << HUOMENNA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theDayPhasePhrase
                     << theTemperaturePhrase << temperatureSentence;
          }
          else if (dayPhasePhraseEmpty && !areaPhraseEmpty)
          {
            sentence << SISAMAASSA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE << theAreaPhrase
                     << theTemperaturePhrase << temperatureSentence;
          }
          else
          {
            sentence << HUOMENNA_SISAMAASSA_LAMPOTILA_ON_SUUNNILLEEN_SAMA_COMPOSITE_PHRASE
                     << theDayPhasePhrase << theAreaPhrase << theTemperaturePhrase
                     << temperatureSentence;
          }
        }
      }
    }
    break;

    case NO_PHRASE_ID:
    {
    }
    break;
  }

  if (theParameters.theAddCommaDelimiterFlag && !sentence.empty())
    theParameters.theSentenceUnderConstruction << Delimiter(COMMA_PUNCTUATION_MARK) << sentence;
  else
    theParameters.theSentenceUnderConstruction << sentence;

  return sentence;
}

Sentence night_sentence(t36hparams& theParameters)
{
  Sentence sentence;

  double temperatureDifference = 100.0;
  int intervalStart = static_cast<int>(kFloatMissing);
  int intervalEnd = static_cast<int>(kFloatMissing);
  bool day1PeriodIncluded = theParameters.theMaxTemperatureDay1 != kFloatMissing;

  if (day1PeriodIncluded)
  {
    temperatureDifference = theParameters.theMean - theParameters.theMeanTemperatureDay1;
  }

  // If day2 is included, we have to use numbers to describe temperature, since
  // day2 story is told before night story
  bool nightlyMinHigherThanDailyMax =
      !(theParameters.theForecastPeriod & DAY2_PERIOD) &&
      (theParameters.theMaxTemperatureDay1 - theParameters.theMinimum < 0);
  bool smallChangeBetweenDay1AndNight =
      !(theParameters.theForecastPeriod & DAY2_PERIOD) &&
      (day1PeriodIncluded && abs(temperatureDifference) <= ABOUT_THE_SAME_UPPER_LIMIT);
  bool moderateChangeBetweenDay1AndNight =
      !(theParameters.theForecastPeriod & DAY2_PERIOD) &&
      (day1PeriodIncluded && abs(temperatureDifference) > ABOUT_THE_SAME_UPPER_LIMIT &&
       abs(temperatureDifference) <= SMALL_CHANGE_UPPER_LIMIT);

  theParameters.theForecastPeriodId = NIGHT_PERIOD;
  theParameters.theSubPeriodId = UNDEFINED_SUBPERIOD;

  Sentence temperatureSentence;

  bool inlandAndCoastSeparately = false;
  if (theParameters.theForecastPeriod & DAY1_PERIOD)
    inlandAndCoastSeparately = theParameters.inlandAndCoastSeparated(DAY1_PERIOD) ||
                               theParameters.morningAndAfternoonSeparated(DAY1_PERIOD);

  if (!inlandAndCoastSeparately)
    inlandAndCoastSeparately = theParameters.inlandAndCoastSeparated(NIGHT_PERIOD);

  if (around_zero_phrase(theParameters) == NO_PHRASE_ID && !inlandAndCoastSeparately)
  {
    if (smallChangeBetweenDay1AndNight)  // no change or small change
    {
      temperatureSentence << SUUNNILLEEN_SAMA_PHRASE;
      theParameters.theTemperaturePhraseId = SUUNNILLEEN_SAMA_PHRASE_ID;
    }
    else if (moderateChangeBetweenDay1AndNight &&
             theParameters.theTemperaturePhraseId !=
                 NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID)  // moderate change
    {
      if (temperatureDifference > 0)
      {
        if (theParameters.theMean <= PAKKASRAJA_TEMPERATURE)
        {
          temperatureSentence << HIEMAN_HEIKOMPAA_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_HEIKOMPAA_PHRASE_ID;
        }
        else if (theParameters.theMean < 0)
        {
          temperatureSentence << HIEMAN_LAUHEMPAA_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_LAUHEMPAA_PHRASE_ID;
        }
        else
        {
          temperatureSentence << HIEMAN_KORKEAMPI_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_KORKEAMPI_PHRASE_ID;
        }
      }
      else
      {
        if (theParameters.theMean <= PAKKASRAJA_TEMPERATURE)
        {
          temperatureSentence << HIEMAN_KIREAMPAA_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_KIREAMPAA_PHRASE_ID;
        }
        else if (theParameters.theMean < 0)
        {
          temperatureSentence << HIEMAN_KYLMEMPAA_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_KYLMEMPAA_PHRASE_ID;
        }
        else
        {
          temperatureSentence << HIEMAN_ALEMPI_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_ALEMPI_PHRASE_ID;
        }
      }
    }
    else if (nightlyMinHigherThanDailyMax)
    {
      temperatureSentence << LAMPOTILA_NOUSEE_PHRASE;
      theParameters.theTemperaturePhraseId = LAMPOTILA_NOUSEE_PHRASE_ID;
    }
    else
    {
      temperatureSentence << temperature_sentence(theParameters, intervalStart, intervalEnd);
    }
  }
  else
  {
    temperatureSentence << temperature_sentence(theParameters, intervalStart, intervalEnd);
  }

  Sentence tonightSentence;
  if (theParameters.theDayAndNightSeparationFlag &&
      theParameters.theFullPeriod.localEndTime().DifferenceInHours(
          theParameters.theFullPeriod.localStartTime()) > 24)
  {
    tonightSentence << PeriodPhraseFactory::create("tonight",
                                                   theParameters.theVariable,
                                                   theParameters.theForecastTime,
                                                   theParameters.theWeatherPeriod,
                                                   theParameters.theWeatherArea);
  }

  sentence << construct_final_sentence(
      theParameters, temperatureSentence, tonightSentence, intervalStart, intervalEnd);

  theParameters.theTomorrowTautologyFlag = false;

  return sentence;
}

Sentence day2_sentence(t36hparams& theParameters)
{
  Sentence sentence;

  int intervalStart = static_cast<int>(kFloatMissing);
  int intervalEnd = static_cast<int>(kFloatMissing);

  double temperatureDifference = 100.0;
  bool day1PeriodIncluded = theParameters.theForecastPeriod & DAY1_PERIOD;

  if (day1PeriodIncluded)
  {
    temperatureDifference = theParameters.theMean - theParameters.theMeanTemperatureDay1;
  }

  bool smallChangeBetweenDay1AndDay2 =
      day1PeriodIncluded && abs(temperatureDifference) <= ABOUT_THE_SAME_UPPER_LIMIT;
  bool moderateChangeBetweenDay1AndDay2 = day1PeriodIncluded &&
                                          abs(temperatureDifference) > ABOUT_THE_SAME_UPPER_LIMIT &&
                                          abs(temperatureDifference) <= SMALL_CHANGE_UPPER_LIMIT;

  theParameters.theForecastPeriodId = DAY2_PERIOD;

  Sentence temperatureSentence;

  if (around_zero_phrase(theParameters) == NO_PHRASE_ID &&
      !theParameters.inlandAndCoastSeparated(DAY1_PERIOD) &&
      !theParameters.inlandAndCoastSeparated(DAY2_PERIOD) &&
      !theParameters.morningAndAfternoonSeparated(DAY1_PERIOD))
  {
    if (smallChangeBetweenDay1AndDay2 && theParameters.theSubPeriodId != DAY2_MORNING_PERIOD &&
        theParameters.theSubPeriodId != DAY2_AFTERNOON_PERIOD)
    {
      temperatureSentence << SUUNNILLEEN_SAMA_PHRASE;
      theParameters.theTemperaturePhraseId = SUUNNILLEEN_SAMA_PHRASE_ID;
    }
    else if (moderateChangeBetweenDay1AndDay2 &&
             theParameters.theSubPeriodId != DAY2_MORNING_PERIOD &&
             theParameters.theSubPeriodId != DAY2_AFTERNOON_PERIOD &&
             theParameters.theTemperaturePhraseId !=
                 NOLLAN_TIENOILLA_TAI_VAHAN_PLUSSAN_PUOLELLA_PHRASE_ID)
    {
      // small change
      if (temperatureDifference > 0)
      {
        if (theParameters.theMean <= PAKKASRAJA_TEMPERATURE)
        {
          temperatureSentence << HIEMAN_HEIKOMPAA_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_HEIKOMPAA_PHRASE_ID;
        }
        else if (theParameters.theMean < 0)
        {
          temperatureSentence << ON_WORD << HIEMAN_LAUHEMPAA_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_LAUHEMPAA_PHRASE_ID;
        }
        else
        {
          temperatureSentence << HIEMAN_KORKEAMPI_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_KORKEAMPI_PHRASE_ID;
        }
      }
      else
      {
        if (theParameters.theMean <= PAKKASRAJA_TEMPERATURE)
        {
          temperatureSentence << HIEMAN_KIREAMPAA_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_KIREAMPAA_PHRASE_ID;
        }
        else if (theParameters.theMean < 0)
        {
          temperatureSentence << HIEMAN_KYLMEMPAA_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_KYLMEMPAA_PHRASE_ID;
        }
        else
        {
          temperatureSentence << HIEMAN_ALEMPI_PHRASE;
          theParameters.theTemperaturePhraseId = HIEMAN_ALEMPI_PHRASE_ID;
        }
      }
    }
    else
    {
      temperatureSentence << temperature_sentence(theParameters, intervalStart, intervalEnd);
    }
  }
  else
  {
    temperatureSentence << temperature_sentence(theParameters, intervalStart, intervalEnd);
  }

  Sentence nextDaySentence;

  // exception: inland and coastal separated and morning and afternoon separated and now we are
  // processing
  // morning on the coastal area
  /*
  if(!theParameters.theTomorrowTautologyFlag &&
         !(theParameters.inlandAndCoastSeparated(DAY2_PERIOD) &&
           theParameters.morningAndAfternoonSeparated(DAY2_PERIOD) &&
           theParameters.theSubPeriodId == DAY2_MORNING_PERIOD &&
           theParameters.theForecastAreaId == COASTAL_AREA)&&
*/
  if (theParameters.theFullPeriod.localEndTime().DifferenceInHours(
          theParameters.theFullPeriod.localStartTime()) > 24)
  {
    // ARE 07.02.2011: if day1 and day2 are included
    if (theParameters.theForecastPeriod & DAY1_PERIOD)
      const_cast<WeatherHistory&>(theParameters.theWeatherArea.history())
          .updateTimePhrase("", "", TextGenPosixTime(1970, 1, 1));

    nextDaySentence << PeriodPhraseFactory::create("next_day",
                                                   theParameters.theVariable,
                                                   theParameters.theForecastTime,
                                                   theParameters.theWeatherPeriod,
                                                   theParameters.theWeatherArea);
  }

  if (!nextDaySentence.empty())
    theParameters.theTomorrowTautologyFlag = true;

  sentence << construct_final_sentence(
      theParameters, temperatureSentence, nextDaySentence, intervalStart, intervalEnd);

  /*
  sentence << nextDaySentence;
  sentence << temperature_phrase(theParameters);
  sentence << temperatureSentence;
  */

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

Sentence construct_sentence(t36hparams& theParameters)
{
  Sentence sentence;

  // Shorthand alias to make code more readable
  auto& p = theParameters;

  int areaMin = (p.theForecastPeriodId == DAY1_PERIOD
                     ? AREA_MIN_DAY1
                     : (p.theForecastPeriodId == NIGHT_PERIOD ? AREA_MIN_NIGHT : AREA_MIN_DAY2));
  int areaMax = (p.theForecastPeriodId == DAY1_PERIOD
                     ? AREA_MAX_DAY1
                     : (p.theForecastPeriodId == NIGHT_PERIOD ? AREA_MAX_NIGHT : AREA_MAX_DAY2));
  int areaMean = (p.theForecastPeriodId == DAY1_PERIOD
                      ? AREA_MEAN_DAY1
                      : (p.theForecastPeriodId == NIGHT_PERIOD ? AREA_MEAN_NIGHT : AREA_MEAN_DAY2));
  int coastMin = (p.theForecastPeriodId == DAY1_PERIOD
                      ? COAST_MIN_DAY1
                      : (p.theForecastPeriodId == NIGHT_PERIOD ? COAST_MIN_NIGHT : COAST_MIN_DAY2));
  int coastMax = (p.theForecastPeriodId == DAY1_PERIOD
                      ? COAST_MAX_DAY1
                      : (p.theForecastPeriodId == NIGHT_PERIOD ? COAST_MAX_NIGHT : COAST_MAX_DAY2));
  int coastMean =
      (p.theForecastPeriodId == DAY1_PERIOD
           ? COAST_MEAN_DAY1
           : (p.theForecastPeriodId == NIGHT_PERIOD ? COAST_MEAN_NIGHT : COAST_MEAN_DAY2));
  int inlandMin =
      (p.theForecastPeriodId == DAY1_PERIOD
           ? INLAND_MIN_DAY1
           : (p.theForecastPeriodId == NIGHT_PERIOD ? INLAND_MIN_NIGHT : INLAND_MIN_DAY2));
  int inlandMax =
      (p.theForecastPeriodId == DAY1_PERIOD
           ? INLAND_MAX_DAY1
           : (p.theForecastPeriodId == NIGHT_PERIOD ? INLAND_MAX_NIGHT : INLAND_MAX_DAY2));
  int inlandMean =
      (p.theForecastPeriodId == DAY1_PERIOD
           ? INLAND_MEAN_DAY1
           : (p.theForecastPeriodId == NIGHT_PERIOD ? INLAND_MEAN_NIGHT : INLAND_MEAN_DAY2));
  if (p.theForecastAreaId == FULL_AREA)
  {
    if (p.theForecastPeriodId == DAY1_PERIOD)
    {
      if (p.theSeasonId == SUMMER_SEASON &&
          p.theWeatherResults[AREA_MAX_DAY1_MORNING]->value() != kFloatMissing &&
          p.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value() != kFloatMissing &&
          p.theWeatherResults[AREA_MAX_DAY1_MORNING]->value() >
              p.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value() +
                  MORNING_AFTERNOON_SEPARATION_LIMIT)
      {
        p.theForecastAreaId = FULL_AREA;
        p.theSubPeriodId = DAY1_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
        p.theMinimum = p.theWeatherResults[AREA_MIN_DAY1_MORNING]->value();
        p.theMaximum = p.theWeatherResults[AREA_MAX_DAY1_MORNING]->value();
        p.theMean = p.theWeatherResults[AREA_MEAN_DAY1_MORNING]->value();

        sentence << day1_sentence(p);

        sentence << Delimiter(COMMA_PUNCTUATION_MARK);

        p.theForecastAreaId = FULL_AREA;
        p.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
        p.theMinimum = p.theWeatherResults[AREA_MIN_DAY1_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[AREA_MEAN_DAY1_AFTERNOON]->value();

        sentence << day1_sentence(p);
      }
      else
      {
        p.theForecastAreaId = FULL_AREA;
        p.theSubPeriodId = UNDEFINED_SUBPERIOD;
        p.theMinimum = p.theWeatherResults[AREA_MIN_DAY1_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[AREA_MAX_DAY1_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[AREA_MEAN_DAY1_AFTERNOON]->value();

        sentence << day1_sentence(p);
      }
    }
    else if (p.theForecastPeriodId == NIGHT_PERIOD)
    {
      p.theForecastAreaId = FULL_AREA;
      p.theMaxTemperatureDay1 = p.theWeatherResults[AREA_MAX_DAY1]->value();
      p.theMeanTemperatureDay1 = p.theWeatherResults[AREA_MEAN_DAY1]->value();
      p.theMinimum = p.theWeatherResults[areaMin]->value();
      p.theMaximum = p.theWeatherResults[areaMax]->value();
      p.theMean = p.theWeatherResults[areaMean]->value();

      if (p.theDayAndNightSeparationFlag || p.theForecastPeriod == NIGHT_PERIOD)
      {
        sentence << night_sentence(p);
      }

      construct_optional_frost_story(p);
    }
    else if (p.theForecastPeriodId == DAY2_PERIOD)
    {
      if (p.theSeasonId == SUMMER_SEASON &&
          p.theWeatherResults[AREA_MAX_DAY2_MORNING]->value() != kFloatMissing &&
          p.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value() != kFloatMissing &&
          p.theWeatherResults[AREA_MAX_DAY2_MORNING]->value() >
              p.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value() +
                  MORNING_AFTERNOON_SEPARATION_LIMIT)
      {
        p.theForecastAreaId = FULL_AREA;
        p.theSubPeriodId = DAY2_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[AREA_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[AREA_MIN_DAY2_MORNING]->value();
        p.theMaximum = p.theWeatherResults[AREA_MAX_DAY2_MORNING]->value();
        p.theMean = p.theWeatherResults[AREA_MEAN_DAY2_MORNING]->value();

        sentence << day2_sentence(p);

        sentence << Delimiter(COMMA_PUNCTUATION_MARK);

        p.theForecastAreaId = FULL_AREA;
        p.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[AREA_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[AREA_MIN_DAY2_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[AREA_MEAN_DAY2_AFTERNOON]->value();

        sentence << day2_sentence(p);
      }
      else
      {
        p.theForecastAreaId = FULL_AREA;
        p.theSubPeriodId = UNDEFINED_SUBPERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[AREA_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[AREA_MIN_DAY2_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[AREA_MAX_DAY2_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[AREA_MEAN_DAY2_AFTERNOON]->value();

        sentence << day2_sentence(p);
      }
    }
  }
  else if (p.theForecastAreaId == INLAND_AREA)
  {
    if (p.theForecastPeriodId == DAY1_PERIOD)
    {
      if (p.theSeasonId == SUMMER_SEASON &&
          p.theWeatherResults[INLAND_MAX_DAY1_MORNING]->value() != kFloatMissing &&
          p.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value() != kFloatMissing &&
          p.theWeatherResults[INLAND_MAX_DAY1_MORNING]->value() >
              p.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value() +
                  MORNING_AFTERNOON_SEPARATION_LIMIT)
      {
        p.theForecastAreaId = INLAND_AREA;
        p.theSubPeriodId = DAY1_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
        p.theMinimum = p.theWeatherResults[INLAND_MIN_DAY1_MORNING]->value();
        p.theMaximum = p.theWeatherResults[INLAND_MAX_DAY1_MORNING]->value();
        p.theMean = p.theWeatherResults[INLAND_MEAN_DAY1_MORNING]->value();

        sentence << day1_sentence(p);

        sentence << Delimiter(COMMA_PUNCTUATION_MARK);

        p.theForecastAreaId = INLAND_AREA;
        p.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
        p.theMinimum = p.theWeatherResults[INLAND_MIN_DAY1_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[INLAND_MEAN_DAY1_AFTERNOON]->value();

        sentence << day1_sentence(p);
      }
      else
      {
        p.theForecastAreaId = INLAND_AREA;
        p.theSubPeriodId = UNDEFINED_SUBPERIOD;
        p.theMinimum = p.theWeatherResults[INLAND_MIN_DAY1_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[INLAND_MAX_DAY1_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[INLAND_MEAN_DAY1_AFTERNOON]->value();

        sentence << day1_sentence(p);
      }
    }
    else if (p.theForecastPeriodId == NIGHT_PERIOD)
    {
      p.theForecastAreaId = INLAND_AREA;
      p.theMaxTemperatureDay1 = p.theWeatherResults[INLAND_MAX_DAY1]->value();
      p.theMeanTemperatureDay1 = p.theWeatherResults[INLAND_MEAN_DAY1]->value();
      p.theMinimum = p.theWeatherResults[inlandMin]->value();
      p.theMaximum = p.theWeatherResults[inlandMax]->value();
      p.theMean = p.theWeatherResults[inlandMean]->value();

      if (p.theDayAndNightSeparationFlag || p.theForecastPeriod == NIGHT_PERIOD)
      {
        sentence << night_sentence(p);
      }

      construct_optional_frost_story(p);
    }
    else if (p.theForecastPeriodId == DAY2_PERIOD)
    {
      if (p.theSeasonId == SUMMER_SEASON &&
          p.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value() != kFloatMissing &&
          p.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value() != kFloatMissing &&
          p.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value() >
              p.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value() +
                  MORNING_AFTERNOON_SEPARATION_LIMIT)
      {
        p.theForecastAreaId = INLAND_AREA;
        p.theSubPeriodId = DAY2_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[INLAND_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[INLAND_MIN_DAY2_MORNING]->value();
        p.theMaximum = p.theWeatherResults[INLAND_MAX_DAY2_MORNING]->value();
        p.theMean = p.theWeatherResults[INLAND_MEAN_DAY2_MORNING]->value();

        sentence << day2_sentence(p);

        sentence << Delimiter(COMMA_PUNCTUATION_MARK);

        p.theForecastAreaId = INLAND_AREA;
        p.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[INLAND_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[INLAND_MIN_DAY2_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[INLAND_MEAN_DAY2_AFTERNOON]->value();

        sentence << day2_sentence(p);
      }
      else
      {
        p.theForecastAreaId = INLAND_AREA;
        p.theSubPeriodId = UNDEFINED_SUBPERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[INLAND_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[INLAND_MIN_DAY2_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[INLAND_MAX_DAY2_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[INLAND_MEAN_DAY2_AFTERNOON]->value();

        sentence << day2_sentence(p);
      }
    }
  }
  else if (p.theForecastAreaId == COASTAL_AREA)
  {
    if (p.theForecastPeriodId == DAY1_PERIOD)
    {
      if (p.theSeasonId == SUMMER_SEASON &&
          p.theWeatherResults[COAST_MAX_DAY1_MORNING]->value() != kFloatMissing &&
          p.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value() != kFloatMissing &&
          p.theWeatherResults[COAST_MAX_DAY1_MORNING]->value() >
              p.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value() +
                  MORNING_AFTERNOON_SEPARATION_LIMIT)
      {
        p.theForecastAreaId = COASTAL_AREA;
        p.theSubPeriodId = DAY1_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY1_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY1_AFTERNOON_PERIOD;
        p.theMinimum = p.theWeatherResults[COAST_MIN_DAY1_MORNING]->value();
        p.theMaximum = p.theWeatherResults[COAST_MAX_DAY1_MORNING]->value();
        p.theMean = p.theWeatherResults[COAST_MEAN_DAY1_MORNING]->value();

        sentence << day1_sentence(p);

        sentence << Delimiter(COMMA_PUNCTUATION_MARK);

        p.theForecastAreaId = COASTAL_AREA;
        p.theSubPeriodId = DAY1_AFTERNOON_PERIOD;
        p.theMinimum = p.theWeatherResults[COAST_MIN_DAY1_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[COAST_MEAN_DAY1_AFTERNOON]->value();

        sentence << day1_sentence(p);
      }
      else
      {
        p.theForecastAreaId = COASTAL_AREA;
        p.theSubPeriodId = UNDEFINED_SUBPERIOD;
        p.theMinimum = p.theWeatherResults[COAST_MIN_DAY1_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[COAST_MAX_DAY1_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[COAST_MEAN_DAY1_AFTERNOON]->value();

        sentence << day1_sentence(p);
      }
    }
    else if (p.theForecastPeriodId == NIGHT_PERIOD)
    {
      p.theForecastAreaId = COASTAL_AREA;
      p.theMaxTemperatureDay1 = p.theWeatherResults[COAST_MAX_DAY1]->value();
      p.theMeanTemperatureDay1 = p.theWeatherResults[COAST_MEAN_DAY1]->value();
      p.theMinimum = p.theWeatherResults[coastMin]->value();
      p.theMaximum = p.theWeatherResults[coastMax]->value();
      p.theMean = p.theWeatherResults[coastMean]->value();

      if (p.theDayAndNightSeparationFlag || p.theForecastPeriod == NIGHT_PERIOD)
      {
        sentence << night_sentence(p);
      }

      construct_optional_frost_story(p);
    }
    else if (p.theForecastPeriodId == DAY2_PERIOD)
    {
      if (p.theSeasonId == SUMMER_SEASON &&
          p.theWeatherResults[COAST_MAX_DAY2_MORNING]->value() != kFloatMissing &&
          p.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value() != kFloatMissing &&
          p.theWeatherResults[COAST_MAX_DAY2_MORNING]->value() >
              p.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value() +
                  MORNING_AFTERNOON_SEPARATION_LIMIT)
      {
        p.theForecastAreaId = COASTAL_AREA;
        p.theSubPeriodId = DAY2_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY2_MORNING_PERIOD;
        p.theForecastSubPeriod |= DAY2_AFTERNOON_PERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[COAST_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[COAST_MIN_DAY2_MORNING]->value();
        p.theMaximum = p.theWeatherResults[COAST_MAX_DAY2_MORNING]->value();
        p.theMean = p.theWeatherResults[COAST_MEAN_DAY2_MORNING]->value();

        sentence << day2_sentence(p);

        sentence << Delimiter(COMMA_PUNCTUATION_MARK);

        p.theForecastAreaId = COASTAL_AREA;
        p.theSubPeriodId = DAY2_AFTERNOON_PERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[COAST_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[COAST_MIN_DAY2_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[COAST_MEAN_DAY2_AFTERNOON]->value();

        sentence << day2_sentence(p);
      }
      else
      {
        p.theForecastAreaId = COASTAL_AREA;
        p.theSubPeriodId = UNDEFINED_SUBPERIOD;
        p.theMeanTemperatureDay1 = p.theWeatherResults[COAST_MEAN_DAY1]->value();
        p.theMinimum = p.theWeatherResults[COAST_MIN_DAY2_AFTERNOON]->value();
        p.theMaximum = p.theWeatherResults[COAST_MAX_DAY2_AFTERNOON]->value();
        p.theMean = p.theWeatherResults[COAST_MEAN_DAY2_AFTERNOON]->value();

        sentence << day2_sentence(p);
      }
    }
  }

  return sentence;
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

Paragraph temperature_max36hours_sentence(t36hparams& theParameters)
{
  auto& p = theParameters;  // shorthand alias

  Paragraph paragraph;

  if (p.theForecastArea == NO_AREA)
    return paragraph;

  const int temperature_limit_coast_inland =
      optional_int(p.theVariable + "::temperature_limit_coast_inland", 3);

  // Day1, Night, Day2
  // 1. Day1 inland
  // 2. Day2 inland
  // 3. Night inland
  // 4. Day1 coastal
  // 5. Day2 coastal
  // 6. Night coastal

  // Day1, Night
  // 1. Day1 inland
  // 2. Day1 coastal
  // 3. Night inland
  // 4. Night coastal

  // Night, Day2
  // 1. Night inland
  // 2. Day2 inland
  // 3. Night coastal
  // 4. Day2 coastal

  // Day1
  // 1. Day1 inland
  // 2. Day1 coastal

  // Night
  // 1. Night inland
  // 2. Night coastal

  vector<int> periodAreas;
  processing_order processingOrder(UNDEFINED_PROCESSING_ORDER);

  if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD &&
      p.theForecastPeriod & DAY2_PERIOD)
  {
    processingOrder = DAY1_DAY2_NIGHT;
  }
  else if (p.theForecastPeriod & NIGHT_PERIOD && p.theForecastPeriod & DAY2_PERIOD)
  {
    processingOrder = NIGHT_DAY2;
  }
  else if (p.theForecastPeriod & DAY1_PERIOD && p.theForecastPeriod & NIGHT_PERIOD)
  {
    processingOrder = DAY1_NIGHT;
  }
  else if (p.theForecastPeriod == DAY1_PERIOD)
  {
    processingOrder = DAY1;
  }
  else if (p.theForecastPeriod == NIGHT_PERIOD)
  {
    processingOrder = NIGHT;
  }

  bool separate_inland_and_coast_day1 = false;
  bool separate_inland_and_coast_day2 = false;
  bool separate_inland_and_coast_night = false;
  bool intervalUsedInland = false;
  bool intervalUsedCoast = false;
  int intervalStartInland = 0;
  int intervalEndInland = 0;
  int intervalStartCoast = 0;
  int intervalEndCoast = 0;

  if (p.theWeatherResults[INLAND_MEAN_DAY1]->value() != kFloatMissing &&
      p.theWeatherResults[COAST_MEAN_DAY1]->value() != kFloatMissing &&
      !(p.theSeasonId == WINTER_SEASON &&
        p.theWeatherResults[COAST_MEAN_DAY1]->value() <= COASTAL_AREA_IGNORE_LIMIT))
  {
    intervalUsedInland = false;
    intervalUsedCoast = false;

    get_interval_details(p,
                         p.theWeatherResults[INLAND_MIN_DAY1]->value(),
                         p.theWeatherResults[INLAND_MEAN_DAY1]->value(),
                         p.theWeatherResults[INLAND_MAX_DAY1]->value(),
                         intervalUsedInland,
                         intervalStartInland,
                         intervalEndInland);
    get_interval_details(p,
                         p.theWeatherResults[COAST_MIN_DAY1]->value(),
                         p.theWeatherResults[COAST_MEAN_DAY1]->value(),
                         p.theWeatherResults[COAST_MAX_DAY1]->value(),
                         intervalUsedCoast,
                         intervalStartCoast,
                         intervalEndCoast);

    float temperature_diff_day1 = abs(round(p.theWeatherResults[INLAND_MEAN_DAY1]->value() -
                                            p.theWeatherResults[COAST_MEAN_DAY1]->value()));

    if (temperature_diff_day1 >= temperature_limit_coast_inland &&
        !p.theCoastalAndInlandTogetherFlag)
    {
      if (intervalUsedInland && intervalUsedCoast)
      {
        if (abs(round(intervalStartCoast - intervalStartInland)) +
                abs(round(intervalEndCoast - intervalEndInland)) >=
            temperature_limit_coast_inland)
          separate_inland_and_coast_day1 = true;
      }
      else
      {
        separate_inland_and_coast_day1 = true;
      }
    }
  }

  if (p.theWeatherResults[INLAND_MEAN_DAY2]->value() != kFloatMissing &&
      p.theWeatherResults[COAST_MEAN_DAY2]->value() != kFloatMissing &&
      !(p.theSeasonId == WINTER_SEASON &&
        p.theWeatherResults[COAST_MEAN_DAY2]->value() <= COASTAL_AREA_IGNORE_LIMIT))
  {
    intervalUsedInland = false;
    intervalUsedCoast = false;

    get_interval_details(p,
                         p.theWeatherResults[INLAND_MIN_DAY2]->value(),
                         p.theWeatherResults[INLAND_MEAN_DAY2]->value(),
                         p.theWeatherResults[INLAND_MAX_DAY2]->value(),
                         intervalUsedInland,
                         intervalStartInland,
                         intervalEndInland);
    get_interval_details(p,
                         p.theWeatherResults[COAST_MIN_DAY2]->value(),
                         p.theWeatherResults[COAST_MEAN_DAY2]->value(),
                         p.theWeatherResults[COAST_MAX_DAY2]->value(),
                         intervalUsedCoast,
                         intervalStartCoast,
                         intervalEndCoast);

    float temperature_diff_day2 = abs(p.theWeatherResults[INLAND_MEAN_DAY2]->value() -
                                      p.theWeatherResults[COAST_MEAN_DAY2]->value());

    if (temperature_diff_day2 >= temperature_limit_coast_inland &&
        !p.theCoastalAndInlandTogetherFlag)
    {
      if (intervalUsedInland && intervalUsedCoast)
      {
        if (abs(round(intervalStartCoast - intervalStartInland)) +
                abs(round(intervalEndCoast - intervalEndInland)) >=
            temperature_limit_coast_inland)
          separate_inland_and_coast_day2 = true;
      }
      else
      {
        separate_inland_and_coast_day2 = true;
      }
    }
  }

  if (p.theWeatherResults[INLAND_MEAN_NIGHT]->value() != kFloatMissing &&
      p.theWeatherResults[COAST_MEAN_NIGHT]->value() != kFloatMissing &&
      !(p.theSeasonId == WINTER_SEASON &&
        p.theWeatherResults[COAST_MEAN_NIGHT]->value() <= COASTAL_AREA_IGNORE_LIMIT))
  {
    intervalUsedInland = false;
    intervalUsedCoast = false;

    get_interval_details(p,
                         p.theWeatherResults[INLAND_MIN_NIGHT]->value(),
                         p.theWeatherResults[INLAND_MEAN_NIGHT]->value(),
                         p.theWeatherResults[INLAND_MAX_NIGHT]->value(),
                         intervalUsedInland,
                         intervalStartInland,
                         intervalEndInland);
    get_interval_details(p,
                         p.theWeatherResults[COAST_MIN_NIGHT]->value(),
                         p.theWeatherResults[COAST_MEAN_NIGHT]->value(),
                         p.theWeatherResults[COAST_MAX_NIGHT]->value(),
                         intervalUsedCoast,
                         intervalStartCoast,
                         intervalEndCoast);

    float temperature_diff_night = abs(p.theWeatherResults[INLAND_MEAN_NIGHT]->value() -
                                       p.theWeatherResults[COAST_MEAN_NIGHT]->value());

    if (temperature_diff_night >= temperature_limit_coast_inland &&
        !p.theCoastalAndInlandTogetherFlag)
    {
      if (intervalUsedInland && intervalUsedCoast)
      {
        if (abs(round(intervalStartCoast - intervalStartInland)) +
                abs(round(intervalEndCoast - intervalEndInland)) >=
            temperature_limit_coast_inland)
          separate_inland_and_coast_night = true;
      }
      else
      {
        separate_inland_and_coast_night = true;
      }
    }
  }

  if (processingOrder == DAY1_DAY2_NIGHT)
  {
    if (separate_inland_and_coast_day1)
    {
      periodAreas.push_back(DAY1_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(DAY1_COASTAL);
    }
    else
    {
      periodAreas.push_back(DAY1_FULL);
    }

    periodAreas.push_back(DELIMITER_DOT);

    if (separate_inland_and_coast_day2)
    {
      periodAreas.push_back(DAY2_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(DAY2_COASTAL);
    }
    else
    {
      periodAreas.push_back(DAY2_FULL);
    }

    periodAreas.push_back(DELIMITER_DOT);

    if (separate_inland_and_coast_night)
    {
      periodAreas.push_back(NIGHT_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(NIGHT_COASTAL);
    }
    else
    {
      periodAreas.push_back(NIGHT_FULL);
    }
  }
  else if (processingOrder == DAY1_NIGHT)
  {
    if (separate_inland_and_coast_day1)
    {
      periodAreas.push_back(DAY1_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(DAY1_COASTAL);
    }
    else
    {
      periodAreas.push_back(DAY1_FULL);
    }

    periodAreas.push_back(DELIMITER_DOT);

    if (separate_inland_and_coast_night)
    {
      periodAreas.push_back(NIGHT_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(NIGHT_COASTAL);
    }
    else
    {
      periodAreas.push_back(NIGHT_FULL);
    }
  }
  else if (processingOrder == NIGHT_DAY2)
  {
    if (separate_inland_and_coast_night)
    {
      periodAreas.push_back(NIGHT_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(NIGHT_COASTAL);
    }
    else
    {
      periodAreas.push_back(NIGHT_FULL);
    }

    periodAreas.push_back(DELIMITER_DOT);

    if (separate_inland_and_coast_day2)
    {
      periodAreas.push_back(DAY2_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(DAY2_COASTAL);
    }
    else
    {
      periodAreas.push_back(DAY2_FULL);
    }
  }
  else if (processingOrder == DAY1)
  {
    if (separate_inland_and_coast_day1)
    {
      periodAreas.push_back(DAY1_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(DAY1_COASTAL);
    }
    else
    {
      periodAreas.push_back(DAY1_FULL);
    }
  }
  else if (processingOrder == NIGHT)
  {
    if (separate_inland_and_coast_night)
    {
      periodAreas.push_back(NIGHT_INLAND);
      periodAreas.push_back(DELIMITER_COMMA);
      periodAreas.push_back(NIGHT_COASTAL);
    }
    else
    {
      periodAreas.push_back(NIGHT_FULL);
    }
  }

  //	  Sentence sentenceUnderConstruction;

  p.theUseLongPhrase = true;
  for (unsigned int i = 0; i < periodAreas.size(); i++)
  {
    int periodArea = periodAreas[i];

    if (periodArea == DELIMITER_DOT)
    {
      p.theUseLongPhrase = true;
      if (!p.theSentenceUnderConstruction.empty())
      {
        // if comparison between today and tomorrow, use comma
        if (i > 0 && processingOrder == DAY1_DAY2_NIGHT && periodAreas[i - 1] == DAY1_FULL &&
            periodAreas[i + 1] == DAY2_FULL && !p.morningAndAfternoonSeparated(DAY1_PERIOD) &&
            !p.morningAndAfternoonSeparated(DAY2_PERIOD))
        {
          p.theAddCommaDelimiterFlag = true;
        }
        else
        {
          if (((processingOrder == DAY1_NIGHT && periodAreas[i - 1] == DAY1_FULL &&
                !separate_inland_and_coast_day1) ||
               (processingOrder == NIGHT_DAY2 && periodAreas[i - 1] == NIGHT_FULL &&
                !separate_inland_and_coast_day2)) &&
              !separate_inland_and_coast_night)
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
      }
      continue;
    }
    if (periodArea == DELIMITER_COMMA)
    {
      if (!p.theSentenceUnderConstruction.empty())
      {
        p.theAddCommaDelimiterFlag = true;
      }
      continue;
    }

    forecast_period_id period_id(NO_PERIOD);
    forecast_area_id area_id(NO_AREA);

    if (periodArea == DAY1_INLAND || periodArea == DAY1_COASTAL || periodArea == DAY1_FULL)
    {
      period_id = DAY1_PERIOD;
      p.theWeatherPeriod = p.theGenerator.period(1);

      if (p.theForecastArea & INLAND_AREA && p.theForecastArea & COASTAL_AREA)
      {
        if (separate_inland_and_coast_day1)
        {
          area_id = periodAreas[i] == DAY1_INLAND ? INLAND_AREA : COASTAL_AREA;
          p.theForecastAreaDay1 |= COASTAL_AREA;
          p.theForecastAreaDay1 |= INLAND_AREA;
        }
        else
        {
          area_id = FULL_AREA;
          p.theForecastAreaDay1 |= FULL_AREA;
        }
      }
      else if (p.theForecastArea & INLAND_AREA)
      {
        area_id = INLAND_AREA;
        p.theForecastAreaDay1 |= INLAND_AREA;
      }
      else if (p.theForecastArea & COASTAL_AREA)
      {
        area_id = COASTAL_AREA;
        p.theForecastAreaDay1 |= COASTAL_AREA;
      }
      else if (p.theForecastArea & FULL_AREA)
      {
        area_id = FULL_AREA;
        p.theForecastAreaDay1 |= FULL_AREA;
      }
    }
    else if (periodArea == NIGHT_INLAND || periodArea == NIGHT_COASTAL || periodArea == NIGHT_FULL)
    {
      period_id = NIGHT_PERIOD;

      if (p.theForecastPeriod & DAY1_PERIOD)
        p.theWeatherPeriod = p.theGenerator.period(2);
      else
        p.theWeatherPeriod = p.theGenerator.period(1);

      if (p.theForecastArea & INLAND_AREA && p.theForecastArea & COASTAL_AREA)
      {
        if (separate_inland_and_coast_night)
        {
          area_id = periodAreas[i] == NIGHT_INLAND ? INLAND_AREA : COASTAL_AREA;
          p.theForecastAreaNight |= COASTAL_AREA;
          p.theForecastAreaNight |= INLAND_AREA;
        }
        else
        {
          area_id = FULL_AREA;
          p.theForecastAreaNight |= FULL_AREA;
        }
      }
      else if (p.theForecastArea & INLAND_AREA)
      {
        area_id = INLAND_AREA;
        p.theForecastAreaNight |= INLAND_AREA;
      }
      else if (p.theForecastArea & COASTAL_AREA)
      {
        area_id = COASTAL_AREA;
        p.theForecastAreaNight |= COASTAL_AREA;
      }
      else if (p.theForecastArea & FULL_AREA)
      {
        area_id = FULL_AREA;
        p.theForecastAreaNight |= FULL_AREA;
      }
    }
    else if (periodArea == DAY2_INLAND || periodArea == DAY2_COASTAL || periodArea == DAY2_FULL)
    {
      period_id = DAY2_PERIOD;
      if (p.theForecastPeriod & DAY1_PERIOD)
        p.theWeatherPeriod = p.theGenerator.period(3);
      else if (p.theForecastPeriod & NIGHT_PERIOD)
        p.theWeatherPeriod = p.theGenerator.period(2);
      else
        p.theWeatherPeriod = p.theGenerator.period(1);

      if (p.theForecastArea & INLAND_AREA && p.theForecastArea & COASTAL_AREA)
      {
        if (separate_inland_and_coast_day2)
        {
          area_id = periodAreas[i] == DAY2_INLAND ? INLAND_AREA : COASTAL_AREA;
          p.theForecastAreaDay2 |= COASTAL_AREA;
          p.theForecastAreaDay2 |= INLAND_AREA;
        }
        else
        {
          area_id = FULL_AREA;
          p.theForecastAreaDay2 |= FULL_AREA;
        }
      }
      else if (p.theForecastArea & INLAND_AREA)
      {
        area_id = INLAND_AREA;
        p.theForecastAreaDay2 |= INLAND_AREA;
      }
      else if (p.theForecastArea & COASTAL_AREA)
      {
        area_id = COASTAL_AREA;
        p.theForecastAreaDay2 |= COASTAL_AREA;
      }
      else if (p.theForecastArea & FULL_AREA)
      {
        area_id = FULL_AREA;
        p.theForecastAreaDay2 |= FULL_AREA;
      }
    }

    p.theForecastAreaId = area_id;
    p.theForecastPeriodId = period_id;
    // p.theSeasonId = forecast_season;

    p.theDayAndNightSeparationFlag = separate_day_and_night(p, p.theForecastAreaId);

    construct_sentence(p);

    p.theAddCommaDelimiterFlag = false;
  }  // for-loop

  if (!p.theSentenceUnderConstruction.empty())
  {
    paragraph << p.theSentenceUnderConstruction;
  }

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

  theLog << "period contains ";

  if (generator00.isday(1))
  {
    if (generator00.size() > 2)
    {
      theLog << "today, night and tomorrow\n";
    }
    else if (generator00.size() == 2)
    {
      theLog << "today and night\n";
    }
    else
    {
      theLog << "today\n";
    }
  }
  else
  {
    if (generator00.size() == 1)
    {
      theLog << "one night\n";
    }
    else
    {
      theLog << "night and tomorrow\n";
    }
  }

  // Period generator
  WeatherPeriod fullPeriod(periodStartTime, periodEndTime);
  NightAndDayPeriodGenerator generator(fullPeriod, itsVar);

  unsigned short forecast_area = 0x0;
  unsigned short forecast_period = 0x0;
  forecast_season_id forecast_season =
      get_forecast_season(itsArea, itsSources, generator.period(1), itsVar);

  // container to hold the results
  weather_result_container_type weatherResults;

  GridForecaster forecaster;

  if (generator.isday(1))
  {
    // when the first period is day, and the second period exists
    // it must be night, and if third period exists it must be day
    forecast_period |= DAY1_PERIOD;
    forecast_period |= (generator.size() > 1 ? NIGHT_PERIOD : 0x0);
    forecast_period |= (generator.size() > 2 ? DAY2_PERIOD : 0x0);
  }
  else
  {
    // if the first period is not day, it must be night, and
    // if second period exists it must be day
    forecast_period |= NIGHT_PERIOD;
    forecast_period |= (generator.size() > 1 ? DAY2_PERIOD : 0x0);
  }

  // Initialize the container for WeatherResult objects
  for (int i = AREA_MIN_DAY1; i < UNDEFINED_WEATHER_RESULT_ID; i++)
  {
    weatherResults.insert(make_pair(i, new WeatherResult(kFloatMissing, 0)));
  }

  WeatherPeriod period = generator.period(1);

  if (forecast_period & DAY1_PERIOD)
  {
    // calculate results for day1
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

    // day1 period exists, so
    // if the area is included, it must have valid values
    forecast_area |=
        (weatherResults[COAST_MIN_DAY1]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
    forecast_area |=
        (weatherResults[INLAND_MIN_DAY1]->value() != kFloatMissing ? INLAND_AREA : 0x0);
    forecast_area |= (weatherResults[AREA_MIN_DAY1]->value() != kFloatMissing ? FULL_AREA : 0x0);

    if (forecast_area == NO_AREA)
    {
      theLog << "Something wrong, NO Coastal area NOR Inland area is included!\n";
    }
    else
    {
      if (forecast_period & NIGHT_PERIOD)
      {
        // calculate results for night
        period = generator.period(2);
        log_start_time_and_end_time(theLog, "Night: ", period);

        if (forecast_area & INLAND_AREA)
        {
          calculate_results(theLog,
                            itsVar,
                            itsSources,
                            itsArea,
                            period,
                            NIGHT_PERIOD,
                            forecast_season,
                            INLAND_AREA,
                            weatherResults);
          valid_value_period_check(
              weatherResults[INLAND_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
        }
        if (forecast_area & COASTAL_AREA && (forecast_period & NIGHT_PERIOD))
        {
          calculate_results(theLog,
                            itsVar,
                            itsSources,
                            itsArea,
                            period,
                            NIGHT_PERIOD,
                            forecast_season,
                            COASTAL_AREA,
                            weatherResults);
          valid_value_period_check(
              weatherResults[COAST_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
        }
        if (forecast_area & FULL_AREA && (forecast_period & NIGHT_PERIOD))
        {
          calculate_results(theLog,
                            itsVar,
                            itsSources,
                            itsArea,
                            period,
                            NIGHT_PERIOD,
                            forecast_season,
                            FULL_AREA,
                            weatherResults);
          valid_value_period_check(
              weatherResults[AREA_MIN_NIGHT]->value(), forecast_period, NIGHT_PERIOD);
        }
        forecast_area |=
            (weatherResults[COAST_MIN_NIGHT]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
        forecast_area |=
            (weatherResults[INLAND_MIN_NIGHT]->value() != kFloatMissing ? INLAND_AREA : 0x0);
        forecast_area |=
            (weatherResults[AREA_MIN_NIGHT]->value() != kFloatMissing ? FULL_AREA : 0x0);
      }

      if (forecast_period & DAY2_PERIOD)
      {
        // calculate results for day2
        period = generator.period(3);
        log_start_time_and_end_time(theLog, "Day2: ", period);

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
          valid_value_period_check(
              weatherResults[COAST_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
        }
        if (forecast_area & FULL_AREA && (forecast_period & NIGHT_PERIOD))
        {
          calculate_results(theLog,
                            itsVar,
                            itsSources,
                            itsArea,
                            period,
                            DAY2_PERIOD,
                            forecast_season,
                            FULL_AREA,
                            weatherResults);
          valid_value_period_check(
              weatherResults[AREA_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
        }

        forecast_area |=
            (weatherResults[COAST_MIN_DAY2]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
        forecast_area |=
            (weatherResults[INLAND_MIN_DAY2]->value() != kFloatMissing ? INLAND_AREA : 0x0);
        forecast_area |=
            (weatherResults[AREA_MIN_DAY2]->value() != kFloatMissing ? FULL_AREA : 0x0);
      }
    }
  }
  else
  {
    period = generator.period(1);
    // calculate results for night
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

    // night period exists, so
    // if the area is included, it must have valid values
    forecast_area |=
        (weatherResults[COAST_MIN_NIGHT]->value() != kFloatMissing ? COASTAL_AREA : 0x0);
    forecast_area |=
        (weatherResults[INLAND_MIN_NIGHT]->value() != kFloatMissing ? INLAND_AREA : 0x0);
    forecast_area |= (weatherResults[AREA_MIN_NIGHT]->value() != kFloatMissing ? FULL_AREA : 0x0);

    if (forecast_area == NO_AREA)
    {
      valid_value_period_check(kFloatMissing, forecast_period, NIGHT_PERIOD);
      theLog << "Something wrong, NO Coastal area NOR Inland area is included! \n";
    }
    else
    {
      if (forecast_period & DAY2_PERIOD)
      {
        // calculate results for day2
        period = generator.period(2);
        log_start_time_and_end_time(theLog, "Day2: ", period);

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
          valid_value_period_check(
              weatherResults[COAST_MIN_DAY2]->value(), forecast_period, DAY2_PERIOD);
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
    }
  }

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
  {
    parameters.theCoastalAndInlandTogetherFlag = true;
  }

  if (forecast_area != NO_AREA)
  {
    paragraph << temperature_max36hours_sentence(parameters);
  }

  log_weather_results(parameters);

  // delete the allocated WeatherResult-objects
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
