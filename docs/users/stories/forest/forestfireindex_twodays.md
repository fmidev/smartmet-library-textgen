# Story "forestfireindex_twodays"

The `forestfireindex_twodays` story has the form:

1. "Metsäpaloindeksi on [viikonpäivänä] x, [viikonpäivänä] y."

In Swedish:

1. "Bränningsindexen är [på veckodag] x, [på veckodag] y."

In English:

1. "The forest fire index is [on weekday] x, [on weekday] y."

The story applies to one- or two-day forecasts. There is no subordinate clause for one-day forecasts. Nothing is reported about the third or later days — the humidity model never extends that far.

Period phrases are of type [today](../../period_phrases.md#today) and [next_day](../../period_phrases.md#next_day). Their settings are controlled with:

```
textgen::[section]::story::forestfireindex_twodays::today::phrases
textgen::[section]::story::forestfireindex_twodays::next_day::phrases
```

Typical settings include, for example:

```
textgen::[section]::story::forestfireindex_twodays::next_day::phrases = followingday
```

The day is defined by the variables:

```
textgen::[section]::story::forestfireindex_twodays::day::starthour = [0-23]
textgen::[section]::story::forestfireindex_twodays::day::endhour = [0-23]
textgen::[section]::story::forestfireindex_twodays::day::maxstarthour = [0-23]
textgen::[section]::story::forestfireindex_twodays::day::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

By default `starthour` = 0 and `endhour` = 0. Flex limits are optional.

Phrases used:

* "metsäpaloindeksi"
* "on"

Additionally, the period phrases [today](../../period_phrases.md#today) and [next_day](../../period_phrases.md#next_day) are required.

The generated analysis functions can be overridden as follows:

```
1. Minimum(Minimum(Temperature)):
textgen::[section]::story::forestfireindex_twodays::fake::day1::maximum = [result]
textgen::[section]::story::forestfireindex_twodays::fake::day2::maximum = [result]
```
