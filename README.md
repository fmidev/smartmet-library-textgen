# smartmet-library-textgen

Part of [SmartMet Server](https://github.com/fmidev/smartmet-server). See the [SmartMet Server documentation](https://github.com/fmidev/smartmet-server) for an overview of the ecosystem.

## Overview

The TextGen library contains algorithms for generating human-readable weather forecast text from gridded weather forecast data (QueryData). Given forecast data and a configuration specifying the forecast area and language, it produces natural-language weather summaries.

## Features

- Natural-language weather text generation from QueryData
- Multi-language output support
- Configurable forecast periods and areas
- Modular story generation (temperature, precipitation, wind, etc.)

## Usage

Used by [smartmet-textgenapps](https://github.com/fmidev/smartmet-textgenapps) for automated weather forecast text production.

## Documentation

Documentation lives in [`docs/`](docs/README.md) as
GitHub-flavoured Markdown and renders directly on GitHub. It is split
into two tracks:

- [`docs/users/`](docs/users/README.md) — for product configurators: story
  catalogue with status tags, configuration layout, periods, headers,
  period phrases.
- [`docs/programmers/`](docs/programmers/README.md) — for library
  developers: architecture, class index, Glyph / Dictionary / TextFormatter
  / Story hierarchies, plus testing and narrative-helper pages.

The top-level [`docs/README.md`](docs/README.md) links to both.

Up to and including the 2005-era material, the project's developer
documentation was written in Finnish with Doxygen markup and generated
into HTML by a separate Doxygen build. That toolchain has been dropped.
The current pages are the source of truth; contributors should edit the
`.md` files directly and commit them alongside the code change they
document. (Some `\brief` / `\see` tokens still appear in the C++
headers — these are inert in the build and can be removed
opportunistically.)

## License

This library is not currently public.

## Contributing

Contact us at beta@fmi.fi for more information.
