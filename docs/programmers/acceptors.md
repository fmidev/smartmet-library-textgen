# Acceptors

An `Acceptor` is a single-value predicate that decides whether a grid
point's value should count toward a reduction. Acceptors are paired
with functions like `Percentage` and `Count`:

```cpp
Percentage(area)        // without acceptor: 100 %
Percentage(area, acc)   // with acceptor: 100 × (n accepted / n total)
```

Or passed as a filter to the analyser so the value array sent to the
reduction excludes outliers.

The base class lives in `calculator`, not `textgen`:

```cpp
// calculator/Acceptor.h
class Acceptor
{
public:
    virtual bool accept(float value) const = 0;
    virtual Acceptor* clone() const = 0;
};
```

## Hierarchy

```
Acceptor                   (abstract, in calculator)
├── DefaultAcceptor        accepts every non-kFloatMissing value
├── NullAcceptor           accepts nothing
├── RangeAcceptor          accepts values inside [lo, hi]
└── TextGen:
    ├── ValueAcceptor
    ├── ComparativeAcceptor
    ├── PositiveValueAcceptor
    ├── AndAcceptor
    ├── OrAcceptor
    └── TemplateAcceptor
```

Concrete acceptors in `textgen/` delegate to the base interface; the
combinators (`And…`, `Or…`) wrap two other acceptors.

## Concrete acceptors

### `DefaultAcceptor` (in calculator)

Accepts every value that is not `kFloatMissing`. The default when no
acceptor is specified.

### `NullAcceptor` (in calculator)

Accepts nothing. Useful for short-circuiting a `Percentage` / `Count`
call that should return zero unconditionally.

### `RangeAcceptor` (in calculator)

Accepts values in `[lo, hi]`:

```cpp
RangeAcceptor rain;
rain.lowerLimit(0.04);   // e.g. "≥ 0.04 mm/h"
rain.upperLimit(kFloatMissing);  // no upper cap
```

This is the workhorse acceptor, used extensively by
[`weather_forecast`](../users/stories/weather/weather_forecast.md) for
precipitation intensity classification.

### `ValueAcceptor` (textgen)

Accepts a single floating value (within a tolerance). Used to check
for specific numeric codes.

### `ComparativeAcceptor` (textgen)

Accepts based on a one-sided comparison: `value < x`, `value > x`,
`value <= x`, `value >= x`.

### `PositiveValueAcceptor` (textgen)

Convenience shorthand for "value > 0".

### `AndAcceptor` / `OrAcceptor` (textgen)

Two-child combinators:

```cpp
AndAcceptor rainyPositiveTemp;
rainyPositiveTemp.first(RangeAcceptor(0.04, kFloatMissing));
rainyPositiveTemp.second(ComparativeAcceptor(Greater, 0.0));
```

### `TemplateAcceptor` (textgen)

Generic acceptor parameterised by a comparator functor — for cases
where the built-in arithmetic comparators aren't enough.

## Typical usage

Acceptors appear most often in two places:

**1. As the filter argument of an analysis call.** For example, to
compute the mean of only the positive temperatures:

```cpp
WeatherResult r = forecaster.analyze(
    fakename,
    sources,
    Temperature,
    Mean,                     // time function
    Mean,                     // area function
    area,
    period,
    PositiveValueAcceptor()); // <-- filter
```

**2. Inside `Percentage` / `Count` reductions.** For example, to
compute the fraction of hours with mean cloudiness ≥ 65 %:

```cpp
RangeAcceptor cloudy;
cloudy.lowerLimit(65);
cloudy.upperLimit(100);

WeatherResult r = forecaster.analyze(
    fakename,
    sources,
    Cloudiness,
    Percentage,  // area function
    Mean,        // time function
    area,
    period,
    cloudy);
```

The analyser hands every per-hour area-mean to `cloudy.accept()`; the
`Percentage` reduction counts the accepted hours as a fraction of the
total.

## See also

* [Analysis functions](functions.md) — which of the `WeatherFunction`
  values consult the acceptor (`Percentage`, `Count`) and which ignore
  it.
* [Mask sources](masks.md) — the geographic counterpart. A `MaskSource`
  narrows *which points* are considered; an `Acceptor` narrows *which
  values* count.
