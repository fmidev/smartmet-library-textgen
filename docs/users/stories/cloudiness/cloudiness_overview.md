# Story "cloudiness_overview"

## Story form

The story describes the cloudiness of a period of one or more days. It is best suited to the first forecast days.

For one day, the story consists of one of the following sub-stories:

1. [enimmäkseen] pilvistä
1. [enimmäkseen] puolipilvistä
1. [enimmäkseen] selkeää
1. pilvistä tai puolipilvistä
1. selkeää tai puolipilvistä
1. pilvistyvää
1. selkenevää
1. vaihtelevaa pilvisyyttä

Forecasts for two or more days are built from the same sub-stories. If the same sub-story falls on consecutive days, it is mentioned only once.

Examples:

1. Tänään ja huomenna enimmäkseen pilvistä, keskiviikkona selkenevää.
1. Tänään pilvistyvää, huomisesta alkaen vaihtelevaa pilvisyyttä.

## Algorithm

The variables used by the algorithm are:

```
::story::cloudiness_overview::clear = [0-100] (=40)
::story::cloudiness_overview::cloudy = [0-100] (=70)

::story::cloudiness_overview::single_class_limit = [0-100]  (=90)
::story::cloudiness_overview::mostly_class_limit = [0-100]  (=80)
::story::cloudiness_overview::no_class_limit = [0-100]      (=20)
::story::cloudiness_overview::trend_limit = [0-100]         (=80)
::story::cloudiness_overview::merge_same = [true](false)     (=true)
::story::cloudiness_overview::merge_similar = [true](false)  (=true)

::story::cloudiness_overview::day::starthour = [0-23]
::story::cloudiness_overview::day::endhour = [0-23]
::story::cloudiness_overview::day::maxstarthour = [0-23]    (=starthour)
::story::cloudiness_overview::day::minendhour = [0-23]      (=minendhour)
```

A percentage of 40 corresponds to cloudiness class 3.2, and 70 to cloudiness class 5.6. Better defaults might be based on the exact values 3.5 and 5.5, but more easily remembered limits are likely preferable here.

If `merge_same` is `true`, consecutive days with the same cloudiness description are merged into a single description. If `merge_similar` is `true`, similar descriptions are additionally merged — for example, "pilvistä" and "enimmäkseen pilvistä". The `merge_same` setting is not needed if `merge_similar` is `true`.

Note that the day should not be defined too short. Otherwise, though rarely, a situation may arise where two consecutive days are forecast to be "becoming cloudy".

The cloudiness type for a single day is determined as follows:

1. Compute the percentage of each class.
   1. The percentage of partly cloudy can be derived from the percentages of the other two types.
1. If cloudy is at least `single_class_limit`, "pilvistä"
1. If clear is at least `single_class_limit`, "selkeää"
1. If partly cloudy is at least `single_class_limit`, "puolipilvistä"
1. If cloudy is at least `mostly_class_limit`, "enimmäkseen pilvistä"
1. If clear is at least `mostly_class_limit`, "enimmäkseen selkeää"
1. If partly cloudy is at least `mostly_class_limit`, "enimmäkseen puolipilvistä"
1. If clear is less than `no_class_limit`, "pilvistä tai puolipilvistä"
1. If cloudy is less than `no_class_limit`, "selkeää tai puolipilvistä"
1. If the increasing trend is at least `trend_limit`, "pilvistyvää"
1. If the decreasing trend is at least `trend_limit`, "selkenevää"
1. "vaihtelevaa pilvisyyttä"

Results can be tested using the helper variables:

```
::fake::day1::cloudy = [result]
::fake::day1::clear  = [result]
::fake::day1::trend  = [result]
```

Subsequent days' results can be set similarly.

### One day

For one day the algorithm above is used as-is.

### Two days

1. If `merge_same` or `merge_similar` is `true`, each day is processed separately.
1. Each day's story is generated separately without period phrases.
1. If the stories differ, period phrases are added and the stories are concatenated.
1. Otherwise the period phrases are joined with "and" before the shared story.

Examples:

* "Tänään selkeää, huomenna pilvistyvää."
* "Tänään ja huomenna pilvistä."

### Three days

1. If `merge_same` or `merge_similar` is `true`, each day is processed separately.
1. Each day's story is generated separately without period phrases.
1. If the stories for the first two days are the same, their period phrases are joined with "and" before the shared story, and the third day is appended with its own period phrase.
1. If the stories for the last two days are the same, their period phrases are combined into an "alkaen" ("starting from …") form, and the first day gets its own period phrase.
1. Otherwise each day is reported separately.

Examples:

* "Tänään ja huomenna pilvistä, torstaina enimmäkseen selkeää."
* "Tänään pilvistyvää, huomisesta alkaen enimmäkseen pilvistä."
* "Tänään pilvistä, huomenna selkeää, torstaina pilvistyvää."

This is actually a special case of the following algorithm for several days.

### Several days

