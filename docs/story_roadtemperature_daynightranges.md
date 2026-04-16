# Story "roadtemperature_daynightranges"

The `roadtemperature_daynightranges` story has the form:

1. Tienpintalämpötila on tänään päivällä x...y astetta, yöllä x...y astetta, huomenna päivällä x...y astetta, yöllä x...y astetta, etc.
1. Tienpintalämpötila on ensi yönä x...y astetta, huomenna päivällä x...y astetta, yöllä x...y astetta, etc.

In the first element — either day or night — the road surface temperature is mentioned for the first and only time; subsequently only temperature intervals are listed.

In Swedish, the sentences are:

1. "Vägtemperaturen är i dag x...y grader, på natten x...y grader, i morgon på dagen x...y grader, på natten x...y grader, etc."
1. "Vägtemperaturen är nästa natten x...y grader, i morgon på dagen x...y grader, på natten x...y grader, etc."

The story reports at most:

```
textgen::[section]::story::roadtemperature_daynightranges::maxperiods = [1-N] (=3)
```

day/night periods. The default is 3, because the road weather model is a rather short forecast. Three periods cover roughly 36 hours, which should be a sufficiently short time. As a safeguard, if the required minima and maxima cannot be computed for some period, the story is interrupted at that period. This ensures at least 2 periods are produced if, for example, one HIRLAM run is missed.

Period phrases are of type [today](period_phrases.md#today) and [tonight](period_phrases.md#tonight). Their settings are controlled with:

```
textgen::[section]::story::roadtemperature_daynightranges::today::phrases
textgen::[section]::story::roadtemperature_daynightranges::tonight::phrases
```

The choice between a single value and an interval is based on:

```
textgen::[section]::story::roadtemperature_daynightranges::mininterval = [1-] (=2)
textgen::[section]::story::roadtemperature_daynightranges::always_interval_zero = [bool] (=false)
```

If the interval length is at least `mininterval`, an interval is reported; otherwise the mean. As an exception, if zero falls in the interval and `always_interval_zero` is true, an interval is always reported.

The day and night are defined by the variables:

```
textgen::[section]::story::roadtemperature_daynightranges::day::starthour = [0-23]
textgen::[section]::story::roadtemperature_daynightranges::day::endhour = [0-23]
textgen::[section]::story::roadtemperature_daynightranges::day::maxstarthour = [0-23]
textgen::[section]::story::roadtemperature_daynightranges::day::minendhour = [0-23]
textgen::[section]::story::roadtemperature_daynightranges::night::maxstarthour = [0-23]
textgen::[section]::story::roadtemperature_daynightranges::night::minendhour = [0-23]
```

Note that the night's start and end times are not defined directly; they are implicitly defined by the day times.

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::roadtemperature_daynightranges::fake::day1::mean = [result]
textgen::[section]::story::roadtemperature_daynightranges::fake::day::minimum = [result]
textgen::[section]::story::roadtemperature_daynightranges::fake::day1::maximum = [result]
textgen::[section]::story::roadtemperature_daynightranges::fake::night1::mean = [result]
...
```

Phrases used:

* "tienpintalämpötila"
* "on"
* "noin"
* "astetta"
* "päivällä"
* "yöllä"

plus the period phrases [today](period_phrases.md#today) and [tonight](period_phrases.md#tonight).
