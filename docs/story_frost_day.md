# Story "frost_day"

The story is empty from mid-October to the end of March! In addition, if the probability of severe frost is very high, the frost is presumably so obvious that there is no need to report it separately.

If the period length is zero nights, an empty story is returned.

If the period length is one night, one of the following stories is returned:

1. ""
1. "[Hallan todennäköisyys] on [ensi yönä] N%."
1. "[Hallan todennäköisyys] on [ensi yönä] N%, rannikolla M%."

In Swedish:

1. ""
1. "[Sannolikheten för nattfrost] är [i natt] N%."
1. "[Sannolikheten för nattfrost] är [i natt] N%, vid kusten M%."

In English:

1. ""
1. "[Probability of frost] is [at night] N%."
1. "[Probability of frost] is [at night] N%, at the coast M%."

The phrase "hallan todennäköisyys" may also be replaced with "ankaran hallan todennäköisyys" depending on the situation.

The story is chosen based on the variables:

```
textgen::[section]::story::frost_day::severe_frost_limit = [0-100] (=10)
textgen::[section]::story::frost_day::frost_limit = [0-100] (=10)
textgen::[section]::story::frost_day::obvious_frost = [0-100] (=90)
textgen::[section]::story::frost_day::precision = [0-100] (=10)
textgen::[section]::story::frost_day::coast_limit = [0-100] (=20)
```

If the probability of severe frost is at least `severe_frost_limit`, the phrase "ankaran hallan todennäköisyys" ("probability of severe frost") is used; otherwise "hallan todennäköisyys" ("probability of frost"). If the probability of frost is below `frost_limit`, an empty story is returned. However, if the probability of severe frost is above `obvious_frost`, the story may still be empty (see the explanation at the top of the page).

The inland and coastal probabilities are reported separately if the difference in probabilities is at least `coast_limit`.

Probabilities are rounded to the precision given by `precision`. A precision of 10 is generally used.

The night is defined by the variables:

```
textgen::[section]::story::frost_day::night::starthour = [0-23]
textgen::[section]::story::frost_day::night::endhour = [0-23]
textgen::[section]::story::frost_day::night::maxstarthour = [0-23]
textgen::[section]::story::frost_day::night::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

No default definition (such as 18–06) is given for the night in the frost context, because the interesting interval for frost may well be larger. The responsibility for defining the correct interval therefore rests with the user.

Period phrases are of type [tonight](period_phrases.md#tonight). Their settings are controlled with:

```
textgen::[section]::story::frost_day::tonight::phrases
```

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::frost_day::fake::area::frost = [result]
textgen::[section]::story::frost_day::fake::area::severe_frost = [result]
textgen::[section]::story::frost_day::fake::coast::value = [result]
textgen::[section]::story::frost_day::fake::inland::value = [result]
```

Phrases used:

* "hallan todennäköisyys"
* "ankaran hallan todennäköisyys"
* "on"
* "rannikolla"