1. If `merge_same` or `merge_similar` is `true`, each day is processed separately.
1. Each day's story is generated separately without period phrases.
1. Loop over the stories:
   1. If the next story differs from the current day's story:
      1. Generate its own period phrase before the story.
   1. Otherwise, if similar days continue to the end:
      1. Generate an "alkaen"-type period phrase for the story.
   1. Otherwise, if there are only two similar days:
      1. Join the period phrases with "and".
   1. Otherwise, there are at least three similar days but not to the end:
      1. Join the period phrases into an "alkaen"-type expression.

### Story similarity

The cloudiness descriptions of two days may be similar enough that it makes sense to merge them in order to clarify the forecast.

All possible stories are listed below, approximately in order from cloudy to clear, to make the merging clearer:

1. pilvistä
1. pilvistyvää
1. enimmäkseen pilvistä
1. pilvistä tai puolipilvistä
1. enimmäkseen puolipilvistä
1. vaihtelevaa pilvisyyttä
1. puolipilvistä
1. selkeää tai puolipilvistä
1. enimmäkseen selkeää
1. selkenevää
1. selkeää

In the table below, the vertical column represents the first day's cloudiness and the horizontal row represents the second day's cloudiness. The combinations shown in the table are the permitted merges.

|  | pilvistä | pilvistyvää | enimmäkseen pilvistä | pilvistä tai puolipilvistä | enimmäkseen puolipilvistä | vaihtelevaa pilvisyyttä | puolipilvistä | selkeää tai puolipilvistä | enimmäkseen selkeää | selkenevää | selkeää |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| pilvistä | = | - | enimmäkseen pilvistä | pilvistä tai puolipilvistä | pilvistä tai puolipilvistä | - | pilvistä tai puolipilvistä | - | - | - | - |
| pilvistyvää | pilvistyvää | = | pilvistyvää | - | - | vaihtelevaa pilvisyyttä | - | - | - | - | - |
| enimmäkseen pilvistä | enimmäkseen pilvistä | - | = | pilvistä tai puolipilvistä | pilvistä tai puolipilvistä | - | pilvistä tai puolipilvistä | - | - | - | - |
| pilvistä tai puolipilvistä | pilvistä tai puolipilvistä | - | pilvistä tai puolipilvistä | = | pilvistä tai puolipilvistä | vaihtelevaa pilvisyyttä | pilvistä tai puolipilvistä | vaihtelevaa pilvisyyttä | - | - | - |
| enimmäkseen puolipilvistä | pilvistä tai puolipilvistä | - | pilvistä tai puolipilvistä | vaihtelevaa pilvisyyttä | = | vaihtelevaa pilvisyyttä | enimmäkseen puolipilvistä | selkeää tai puolipilvistä | selkeää tai puolipilvistä | - | selkeää tai puolipilvistä |
| vaihtelevaa pilvisyyttä | - | vaihtelevaa pilvisyyttä | vaihtelevaa pilvisyyttä | vaihtelevaa pilvisyyttä | vaihtelevaa pilvisyyttä | = | vaihtelevaa pilvisyyttä | vaihtelevaa pilvisyyttä | - | - | - |
| puolipilvistä | puolipilvistä tai pilvistä | - | pilvistä tai puolipilvistä | pilvistä tai puolipilvistä | enimmäkseen puolipilvistä | vaihtelevaa pilvisyyttä | = | selkeää tai puolipilvistä | selkeää tai puolipilvistä | - | selkeää tai puolipilvistä |
| selkeää tai puolipilvistä | - | - | vaihtelevaa pilvisyyttä | vaihtelevaa pilvisyyttä | selkeää tai puolipilvistä | vaihtelevaa pilvisyyttä | selkeää tai puolipilvistä | = | selkeää tai puolipilvistä | - | selkeää tai puolipilvistä |
| enimmäkseen selkeää | - | - | - | - | selkeää tai puolipilvistä | - | selkeää tai puolipilvistä | selkeää tai puolipilvistä | = | - | enimmäkseen selkeää |
| selkenevää | - | - | - | - | - | - | - | - | selkenevää | = | selkenevää |
| selkeää | - | - | - | - | selkeää tai puolipilvistä | - | selkeää tai puolipilvistä | selkeää tai puolipilvistä | enimmäkseen selkeää | - | = |

For example, "vaihtelevaa pilvisyyttä" and "pilvistyvää" are merged into "vaihtelevaa pilvisyyttä". However, the phrases "vaihtelevaa pilvisyyttä" and "selkenevää" are intentionally kept separate, because clearing is generally a significant change to better weather, and does not necessarily imply any other reportable change in precipitation.

For three or more days, cloudiness is considered the same if every two consecutive days can be merged, and their results can in turn be merged recursively into a single result.

## Phrases required

Phrases used:

* "enimmäkseen"
* "pilvistä"
* "puolipilvistä"
* "selkeää"
* "tai"
* "ja"
* "pilvistyvää"
* "selkenevää"
* "vaihtelevaa pilvisyyttä"

Additionally, the period phrases `days` are required.
