# Glyphs: the Composite hierarchy

Every unit of composable text in TextGen is a `Glyph`. A final `Document` is
just a tree of Glyphs. This page covers the tree shape, the key leaf types,
and how `realize()` resolves each node.

## Hierarchy

```
Glyph                           (abstract)
├── GlyphContainer              (owns std::list<std::shared_ptr<Glyph>> of children)
│   ├── Document
│   ├── Header
│   ├── Paragraph
│   ├── Sentence
│   └── TimePhrase
├── Delimiter
├── Integer
├── Real
├── IntegerRange
├── RealRange
├── PositiveRange
├── Phrase
├── LocationPhrase
├── Text
├── TimePeriod
├── WeatherTime
├── SectionTag
└── StoryTag
```

## The base class

```cpp
class Glyph
{
 public:
  virtual ~Glyph();
  virtual std::shared_ptr<Glyph> clone() const = 0;
  virtual std::string realize(const Dictionary&) const = 0;
  virtual std::string realize(const TextFormatter&) const = 0;
  virtual bool isDelimiter() const = 0;
};
```

Two `realize()` overloads implement **double dispatch**: a
formatter visiting a container calls each child's `realize(formatter)`,
which virtually dispatches on the Glyph type *and* on the formatter type
at once. That is how the same `Document` can be rendered as plain text,
HTML, speech, or debug output without adding code to every Glyph.

## Containers

### `Document`

What `TextGenerator::generate()` returns. Holds a mixed sequence of
`Header`, `Paragraph`, and marker tags (`SectionTag`, `StoryTag`).

### `Paragraph`

A list of `Sentence` glyphs. Formatters typically insert paragraph breaks
between Paragraphs inside a Document.

### `Sentence`

A list of leaf glyphs. `Sentence::realize()` auto-capitalises the first
word and appends a period. Delimiters (`Delimiter(",")`) attach to the
preceding token without whitespace.

### `Header`

Same structural role as a Sentence but represents a title rather than a body
sentence. Built by `HeaderFactory`.

### `TimePhrase`

A Sentence specialised to describe a time period ("tänään",
"huomisaamusta alkaen"). Built by `PeriodPhraseFactory`.

## Numeric leaves

`Integer`, `Real`, `IntegerRange`, `RealRange`, and `PositiveRange` all
render to a number string using a formatting policy. Integers print
directly; floats use a default single-decimal format with trailing `.0`
stripped. You can plug in a custom policy — for example, a wind-direction
policy that rounds to the nearest 45°.

## Phrase leaves

`Phrase("key")` defers its rendering to the `Dictionary`. At render time
the dictionary is asked for the translation of `"key"` for the configured
language and the result is emitted.

`LocationPhrase` wraps a place name: it returns the name capitalised
(including hyphen-separated parts) unless the dictionary has an override
entry.

`Text` is a literal string that bypasses the dictionary entirely.

## Marker leaves

`SectionTag` and `StoryTag` are paired open/close markers that
`TextGenerator` inserts around each section and each story. They are
invisible in `PlainTextFormatter` output but carry the hooks that
`HtmlTextFormatter` / `CssTextFormatter` / `SoneraTextFormatter` use to
wrap markup around the corresponding paragraph or clause.

## Building a sentence

```cpp
Sentence s;
s << Phrase("kaakko")             // dictionary key "kaakko" (south-east)
  << Integer(12);
std::cout << s.realize(dict);     // "South east 12."
```

Types have default conversions so bare strings and ints work too:

```cpp
Sentence s;
s << "kaakko" << 12;              // identical to the above
```

Delimiters stick to the preceding token:

```cpp
Sentence s;
s << "kaakko" << 12 << Delimiter(",") << "etelä" << 13;
// "Kaakko 12, etelä 13."
```

Paragraphs compose sentences:

```cpp
Paragraph p;
Sentence s1; s1 << "ke" << "kaakko" << 12;
Sentence s2; s2 << "to" << "luode" << 13;
p << s1 << s2;
// "Wed south east 12. Thu north west 13."
```

## See also

* [dictionaries.md](dictionaries.md) — how `Phrase("key")` resolves
* [text-formatters.md](text-formatters.md) — the Visitor side of double
  dispatch
