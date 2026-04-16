# Story "wind_overview"

> **Status:** Primary. The flagship wind narrative. Implementation: `textgen/wind_overview.cpp` (~3 959 LOC).
>
> **Owner:** `WindStory::overview()`.

The 2005-era specification below describes the intended behaviour. The
current implementation matches this closely but has been extended — when in
doubt, consult the source and the unit tests under `test/WindStoryTest.cpp`.

## Story form

The story describes a wind forecast for a one- or several-day period. It is best suited to the first 1–3 days.

The general form is:

1. [until the next morning], [remaining time]

The time definitions for the first part come from the variables:

```
textgen::[section]::story::wind_overview::day::starthour = [0-23] (6)
textgen::[section]::story::wind_overview::day::maxstarthour = [0-23] (starthour)
textgen::[section]::story::wind_overview::night::starthour = [0-23] (18)
textgen::[section]::story::wind_overview::night::maxstarthour = [0-23] (starthour)
textgen::[section]::story::wind_overview::night::endhour = [0-23] (06)
textgen::[section]::story::wind_overview::night::minendhour = [0-23] (endhour)
```

Typical settings are:

```
textgen::[section]::story::wind_overview::day::starthour = 6
textgen::[section]::story::wind_overview::day::maxstarthour = 12
textgen::[section]::story::wind_overview::night::starthour = 18
textgen::[section]::story::wind_overview::night::maxstarthour = 0
textgen::[section]::story::wind_overview::night::endhour = 6
```

With these, the story reports up to the next morning, always until midnight, after which the upcoming day is reported.

### First day

If the day has progressed too far — i.e. the clock is past `day::maxstarthour` — the wind is reported up to the following morning. Both the remaining day and the night are included.

Possible stories for this "extended" night period:

1. "[Ajanjaksona] tyyntä."
1. "[Ajanjaksona] heikkoa tuulta."
1. "[Ajanjaksona] [voimakasta] [suuntatuulta]."
1. "[Ajanjaksona] myrskyä."
1. "[Ajanjaksona] hirmumyrskyä."
1. "[Ajanjaksona] [suuntatuulta] X-Y m/s."

