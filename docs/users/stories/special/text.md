# Story "text"

> **Status:** Specialized. Emits literal configured text.
>
> **Owner:** `SpecialStory::text()`.
> **Implementation:** `textgen/special_text.cpp` (~161 LOC).

Emits the literal text stored in a configuration variable. Used for
boilerplate clauses that should not be translated via the dictionary —
for example, legal disclaimers or sign-off lines that are written directly
into the config.

## Configuration root

```
textgen::[section]::story::text::*
```

The exact variable names are documented in `textgen/special_text.cpp`;
look for `Settings::` calls.
