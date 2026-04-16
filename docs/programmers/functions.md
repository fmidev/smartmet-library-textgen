# Analysis functions

The `TextGen::WeatherFunction` enumeration, defined in
[`calculator/WeatherFunction.h`](https://github.com/fmidev/smartmet-library-calculator/blob/master/calculator/WeatherFunction.h),
lists the reductions that the weather analyser knows how to apply. Every
story ends up invoking a pair of these — one for the **area** reduction
(points → value) and one for the **time** reduction (hours → value).
For example, "mean of daily maximum temperature" is
`Mean(time) · Maximum(area)` (the area reduction runs first, per hour,
then the time reduction averages across days).

The enum values are:

```cpp
enum WeatherFunction
{
    NullFunction,
    Mean,
    Maximum,
    Minimum,
    Median,
    Sum,
    StandardDeviation,
    Percentage,
    Count,
    Change,
    Trend,
    Peak
};
```

The rest of this page documents each one.

## `Mean`

Arithmetic mean over the input set (points or hours).

**Quality.** For an area reduction, the quality reported alongside the
result is the area-wise **standard deviation** — higher deviation means
lower quality. Stories can use this value to decide whether to split
the area into sub-regions. (A time-domain `Mean` also returns a
quality, but see [Quality](../users/quality.md) — only area integration
is treated as quality-bearing; the time integration is assumed exact.)

## `Maximum`

Largest value over the input set. Quality = 0 (exact; no uncertainty is
reported).

## `Minimum`

Smallest value. Quality = 0.

## `Median`

Middle value (sorted). Quality = 0. Used by
[`wind_overview`](../users/stories/wind/wind_overview.md) as the anchor
for the wind-speed range.

## `Sum`

Sum of the inputs. Quality = 0.

Typical use: `Sum(time) · Mean(area)` for total precipitation —
"total rainfall averaged over the area".

## `StandardDeviation`

Unbiased sample standard deviation. Quality = 0 itself; used as a
quality proxy when another function is the main result.

## `Percentage`

Fraction of the inputs that fall inside a configurable acceptor's
range, expressed as 0…100. The acceptor is supplied by the caller.
Quality = 0.

Typical use: "percentage of grid points with cloudiness ≥ 65 %" →
`Percentage(area)` with a `ValueAcceptor(65, 100)`.

## `Count`

Number of inputs that fall inside an acceptor's range (absolute count,
not a fraction). Quality = 0.

## `Change`

Difference between the last and first values. Typically used on a time
series. Quality = 0.

## `Trend`

Difference between the last and first values — **identical** to
`Change` in value — but with a meaningful quality. The quality is the
fraction of consecutive time-series steps whose sign matches the
overall change. So:

* A monotonically increasing or decreasing series → quality 1.0
* A series with steady upward drift and minor jitter → quality clearly
  above 0.5
* A noisy series with no overall direction → quality near 0.5

Intended for use only along the time dimension; the analyser does not
enforce this at runtime.

Rule of thumb from [Quality](../users/quality.md): if `Trend` quality
is above 0.5, you can speak of a "trend" in the series; below 0.5 the
endpoints' difference is probably just noise.

## `Peak`

Largest signed excursion from the mean (either positive or negative),
returned with its sign. A symmetric variant of `Maximum` / `Minimum`
that does not depend on the series direction.

## `NullFunction`

Placeholder returned when no function is required. Stories should not
invoke it.

## Combining area and time reductions

Most of TextGen's analyses are `TimeFunction(AreaFunction(parameter))`
pairs. The area reduction runs first for each hour of the forecast
period, producing a time series; the time reduction then reduces that
series.

Examples, using the notation `timefn · areafn`:

| Call | Meaning |
| --- | --- |
| `Mean · Mean` | Mean over the whole period, averaged over the area |
| `Mean · Maximum` | Mean of the hourly maxima |
| `Maximum · Maximum` | Peak value over the whole period and area |
| `Minimum · Minimum` | Lowest single value anywhere, any hour |
| `Mean · Percentage` | Mean (over time) of the "fraction in range" (over area) |
| `Sum · Mean` | Total (over time) of the area-mean value — e.g. total precipitation |
| `Trend · Mean` | Trend (over time) of the area-mean value — endpoints' diff plus quality |
| `Change · Mean` | Difference between last and first area means |

The pairing a given story uses is spelled out in its configuration's
`fake::*` sub-tree — if a story takes `fake::day1::mean`, the
underlying call is `Mean · Mean` for day 1.

## Writing a new analyser

Functions that reduce a collection of `WeatherResult` values live in
the `calculator` library (e.g.
[`GridForecaster`](https://github.com/fmidev/smartmet-library-calculator/blob/master/calculator/GridForecaster.h)),
not in `textgen`. Adding a new function means:

1. Append the enum value in `calculator/WeatherFunction.h`.
2. Implement it in the analyser dispatch (search `calculator/` for the
   existing `case Mean:` lines).
3. Document it here.

## See also

* [Quality](../users/quality.md) — how quality is interpreted and used
  for automatic period splitting
* [Architecture overview](architecture.md) — where analysis fits in the
  request flow
