# Configuration layout

TextGen reads its configuration through [`libconfig`][libconfig] via
`calculator/Settings.h`. Everything is keyed under the `textgen::` namespace.

[libconfig]: https://hyperrealm.github.io/libconfig/

## Top-level shape

`TextGenerator::generate()` drives the whole pipeline from a single variable:

```
textgen::sections = "morning, afternoon"
```

Each name in that list becomes a **section** (a paragraph in the output
document). For each section `X`, the generator expects:

```
textgen::X::period   = ...       # how to build the WeatherPeriod
textgen::X::header   = ...       # (optional) what header/title to emit
textgen::X::content  = "story1, story2, ..."   # named stories to generate
```

### Minimal example

```
textgen::sections = today

textgen::today::period::type     = until
textgen::today::period::days     = 0
textgen::today::period::endhour  = 18

textgen::today::header = morning
textgen::today::content = temperature_mean, precipitation_total
```

Produces a one-paragraph document with a "morning" header, a mean temperature
sentence, and a total precipitation sentence.

## Periods

Under `textgen::X::period::*` you specify how to build the
`WeatherPeriod` for section `X`. The full list of period types is documented
in [periods.md](periods.md). The most common types are:

| Type | Meaning |
| --- | --- |
| `now` | Period whose start and end equal the forecast time (useful with `report_*` headers). |
| `until` | From forecast time to a fixed end hour, possibly days later. |
| `from_until` | Both start and end are offsets from the forecast time. |

## Subperiods

A section can be split into days:

```
textgen::today::subperiods = true
textgen::today::subperiod::day::starthour    = 6
textgen::today::subperiod::day::endhour      = 6
textgen::today::subperiod::day::maxstarthour = 24
textgen::today::subperiod::day::minendhour   = 18
```

Each day looks up its stories from `textgen::today::day<N>::content` if set,
falling back to `textgen::today::content` otherwise. See
[subperiods.md](subperiods.md).

## Stories

Each story named in `::content` consumes a subtree of variables under

```
textgen::<section>::story::<story_name>::...
```

For example, `precipitation_daily_sums` reads:

```
textgen::<section>::story::precipitation_daily_sums::minrain      = 0.2
textgen::<section>::story::precipitation_daily_sums::mininterval  = 1
textgen::<section>::story::precipitation_daily_sums::today::phrases    = today
textgen::<section>::story::precipitation_daily_sums::next_day::phrases = followingday
```

The specific variables that each story reads are documented in the per-story
pages under [stories/](stories/README.md).

### Day / night definitions

Many stories need to know what "day" or "night" means. The convention is:

```
...::<period>::starthour     # earliest start hour
...::<period>::endhour       # latest end hour
...::<period>::maxstarthour  # flex: if the forecast period is short,
                             # starthour may be relaxed up to this
...::<period>::minendhour    # flex: endhour may be relaxed down to this
```

If the flex limits are omitted, no flexing is allowed — the day/night period
must fit exactly.

## Fake values (testing / debugging)

Almost every story supports a `fake::` subtree that bypasses the analyser
and injects a fixed value into the generated text. This is how regression
tests work.

```
textgen::<section>::story::temperature_mean::fake::mean = 12
```

The fake keys available are listed in each story's documentation.

## Phrases

Phrase lookup resolves tokens like `"päivän ylin lämpötila"` to translated
strings via the configured dictionary. The dictionary is typically
initialized globally at program start — see
[programmers/dictionaries.md](../programmers/dictionaries.md).

## Quality and automatic period splitting

When a story's analysis produces poor quality, the framework may split the
period automatically. See [quality.md](quality.md) for the rules and the
variables that control it.
