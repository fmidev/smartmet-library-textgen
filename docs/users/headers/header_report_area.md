# Header type "report_area"

Produces a header of the form "Sääennuste Uudellemaalle tiistaina kello 18". The clock time is taken from the start of the period. The type pairs particularly well with the period type "now".

Possible headers:

1. "Sääennuste alueelle N-na kello K".

In Swedish:

1. "Väderraport för omrodet N-na klockan K."

In English:

1. "Weather report for area N-na K o'clock."

The header is adjusted with the variables:

```
textgen::[section]::header::compact = [bool] (=false)
textgen::[section]::header::date = [bool] (=true)
textgen::[section]::header::time = [bool] (=true)
```

With `compact` only the area name is given, without "Sääennuste ...lle". With
`date` the weekday is included, with `time` the clock time. The area must be
named.
