# Story "roadcondition_overview"

The possible road conditions, in order of importance, are:

1. icy
1. partly icy
1. frost (rime)
1. wet snow (slush)
1. dry snow (snow)
1. wet
1. moist
1. dry

Typical sentences describing these conditions are:

1. Tiet ovat [yleisesti, monin paikoin, paikoin] jäisiä
1. Tiet ovat [yleisesti, monin paikoin, paikoin] osittain jäisiä.
1. Teillä on [yleisesti, monin paikoin, paikoin] kuuraa
1. Teillä on [-, monin paikoin, paikoin] sohjoa
1. Teillä on [-, monin paikoin, paikoin] lunta
1. Tiet ovat [-, monin paikoin, paikoin] märkiä
1. Tiet ovat [-, monin paikoin, paikoin] kosteita
1. Tiet ovat kuivia

In Swedish:

1. Vägarna är [allmänt, flerstädes, lokalt] isiga
1. Vägarna är [allmänt, flerstädes, lokalt] delvis isiga
1. Vägarna är [allmänt, flerstädes, lokalt] täckta av rimfrost
1. Vägarna är [-, flerstädes, lokalt] täckta av snösörja
1. Vägarna är [-, flerstädes, lokalt] täckta av snö
1. Vägarna är [-, flerstädes, lokalt] våta
1. Vägarna är [-, flerstädes, lokalt] fuktiga
1. Vägarna är torra

In English:

1. The roads are [generally, in many places, in some places] icy
1. The roads are [generally, in many places, in some places] partly icy
1. The roads are [generally, in many places, in some places] covered by frost
1. The roads are [-, in many places, in some places] covered by slush
1. The roads are [-, in many places, in some places] covered by snow
1. The roads are [-, in many places, in some places] wet
1. The roads are [-, in many places, in some places] moist
1. The roads are dry

In particular, "the roads are dry" always means there is no other road condition. In addition, the "generally" phrase is rarely used for more common conditions, as dropping the whole phrase achieves the same effect ("the roads are covered by snow").

The first three conditions (icy, partly icy, frost) are more important than the others.

The forecast covers only the first 30 hours. The length can be changed with:

```
::maxhours = [0-N] (=30)
```

but in practice the length is upper-bounded by the limited length of the road weather model. A sensible maximum is around 30 hours.

## Road condition for a single period

Define:

1. A condition is general if it occurs over Y percent. Y is large enough that no other condition can occur "in many places".
1. A condition is "in many places" if it occurs over M percent. Only one condition can occur "in many places", so M is over 50 percent.
1. A condition is "in some places" if it occurs over P percent, where P is under 50 percent.

In practice, P = 100 − Y, so "generally" and "in some places" are mutually exclusive. Sensible values are e.g. Y=90, M=50, P=10.

Settings can be controlled with:

```
textgen::[section]::story::roadcondition_overview::generally_limit = [0-100] (=90)
textgen::[section]::story::roadcondition_overview::manyplaces_limit = [0-100] (=50)
textgen::[section]::story::roadcondition_overview::someplaces_limit = [0-100] (=10)
```

Hence, if some condition is "general", the possible cases are already listed in the earlier example sentences. If one condition is "in many places" and nothing but "dry" is present even "in some places", we still end up with earlier example cases.

The cases that remain are:

1. One condition "in many places", one or more others "in some places"
1. Several conditions "in some places"

Below are listed all possible combinations where one condition is "in many places" and another is "in some places". If two or more other conditions are "in some places", only the _most important_ one is mentioned — not the most frequent.

|  | icy | partly icy | frost | slush | snow | wet | moist | dry |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| The roads are in many places icy | - | , in some places partly icy | , in some places covered by frost |  |  |  |  |  |
| The roads are in many places partly icy | , in some places icy | - | , in some places covered by frost |  |  |  |  |  |
| The roads are in many places covered by frost | , in some places the roads are icy | , in some places the roads are partly icy | - |  |  |  |  |  |
| The roads are in many places covered by slush | , in some places the roads are icy | , in some places the roads are partly icy | , in some places covered by frost | - | , in some places covered by snow |  |  |  |
| The roads are in many places covered by snow | , in some places the roads are icy | , in some places the roads are partly icy | , in some places covered by frost | , in some places covered by slush | - |  |  |  |
| The roads are in many places wet | , in some places icy | , in some places partly icy | , in some places covered by frost | , in some places covered by slush | , in some places covered by snow | - | , in some places moist |  |
| The roads are in many places moist | , in some places icy | , in some places partly icy | , in some places covered by frost | , in some places covered by slush | , in some places covered by snow | , in some places wet | - |  |

