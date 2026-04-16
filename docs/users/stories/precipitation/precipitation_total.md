# Story "precipitation_total"

The `precipitation_total` story has the form:

1. "Sadesumma N millimetriä."

In Swedish:

1. "Nederbördsumman N millimeter."

In English:

1. "Total precipitation N millimeters."

Phrases used:

* "sadesumma"
* "millimetriä"

Hourly precipitation amounts that are too small do not belong in the total. Such small precipitation can be filtered out of the sum using the variable:

```
textgen::[section]::story::precipitation_total::minrain = [0-X]
```

The generated analysis functions can be overridden as follows:

```
1. Mean(Sum(Precipitation)):
textgen::[section]::story::precipitation_total::fake::mean = [result]
```
