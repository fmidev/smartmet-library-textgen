# Architecture

TextGen has four layered hierarchies and a small set of factories.
This page walks through a `TextGenerator::generate()` call end-to-end and
then zooms into each hierarchy.

## Request flow

```
    ┌─────────────────────────────────────────────────────────┐
    │ caller: textgenapps (or equivalent)                     │
    └─────────────────────────────────────────────────────────┘
                                │
                                │  TextGenerator::generate(WeatherArea)
                                ▼
    ┌─────────────────────────────────────────────────────────┐
    │ TextGenerator (TextGenerator.cpp)                       │
    │ ─ reads textgen::sections                               │
    │ ─ for each section:                                     │
    │       build WeatherPeriod via WeatherPeriodFactory      │
    │       build Header       via HeaderFactory              │
    │       for each name in ::content:                       │
    │           StoryFactory::create(name)  ── builds Paragraph│
    └─────────────────────────────────────────────────────────┘
                                │
                                ▼
    ┌─────────────────────────────────────────────────────────┐
    │ StoryFactory (StoryFactory.cpp)                         │
    │ tries every parameter Story class in turn; the first    │
    │ whose hasStory(name) returns true owns the name.        │
    └─────────────────────────────────────────────────────────┘
                                │
                                ▼
    ┌─────────────────────────────────────────────────────────┐
    │ <Parameter>Story  (e.g. WindStory)                      │
    │ WindStory::makeStory("wind_overview") dispatches to     │
    │ the generator WindStory::overview(), whose body lives   │
    │ in wind_overview.cpp.                                   │
    └─────────────────────────────────────────────────────────┘
                                │
                                ▼
    ┌─────────────────────────────────────────────────────────┐
    │ Generator (e.g. wind_overview.cpp)                      │
    │ pulls analysis from AnalysisSources, reads its          │
    │ threshold/phrase variables from Settings, emits a       │
    │ Paragraph built from Sentence / Phrase / Number Glyphs. │
    └─────────────────────────────────────────────────────────┘
                                │
                                ▼
    ┌─────────────────────────────────────────────────────────┐
    │ Document (GlyphContainer of Headers + Paragraphs)       │
    │ realize(TextFormatter)  →  final string                 │
    └─────────────────────────────────────────────────────────┘
```

Two pattern labels help keep the vocabulary straight:

* **Composite** — everything user-visible is a `Glyph`. `Document`,
  `Paragraph`, `Sentence`, `Header`, `TimePhrase` are `GlyphContainer`s.
  `Phrase`, `Integer`, `Real`, `Delimiter`, `LocationPhrase`, … are leaves.
* **Visitor** — `TextFormatter` (plain text, HTML, CSS, speech, debug)
  visits every glyph type. Each subclass decides how the glyph becomes
  a string.

## Hierarchy 1 — Glyph (the Composite)

```
Glyph
├── GlyphContainer
│   ├── Document
│   ├── Header
│   ├── Paragraph
│   ├── Sentence
│   └── TimePhrase
├── Delimiter
├── Integer
├── IntegerRange
├── LocationPhrase
├── Phrase
├── PositiveRange
├── Real
├── RealRange
├── SectionTag
├── StoryTag
├── Text
├── TimePeriod
└── WeatherTime
```

See [glyphs.md](glyphs.md) for details on what each leaf/container does.

## Hierarchy 2 — Dictionary

```
Dictionary
├── BasicDictionary          (in-memory; used programmatically and in tests)
├── FileDictionary           (pipe-delimited file, one language)
├── DebugDictionary          (returns the key unchanged; for debugging)
├── DatabaseDictionary       (single-language MySQL/MariaDB lookup)
│   └── (MySQLDictionary / PostgreSQLDictionary live under this)
└── NullDictionary           (always returns empty string)
```

Two **aggregator** classes wrap a set of language-specific dictionaries:

* `FileDictionaries` — fronts N `FileDictionary` instances, one per language.
* `DatabaseDictionaries` — same idea for DB backends.

