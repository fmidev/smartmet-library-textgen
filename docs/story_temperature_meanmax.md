# Story "temperature_meanmax"

The `temperature_meanmax` story has the form:

1. "Keskimääräinen ylin lämpötila N astetta."

In Swedish:

1. "Den högsta temperaturen i medeltal N grader."

In English:

1. "Mean maximum temperature N degrees."

Phrases used:

* "keskimääräinen ylin lämpötila"
* "astetta"

The generated analysis functions can be overridden as follows:

```
1. Mean(Mean(MaxTemperature)):
textgen::[section]::story::temperature_meanmax::fake::mean = [result]
```
