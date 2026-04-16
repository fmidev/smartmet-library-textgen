# Story "pressure_mean"

The `pressure_mean` story has the form:

1. "Paine on X hPa."

In Swedish:

1. "Tryck är X hPa."

In English:

1. "Pressure is X hPa."

Phrases used:

* "paine"
* "on"
* "hehtopascalia"

The generated analysis functions can be overridden as follows:

```
1. Mean(Mean(Pressure))
textgen::[section]::story::pressure_mean::fake::mean = [result]
```
