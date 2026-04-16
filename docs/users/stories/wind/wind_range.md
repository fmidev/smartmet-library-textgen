# Story "wind_range"

> **Status:** Trivial. Emits a single sentence describing wind speed and
> direction for a period.
>
> For real wind products prefer
> [`wind_overview`](wind_overview.md) — it uses the same speed and
> direction classifications but adds change detection, rate phrases,
> gusty-wind handling, and coast/inland awareness.
>
> **Owner:** `WindStory::range()`.
> **Implementation:** `textgen/wind_range.cpp` (~108 LOC).

## What it produces

Two possible forms:

1. "[Pohjoistuulta] noin X metriä sekunnissa."
2. "[Pohjoistuulta] X…Y metriä sekunnissa."

In place of "pohjoistuulta" the story inserts the matching direction
phrase:

1. "pohjoistuulta"
2. "pohjoisen puoleista tuulta"
3. "suunnaltaan vaihtelevaa tuulta"

The wind interval is collapsed to a single number (the mean) if the
interval is smaller than:

```
textgen::[section]::story::wind_range::mininterval = [0-] (0)
```

## Direction selection

| Standard deviation of wind direction | Phrase |
| --- | --- |
| `< accurate` | "N-tuulta" |
| `>= variable` | "N-puoleista tuulta" |
| otherwise | "suunnaltaan vaihtelevaa tuulta" |

Controlled by:

```
textgen::[section]::story::wind_range::direction::accurate (= 22.5)
textgen::[section]::story::wind_range::direction::variable (= 45)
```

This is a simplified version of the direction classification used by
[`wind_overview`](wind_overview.md#wind-direction); see that page for
the 8-way and 16-way compass tables and for the full precedence rules
(e.g. "suunnaltaan vaihtelevaa" is suppressed when the warning value
exceeds 6.5 m/s).

## Speed classification

`wind_range` does not itself verbalise the speed as "heikkoa" /
"kohtalaista" / "navakkaa" — it simply reports the numeric range. For
the corresponding wind-class vocabulary see
[`wind_overview` → Wind-speed classification](wind_overview.md#wind-speed-classification).

## Fake values

```
textgen::[section]::story::wind_range::fake::speed::minimum = [result]
textgen::[section]::story::wind_range::fake::speed::maximum = [result]
textgen::[section]::story::wind_range::fake::speed::mean = [result]

textgen::[section]::story::wind_range::fake::direction::mean = [result]
textgen::[section]::story::wind_range::fake::direction::sdev = [result]
```

## Phrases required

* "noin"
* "metriä sekunnissa"
* "suunnaltaan vaihtelevaa"
* "tuulta"
* "N-tuulta", N = 1…8
* "N-puoleista tuulta", N = 1…8