As a special case, if the roads are "in many places dry", only the remaining "in some places" conditions are reported. If there is only one such condition, the trivial list of individual conditions applies. If there are more, only the two most important are reported, according to the table below:

|  | partly icy | frost | slush | snow | wet | moist | dry |
| --- | --- | --- | --- | --- | --- | --- | --- |
| The roads are in some places icy | or partly icy | or covered by frost |  |  |  |  |  |
| The roads are in some places partly icy | - | or covered by frost |  |  |  |  |  |
| The roads are in some places covered by frost | - | - |  |  |  |  |  |
| The roads are in some places covered by slush | - | - | - | or covered by snow |  |  |  |
| The roads are in some places covered by snow | - | - | - | - |  |  |  |
| The roads are in some places wet | - | - | - | - | - | or moist |  |
| The roads are in some places moist | - | - | - | - | - | - |  |

## Road condition for multiple periods

The Finnish Road Administration is particularly interested in morning, day, evening, and night periods. In principle each of these can be reported separately, but for readability an algorithm is needed to merge two adjacent time intervals if their road conditions are sufficiently similar.

Definition:

> Two road conditions are the same if they would be described with the same text.

This makes it possible to design an algorithm that compares conditions directly by the percentage of each. However, possible algorithm changes can be made much more safely if the generated `Sentence` object is compared instead.

The comparison could be implemented by adding an `operator==` method to the `Sentence` object. However, this would suggest adding the operator to the entire `Glyph` hierarchy, where it would need to be implemented especially carefully (see for example the C++ Journal for the correct solution). A considerably simpler approach is to realize the text using `DebugDictionary` and `DebugTextFormatter` — then comparing the result requires only a plain string comparison.

What remains is choosing a phrase describing the combined time interval for merged conditions.

The periods are:

1. morning
1. day
1. evening
1. night

The easiest way to combine time phrases is to use an "alkaen" ("starting from …") phrase, i.e.:

1. "aamusta alkaen" (from the morning)
1. "aamupäivästä alkaen" (from before noon)
1. "illasta alkaen" (from the evening)
1. "yöstä alkaen" (from the night)

Note that the day start is refined to "before noon". In addition, a day-specifying time phrase is needed — the "today" type fits.

Possible results, for example:

| today | tomorrow | Monday |
| --- | --- | --- |
| aamulla | huomisaamuna | maanantaiaamuna |
| päivällä | huomenna päivällä | maanantaina päivällä |
| illalla | huomenna illalla | maanantaina illalla |
| yöllä | tiistain vastaisena yönä | tiistain vastaisena yönä |
| aamusta alkaen | huomisaamusta alkaen | maanantaiaamusta alkaen |
| aamupäivästä alkaen | huomenna aamupäivästä alkaen | maanantaiaamupäivästä alkaen |
| illasta alkaen | huomenna illasta alkaen | maanantai-illasta alkaen |
| yöstä alkaen | tiistain vastaisesta yöstä alkaen | tiistain vastaisesta yöstä alkaen |

When using the phrases, the most recent date qualifier must be remembered, because the phrases "huomenna" and "viikonpäivänä" should not be repeated. When the qualifier is the same, the time phrase type is always chosen as "today". The time phrases should therefore be selected with such reduction in mind.

For example:

* Huomisaamuna X, päivällä Y, illasta alkaen Z
* Illasta alkaen X, huomisaamusta alkaen Y, illalla Z.
* Huomenna illasta alkaen X, tiistaiaamuna Y, päivällä Z

In Swedish:

