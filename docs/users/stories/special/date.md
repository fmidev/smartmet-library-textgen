# Story "date"

> **Status:** Specialized. Emits a date phrase.
>
> **Owner:** `SpecialStory::date()`.
> **Implementation:** `textgen/special_date.cpp` (~72 LOC).

Emits a sentence containing a formatted date, typically the forecast date.
Useful when a product needs a "Forecast issued on …" line.

## Configuration root

```
textgen::[section]::story::date::*
```

Reads the date source (forecast time vs. period start time) and any
formatting options. See `textgen/special_date.cpp`.
