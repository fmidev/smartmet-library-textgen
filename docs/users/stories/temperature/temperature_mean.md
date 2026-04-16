# Story "temperature_mean"

The `temperature_mean` story has the form:

1. "Keskilämpötila N astetta."

In Swedish:

1. "Medeltemperaturen N grader."

In English:

1. "Mean temperature N degrees."

Phrases used:

* "keskilämpötila"
* "astetta"

The generated analysis functions can be overridden as follows:

```
1. Mean(Mean(Temperature)):
textgen::[section]::story::temperature_mean::fake::mean = [result]
```
