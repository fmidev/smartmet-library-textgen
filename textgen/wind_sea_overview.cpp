// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WindStory::sea_overview
 *
 * A wind narrative for open-sea areas built on the area MEAN wind.
 *
 * Unlike wind_overview, no spatial tail statistic ("top wind") is used
 * for anything. Change detection runs on the time-smoothed hourly area
 * mean, the reported range comes from spatial percentiles around the
 * mean, and direction is the vector mean with its spread. Gusts are
 * never mixed into the speed statistics; an optional separate sentence
 * can mention strong gusts.
 *
 * See docs/users/stories/wind/wind_sea_overview.md for the settings.
 */
// ======================================================================

#include "Delimiter.h"
#include "Integer.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PositiveRange.h"
#include "QualifiedDirectionPhrase.h"
#include "Sentence.h"
#include "UnitFactory.h"
#include "WeatherForecast.h"
#include "WeekdayTools.h"
#include "WindStory.h"
#include "WindStoryTools.h"

#include <calculator/GridForecaster.h>
#include <calculator/RangeAcceptor.h>
#include <calculator/Settings.h>
#include <calculator/TimeTools.h>
#include <calculator/WeatherArea.h>
#include <calculator/WeatherPeriod.h>
#include <calculator/WeatherResult.h>
#include <calculator/WeatherResultTools.h>
#include <macgyver/Exception.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace TextGen;
using namespace TextGen::WindStoryTools;
using namespace std;

namespace TextGen
{
namespace
{
// Phrase keys (all exist in the po dictionaries, shared with wind_overview)
const char* const TIME_RATE_CHANGE_DIRECTION_PHRASE =
    "[iltapaivalla] [nopeasti] [heikkenevaa] [etelatuulta]";
const char* const TIME_RATE_CHANGE_PHRASE = "[iltapaivalla] [nopeasti] [heikkenevaa]";
const char* const TIME_DIRECTION_PHRASE = "[iltapaivalla] [etelatuulta]";
const char* const TIME_WIND_TURNS_PHRASE = "[iltapaivalla] tuuli kaantyy [etelaan]";
const char* const TIME_WIND_VEERS_PHRASE = "[iltapaivalla] tuuli kaantyy myotapaivaan [etelaan]";
const char* const TIME_WIND_BACKS_PHRASE = "[iltapaivalla] tuuli kaantyy vastapaivaan [etelaan]";
// Qualified direction glyphs: [1] = turn-to form, [2] = noun form, see QualifiedDirectionPhrase
const char* const VEERING_DIRECTION_KEY = "myotapaivaan kaantyen [etelaan] [etelatuulta]";
const char* const BACKING_DIRECTION_KEY = "vastapaivaan kaantyen [etelaan] [etelatuulta]";
// The turn without its sense: Finnish "pohjoiseen kääntyvää tuulta", most languages the noun
const char* const TURNING_DIRECTION_KEY = "kaantyen [etelaan] [etelatuulta]";
const char* const GUST_PHRASE = "[aika] paikoin [puuskia], kovimmillaan [n] [m/s]";
const char* const GUST_QUADRANT_PHRASE =
    "[aika] [suunta] paikoin [puuskia], kovimmillaan [n] [m/s]";
const char* const STRONG_GUSTS_WORD = "voimakkaita puuskia";
const char* const VERY_STRONG_GUSTS_WORD = "hyvin voimakkaita puuskia";
const char* const STRENGTHENING_WORD = "voimistuvaa";
const char* const WEAKENING_WORD = "heikkenevaa";
const char* const GRADUALLY_WORD = "vahitellen";
const char* const FAST_WORD = "nopeasti";
const char* const WIND_WORD = "tuulta";  // partitive, no direction
const char* const INITIALLY_WORD = "aluksi";
const char* const VARIABLE_WIND_PHRASE = "suunnaltaan vaihtelevaa tuulta";

// ----------------------------------------------------------------------
// Settings
// ----------------------------------------------------------------------

struct SeaParams
{
  double speedChangeThreshold = 3.0;  // m/s change of the smoothed area mean
  double weakWindLimit = 4.0;         // m/s, no changes reported below this
  int smoothingHours = 3;             // running mean window (odd)
  int maxChanges = 3;                 // reported speed changes per story
  int gradualHours = 8;               // "vahitellen" when a change lasts at least this
  int fastHours = 3;                  // "nopeasti" when a change of fastChange fits in this
  double fastChange = 5.0;
  int rangeLowerPercentile = 25;
  int rangeUpperPercentile = 75;
  int rangeMinWidth = 2;
  int rangeMaxWidth = 5;
  int rangeHours = 3;                      // hours at the end of a change used for its range
  int rangeReportMinDifference = 2;        // a steady phase repeats the range only if a bound
                                           // moved at least this much
  double directionChangeThreshold = 45.0;  // degrees
  double directionVariableMaxSpeed = 6.0;  // "vaihtelevaa" only below this mean speed
  int directionSplitMinHours = 6;          // a steady phase at least this long may be split
  bool gustReporting = false;
  double gustLimit = 15.0;
  bool veeringBacking = false;  // distinguish clockwise (veering) and counterclockwise turns
  bool separateInitialSentence = false;  // steady sentence + change sentence instead of "aluksi"
  string rangeSeparator = "-";
  bool weekdays = true;  // default for marking a change of day, see day::phrases

