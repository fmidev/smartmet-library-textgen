# Story "wind_range"

The `wind_range` story has the form:

1. "[Pohjoistuulta] noin X metriä sekunnissa."
1. "[Pohjoistuulta] X...Y metriä sekunnissa."

In place of the phrase "pohjoistuulta" a phrase suitable for the data is inserted, of the form:

1. "pohjoistuulta"
1. "pohjoisen puoleista tuulta"
1. "suunnaltaan vaihtelevaa tuulta"

The wind interval is collapsed to a single number (the mean) if the interval is smaller than:

```
textgen::[section]::story::wind_range::mininterval = [0-] (0)
```

Settings related to the story:

```
textgen::[section]::story::wind_range::mininterval = [1-]
textgen::[section]::story::wind_range::direction::accurate (=22.5)
textgen::[section]::story::wind_range::direction::variable (=45)
```

The direction phrase is chosen as follows:

1. If quality < `accurate`, "N-tuulta"
1. If quality >= `variable`, "N-puoleista tuulta"
1. Otherwise, "suunnaltaan vaihtelevaa tuulta"

The generated analysis functions can be overridden as follows:

```
textgen::[section]::story::wind_range::fake::speed::minimum = [result]
textgen::[section]::story::wind_range::fake::speed::maximum = [result]
textgen::[section]::story::wind_range::fake::speed::mean = [result]

textgen::[section]::story::wind_range::fake::direction::mean = [result]
textgen::[section]::story::wind_range::fake::direction::sdev = [result]
```

Phrases used:

* "noin"
* "metriä sekunnissa"
* "suunnaltaan vaihtelevaa"
* "tuulta"
* "N-tuulta", N=1–8
* "N-puoleista tuulta", N=1–8