| today | tomorrow | Monday |
| --- | --- | --- |
| på morgonen | i morgon bitti | på måndag morgon |
| på dagen | i morgon på dagen | på måndag på dagen |
| på kvällen | i morgon kväll | på måndag kväll |
| på natten | natten mot tisdagen | natten mot tisdagen |
| från morgonen | från morgon bitti | från måndag morgon |
| från förmiddagen | från i morgon förmiddag | från måndag förmiddag |
| från kvällen | från i morgon kväll | från måndag kväll |
| från natten | från natten mot tisdag | från natten mot tisdag |

And in English:

| today | tomorrow | Monday |
| --- | --- | --- |
| in the morning | tomorrow morning | on Monday morning |
| during the day | tomorrow during the day | on Monday during the day |
| in the evening | tomorrow evening | on Monday evening |
| at night | on Tuesday night | on Tuesday night |
| from the morning | from tomorrow morning | from Monday morning |
| from before noon | from tomorrow before noon | from Monday before noon |
| from the evening | from tomorrow evening | from Monday evening |
| from the night | from tomorrow night | from Tuesday night |

## Phrases required

Phrases required in each language:

| Finnish | Swedish | English |
| --- | --- | --- |
| tiet ovat | vägarna är | the roads are |
| teillä on | vägarna är | the roads are |
| jäisiä | isiga | icy |
| osittain jäisiä | delvis isiga | partly icy |
| kuuraa | täckta av rimfrost | covered by frost |
| sohjoa | täckta av snösörja | covered by slush |
| lunta | täckta av snö | covered by snow |
| märkiä | våta | wet |
| kosteita | fuktiga | moist |
| kuivia | torra | dry |
| kuuraisia | täckta av rimfrost | covered by frost |
| sohjoisia | täckta av snösörja | covered by slush |
| lumisia | täckta av snö | covered by snow |
| tiet ovat (subordinate clause) | (empty) | (empty) |
| yleisesti | allmänt | generally |
| monin paikoin | flerstädes | in many places |
| paikoin | lokalt | in some places |

Additionally, the time phrases listed below are required:

| key | fi | sv | en |
| --- | --- | --- | --- |
| N-aamuna | maanantaiaamuna | på måndag morgon | on Monday morning |
| N-aamupäivästä alkaen | maanantaiaamupäivästä alkaen | från måndag förmiddag | from Monday before noon |
| N-aamusta alkaen | maanantaiaamusta alkaen | från måndag morgon | from Monday morning |
| N-iltana | maanantai-iltana | på måndag kväll | on Monday evening |
| N-illasta alkaen | maanantai-illasta alkaen | från måndag kväll | from Monday evening |
| N-päivällä | maanantaina päivällä | på måndag på dagen | on Monday during the day |
| N-vastaisena yönä | tiistain vastaisena yönä | natten mot tisdagen | on Tuesday night |
| N-vastaisesta yöstä alkaen | tiistain vastaisesta yöstä alkaen | från natten mot tisdag | from Tuesday night |
| aamulla | aamulla | på morgonen | in the morning |
| aamusta alkaen | aamusta alkaen | från morgonen | from the morning |
| päivällä | päivällä | på dagen | during the day |
| aamupäivästä alkaen | aamupäivästä alkaen | från förmiddagen | from before noon |
| illalla | illalla | på kvällen | in the evening |
| illasta alkaen | illasta alkaen | från kvällen | from the evening |
| yöllä | yöllä | på natten | at night |
| yöstä alkaen | yöstä alkaen | från natten | from the night |
| huomisaamuna | huomisaamuna | i morgon bitti | tomorrow morning |
| huomisaamusta alkaen | huomisaamusta alkaen | från morgon bitti | from tomorrow morning |
| huomisaamupäivästä alkaen | huomisaamupäivästä alkaen | från i morgon förmiddag | from tomorrow before noon |
| huomisillasta alkaen | huomisillasta alkaen | från i morgon kväll | from tomorrow evening |
| huomenna päivällä | huomenna päivällä | i morgon på dagen | tomorrow during the day |
| huomisiltana | huomisiltana | i morgon kväll | tomorrow evening |