  // Local convective gust cells (see wind_overview): hours where a small part of the area has
  // gusts above the cutoff for a short time are cleaned from the area statistics and
  // optionally reported in a separate sentence
  double cellCutoff = KOVA_LOWER_LIMIT;  // m/s on the gust
  double cellMaxDuration = 3.0;          // hours, longer runs are synoptic
  double cellMaxAreaFraction = 10.0;     // %, larger shares are synoptic
  double cellMinAreaFraction = 0.0;      // %
  bool cellReporting = false;
  string cellStyle = "sentence";  // "quadrant" adds the part of the area
};

SeaParams read_params(const string& var)
{
  using namespace Settings;
  SeaParams p;
  p.speedChangeThreshold =
      optional_double(var + "::speed_change_threshold", p.speedChangeThreshold);
  p.weakWindLimit = optional_double(var + "::weak_wind_limit", p.weakWindLimit);
  p.smoothingHours = optional_int(var + "::smoothing_hours", p.smoothingHours);
  p.maxChanges = optional_int(var + "::max_changes", p.maxChanges);
  p.gradualHours = optional_int(var + "::gradual_hours", p.gradualHours);
  p.fastHours = optional_int(var + "::fast_hours", p.fastHours);
  p.fastChange = optional_double(var + "::fast_change", p.fastChange);
  p.rangeLowerPercentile = optional_int(var + "::range_lower_percentile", p.rangeLowerPercentile);
  p.rangeUpperPercentile = optional_int(var + "::range_upper_percentile", p.rangeUpperPercentile);
  p.rangeMinWidth = optional_int(var + "::range_min_width", p.rangeMinWidth);
  p.rangeMaxWidth = optional_int(var + "::range_max_width", p.rangeMaxWidth);
  p.rangeHours = optional_int(var + "::range_hours", p.rangeHours);
  p.rangeReportMinDifference =
      optional_int(var + "::range_report_min_difference", p.rangeReportMinDifference);
  p.directionChangeThreshold =
      optional_double(var + "::direction_change_threshold", p.directionChangeThreshold);
  p.directionVariableMaxSpeed =
      optional_double(var + "::direction_variable_max_speed", p.directionVariableMaxSpeed);
  p.directionSplitMinHours =
      optional_int(var + "::direction_split_min_hours", p.directionSplitMinHours);
  p.gustReporting = optional_bool(var + "::gust_reporting", p.gustReporting);
  p.gustLimit = optional_double(var + "::gust_limit", p.gustLimit);
  p.veeringBacking = (optional_string(var + "::turn_phrases", "plain") == "veering_backing");
  p.separateInitialSentence =
      optional_bool(var + "::separate_initial_sentence", p.separateInitialSentence);
  p.rangeSeparator = optional_string(var + "::rangeseparator", p.rangeSeparator);
  p.weekdays = optional_bool(var + "::weekdays", p.weekdays);
  p.cellCutoff = optional_double(var + "::convective_cell_cutoff", p.cellCutoff);
  p.cellMaxDuration = optional_double(var + "::convective_cell_max_duration", p.cellMaxDuration);
  p.cellMaxAreaFraction =
      optional_double(var + "::convective_cell_max_area_fraction", p.cellMaxAreaFraction);
  p.cellMinAreaFraction =
      optional_double(var + "::convective_cell_min_area_fraction", p.cellMinAreaFraction);
  p.cellReporting = optional_bool(var + "::convective_cell_reporting", p.cellReporting);
  p.cellStyle = optional_string(var + "::convective_cell_style", p.cellStyle);
  if (p.smoothingHours < 1)
    p.smoothingHours = 1;
  if (p.smoothingHours % 2 == 0)
    p.smoothingHours++;
  if (p.rangeHours < 1)
    p.rangeHours = 1;
  if (p.maxChanges < 0)
    p.maxChanges = 0;
  return p;
}

// ----------------------------------------------------------------------
// Hourly data
// ----------------------------------------------------------------------

struct HourData
{
  TextGenPosixTime time;
  float mean = kFloatMissing;                             // area mean wind speed
  float smooth = kFloatMissing;                           // time smoothed area mean
  float lower = kFloatMissing;                            // spatial lower percentile of wind speed
  float upper = kFloatMissing;                            // spatial upper percentile of wind speed
  WeatherResult direction{kFloatMissing, kFloatMissing};  // vector mean + spread
  float gust = kFloatMissing;                             // area maximum gust
  float gustShare = kFloatMissing;  // % of the area with gusts at or above the cell cutoff
  bool cell = false;                // hour belongs to a convective cell, statistics exclude it
};

// A local convective gust cell: a run of hours
struct ConvectiveCell
{
  int beg = 0;  // first and last hour index
  int end = 0;
  int peakIndex = 0;  // hour of the strongest gust
  float peakGust = 0.0F;
  WeatherArea::Type quadrant = WeatherArea::Full;  // part of the area, if one dominates
};

bool cell_detection_enabled(const SeaParams& params)
{
  return params.cellMaxDuration > 0.0 || params.cellMaxAreaFraction > 0.0;
}

string stamp(const TextGenPosixTime& t)
{
  ostringstream os;
  os << setfill('0') << setw(4) << t.GetYear() << setw(2) << t.GetMonth() << setw(2) << t.GetDay()
     << setw(2) << t.GetHour() << setw(2) << t.GetMin();
  return os.str();
}

// Percentile of the spatial wind speed distribution at one hour, from 1 m/s bins.
// Returns the lower edge of the bin in which the cumulative share reaches the percentile.
void spatial_percentiles(GridForecaster& forecaster,
                         const AnalysisSources& sources,
                         const WeatherArea& area,
                         const WeatherPeriod& period,
                         const string& var,
                         int lowerPercentile,
                         int upperPercentile,
                         float& lower,
                         float& upper,
                         const Acceptor& pointAcceptor = DefaultAcceptor())
{
  const int topLimit = static_cast<int>(HIRMUMYRSKY_LOWER_LIMIT);  // last bin is open ended
  float cumulative = 0.0;
  lower = kFloatMissing;
  upper = kFloatMissing;
  for (int bin = 0; bin <= topLimit; bin++)
  {
    const auto binStart = static_cast<float>(bin);
    RangeAcceptor acceptor;
    acceptor.lowerLimit(binStart);
    if (bin < topLimit)
      acceptor.upperLimit(binStart + 1.0F - 0.0001F);
    WeatherResult share = forecaster.analyze(var + "::fake::distribution::share",
                                             sources,
                                             WindSpeed,
                                             Mean,
                                             Percentage,
                                             area,
                                             period,
                                             DefaultAcceptor(),
                                             pointAcceptor,
                                             acceptor);
    if (share.value() == kFloatMissing)
      return;
    cumulative += share.value();
    if (lower == kFloatMissing && cumulative >= lowerPercentile)
      lower = binStart;
    if (upper == kFloatMissing && cumulative >= upperPercentile)
    {
      upper = binStart;
      break;
    }
  }
}

// Wind speed statistics of one hour. With an acceptor only the grid points it accepts are used
// (removal of a convective cell); the fake keys then carry the given suffix, and fake data
// without the suffixed keys is left as it is.
void analyze_speed(GridForecaster& forecaster,
                   const AnalysisSources& sources,
                   const WeatherArea& area,
                   const string& var,
                   const SeaParams& params,
                   HourData& h,
                   const Acceptor& acceptor = DefaultAcceptor(),
                   const string& suffix = "")
{
  const WeatherPeriod hour(h.time, h.time);
  const string fakeBase = var + "::fake::" + stamp(h.time);
  const string meanKey = fakeBase + "::speed::mean" + suffix;
  if (!suffix.empty() && Settings::isset(fakeBase + "::speed::mean") && !Settings::isset(meanKey))
    return;

  WeatherResult mean =
      forecaster.analyze(meanKey, sources, WindSpeed, Mean, Mean, area, hour, acceptor);
  if (!suffix.empty() && mean.value() == kFloatMissing)
    return;  // nothing left after the removal, keep the unfiltered statistics
  WeatherResultTools::checkMissingValue("wind_sea_overview", WindSpeed, mean);
  h.mean = mean.value();

  // Spatial percentiles: fake values may be given directly for testing
  const string lowerKey = fakeBase + "::speed::lower" + suffix;
  const string upperKey = fakeBase + "::speed::upper" + suffix;
  if (Settings::isset(lowerKey) && Settings::isset(upperKey))
  {
    h.lower = Settings::require_result(lowerKey).value();
    h.upper = Settings::require_result(upperKey).value();
  }
  else if (!Settings::isset(meanKey))
  {
    spatial_percentiles(forecaster,
                        sources,
                        area,
                        hour,
                        var,
                        params.rangeLowerPercentile,
                        params.rangeUpperPercentile,
                        h.lower,
                        h.upper,
                        acceptor);
  }
  if (h.lower == kFloatMissing)
    h.lower = h.mean;
  if (h.upper == kFloatMissing)
    h.upper = h.mean;
}

// Area maximum gust of one hour, with an optional acceptor as in analyze_speed
void analyze_gust(GridForecaster& forecaster,
                  const AnalysisSources& sources,
                  const WeatherArea& area,
                  const string& var,
                  HourData& h,
                  const Acceptor& acceptor = DefaultAcceptor(),
                  const string& suffix = "")
{
  const WeatherPeriod hour(h.time, h.time);
  const string fakeBase = var + "::fake::" + stamp(h.time);
  const string key = fakeBase + "::gust::maximum" + suffix;
  if (!suffix.empty() && Settings::isset(fakeBase + "::gust::maximum") && !Settings::isset(key))
    return;
  WeatherResult gust =
      forecaster.analyze(key, sources, GustSpeed, Maximum, Maximum, area, hour, acceptor);
  if (suffix.empty() || gust.value() != kFloatMissing)
    h.gust = gust.value();
}

// Share (%) of the area where the gust of one hour is at or above the cell cutoff
float analyze_gust_share(GridForecaster& forecaster,
                         const AnalysisSources& sources,
                         const WeatherArea& area,
                         const string& var,
                         const SeaParams& params,
                         const HourData& h)
{
  const WeatherPeriod hour(h.time, h.time);
  RangeAcceptor aboveCutoff;
  aboveCutoff.lowerLimit(static_cast<float>(params.cellCutoff));
  return forecaster
      .analyze(var + "::fake::" + stamp(h.time) + "::gust::share",
               sources,
               GustSpeed,
               Mean,
               Percentage,
               area,
               hour,
               DefaultAcceptor(),
               DefaultAcceptor(),
               aboveCutoff)
      .value();
}

// Runs of hours where a small part of the area has gusts above the cutoff for a short time.
// Longer runs and larger shares are synoptic and left alone.
vector<ConvectiveCell> detect_cells(const vector<HourData>& hours,
                                    const SeaParams& params,
                                    MessageLogger& log)
{
  vector<ConvectiveCell> cells;
  if (!cell_detection_enabled(params))
    return cells;

  log << "Convective cell detection: cutoff=" << params.cellCutoff << " m/s on the gust, area "
      << "share window=(" << params.cellMinAreaFraction << "%, " << params.cellMaxAreaFraction
      << "%), max duration=" << params.cellMaxDuration << " h\n";

  auto flagged = [&](int i)
  {
    const float share = hours[i].gustShare;
    if (share == kFloatMissing)
      return false;
    return share > params.cellMinAreaFraction &&
           (params.cellMaxAreaFraction <= 0.0 || share < params.cellMaxAreaFraction);
  };

  const int n = static_cast<int>(hours.size());
  int i = 0;
  while (i < n)
  {
    if (!flagged(i))
    {
      i++;
      continue;
    }
    ConvectiveCell cell;
    cell.beg = i;
    cell.peakIndex = i;
    while (i < n && flagged(i))
    {
      if (hours[i].gust != kFloatMissing && hours[i].gust > cell.peakGust)
      {
        cell.peakGust = hours[i].gust;
        cell.peakIndex = i;
      }
      i++;
    }
    cell.end = i - 1;
    const int duration = cell.end - cell.beg + 1;
    if (params.cellMaxDuration > 0.0 && duration >= params.cellMaxDuration)
    {
      log << "Convective candidate rejected: " << hours[cell.beg].time.ToIsoExtendedStr() << " - "
          << hours[cell.end].time.ToIsoExtendedStr() << " lasts " << duration
          << " h, treated as synoptic\n";
      continue;
    }
    log << "Convective cell: " << hours[cell.beg].time.ToIsoExtendedStr() << " - "
        << hours[cell.end].time.ToIsoExtendedStr() << " peak gust " << fixed << setprecision(1)
        << cell.peakGust << " m/s\n";
    cells.push_back(cell);
  }
  return cells;
}

// The part of the area where a cell is, when the gusts of one quadrant are clearly the
// strongest. Otherwise the cell moved or is ambiguous and the whole area is kept.
void determine_cell_quadrants(GridForecaster& forecaster,
                              const AnalysisSources& sources,
                              const WeatherArea& area,
                              const string& var,
                              const vector<HourData>& hours,
                              vector<ConvectiveCell>& cells,
                              MessageLogger& log)
{
  if (area.isPoint())
    return;
  const array<pair<WeatherArea::Type, const char*>, 4> quadrants = {
      {{WeatherArea::Northern, "north"},
       {WeatherArea::Southern, "south"},
       {WeatherArea::Eastern, "east"},
       {WeatherArea::Western, "west"}}};
  for (auto& cell : cells)
  {
    const WeatherPeriod period(hours[cell.beg].time, hours[cell.end].time);
    const string fakeBase = var + "::fake::" + stamp(hours[cell.beg].time) + "::gust::quadrant::";
    array<float, 4> peak{};
    bool anyValid = false;
    for (size_t q = 0; q < quadrants.size(); q++)
    {
      WeatherArea quadrant(area);
      quadrant.type(quadrants[q].first);
      WeatherResult result = forecaster.analyze(
          fakeBase + quadrants[q].second, sources, GustSpeed, Maximum, Maximum, quadrant, period);
      peak[q] = (result.value() == kFloatMissing ? 0.0F : result.value());
      anyValid = anyValid || result.value() != kFloatMissing;
    }
    if (!anyValid)
      continue;
    size_t best = 0;
    for (size_t q = 1; q < quadrants.size(); q++)
      if (peak[q] > peak[best])
        best = q;
    float secondBest = 0.0F;
    for (size_t q = 0; q < quadrants.size(); q++)
      if (q != best)
        secondBest = max(secondBest, peak[q]);
    if (peak[best] - secondBest >= 1.0F)
      cell.quadrant = quadrants[best].first;
    log << "Convective cell quadrant peaks: N=" << fixed << setprecision(1) << peak[0]
        << " S=" << peak[1] << " E=" << peak[2] << " W=" << peak[3] << " -> "
        << (cell.quadrant == WeatherArea::Full ? "whole area" : quadrants[best].second) << '\n';
  }
}

vector<HourData> collect_hours(const string& var,
                               const AnalysisSources& sources,
                               const WeatherArea& area,
                               const WeatherPeriod& period,
                               const SeaParams& params,
                               vector<ConvectiveCell>& cells,
                               MessageLogger& log)
{
  GridForecaster forecaster;
  vector<HourData> hours;

  // Gusts are needed for the gust sentence and for cell detection. Data without gusts is
  // accepted when only the detection would use them.
  bool gustsNeeded = params.gustReporting || cell_detection_enabled(params);

  TextGenPosixTime t = period.localStartTime();
  while (t <= period.localEndTime())
  {
    HourData h;
    h.time = t;
    WeatherPeriod hour(t, t);
    const string fakeBase = var + "::fake::" + stamp(t);

    analyze_speed(forecaster, sources, area, var, params, h);

    h.direction = forecaster.analyze(
        fakeBase + "::direction::mean", sources, WindDirection, Mean, Mean, area, hour);
    WeatherResultTools::checkMissingValue("wind_sea_overview", WindDirection, h.direction);

    if (gustsNeeded)
    {
      try
      {
        analyze_gust(forecaster, sources, area, var, h);
        if (cell_detection_enabled(params))
          h.gustShare = analyze_gust_share(forecaster, sources, area, var, params, h);
      }
      catch (...)
      {
        if (params.gustReporting)
          throw;
        log << "Gust data not available, convective cell detection disabled\n";
        gustsNeeded = false;
        h.gust = kFloatMissing;
        h.gustShare = kFloatMissing;
      }
    }

    hours.push_back(h);
    t.ChangeByHours(1);
  }

  // Convective cells: exclude the grid points above the cutoff from the statistics of the hours
  // the cells cover, so that a local cell does not disturb the forecast for the whole area
  cells = detect_cells(hours, params, log);
  if (!cells.empty())
  {
    RangeAcceptor belowCutoff;
    belowCutoff.upperLimit(static_cast<float>(params.cellCutoff) - 0.0001F);
    for (const auto& cell : cells)
      for (int i = cell.beg; i <= cell.end; i++)
      {
        analyze_speed(forecaster, sources, area, var, params, hours[i], belowCutoff, "::no_cell");
        analyze_gust(forecaster, sources, area, var, hours[i], belowCutoff, "::no_cell");
        hours[i].cell = true;
      }
    if (params.cellReporting && params.cellStyle == "quadrant")
      determine_cell_quadrants(forecaster, sources, area, var, hours, cells, log);
  }

  // Time smoothing of the area mean (centered running mean, shrinking at the ends)
  const int half = (params.smoothingHours - 1) / 2;
  const int n = static_cast<int>(hours.size());
  for (int i = 0; i < n; i++)
  {
    double sum = 0.0;
    int count = 0;
    for (int j = max(0, i - half); j <= min(n - 1, i + half); j++)
    {
      sum += hours[j].mean;
      count++;
    }
    hours[i].smooth = static_cast<float>(sum / count);
  }

  log << "Hourly area statistics (time, mean, smoothed, p" << params.rangeLowerPercentile << ", p"
      << params.rangeUpperPercentile << ", direction, spread, gust):\n";
  for (const auto& h : hours)
  {
    log << "  " << h.time.ToIsoExtendedStr() << fixed << setprecision(1) << "  mean=" << h.mean
        << "  smooth=" << h.smooth << "  lo=" << h.lower << "  hi=" << h.upper
        << "  dir=" << h.direction.value() << "  spread=" << h.direction.error();
    if (h.gust != kFloatMissing)
      log << "  gust=" << h.gust;
    if (h.gustShare != kFloatMissing)
      log << "  gust_share=" << h.gustShare << "%";
    if (h.cell)
      log << "  (convective cell removed)";
    log << '\n';
  }
  return hours;
}

// ----------------------------------------------------------------------
// Phases
// ----------------------------------------------------------------------

enum class PhaseType
{
  Steady,
  Strengthening,
  Weakening
};

const char* phase_type_string(PhaseType t)
{
  switch (t)
  {
    case PhaseType::Steady:
      return "steady";
    case PhaseType::Strengthening:
      return "strengthening";
    case PhaseType::Weakening:
      return "weakening";
  }
  return "?";
}

struct Phase
{
  int beg = 0;  // index into hours
  int end = 0;
  PhaseType type = PhaseType::Steady;
  float change = 0.0;  // smoothed mean at end minus at beg
};

// Turning points of the smoothed series: every change between rising, flat and falling,
// plus both end points. Steps smaller than FLAT_STEP are considered flat.
const float FLAT_STEP = 0.05F;

int step_sign(float d)
{
  if (d > FLAT_STEP)
    return 1;
  if (d < -FLAT_STEP)
    return -1;
  return 0;
}

vector<int> turning_points(const vector<HourData>& hours)
{
  const int n = static_cast<int>(hours.size());
  vector<int> pts;
  pts.push_back(0);
  if (n < 2)
    return pts;
  int prevSign = step_sign(hours[1].smooth - hours[0].smooth);
  for (int i = 2; i < n; i++)
  {
    int s = step_sign(hours[i].smooth - hours[i - 1].smooth);
    if (s != prevSign)
      pts.push_back(i - 1);
    prevSign = s;
  }
  if (pts.back() != n - 1)
    pts.push_back(n - 1);
  return pts;
}

int leg_sign(const vector<HourData>& hours, int a, int b)
{
  return step_sign(hours[b].smooth - hours[a].smooth);
}

// Remove points where the two legs have the same tendency (rising, flat or falling)
void merge_monotone(vector<int>& pts, const vector<HourData>& hours)
{
  bool changed = true;
  while (changed && pts.size() > 2)
  {
    changed = false;
    for (size_t i = 1; i + 1 < pts.size(); i++)
    {
      if (leg_sign(hours, pts[i - 1], pts[i]) == leg_sign(hours, pts[i], pts[i + 1]))
      {
        pts.erase(pts.begin() + i);
        changed = true;
        break;
      }
    }
  }
}

// Remove small wiggles inside a trend: an interior leg whose amplitude is below the threshold and
// whose neighbouring legs share the same tendency is eliminated together with its two turning
// points. Small legs between opposite trends are kept and later classified as steady.
void remove_small_wiggles(vector<int>& pts, const vector<HourData>& hours, double threshold)
{
  merge_monotone(pts, hours);
  while (pts.size() > 3)
  {
    int bestLeg = -1;
    float bestAmp = 0.0;
    for (size_t i = 1; i + 2 < pts.size(); i++)  // leg pts[i] -> pts[i+1], both interior
    {
      const int before = leg_sign(hours, pts[i - 1], pts[i]);
      const int after = leg_sign(hours, pts[i + 1], pts[i + 2]);
      if (before != after || before == 0)
        continue;
      float amp = fabs(hours[pts[i + 1]].smooth - hours[pts[i]].smooth);
      if (bestLeg < 0 || amp < bestAmp)
      {
        bestLeg = static_cast<int>(i);
        bestAmp = amp;
      }
    }
    if (bestLeg < 0 || bestAmp >= threshold)
      break;
    pts.erase(pts.begin() + bestLeg, pts.begin() + bestLeg + 2);
    merge_monotone(pts, hours);
  }
}

// Sharpen the bounds of significant legs: hours at either end where the series only creeps
// towards the final value are split off into small (steady) legs so that the change is timed
// where it actually happens.
void sharpen_legs(vector<int>& pts, const vector<HourData>& hours, double threshold)
{
  vector<int> result;
  for (size_t i = 0; i + 1 < pts.size(); i++)
  {
    const int a = pts[i];
    const int b = pts[i + 1];
    const float change = hours[b].smooth - hours[a].smooth;
    result.push_back(a);
    if (fabs(change) < threshold || b - a < 3)
      continue;
    // an hour is "creeping" when its step is well below the mean slope of the leg and the
    // series is still close to the value at that end of the leg
    const float meanSlope = fabs(change) / static_cast<float>(b - a);
    const float creepStep = 0.5F * meanSlope;
    const float tolerance = max(0.5F, 0.15F * fabs(change));
    int newStart = a;
    while (newStart + 1 < b &&
           fabs(hours[newStart + 1].smooth - hours[newStart].smooth) < creepStep &&
           fabs(hours[newStart + 1].smooth - hours[a].smooth) <= tolerance)
      newStart++;
    int newEnd = b;
    while (newEnd - 1 > newStart &&
           fabs(hours[newEnd].smooth - hours[newEnd - 1].smooth) < creepStep &&
           fabs(hours[b].smooth - hours[newEnd - 1].smooth) <= tolerance)
      newEnd--;
    if (newEnd - newStart < 1)
      continue;
    if (newStart > a)
      result.push_back(newStart);
    if (newEnd < b)
      result.push_back(newEnd);
  }
  result.push_back(pts.back());
  pts = result;
}

vector<Phase> build_phases(const vector<int>& pts,
                           const vector<HourData>& hours,
                           const SeaParams& params)
{
  vector<Phase> phases;
  for (size_t i = 0; i + 1 < pts.size(); i++)
  {
    Phase ph;
    ph.beg = pts[i];
    ph.end = pts[i + 1];
    ph.change = hours[ph.end].smooth - hours[ph.beg].smooth;
    bool weak =
        hours[ph.beg].smooth < params.weakWindLimit && hours[ph.end].smooth < params.weakWindLimit;
    if (weak || fabs(ph.change) < params.speedChangeThreshold)
      ph.type = PhaseType::Steady;
    else
      ph.type = (ph.change > 0 ? PhaseType::Strengthening : PhaseType::Weakening);
    phases.push_back(ph);
  }
  // a steady lead-in or tail of an hour or two is part of the neighbouring change
  const int minSteadyHours = 2;
  if (phases.size() > 1 && phases.front().type == PhaseType::Steady &&
      phases.front().end - phases.front().beg < minSteadyHours &&
      phases[1].type != PhaseType::Steady)
  {
    phases[1].beg = phases.front().beg;
    phases[1].change = hours[phases[1].end].smooth - hours[phases[1].beg].smooth;
    phases.erase(phases.begin());
  }
  if (phases.size() > 1 && phases.back().type == PhaseType::Steady &&
      phases.back().end - phases.back().beg < minSteadyHours &&
      phases[phases.size() - 2].type != PhaseType::Steady)
  {
    Phase& prev = phases[phases.size() - 2];
    prev.end = phases.back().end;
    prev.change = hours[prev.end].smooth - hours[prev.beg].smooth;
    phases.pop_back();
  }
  // merge consecutive steady phases
  for (size_t i = 0; i + 1 < phases.size();)
  {
    if (phases[i].type == PhaseType::Steady && phases[i + 1].type == PhaseType::Steady)
    {
      phases[i].end = phases[i + 1].end;
      phases[i].change = hours[phases[i].end].smooth - hours[phases[i].beg].smooth;
      phases.erase(phases.begin() + i + 1);
    }
    else
      i++;
  }
  return phases;
}

int count_changes(const vector<Phase>& phases)
{
  int n = 0;
  for (const auto& ph : phases)
    if (ph.type != PhaseType::Steady)
      n++;
  return n;
}

// Limit the number of reported changes by dropping the weakest ones
void limit_changes(vector<Phase>& phases, const vector<HourData>& hours, const SeaParams& params)
{
  while (count_changes(phases) > params.maxChanges)
  {
    int weakest = -1;
    for (size_t i = 0; i < phases.size(); i++)
    {
      if (phases[i].type == PhaseType::Steady)
        continue;
      if (weakest < 0 || fabs(phases[i].change) < fabs(phases[weakest].change))
        weakest = static_cast<int>(i);
    }
    if (weakest < 0)
      break;
    phases[weakest].type = PhaseType::Steady;
    // merge with steady neighbours
    for (size_t i = 0; i + 1 < phases.size();)
    {
      if (phases[i].type == PhaseType::Steady && phases[i + 1].type == PhaseType::Steady)
      {
        phases[i].end = phases[i + 1].end;
        phases[i].change = hours[phases[i].end].smooth - hours[phases[i].beg].smooth;
        phases.erase(phases.begin() + i + 1);
      }
      else
        i++;
    }
  }
}

// ----------------------------------------------------------------------
// Direction
// ----------------------------------------------------------------------

double angle_difference(double a, double b)
{
  double d = fabs(a - b);
  while (d > 360.0)
    d -= 360.0;
  return (d > 180.0 ? 360.0 - d : d);
}

// Signed shortest turn from one direction to another in degrees: positive = clockwise (veering)
double signed_turn(double from, double to)
{
  double d = fmod(to - from, 360.0);
  if (d > 180.0)
    d -= 360.0;
  if (d <= -180.0)
    d += 360.0;
  return d;
}

struct DirectionInfo
{
  bool variable = false;
  bool exact = false;  // good accuracy -> "N-tuulta", otherwise "N-puoleista tuulta"
  int compass8 = 0;    // 1..8
  double degrees = 0.0;
  string phrase;      // partitive phrase key
  string turnPhrase;  // "etelaan" style key
};

const std::array<const char*, 9> TURN_KEYS = {"",
                                              "pohjoiseen",
                                              "koilliseen",
                                              "itaan",
                                              "kaakkoon",
                                              "etelaan",
                                              "lounaaseen",
                                              "lanteen",
                                              "luoteeseen"};

// Direction over a run of hours from the hourly vector means: speed weighted circular mean,
// spread = the larger of the mean hourly spread and the angular scatter of the hourly means.
DirectionInfo analyze_direction(
    const string& var, const vector<HourData>& hours, int beg, int end, const SeaParams& params)
{
  DirectionInfo info;
  double sx = 0.0;
  double sy = 0.0;
  double weightSum = 0.0;
  double errSum = 0.0;
  double meanSpeed = 0.0;
  for (int i = beg; i <= end; i++)
  {
    const double rad = hours[i].direction.value() * M_PI / 180.0;
    const double w = max(0.1, static_cast<double>(hours[i].mean));
    sx += w * sin(rad);
    sy += w * cos(rad);
    weightSum += w;
    errSum += hours[i].direction.error();
    meanSpeed += hours[i].smooth;
  }
  const int n = end - beg + 1;
  meanSpeed /= n;
  double deg = atan2(sx, sy) * 180.0 / M_PI;
  if (deg < 0)
    deg += 360.0;
  // angular scatter of the hourly means around the weighted mean
  double scatter = 0.0;
  for (int i = beg; i <= end; i++)
  {
    const double d = angle_difference(hours[i].direction.value(), deg);
    scatter += d * d;
  }
  scatter = sqrt(scatter / n);
  const double spread = max(errSum / n, scatter);
  const WeatherResult dir(static_cast<float>(deg), static_cast<float>(spread));

  WindDirectionAccuracy accuracy = direction_accuracy(dir.error(), var);
  info.degrees = dir.value();
  info.compass8 = direction8th(dir.value());
  if (accuracy == bad_accuracy && meanSpeed < params.directionVariableMaxSpeed)
  {
    info.variable = true;
    info.phrase = VARIABLE_WIND_PHRASE;
  }
  else
  {
    info.exact = (accuracy == good_accuracy);
    info.phrase = to_string(info.compass8) + (info.exact ? "-tuulta" : "-puoleista tuulta");
    info.turnPhrase = TURN_KEYS[info.compass8];
  }
  return info;
}

bool direction_changed(const DirectionInfo& a, const DirectionInfo& b, const SeaParams& params)
{
  if (a.variable != b.variable)
    return true;
  if (a.variable && b.variable)
    return false;
  if (a.compass8 == b.compass8)
    return false;
  return angle_difference(a.degrees, b.degrees) >= params.directionChangeThreshold;
}

// Split long steady phases where the direction turns without a speed change
void split_steady_phases_on_direction(vector<Phase>& phases,
                                      const vector<HourData>& hours,
                                      const string& var,
                                      const SeaParams& params,
                                      MessageLogger& log)
{
  for (size_t p = 0; p < phases.size(); p++)
  {
    Phase& ph = phases[p];
    if (ph.type != PhaseType::Steady)
      continue;
    if (ph.end - ph.beg < params.directionSplitMinHours)
      continue;
    const int probe = min(params.rangeHours - 1, (ph.end - ph.beg) / 2);
    DirectionInfo startDir = analyze_direction(var, hours, ph.beg, ph.beg + probe, params);
    DirectionInfo endDir = analyze_direction(var, hours, ph.end - probe, ph.end, params);
    if (!direction_changed(startDir, endDir, params))
      continue;
    // find the first hour where the turn has progressed at least half way
    int splitAt = -1;
    for (int i = ph.beg + probe + 1; i < ph.end - probe; i++)
    {
      double turned = angle_difference(hours[i].direction.value(), startDir.degrees);
      double total = angle_difference(endDir.degrees, startDir.degrees);
      if (startDir.variable || endDir.variable || turned >= total / 2.0)
      {
        splitAt = i;
        break;
      }
    }
    if (splitAt <= ph.beg || splitAt >= ph.end)
      continue;
    log << "Splitting steady phase " << hours[ph.beg].time.ToIsoExtendedStr() << " - "
        << hours[ph.end].time.ToIsoExtendedStr() << " at " << hours[splitAt].time.ToIsoExtendedStr()
        << " because the direction turns\n";
    Phase second;
    second.beg = splitAt;
    second.end = ph.end;
    second.type = PhaseType::Steady;
    second.change = hours[second.end].smooth - hours[second.beg].smooth;
    ph.end = splitAt;
    ph.change = hours[ph.end].smooth - hours[ph.beg].smooth;
    phases.insert(phases.begin() + p + 1, second);
    // the inserted phase is examined in the next iteration
  }
}

// ----------------------------------------------------------------------
// Ranges
// ----------------------------------------------------------------------

struct Range
{
  int lower = 0;
  int upper = 0;
  bool operator==(const Range& o) const { return lower == o.lower && upper == o.upper; }
  bool operator!=(const Range& o) const { return !(*this == o); }
  bool differs(const Range& o, int minDifference) const
  {
    return abs(lower - o.lower) >= minDifference || abs(upper - o.upper) >= minDifference;
  }
};

Range compute_range(const vector<HourData>& hours, int beg, int end, const SeaParams& params)
{
  double lo = 0.0;
  double hi = 0.0;
  double mean = 0.0;
  for (int i = beg; i <= end; i++)
  {
    lo += hours[i].lower;
    hi += hours[i].upper;
    mean += hours[i].mean;
  }
  const int n = end - beg + 1;
  lo /= n;
  hi /= n;
  mean /= n;

  Range r;
  r.lower = static_cast<int>(lround(lo));
  r.upper = static_cast<int>(lround(hi));
  const int roundedMean = static_cast<int>(lround(mean));
  r.lower = min(r.lower, roundedMean);
  r.upper = max(r.upper, roundedMean);

  if (r.upper - r.lower < params.rangeMinWidth)
  {
    // widen symmetrically around the mean
    while (r.upper - r.lower < params.rangeMinWidth)
    {
      if (roundedMean - r.lower <= r.upper - roundedMean && r.lower > 0)
        r.lower--;
      else
        r.upper++;
    }
  }
  while (r.upper - r.lower > params.rangeMaxWidth)
  {
    // shrink from the side further away from the mean
    if (mean - r.lower > r.upper - mean)
      r.lower++;
    else
      r.upper--;
  }
  if (r.lower < 0)
    r.lower = 0;
  return r;
}

// First hour after beg whose hourly range has moved from the given range by at least
// range_report_min_difference, or -1 if there is none up to end
int range_change_start(const vector<HourData>& hours,
                       int beg,
                       int end,
                       const Range& startRange,
                       const SeaParams& params)
{
  for (int i = beg + 1; i <= end; i++)
    if (compute_range(hours, i, i, params).differs(startRange, params.rangeReportMinDifference))
      return i;
  return -1;
}

// ----------------------------------------------------------------------
// Sentence helpers
// ----------------------------------------------------------------------

string rate_word(const Phase& ph, const SeaParams& params)
{
  const int duration = ph.end - ph.beg;
  if (duration <= params.fastHours && fabs(ph.change) >= params.fastChange)
    return FAST_WORD;
  if (duration >= params.gradualHours)
    return GRADUALLY_WORD;
  return EMPTY_STRING;
}

// Time phrases of the story. A change of day is marked with WeekdayTools::day_phase_phrase,
// using the history of the area so that the stories of one product agree on the day.
class TimeWords
{
 public:
  TimeWords(const string& var,
            const TextGenPosixTime& forecastTime,
            vector<string> preferences,
            WeatherHistory& history)
      : itsVar(var),
        itsForecastTime(forecastTime),
        itsPreferences(std::move(preferences)),
        itsHistory(history)
  {
  }

