# Story "wind_overview"

> **Status:** Primary. The flagship wind narrative — use it for almost
> every wind product.
>
> **Owner:** `WindStory::overview()`.
> **Implementation:** `textgen/wind_overview.cpp` (~3 959 LOC).
>
> This page reflects the 2011–2016 specification. The older 2005 design
> described a much simpler period-based algorithm; that design is gone.
> The current implementation uses time-series smoothing and change-point
> detection, as documented below.

## What it produces

`wind_overview` describes:

* the wind **direction and speed at the start** of the forecast period, and
* the **direction and speed changes** that occur during the period.

Examples:

* "Etelätuulta 5 m/s. Iltapäivällä tuuli kääntyy kakkoon."
* "Idästä 15…20 m/s ja puuskittaista itätuulta."
* "Vähitellen voimistuvaa lounaan puoleista tuulta, aluksi 4–6 m/s,
  aamupäivällä 7–10 m/s."
* "Aamupäivästä alkaen voimistuvaa etelänpuoleista tuulta, iltapäivällä
  6–11 m/s, ylimmillään 13 m/s."

## Wind speed

### How wind speed is reported

Wind speed is reported either as a **range** ("tuulihaarukka", e.g.
3…6 m/s) or as a **single value** (e.g. 5 m/s).

Two statistics are computed from the area:

* **Median** of the spatial mean wind speed — used as the main
  reported value / range anchor.
* **Warning value** (`varoitusarvo`) — the maximum 10-minute mean at
  editing time; used as the upper bound and gate for several phrase
  decisions.

When wind is **weak** (warning < 5 m/s), no speed or direction
**changes** are reported. The story just integrates direction and speed
over the whole period. The wind is then described either as
"suunnaltaan vaihtelevaa" or "N-puoleista heikkoa tuulta".

