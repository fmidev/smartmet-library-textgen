# Story "temperature_day"

The `temperature_day` story has the form:

1. ""
1. "Lämpötila on [viikonpäivänä] [x...y] astetta, [sivulause]"
1. "Päivän ylin lämpötila on [viikonpäivänä] [x...y] astetta, [sivulause]. Yön alin lämpötila on [viikonpäivän vastaisena yönä] [x...y] astetta, [sivulause]."

where

* the phrase [x..y] can be of the form "X...Y" or "about X"
* the day and/or night period may be missing depending on the clock time and period length
* the subordinate clause is included if the temperature on the coast is significantly different

A possible subordinate clause has the form:

1. "rannikolla [x...y] astetta"
1. "rannikolla huomattavasti korkeampi"
1. "rannikolla korkeampi"
1. "rannikolla hieman korkeampi"
1. "rannikolla suunnilleen sama"
1. "rannikolla hieman alempi"
1. "rannikolla alempi"
1. "rannikolla huomattavasti alempi"

The sentence is repeated as many times as there are day/night periods in the data.

Variables controlling phrase selection:

```
textgen::[section]::story::temperature_day::day_night_limit = [N] (=3)

textgen::[section]::story::temperature_day::coast_numeric_limit = [N] (=8)
textgen::[section]::story::temperature_day::coast_not_below = [N] (=-15)

textgen::[section]::story::temperature_day::coast_limit = [N] (=2)

textgen::[section]::story::temperature_day::comparison::significantly_higher = [N]
textgen::[section]::story::temperature_day::comparison::higher = [N]
textgen::[section]::story::temperature_day::comparison::somewhat_higher = [N]
textgen::[section]::story::temperature_day::comparison::somewhat_lower = [N]
textgen::[section]::story::temperature_day::comparison::lower = [N]
textgen::[section]::story::temperature_day::comparison::significantly_lower = [N]
```

Algorithm:

1. If the difference between the day's maximum and the night's minimum temperature is less than `day_night_limit`, the day and night are not reported separately. However, the coast may still be reported separately.
1. If the difference between inland and coast is at least `coast_numeric_limit`, both areas' temperatures are given as numbers. This provides more accurate information about significant temperature differences.
1. If the area's temperature is below `coast_not_below`, coast and inland are not reported separately.
1. If the difference is at least `coast_limit`, the temperatures are compared in words according to the `comparison` variable limits.

Period phrases are of type [today](../../period_phrases.md#today) and [tonight](../../period_phrases.md#tonight). Related settings are controlled with:

```
textgen::[section]::story::temperature_day::today::phrases
textgen::[section]::story::temperature_day::tonight::phrases
```

The choice between a single value and an interval is based on:

```
textgen::[section]::story::temperature_day::mininterval = [1-] (=2)
textgen::[section]::story::temperature_day::always_interval_zero = [bool] (=false)
```

If the interval length is at least `mininterval`, an interval is reported; otherwise the mean. As an exception, if zero falls in the interval and `always_interval_zero` is true, an interval is always reported.

The day and night are defined by the variables:

```
textgen::[section]::story::temperature_day::day::starthour = [0-23]
textgen::[section]::story::temperature_day::day::endhour = [0-23]
textgen::[section]::story::temperature_day::day::maxstarthour = [0-23]
textgen::[section]::story::temperature_day::day::minendhour = [0-23]

textgen::[section]::story::temperature_day::night::starthour = [0-23]
textgen::[section]::story::temperature_day::night::endhour = [0-23]
textgen::[section]::story::temperature_day::night::maxstarthour = [0-23]
textgen::[section]::story::temperature_day::night::minendhour = [0-23]
```

The definitions could be, for example:

```
textgen::[section]::story::temperature_day::day::starthour    = 6
textgen::[section]::story::temperature_day::day::endhour      = 21
textgen::[section]::story::temperature_day::day::maxstarthour = 13
textgen::[section]::story::temperature_day::day::minendhour   = 21

textgen::[section]::story::temperature_day::night::starthour    = 18
textgen::[section]::story::temperature_day::night::endhour      = 8
textgen::[section]::story::temperature_day::night::maxstarthour = 23
textgen::[section]::story::temperature_day::night::minendhour   = 6
```

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::temperature_day::fake::day<N>::area::mean = [result]
textgen::[section]::story::temperature_day::fake::day<N>::area::minimum = [result]
textgen::[section]::story::temperature_day::fake::day<N>::area::maximum = [result]

textgen::[section]::story::temperature_day::fake::day<N>::coast::mean = [result]
textgen::[section]::story::temperature_day::fake::day<N>::coast::minimum = [result]
textgen::[section]::story::temperature_day::fake::day<N>::coast::maximum = [result]

textgen::[section]::story::temperature_day::fake::day<N>::inland::mean = [result]
textgen::[section]::story::temperature_day::fake::day<N>::inland::minimum = [result]
textgen::[section]::story::temperature_day::fake::day<N>::inland::maximum = [result]

textgen::[section]::story::temperature_day::fake::night<N>::area::mean = [result]
textgen::[section]::story::temperature_day::fake::night<N>::area::minimum = [result]
textgen::[section]::story::temperature_day::fake::night<N>::area::maximum = [result]

textgen::[section]::story::temperature_day::fake::night<N>::coast::mean = [result]
textgen::[section]::story::temperature_day::fake::night<N>::coast::minimum = [result]
textgen::[section]::story::temperature_day::fake::night<N>::coast::maximum = [result]

textgen::[section]::story::temperature_day::fake::night<N>::inland::mean = [result]
textgen::[section]::story::temperature_day::fake::night<N>::inland::minimum = [result]
textgen::[section]::story::temperature_day::fake::night<N>::inland::maximum = [result]
```

Phrases used:

* "päivän ylin lämpötila"
* "on"
* "noin"
* "astetta"
* "huomattavasti korkeampi"
* "korkeampi"
* "hieman korkeampi"
* "suunnilleen sama"
* "hieman alempi"
* "alempi"
* "huomattavasti alempi"
* "rannikolla"

plus the period phrases [today](../../period_phrases.md#today) and [tonight](../../period_phrases.md#tonight).
