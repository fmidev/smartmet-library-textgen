# Text formatters

A **TextFormatter** is the Visitor side of TextGen's double dispatch. It
visits each Glyph type and decides how that glyph becomes a string.
Swapping the formatter is how the same `Document` becomes plain text, HTML,
CSS-styled output, TTS-friendly speech text, or a debug dump.

## Hierarchy

```
TextFormatter                     (abstract)
├── PlainTextFormatter
│   ├── PlainLinesTextFormatter   (one sentence per line)
│   └── SingleLineTextFormatter   (everything on one line)
├── HtmlTextFormatter
├── CssTextFormatter              (HTML + class hooks)
├── SpeechTextFormatter           (TTS-friendly)
├── SoneraTextFormatter           (Sonera SMS format)
├── WmlTextFormatter              (legacy Wireless Markup Language)
├── DebugTextFormatter            (shows dictionary keys)
└── ExtendedDebugTextFormatter    (adds section/story tags)
```

## Interface

Each concrete formatter overrides a `visit(const X&)` method for every
Glyph subtype: `Document`, `Header`, `Paragraph`, `Sentence`, `Phrase`,
`Integer`, `Real`, `Delimiter`, `SectionTag`, `StoryTag`, …

The base class provides sensible defaults for most leaves and delegates
container-handling to subclasses.

## Typical usage

```cpp
PlainTextFormatter fmt;
fmt.dictionary(dict);
std::string out = doc.realize(fmt);
```

or via the factory:

```cpp
auto fmt = TextFormatterFactory::create("html");
fmt->dictionary(dict);
std::cout << doc.realize(*fmt);
```

## What each formatter does

| Formatter | Output | When to reach for it |
| --- | --- | --- |
| `PlainTextFormatter` | Space-separated prose. | Default. Regression tests, CLI. |
| `PlainLinesTextFormatter` | One sentence per line. | Readable logs, email body. |
| `SingleLineTextFormatter` | Everything on a single line. | SMS-like outputs. |
| `HtmlTextFormatter` | Paragraph / span HTML. | Web product output. |
| `CssTextFormatter` | HTML with class hooks on sections and stories. | Styled web product output. |
| `SpeechTextFormatter` | TTS-friendly text (number expansion, unit words). | Audio weather bulletins. |
| `SoneraTextFormatter` | Sonera-specific format. | Legacy Sonera SMS product. |
| `WmlTextFormatter` | WML markup. | Legacy mobile output. Likely unused now. |
| `DebugTextFormatter` | Dictionary keys in square brackets. | Debugging phrase lookup. |
| `ExtendedDebugTextFormatter` | Debug output with `SectionTag` / `StoryTag` visible. | Diagnosing which story produced which clause. |

## Why double dispatch

`Sentence::realize(TextFormatter&)` calls `fmt.visit(*this)`. That one line
dispatches on (1) the sentence/paragraph/phrase type — because the method is
virtual in `Glyph` — and (2) the formatter type — because `visit()` is
virtual in `TextFormatter`. The net effect is that adding a new formatter
requires no changes in `Glyph`, and adding a new Glyph requires one new
`visit()` overload in each formatter (typically handled in the base class
with a sensible default).

## Tools and shared helpers

`TextFormatterTools` is a namespace of helpers shared by several concrete
formatters: capitalisation, delimiter attachment, language-aware whitespace.

## See also

* [glyphs.md](glyphs.md) — the Composite side of the dispatch
* [classes.md#text-formatters-visitor](classes.md#text-formatters-visitor) —
  one-liner per class
