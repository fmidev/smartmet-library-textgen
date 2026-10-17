# Story "frost_onenight"

> **Status:** Primary (modern frost product).
>
> **Owner:** `FrostStory::onenight()`.
> **Implementation:** `textgen/frost_onenight.cpp` (~834 LOC).

## What it produces

A detailed single-night frost narrative combining:

* probability of frost / severe frost
* coast vs. inland decomposition
* growing season check: nothing is reported for an area where the growing
  season has not started
* "already obvious" suppression: if night frost (minimum temperature below
  zero) is already widespread on both the coast and inland, the story stays
  silent

## Configuration root

```
textgen::[section]::story::frost_onenight::*
```

The variables read are:

```
textgen::[section]::story::frost_onenight::night::starthour = [0-23]
textgen::[section]::story::frost_onenight::night::endhour = [0-23]
textgen::[section]::story::frost_onenight::night::maxstarthour = [0-23] (starthour)
textgen::[section]::story::frost_onenight::night::minendhour = [0-23] (endhour)
textgen::[section]::story::frost_onenight::required_night_frost_percentage = [0-100] (=20)
textgen::[section]::story::frost_onenight::required_severe_frost_probability = [0-100] (=20)
textgen::[section]::story::frost_onenight::required_growing_season_percentage::default = [0-100] (=33.33)
textgen::[section]::story::frost_onenight::required_growing_season_percentage::[area] = [0-100]
textgen::[section]::story::frost_onenight::separate_coastal_area_percentage = [0-100] (=5)
textgen::[section]::story::frost_onenight::dont_report_coastal_area = [area,area,...]
textgen::[section]::story::frost_onenight::not_before_date = [MMDD] (=0)
textgen::[section]::story::frost_onenight::not_after_date = [MMDD] (=0)
```

The night is not reported at all if the night frost percentage is at least
`required_night_frost_percentage` both on the coast and inland. Severe frost
is mentioned when its probability exceeds `required_severe_frost_probability`.
The coast is reported separately only if it covers at least
`separate_coastal_area_percentage` of the area and the area is not listed in
`dont_report_coastal_area`.

The story is empty unless the temperature sum file named by the global
`textgen::temperaturesum_forecast` variable exists (or `regression_test` is
`true`).

See `textgen/frost_onenight.cpp` for the `fake::` variables and
`test/FrostStoryTest.cpp` for working configurations.

## Seasonal silence

`frost_onenight` reports frost only for areas where the growing season is
going on: at least `required_growing_season_percentage` (per area name, or
`default`) of the area must have reached the growing season. In addition,
when `not_before_date` and/or `not_after_date` are set (as `MMDD`), periods
starting outside those dates are not reported.

## Related

* [`frost_day`](frost_day.md) — simpler single-night form.
* [`frost_twonights`](frost_twonights.md) — two-night comparison.
