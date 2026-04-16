# Story "pop_days"

The `pop_days` story has the form:

1. "Sateen todennäköisyys on [viikonpäivänä] x%"
   1. ", [viikonpäivänä] huomattavasti suurempi."
   1. ", [viikonpäivänä] suurempi."
   1. ", [viikonpäivänä] hieman suurempi."
   1. ", [viikonpäivänä] sama."
   1. ", [viikonpäivänä] hieman pienempi."
   1. ", [viikonpäivänä] pienempi."
   1. ", [viikonpäivänä] huomattavasti pienempi."
1. "Poudan todennäköisyys on [viikonpäivänä] x%"

where the last form is used if the negation option is on.

In Swedish:

1. "Sannolikheten för nederbörd är [på veckodag] x%"
   1. ", [på veckodag] betydligt större."
   1. ", [på veckodag] större."
   1. ", [på veckodag] något större."
   1. ", [på veckodag] densamma."
   1. ", [på veckodag] något mindre."
   1. ", [på veckodag] mindre."
   1. ", [på veckodag] betydligt mindre."
1. "Sannolikheten för ingen nederbörd är [på veckodag] x%"

In English:

1. "Probability of precipitation is [on weekday] x%"
   1. ", [on weekday] significantly greater."
   1. ", [on weekday] greater."
   1. ", [on weekday] somewhat greater."
   1. ", [on weekday] the same."
   1. ", [on weekday] somewhat smaller."
   1. ", [on weekday] smaller."
   1. ", [on weekday] significantly smaller."
1. "Probability of no precipitation is [on weekday] x%"

The story applies to one- or two-day forecasts. There is no subordinate clause for a one-day forecast. Nothing is reported about the third or subsequent days.

If a day's probability of precipitation falls below or above the configured limits, that day is not reported. What remains is therefore either an empty story or a story of the form "Sateen todennäköisyys on [viikonpäivänä] X%."

The probability of precipitation is computed as follows:

1. Compute the maximum over time, then the regional mean
1. Compute the mean over time, then the regional mean
1. Take the mean of these two values

When we speak about the current day, the first value corresponds to the interpretation "The average probability over the area that, at some moment of the day, there is precipitation." The second value corresponds to "The average probability over the area that, at any given moment, there is precipitation." People's perception of a regional probability likely varies between these two interpretations, so the mean of them is taken.

Whether the story refers to the probability of precipitation or the probability of no precipitation is determined by the possible variable:

```
textgen::[section]::story::pop_days::negate
```

With the value `true`, this is the probability of no precipitation. In all other cases it is the probability of precipitation.

Period phrases are of type [today](period_phrases.md#today) and [next_day](period_phrases.md#next_day). Their settings are controlled with:

```
textgen::[section]::story::pop_days::today::phrases
textgen::[section]::story::pop_days::next_day::phrases
```

Typical settings include, for example:

```
textgen::[section]::story::pop_days::next_day::phrases = followingday
```

The day is defined by the variables:

```
textgen::[section]::story::pop_days::day::starthour = [0-23]
textgen::[section]::story::pop_days::day::endhour = [0-23]
textgen::[section]::story::pop_days::day::maxstarthour = [0-23]
textgen::[section]::story::pop_days::day::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

By default `starthour` = 0 and `endhour` = 0. Flex limits are optional.

The probability of precipitation is rounded to the precision:

```
textgen::[section]::story::pop_days::precision = [0-100]
```

The probability must be at least:

```
textgen::[section]::story::pop_days::minimum = [0-100] (default = 10)
```

before it is reported, and at most:

```
textgen::[section]::story::pop_days::maximum = [0-100] (default = 100)
```

The magnitude of the difference is classified using the following limits:

```
textgen::[section]::story::pop_days::comparison::significantly_greater = [0-100]
textgen::[section]::story::pop_days::comparison::greater = [0-100]
textgen::[section]::story::pop_days::comparison::somewhat_greater = [0-100]
textgen::[section]::story::pop_days::comparison::somewhat_smaller = [0-100]
textgen::[section]::story::pop_days::comparison::smaller = [0-100]
textgen::[section]::story::pop_days::comparison::significantly_smaller = [0-100]
```

applied to results `pop2` and `pop1` as follows:

1. If pop2 − pop1 >= `significantly_greater`, "significantly greater" is chosen
1. If pop2 − pop1 >= `greater`, "greater" is chosen
1. If pop2 − pop1 >= `somewhat_greater`, "somewhat greater" is chosen
1. If pop1 − pop2 >= `significantly_smaller`, "significantly smaller" is chosen
1. If pop1 − pop2 >= `smaller`, "smaller" is chosen
1. If pop1 − pop2 >= `somewhat_smaller`, "somewhat smaller" is chosen
1. Otherwise "the same" is chosen

Phrases used:

* "sateen todennäköisyys"
* "poudan todennäköisyys"
* "on"
* "huomattavasti suurempi"
* "suurempi"
* "hieman suurempi"
* "sama"
* "hieman pienempi"
* "pienempi"
* "huomattavasti pienempi"

Additionally, the period phrases [today](period_phrases.md#today) and [next_day](period_phrases.md#next_day) are required.

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::pop_days::fake::day1::meanmax = [result]
textgen::[section]::story::pop_days::fake::day1::meanmean = [result]
textgen::[section]::story::pop_days::fake::day2::meanmax = [result]
textgen::[section]::story::pop_days::fake::day2::meanmean = [result]
```
