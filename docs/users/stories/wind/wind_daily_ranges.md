# Story "wind_daily_ranges"

## Story form

The story describes a wind forecast for a one- or several-day period. The story is best suited to the first 1–3 days. The third day is the last one for which a forecast is reported.

The story does not use speed classes, to simplify the algorithm. Direction classes are used.

The general form is:

1. [Viikonpäivänä] [suuntatuulta] X-Y m/s
1. [Viikonpäivänä] [suuntatuulta] noin X m/s

This is repeated for each day forward. To avoid repetition, either the direction or the speed may be omitted. If both direction and speed are the same as the previous day's, either the whole day is dropped, or the previous sentence's time phrase becomes "starting from weekday".

Winds are reported only for day periods, which are configured with the variables:

```
textgen::[section]::story::wind_daily_ranges::day::starthour = [0-23] (6)
textgen::[section]::story::wind_daily_ranges::day::maxstarthour = [0-23] (starthour)
textgen::[section]::story::wind_daily_ranges::day::endhour = [0-23] (18)
textgen::[section]::story::wind_daily_ranges::day::minendhour = [0-23] (endhour)
```

Typical settings for farmers, for example, are:

```
textgen::[section]::story::wind_daily_ranges::day::starthour = 4
textgen::[section]::story::wind_daily_ranges::day::maxstarthour = 12
textgen::[section]::story::wind_daily_ranges::day::endhour = 21
textgen::[section]::story::wind_daily_ranges::day::minendhour = 18
```

The day is thus stretched to the needs of farmers.

The wind interval is collapsed to a single number (the mean) if the interval is smaller than:

```
textgen::[section]::story::wind_daily_ranges::mininterval = [0-] (0)
```

## Time phrases

