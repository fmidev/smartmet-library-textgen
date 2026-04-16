# Story "pop_twodays"

The `pop_twodays` story has the form:

1. "Sateen todennäköisyys on [viikonpäivänä] x%"
   1. ", [viikonpäivänä] huomattavasti suurempi."
   1. ", [viikonpäivänä] suurempi."
   1. ", [viikonpäivänä] hieman suurempi."
   1. ", [viikonpäivänä] sama."
   1. ", [viikonpäivänä] hieman pienempi."
   1. ", [viikonpäivänä] pienempi."
   1. ", [viikonpäivänä] huomattavasti pienempi."

In Swedish:

1. "Sannolikheten för nederbörd är [på veckodag] x%"
   1. ", [på veckodag] betydligt större."
   1. ", [på veckodag] större."
   1. ", [på veckodag] något större."
   1. ", [på veckodag] densamma."
   1. ", [på veckodag] något mindre."
   1. ", [på veckodag] mindre."
   1. ", [på veckodag] betydligt mindre."

In English:

1. "Probability of precipitation is [on weekday] x%"
   1. ", [on weekday] significantly greater."
   1. ", [on weekday] greater."
   1. ", [on weekday] somewhat greater."
   1. ", [on weekday] the same."
   1. ", [on weekday] somewhat smaller."
   1. ", [on weekday] smaller."
   1. ", [on weekday] significantly smaller."

The story applies to one- or two-day forecasts. There is no subordinate clause for one-day forecasts. Nothing is reported about the third or later days.

If a day's probability of precipitation falls below the configured limit, that day is not reported. What remains is therefore either an empty story or a story of the form "Sateen todennäköisyys on [viikonpäivänä] X%."

Period phrases are of type [today](../../period_phrases.md#today) and [next_day](../../period_phrases.md#next_day). Their settings are controlled with:

```
textgen::[section]::story::pop_twodays::today::phrases
textgen::[section]::story::pop_twodays::next_day::phrases
```

Typical settings include, for example:

```
textgen::[section]::story::pop_twodays::next_day::phrases = followingday
```

The day is defined by the variables:

```
textgen::[section]::story::pop_twodays::day::starthour = [0-23]
textgen::[section]::story::pop_twodays::day::endhour = [0-23]
textgen::[section]::story::pop_twodays::day::maxstarthour = [0-23]
textgen::[section]::story::pop_twodays::day::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

By default `starthour` = 0 and `endhour` = 0. Flex limits are optional.

The probability of precipitation is rounded to the precision:

```
textgen::[section]::story::pop_twodays::precision = [0-100]
```

The probability must be at least:

```
textgen::[section]::story::pop_twodays::limit = [0-100]
```

before it is reported.

The magnitude of the difference is classified using the following limits:

```
textgen::[section]::story::pop_twodays::comparison::significantly_greater = [0-100]
textgen::[section]::story::pop_twodays::comparison::greater = [0-100]
textgen::[section]::story::pop_twodays::comparison::somewhat_greater = [0-100]
textgen::[section]::story::pop_twodays::comparison::somewhat_smaller = [0-100]
textgen::[section]::story::pop_twodays::comparison::smaller = [0-100]
textgen::[section]::story::pop_twodays::comparison::significantly_smaller = [0-100]
```

applied to results `pop2` and `pop1`:

1. If pop2 − pop1 >= `significantly_greater`, "significantly greater" is chosen
1. If pop2 − pop1 >= `greater`, "greater" is chosen
1. If pop2 − pop1 >= `somewhat_greater`, "somewhat greater" is chosen
1. If pop1 − pop2 >= `significantly_smaller`, "significantly smaller" is chosen
1. If pop1 − pop2 >= `smaller`, "smaller" is chosen
1. If pop1 − pop2 >= `somewhat_smaller`, "somewhat smaller" is chosen
1. Otherwise "the same" is chosen

Phrases used:

* "sateen todennäköisyys"
* "on"
* "huomattavasti suurempi"
* "suurempi"
* "hieman suurempi"
* "sama"
* "hieman pienempi"
* "pienempi"
* "huomattavasti pienempi"

Additionally, the period phrases [today](../../period_phrases.md#today) and [next_day](../../period_phrases.md#next_day) are required.

The generated analysis functions can be overridden as follows:

```
1. Minimum(Minimum(Temperature)):
textgen::[section]::story::pop_twodays::fake::day1::maximum = [result]
textgen::[section]::story::pop_twodays::fake::day2::maximum = [result]
```
