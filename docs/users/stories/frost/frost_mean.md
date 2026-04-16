# Story "frost_mean"

The interval's frost probabilities are used to pick the mean for the text.

The `frost_mean` story has the form:

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
textgen::[section]::story::frost_mean::severe_frost_limit = [0-100]
textgen::[section]::story::frost_mean::frost_limit = [0-100]
textgen::[section]::story::frost_mean::precision = [0-100]
```

If the probability of severe frost is at least `severe_frost_limit`, phrase 3 is returned. Otherwise, if the probability of frost is at least `frost_limit`, phrase 2 is returned. Otherwise an empty story is returned.

Probabilities are rounded to the precision given by `precision`. Precision 10 is typically used.

Phrases used:

* "hallan todennäköisyys"
* "ankaran hallan todennäköisyys"
* "on"

The generated analysis functions can be overridden as follows:

```
1. Mean(Maximum(Frost)):
textgen::[section]::story::frost_mean::fake::mean = [result]
1. Mean(Maximum(SevereFrost)):
textgen::[section]::story::frost_mean::fake::severe_mean = [result]
```
