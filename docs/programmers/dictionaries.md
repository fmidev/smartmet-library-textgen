# Dictionaries

A **Dictionary** maps phrase keys (like `"kaakko"` or `"hallan todennäköisyys"`)
to their translation in a given language. `Phrase("key").realize(dict)`
looks up the key and returns the translated string.

## Hierarchy

```
Dictionary                      (abstract)
├── BasicDictionary             (in-memory)
├── FileDictionary              (pipe-delimited key|translation file, legacy)
├── PoDictionary                (gettext .po file — recommended)
├── DebugDictionary             (returns the key unchanged)
├── NullDictionary              (returns empty string)
└── DatabaseDictionary          (abstract, SQL-backed, one language — deprecated)
```

Concrete SQL backends inherit from `DatabaseDictionary`:
`MySQLDictionary`, `PostgreSQLDictionary`. Both are **deprecated** and
will be removed after the PO-dictionary release; new deployments should
use `PoDictionary` / `PoDictionaries`.

Three **multi-language aggregators** wrap N single-language dictionaries,
dispatching on the language passed to `init()`:

```
FileDictionaries       holds one FileDictionary per language
PoDictionaries         holds one PoDictionary per language (recommended)
DatabaseDictionaries   holds one DatabaseDictionary per language (deprecated)
```

## API

```cpp
class Dictionary
{
 public:
  virtual void init(const std::string& theLanguage) = 0;
  virtual bool contains(const std::string& theKey) const = 0;
  virtual const std::string& find(const std::string& theKey) const = 0;
  virtual void insert(const std::string& theKey,
                      const std::string& thePhrase) = 0;

  virtual size_type size() const = 0;
  virtual bool empty() const = 0;
};
```

## Typical usage

Dictionaries are created via the factory and initialised for a language:

```cpp
auto dict = DictionaryFactory::create("mysql");
dict->init("fi");
std::cout << dict->find("sää");
```

The factory names are `"po"` and `"multipo"` (recommended), `"null"`,
`"basic"`, `"file"`, `"multifile"`, and the legacy `"mysql"`, `"multimysql"`,
`"postgresql"` and `"multipostgresql"`; any other name throws. The SQL-backed names
are deprecated and will be removed after the PO-dictionary release.

## Global dictionary

There is no process-wide dictionary singleton. The application creates the
dictionary and hands it to the text formatter (`TextFormatter::dictionary()`),
or passes it to `Glyph::realize(const Dictionary&)` directly:

```cpp
std::shared_ptr<Dictionary> dict(DictionaryFactory::create("multipo"));
dict->init("fi");
PlainTextFormatter formatter;
formatter.dictionary(dict);
std::cout << document.realize(formatter);
```

## Relationship to `Phrase`

```cpp
Sentence s;
s << Phrase("kaakko");      // just stores the key
std::cout << s.realize(dict);
//                 ^-- Phrase asks dict->find("kaakko") here
```

`Phrase::realize(Dictionary&)` and `Phrase::realize(TextFormatter&)` both
resolve through the dictionary — the formatter variant routes via the
formatter's own dictionary reference, which is how HTML / speech / debug
formats can share the same underlying language data.

## Test fixtures

The translations are the gettext `.po` files in `po/` (23 files: `fi`, `en`,
`sv`, `de`, `fr`, `es`, `it`, `ru`, `ja`, `ko`, `zh`, `ar`, `lv`, …, plus
`en-marine` and `sonera`), which are installed to `/usr/share/smartmet/textgen`.
The story tests read them through `PoDictionaries` from
`textgen::podictionaries`, which defaults to that installed directory.
`test/dictionaries/` keeps only `en.txt` and `fi.txt` (pipe-delimited
`key|translation`) for the `FileDictionary` tests. `tools/txt_to_po.py`
converted the old `.txt` fixtures into `.po` files.

## See also

* [glyphs.md](glyphs.md) — how `Phrase` hooks into the Glyph tree
* [classes.md#dictionaries](classes.md#dictionaries) — one-liner per class