  // The phrase without a day marker, for comparisons
  string plain(const TextGenPosixTime& t, bool alkaen) const
  {
    string phrase = get_time_phrase(t, itsVar, alkaen);
    return (phrase.empty() ? EMPTY_STRING : phrase);
  }

  // The phrase as it is written; call in the order the phrases appear in the text
  string operator()(const TextGenPosixTime& t, bool alkaen)
  {
    const string phrase = plain(t, alkaen);
    if (phrase == EMPTY_STRING)
      return phrase;
    return WeekdayTools::day_phase_phrase(t, itsForecastTime, phrase, itsPreferences, itsHistory);
  }

  // The time by which something has happened, "aamuksi", "by the morning"; used for the
  // completion of a turn. Languages without the form translate it like the plain phrase.
  string by(const TextGenPosixTime& t)
  {
    static const map<string, string> translative = {{"aamuyolla", "aamuyoksi"},
                                                    {"aamulla", "aamuksi"},
                                                    {"aamupaivalla", "aamupaivaksi"},
                                                    {"iltapaivalla", "iltapaivaksi"},
                                                    {"illalla", "illaksi"},
                                                    {"iltayolla", "iltayoksi"},
                                                    {"keskiyolla", "keskiyoksi"}};
    const string phrase = plain(t, false);
    auto it = translative.find(phrase);
    if (it == translative.end())
      return (*this)(t, false);
    return WeekdayTools::day_phase_phrase(
        t, itsForecastTime, it->second, itsPreferences, itsHistory);
  }

