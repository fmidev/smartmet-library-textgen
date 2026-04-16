# Story "temperature_range"

The `temperature_range` story has the form:

1. "Lämpötila on noin X astetta."
1. "Lämpötila on X...Y astetta."

In Swedish:

1. "Temperatur är cirka X grader."
1. "Temperatur är X...Y grader."

In English:

1. "Temperature is about X degrees."
1. "Temperature is X...Y degrees."

Phrases used:

* "lämpötila"
* "noin"
* "astetta"

The generated analysis functions can be overridden as follows:

```
1. Mean(Min(Temperature))...Mean(Max(Temperature))
textgen::[section]::story::temperature_range::fake::minimum = [result]
textgen::[section]::story::temperature_range::fake::maximum = [result]
textgen::[section]::story::temperature_range::fake::mean = [result]
```

Variables related to the story:

```
textgen::[section]::story::temperature_range::mininterval = [1-]
textgen::[section]::story::temperature_range::always_interval_zero = [bool]
```

If the interval length is at least `mininterval`, an interval is reported; otherwise the mean is reported. As an exception, if zero falls in the interval and `always_interval_zero` is true, an interval is always reported.

Default for `mininterval` is 2; default for `always_interval_zero` is false.
