# Story "precipitation_classification"

The `precipitation_classification` story has the form:

1. "Sadesumma yli N millimetriä."
1. "Sadesumma monin paikoin yli N millimetriä."
1. "Sadesumma paikoin yli N millimetriä."
1. "Sadesumma 0 millimetriä."
1. "Sadesumma N-M millimetriä."
1. "Sadesumma N-M millimetriä, paikoin enemmän."
1. "Sadesumma N-M millimetriä, monin paikoin enemmän."

In Swedish:

1. "Nederbördsumman över N millimeter."
1. "Nederbördsumman på många håll över N millimeter."
1. "Nederbördsumman lokat över N millimeter."
1. "Nederbördsumman 0 millimeter."
1. "Nederbördsumman N-M millimeter."
1. "Nederbördsumman N-M millimeter, lokalt mera."
1. "Nederbördsumman N-M millimeter, mera på många håll."

In English:

1. "Total precipitation over N millimeters."
1. "Total precipitation in many places over N millimeters."
1. "Total precipitation in some places over N millimeters."
1. "Total precipitation 0 millimeters."
1. "Total precipitation N-M millimeters."
1. "Total precipitation N-M millimeters, more in many places."
1. "Total precipitation N-M millimeters, more in some places."

The logic uses the area's minimum and maximum precipitation sums plus the optional variables:

```
textgen::[section]::story::precipitation_classification::max_some_places = [0-100]
textgen::[section]::story::precipitation_classification::max_many_places = [0-100]
textgen::[section]::story::precipitation_classification::some_places = [0-100]
textgen::[section]::story::precipitation_classification::many_places = [0-100]
textgen::[section]::story::precipitation_classification::classes = [range1,range2,...]
```

The text is chosen with the following logic:

1. The maximum precipitation amount MAXCLASS is taken from all classes
1. If the maximum precipitation sum exceeds MAXCLASS
   1. If the minimum precipitation sum exceeds MAXCLASS, story 1 is returned
   1. If the precipitation sum exceeds the configured limit `max_many_places` in a sufficient number of points, story 2 is returned
   1. If the precipitation sum exceeds the configured limit `max_some_places` in a sufficient number of points, story 3 is returned
   1. Otherwise story 5 is returned!!!
1. If the maximum precipitation sum is 0, story 4 is returned
1. The mean and maximum precipitation sums are computed
1. A precipitation class is formed based on the mean. If multiple classes are possible, the first of them is chosen
1. If the maximum falls in the same class, story 5 is returned
1. If the regional exceedance percentage exceeds the limit `many_places`, story 6 is returned. If the limit has not been set, story 6 is never returned
1. If the regional exceedance percentage exceeds the limit `some_places`, story 7 is returned. If the limit has not been set, story 7 is never returned
1. Otherwise story 5 is returned

Phrases used:

* "sadesumma"
* "paikoin"
* "monin paikoin"
* "yli"
* "millimetriä"
* "paikoin enemmän"
* "monin paikoin enemmän"

PS. The name of the story is not `precipitation_class` because, following coding conventions, that would require a method named `class` in the PrecipitationStory class, which is not allowed.

Hourly precipitation amounts that are too small are in principle not part of the total. Such small amounts can be filtered out using:

```
textgen::[section]::story::precipitation_range::minrain = [0-X]
```

The generated analysis functions can be overridden as follows:

```
1. Mean(Sum(Precipitation))
textgen::[section]::story::precipitation_classification::fake::mean = [result]
1. Minimum(Sum(Precipitation))
textgen::[section]::story::precipitation_classification::fake::minimum = [result]
1. Maximum(Sum(Precipitation))
textgen::[section]::story::precipitation_classification::fake::maximum = [result]
1. Percentage(Sum(Precipitation))
textgen::[section]::story::precipitation_classification::fake::percentage = [result]
```