 private:
  const string& itsVar;
  TextGenPosixTime itsForecastTime;
  vector<string> itsPreferences;
  WeatherHistory& itsHistory;
};

// Sentence about the strongest of the detected cells, covering all of them
Sentence cell_sentence(const vector<HourData>& hours,
                       const vector<ConvectiveCell>& cells,
                       const SeaParams& params,
                       TimeWords& timeWord,
                       MessageLogger& log)
{
  const ConvectiveCell* strongest = &cells[0];
  WeatherArea::Type quadrant = cells[0].quadrant;
  for (const auto& cell : cells)
  {
    if (cell.peakGust > strongest->peakGust)
      strongest = &cell;
    if (cell.quadrant != quadrant)
      quadrant = WeatherArea::Full;  // the cells disagree
  }
  const char* quadrantWord = "";
  if (params.cellStyle == "quadrant")
    switch (quadrant)
    {
      case WeatherArea::Northern:
        quadrantWord = "pohjoisosissa";
        break;
      case WeatherArea::Southern:
        quadrantWord = "etelaosissa";
        break;
      case WeatherArea::Eastern:
        quadrantWord = "itaosissa";
        break;
      case WeatherArea::Western:
        quadrantWord = "lansiosissa";
        break;
      default:
        break;
    }
  const char* gustsWord =
      (strongest->peakGust >= MYRSKY_LOWER_LIMIT ? VERY_STRONG_GUSTS_WORD : STRONG_GUSTS_WORD);
  const string when = timeWord(hours[strongest->peakIndex].time, false);

  Sentence sentence;
  if (*quadrantWord != 0)
    sentence << GUST_QUADRANT_PHRASE << when << quadrantWord;
  else
    sentence << GUST_PHRASE << when;
  sentence << gustsWord << Integer(static_cast<int>(lround(strongest->peakGust)))
           << *UnitFactory::create(MetersPerSecond);
  log << "Convective cell sentence: peak gust " << fixed << setprecision(1) << strongest->peakGust
      << " m/s at " << hours[strongest->peakIndex].time.ToIsoExtendedStr() << '\n';
  return sentence;
}

void append_range(Sentence& sentence, const Range& r, const SeaParams& params)
{
  sentence << PositiveRange(r.lower, r.upper, params.rangeSeparator)
           << *UnitFactory::create(MetersPerSecond);
}

// The direction of a turn as a qualified direction glyph, or the plain direction when no key is
// given. Variable winds are never qualified.
void append_direction(Sentence& sentence, const DirectionInfo& dir, const char* key)
{
  if (key != nullptr && !dir.variable)
    sentence << QualifiedDirectionPhrase(key, dir.turnPhrase, dir.phrase);
  else
    sentence << dir.phrase;
}

const char* turn_key(bool senseKnown, bool veering, const char* plainKey)
{
  if (!senseKnown)
    return plainKey;
  return (veering ? VEERING_DIRECTION_KEY : BACKING_DIRECTION_KEY);
}

}  // namespace

// ----------------------------------------------------------------------
/*!
 * \brief Generate story on wind at sea from the area mean wind
 *
 * \return The story
 */
// ----------------------------------------------------------------------

Paragraph WindStory::sea_overview() const
{
  try
  {
    MessageLogger log("WindStory::sea_overview");
    Paragraph paragraph;

    const SeaParams params = read_params(itsVar);
    log << "Period " << itsPeriod.localStartTime().ToIsoExtendedStr() << " - "
        << itsPeriod.localEndTime().ToIsoExtendedStr() << '\n';

    vector<ConvectiveCell> cells;
    vector<HourData> hours =
        collect_hours(itsVar, itsSources, itsArea, itsPeriod, params, cells, log);
    if (hours.empty())
      return paragraph;
    WeatherHistory& history = const_cast<WeatherArea&>(itsArea).history();
    WeekdayTools::forget_history_outside(history, itsPeriod);
    TimeWords timeWord(itsVar,
                       itsForecastTime,
                       WeekdayTools::day_phrase_preferences(itsVar, params.weekdays),
                       history);
    // The reader starts from the beginning of the forecast period: a phrase for an earlier
    // part of the day than that on the next day needs no marker
    if (history.latestDayPhasePhrase.empty())
      history.updateTimePhrase(
          "", timeWord.plain(itsPeriod.localStartTime(), false), itsPeriod.localStartTime());

    // 1. phases of the smoothed area mean
    vector<int> pts = turning_points(hours);
    remove_small_wiggles(pts, hours, params.speedChangeThreshold);
    sharpen_legs(pts, hours, params.speedChangeThreshold);
    vector<Phase> phases = build_phases(pts, hours, params);
    limit_changes(phases, hours, params);
    split_steady_phases_on_direction(phases, hours, itsVar, params, log);

    log << "Phases:\n";
    for (const auto& ph : phases)
      log << "  " << hours[ph.beg].time.ToIsoExtendedStr() << " - "
          << hours[ph.end].time.ToIsoExtendedStr() << "  " << phase_type_string(ph.type) << fixed
          << setprecision(1) << "  change=" << ph.change << '\n';

    // 2. sentences. A sentence about convective cells, when enabled, follows the sentence of
    //    the phase in which the strongest cell occurs so that the narrative stays chronological
    bool haveReported = false;
    DirectionInfo reportedDir;
    Range reportedRange;
    bool cellPending = params.cellReporting && !cells.empty();
    int cellAnchor = -1;  // the strongest cell
    for (size_t i = 0; cellPending && i < cells.size(); i++)
      if (cellAnchor < 0 || cells[i].peakGust > cells[cellAnchor].peakGust)
        cellAnchor = static_cast<int>(i);
    auto emit_cell_sentence = [&]()
    {
      paragraph << cell_sentence(hours, cells, params, timeWord, log);
      cellPending = false;
    };

    for (size_t p = 0; p < phases.size(); p++)
    {
      const Phase& ph = phases[p];
      const bool first = !haveReported;
      Sentence sentence;

      if (ph.type == PhaseType::Steady)
      {
        DirectionInfo dir = analyze_direction(itsVar, hours, ph.beg, ph.end, params);
        Range range = compute_range(hours, ph.beg, ph.end, params);
        log << "Steady phase: direction " << dir.phrase << ", range " << range.lower << "-"
            << range.upper << '\n';

        if (first)
        {
          sentence << TIME_DIRECTION_PHRASE << EMPTY_STRING << dir.phrase;
          append_range(sentence, range, params);
        }
        else
        {
          const bool dirChanged = direction_changed(reportedDir, dir, params);
          if (!dirChanged)
          {
            log << "Steady phase adds nothing new, skipped\n";
            continue;
          }
          if (dir.variable)
          {
            log << "Steady phase with variable wind adds nothing new, skipped\n";
            continue;
          }
          const bool rangeChanged = range.differs(reportedRange, params.rangeReportMinDifference);
          const string when = timeWord(hours[ph.beg].time, false);
          const bool haveReference = params.veeringBacking && !reportedDir.variable;
          const bool veering = haveReference && signed_turn(reportedDir.degrees, dir.degrees) > 0;
          if (!rangeChanged && !dir.variable && !dir.turnPhrase.empty())
          {
            const char* key = TIME_WIND_TURNS_PHRASE;
            if (haveReference)
              key = (veering ? TIME_WIND_VEERS_PHRASE : TIME_WIND_BACKS_PHRASE);
            sentence << key << when << dir.turnPhrase;
          }
          else
          {
            sentence << TIME_DIRECTION_PHRASE << when;
            append_direction(sentence, dir, turn_key(haveReference, veering, nullptr));
            if (rangeChanged)
              append_range(sentence, range, params);
          }
        }
        reportedDir = dir;
        reportedRange = range;
      }
      else
      {
        // direction at the start and at the end of the change. The end state is described by
        // the first hours after the change when a steady phase follows, otherwise by the last
        // hours of the change.
        const int startEnd = min(ph.end, ph.beg + params.rangeHours - 1);
        int rangeBeg = max(ph.beg, ph.end - (params.rangeHours - 1));
        int rangeEnd = ph.end;
        if (p + 1 < phases.size() && phases[p + 1].type == PhaseType::Steady)
        {
          rangeBeg = ph.end;
          rangeEnd = min(phases[p + 1].end, ph.end + params.rangeHours - 1);
        }
        DirectionInfo startDir = analyze_direction(itsVar, hours, ph.beg, startEnd, params);
        DirectionInfo dir = analyze_direction(itsVar, hours, rangeBeg, rangeEnd, params);
        Range endRange = compute_range(hours, rangeBeg, rangeEnd, params);
        const string changeWord =
            (ph.type == PhaseType::Strengthening ? STRENGTHENING_WORD : WEAKENING_WORD);
        const string rate = rate_word(ph, params);
        // the change is timed at the hour it completes when a steady phase follows, otherwise
        // at the middle of the hours the range describes
        const bool steadyFollows = (rangeBeg == ph.end && rangeEnd > ph.end);
        const TextGenPosixTime endTime =
            (steadyFollows ? hours[ph.end].time : hours[(rangeBeg + rangeEnd) / 2].time);
        // the wind at the first point of the leg still has the old value: the change starts after
        // it
        TextGenPosixTime startTime = hours[min(ph.end, ph.beg + 1)].time;
        const Range startRange = compute_range(hours, ph.beg, startEnd, params);
        log << "Change phase: " << changeWord << " " << rate << ", direction " << startDir.phrase
            << " -> " << dir.phrase << ", end range " << endRange.lower << "-" << endRange.upper
            << '\n';

        // Optionally a change from the start of the story is written as a steady sentence for
        // the hours before the range has moved, followed by a time phrased change sentence,
        // instead of one sentence with "aluksi". The single sentence is kept when the range
        // already moves within the range_hours the start range describes, or only at the end of
        // the change.
        bool splitInitial = false;
        if (first && params.separateInitialSentence && startRange != endRange)
        {
          const int changeStart = range_change_start(hours, ph.beg, ph.end, startRange, params);
          if (changeStart > startEnd && changeStart < ph.end)
          {
            Sentence steady;
            steady << TIME_DIRECTION_PHRASE << EMPTY_STRING << startDir.phrase;
            append_range(steady, startRange, params);
            paragraph << steady;
            reportedDir = startDir;
            reportedRange = startRange;
            haveReported = true;
            startTime = hours[changeStart].time;
            splitInitial = true;
            log << "Separate initial sentence, change starts " << startTime.ToIsoExtendedStr()
                << '\n';
          }
        }
        // A change from the start of the story that ends in a turn always starts with the
        // state at the start: "Koillistuulta 1-3 m/s. Vähitellen voimistuvaa, aamuyöksi etelään
        // kääntyvää tuulta 5-7 m/s." The change sentence then has no time phrase of its own.
        const bool tailHasDirection = !dir.variable && direction_changed(startDir, dir, params);
        bool startStateFirst = false;
        if (first && !splitInitial && tailHasDirection)
        {
          Sentence steady;
          steady << TIME_DIRECTION_PHRASE << EMPTY_STRING << startDir.phrase;
          append_range(steady, startRange, params);
          paragraph << steady;
          reportedDir = startDir;
          reportedRange = startRange;
          haveReported = true;
          startStateFirst = true;
          log << "State at the start in its own sentence, the change ends in a turn\n";
        }
        const bool singleInitial = first && !splitInitial && !startStateFirst;

        // Head of the sentence: the direction at the start of the change, unless it is variable
        // or has already been reported. The tail carries the end direction when it differs.
        string headDirWord = WIND_WORD;
        if (!tailHasDirection && !startDir.variable &&
            (singleInitial || direction_changed(reportedDir, startDir, params)))
          headDirWord = startDir.phrase;

        // When the tail carries the direction, the wind is named once, at the end: "vähitellen
        // heikkenevää, aamulla pohjoiseen kääntyvää tuulta"
        auto append_head = [&](Sentence& target, const string& time)
        {
          if (tailHasDirection)
            target << TIME_RATE_CHANGE_PHRASE << time << rate << changeWord;
          else
            target << TIME_RATE_CHANGE_DIRECTION_PHRASE << time << rate << changeWord
                   << headDirWord;
        };

        auto append_tail = [&](Sentence& target)
        {
          if (tailHasDirection)
          {
            // the tail of a change sentence names the turn and the time by which it has
            // happened, where the language does so: "aamuksi pohjoiseen kääntyvää tuulta"
            target << TIME_DIRECTION_PHRASE << timeWord.by(endTime);
            const bool senseKnown = params.veeringBacking && !startDir.variable;
            append_direction(target,
                             dir,
                             turn_key(senseKnown,
                                      signed_turn(startDir.degrees, dir.degrees) > 0,
                                      TURNING_DIRECTION_KEY));
          }
          else
            target << timeWord(endTime, false);
          append_range(target, endRange, params);
        };

        if (singleInitial)
        {
          append_head(sentence, EMPTY_STRING);
          if (startRange != endRange)
          {
            sentence << Delimiter(COMMA_PUNCTUATION_MARK) << INITIALLY_WORD;
            append_range(sentence, startRange, params);
          }
          sentence << Delimiter(COMMA_PUNCTUATION_MARK);
          append_tail(sentence);
        }
        else
        {
          if (timeWord.plain(startTime, false) == timeWord.plain(endTime, false) &&
              TimeTools::isSameDay(startTime, endTime) && !tailHasDirection)
          {
            // change within one part of the day
            append_head(sentence, timeWord(startTime, false));
            append_range(sentence, endRange, params);
          }
          else
          {
            append_head(sentence,
                        startStateFirst ? string(EMPTY_STRING) : timeWord(startTime, true));
            sentence << Delimiter(COMMA_PUNCTUATION_MARK);
            append_tail(sentence);
          }
        }
        reportedDir = dir;
        reportedRange = endRange;
      }

      paragraph << sentence;
      haveReported = true;
      if (cellPending && cells[cellAnchor].peakIndex <= ph.end)
        emit_cell_sentence();
    }
    if (cellPending)
      emit_cell_sentence();

    // 3. optional gust sentence on the area maximum gust. The hours of a convective cell only
    //    contribute the gusts outside the cell, so this describes widespread gusts.
    if (params.gustReporting)
    {
      int gustIndex = -1;
      for (size_t i = 0; i < hours.size(); i++)
        if (hours[i].gust != kFloatMissing &&
            (gustIndex < 0 || hours[i].gust > hours[gustIndex].gust))
          gustIndex = static_cast<int>(i);
      if (gustIndex >= 0 && hours[gustIndex].gust >= params.gustLimit)
      {
        Sentence sentence;
        const float gust = hours[gustIndex].gust;
        sentence << GUST_PHRASE << timeWord(hours[gustIndex].time, false)
                 << (gust >= MYRSKY_LOWER_LIMIT ? VERY_STRONG_GUSTS_WORD : STRONG_GUSTS_WORD)
                 << Integer(static_cast<int>(lround(gust)))
                 << *UnitFactory::create(MetersPerSecond);
        paragraph << sentence;
        log << "Gust sentence: maximum gust " << hours[gustIndex].gust << " m/s at "
            << hours[gustIndex].time.ToIsoExtendedStr() << '\n';
      }
    }

    log << paragraph;
    return paragraph;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
