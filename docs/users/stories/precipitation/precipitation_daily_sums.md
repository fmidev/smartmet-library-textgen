# Story "precipitation_daily_sums"

The `precipitation_daily_sums` story has the form:

1. "Sadesumma on tänään X-Y millimetriä, huomenna X-Y millimetriä, ..."
1. "Sadesumma on tänään aamupäivästä alkaen X-Y millimetriä, huomenna X-Y millimetriä, ..."
1. "Sadesumma on tänään X-Y millimetriä, huomenna sama, ..."

In Swedish:

1. "Nederbördssumman är i dag X-Y millimeter, i morgon X-Y millimeter, ..."
1. "Nederbördssumman är i dag från förmiddagen X-Y millimeter, i morgon X-Y millimeter, ..."
1. "Nederbördssumman är i dag X-Y millimeter, i morgon densamma, ..."

In English:

1. "Total precipitation is today X-Y millimeters, tomorrow X-Y millimeters, ..."
1. "Total precipitation is today from before noon X-Y millimeters, tomorrow X-Y millimeters, ..."
1. "Total precipitation is today X-Y millimeters, tomorrow the same, ..."

Phrases used:

* "sadesumma"
* "on"
* "millimetriä"

plus period phrases.

Hourly precipitation amounts that are too small are in principle not part of the total. They can be filtered out with:

```
textgen::[section]::story::precipitation_daily_sums::minrain = [0-X]
```

The precipitation sum is reported as a mean if the difference between minimum and maximum is less than:

```
textgen::[section]::story::precipitation_daily_sums::mininterval = [0-X] (=1)
```

If the mean is zero but the maximum differs from zero, the sum is always reported as an interval 0-X. If the maximum is also zero, the day's precipitation sum is not reported at all when:

```
textgen::[section]::story::precipitation_daily_sums::ignore_fair_days = [bool] (=true)
```

is `true`.

The day is defined by the variables:

```
textgen::[section]::story::precipitation_daily_sums::day::starthour = [0-23] (6)
textgen::[section]::story::precipitation_daily_sums::day::maxstarthour = [0-23] (starthour)
textgen::[section]::story::precipitation_daily_sums::day::endhour = [0-23] (18)
textgen::[section]::story::precipitation_daily_sums::day::minendhour = [0-23] (endhour)
```

Two consecutive periods' precipitation sums are considered equal within sufficient tolerance if the minimums and maximums differ by at most the amounts below:

```
textgen::[section]::story::precipitation_daily_sums::same::minimum = [0-] (0)
textgen::[section]::story::precipitation_daily_sums::same::maximum = [0-] (0)
```

In practice, the algorithm uses this simplification only for the second forecast day of the whole period.

The first day's period phrase is of type [today](../../period_phrases.md#today), subsequent days' phrases are of type [next_day](../../period_phrases.md#next_day). The corresponding variables are:

```
textgen::[section]::story::precipitation_daily_sums::today::phrases
textgen::[section]::story::precipitation_daily_sums::next_day::phrases
```

The generated analysis functions can be overridden as follows:

```
1. Mean(Sum(Precipitation)):
textgen::[section]::story::precipitation_daily_sums::fake::day1::mean = [result]
textgen::[section]::story::precipitation_daily_sums::fake::day1::minimum = [result]
textgen::[section]::story::precipitation_daily_sums::fake::day1::maximum = [result]
textgen::[section]::story::precipitation_daily_sums::fake::day2::mean = [result]
textgen::[section]::story::precipitation_daily_sums::fake::day2::minimum = [result]
textgen::[section]::story::precipitation_daily_sums::fake::day2::maximum = [result]
```
