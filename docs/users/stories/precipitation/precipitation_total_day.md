# Story "precipitation_total_day"

> **Status:** Trivial. Single-day variant of
> [`precipitation_total`](precipitation_total.md).
>
> **Owner:** `PrecipitationStory::total_day()`.
> **Implementation:** `textgen/precipitation_total_day.cpp` (~86 LOC).

## What it produces

One sentence of the form "Sadesumma on tänään N millimetriä." restricted
to a single day period.

## Configuration root

```
textgen::[section]::story::precipitation_total_day::*
```

Reads `minrain` (filter threshold), the `day::*` period definition, and
the `today::phrases` preference list.

Prefer [`precipitation_daily_sums`](precipitation_daily_sums.md) for
multi-day products.
