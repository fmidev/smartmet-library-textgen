# Story "temperature_weekly_minmax"

The period must be at least 2 days long.

The stories are of the form:

1. "Päivien ylin lämpötila on A...B astetta, öiden alin lämpötila C...D astetta."
1. "Päivien ylin lämpötila on noin X astetta, öiden alin lämpötila noin Y astetta."

In Swedish:

1. "Dagstemperaturerna är A...B grader, natttemperaturerna C...D grader."
1. "Dagstemperaturerna är cirka X grader, natttemperaturerna circa Y grader."

In English:

1. "Daily maximum temperature is A...B degrees, nightly minimum temperatures C...D degrees."
1. "Daily maximum temperature is about X degrees, nightly minimum temperatures about Y degrees."

If the interval is smaller than:

```
textgen::[section]::story::temperature_weekly_minmax::day::mininterval = [1-] (default=2)
textgen::[section]::story::temperature_weekly_minmax::night::mininterval = [1-] (default=2)
```

the interval is collapsed into a single "about" reading.

If the variable:

```
textgen::[section]::story::temperature_weekly_minmax::day::always_interval_zero = [bool] (default = false)
textgen::[section]::story::temperature_weekly_minmax::night::always_interval_zero = [bool] (default = false)
```

is true, intervals that contain the reading 0 are always shown as an interval if the lower and upper bounds differ by even a degree.

If the variable:

```
textgen::[section]::story::temperature_weekly_minmax::emphasize_night_minimum = [bool] (default=false)
```

is true, no upper bound is shown for the night's minimum temperature.

Days and nights are defined by:

```
textgen::[section]::story::temperature_weekly_minmax::day::starthour = [0-23]
textgen::[section]::story::temperature_weekly_minmax::day::endhour = [0-23]
textgen::[section]::story::temperature_weekly_minmax::day::maxstarthour = [0-23]
textgen::[section]::story::temperature_weekly_minmax::day::minendhour = [0-23]

textgen::[section]::story::temperature_weekly_minmax::night::starthour = [0-23]
textgen::[section]::story::temperature_weekly_minmax::night::endhour = [0-23]
textgen::[section]::story::temperature_weekly_minmax::night::maxstarthour = [0-23]
textgen::[section]::story::temperature_weekly_minmax::night::minendhour = [0-23]
```

Typical settings include, for example:

```
textgen::[section]::story::temperature_weekly_minmax::day::starthour = 6
textgen::[section]::story::temperature_weekly_minmax::day::endhour = 18
textgen::[section]::story::temperature_weekly_minmax::night::starthour = 18
textgen::[section]::story::temperature_weekly_minmax::night::endhour = 6
```

The computation results can be replaced with the variables:

```
textgen::[section]::story::temperature_weekly_minmax::fake::day::minimum = [result]
textgen::[section]::story::temperature_weekly_minmax::fake::day::maximum = [result]
textgen::[section]::story::temperature_weekly_minmax::fake::day::mean = [result]

textgen::[section]::story::temperature_weekly_minmax::fake::night::minimum = [result]
textgen::[section]::story::temperature_weekly_minmax::fake::night:maximum = [result]
textgen::[section]::story::temperature_weekly_minmax::fake::night:mean = [result]
```

Phrases used:

* "päivien ylin lämpötila"
* "öiden alin lämpötila"
* "on"
* "noin"
* "astetta"
