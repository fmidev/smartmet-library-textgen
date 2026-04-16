# Story "relativehumidity_lowest"

The `relativehumidity_lowest` story has the form:

1. "Alin suhteellinen kosteus on [viikonpäivänä] x%"
   1. ", [viikonpäivänä] huomattavasti suurempi."
   1. ", [viikonpäivänä] suurempi."
   1. ", [viikonpäivänä] hieman suurempi."
   1. ", [viikonpäivänä] sama."
   1. ", [viikonpäivänä] hieman pienempi."
   1. ", [viikonpäivänä] pienempi."
   1. ", [viikonpäivänä] huomattavasti pienempi."

In Swedish:

1. "Den lägsta relativa fuktigheten är [på veckodag] x%"
   1. ", [på veckodag] betydligt större."
   1. ", [på veckodag] större."
   1. ", [på veckodag] något större."
   1. ", [på veckodag] densamma."
   1. ", [på veckodag] något mindre."
   1. ", [på veckodag] mindre."
   1. ", [på veckodag] betydligt mindre."

In English:

1. "The smallest relative humidity [on weekday] is x%"
   1. ", [on weekday] significantly greater."
   1. ", [on weekday] greater."
   1. ", [on weekday] somewhat greater."
   1. ", [on weekday] the same."
   1. ", [on weekday] somewhat smaller."
   1. ", [on weekday] smaller."
   1. ", [on weekday] significantly smaller."

The story applies to one- or two-day forecasts. There is no subordinate clause for a one-day forecast. Nothing is reported about the third or later days.

Period phrases are of type [today](../../period_phrases.md#today) and [next_day](../../period_phrases.md#next_day). Their settings are controlled with:

```
textgen::[section]::story::relativehumidity_lowest::today::phrases
textgen::[section]::story::relativehumidity_lowest::next_day::phrases
```

Typical settings include, for example:

```
textgen::[section]::story::relativehumidity_lowest::next_day::phrases = followingday
```

The day is defined by the variables:

```
textgen::[section]::story::relativehumidity_lowest::day::starthour = [0-23]
textgen::[section]::story::relativehumidity_lowest::day::endhour = [0-23]
textgen::[section]::story::relativehumidity_lowest::day::maxstarthour = [0-23]
textgen::[section]::story::relativehumidity_lowest::day::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

By default `starthour` = 0 and `endhour` = 0. Flex limits are optional.

Relative humidity is rounded to the precision:

```
textgen::[section]::story::relativehumidity_lowest::precision = [0-100]
```

The magnitude of the difference is classified using the following limits:

```
textgen::[section]::story::relativehumidity_lowest::comparison::significantly_greater = [0-100]
textgen::[section]::story::relativehumidity_lowest::comparison::greater = [0-100]
textgen::[section]::story::relativehumidity_lowest::comparison::somewhat_greater = [0-100]
textgen::[section]::story::relativehumidity_lowest::comparison::somewhat_smaller = [0-100]
textgen::[section]::story::relativehumidity_lowest::comparison::smaller = [0-100]
textgen::[section]::story::relativehumidity_lowest::comparison::significantly_smaller = [0-100]
```

applied to results `humidity2` and `humidity1`:

1. If humidity2 − humidity1 >= `significantly_greater`, "significantly greater" is chosen
1. If humidity2 − humidity1 >= `greater`, "greater" is chosen
1. If humidity2 − humidity1 >= `somewhat_greater`, "somewhat greater" is chosen
1. If humidity1 − humidity2 >= `significantly_smaller`, "significantly smaller" is chosen
1. If humidity1 − humidity2 >= `smaller`, "smaller" is chosen
1. If humidity1 − humidity2 >= `somewhat_smaller`, "somewhat smaller" is chosen
1. Otherwise "the same" is chosen

Phrases used:

* "alin suhteellinen kosteus"
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
textgen::[section]::story::relativehumidity_lowest::fake::day1::minimum = [result]
textgen::[section]::story::relativehumidity_lowest::fake::day2::minimum = [result]
```
