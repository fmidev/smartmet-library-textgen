# Story "table"

> **Status:** Specialized. Renders a table from configuration.
>
> **Owner:** `SpecialStory`.

Dispatched by `SpecialStory::hasStory("table")`. Emits a structured table
(typically rendered as an HTML table by `HtmlTextFormatter`, or as aligned
plain text otherwise).

## Configuration root

```
textgen::[section]::story::table::*
```

The table structure (columns, rows, data source) is specified entirely in
configuration. See `textgen/SpecialStory.cpp` — the `table()` method
documents the expected variable layout.
