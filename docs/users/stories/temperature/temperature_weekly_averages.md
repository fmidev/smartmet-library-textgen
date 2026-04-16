# Story "temperature_weekly_averages"

The story is otherwise similar to [story_temperature_weekly_minmax](story_temperature_weekly_minmax.md), but the sentences report the mean temperatures during days and nights instead of the day's maxima and night's minima.

The period must be at least 2 days long.

The stories are of the form:

1. "Päivälämpötila on A...B astetta, yölämpötila C...D astetta."
1. "Päivälämpötila on noin X astetta, yölämpötila noin Y astetta."

In Swedish:

1. "Dagstemperaturen är A...B grader, natttemperaturen C...D grader."
1. "Dagstemperaturen är cirka X grader, natttemperaturen circa Y grader."

In English:

1. "Daily temperature is A...B degrees, nightly temperature C...D degrees."
1. "Daily temperature is about X degrees, nightly temperature about Y degrees."

If the interval is smaller than:

```
textgen::[section]::story::temperature_weekly_averages::day::mininterval = [1-] (default=2)
textgen::[section]::story::temperature_weekly_averages::night::mininterval = [1-] (default=2)
```

the interval is collapsed into a single "about" reading.

If the variable:

```
textgen::[section]::story::temperature_weekly_averages::day::always_interval_zero = [bool] (default = false)
textgen::[section]::story::temperature_weekly_averages::night::always_interval_zero = [bool] (default = false)
```

is true, intervals that contain the reading 0 are always shown as an interval if the lower and upper bounds differ by even a degree.

Days and nights are defined by:

```
textgen::[section]::story::temperature_weekly_averages::day::starthour = [0-23]
textgen::[section]::story::temperature_weekly_averages::day::endhour = [0-23]
textgen::[section]::story::temperature_weekly_averages::day::maxstarthour = [0-23]
textgen::[section]::story::temperature_weekly_averages::day::minendhour = [0-23]

textgen::[section]::story::temperature_weekly_averages::night::starthour = [0-23]
textgen::[section]::story::temperature_weekly_averages::night::endhour = [0-23]
textgen::[section]::story::temperature_weekly_averages::night::maxstarthour = [0-23]
textgen::[section]::story::temperature_weekly_averages::night::minendhour = [0-23]
```

Typical settings include, for example:

```
textgen::[section]::story::temperature_weekly_averages::day::starthour = 6
textgen::[section]::story::temperature_weekly_averages::day::endhour = 18
textgen::[section]::story::temperature_weekly_averages::night::starthour = 18
textgen::[section]::story::temperature_weekly_averages::night::endhour = 6
```

The computation results can be replaced with the variables:

```
textgen::[section]::story::temperature_weekly_averages::fake::day::minimum = [result]
textgen::[section]::story::temperature_weekly_averages::fake::day::maximum = [result]
textgen::[section]::story::temperature_weekly_averages::fake::day::mean = [result]

textgen::[section]::story::temperature_weekly_averages::fake::night::minimum = [result]
textgen::[section]::story::temperature_weekly_averages::fake::night:maximum = [result]
textgen::[section]::story::temperature_weekly_averages::fake::night:mean = [result]
```

Phrases used:

* "päivälämpötila"
* "yölämpötila"
* "on"
* "noin"
* "astetta"
