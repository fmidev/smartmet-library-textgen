# Story "temperature_max36hours"

> **Status:** Primary. The flagship temperature narrative — use it for
> almost every modern temperature product.
>
> **Owner:** `TemperatureStory::max36hours()`.
> **Implementation:** `textgen/temperature_max36hours.cpp` (~4 242 LOC).
>
> This page is derived by source-mining — there is no separate
> specification document. When the source and this page disagree, the
> source wins; please fix the page.

## What it produces

A multi-sentence temperature narrative covering up to a ~36-hour
forecast, split into day and night sub-periods.  Example sentences:

* "Päivän ylin lämpötila on noin 15 astetta, yön alin lämpötila noin
  7 astetta."
* "Päivälämpötila on tänään 13…17 astetta, rannikolla noin 20 astetta,
  huomenna pari astetta korkeampi."
* "Lämpötila on tänään nollan tienoilla, öisin pikkupakkasta."

The story also integrates a **frost sub-story** (via `FrostStory`) when
configured to do so.

## How it decomposes the period

A `NightAndDayPeriodGenerator` turns the requested forecast period into
one of five **processing orders**, depending on where the forecast time
falls within the day/night rhythm:

| Processing order | Description |
| --- | --- |
| `DAY1` | Day 1 only |
| `DAY1_NIGHT` | Day 1 then the following night |
| `DAY1_DAY2_NIGHT` | Day 1, day 2, and the night between them |
| `NIGHT_DAY2` | The coming night then day 2 |
| `NIGHT` | Night only |

Each produced sub-period is then further split into **inland**,
**coast**, and **full-area** variants if the analysis suggests the
coast vs. inland numbers differ enough to mention separately.

## Coast vs. inland split

For each sub-period the story decides whether to emit a shared sentence
for the whole area, or to split the sentence into "rannikolla X,
sisämaassa Y". The split is triggered when the inland–coast difference
(of min, mean, or max) exceeds:

```
textgen::[section]::story::temperature_max36hours::temperature_limit_coast_inland = [N]
```

Default is **3 °C**. Below `-15 °C` the coast/inland split is
suppressed (`COASTAL_AREA_IGNORE_LIMIT` = −15.0 in the source) because
the distinction is rarely interesting at low temperatures.

## Area splitting (horizontal / vertical)

If the forecast area is named in a specific config list, the story can
**split the area in half** and produce two sequential sentences. The
split is driven by `check_area_splitting()` in
`WeatherForecast.cpp` and reads:

```
textgen::<section>::story::temperature_max36hours::areas_to_split = area1,area2,...
textgen::split_the_area::<area>::method = horizontal[:latitude]
textgen::split_the_area::<area>::method = vertical[:longitude]
```

If the split method is given but no specific latitude/longitude is
supplied, the area is split through its centre.

With horizontal splitting the paragraph reads "southern part …" then
"northern part …"; with vertical splitting "western part …" then
"eastern part …". The same mechanism is also used by
[`temperature_anomaly`](temperature_anomaly.md) and
[`weather_forecast`](../weather/weather_forecast.md).

## Frost sub-story integration

When the configured `frost_story` variable is set, its output is
produced by `FrostStory` and appended in the paragraph:

```
textgen::[section]::story::temperature_max36hours::frost_story = frost_onenight
```

The target story name must be one recognised by
[`FrostStory::hasStory`](../frost/frost_onenight.md).

## Seasonal adjustment

The story reads `textgen::units::celsius::format` to control how
temperature numbers are rendered:

| Value | Effect |
| --- | --- |
| `SI` (default) | `Integer + UnitFactory("DegreesCelsius")` → "13 °C" |
| `phrase` | A Finnish word after the integer (chosen by temperature-phrase id) |
| `none` | Just the integer, no unit |

Additionally the story internally tracks a **season id** (`SUMMER` vs.
`WINTER`) and uses it to pick between warmer-season phrases
("hieman lauhempaa") and colder-season phrases ("hieman lämpimämpää"
style variants). The season is inferred from the forecast time via
`SeasonTools`.

## Phrase vocabulary

The story can emit a rich phrase vocabulary, internally enumerated as
`temperature_phrase_id`. Highlights:

