# Story "weather_shortoverview"

## Story form

The story describes the weather of a several-day period in terms of precipitation and cloudiness.

Possible stories:

1. "[Enimmäkseen selkeää], poutaa."
1. "[Enimmäkseen selkeää], [viikonpäivänä] [sadetta]"
1. "[Enimmäkseen selkeää], [viikonpäivänä] paikoin [sadetta]"
1. "[Enimmäkseen selkeää], ajoittain sateista."
1. "Sää on epävakaista."

Cloudiness alternatives:

1. "enimmäkseen selkeää"
1. "enimmäkseen pilvistä"
1. "enimmäkseen puolipilvistä"
1. "enimmäkseen selkeää tai puolipilvistä"
1. "enimmäkseen pilvistä tai puolipilvistä"
1. "vaihtelevaa pilvisyyttä"

Precipitation-form alternatives:

1. "sadetta"
1. "räntäsadetta"
1. "lumisadetta"
1. "vesi- tai räntäsadetta"
1. "lumi- tai räntäsadetta"

## Algorithms

The story is constructed using the algorithms below.

### Cloudiness

Depending on the variable

```
textgen::[section]::story::weather_shortoverview::cloudiness::fullrange = [boolean] (=true)
```

cloudiness is computed either for the entire interval, or for subintervals:

```
textgen::[section]::story::weather_shortoverview::cloudiness::day::starthour = [0-23] (= 0)
textgen::[section]::story::weather_shortoverview::cloudiness::day::endhour = [0-23] (= 0)
textgen::[section]::story::weather_shortoverview::cloudiness::day::maxstarthour = [0-23] (= starthour)
textgen::[section]::story::weather_shortoverview::cloudiness::day::minendhour = [0-23] (= minendhour)
```

With the default values only full days are counted. Some customers might want only days (06–18) taken into account.

For this partitioning, the percentages of clear, partly cloudy, and cloudy are computed. The class limits use the percentage values:

```
textgen::[section]::story::weather_shortoverview::cloudiness::clear = [0-100] (=40)
textgen::[section]::story::weather_shortoverview::cloudiness::cloudy = [0-100] (=70)
```

A percentage of 40 corresponds to cloudiness class 3.2, and 70 to cloudiness class 5.6. Better defaults might be based on the exact values 3.5 and 5.5, but easier-to-remember limits are probably preferable here.

Based on the limits, the cloudiness percentage is classified as:

1. clear, if percentage >= `cloudiness::clear`
1. cloudy, if percentage >= `cloudiness::cloudy`
1. otherwise partly cloudy

Denoting the resulting percentages as N1, N2, and N3, and using the variables:

```
textgen::[section]::story::weather_shortoverview::cloudiness::single_limit = [50-100] (=60)
textgen::[section]::story::weather_shortoverview::cloudiness::double_limit = [50-100] (=20)
```

the cloudiness description is chosen as follows:

1. If N1 >= `single_limit`, "enimmäkseen selkeää"
1. If N2 >= `single_limit`, "enimmäkseen puolipilvistä"
1. If N3 >= `single_limit`, "enimmäkseen pilvistä"
1. If N1 < `double_limit`, "enimmäkseen pilvistä tai puolipilvistä"
1. If N3 < `double_limit`, "enimmäkseen selkeää tai puolipilvistä"
1. Otherwise "vaihtelevaa pilvisyyttä"

Open issue: cloudiness would hardly be reported for a day on which it rains. By the same logic, that day should not be included in the percentage calculations either. In principle the story could be "enimmäkseen selkeää, viikonpäivänä sadetta." The cloudiness story therefore needs to know, in some sense, how precipitation is reported.

### Precipitation

Precipitation is examined mostly at the level of individual days. The user must define the acceptable days through the variables:

```
textgen::[section]::story::weather_shortoverview::precipitation::day::starthour = [0-23] (= 0)
textgen::[section]::story::weather_shortoverview::precipitation::day::endhour = [0-23] (= 0)
textgen::[section]::story::weather_shortoverview::precipitation::day::maxstarthour = [0-23] (= starthour)
textgen::[section]::story::weather_shortoverview::precipitation::day::minendhour = [0-23] (= minendhour)
```

By default only full days are considered. More sensible flexible values might be, for example:

```
textgen::[section]::story::weather_shortoverview::precipitation::day::starthour = 0
textgen::[section]::story::weather_shortoverview::precipitation::day::endhour = 0
textgen::[section]::story::weather_shortoverview::precipitation::day::maxstarthour = 6
textgen::[section]::story::weather_shortoverview::precipitation::day::minendhour = 18
```

In this case partially incomplete days would also be taken into account; what matters is specifically the daytime.

For each individual day, precipitation sums R1, R2, …, RN are computed. The settings used in connection with these are:

```
textgen::[section]::story::weather_shortoverview::precipitation::rainy = [0-] (=1)
textgen::[section]::story::weather_shortoverview::precipitation::partly_rainy = [0-] (=0.1)
```

These are used to determine whether a day is rainy or only partially rainy. The regional distribution therefore plays no role!

In addition, the variable

```
textgen::[section]::story::weather_shortoverview::precipitation::unstable = [0-] (=50)
```

is needed.

Let N1 be the number of days for which Ri >= `rainy_limit` and N2 the number of days for which Ri >= `mostly_rainy_limit`. The precipitation story is then generated as follows:

1. If N1 == N2 == 0, "poutaa"
1. If N1 == 1 && N2 == 1, "[viikonpäivänä] [sadetta]"
1. If N1 == 0 && N2 == 1, "[viikonpäivänä] paikoin [sadetta]"
1. If Ri >= `rainy_limit` on at least `unstable_limit` of the days, "sää on epävakaista"
1. Otherwise "ajoittain sateista"

## Testing results

Analysis results can be replaced in regression tests with the variables:

```
textgen::[section]::story::weather_shortoverview::fake::clear_percentage = [result]
textgen::[section]::story::weather_shortoverview::fake::cloudy_percentage = [result]

textgen::[section]::story::weather_shortoverview::fake::day1::precipitation = [result]
textgen::[section]::story::weather_shortoverview::fake::day2::precipitation = [result]
...
```

## Phrases required

Phrases required:

* "enimmäkseen"
* "selkeää"
* "pilvistä"
* "puolipilvistä"
* "vaihtelevaa pilvisyyttä"
* "poutaa"
* "N-na", N=1–7
* "paikoin"
* "sadetta"
* "ajoittain sateista"
* "sää on epävakaista"
* "tai"
* "räntäsadetta"
* "lumisadetta"
* "vesi-"
* "lumi-"
