# Creating subperiods

## Subperiods

For each paragraph you can separately specify whether its associated period should be split into smaller parts. For example, if the configuration file contains the section:

```
textgen::sections::part1
```

the part can be split into days like this:

```
textgen::part1
{
    subperiods = true
    subperiod::day
    {
        starthour = 6
        endhour = 6
        maxstarthour = 24
        minendhour = 18
    }
}
```

In that case the stories associated with each day are listed in the variables:

```
textgen::part1::day1::content
textgen::part1::day2::content
textgen::part1::day3::content
...
```

or, if the variable for the relevant day is not set, the variable

```
textgen::part1::content
```

is used.

Stories of the form `parameter_day` are the best fit for subperiods. Other stories may also work, but they are generally not designed specifically for periods under 24 hours.
