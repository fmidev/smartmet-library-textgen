# Story "evaporation_day"

The story form is one of:

1. "Haihdunta on erittäin huono, X millimetriä."
1. "Haihdunta on huono, X millimetriä."
1. "Haihdunta on tyydyttävä, X millimetriä."
1. "Haihdunta on hyvä, X millimetriä."
1. "Haihdunta on erittäin hyvä, X millimetriä."

In Swedish:

1. "Avdunstning är mycket svag, X millimeter."
1. "Avdunstning är svag, X millimeter."
1. "Avdunstning är måttlig, X millimeter."
1. "Avdunstning är bra, X millimeter."
1. "Avdunstning är mycket bra, X millimeter."

In English:

1. "Evaporation is very poor, X millimeters."
1. "Evaporation is poor, X millimeters."
1. "Evaporation is satisfactory, X millimeters."
1. "Evaporation is good, X millimeters."
1. "Evaporation is very good, X millimeters."

The story applies to a single day only. If data is missing for that day, the story is empty.

Evaporation classification thresholds:

* under 1.5 mm/d — very poor
* 1.5–3 mm/d — poor
* 3.1–5 mm/d — satisfactory
* 5.1–7.5 mm/d — good
* over 7.5 mm/d — excellent

The day is defined by the variables:

```
textgen::[section]::story::evaporation_day::day::starthour = [0-23]
textgen::[section]::story::evaporation_day::day::endhour = [0-23]
textgen::[section]::story::evaporation_day::day::maxstarthour = [0-23]
textgen::[section]::story::evaporation_day::day::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::evaporation_day::fake::mean = [result]
```

Phrases used:

* "haihdunta on erittäin huono"
* "haihdunta on huono"
* "haihdunta on tyydyttävä"
* "haihdunta on hyvä"
* "haihdunta on erittäin hyvä"
* "millimetriä"
