# Story "dewpoint_range"

The `dewpoint_range` story has the form:

1. "Kastepiste on noin X astetta."
1. "Kastepiste on X...Y astetta."

In Swedish:

1. "Daggpunkt är cirka X grader."
1. "Daggpunkt är X...Y grader."

In English:

1. "Dewpoint is about X degrees."
1. "Dewpoint is X...Y degrees."

Phrases used:

* "kastepiste"
* "noin"
* "astetta"

The generated analysis functions can be overridden as follows:

```
1. Mean(Min(Dewpoint))...Mean(Max(Dewpoint))
textgen::[section]::story::dewpoint_range::fake::minimum = [result]
textgen::[section]::story::dewpoint_range::fake::maximum = [result]
```

Variables related to the story are:

```
textgen::[section]::story::dewpoint_range::mininterval = [1-]
```

If the interval length is at least `mininterval`, an interval is reported; otherwise the mean is reported.