The first day's time phrase is of type [today](../../period_phrases.md#today), subsequent days' phrases of type [next_day](../../period_phrases.md#next_day). The corresponding variables are:

```
textgen::[section]::story::wind_daily_ranges::today::phrases
textgen::[section]::story::wind_daily_ranges::next_day::phrases
```

As a special case, if two days are merged at the end, the variable

```
textgen::[section]::story::wind_daily_ranges::next_days::phrases
```

is used.

## Wind speed

The speed range is obtained by computing the minimum and maximum of the time means.

Two wind speeds are considered the same if the minimums and maximums differ by at most the amounts below:

```
textgen::[section]::story::wind_daily_ranges::same::minimum = [0-] (0)
textgen::[section]::story::wind_daily_ranges::same::maximum = [0-] (0)
```

Multiple speeds are considered the same if all pairs are considered the same. The common speed interval is obtained by taking the minimum and maximum of the intervals' bounds as the new interval bounds.

## Wind direction

The wind direction for each time interval is obtained by computing the mean direction over time and space. Based on the direction's quality index, the direction's accuracy is first weakened to "N-puoleista tuulta" (approximately N) and then to "suunnaltaan vaihtelevaa tuulta" (variable direction).

The required quality (direction's standard deviation) is controlled by:

```
textgen::[section]::story::wind_daily_ranges::direction::accurate (= 22.5)
textgen::[section]::story::wind_daily_ranges::direction::variable (= 45)
```

Defaults are given in parentheses.

The direction phrase is chosen as follows:

1. If quality < `accurate`, "N-tuulta"
1. If quality >= `variable`, "N-puoleista tuulta"
1. Otherwise, "suunnaltaan vaihtelevaa tuulta"

Two wind directions are never compared directly with each other, because rounding to an intermediate direction has caused significant loss of accuracy. Instead, the direction is computed for two or more days as a whole, and the resulting quality is checked.

## Testing results

For each day period, the regional minimum, mean, and maximum are computed from the point-wise time means. These results can be faked for testing with:

```
textgen::[section]::story::wind_daily_ranges::fake::day1::speed::mean = [result]
textgen::[section]::story::wind_daily_ranges::fake::day1::speed::minimum = [result]
textgen::[section]::story::wind_daily_ranges::fake::day1::speed::maximum = [result]
textgen::[section]::story::wind_daily_ranges::fake::day1::direction::mean = [result]
...
```

In addition, there are special cases for days 1–2, 2–3, and 1–3:

```
textgen::[section]::story::wind_daily_ranges::fake::days1-2::speed::mean = [result]
textgen::[section]::story::wind_daily_ranges::fake::days1-2::speed::minimum = [result]
textgen::[section]::story::wind_daily_ranges::fake::days1-2::speed::maximum = [result]
textgen::[section]::story::wind_daily_ranges::fake::days1-2::direction::mean = [result]

textgen::[section]::story::wind_daily_ranges::fake::days2-3::speed::mean = [result]
textgen::[section]::story::wind_daily_ranges::fake::days2-3::speed::minimum = [result]
textgen::[section]::story::wind_daily_ranges::fake::days2-3::speed::maximum = [result]
textgen::[section]::story::wind_daily_ranges::fake::days2-3::direction::mean = [result]

textgen::[section]::story::wind_daily_ranges::fake::days1-3::speed::mean = [result]
textgen::[section]::story::wind_daily_ranges::fake::days1-3::speed::minimum = [result]
textgen::[section]::story::wind_daily_ranges::fake::days1-3::speed::maximum = [result]
textgen::[section]::story::wind_daily_ranges::fake::days1-3::direction::mean = [result]
```

## Algorithm

1. If there is 1 day: [Tänään] [itätuulta] [X-Y m/s]
1. If there are 2 days:
   1. If D(D1,D2) is good quality or D1,D2 are variable:
      1. If W1==W2, [Tänään ja huomenna] [itätuulta] [X-Y m/s]
      1. If W1<>W2, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [X-Y m/s]
   1. If D(D1,D2) is not good quality:
      1. If W1==W2, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [länsituulta]
      1. If W1<>W2, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [länsituulta] [X-Y m/s]
1. Otherwise there are 3 days (additional days are ignored):
   1. If D(D1,D2,D3) is good quality or D1,D2,D3 are variable:
      1. If W1==W2==W3, [Viikonpäivästä alkaen] [itätuulta] [X-Y m/s]
      1. If W1==W2<>W3, [Itätuulta] [X-Y m/s], viikonpäivänä [X-Y m/s]
      1. If W1<>W2==W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivästä alkaen] [X-Y m/s]
      1. If W1<>W2<>W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [X-Y m/s], [viikonpäivänä] [X-Y m/s]
   1. If D(D1,D2) is good quality or D1,D2 are variable:
      1. If W1==W2==W3, [Itätuulta] [X-Y m/s], viikonpäivänä [länsituulta]
      1. If W1==W2<>W3, [Itätuulta] [X-Y m/s], viikonpäivänä [länsituulta] [X-Y m/s]
      1. If W1<>W2==W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [X-Y m/s], [viikonpäivänä] [länsituulta]
      1. If W1<>W2<>W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [X-Y m/s], [viikonpäivänä] [länsituulta] [X-Y m/s]
   1. If D(D2,D3) is good quality or D2,D3 are variable:
      1. If W1==W2==W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivästä alkaen] [länsituulta]
      1. If W1==W2<>W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [länsituulta], [viikonpäivänä] [X-Y m/s]
      1. If W1<>W2==W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivästä alkaen] [länsituulta] [X-Y m/s]
      1. If W1<>W2<>W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [länsituulta] [X-Y m/s], [viikonpäivänä] [X-Y m/s]
   1. Otherwise D1<>D2 and D2<>D3 (but not necessarily D1<>D3):
      1. If W1==W2==W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [länsituulta], [viikonpäivänä] [etelätuulta]
      1. If W1==W2<>W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [länsituulta], [viikonpäivänä] [etelätuulta] [X-Y m/s]
      1. If W1<>W2==W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [länsituulta] [X-Y m/s], [viikonpäivänä] [etelätuulta]
      1. If W1<>W2<>W3, [Tänään] [itätuulta] [X-Y m/s], [viikonpäivänä] [länsituulta] [X-Y m/s], [viikonpäivänä] [etelätuulta] [X-Y m/s]

The first day's type is [today](../../period_phrases.md#today), subsequent days' type is [next_day](../../period_phrases.md#next_day) or [next_days](../../period_phrases.md#next_days). If all days are merged, the [days](../../period_phrases.md#days) type is used.

Note that in certain cases a whole day is skipped because the direction and speed were the same as the previous day's. In those cases the weekday name _must_ be mentioned for the next day. In those places `viikonpäivänä` is marked without optional brackets.

Additionally, some meteorologists have a convention of placing the wind speed on the latter day if two days' speeds are equal. This could be implemented as an option in the future if desired. Care must then be taken with three-day special cases, e.g. when all three days' speeds are the same.

## Phrases required

Phrases required:

* "m/s"
* "noin"
* "metriä sekunnissa"
* "tuulta"
* "suunnaltaan vaihtelevaa"
* "N-tuulta", N=1–8
* "N-tuuleksi", N=1–8
* "N-puoleista tuulta", N=1–8
* "N-puoleiseksi tuuleksi", N=1–8

Additionally, the period phrases [today](../../period_phrases.md#today), [next_day](../../period_phrases.md#next_day), and [next_days](../../period_phrases.md#next_days) are required.
