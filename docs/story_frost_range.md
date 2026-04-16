# Story "frost_range"

The interval's frost probabilities are used to pick the minimum and maximum for the text.

The `frost_range` story has the form:

1. ""
1. "Hallan todennäköisyys on N%."
1. "Ankaran hallan todennäköisyys on N%."
1. "Hallan todennäköisyys on N-M%."
1. "Ankaran hallan todennäköisyys on N-M%."

In Swedish:

1. ""
1. "Sannolikheten för nattfrost är N%."
1. "Sannolikheten för stark nattforst är N%."
1. "Sannolikheten för nattfrost är N-M%."
1. "Sannolikheten för stark nattforst är N-M%."

In English:

1. ""
1. "Probability of frost is N%."
1. "Probability of severe frost is N%."
1. "Probability of frost is N-M%."
1. "Probability of severe frost is N-M%."

The story is chosen based on the variables:

```
textgen::[section]::story::frost_range::severe_frost_limit = [0-100]
textgen::[section]::story::frost_range::frost_limit = [0-100]
textgen::[section]::story::frost_range::precision = [0-100]
```

If the probability of severe frost is at least `severe_frost_limit`, phrase 3 is returned. Otherwise, if the probability of frost is at least `frost_limit`, phrase 2 is returned. Otherwise an empty story is returned.

Probabilities are rounded to the precision given by `precision`. Precision 10 is typically used.

If the rounded minimum and maximum probabilities are the same, only one reading is reported, not a range. As an exception, if the maximum is zero, an empty phrase is returned.

Phrases used:

* "hallan todennäköisyys"
* "ankaran hallan todennäköisyys"
* "on"

The generated analysis functions can be overridden as follows:

```
1. Maximum(Maximum(Frost)):
textgen::[section]::story::frost_range::fake::maximum = [result]
1. Minimum(Maximum(Frost)):
textgen::[section]::story::frost_range::fake::minimum = [result]
1. Maximum(Maximum(SevereFrost)):
textgen::[section]::story::frost_range::fake::severe_maximum = [result]
1. Minimum(Maximum(SevereFrost)):
textgen::[section]::story::frost_range::fake::severe_minimum = [result]
```
