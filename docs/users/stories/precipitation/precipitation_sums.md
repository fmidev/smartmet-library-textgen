# Story "precipitation_sums"

The `precipitation_sums` story has the form:

1. "Ensimmäisen 12 tunnin sademäärä on X-Y millimetriä, seuraavan 12 tunnin X-Y millimetriä."
1. "Seuraavan 24 tunnin sademäärä on 0 millimetriä."

In Swedish:

1. "Nederbördssumman för de första 12 timmar är X-Y millimeter, för de följande 12 timmar X-Y millimeter."
1. "Nederbördssumman för de följande 24 timmar är 0 millimeter."

In English:

1. "Total precipitation for the first 12 hours is X-Y millimeters, for the following 12 hours X-Y millimeters."
1. "Total precipitation for the next 24 hours is 0 millimeters."

Phrases used:

* "ensimmäisen 12 tunnin sademäärä"
* "seuraavan 12 tunnin"
* "on"
* "seuraavan 24 tunnin sademäärä"
* "millimetriä"

Hourly precipitation amounts that are too small do not belong in the total. Such small precipitation can be filtered out of the sum using:

```
textgen::[section]::story::precipitation_sums::minrain = [0-X]
```

The precipitation sum is reported as a mean if the difference between minimum and maximum is less than:

```
textgen::[section]::story::precipitation_sums::mininterval = [0-X] (=1)
```

The generated analysis functions can be overridden as follows:

```
1. Mean(Sum(Precipitation)):
textgen::[section]::story::precipitation_sums::fake::period1::mean = [result]
textgen::[section]::story::precipitation_sums::fake::period1::minimum = [result]
textgen::[section]::story::precipitation_sums::fake::period1::maximum = [result]
textgen::[section]::story::precipitation_sums::fake::period2::mean = [result]
textgen::[section]::story::precipitation_sums::fake::period2::minimum = [result]
textgen::[section]::story::precipitation_sums::fake::period2::maximum = [result]
```
