# Class index

One-line purpose for every class in `smartmet-library-textgen`. Grouped by
role. For deeper explanations of a hierarchy, follow the link at the top of
each section.

Some classes are abstract bases; their subclasses live immediately underneath
in the tables. Namespaces (like `AreaTools`, `WeekdayTools`) are listed with
classes where it aids navigation.

---

## Entry point

| Class | Purpose |
| --- | --- |
| `TextGenerator` | Top-level driver. Reads `textgen::sections` from the config, builds the `Document` section by section, and returns it. |

---

## Composite: `Glyph` and friends

See [glyphs.md](glyphs.md).

| Class | Purpose |
| --- | --- |
| `Glyph` | Abstract base. Any unit of composed text. Supports `realize(Dictionary)` and `realize(TextFormatter)` via double dispatch. |
| `GlyphContainer` | `Glyph` that owns a list of child glyphs. |
| `Document` | Top-level container. Result of `TextGenerator::generate()`. |
| `Header` | Container holding one section header (from `HeaderFactory`). |
| `Paragraph` | Container for a sequence of sentences. |
| `Sentence` | Container that auto-capitalises the first word and appends a period. |
| `TimePhrase` | Container wrapping a `Sentence` that describes a period ("tänään", "huomisaamusta alkaen"). |
| `Phrase` | Leaf. A dictionary-key token that resolves at format time. |
| `LocationPhrase` | Leaf. Place name, auto-capitalised, with dictionary override. |
| `Integer` | Leaf. An `int` with optional formatting policy. |
| `Real` | Leaf. A `float`/`double` with optional formatting policy. |
| `IntegerRange` | Leaf. `"X...Y"` integer range. |
| `RealRange` | Leaf. `"X...Y"` floating-point range. |
| `PositiveRange` | Leaf. Range clamped to positive values (wind, precipitation). |
| `Delimiter` | Leaf. Punctuation that attaches to the preceding token without a space. |
| `WeatherTime` | Leaf. Clock-time rendering. |
| `TimePeriod` | Leaf. Period start/end rendering. |
| `Text` | Leaf. Literal text that bypasses the dictionary. |
| `SectionTag` | Marker. Paired tags inserted around each section so formatters can wrap markup around it. |
| `StoryTag` | Marker. Paired tags inserted around each story. |

---

## Dictionaries

See [dictionaries.md](dictionaries.md).

| Class | Purpose |
| --- | --- |
| `Dictionary` | Abstract base. `init(language)`, `find(key)`, `contains(key)`, `insert(key, phrase)`. |
| `BasicDictionary` | In-memory dictionary populated by code. Useful for programmatic setups and tests. |
| `FileDictionary` | Reads a pipe-delimited `key|translation` file for a single language. |
| `DebugDictionary` | Returns the key unchanged. Useful for inspecting generator output without translation noise. |
| `NullDictionary` | Always returns the empty string (suppresses output). |
| `DatabaseDictionary` | Abstract SQL-backed single-language dictionary. |
| `MySQLDictionary` | MySQL/MariaDB backend for `DatabaseDictionary`. |
| `PostgreSQLDictionary` | PostgreSQL backend. |
| `FileDictionaries` | Fronts several `FileDictionary` instances, one per language. |
| `DatabaseDictionaries` | Same idea for SQL backends. |
| `DictionaryFactory` | Creates a `Dictionary*` by name string (`"mysql"`, `"file"`, `"multimysqlplusgeneric"`, …). |

---

## Text formatters (Visitor)

See [text-formatters.md](text-formatters.md).

