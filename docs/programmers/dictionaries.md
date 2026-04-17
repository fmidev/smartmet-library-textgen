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

Factory names in current use include `"po"` and `"multipo"` (recommended),
plus legacy values `"file"`, `"mysql"`, `"postgresql"`,
`"multimysqlplusgeneric"`, and similar combinations. The SQL-backed names
are deprecated and will be removed after the PO-dictionary release.

## Global dictionary

The dictionary is needed almost everywhere. Threading it through every call
site as a parameter is impractical, so a process-wide singleton is
traditionally used: `TheDictionary`.

```cpp
TheDictionary::instance().dictionary(DictionaryFactory::create("mysql"));
TheDictionary::instance().init("fi");
std::cout << TheDictionary::instance().find("sää");
```

`TheDictionary` exposes the same interface as `Dictionary`; the additional
`dictionary()` method is what installs the backing object.

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

Test dictionaries live under `test/dictionaries/` in both the legacy
pipe-delimited form (`<lang>.txt`) and the new gettext form
(`<lang>.po`), for the 22 languages the library is actively translated
into (`fi`, `en`, `sv`, `de`, `fr`, `es`, `it`, `ru`, `ja`, `ko`, `zh`,
`ar`, …). The `.po` files are generated from the `.txt` sources by
`tools/txt_to_po.py` and will replace the `.txt` files once all
downstream tests have moved over. A test typically loads one
`PoDictionary` (or `FileDictionary`) per language and runs the target
generator against each.

## See also

* [glyphs.md](glyphs.md) — how `Phrase` hooks into the Glyph tree
* [classes.md#dictionaries](classes.md#dictionaries) — one-liner per class