The time phrase type is [until_morning](../../period_phrases.md#until_morning), controlled with:

```
textgen::[section]::story::wind_overview::until_morning::phrases
```

A suitable value is, for example:

```
textgen::[section]::story::wind_overview::until_morning::phrases = atnight,weekday
```

because the phrase "tonight" may not be desirable; "atnight" likely fits better.

Possible wind strengths:

1. "kohtalaista" (moderate)
1. "navakkaa" (fresh)
1. "kovaa" (strong)

and directions:

1. "pohjoistuulta" (north)
1. "koillistuulta" (northeast)
1. "itätuulta" (east)
1. "kaakkoistuulta" (southeast)
1. "etelätuulta" (south)
1. "lounaistuulta" (southwest)
1. "länsituulta" (west)
1. "luoteistuulta" (northwest)
1. "pohjoisen puoleista tuulta" (approximately north)
1. "koillisen puoleista tuulta" (approximately northeast)
1. "idän puoleista tuulta" (approximately east)
1. "kaakon puoleista tuulta" (approximately southeast)
1. "etelän puoleista tuulta" (approximately south)
1. "lounaan puoleista tuulta" (approximately southwest)
1. "lännen puoleista tuulta" (approximately west)
1. "luoteen puoleista tuulta" (approximately northwest)
1. "suunnaltaan vaihtelevaa tuulta" (variable direction)

Direction and speed computation for different periods is handled separately below.

If the day is long enough to handle, it is handled like the night above. In that case the time phrase is of type [until_tonight](../../period_phrases.md#until_tonight), controlled with:

```
textgen::[section]::story::wind_overview::until_tonight::phrases
```

A recommended value is, for example:

```
textgen::[section]::story::wind_overview::until_tonight::phrases = atday,weekday
```

In addition, a subordinate clause about the night's wind is generated, of the form:

1. ", [ajanjaksona] [voimakasta] [suuntatuulta]."
1. ", [ajanjaksona] tuuli heikkenee."
1. ", [ajanjaksona] tuuli [heikkenee/voimistuu] [voimakkaaksi]."
1. ", [ajanjaksona] tuuli [heikkenee/voimistuu] [voimakkaaksi] [suuntatuuleksi]."
1. ", [ajanjaksona] [suuntatuulta]."
1. ", [ajanjaksona] [suuntatuulta] X-Y m/s."

The subordinate clause's time phrase is of type [tonight](../../period_phrases.md#tonight), controlled with:

```
textgen::[section]::story::wind_overview::tonight::phrases
```

A recommended value is, for example:

```
textgen::[section]::story::wind_overview::tonight::phrases = atnight,weekday
```

In this context "tonight" could also be used, which was not recommended when the day part was absent.

In addition, the day and night winds may be similar enough that no subordinate clause is used. In that case no time phrase is used at all.

### Second day

In this case the end of the examined interval is at most 24 hours away, so we can talk only about the following day.

The second day can be reported in its own sentence or as a subordinate clause. The form is:

1. "[Ajanjaksona] edelleen tyyntä."
1. "[Ajanjaksona] edelleen [voimakasta] tuulta."
1. "[Ajanjaksona] edelleen [voimakasta] tuulta."
1. "[Ajanjaksona] [suuntatuulta]."
1. "[Ajanjaksona] tuuli tyyntyy."
1. "[Ajanjaksona] tuuli heikkenee."
1. "[Ajanjaksona] tuuli heikkenee [voimakkaaksi]."
1. "[Ajanjaksona] tuuli voimistuu [voimakkaaksi]."
1. "[Ajanjaksona] [voimakasta] [suuntatuulta]."
1. "[Ajanjaksona] [voimakkaaksi] [heikkenevää/voimistuvaa] [suuntatuulta]."

The time phrase is of type [next_day](../../period_phrases.md#next_day), controlled with:

```
textgen::[section]::story::wind_overview::next_day::phrases
```

A recommended value is, for example:

```
textgen::[section]::story::wind_overview::next_day::phrases = tomorrow,followingday
```

### Remaining days

In this case the end of the examined interval is more than 24 hours away. This remaining time is reported as one sentence or subordinate clause:

1. "[ajanjaksosta alkaen] tyyntä."
1. "[ajanjaksosta alkaen] [suuntatuulta]."
1. "[ajanjaksosta alkaen] [voimakasta] [suuntatuulta]."
1. "[ajanjaksosta alkaen] [heikkenevää/voimistuvaa] [suuntatuulta]."
1. "[ajanjaksosta alkaen] [voimakkaaksi] [heikkenevää/voimistuvaa] [suuntatuulta]."
1. "[ajanjaksosta alkaen] [voimakkaaksi] [heikkenevää/voimistuvaa] tuulta."
1. "[ajanjaksosta alkaen] [voimakasta] tai [voimakasta] [suuntatuulta]."
1. "[ajanjaksosta alkaen] [voimakasta] tai [voimakasta] tuulta."
1. "[ajanjaksosta alkaen] ajoittain [voimakasta] tuulta."
1. "[ajanjaksosta alkaen] ajoittain [voimakasta] [suuntatuulta]."

The time phrase is of type [next_days](../../period_phrases.md#next_days), controlled with:

```
textgen::[section]::story::wind_overview::next_days::phrases
```

A recommended value is, for example:

```
textgen::[section]::story::wind_overview::next_days::phrases = tomorrow,weekday
```

## Algorithms

### Wind speed

The wind speed class for each interval is obtained by computing the mean speed over time and space.

The speed range is obtained by computing the minimum and maximum of the time means.

The value of:

```
textgen::[section]::story::wind_overview::range_limit = [0-] (0)
```

is the limit below which the wind is reported as a range. In practice the value 0 means no range is ever given.

### Wind direction

The direction for each interval is obtained by computing the mean direction over time and space. Based on the direction's quality index, the accuracy is first weakened to "approximately N" and then to "variable".

Required quality is controlled by:

```
textgen::[section]::story::wind_overview::direction::accurate (= 0.8)
textgen::[section]::story::wind_overview::direction::variable (= 0.4)
```

Defaults are given in parentheses.

The direction phrase is chosen as follows:

1. If quality >= `accurate`, "N-tuulta"
1. If quality >= `variable`, "N-puoleista tuulta"
1. Otherwise, "suunnaltaan vaihtelevaa tuulta"

### Algorithm — first day

If only the extended night period remains:

1. Compute mean speed and direction
1. If the mean falls below `range_limit`, "[suuntatuulta] X-Y m/s."
1. Compute the speed class
1. If the class is calm, "Tyyntä."
1. If the class is light wind, "Heikkoa tuulta."
1. If the class is storm, "Myrskyä."
1. If the class is severe storm, "Hirmumyrskyä."
1. Otherwise "[Voimakasta] [suuntatuulta]."

If both a day and a night period remain, the day period is handled as above. In addition, the computed speed and direction classes are stored for later use.

The night's story is generated after the time qualifier as follows:

1. If the mean wind falls below `range_limit`, "[suuntatuulta] X-Y m/s."
1. If the speed and direction class are the same, the night is not reported separately.
1. If the speed is the same, "[suuntatuulta]."
1. If the speed class increases:
   1. If the class is light wind, "heikkoa tuulta"
   1. Otherwise "[voimakasta] [suuntatuulta]."
1. If the speed class decreases:
   1. If the class is calm, "tuuli tyyntyy."
   1. If the class is light wind, "tuuli heikkenee."
   1. Otherwise "[voimakasta] [suuntatuulta]."

The last reported speed and direction class is stored for handling the next period.

### Algorithm — second day

The story part is generated as follows:

1. If the mean wind falls below `range_limit`:
   1. If the direction is the same as earlier, "X-Y m/s"
   1. Otherwise "[suuntatuulta] X-Y m/s"
1. If a single class suffices for describing the speed:
   1. If the wind strengthens:
      1. If the class is light wind, "heikkoa tuulta"
      1. If the class is storm, "tuuli voimistuu myrskyksi"
      1. If the class is severe storm, "hirmumyrskyä"
      1. Otherwise "tuuli voimistuu [voimakkaaksi] [suuntatuuleksi]"
   1. If the wind weakens:
      1. If the class is calm, "tuuli tyyntyy".
      1. If the class is light wind, "tuuli heikkenee"
      1. If the class is storm, "myrskyä"
      1. Otherwise "tuuli heikkenee [voimakkaaksi] [suuntatuuleksi]"
1. If there is a clear trend:
   1. If at the end calm, "tuuli tyyntyy"
   1. If at the end light wind, "tuuli heikkenee"
   1. If at the end storm, "tuuli voimistuu myrskyksi"
   1. If at the end severe storm, "tuuli voimistuu hirmumyrskyksi"
   1. If the direction description is good, "[voimakkaaksi] heikkenevää/voimistuvaa [suuntatuulta]"
   1. Otherwise "tuuli heikkenee/voimistuu [voimakkaaksi]"
1. Compute the maximum of the mean wind:
   1. If the direction is clear, "ajoittain [voimakasta] [suuntatuulta]"
   1. Otherwise "ajoittain [voimakasta] tuulta"

### Algorithm — from second day onward

In this case the period length likely exceeds 24 hours, and the wind must be summarised relative to the first day and night.

1. Compute the speed and direction class
1. If the speed class is good:
   1. If the direction class is good, "[voimakasta] [suuntatuulta]"
   1. Otherwise "[voimakasta] [tuulta]"
1. If there is a good trend:
   1. Compute a new speed class for the last 12 hours
   1. If the direction class is good, "[voimakkaaksi] heikkenevää/voimistuvaa [suuntatuulta]"
   1. Otherwise "[voimakkaaksi] heikkenevää/voimistuvaa tuulta"
1. If the speed can be described with two adjacent classes:
   1. If the direction class is good, "[voimakasta] tai [voimakasta] [suuntatuulta]"
   1. Otherwise "[voimakasta] tai [voimakasta] tuulta"
1. Take the mean's maximum and "ajoittain [voimakasta] tuulta"

## Wind classes

The established wind classes are:

| Speed | Finnish | Swedish |
| --- | --- | --- |
| 0 m/s | tyyntä | lugnt |
| 1–3 m/s | heikkoa tuulta | svag vind |
| 4–7 m/s | kohtalaista tuulta | måttlig vind |
| 8–13 m/s | navakkaa tuulta | frisk vind |
| 14–20 m/s | kovaa tuulta | hård vind |
| 21–32 m/s | myrskyä | storm |
| 32– m/s | hirmumyrskyä | orkan |

## Phrases required

Phrases required:

* "m/s"
* "metriä sekunnissa"
* "tyyntä"
* "heikkoa"
* "kohtalaista"
* "navakkaa"
* "kovaa"
* "myrskyä"
* "hirmumyrskyä"
* "tyyneksi"
* "heikoksi"
* "kohtalaiseksi"
* "navakaksi"
* "kovaksi"
* "myrskyksi"
* "hirmumyrskyksi"
* "tuuli"
* "tuulta"
* "yöllä"
* "tuuli tyyntyy"
* "tuuli heikkenee"
* "tuuli voimistuu"
* "edelleen"
* "heikkenevää"
* "voimistuvaa"
* "suunnaltaan vaihtelevaa"
* "N-tuulta", N=1–8
* "N-tuuleksi", N=1–8
* "N-puoleista tuulta", N=1–8
* "N-puoleiseksi tuuleksi", N=1–8

Additionally, the period phrases [until_tonight](../../period_phrases.md#until_tonight), [until_morning](../../period_phrases.md#until_morning), [tonight](../../period_phrases.md#tonight), [next_day](../../period_phrases.md#next_day), and [next_days](../../period_phrases.md#next_days) are required.

Key phrases in different languages:

| fi | sv | en |
| --- | --- | --- |
| metriä sekunnissa | meter i sekunden | meters per second |
| tyyntä | lugnt | calm winds |
| heikkoa | svag | light |
| kohtalaista | måttlig | moderate |
| navakkaa | frisk | fresh |
| kovaa | hård | strong |
| myrskyä | storm | stormy |
| hirmumyrskyä | orkan | severe storm |
| tyyneksi | till lugnt | calm |
| heikoksi | till svag | light |
| kohtalaiseksi | till måttlig | moderate |
| navakaksi | till frisk | fresh |
| kovaksi | till hård | strong |
| myrskyksi | till storm | storm |
| hirmumyrskyksi | till orkan | severe storm |
| tuuli | vinden | wind |
| tuulta | vind | wind |
| tuuli tyyntyy | avtar vinden helt | the wind becomes calm |
| tuuli heikkenee | avtar vinden | the wind weakens to |
| tuuli voimistuu | tilltar vinden | the wind strengthens to |
| edelleen | fortfarande | continued |
| heikkenevää | avtagande | weakening |
| voimistuvaa | tilltagande | strengthening |
| suunnaltaan vaihtelevaa | varierande | variable |
| pohjoistuulta ... | nordlig vind ... | northern wind ... |
| pohjoistuuleksi ... | nordlig vind ... | northerly wind ... |
| pohjoisen puoleista tuulta ... | vind omkring nord ... | approximately northerly wind ... |
| pohjoisen puoleiseksi tuuleksi ... | vind omkring nord ... | approximately northerly wind ... |
