# Story "temperature_dailymax"

The `temperature_dailymax` story has the form:

1. "Päivän ylin lämpötila on [viikonpäivänä] noin x astetta"
1. "Päivän ylin lämpötila on [viikonpäivänä] x...y astetta"

In Swedish:

1. "Dagens högsta temperatur är [på veckodag] cirka x grader"
1. "Dagens högsta temperatur är [på veckodag] x...y grader"

In English:

1. "The maximum day temperature [on Weekday] is about x degrees"
1. "The maximum day temperature [on Weekday] is x...y degrees"

The subordinate clause continues when needed:

1. ", [seuraavana päivänä] huomattavasti korkeampi"
1. ", [seuraavana päivänä] korkeampi"
1. ", [seuraavana päivänä] hieman korkeampi"
1. ", [seuraavana päivänä] suunnilleen sama"
1. ", [seuraavana päivänä] hieman alempi"
1. ", [seuraavana päivänä] alempi"
1. ", [seuraavana päivänä] huomattavasti alempi"

In Swedish:

1. ", [på följande dag] betydligt högre."
1. ", [på följande dag] högre."
1. ", [på följande dag] något högre."
1. ", [på följande dag] ungefär densamma."
1. ", [på följande dag] något lägre."
1. ", [på följande dag] lägre."
1. ", [på följande dag] betydligt lägre."

In English:

1. ", [the following day] significantly higher."
1. ", [the following day] higher."
1. ", [the following day] somewhat higher."
1. ", [the following day] about the same."
1. ", [the following day] somewhat lower."
1. ", [the following day] lower."
1. ", [the following day] significantly lower."

Period phrases are of type [today](../../period_phrases.md#today) and [next_day](../../period_phrases.md#next_day). Their settings are controlled with:

```
textgen::[section]::story::temperature_dailymax::today::phrases
textgen::[section]::story::temperature_dailymax::next_day::phrases
```

Typical settings include, for example:

```
textgen::[section]::story::temperature_dailymax::next_day::phrases = followingday
```

Comparative selection is based on the variables:

```
textgen::[section]::story::temperature_dailymax::comparison::significantly_higher = [N]
textgen::[section]::story::temperature_dailymax::comparison::higher = [N]
textgen::[section]::story::temperature_dailymax::comparison::somewhat_higher = [N]
textgen::[section]::story::temperature_dailymax::comparison::somewhat_lower = [N]
textgen::[section]::story::temperature_dailymax::comparison::lower = [N]
textgen::[section]::story::temperature_dailymax::comparison::significantly_lower = [N]
```

and the computed temperature ranges. If the first day's mean is A and the next day's mean is B, the choice is:

1. Phrase 1, if B−A >= `significantly_higher`
1. Phrase 2, if B−A >= `higher`
1. Phrase 3, if B−A >= `somewhat_higher`
1. Phrase 7, if A−B >= `significantly_lower`
1. Phrase 6, if A−B >= `lower`
1. Phrase 5, if A−B >= `somewhat_lower`
1. Phrase 4 otherwise

If there are more days, each subsequent day's maximum is listed as a subordinate clause:

* ", viikonpäivänä noin x astetta"
* ", viikonpäivänä x...y astetta"

The choice between a single value and an interval is based on:

```
textgen::[section]::story::temperature_dailymax::mininterval = [1-]
textgen::[section]::story::temperature_dailymax::always_interval_zero = [bool]
```

If the interval length is at least `mininterval`, an interval is reported; otherwise the mean. As an exception, if zero falls in the interval and `always_interval_zero` is true, an interval is always reported.

Default for `mininterval` is 2; default for `always_interval_zero` is false.

The day is defined by the variables:

```
textgen::[section]::story::temperature_dailymax::day::starthour = [0-23]
textgen::[section]::story::temperature_dailymax::day::endhour = [0-23]
textgen::[section]::story::temperature_dailymax::day::maxstarthour = [0-23]
textgen::[section]::story::temperature_dailymax::day::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

The definitions could be, for example:

```
textgen::[section]::story::temperature_dailymax::day::starthour    = 6
textgen::[section]::story::temperature_dailymax::day::endhour      = 18
textgen::[section]::story::temperature_dailymax::day::maxstarthour = 12
```

In that case the day's maximum is always reported up to 12 noon, even though the period already begins at 06. At the end of the period flex is rarely needed.

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::temperature_dailymax::fake::day1::mean = [result]
textgen::[section]::story::temperature_dailymax::fake::day1::minimum = [result]
textgen::[section]::story::temperature_dailymax::fake::day1::maximum = [result]
textgen::[section]::story::temperature_dailymax::fake::day2::mean = [result]
...
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

plus the period phrases [today](../../period_phrases.md#today) and [next_day](../../period_phrases.md#next_day).