| Class | Purpose |
| --- | --- |
| `TextFormatter` | Abstract base. Visits each `Glyph` subtype and produces a formatted string. |
| `PlainTextFormatter` | Space-separated plain text. |
| `PlainLinesTextFormatter` | Plain text with one sentence per line. |
| `SingleLineTextFormatter` | Plain text, all on one line. |
| `HtmlTextFormatter` | HTML output with classed paragraphs. |
| `CssTextFormatter` | HTML with embedded CSS hooks around stories/sections. |
| `SpeechTextFormatter` | Text-to-speech oriented format. |
| `SoneraTextFormatter` | Customer-specific format for Sonera SMS products. |
| `WmlTextFormatter` | Wireless Markup Language output (legacy mobile). |
| `DebugTextFormatter` | Shows dictionary keys in square brackets. |
| `ExtendedDebugTextFormatter` | Same idea but more verbose; includes section/story tags. |
| `TextFormatterFactory` | Creates a `TextFormatter*` by name. |
| `TextFormatterTools` | Helpers shared by several formatters. |

---

## Stories

See [stories.md](stories.md).

| Class | Purpose |
| --- | --- |
| `Story` | Abstract base. `hasStory(name)` and `makeStory(name)`. |
| `StoryFactory` | Namespace. `create(name)` walks every parameter story class in turn and dispatches. |
| `TemperatureStory` | Owns all `temperature_*` generators. |
| `PrecipitationStory` | Owns `pop_*` and `precipitation_*`. |
| `CloudinessStory` | Owns `cloudiness_overview`. |
| `WeatherStory` | Owns `weather_overview`, `weather_forecast`, `weather_shortoverview`, `weather_thunderprobability*`. |
| `WindStory` | Owns `wind_overview`, `wind_anomaly`, `wind_range`, `wind_daily_ranges`, `wind_simple_overview`. |
| `FrostStory` | Owns `frost_onenight`, `frost_twonights`, `frost_day`, `frost_mean/maximum/range`. |
| `RelativeHumidityStory` | Owns the three `relativehumidity_*` stories. |
| `RoadStory` | Owns `roadwarning_*`, `roadcondition_*`, `roadtemperature_*`. |
| `ForestStory` | Owns `forestfireindex_twodays`, `forestfirewarning_county`, `evaporation_day`. |
| `DewPointStory` | Owns `dewpoint_range`. |
| `PressureStory` | Owns `pressure_mean`. |
| `WaveStory` | Owns `wave_range`. |
| `SpecialStory` | Owns non-weather stories: `none`, `date`, `text`, `table`. |
| `WeatherForecastStory` | Composition scaffolding used by `weather_forecast.cpp` to compose multi-part weather narratives. |

### Story-tools helpers

Shared code extracted from individual generators in the same family.

| Class / namespace | Purpose |
| --- | --- |
| `TemperatureStoryTools` | Day/night range helpers, comparative phrasing, coast sub-clause. |
| `PrecipitationStoryTools` | "Paikoin / monin paikoin" classification and precipitation-class helpers. |
| `WindStoryTools` | Wind speed/direction classification and phrase selection. |
| `CloudinessStoryTools` | Cloudiness classification, similarity-merging. |
| `FrostStoryTools` | Frost probability classification. |
| `RoadStoryTools` | Road condition classification and merging rules. |

### Narrative helpers (used by `weather_forecast` / `weather_overview`)

| Class | Purpose |
| --- | --- |
| `WeatherForecast` | Shared types and helpers for the `weather_overview` / `weather_forecast` family. |
| `CloudinessForecast` | Cloudiness classifier and narrative helper with multi-period merging. |
| `PrecipitationForecast` | Precipitation classifier with "paikoin / monin paikoin" tautology avoidance. |
| `FogForecast` | Fog-period classification (dense fog optionally enabled via `ENABLE_DENSE_FOG`). |
| `ThunderForecast` | Thunder-probability helper. |
| `WindForecast` | Wind narrative composition helper. |
| `WindForecastStructs` | Plain structs used by `WindForecast`. |

---

## Factories

Listed together for convenience. Each one is covered in the section for the
hierarchy it creates.

