# Getting started

This page shows the smallest useful `textgen` pipeline: one section, one
story, one line of output. See [configuration.md](configuration.md) for the
full layout and [stories/](stories/README.md) for what each story does.

## Context

TextGen is a C++ library, not a standalone CLI. It is normally driven from
[smartmet-textgenapps](https://github.com/fmidev/smartmet-textgenapps), which
loads a `libconfig` file and calls `TextGenerator::generate()`. If you just
want to try the library from code, the skeleton is:

```cpp
#include <calculator/LatestWeatherSource.h>
#include <calculator/Settings.h>
#include <calculator/WeatherArea.h>
#include <textgen/Document.h>
#include <textgen/PlainTextFormatter.h>
#include <textgen/TextGenerator.h>
#include <textgen/DictionaryFactory.h>
#include <textgen/FileDictionaries.h>

int main()
{
  using namespace TextGen;

  // 1. Load the libconfig configuration.
  Settings::read("my-config.conf");

  // 2. Set up a dictionary for the target language.
  auto dict = DictionaryFactory::create("multimysqlplusgeneric");
  dict->init("en");

  // 3. Point the generator at its data source.
  TextGenerator gen;
  gen.sources(/* AnalysisSources wired up to QueryData/GRIB/... */);
  gen.time(TextGenPosixTime::Now());

  // 4. Generate for a weather area (by name or polygon).
  WeatherArea area("Helsinki");
  Document doc = gen.generate(area);

  // 5. Format the Document into a plain string.
  PlainTextFormatter fmt;
  fmt.dictionary(dict);
  std::cout << doc.realize(fmt) << std::endl;
}
```

## Minimal configuration

```
# my-config.conf

# --- Forecast data ---
textgen::forecast = "/smartmet/data/pal/querydata/pal"

# --- One-section text ---
textgen::sections = "day1"

textgen::day1::period::type    = "until"
textgen::day1::period::days    = 0
textgen::day1::period::endhour = 18

textgen::day1::header  = "none"
textgen::day1::content = "temperature_range"

# --- Story configuration ---
textgen::day1::story::temperature_range::mininterval = 2
```

That prints something like:

```
Temperature is 10...14 degrees.
```

## What to read next

* **Add more stories.** Pick one from the [story catalogue](stories/README.md)
  and add it to `::content`. Prefer the *Primary* ones for real products.
* **Customise the period.** See [periods.md](periods.md) for `until`,
  `from_until`, and multi-day period types.
* **Add a title.** Switch `::header` from `"none"` to a real type — see
  [headers/](headers/README.md).
* **Split into days.** Set `::subperiods = true` to get one story sequence
  per forecast day. See [subperiods.md](subperiods.md).
* **Fake analysis values for testing.** Every story supports
  `::fake::*` keys — see [configuration.md#fake-values](configuration.md#fake-values-testing--debugging).
