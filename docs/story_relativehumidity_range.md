# Story "relativehumidity_range"

The `relativehumidity_range` story has the form:

1. "Suhteellinen kosteus on noin X%."
1. "Suhteellinen kosteus on X...Y%."

In Swedish:

1. "Relativ fuktighet är cirka X%."
1. "Relativ fuktighet är X...Y%."

In English:

1. "Relative humidity is about X%."
1. "Relative humidity is X...Y%."

Phrases used:

* "suhteellinen kosteus"
* "noin"
* "prosenttia"

Relative humidity is rounded to the precision given by:

```
textgen::[section]::story::relativehumidity_range::precision = [0-100]
```

The generated analysis functions can be overridden as follows:

```
1. Mean(Min(Relativehumidity))...Mean(Max(Relativehumidity))
textgen::[section]::story::relativehumidity_range::fake::minimum = [result]
textgen::[section]::story::relativehumidity_range::fake::maximum = [result]
```
