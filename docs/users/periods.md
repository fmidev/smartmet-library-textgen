# Creating periods

## WeatherPeriodFactory namespace

The `TextGen::WeatherPeriodFactory` namespace contains a `create` function that can be used to create named weather periods. The function is given the forecast creation time in local time and the name of the variable describing the weather period. The return value is a `TextGen::WeatherPeriod` object.

Example:

```cpp
NFmiTime t(2003,5,10);
WeatherPeriod period = WeatherPeriodFactory::create(t,"textgen::period");
```

In this case the factory assumes that, for example, the variable `textgen::period::type` has been set. The other required variables depend on the period type and are documented in the documentation for each period type.

### Period type "now"

The start and end times of the period are set to the given time. The period is mainly useful for generating headers, for example with the "report_area" header.

### Period type "until"

Creating the period requires the variables:

```
variable::type = until
variable::days = <integer>
variable::endhour = <integer>
variable::switchhour = <integer>
```

From these settings the start time of the period is obtained simply by rounding the given time up to the next full hour. The end time is obtained by first adding `variable::days` to the start time. If the hour portion is at least `variable::switchhour`, one more day is added. After that the hour is set to `variable::endhour`.

For example, if

```
variable::days = 1
variable::endhour = 18
variable::switchhour = 12
```

we get:

* Monday at 11:00 → period Monday 11:00 – Tuesday 18:00
* Monday at 11:01 → period Monday 12:00 – Wednesday 18:00

### Period type "from_until"

Creating the period requires the variables:

```
variable::type = from_until
variable::startday = <integer>
variable::starthour = <integer>
variable::switchhour = <integer>
variable::days = <integer>
variable::endhour = <integer>
```

From these settings the start time of the period is obtained by first rounding the clock up to the next full hour. Then `variable::startday` days are added. After that one more day is added if the hour is at least `variable::switchhour`. Finally the hour is set to `variable::starthour`.

The end time is obtained by adding `variable::days` days to the start time and setting the hour to `variable::endhour`.

For example, if

```
variable::startday = 1
variable::starthour = 18
variable::switchhour = 12
variable::days = 2
variable::endhour = 6
```

we get:

* Monday at 11:00 → period Tuesday 18:00 – Thursday 06:00
* Monday at 11:01 → period Wednesday 18:00 – Friday 06:00
