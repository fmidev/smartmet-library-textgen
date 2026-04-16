# Story "relativehumidity_day"

The `relativehumidity_day` story reports the day's relative humidity. The story is aimed at farmers, who are mainly interested in the lowest forecast relative humidity. The returned value is the regional mean of the temporal minimum.

The story has one of the following forms:

1. "Alin suhteellinen kosteus on [viikonpäivänä] x%"
1. "Alin suhteellinen kosteus on [viikonpäivänä] x%, rannikolla y%"

In Swedish:

1. "Den lägsta relativa fuktigheten är [på veckodag] x%"
1. "Den lägsta relative fuktigheten är [på veckodag] x%, vid kusten y%"

In English:

1. "The smallest relative humidity [on weekday] is x%"
1. "The smallest relative humidity [on weekday] is x%, on the coastal area y%"

Period phrases are of type [today](period_phrases.md#today). Their settings are controlled with:

```
textgen::[section]::story::relativehumidity_day::today::phrases
```

The lowest relative humidity is searched from a time interval defined by:

```
textgen::[section]::story::relativehumidity_day::day::starthour = [0-23]
textgen::[section]::story::relativehumidity_day::day::endhour = [0-23]
textgen::[section]::story::relativehumidity_day::day::maxstarthour = [0-23]
textgen::[section]::story::relativehumidity_day::day::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

By default these settings are determined directly by the period length itself.

Relative humidity is rounded to the precision:

```
textgen::[section]::story::relativehumidity_day::precision = [0-100] (= 10%)
```

Phrases used:

* "alin suhteellinen kosteus"
* "on"
* "rannikolla"

Additionally, the period phrases [today](period_phrases.md#today) are required.

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::relativehumidity_day::fake::area::minimum = [result]
textgen::[section]::story::relativehumidity_day::fake::coast::minimum = [result]
textgen::[section]::story::relativehumidity_day::fake::inland::minimum = [result]
```

The story is constructed as follows:

1. Compute mean(min(rh)) for the area, the coast, and the inland
1. If either coast or inland is missing, the story of the form "Alin suhteellinen kosteus on [viikonpäivänä] x%" is returned
1. If the difference between coast and inland falls below the configured `coast_limit`, the story of the form "Alin suhteellinen kosteus on [viikonpäivänä] x%" is returned
1. Otherwise, "Alin suhteellinen kosteus on [viikonpäivänä] x%, rannikolla y%" is returned

Variables used:

```
textgen::[section]::story::relativehumidity_day::coast_limit = [0-100] (= 30%)
```
