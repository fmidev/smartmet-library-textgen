# Story "roadcondition_shortview"

> **Status:** Active. Shorter variant of
> [`roadcondition_overview`](roadcondition_overview.md).
>
> **Owner:** `RoadStory::condition_shortview()`.
> **Implementation:** `textgen/roadcondition_shortview.cpp` (~647 LOC).

## What it produces

A compact road-surface-condition summary for up to 30 hours. The same
eight conditions as [`roadcondition_overview`](roadcondition_overview.md),
with the same coverage classification ("yleisesti / monin paikoin /
paikoin"), but fewer time phrases and less aggressive merging.

## Condition classes

From `RoadConditionType` in the source:

| Enum | Name |
| --- | --- |
| `DRY` | kuiva |
| `MOIST` | kostea |
| `WET` | märkä |
| `SLUSH` | sohjo |
| `FROST` | kuura |
| `PARTLY_ICY` | osittain jäinen |
| `ICY` | jäinen |
| `SNOW` | lumi |

Ordered by importance in `condition_importance()`. Worse conditions
dominate when multiple apply to the same sub-period.

## Configuration parameters

All settings live under
`textgen::[section]::story::roadcondition_shortview::*`.

| Parameter | Default | Meaning |
| --- | --- | --- |
| `maxhours` | 30 | Maximum hours reported |
| `generally_limit` | 90 % | "Yleisesti" threshold |
| `manyplaces_limit` | 50 % | "Monin paikoin" threshold |
| `someplaces_limit` | 10 % | "Paikoin" threshold |

For the full meaning of the three coverage thresholds see
[`roadcondition_overview` → Coverage classification](roadcondition_overview.md#road-condition-for-one-period).

## See also

* [`roadcondition_overview`](roadcondition_overview.md) — full-length
  variant with multi-period merging
* [`roadwarning_shortview`](roadwarning_shortview.md) — the
  matching warning-state shortview
