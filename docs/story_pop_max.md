# Story "pop_max"

The `pop_max` story has the form:

1. "Sateen todennäköisyys on x%"

In Swedish:

1. "Sannolikheten för nederbörd är x%"

In English:

1. "Probability of precipitation is x%"

The probability of precipitation is computed by taking the regional mean of the temporal maxima.

The probability of precipitation is rounded to the precision:

```
textgen::[section]::story::pop_max::precision = [0-100]
```

The probability of precipitation must be at least:

```
textgen::[section]::story::pop_max::minimum = [0-100] (default = 10)
```

before the probability is reported, and at most:

```
textgen::[section]::story::pop_max::maximum = [0-100] (default = 100)
```

Phrases used:

* "sateen todennäköisyys"
* "on"

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::pop_max::fake::max = [result]
```