| Phrase id | English gloss |
| --- | --- |
| `NOIN_ASTETTA` | "about N degrees" |
| `TIENOILLA_ASTETTA` | "around N degrees" |
| `LAHELLA_ASTETTA` | "close to N degrees" |
| `TUNTUMASSA_ASTETTA` | "in the vicinity of N degrees" |
| `VAJAAT_ASTETTA` | "a bit under N degrees" |
| `VAHAN_YLI_ASTETTA` | "a little over N degrees" |
| `LAMPOTILA_VALILLA` | "temperature between X and Y" |
| `NOLLAN_TIENOILLA` | "around zero" |
| `VAHAN_NOLLAN_ALAPUOLELLA` | "slightly below zero" |
| `VAHAN_PLUSSAN_PUOLELLA` | "slightly above zero" |
| `PIKKUPAKKASTA` | "light frost" |
| `LAMPOTILA_NOUSEE` | "temperature rises" |
| `SUUNNILLEEN_SAMA` | "about the same" |
| `HIEMAN_KORKEAMPI` / `HIEMAN_ALEMPI` | "slightly higher" / "slightly lower" |
| `HIEMAN_LAUHEMPAA` / `HIEMAN_KYLMEMPAA` | "slightly milder" / "slightly colder" |
| `HIEMAN_HEIKOMPAA` / `HIEMAN_KIREAMPAA` | (frost) "slightly weaker" / "slightly sharper" |

The decision about which phrase to use is driven by the value itself,
its rounding precision, and the adjacent sub-period's mean (for
comparative phrases).

## Morning / afternoon split

For long enough day periods the story can split a day into **morning**
(`DAY1_MORNING_PERIOD`) and **afternoon** (`DAY1_AFTERNOON_PERIOD`)
halves when the two halves differ by more than
`MORNING_AFTERNOON_SEPARATION_LIMIT` (3.0 °C in the source). The
`specify_part_of_the_day` variable disables this behaviour:

```
textgen::[section]::story::temperature_max36hours::specify_part_of_the_day = false
```

## Single value vs. interval

```
textgen::[section]::story::temperature_max36hours::mininterval = [N] (default 2)
textgen::[section]::story::temperature_max36hours::always_interval_zero = [bool] (default false)
textgen::[section]::story::temperature_max36hours::rangeseparator = [string] (default "...")
```

Standard rules: if (max − min) ≥ `mininterval`, report as an interval;
otherwise collapse to the mean. If `always_interval_zero` is true and
zero falls inside the interval, always report as an interval so the
sign is explicit ("−1…+2 astetta").

## Configuration parameters

All settings live under `textgen::[section]::story::temperature_max36hours::*`.

| Parameter | Default | Meaning |
| --- | --- | --- |
| `temperature_limit_coast_inland` | 3 °C | Minimum inland–coast difference to split the sub-sentence |
| `separate_coastal_area_percentage` | (source) | Minimum share of the area that must be coast for a coast sub-clause to be emitted |
| `specify_part_of_the_day` | true | If false, do not split a day into morning / afternoon |
| `mininterval` | 2 °C | Collapse a range narrower than this to a single "about" value |
| `always_interval_zero` | false | Always render as an interval when it contains zero |
| `rangeseparator` | `...` | Separator between min and max inside a range |
| `frost_story` | — | Name of a `FrostStory` method to append (e.g. `frost_onenight`) |
| `areas_to_split` | — | Comma-separated list of area names that should be split in half |

Additionally the story honours the global
`textgen::units::celsius::format` setting (SI / phrase / none).

### Internal constants

The following are source-level constants, not user-configurable:

| Constant | Value | Meaning |
| --- | --- | --- |
| `COASTAL_AREA_IGNORE_LIMIT` | −15.0 °C | Below this the coast/inland split is suppressed |
| `DAY_NIGHT_SEPARATION_LIMIT` | 3.0 °C | Minimum day/night difference to split |
| `MORNING_AFTERNOON_SEPARATION_LIMIT` | 3.0 °C | Minimum morning/afternoon difference to split |

## Fake values

The story exposes many `fake::*` keys for regression-testing individual
branches of the algorithm. Because the per-sub-period matrix is large
(day1/day2/night × inland/coast/full × min/max/mean, and optionally
morning/afternoon), the authoritative list is in
`test/TemperatureStoryTest.cpp`. Search for `temperature_max36hours`
there — every test configuration enumerates exactly the keys it needs.

Representative keys:

```
textgen::<section>::story::temperature_max36hours::fake::day1::mean    = [result]
textgen::<section>::story::temperature_max36hours::fake::day1::min     = [result]
textgen::<section>::story::temperature_max36hours::fake::day1::max     = [result]
textgen::<section>::story::temperature_max36hours::fake::day1::coast::mean = [result]
textgen::<section>::story::temperature_max36hours::fake::day1::inland::mean = [result]
textgen::<section>::story::temperature_max36hours::fake::night::mean   = [result]
textgen::<section>::story::temperature_max36hours::fake::day2::mean    = [result]
textgen::<section>::story::temperature_max36hours::fake::area_percentage = [result]
```

## See also

* [`temperature_anomaly`](temperature_anomaly.md) — climatology-relative
  form of the same idea.
* [`frost_onenight`](../frost/frost_onenight.md) — the usual target of
  `frost_story`.
* [`temperature_day`](temperature_day.md) — simpler day+night story
  without climatology or area splitting.
