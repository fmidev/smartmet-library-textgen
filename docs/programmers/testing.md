# Testing

TextGen's tests live under `test/` and use SmartMet's own `regression/tframe.h`
framework (not Boost.Test). Each `*Test.cpp` compiles to its own executable; the
`test` target runs them all in sequence and reports a total error count.

## Running the tests

```bash
cd textgen
make                           # build libsmartmet-textgen.so
cd test
make                           # build the test executables
make test                      # run them
```

If tests fail with a shared-library load error, point the loader at
the freshly built library:

```bash
LD_LIBRARY_PATH=.. make test
```

`make test` links against `../libsmartmet-textgen.so`, the local build.

## Running a single test

```bash
cd test
make TemperatureStoryTest && ./TemperatureStoryTest
```

tframe runs every case registered in the file's `tests::test()` method; it has no
command-line selection of cases. To run one case while debugging, comment out the
other `TEST(...)` lines temporarily.

## Layout of a story test

Every story test follows the same template
(`test/TemperatureStoryTest.cpp` is a good reference):

```cpp
#include "Dictionary.h"
#include "DictionaryFactory.h"
#include "PlainTextFormatter.h"
#include "TemperatureStory.h"
#include <calculator/Settings.h>
#include <regression/tframe.h>

std::shared_ptr<TextGen::Dictionary> dict;
TextGen::PlainTextFormatter formatter;

void require(const TextGen::Story& story,
             const std::string& language,
             const std::string& name,
             const std::string& expected)
{
    dict->init(language);
    formatter.dictionary(dict);
    TextGen::Paragraph para = story.makeStory(name);
    const std::string got = para.realize(formatter);
    if (got != expected)
        TEST_FAILED((got + " <> " + expected).c_str());
}

void test_temperature_mean()
{
    using namespace TextGen;

    TemperatureStory story(forecastTime, sources, area, period, "mean");
    const std::string fun = "temperature_mean";

    Settings::set("mean::fake::mean", "0.1,0");
    require(story, "fi", fun, "Keskilämpötila 0 astetta.");
    require(story, "sv", fun, "Medeltemperaturen 0 grader.");
    require(story, "en", fun, "Mean temperature 0 degrees.");

    TEST_PASSED();
}
```

The `require()` helper drives the same story with three languages and
asserts each translated output. This is why every generator doc ends
with a phrase vocabulary list — each phrase needs a `key|translation`
entry in every test dictionary.

## The `fake::*` convention

Every story supports a `fake::` subtree that bypasses the analyser and
injects a fixed result into the generated text:

```cpp
Settings::set("my_story::fake::mean", "10.5,0");
```

The value is `"<result>,<stddev>"` — a `WeatherResult` literal. The
standard deviation is the *quality* ([see Quality](../users/quality.md));
use `0` for "exact" and a positive number to simulate poor-quality
data (which will cause automatic period splitting).

The `fake::*` keys a story understands are the same ones its source
passes to `GridForecaster::analyze()`. The cleanest way to discover
them is:

```bash
grep '"::fake::' textgen/my_story.cpp
```

or look at an existing test case — every test configuration enumerates
exactly the keys it uses.

## Test dictionaries

The story tests use the `po` dictionary (`DictionaryFactory::create("po")`), which
reads `<language>.po` files from the directory named by `textgen::podictionaries`.
**Most tests do not set it, so they read the installed files in
`/usr/share/smartmet/textgen`**, not the repository's `po/` directory; only
`SpecialStoryTest` points it at `../po`. To test an edited `.po` file, install it
first, or set `textgen::podictionaries` to `../po` in the test.

The repository's `po/` directory has 23 files: `ar`, `da`, `de`, `en`, `en-marine`
(for `weather_forecast_at_sea`), `es`, `et`, `fi`, `fr`, `id`, `it`, `ja`, `ko`, `lv`,
`nl`, `no`, `ru`, `sonera`, `sv`, `sw`, `th`, `vi` and `zh`. `test/dictionaries/` only
holds pipe-delimited `en.txt` and `fi.txt` (`key|translation`) for the
`FileDictionary` tests.

Adding a new phrase means:

1. Add the key and translation to **every** `.po` file. A missing
   translation will cause the test to emit the raw key, which will
   fail the equality check in `require()`.
2. Reference the key via `Phrase("new-key")` in the generator.
3. Add expected-text assertions in the relevant `*Test.cpp` for each
   language.

## Integration with CI

CI (`.circleci/config.yml`) runs the same `make test` target inside
the `fmidev/smartmet-cibase-{8,10}` Docker images. Any test that
passes locally should also pass in CI, modulo environment-specific
differences (timezone, locale).

## Writing a new story test

1. Copy an existing `*StoryTest.cpp` that tests the same parameter
   family (e.g. `WindStoryTest.cpp` for a new wind story).
2. Add the `fake::*` assertions for the values you expect to drive
   each branch of the algorithm. Aim for branch coverage —
   one test case per distinct sentence the story can emit.
3. Add the new phrase keys to every `.po` file.
4. Register the test function with `TEST(name)` in the `tests::test()` method at
   the bottom of the file.
5. `make <TestName> && ./<TestName>`.

## Debugging output

`DebugTextFormatter` replaces dictionary lookups with the raw key
surrounded by square brackets. Substitute it for `PlainTextFormatter`
in the test harness to see exactly which keys your generator produced:

```cpp
TextGen::DebugTextFormatter dbg;
dbg.dictionary(dict);
std::cerr << para.realize(dbg) << std::endl;
```

`ExtendedDebugTextFormatter` additionally emits `SectionTag` and
`StoryTag` markers, useful when debugging a multi-section `Document`.

## See also

* [Stories](stories.md) — for how `StoryFactory::create()` dispatches
  a name to a generator.
* [Dictionaries](dictionaries.md) — for the dictionary interface the
  tests rely on.
* [Text formatters](text-formatters.md) — for the choice between
  `PlainTextFormatter` and `DebugTextFormatter`.
