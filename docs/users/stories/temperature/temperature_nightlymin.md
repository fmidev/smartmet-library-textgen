# Story "temperature_nightlymin"

The `temperature_nightlymin` story has the form:

1. "Yön alin lämpötila on [viikonpäivän vastaisena yönä] noin x astetta"
1. "Yön alin lämpötila on [viikonpäivän vastaisena yönä] x...y astetta"

In Swedish:

1. "Dagens högsta temperatur är [på natten mot veckodag] cirka x grader"
1. "Dagens högsta temperatur är [på natten mot veckodag] x...y grader"

In English:

1. "The maximum day temperature [on Weekday night] is about x degrees"
1. "The maximum day temperature [on Weekday night] is x...y degrees"

The subordinate clause continues when needed:

1. ", [seuraavana yönä] huomattavasti korkeampi"
1. ", [seuraavana yönä] korkeampi"
1. ", [seuraavana yönä] hieman korkeampi"
1. ", [seuraavana yönä] suunnilleen sama"
1. ", [seuraavana yönä] hieman alempi"
1. ", [seuraavana yönä] alempi"
1. ", [seuraavana yönä] huomattavasti alempi"

In Swedish:

1. ", [på följande natt] betydligt högre."
1. ", [på följande natt] högre."
1. ", [på följande natt] något högre."
1. ", [på följande natt] ungefär densamma."
1. ", [på följande natt] något lägre."
1. ", [på följande natt] lägre."
1. ", [på följande natt] betydligt lägre."

In English:

1. ", [the following night] significantly higher."
1. ", [the following night] higher."
1. ", [the following night] somewhat higher."
1. ", [the following night] about the same."
1. ", [the following night] somewhat lower."
1. ", [the following night] lower."
1. ", [the following night] significantly lower."

Period phrases are of type [tonight](../../period_phrases.md#tonight) and [next_night](../../period_phrases.md#next_night). Their settings are controlled with:

```
textgen::[section]::story::temperature_nightlymin::tonight::phrases
textgen::[section]::story::temperature_nightlymin::next_night::phrases
```

Typical settings include, for example:

```
textgen::[section]::story::temperature_nightlymin::tonight::phrases = tonight
textgen::[section]::story::temperature_nightlymin::next_night::phrases = followingnight
```

Comparative selection is based on the variables:

```
textgen::[section]::story::temperature_nightlymin::comparison::significantly_higher = [N]
textgen::[section]::story::temperature_nightlymin::comparison::higher = [N]
textgen::[section]::story::temperature_nightlymin::comparison::somewhat_higher = [N]
textgen::[section]::story::temperature_nightlymin::comparison::somewhat_lower = [N]
textgen::[section]::story::temperature_nightlymin::comparison::lower = [N]
textgen::[section]::story::temperature_nightlymin::comparison::significantly_lower = [N]
```

and the computed temperature ranges. If the first night's mean is A and the next night's is B, the choice is:

1. Phrase 1, if B−A >= `significantly_higher`
1. Phrase 2, if B−A >= `higher`
1. Phrase 3, if B−A >= `somewhat_higher`
1. Phrase 7, if A−B >= `significantly_lower`
1. Phrase 6, if A−B >= `lower`
1. Phrase 5, if A−B >= `somewhat_lower`
1. Phrase 4 otherwise

If there are more nights, each subsequent night's minimum is listed as a subordinate clause:

* ", viikonpäivän vastaisena yönä noin x astetta"
* ", viikonpäivän vastaisena yönä x...y astetta"

The choice between a single value and an interval is based on:

```
textgen::[section]::story::temperature_nightlymin::mininterval = [1-]
textgen::[section]::story::temperature_nightlymin::always_interval_zero = [bool]
```

If the interval length is at least `mininterval`, an interval is reported; otherwise the mean. As an exception, if zero falls in the interval and `always_interval_zero` is true, an interval is always reported.

Default for `mininterval` is 2; default for `always_interval_zero` is false.

The night is defined by the variables:

```
textgen::[section]::story::temperature_nightlymin::night::starthour = [0-23]
textgen::[section]::story::temperature_nightlymin::night::endhour = [0-23]
textgen::[section]::story::temperature_nightlymin::night::maxstarthour = [0-23]
textgen::[section]::story::temperature_nightlymin::night::minendhour = [0-23]
```

The normal interval is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

The definitions could be, for example:

```
textgen::[section]::story::temperature_nightlymin::night::starthour    = 18
textgen::[section]::story::temperature_nightlymin::night::endhour      = 6
textgen::[section]::story::temperature_nightlymin::night::maxstarthour = 01
```

In that case the night's minimum is always reported up to 01:00 in the morning, even though the period already begins at 18:00. At the end of the period flex is rarely needed.

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::temperature_nightlymin::fake::night1::mean = [result]
textgen::[section]::story::temperature_nightlymin::fake::night1::minimum = [result]
textgen::[section]::story::temperature_nightlymin::fake::night1::maximum = [result]
textgen::[section]::story::temperature_nightlymin::fake::night2::mean = [result]
...
```

Phrases used:

* "yön alin lämpötila"
* "on"
* "noin"
* "astetta"
* "huomattavasti korkeampi"
* "korkeampi"
* "hieman korkeampi"
* "suunnilleen sama"
* "hieman alempi"
* "alempi"
* "huomattavasti alempi"

plus the period phrases [tonight](../../period_phrases.md#tonight) and [next_night](../../period_phrases.md#next_night).
