# Period phrases

## Introduction

Period phrases are phrases that describe some period. Typically this is a near-term time, such as the day currently in progress, the next night, or tomorrow.

## PeriodPhraseFactory

The `create` method of the `TextGen::PeriodPhraseFactory` namespace creates `TextGen::Sentence` objects containing the essential phrases describing a period. As a rule, the sentence contains only a single phrase, i.e. the sentence is intended to be attached to another sentence using `operator<<`.

The API of the `create` method is:

```cpp
TextGen::Sentence create(const std::string & theType,
                         const std::string & theVariable,
                         const NFmiTime & theForecastTime,
                         const WeatherAnalysis::WeatherPeriod & thePeriod);
```

The variable `theVariable` is the prefix for the variables needed to generate a phrase of type `theType`. Typically `theVariable` is of the form `textgen::[section]::[story]::[period]`.

### Variables

Period phrases are controlled with variables of the form:

```
textgen::[section]::[story]::[period] = [preference_order].
```

The variable's value is a list of names that defines the preference order of the different phrases. The possible names are listed below:

| Name | Produces (Finnish examples; translated) |
| --- | --- |
| weekday | "on weekday", "on the night toward weekday", "starting from weekday" |
| today | "today" |
| tonight | "tonight" |
| tomorrow | "tomorrow", "starting from tomorrow" |
| atday | "during the day" |
| atnight | "at night" |
| followingday | "on the following day" |
| followingnight | "on the following night" |
| none | - |
| none! | - |

Each story defines a style for each of its time periods, e.g. `today` and `next_day`. Each of these has a default order for the respective phrase. The user-defined order takes preference over the default.

### "until_tonight"

The period covers the day currently in progress up to evening. The possible phrases in default order are:

1. none (*)
1. today (*)
1. atday (*)
1. weekday
1. none!

The phrases marked with an asterisk have additional conditions relative to the forecast time.

### "until_morning"

The period covers the span up to the following morning, possibly including a few hours of the day currently in progress. The period is typically used when the day has already progressed so far that the night period is about to begin.

Possible phrases in default order:

1. none (*)
1. tonight (*)
1. atnight (*)
1. weekday
1. none!

### "today"

The period covers some daytime span, possibly a full 24-hour day. Often the span is the day currently in progress, but not always.

Possible phrases in default order:

1. none (*)
1. today (*)
1. tomorrow (*)
1. atday (*)
1. weekday
1. none!

### "tonight"

The period covers some nighttime span. Often the span is the next night, but not always.

Possible phrases in default order:

1. none (*)
1. tonight (*)
1. atnight (*)
1. weekday
1. none!

### "days"

The period covers at least one day, possibly two or more. Often the span starts from the day currently in progress, often from tomorrow.

Possible phrases in default order:

1. none (*)
1. today (*)
1. tomorrow (*)
1. followingday
1. weekday
1. none!

The order of phrases can be chosen with the normal `::days::phrases` variable, but it can also be overridden for different period lengths with the variables:

```
::days::phrases::days1
::days::phrases::days2
::days::phrases::days3
...
```

Day counting uses the normal variables:

```
::day::starthour
::day::endhour
::day::maxstarthour
::day::minendhour
```

The corresponding realized phrases are:

1. If there is one day:
   1. ""
   1. "Today"
   1. "Tomorrow"
   1. (not used)
   1. "On weekday"
1. If there are two days:
   1. ""
   1. Depends on tomorrow,followingday,weekday order:
      1. "Today and tomorrow" if tomorrow
      1. "Today and on the following day" if followingday
      1. "Today and on weekday" if weekday
   1. Depends on followingday,weekday order:
      1. "Tomorrow and on the following day" if followingday
      1. "Tomorrow and on weekday" if weekday
   1. (not used)
   1. "On weekday and weekday"
1. If there are three or more days:
   1. ""
   1. (not used)
   1. "Starting from tomorrow"
   1. (not used)
   1. "Starting from weekday"

### "next_night"

The period covers a night, without any daytime moments (which are reported separately before this part).

Possible phrases in default order:

1. tonight (*)
1. atnight (*)
1. followingnight
1. weekday
1. none!

### "next_day"

The period covers the span from the following morning to evening, or the full following 24-hour day.

Possible phrases in default order:

1. tomorrow (*)
1. followingday
1. weekday
1. none!

### "next_days"

The period covers the span from the following morning onward, for at least two days (possibly slightly under 48h).

Possible phrases in default order:

1. tomorrow (*)
1. weekday
1. none!

### "remaining_days"

The period covers the span from the following morning onward, for one or more days.

1. If length = 1 day
   1. Returns the result of `next_day`
1. Otherwise
   1. Returns the result of `next_days`

### "remaining_day"

The period covers the remainder of the day without an actual day specifier. Often the combination "today" + "remaining_day" is used to produce phrases like "today starting from the afternoon".

Phrases are chosen based on the period's start hour according to the following table:

| Hour | Phrase |
| --- | --- |
| 00 | - |
| 01 | - |
| 02 | - |
| 03 | starting from late night |
| 04 | starting from late night |
| 05 | starting from late night |
| 06 | starting from the morning |
| 07 | starting from the morning |
| 08 | starting from the morning |
| 09 | starting from before noon |
| 10 | starting from before noon |
| 11 | starting from midday |
| 12 | starting from midday |
| 13 | starting from the afternoon |
| 14 | starting from the afternoon |
| 15 | starting from the afternoon |
| 16 | starting from the afternoon |
| 17 | starting from the afternoon |
| 18 | starting from the evening |
| 19 | starting from the evening |
| 20 | starting from the evening |
| 21 | starting from the evening |
| 22 | starting from the night |
| 23 | starting from the night |

Note that the latter part of the table should only be used very rarely — the day's forecast should not be reported very late on the same day. In practice the latest reasonable time of day is somewhere around the early evening.