`DictionaryFactory::create(name)` returns a `Dictionary*` by name string
("mysql", "postgresql", "file", "multimysqlplusgeneric", …). `TheDictionary`
is an optional singleton wrapper. See [dictionaries.md](dictionaries.md).

## Hierarchy 3 — TextFormatter (the Visitor)

```
TextFormatter
├── PlainTextFormatter
│   ├── PlainLinesTextFormatter
│   └── SingleLineTextFormatter
├── HtmlTextFormatter
├── CssTextFormatter
├── SpeechTextFormatter      (TTS-oriented)
├── SoneraTextFormatter      (customer-specific format)
├── WmlTextFormatter         (legacy WML output)
├── DebugTextFormatter
└── ExtendedDebugTextFormatter
```

`TextFormatterFactory::create(name)` returns one by name string.

## Hierarchy 4 — Story

```
Story                       (interface: makeStory, hasStory)
├── TemperatureStory         → temperature_mean, temperature_max36hours, ...
├── PrecipitationStory       → pop_max, precipitation_classification, ...
├── CloudinessStory          → cloudiness_overview
├── WeatherStory             → weather_overview, weather_forecast, ...
├── WindStory                → wind_overview, wind_anomaly, ...
├── FrostStory               → frost_onenight, frost_twonights, ...
├── RelativeHumidityStory    → relativehumidity_day, ...
├── RoadStory                → roadcondition_overview, roadwarning_overview, ...
├── ForestStory              → evaporation_day, forestfirewarning_county, ...
├── DewPointStory            → dewpoint_range
├── PressureStory            → pressure_mean
├── WaveStory                → wave_range
└── SpecialStory             → none, date, text, table
```

`StoryFactory::create(name)` walks this list and dispatches to the first
class that claims the name. See [stories.md](stories.md).

## Hierarchy 5 — MaskSource (area filtering)

```
MaskSource                 (in calculator; not owned by textgen)
├── CoastMaskSource
├── InlandMaskSource
├── LandMaskSource
├── EasternMaskSource
├── WesternMaskSource
├── NorthernMaskSource
├── SouthernMaskSource
└── NullMaskSource
```

Used to restrict an analysis to a geographic sub-area (`rannikolla`,
`sisämaassa`). See [class index](classes.md#mask-sources) for a per-class
blurb.

## Hierarchy 6 — Acceptor (value filtering)

```
Acceptor                   (in calculator; not owned by textgen)
├── ValueAcceptor
├── ComparativeAcceptor
├── PositiveValueAcceptor
├── AndAcceptor
├── OrAcceptor
└── TemplateAcceptor
```

Used with analysis functions to skip values that should not count (e.g.
ignore precipitation under 0.1 mm). See [class index](classes.md#acceptors).

## Factories at a glance

| Factory | Produces | Input name examples |
| --- | --- | --- |
| `StoryFactory` | `Paragraph` | `"wind_overview"`, `"temperature_mean"` |
| `HeaderFactory` | `Header` | `"until"`, `"several_days"`, `"none"` |
| `WeatherPeriodFactory` | `WeatherPeriod` | reads `::period::type` ("now", "until", …) |
| `PeriodPhraseFactory` | `Sentence` | type + variable prefix ("today", "tonight", …) |
| `DictionaryFactory` | `Dictionary*` | `"file"`, `"mysql"`, `"multimysqlplusgeneric"` |
| `TextFormatterFactory` | `TextFormatter*` | `"plain"`, `"html"`, `"speech"` |
| `UnitFactory` | `Sentence` | `"celsius"`, `"millimetres"`, … |

## Configuration surface

Everything tunable lives under the `textgen::` key in a `libconfig` file,
read through `calculator/Settings.h`. See the user guide's
[configuration.md](../users/configuration.md) for the variable layout.

## Where the algorithm code lives

Very small stories (< 100 LOC) are often inline methods of the owning Story
class. Anything larger lives in its own lowercase file:

* `wind_overview.cpp` implements `WindStory::overview`
* `temperature_max36hours.cpp` implements `TemperatureStory::max36hours`

See the [user catalogue](../users/stories/README.md) for size + status tags.
