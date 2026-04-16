# Story "roadwarning_shortview"

> **Status:** Active. Shorter variant of
> [`roadwarning_overview`](roadwarning_overview.md).
>
> **Owner:** `RoadStory::warning_shortview()`.
> **Implementation:** `textgen/roadwarning_shortview.cpp` (~945 LOC).

## What it produces

A compact road-warning summary for a shorter forecast window. Covers the
same warning classes (slipperiness, visibility, wind) as
`roadwarning_overview` but does less merging and produces fewer time
phrases.

## Configuration root

```
textgen::[section]::story::roadwarning_shortview::*
```

Mirrors `roadwarning_overview`'s settings. See that story's page and
`textgen/roadwarning_shortview.cpp` for the exact variable list.
