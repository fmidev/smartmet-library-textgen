# Story "precipitation_range"

The `precipitation_range` story has the form:

1. "Sadesumma on yli N millimetriä."
1. "Sadesumma on N millimetriä."
1. "Sadesumma on N-M millimetriä."

In Swedish:

1. "Nederbördsumman är över N millimeter."
1. "Nederbördsumman är N millimeter."
1. "Nederbördsumman är N-M millimeter."

In English:

1. "Total precipitation is over N millimeters."
1. "Total precipitation is N millimeters."
1. "Total precipitation is N-M millimeters."

Hourly precipitation amounts that are too small do not belong in the total. Such small precipitation can be filtered out of the sum using the variable:

```
textgen::[section]::story::precipitation_range::minrain = [0-X]
```

The logic uses the area's minimum and maximum precipitation sums plus the optional variable:

```
textgen::[section]::story::precipitation_range::maxrain
```

If the variable value is set to X and the minimum precipitation sum is at least X, story 1 is returned. If minimum and maximum are equal, story 2 is returned. Otherwise story 3 is returned.

Phrases used:

* "sadesumma"
* "on"
* "yli"
* "millimetriä"

The generated analysis functions can be overridden as follows:

```
1. Minimum(Sum(Precipitation)):
textgen::[section]::story::precipitation_total::fake::minimum = [result]
1. Maximum(Sum(Precipitation)):
textgen::[section]::story::precipitation_total::fake::maximum = [result]
```
