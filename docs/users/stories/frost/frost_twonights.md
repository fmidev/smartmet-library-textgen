# Story "frost_twonights"

The story is empty from mid-October to the end of March! In addition, if the probability of severe frost is very high on both days, the frost is so obvious that it need not be reported separately.

If the period length is zero nights, an empty story is returned.

If the period length is one night, one of the stories is returned:

1. ""
1. "Hallan todennäköisyys on N%."
1. "Ankaran hallan todennäköisyys on N%."

In Swedish:

1. ""
1. "Sannolikheten för nattfrost är N%."
1. "Sannolikheten för stark nattforst är N%."

In English:

1. ""
1. "Probability of frost is N%."
1. "Probability of severe frost is N%."

The story is chosen based on the variables:

```
textgen::[section]::story::frost_twonights::severe_frost_limit = [0-100]
textgen::[section]::story::frost_twonights::frost_limit = [0-100]
textgen::[section]::story::frost_twonights::obvious_frost = [0-100] (=90)
textgen::[section]::story::frost_twonights::precision = [0-100]
```

If the probability of severe frost is at least `severe_frost_limit`, phrase 3 is returned. Otherwise, if the probability of frost is at least `frost_limit`, phrase 2 is returned. Otherwise an empty story is returned. If the probability of severe frost is above `obvious_frost`, the story may be left empty (see the explanation at the top of the page).

Probabilities are rounded to the precision given by `precision`. Precision 10 is typically used.

The night is defined by the variables:

```
textgen::[section]::story::frost_twonights::night::starthour = [0-23]
textgen::[section]::story::frost_twonights::night::endhour = [0-23]
textgen::[section]::story::frost_twonights::night::maxstarthour = [0-23]
textgen::[section]::story::frost_twonights::night::minendhour = [0-23]
```

The normal interval for the day is `starthour` – `endhour`. If the period is shorter than these settings require, the range can flex within the limits of `maxstarthour` – `minendhour`. Without flex limits, no flexing is allowed.

No default definition (such as 18–06) is given for the night in the frost context because the interesting interval for frost may well be longer. The responsibility for defining the correct interval rests with the user.

If the period length is two or more nights, one of the stories below is returned. Only the first two nights are reported; subsequent days are not:

1. "Hallan todennäköisyys on viikonpäivän vastaisena yönä x%"
   1. + ", seuraavana yönä ankaran hallan todennäköisyys on x%."
   1. + ", seuraavana yönä sama."
   1. + ", seuraavana yönä x%."
   1. + ", seuraava yö on lämpimämpi."
1. "Ankaran hallan todennäköisyys on viikonpäivän vastaisena yönä x%"
   1. + ", seuraavana yönä sama."
   1. + ", seuraavana yönä x%."
   1. + ", seuraavana yönä hallan todennäköisyys on x%."
   1. + ", seuraava yö on huomattavasti lämpimämpi."

In Swedish:

1. "Sannolikheten för nattfrost är natten mot veckodag x%",
   1. + ", följande natt sannolikheten för stark nattfrost är x%."
   1. + ", följande natt densamma."
   1. + ", följande natt x%."
   1. + ", följande natt är varmare."
1. "Sannolikheten för stark nattfrost är natten mot veckodag x%",
   1. + ", följande natt densamma."
   1. + ", följande natt x%."
   1. + ", följande natt sannolikheten för nattfrost är x%."
   1. + ", följande natt är betydligt varmare."

In English:

1. "Probability of frost is on Monday night x%",
   1. + ", the following night probability of severe frost is x%",
   1. + ", the following night the same."
   1. + ", the following night x%."
   1. + ", the following night is warmer."
1. "Probability of severe frost is on Monday night x%",
   1. + ", the following night the same."
   1. + ", the following night x%."
   1. + ", the following night probability of frost is x%."
   1. + ", the following night is significantly warmer."

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::frost_mean::fake::day1::mean = [result]
textgen::[section]::story::frost_mean::fake::day1::severe_mean = [result]
textgen::[section]::story::frost_mean::fake::day2::mean = [result]
textgen::[section]::story::frost_mean::fake::day2::severe_mean = [result]
```

Phrases used:

* "hallan todennäköisyys"
* "ankaran hallan todennäköisyys"
* "on"
* "N-vastaisena yönä", N=1–7
* "seuraavana yönä"
* "sama"
* "seuraava yö"
* "lämpimämpi"
* "huomattavasti lämpimämpi"
