# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

`smartmet-library-textgen` generates natural-language weather forecast text from gridded QueryData. Given forecast data, a configuration, and a language, it produces human-readable weather summaries (temperature, precipitation, wind, cloudiness, frost, road conditions, etc.). Used by [smartmet-textgenapps](https://github.com/fmidev/smartmet-textgenapps) for automated forecast production.

## Build commands

```bash
make                # Build libsmartmet-textgen.so
make test           # Run all 47 test executables
make format         # clang-format all source and test files
make clean          # Clean build artifacts
make install        # Install headers to smartmet/textgen/ and library
make rpm            # Build RPM package
```

To run a single test:

```bash
cd test && make BasicDictionaryTest && ./BasicDictionaryTest
```

Tests link against the locally built `../libsmartmet-textgen.so`. If you get linker errors at runtime, set `LD_LIBRARY_PATH=.. make test`.

## Dependencies

Libraries: `smartmet-calculator`, `smartmet-newbase`, `smartmet-macgyver`, `boost_locale`, `boost_thread`, `mysqlpp`, `libpqxx`, `gdal`. MySQL/MariaDB dev headers needed (`-isystem /usr/include/mysql`).

## Architecture

The library uses the **Composite pattern** for document structure and the **Visitor pattern** for text rendering.

### Glyph hierarchy (Composite)

`Glyph` is the abstract base for all text elements. Each glyph implements `realize(Dictionary)` for simple string output and `realize(TextFormatter)` for formatted output via double dispatch.

```
Glyph (abstract)
├── GlyphContainer (composite base: std::list<shared_ptr<Glyph>>)
│   ├── Document      — top-level container
│   ├── Header        — document header with optional forecast time
│   ├── Paragraph     — group of sentences
│   └── Sentence      — auto-capitalizes first word, adds period
├── Phrase            — dictionary key lookup
├── Integer / Real    — numeric values with unit formatting
├── IntegerRange      — "X...Y" ranges
├── PositiveRange     — ranges clamped to positive values (wind/precip)
└── TimePhrase        — time-of-day expressions
```

### Dictionary system (multilingual support)

`Dictionary` (abstract) maps phrase keys to translated strings. Implementations:
- `PoDictionary` / `PoDictionaries` — gettext `.po` file backend; **recommended** new path
- `FileDictionary` — reads pipe-delimited files (`key|translation`), used in tests
- `DatabaseDictionary` / `PostgreSQLDictionary` / `MySQLDictionary` — SQL backends, **deprecated** and scheduled for removal after the PO-dictionary release
- `BasicDictionary` — in-memory, for programmatic use

See `docs/programmers/dictionaries.md` for the full hierarchy and
`docs/users/transitioning-from-localized-database-translations.md` for
migrating database-backed deployments.

Test dictionaries live in `test/dictionaries/` (22 languages: fi, en, sv, de, fr, es, it, ru, ja, ko, zh, ar, etc.).

### TextFormatter system (Visitor)

`TextFormatter` (abstract) visits each glyph type to produce output. Implementations: `PlainTextFormatter`, `HtmlTextFormatter`, `CssTextFormatter`, `SpeechTextFormatter`, `PlainLinesTextFormatter`, `SingleLineTextFormatter`, and others. Created via `TextFormatterFactory`.

### Story system (content generation)

Each weather parameter has a `Story` subclass: `TemperatureStory`, `PrecipitationStory`, `WindStory`, `CloudinessStory`, `WeatherForecastStory`, `FrostStory`, `PressureStory`, `WaveStory`, `RoadStory`, `ForestStory`, etc. Stories produce `Paragraph` objects from analysis data.

`StoryFactory::create()` dispatches by name string (e.g. `"temperature_range"`, `"wind_overview"`) — see `StoryFactory.cpp` for the full map.

### TextGenerator (entry point)

`TextGenerator` ties everything together: takes `AnalysisSources` + `WeatherArea` + configuration → generates a `Document`. Uses Pimple idiom. Configuration is read via `calculator/Settings.h` (libconfig-based global settings).

### Supporting infrastructure

- **Mask sources**: `CoastMaskSource`, `InlandMaskSource`, `LandMaskSource`, `EasternMaskSource`, etc. — geographic region masking for area-specific forecasts
- **Acceptors**: `AndAcceptor`, `OrAcceptor`, `ComparativeAcceptor`, etc. — filter weather values
- **Tools namespaces**: `TemperatureStoryTools`, `WindStoryTools`, `PrecipitationStoryTools`, etc. — shared helper functions used across story implementations

## Namespace

Everything is in `namespace TextGen`. Internal includes use `#include "Header.h"`, SmartMet dependencies use `#include <calculator/AnalysisSources.h>`.

## Documentation

`docs/` contains Finnish-language documentation for individual story algorithms, period phrases, and configuration variables. Each `story_*.txt` file documents the configuration parameters and output logic for the corresponding story.
