# Story "weather_thunderprobability"

## Story form

The story describes the probability of thunder for a period of one or more days. The reported probability expresses the probability of thunder during the whole period.

The possible stories are:

1. ""
1. "Ukkosen todennäköisyys on X%."

The non-empty possibilities in Swedish:

1. "Sannolikheten för åska är X%."

and in English:

1. "Probability of thunder is X%."

The probability is reported with the precision:

```
textgen::[section]::story::weather_thunderprobability::precision = [0-100] (= 10)
```

The probability is not reported if it is below:

```
textgen::[section]::story::weather_thunderprobability::limit = [0-100] (= 10)
```

Analysis results can be replaced in regression tests with:

```
textgen::[section]::story::weather_thunderprobability::fake::probability = [result]
```

## Phrases required

Phrases required:

* "ukkosen todennäköisyys"
* "on"
* "prosenttia"