Gustiness is reported separately when it matters — see
[Puuskatuuli](#puuskatuuli) below.

### Rate-of-change phrases

When a speed change is reported, the story qualifies the *rate* of the
change:

| Condition | Phrase | Example |
| --- | --- | --- |
| Speed changes by at most 2.5 m/s | "vähän" | "illalla vähän voimistuvaa tuulta" |
| Change unfolds over at least 12 hours | "vähitellen" | "vähitellen voimistuvaa lounaan puoleista tuulta" |
| Change of at least 5 m/s within at most 6 hours | "nopeasti" | "iltapäivällä nopeasti voimistuvaa tuulta" |

### "alkaen" phrase

When a change period lasts at least 6 hours, the "alkaen" form is used:
"Aamupäivästä alkaen voimistuvaa etelänpuoleista tuulta, iltapäivällä
7–10 m/s, ylimmillään 13 m/s."

### Wind-speed classification

Verbal descriptions of wind strength use the following classes:

| Mean speed (m/s) | Class |
| --- | --- |
| < 0.5 | tyyntä (calm) |
| 0.5 – 3.5 | heikkoa (light) |
| 3.5 – 7.5 | kohtalaista (moderate) |
| 7.5 – 13.5 | navakkaa (fresh) |
| 13.5 – 20.5 | kovaa (strong) |
| 20.5 – 32.5 | myrskyä (storm) |
| ≥ 32.5 | hirmumyrskyä (hurricane) |

Example: "navakkaa kaakonpuoleista tuulta".

## Tuulihaarukka (wind-speed range)

Because wind speed usually varies across the area, the value is reported
as a range.

### Range size

| Parameter | Default | Effect |
| --- | --- | --- |
| `wind_speed_interval_min_size` | 2 m/s | Below this span (e.g. 10…12 m/s), collapse to a single value (spatial mean) |
| `wind_speed_interval_max_size` | 5 m/s | Above this span, cap the upper bound and add a "kovimmillaan" phrase |

If the range exceeds the maximum, the story reads e.g. "tuulen nopeus on
13…18 m/s, kovimmillaan 21 m/s".

### How the range is determined

In three stages:

1. **Lower bound.** Start from the median of the spatial mean wind
   speed. The initial lower bound is `median − 1 m/s`.
2. **Coverage check.** The reported range must contain at least 2/3 of
   the area's grid-point time series for the reporting period. If not,
   extend the range downward until 2/3 coverage is achieved.
3. **Upper bound.** If the maximum exceeds the interval's configured
   upper bound (`wind_speed_interval_max_size`), find the largest range
   still enclosing 2/3 of points and report the true maximum separately
   using "kovimmillaan N m/s" (where N is the maximum of the area's
   warning-value time series).

Worked example:

1. 5–17 m/s (initial range)
2. 6–14 m/s, kovimmillaan 17 m/s (2/3 of points fall in 6–14; the
   maximum is tagged)
3. 6–11 m/s, kovimmillaan 17 m/s (final reported range)

## Puuskatuuli (gusty wind)

The "puuskakeskiarvo" is the time-series mean of the area's maximum
gust values.

A puuska (gust) phrase is emitted when:

* the warning value exceeds 20 m/s, **and**
* `gust mean − range upper bound > gusty_wind_max_wind_difference`
  (default 5 m/s).

Example: if the range is "itätuulta 15…20 m/s" and the gust mean is
26 m/s, the sentence becomes "puuskittaista itätuulta".

## Wind direction

### How wind direction is reported

* A direction **change** is reported only if the change is at least 45°.
  If the speed change also causes a change point, the new direction is
  also mentioned.
* "Suunnaltaan vaihteleva tuuli" is used only when the warning value is
  at most 6.5 m/s. If the warning exceeds this and the direction standard
  deviation is over 45°, use "N-puoleista" instead.
* Four direction forms are possible:
  1. A specific compass direction (e.g. `pohjoistuulta`)
  2. Between two compass directions (e.g. `pohjoisen ja koillisen välistä`)
  3. "N-puoleista" (approximately N) — when deviation is 22.5° – 45°
  4. Variable — when deviation > 45° and warning ≤ 7 m/s
* When between two compass directions, the main compass direction is
  always reported first: "pohjoisen ja koillisen välistä tuulta", NOT
  "koillisen ja pohjoisen välistä tuulta".

### 8-way compass

| Direction (degrees) | Name |
| --- | --- |
| 337.5 – 22.5 | pohjoinen |
| 22.5 – 67.5 | koillinen |
| 67.5 – 112.5 | itä |
| 112.5 – 157.5 | kaakko |
| 157.5 – 202.5 | etelä |
| 202.5 – 247.5 | lounas |
| 247.5 – 292.5 | länsi |
| 292.5 – 337.5 | luode |

### 16-way compass

| Direction (degrees) | Name |
| --- | --- |
| 348.75 – 11.25 | pohjoinen |
| 11.25 – 33.75 | pohjoisen ja koillisen välinen |
| 33.75 – 56.25 | koillinen |
| 56.25 – 78.75 | idän ja koillisen välinen |
| 78.75 – 101.25 | itä |
| 101.25 – 123.75 | idän ja kaakon välinen |
| 123.75 – 146.25 | kaakko |
| 146.25 – 168.75 | etelän ja kaakon välinen |
| 168.75 – 191.25 | etelä |
| 191.25 – 213.75 | etelän ja lounaan välinen |
| 213.75 – 236.25 | lounas |
| 236.25 – 258.75 | lännen ja lounaan välinen |
| 258.75 – 281.25 | länsi |
| 281.25 – 303.75 | lännen ja luoteen välinen |
| 303.75 – 326.25 | luode |
| 326.25 – 348.75 | pohjoisen ja luoteen välinen |

### How direction is determined

1. **Mode direction.** Compute each compass direction's share of the
   area (e.g. pohjoinen 48%, koillinen 35%, itä 8%, …). The shares are
   weighted so that the chosen direction must cover at least 85% of the
   area.
2. **16 sectors are used** internally, so each sector spans 22.5°.
3. **Classification by deviation** of the mean direction:

| Deviation | Treatment |
| --- | --- |
| < 22.5° | Specific direction: "pohjoistuulta 3–5 m/s" |
| 22.5° – 45° | "N-puoleista" phrase: "pohjoisen puoleista tuulta" |
| ≥ 45° | Variable, unless warning > 7 m/s |

When the deviation is 22.5°–45°, the 8-way compass is used — we never
emit "lännen ja luoteen välisen tuulen suunnasta tuulta"; instead we
report "lännen tai luoteen suunnasta".

## Story construction

Five phases: raw data → time-series smoothing → change-point detection
→ event periods → story formation.

### 1. Raw data

For each hour in the forecast period the story reads:

* timestamp
* area maximum wind speed
* area minimum wind speed
* area mean speed + standard deviation
* area median of the mean speed
* area warning value
* area mean gust speed
* area mean direction + standard deviation

### 2. Time-series smoothing

The raw time series is smoothed so that short-term noise is removed but
longer-range trends remain visible. The algorithm walks the series in
windows of three consecutive forecast points (V0, V1, V2):

* Compute the interpolated forecast V1ᵢ for the middle point (predicted
  from V0 and V2).
* Compute the residual dᵢ = |V1ₒ − V1ᵢ| (original vs. interpolated).
* The hour with the smallest dᵢ below the configured maximum
  (`max_error_wind_speed`, default 2.0 m/s; `max_error_wind_direction`,
  default 10°) is removed from the time series.
* The pass is repeated until no point qualifies for removal.
* After that, removed hours are replaced by their interpolated values.
* Finally the algorithm restores any local maxima/minima inadvertently
  smoothed out, if they were significant change points.

**Exception for wind direction.** When smoothing direction, a point is
*not* removed if the direction flips by more than 180° across three
consecutive points (e.g. 355.1° → 1.5° — small numerical residual, but
physically the direction has not flipped).

### 3. Change-point detection

From the smoothed series' local maxima and minima, potential change
points are extracted:

| Parameter | Default | Controls |
| --- | --- | --- |
| `wind_speed_threshold` | 3.0 m/s | Minimum speed change to be reported |
| `wind_direction_threshold` | 45° | Minimum direction change to be reported |

When the wind is weak (< 5 m/s), speed and direction changes are not
treated as change points. The wind is reported as either "suunnaltaan
vaihteleva" or "N-puoleista heikkoa tuulta" over the whole period.

### 4. Event periods

The intervals between change points are **event periods**. Each event
period is classified as one of:

* tuuli voimistuu (wind strengthens)
* tuuli heikkenee (wind weakens)
* tuuli kääntyy (wind turns)
* tuuli muuttuu vaihtelevaksi (wind becomes variable)
* ei muutosta (no change)

When both speed and direction change simultaneously we speak of a
**combined event**: e.g. "tuuli voimistuu ja kääntyy etelään".

Example (smoothed + change points):

```
 ----VA------|-----PO---------|PO -->----IT    wind direction
 ----N---|----V---|---N---|--------H--------   wind speed
 ----1---|-2-|-3-|-4- -|--5--|---6--------    potential periods
```

Legend: VA = variable, PO = pohjoistuuli, IT = itätuuli, N = no speed
change, V = strengthens, H = weakens. Period 1 is weak variable; period 2
starts strengthening while still variable; period 3 is strengthening
pohjoistuuli; period 4 is pohjoistuuli that stops strengthening;
period 5 pohjoistuuli starts weakening; period 6 weakening continues
while direction turns to itätuuli.

After potential periods are identified they are merged:

* Weak-wind periods (< 5 m/s) merge.
* Consecutive periods where wind keeps turning in the same direction
  merge *if* speed trend is also consistent (both strengthening / both
  weakening / both flat).
* Consecutive strengthening or weakening periods merge if the direction
  does not change.
* A short (≤ 2 h) non-variable period between variable-direction
  periods merges with the variable neighbours.
* A short (≤ 2 h) last period where wind is still variable merges with
  the preceding period.
* Successive identical-state periods merge.
* Short periods with no change merge into the preceding period (e.g.
  rules 3 and 4 above).
* An individual event merges with the immediately following combined
  event ("tuuli kääntyy ja heikkenee" + "tuuli heikkenee").
* A short (≤ 2 h) period with no change at the very start of the
  forecast merges with what follows.

### 5. Story formation

**Main rules:**

* Report wind-direction and wind-speed changes detected in the forecast
  period.
* If no change is detected, report a single direction + speed integrated
  over the whole period.
* A direction change is reported when it is at least
  `wind_direction_threshold` (default 45°).
* A speed change is reported when it is at least
  `wind_speed_threshold` (default 3.0 m/s).

**Direction refinements:**

* The story starts with the direction and then only reports changes,
  e.g. "Etelätuulta 5 m/s. Iltapäivällä tuuli kääntyy kakkoon."
* When the wind is variable, changes are not prefixed with "tuuli
  kääntyy"; the new direction is simply introduced, e.g. "Iltapäivästä
  alkaen suunnaltaan vaihtelevaa tuulta 3–6 m/s. Illasta alkaen
  kaakkoistuulta."
* If the wind is variable for at least 3 hours, use "tuuli muuttuu
  tilapäisesti vaihtelevaksi".

**Speed refinements:**

* The story opens with the speed; subsequent sentences report only the
  changes, e.g. "Etelätuulta 5 m/s. Iltapäivällä eteläinen tuuli alkaa
  voimistua. Illalla tuuli lännen voimistuu edelleen 9–12 m/s."
* Speed changes may be qualified with "vähän", "nopeasti", or
  "vähitellen":
  * "vähän" when speed changes at most 2.5 m/s: "Iltapäivällä vähän
    voimistuvaa etelänpuoleista tuulta 4–8 m/s."
  * "nopeasti" when the change is at least 5 m/s within at most 6 hours:
    "Iltapäivällä nopeasti voimistuvaa etelätuulta 7–12 m/s."
  * "vähitellen" when the change unfolds over at least 12 hours:
    "Aamupäivästä alkaen lännen ja lounaan välinen tuuli alkaa voimistua
    vähitellen, yöllä 12–17 m/s, ylimmillään 19 m/s."

## Configuration parameters

All variables live under `textgen::[section]::story::wind_overview::*`.

| Parameter | Default | Meaning |
| --- | --- | --- |
| `wind_calc_top_share` | 80.0 % | Weight of peak wind in the calculated wind when the gust exceeds 10 m/s for over 10 % of the period |
| `wind_calc_top_share_e` | 50.0 % | Weight of peak wind otherwise, or when peak < 10 m/s for the whole period, or peak > 10 % of period |
| `wind_speed_interval_min_size` | 2 m/s | Minimum wind-speed range size |
| `wind_speed_interval_max_size` | 5 m/s | Maximum wind-speed range size |
| `wind_speed_threshold` | 3.0 m/s | Minimum speed change that is reported |
| `wind_speed_warning_threshold` | 11.0 m/s | Warning-value threshold above which the warning is used as the range upper bound |
| `wind_speed_top_coverage` | 98 % | 98 % of points must fall below the range upper bound (i.e. ≤ 2 % may be above); a localised strong wind therefore does not bloat the range |
| `wind_direction_threshold` | 45° | Minimum direction change that is reported |
| `wind_direction_min_speed` | 7.0 m/s | If mean speed exceeds this, report a direction even when it would otherwise be variable (deviation > 45°) |
| `max_error_wind_speed` | 2.0 | Smoothing tolerance for the speed time series (small = smooth lightly, large = smooth aggressively) |
| `max_error_wind_direction` | 10.0 | Smoothing tolerance for the direction time series |
| `gusty_wind_max_wind_difference` | 5.0 | Minimum difference (gust mean − range upper bound) that triggers the "puuskittaista" phrase |

## See also

* [`wind_anomaly`](wind_anomaly.md) — wind relative to climatology
* [`wind_daily_ranges`](wind_daily_ranges.md) — per-day summary (Legacy)
* [`wind_range`](wind_range.md) — single-sentence range (Trivial)
* [`wind_simple_overview`](wind_simple_overview.md) — superseded by this
  story