| Factory | Builds | Input |
| --- | --- | --- |
| `StoryFactory` (namespace) | `Paragraph` | Story name string |
| `HeaderFactory` (namespace) | `Header` | Header type name |
| `WeatherPeriodFactory` (namespace) | `WeatherPeriod` | Reads `::period::type` ("now", "until", …) |
| `PeriodPhraseFactory` (namespace) | `Sentence` | Type + variable prefix ("today", "tonight", …) |
| `DictionaryFactory` | `Dictionary*` | Backend name ("file", "mysql", …) |
| `TextFormatterFactory` | `TextFormatter*` | Formatter name ("plain", "html", …) |
| `UnitFactory` (namespace) | `Sentence` | Unit name ("celsius", "millimetres", …) |

---

## Period / time tools

| Class / namespace | Purpose |
| --- | --- |
| `NightAndDayPeriodGenerator` | Splits a forecast period into alternating night/day subperiods. |
| `MorningAndEveningPeriodGenerator` | Splits into morning / afternoon / evening / night subperiods. |
| `WeekdayTools` | Namespace. Weekday phrasing helpers (`on_weekday`, `night_against_weekday`). |

---

## Geographic masks

See [architecture.md#hierarchy-5--masksource-area-filtering](architecture.md#hierarchy-5--masksource-area-filtering).

The `MaskSource` base class itself lives in `calculator`, not `textgen`.

| Class | Purpose |
| --- | --- |
| `CoastMaskSource` | Restricts analysis to coastal grid points. |
| `InlandMaskSource` | Restricts to inland (non-coastal) points. |
| `LandMaskSource` | Excludes sea points. |
| `NorthernMaskSource` | Northern half of an area. |
| `SouthernMaskSource` | Southern half. |
| `EasternMaskSource` | Eastern half. |
| `WesternMaskSource` | Western half. |
| `NullMaskSource` | No restriction (identity). |
| `SubMaskExtractor` | Namespace. Builds an `NFmiIndexMask` for a location or sub-area from an `AnalysisSources`/`WeatherArea`. |

---

## Acceptors (value filters)

The `Acceptor` base class itself lives in `calculator`.

| Class | Purpose |
| --- | --- |
| `ValueAcceptor` | Accepts values inside a fixed `[min, max]` range. |
| `ComparativeAcceptor` | Accepts values satisfying one comparison (`>= x`, `< x`, …). |
| `PositiveValueAcceptor` | Accepts strictly positive values. |
| `AndAcceptor` | Logical AND of two acceptors. |
| `OrAcceptor` | Logical OR of two acceptors. |
| `TemplateAcceptor` | Template-style acceptor parameterised by a comparison functor. |

---

## Forecast-area and climatology tools

| Class / namespace | Purpose |
| --- | --- |
| `AreaTools` | Namespace. Geographic helpers (`forecast_area_id`, coast/inland dispatch). |
| `ClimatologyTools` | Namespace. Climatological reference lookups. |
| `GridClimatology` | Gridded climatology dataset. |
| `SeasonTools` | Namespace. Season detection (winter/spring/summer/autumn) for threshold overrides. |
| `TemperatureTools` | Namespace. Temperature-specific helpers (rounding, range formatting). |
| `TemperatureRange` | Value struct: minimum/maximum temperature pair. |

---

## External data

| Class | Purpose |
| --- | --- |
| `PostGISDataSource` | Reads area polygons from a PostGIS database. |
| `FireWarnings` | Reads the daily forest-fire warning file used by `forestfirewarning_county`. |

---

## Small utilities

| Class / namespace | Purpose |
| --- | --- |
| `WeatherFact` | Small container pairing a fact value with a `WeatherPeriod`. |
| `WeatherObserver` | Abstract. Observes actual (measured) weather. Rarely used. |
| `WeatherStatistician` | Abstract. Computes statistics from observations. Rarely used. |
| `MessageLogger` | Indented scope-based debug logger. |
| `MessageLoggerStream` | Output-stream adapter for `MessageLogger`. |
| `PrecipitationPeriodTools` | Namespace. Helpers for splitting a period by rain onset/end. |
