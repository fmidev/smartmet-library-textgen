# Story "wind_simple_overview"

> **Status:** Legacy. Superseded by [`wind_overview`](wind_overview.md) and
> [`wind_anomaly`](wind_anomaly.md). The original specification noted the
> story was not well defined; prefer one of the Primary wind stories for
> new products.
>
> **Owner:** `WindStory` (generator `wind_simple_overview.cpp`, ~297 LOC).

## Story form

The story describes a wind forecast for a one- or several-day period. It is best suited to the first 1–3 days.

Speed classes are not used, to simplify the algorithm. Direction classes are used.

The general form is:

1. [until the next morning], [remaining time]

The time definitions for the first part come from the variables:

```
textgen::[section]::story::wind_simple_overview::day::starthour = [0-23] (6)
textgen::[section]::story::wind_simple_overview::day::maxstarthour = [0-23] (starthour)
textgen::[section]::story::wind_simple_overview::night::starthour = [0-23] (18)
textgen::[section]::story::wind_simple_overview::night::maxstarthour = [0-23] (starthour)
textgen::[section]::story::wind_simple_overview::night::endhour = [0-23] (06)
textgen::[section]::story::wind_simple_overview::night::minendhour = [0-23] (endhour)
```

Typical settings are:

```
textgen::[section]::story::wind_simple_overview::day::starthour = 6
textgen::[section]::story::wind_simple_overview::day::maxstarthour = 12
textgen::[section]::story::wind_simple_overview::night::starthour = 18
textgen::[section]::story::wind_simple_overview::night::maxstarthour = 0
textgen::[section]::story::wind_simple_overview::night::endhour = 6
```

With these, the story reports up to the next morning, always until midnight, after which the upcoming day is reported.

### First day

If the day has progressed too far — i.e. the clock is past `day::maxstarthour` — the wind is reported up to the following morning. Both the remaining day and the night are included.

Possible stories for this "extended" night period:

1. "[Ajanjaksona] X-Y m/s."
1. "[Ajanjaksona] [suuntatuulta] X-Y m/s."

The time phrase type is [until_morning](../../period_phrases.md#until_morning), controlled with:

```
textgen::[section]::story::wind_simple_overview::until_morning::phrases
```

A suitable value is, for example:

```
textgen::[section]::story::wind_simple_overview::until_morning::phrases = atnight,weekday
```

because the phrase "tonight" may not be desirable; "atnight" likely fits better.

Direction and speed computation for different periods is handled separately below.

If the day is long enough to handle, it is handled like the night above. In that case the time phrase is of type [until_tonight](../../period_phrases.md#until_tonight), controlled with:

```
textgen::[section]::story::wind_simple_overview::until_tonight::phrases
```

A recommended value is, for example:

```
textgen::[section]::story::wind_simple_overview::until_tonight::phrases = atday,weekday
```

In addition, a subordinate clause about the night's wind is generated, of the form:

1. ", [ajanjaksona] [suuntatuulta] X-Y m/s."

The subordinate clause's time phrase is of type [tonight](../../period_phrases.md#tonight), controlled with:

```
textgen::[section]::story::wind_simple_overview::tonight::phrases
```

A recommended value is, for example:

```
textgen::[section]::story::wind_simple_overview::tonight::phrases = atnight,weekday
```

In this context "tonight" could also be used, which was not recommended when the day part was absent.

### Second day

In this case the end of the examined interval is at most 24 hours away, so we can talk only about the following day.

The second day can be reported in its own sentence or as a subordinate clause. The form is:

1. "[Ajanjaksona] [suuntatuulta] X-Y m/s."

The time phrase is of type [next_day](../../period_phrases.md#next_day), controlled with:

```
textgen::[section]::story::wind_simple_overview::next_day::phrases
```

A recommended value is, for example:

```
textgen::[section]::story::wind_simple_overview::next_day::phrases = tomorrow,followingday
```

### Remaining days

In this case the end of the examined interval is more than 24 hours away. This remaining time is reported as one sentence or subordinate clause:

1. "[Ajanjaksosta alkaen] [suuntatuulta] X-Y m/s."

The time phrase is of type [next_days](../../period_phrases.md#next_days), controlled with:

```
textgen::[section]::story::wind_simple_overview::next_days::phrases
```

A recommended value is, for example:

```
textgen::[section]::story::wind_simple_overview::next_days::phrases = tomorrow,weekday
```

## Algorithms

### Wind speed

The speed range is obtained by computing the minimum and maximum of the time means.

### Wind direction

The direction for each interval is obtained by computing the mean direction over time and space. Based on the direction's quality index, the direction's accuracy is first weakened to "approximately N" and then to "variable".

The required quality (standard deviation of the direction) is controlled by:

```
textgen::[section]::story::wind_simple_overview::direction::accurate (= 22.5)
textgen::[section]::story::wind_simple_overview::direction::variable (= 45)
```

Defaults are given in parentheses.

The direction phrase is chosen as follows:

1. If quality >= `accurate`, "N-tuulta"
1. If quality >= `variable`, "N-puoleista tuulta"
1. Otherwise, "suunnaltaan vaihtelevaa tuulta"

## Phrases required

Phrases required:

* "m/s"
* "metriä sekunnissa"
* "tuulta"
* "suunnaltaan vaihtelevaa"
* "N-tuulta", N=1–8
* "N-tuuleksi", N=1–8
* "N-puoleista tuulta", N=1–8
* "N-puoleiseksi tuuleksi", N=1–8

Additionally, the period phrases [until_tonight](../../period_phrases.md#until_tonight), [until_morning](../../period_phrases.md#until_morning), [tonight](../../period_phrases.md#tonight), [next_day](../../period_phrases.md#next_day), and [next_days](../../period_phrases.md#next_days) are required.
