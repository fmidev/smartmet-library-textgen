# Story "temperature_weekly_averages_trend"

The period must be at least 2 days long. Start and end hours must be 06 or 18.

The stories are of the form:

1. "Päivälämpötila on A...B astetta."
1. "Päivälämpötila on noin A...B astetta."
1. "Päivälämpötila on aluksi A...B astetta, lopuksi C...D astetta."
1. "Yölämpötila on aluksi A...B astetta, lopuksi C...D astetta."
1. "Yölämpötila on A...B astetta."
1. "Yölämpötila on noin A...B astetta."
1. "Päivälämpötila on A...B astetta, yölämpötila C...D astetta."
1. "Päivälämpötila on noin X astetta, yölämpötila noin Y astetta."

The story may have two separate sentences when needed.

In Swedish:

1. "Dagstemperaturen är A...B grader."
1. "Dagstemperaturen är circa A...B grader."
1. "Dagstemperaturen är i början A...B grader, i slutet C...D grader."
1. "Natttemperaturen är i början A...B grader, i slutet C...D grader."
1. "Natttemperaturen är A...B grader."
1. "Natttemperaturen är circa A...B grader."
1. "Dagstemperaturen är A...B grader, natttemperaturen C...D grader."
1. "Dagstemperaturen är circa X grader, natttemperaturen circa Y grader."

In English:

1. "Daily maximum temperature is A...B degrees."
1. "Daily maximum temperature is about A...B degrees."
1. "Daily maximum temperature is in the beginning A...B degrees, in the end C...D degrees."
1. "Daily minimum temperaturen is in the beginning A...B degrees, in the end C...D degrees."
1. "Daily minimum temperaturen is A...B degrees."
1. "Daily minimum temperaturen is about A...B degrees."
1. "Daily maximum temperature is A...B degrees, nightly minimum temperature C...D degrees."
1. "Daily maximum temperature is about X degrees, nightly minimum temperatures about Y degrees."

If the interval is smaller than:

```
textgen::[section]::story::temperature_weekly_averages::mininterval = [1-]
```

the interval is collapsed into a single "about" reading.

If the variable:

```
textgen::[section]::story::temperature_weekly_averages::always_interval_zero = [bool]
```

is true, intervals that contain the reading 0 are always shown as an interval if the lower and upper bounds differ by even a degree.

If the variable:

```
textgen::[section]::story::temperature_weekly_averages::emphasize_night_minimum = [bool]
```

is true, no upper bound is shown for the night's minimum temperature.

The story always reports the day temperature first, then the night temperature. If either allows a split into "in the beginning" and "in the end" parts, the story consists of two sentences.

Phrases used:

* "päivälämpötila"
* "yölämpötila"
* "on"
* "noin"
* "astetta"
* "aluksi"
* "lopuksi"
