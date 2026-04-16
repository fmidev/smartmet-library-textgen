# Mask sources

A `MaskSource` produces an index mask over a forecast grid that selects
a subset of the points in a `WeatherArea`. Masks are how a story says
"compute this only over the coast" or "only over the western half" —
the mask is applied before the analysis, so the point set seen by the
analyser already excludes the irrelevant grid points.

The base class lives in `calculator`, not `textgen`:

```cpp
// calculator/MaskSource.h
class MaskSource
{
public:
    using mask_type  = std::shared_ptr<NFmiIndexMask>;
    using masks_type = std::shared_ptr<NFmiIndexMaskSource>;

    virtual mask_type  mask (const WeatherArea&,
                             const std::string& data,
                             const WeatherSource&) const = 0;
    virtual masks_type masks(const WeatherArea&,
                             const std::string& data,
                             const WeatherSource&) const = 0;
};
```

All concrete `MaskSource` subclasses below live under `textgen/`.

## Hierarchy

```
MaskSource                 (abstract, in calculator)
├── CoastMaskSource        restricts to coastal grid points
├── InlandMaskSource       restricts to inland (non-coastal) points
├── LandMaskSource         excludes sea points
├── NorthernMaskSource     northern half of the area
├── SouthernMaskSource     southern half
├── EasternMaskSource      eastern half
├── WesternMaskSource      western half
└── NullMaskSource         no restriction (identity)
```

The `Null…` variant is the default and simply returns a mask that
covers every grid point.

## Typical usage

Masks enter the pipeline through `AnalysisSources`. `TextGenerator`
constructs a default set at startup and hands them to every story:

```cpp
AnalysisSources sources;
sources.coastMaskSource    (std::make_shared<CoastMaskSource>    (coastArea));
sources.inlandMaskSource   (std::make_shared<InlandMaskSource>   (coastArea));
sources.landMaskSource     (std::make_shared<LandMaskSource>     (landArea));
sources.northernMaskSource (std::make_shared<NorthernMaskSource> ());
// ...
```

Inside a story, an analysis call picks a mask by role:

```cpp
WeatherResult r = forecaster.analyze(fakename,
                                     sources,
                                     Temperature,
                                     Mean,   // time function
                                     Mean,   // area function
                                     area,
                                     period,
                                     DefaultAcceptor(),
                                     sources.coastMaskSource());  // <--
```

The analyser then iterates only over grid points the `CoastMaskSource`
keeps.

## `SubMaskExtractor`

A separate namespace in `textgen/SubMaskExtractor.h` that builds
`NFmiIndexMask` instances directly from a `WeatherArea` and an
`AnalysisSources`. Useful when a story wants to split an area into
parts that are not covered by the standard mask-source list (e.g. a
bounding-box quadrant, or the grid points inside an SVG polygon). See
the helper functions it exports:

```cpp
double GetLocationCoordinates(const AnalysisSources&,
                              const WeatherParameter&,
                              /* ... */);
```

## Related utilities

The `AreaTools` namespace (`textgen/AreaTools.h`) complements
`MaskSource` by mapping a `WeatherArea` to an enum:

```cpp
enum forecast_area_id
{
    FULL_AREA,
    INLAND_AREA,
    COASTAL_AREA,
    /* ... */
};
```

Stories use `forecast_area_id` to decide *which* mask to request.

## See also

* [`class index — mask sources`](classes.md#geographic-masks) for the
  one-liner list.
* [`WeatherArea` docs in calculator](https://github.com/fmidev/smartmet-library-calculator/blob/master/calculator/WeatherArea.h)
  for the underlying area type.
