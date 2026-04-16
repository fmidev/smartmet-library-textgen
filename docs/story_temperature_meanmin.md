# Story "temperature_meanmin"

The `temperature_meanmin` story has the form:

1. "Keskimääräinen alin lämpötila N astetta."

In Swedish:

1. "Den lägsta temperaturen i medeltal N grader."

In English:

1. "Mean minimum temperature N degrees."

Phrases used:

* "keskimääräinen alin lämpötila"
* "astetta"

The generated analysis functions can be overridden as follows:

```
1. Mean(Mean(MinTemperature)):
textgen::[section]::story::temperature_meanmin::fake::mean = [result]
```
