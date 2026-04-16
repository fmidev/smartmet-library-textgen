# Story "weather_forecast" (and "weather_forecast_at_sea")

> **Status:** Primary. The composed multi-part weather narrative.
> `weather_forecast_at_sea` is the marine variant — added in spring 2017
> — and additionally reports visibility.
>
> **Owner:** `WeatherStory::forecast()` / `WeatherStory::forecast_at_sea()`.
> **Implementation:** `textgen/weather_forecast.cpp` (~1 528 LOC).
> **Scaffolding:** `WeatherForecastStory`, plus
> `CloudinessForecast`, `PrecipitationForecast`, `FogForecast`,
> `ThunderForecast`.
>
> This page is a translation of the authoritative Finnish specification
> (Säätarina). The 2005 Doxygen material for `weather_overview` does
> **not** describe this story.

## What it produces

A generic weather paragraph good for essentially any product. Example:

> Paikoin heikkoa sadetta, paikoin myös ukkosta. Illalla sää on verrattain
> pilvinen ja poutainen. Yöllä yksittäiset sadekuurot ovat mahdollisia.
> Sää viilenee huomattavasti. Päivän ylin lämpötila on 14…19 astetta.
> Yön alin lämpötila on 9…12 astetta. Tuuli saa sään tuntumaan
> viileämmältä.

The marine variant (`weather_forecast_at_sea`) additionally reports
visibility when a rain, thunder, fog, or snow phenomenon degrades it.
If visibility is not degraded, it simply says **"Hyvä näkyvyys"**.

## Reported phenomena

Four (five for the marine variant) kinds of weather phenomena are
considered, in priority order:

| # | Phenomenon | When reported |
| - | --- | --- |
| 1 | **Precipitation** | Start, intensity, form, area coverage, possible form change, end |
| 2 | **Thunder** | Only together with precipitation |
| 3 | **Fog** | Only when it is not raining |
| 4 | **Cloudiness** | Only when it is fair; reports total cloudiness or its trend |
| 5 | **Visibility** | Marine variant only; see [Visibility](#visibility) |

## Data model

For each hour of the forecast period the story reads the area's values
for:

* **Precipitation**
  * intensity (mm/h), e.g. 0.835 mm/h
  * precipitation-area coverage — percentage of grid points over the
    intensity threshold, e.g. 85 %
  * forms — percentage split of water / drizzle / sleet / snow /
    freezing, e.g. 90 % water, 2 % drizzle, 8 % freezing
  * type — shower vs continuous
  * geographic distribution of the precipitation area
* **Cloudiness**
  * total cloudiness over the area, e.g. 35–85 %
  * geographic distribution
* **Thunder**
  * thunder probability over the area, e.g. 5.5 %
  * geographic distribution of the thunder area
* **Fog**
  * fog-area coverage (normal and dense tracked separately), e.g.
    35 % normal + 5 % dense
  * geographic distribution

## Decision order

The pipeline runs in five steps:

1. Compute the hourly time series for precipitation, cloudiness,
   thunder, and fog (using the
   [calculation rules](#calculation-rules) below).
2. From the precipitation time series find **rain periods** and
   **no-rain periods**.
3. Resolve special cases around the period boundaries (e.g. merge a
   short no-rain gap into its neighbours).
4. Decide which of the four phenomena is dominant in the forecast
   period and emit it first.
5. Report the remaining changes in chronological order.

More detail on each step:

1. Time series for precipitation, cloudiness, thunder, fog start 8
   hours before the forecast period and end 8 hours after it, so that
   changes at the period's edges can still be recognised reliably.

2. A **rain period** is an interval in which every hour's intensity
   exceeds the "weak rain" lower bound. The intensity measure is
   `Max(Sum(intensity), Mean(intensity))`. The weak-rain lower bound
   depends on the form (see
   [Precipitation intensity table](#precipitation-intensity-table)).
   A **no-rain period** is simply a span in which it is not raining.

3. Boundary adjustments applied to rain/no-rain periods:
   * A no-rain period of at most 1 hour inside the forecast period is
     not reported.
   * A short (≤ 1 h) no-rain period at the **start** of the forecast
     period is dropped — the story opens with the direct rain event.
     Example: if at 06:00 fair was forecast and at 07:00 rain starts,
     the story opens with the rain, not "from 06:00 fair".
   * A short (≤ 1 h) no-rain period at the **end** of the forecast,
     preceded by either a long or a weak rain, is not reported.
     Example: if rain begins at 20:00 and at 05:00 the next morning it
     clears for an hour, but at 06:00 rain returns, the story reads
     "Illasta alkaen sadetta verrattain pilvinen ja poutainen.
     Aamuyöllä paikoin heikkoa lumisadetta" — no "clears" phrase in
     between.
4. Two rain periods merge into one when
   * the no-rain gap between them is at most three hours, and
   * the total length of the surrounding rain periods is at least as
     long as the gap. Example: two 1-hour rains with 2 hours of fair
     between them are **not** merged; two rain periods with a 1-hour
     gap **are** merged.
5. One rain period splits into two when the **type** changes from
   shower to continuous (or vice versa). Exception: if both periods
   contain thunder, they stay as one.
6. The first phenomenon dominant in the forecast period is named, then
   the remaining ones are listed in chronological order. If no change
   happens inside the forecast, the sentence is short, e.g.
   "Sää on verrattain pilvinen ja poutainen."

Possible changes are: rain begins, rain ends, cloudiness increases,
cloudiness decreases, short rain spell. When rain ends, the story also
reports cloudiness after the rain, and any fog.

## Time concepts

Reports use the standard Finnish time-of-day boundaries
(see [time_concepts.md reference](../../timeperiods.md)):

| Phrase | Hour range (local) |
| --- | --- |
| keskiyöllä | 00:00 – 03:00 |
| aamuyöllä | 02:00 – 06:00 |
| aamulla | 06:00 – 09:00 |
| aamupäivällä | 09:00 – 12:00 |
| päivällä | 10:00 – 18:00 |
| iltapäivällä | 12:00 – 18:00 |
| illalla | 17:00 – 22:00 |
| iltayöstä | 21:00 – 24:00 |
| yöllä | 22:00 – 06:00 |

When reporting longer rain or fog the story may combine two adjacent
phrases: "aamulla ja aamupäivällä" (06–12), "iltapäivällä ja illalla"
(12–22), "illalla ja iltayöstä" (17–24), "iltayöstä ja keskiyöllä"
(21–03), "keskiyöllä ja aamuyöllä" (00–06), "aamuyöllä ja aamulla"
(02–09).

For periods that start at a boundary, the story uses "alkaen" forms:

| Phrase | Hour range |
| --- | --- |
| aamusta alkaen | 06:00 – 09:00 |
| aamupäivästä alkaen | 09:00 – 12:00 |
| iltapäivästä alkaen | 12:00 – 18:00 |
| illasta alkaen | 17:00 – 22:00 |
| iltayöstä alkaen | 21:00 – 24:00 |
| keskiyöstä alkaen | 00:00 – 03:00 |
| aamuyöstä alkaen | 02:00 – 06:00 |

## Calculation rules

For each of the tracked quantities the table lists the hour-by-hour
input and the reduction operator that produces the period value. Here
`TIME1` is the first forecast hour, `TIME2` is the second, etc., and
`SUM` is the period-wide aggregate.

| Quantity | TIME1 (example) | TIMEn | Aggregate | Notes |
| --- | --- | --- | --- | --- |
| Sade (%) | S1 = percentage of grid points with intensity > 0.04 mm/h (water) or > 0.02 mm/h (drizzle / sleet / snow) | Sn | `Mean(S1…Sn)` | Coverage classes: 0 % < S < 10 % → enimmäkseen poutainen; 10 % < S < 50 % → paikoin; 50 % < S < 90 % → monin paikoin |
| Pouta (%) | P1 = 100 − S1 | Pn | `Mean` | If every Pi = 100, no rain inspection is performed |
| Laaja (%) | J1 = 100 − K1 | Jn | `Mean` | Percentage of rain area whose rain is non-shower |
| Kuuro (%) | K1 = shower share of rain | Kn | `Mean` | |
| Tyyppi (shower/continuous) | T1 = `Max(K1,J1)` | Tn = `Max(Kn,Jn)` | `Max` | The dominant type in each hour wins; the period type is whichever is dominant across hours |
| Vesi (%) | V1 = share of water among forms | Vn | Area: `Percentage`; time: `Mean` | |
| Tihku (%) | T1 = share of drizzle | Tn | Area: `Percentage`; time: `Mean` | |
| Räntä (%) | R1 = share of sleet | Rn | Area: `Percentage`; time: `Mean` | |
| Lumi (%) | L1 = share of snow | Ln | Area: `Percentage`; time: `Mean` | |
| Jäätävä (%) | JÄÄ1 = share of freezing | JÄÄn | Area: `Percentage`; time: `Mean` | Mentioned if share is at least 10 %. Applies only to continuous rain; not mentioned for showers. Warrants a warning phrase. |
| Olomuoto | All JÄÄ1 | All JÄÄn | — | At most two most-common forms are reported, plus freezing if present. A form must reach at least 10 % to be mentioned. |
| Intensiteetti (mm/h) | I1 — area: `Maximum` + `Mean`; time: `Sum` | In | Imax = `Mean(I1max, I2max, …)`, Imean = `Mean(I1mean, I2mean, …)`, I = `Mean(Imax, Imean)` | Classes: 0.04 mm/h < I < 0.4 mm/h → heikkoa; I > 2.0 mm/h → runsasta; I > 1.5 mm/h → runsasta (lumi). If many forms or form changes, no intensity is emitted. |
| Ukkosen tn (%) | U1 — area: `Maximum`; time: `Maximum` | Un | `Maximum` | Mentioned only with precipitation |
| Kokonaispilvisyys (%) | N1 — area: `Mean`; time: `Mean` | Nn | min = `Minimum(N1…Nn)`, max = `Maximum(N1…Nn)`, mean = `Mean(N1…Nn)` | First compute hourly area means. For min/max over time: if the range spans partly-cloudy class, use "vaihtelevaa pilvisyyttä"; otherwise use the mean. |

## Change detection rules

Once the per-hour time series exist, the story looks for qualifying
transitions:

| Change | Precondition at hour 1 | Precondition at subsequent hours | Notes |
| --- | --- | --- | --- |
| Pilvistyminen (becoming cloudy) | N1 = first hour's total cloudiness | For every later hour, mean cloudiness ≥ 65 % | Also looks at the 8 hours before the forecast starts so that a trend that spans the boundary is recognised |
| Selkeneminen (clearing) | N1 | For every later hour, mean cloudiness ≤ 20 % | Mirror of pilvistyminen |
| Sateen alkaminen (rain begins) | S1 | Rain begins once the intensity minimum is exceeded for at least 6 consecutive hours | Intensity threshold depends on form (see intensity table) |
| Poutaantuminen (rain ends) | S1 | Weather becomes fair once the intensity drops under the minimum for at least 6 hours | "Poutaantuminen" is **not** used if the preceding rain was short (< 6 h) or had only minor coverage (< 10 %); in those cases the story just names the rain's time range (e.g. "aamupäivällä lumisadetta") |
| Olomuodon muutos (form change) | OM1 | OMn | Required: at start, at least 90 % is some form A; at end, at least 90 % is some form B. Additionally, the Pearson correlation in the time series must be ≤ −0.65 for the decreasing form and ≥ +0.65 for the rising form |
| Sadetyypin muutos (type change) | TY1 | TYn | Walk the hourly series and compare each type with that of the first hour. If the type changes and then stays the same until the period ends, conclude the type has changed. |

## Inference rules

Stated in Finnish in the spec, English here, rewritten in phrase form:

**1. "Yleisesti"** — a weather kind is *general* if it covers more than
Y % of the area. Y is large enough that no other kind can be "in many
places". The only two "kinds" are **precipitation** and **fair**.

Suppose the precipitation area covers Y = 90 % (or 80 %):

1. Aamupäivällä sadetta / vesisadetta / lumisadetta / räntää
2. …vesi- ja räntäsadetta
3. …vesi- ja lumisadetta
4. …tihku- ja lumisadetta
5. Aamupäivällä yleisesti sadetta / vesisadetta / lumisadetta / räntää

Here the precipitation area covers about 90 % of the grid-point area
and the type is continuous in the majority of points.

1. Aamupäivällä sade- / vesi- / lumi- / räntäkuuroja
2. Aamupäivällä yleisesti sade- / vesi- / lumi- / räntäkuuroja

Here the precipitation area covers 90 % and the type is shower in
the majority of points. The form is mentioned only in option 2.

For forms:

* snow + water or sleet = snow + water
* snow + water + freezing = snow + water that may also be freezing
* or the two most common forms (of max three) — freezing is reported
  as a third if its share is sufficient
* during the actual summer, when only water form rains, the word
  "water" is not needed: *vesisade = sade*

**2. "Enimmäkseen poutaa"** — when the fair area covers 90–100 %:

When the precipitation type is continuous: …enimmäkseen poutaa.

When the type is shower:

1. …enimmäkseen on poutaa. Yksittäiset sadekuurot ovat kuitenkin
   mahdollisia.
2. Sää on melko selkeä, mutta edelleen paikalliset lumikuurot
   mahdollisia.
3. Sää on osaksi selkeä tai selkenevää ja enimmäkseen poutaista.
4. Sää on aurinkoinen ja enimmäkseen poutainen.

Note: if S % < 10 or P % ≥ 90, the rain would be freezing.

**3.** 100 % fair → only cloudiness is reported. The mean and deviation
are computed; if `mean − deviation` and `mean + deviation` stay within
one of Lasse Winberg's five cloudiness classes (see
[Cloudiness classes](#cloudiness-classes)), the corresponding phrase
is used. Otherwise a two-class pair phrase like "sää vaihtelee
puolipilvisestä pilviseen" is used.

**4. "Monin paikoin"** — a weather kind is "in many places" if it covers
more than M %. Only one kind can be "in many places", so M ≥ 50 %
(51 %; 60 % is even safer).

Thus with precipitation > 50 %, cloudiness is reported first:

1. Päivällä pilvisyys vaihtelee ja monin paikoin voi tulla
   sadekuuroja
2. …ja monin paikoin sadetta / sadekuuroja
3. …ja monin paikoin lumi- tai räntäsadetta
4. …ja kohtalainen sateen / sadekuurojen mahdollisuus

**5. "Paikoin"** — a kind is "in some places" if it covers more than
P %, where P < 50 %.

Thus with precipitation between 10 % and 50 %:

1. Päivällä pilvisyys vaihtelee ja paikoin voi tulla sadekuuroja
2. …ja paikoin sadetta / sadekuuroja
3. …ja pieni sateen / sadekuurojen mahdollisuus
4. …paikoin voi tulla sadekuuroja
5. …paikoin tulee vähäistä sadetta
6. …paikoin tihkuttaa vähän vettä
7. …ja paikoin vähäistä lumi- tai räntäsadetta

## Precipitation

### Forms

* water
* drizzle
* snow
* sleet
* freezing

Multiple forms may be active simultaneously. The story names the two
most common plus any possible freezing. A form must reach ≥ 10 % to be
mentioned.

**Sari Hartonen's note on sleet:**

* In a shower-type rain the sleet form is *not* named; replace with
  "vesi- tai lumikuuroja".
* In a continuous rain the sleet is named only if it exceeds the
  combined share of water + drizzle + snow. Otherwise use "vesi- tai
  lumisadetta".

### Form changes

If a form changes into another during the period, this is reported:
"Lumisade muuttuu aamulla vesisateeksi." The following transitions are
reported:

* vesisade → lumisade
* vesisade → tihkusade
* vesisade → räntäsade
* tihkusade → lumisade
* tihkusade → vesisade
* tihkusade → räntäsade
* räntäsade → lumisade
* räntäsade → tihkusade
* räntäsade → vesisade
* lumisade → vesisade
* lumisade → tihkusade
* lumisade → räntäsade

### Precipitation type and its changes

Rain is either shower-type or continuous.

**Type for a single hour.** Whichever type is dominant. Example: if
showers make 51 % at 13:00, the hour's type is shower.

**Type for the whole period.** Whichever type is dominant across hours.
Example: in a 6-hour rain, 4 hours continuous + 2 hours shower → the
period is continuous. A tie → continuous.

**Type change within the period.** If the type changes mid-period, the
period is split into two sub-periods reported separately.

A type may change only **once** during a period. Example: 5 h
continuous then 3 h shower → report two periods. If the type changes
more than once, the period is not split; we just pick the more common
type (see previous paragraph).

### Precipitation intensity

The intensity determines whether the weather is fair or rainy and
describes the rain in words (heikko / runsas). Computation rule: the
average of the maximum intensity and the mean intensity. When reporting
a rain period we also mention the **maximum** so that a short burst can
be called out even when the period average is weak — e.g.
"Iltapäivästä alkaen sadetta voi olla runsasta" or "Iltayöhön asti
kuurot voivat olla voimakkaita". For this phrase to apply the strong
rain must last at least 2 hours.

#### Precipitation intensity table

| Form | mm/h | Phrase | Notes |
| --- | --- | --- | --- |
| vesi | 0.04 – 0.4 | heikko vesisade / sadekuuro | |
| vesi | 0.4 – 2.0 | kohtalainen vesisade / vesikuuro | |
| vesi | > 2.0 | runsas vesisade / sadekuuro | |
| tihku | ≥ 0.02 | tihkusade | ei kuuroa |
| räntä | 0.02 – 0.4 | heikko räntäsade / räntäkuuro | |
| räntä | 0.4 – 1.7 | kohtalainen räntäsade / räntäkuuro | |
| räntä | > 1.7 | runsas räntäsade / räntäkuuro | |
| lumi | 0.02 – 0.4 | heikko lumisade / lumikuuro | |
| lumi | 0.4 – 1.5 | kohtalainen lumisade / lumikuuro | |
| lumi | > 1.5 | runsas lumisade / runsas lumikuuro | |
| jäätävä | ≥ 0.02 | jäätävä sade | ei kuuroa |

### Definition of fair (pouta)

Fair weather is when the intensity minimum is not exceeded: under
0.04 mm/h (water) or under 0.02 mm/h (drizzle / snow / sleet).

### Rain starting and ending

**Rain starts** when the intensity minimum is exceeded
(see [Precipitation intensity table](#precipitation-intensity-table)).
Rain onset is reported when the rain lasts at least six hours.

**Rain ends** when, after at least six hours of rain, the intensity
drops below the minimum.

### Rain area

**Bounding box.** The rain area is the smallest rectangle that contains
every grid point of the area predicted to receive at least weak rain.
Additionally a rectangle covering the whole forecast area is defined.
Now the rain area's location within the whole area can be examined.
(Kaisa's note: this is done as a rain/fair question. We don't split the
area by form or type. Instead, inside a sub-area we check type, form,
thunder etc. separately.)

The rain area's location can be examined at one or several hours. When
looking at consecutive hours the movement of the rain area can be
inferred. The illustration below shows the rain area (blue) in the
eastern half of the forecast area (brown). Bounding boxes and the four
sub-areas (NE, SE, SW, NW) are drawn.

> *(A diagram appears here in the PDF; not reproduced.)*

**Area shape vs. rain area.** The area to be reported may be shaped
(curved, narrow, flat) such that it does not fit inside the bounding
box "representatively". Either it doesn't reach all four quadrants, or
it is so narrow or flat that a cardinal-direction-based division is not
useful. The following conditions must hold for the compass-based
division to be used:

* The area spans all four quadrants of the bounding box.
* The area's maximum width is at least half the bounding box's maximum
  width.
* The area's maximum length is at least half the bounding box's
  maximum length.

Additionally, if the area contains both coast and inland, the
compass-based division is not used. The coast/inland division is used
instead.

**Rain area position phrases.** The rain area's position is reported
only if it can be localised to a sub-area. The forecast area is split
into four equal sectors — NE, SE, SW, NW. If the rain area reaches two
adjacent sectors they merge — NE + SE = east. If the rain area covers
three out of four sub-areas, the story reports the *fair* area instead.
For example, if rain is in NE, SE, and SW, the phrase is "sadetta
paitsi alueen lounaisosissa poutaa".

| Rain-area position | Phrase |
| --- | --- |
| NE | alueen koillisosassa |
| SE | alueen kaakkoisosassa |
| SW | alueen lounaisosassa |
| NW | alueen luoteisosassa |
| NE, SE | alueen itäosissa |
| SW, NW | alueen länsiosissa |
| NE, SE, SW | * * sadetta, paitsi alueen luoteisosissa poutaa ? |
| NW, SW, NE | * * sadetta, paitsi alueen kaakkoisosissa poutaa ? |
| SW, NW, SE | * * sadetta, paitsi alueen koillisosissa poutaa ? |
| NE, SE, NW | * * sadetta, paitsi alueen lounaisosissa poutaa ? |

**Rain-area movements**:

* Rain area arriving from a given compass direction:
  "sadealue saapuu lännestä", "sadetta lännestä alkaen".
* Rain area crossing the area:
  "sadealue kulkee alueen yli aamupäivän aikana".
* Rain area migrating within the area:
  "sadealue siirtyy iltapäivän aikana lännestä itään".

Current implementation:

* **Arrival.** Look at the rain period's first hour and the hour
  before it. If over 80 % of the rain points lie in one of the
  sub-areas (NE, SE, SW, NW, N, S, E, W), we say the rain area is
  arriving from that direction.
* **End.** Look at the rain period's last hour + the following hour.
  If 80 % of the rain points lie in one sub-area, we say the weather
  clears from the opposite direction.
  Example: if 80 % of rain points were in the east, we say the
  weather clears from the west.

Kaisa's note: rain-area movement needs further analysis:

1. First examine the forecast period (e.g. 24 h) for rain periods and
   fair periods.
2. Then analyse the sub-areas.
3. If a rain area is stationary, examine rain phenomena within the
   sub-area, and finally fair phenomena afterwards. Should rain areas
   be merged?
4. If the rain area moves, analyse rain phenomena over the **whole**
   area.

### Reporting rain

#### Long-duration vs. short-duration rain

For long-duration rain, report start and end as a rule:

> "Aamupäivästä alkaen monin paikoin lumisadetta. Illalla sää poutaantuu."

If a long rain starts **and** ends within a specific time-of-day window
(see [Time concepts](#time-concepts)), do not report start/end; just
name the rain's time window:

> "Yöllä lumisadetta" or "Aamuyöllä ja aamulla sadetta"

For short-duration rain, report the time window. Example: if the rain
period is 13:00 – 18:00:

> "Iltapäivällä monin paikoin lumikuuroja."

#### Weak or sparse rain

If the rain is weak (see
[Precipitation intensity table](#precipitation-intensity-table)) or the
coverage averages below 10 %, the "poutaantuminen" word is not used.

#### Multiple forms

A rain phrase may name two forms, e.g. "vesi- tai räntäsadetta". Each
form must cover at least 10 %; otherwise only the more general form is
named.

#### Coast and inland distinction

Coast and inland are reported separately if:

* it is raining in one and not the other, or
* the rain intensity differs clearly (heikko 0.04…0.4 vs. runsas > 2.0),
  or
* the form differs (vesi vs. räntä).

### Rain phrases

* heikkoja sadekuuroja
* voimakkaita sadekuuroja
* sadekuuroja
* sadekuuroja, jotka voivat olla jäätäviä
* lumikuuroja
* räntäkuuroja
* vesi- tai räntäkuuroja
* heikkoa sadetta
* heikkoa sadetta, joka voi olla jäätävää
* runsasta sadetta
* runsasta sadetta, joka voi olla jäätävää
* räntäsadetta
* vesisadetta
* vesisadetta, joka voi olla jäätävää
* lumisadetta
* tihkusadetta
* tihkusadetta, joka voi olla jäätävää
* heikkoja lumikuuroja
* sakeita lumikuuroja
* heikkoa lumisadetta
* runsasta lumisadetta
* vesi- tai lumisadetta
* lumi- tai vesisadetta, joka voi olla jäätävää (refers specifically to water)
* vesi- tai räntäsadetta
* räntä- tai vesisadetta, joka voi olla jäätävää (refers specifically to water)
* lumi- tai räntäsadetta
* sää on enimmäkseen poutaista
* sää on enimmäkseen poutaista, yksittäiset sadekuurot ovat kuitenkin mahdollisia
* sää on enimmäkseen poutaista, yksittäiset räntäkuurot ovat kuitenkin mahdollisia
* sää on enimmäkseen poutaista, yksittäiset vesi- tai vesikuurot ovat kuitenkin mahdollisia
* sää on enimmäkseen poutaista, yksittäiset vesi- tai lumikuurot ovat kuitenkin mahdollisia
* sää on enimmäkseen poutaista, yksittäiset lumi- tai räntäkuurot ovat kuitenkin mahdollisia

Changes: "sade alkaa", "poutaantuu". The phrase "sää jatkuu sateisena"
is **not** used.

## Fog

### Reporting fog

Fog is reported with its time window and a possible "sakea sumu"
qualifier.

Coast and inland are reported separately if their fog coverages differ
by at least 50 percentage points. Example: if the coast's fog coverage
is 85 % and the inland's 30 %, the story reads "rannikolla monin paikoin
sumua, sisämaassa paikoin sumua".

### Fog extent

Area-wide fog coverage is the time-series mean of hourly coverages.
Additional rules:

* A 1-hour fog period at the start or end of the forecast is ignored.
* When several fog periods fall inside the forecast:
  * If the longest period covers 70 % of the forecast, use its mean
    coverage directly.
  * Otherwise, walk through the fog periods and take their mean.

### Fog phrases

The table lists normal (moderate) fog coverages and their phrases. The
"joka voi olla sakeaa" addition is tacked on when dense-fog coverage
reaches at least 20 % of the area.

| Fog coverage | Phrase |
| --- | --- |
| fog < 20 | — |
| 20 ≤ fog ≤ 50 | paikoin sumua (, joka voi olla sakeaa) |
| 50 < fog < 90 | monin paikoin sumua (, joka voi olla sakeaa) |
| fog ≥ 90 | sumua (, joka voi olla sakeaa) |

## Visibility (marine variant only)

The `weather_forecast_at_sea` variant was added in spring 2017 and
reports visibility. Weather_forecast_at_sea names visibility-degrading
phenomena (rain, thunder, fog) and how they affect visibility
(poor visibility, mostly-good visibility, otherwise good visibility).
If no visibility-degrading phenomenon exists, the story says
**"Hyvä näkyvyys"**.

The marine `SÄÄosio` is modified so that the focus is on visibility and
on the rain/fog events that degrade it. The same operational logic as
in the land forecast is used, plus the end sentence reports cloudiness.

### Visibility operational logic

1. Compute time series for precipitation, cloudiness, thunder
   probability, and fog.
2. Find rain and no-rain periods (see the
   [Calculation rules](#calculation-rules) and
   [intensity table](#precipitation-intensity-table)).
3. Resolve boundary cases and merge rain periods (same as land
   forecast).
4. Report the dominant condition in the forecast period. The rain
   type is whichever is more common.
5. Weak water rain is merged into the fair period, but a fog in that
   period is still reported. Freezing is warned about.

### How visibility is reported

1. If there are no change points (= no rain) and no fog, the sentence
   is **"Hyvä näkyvyys"**.
2. "Huono näkyvyys" is used for extensive and long-lasting snowfall
   (definition below).
3.
   a. If over half of the forecast (rain share ≤ 0.5 × forecast length)
      has precipitation such that it reaches reasonably the
      "monin_paikoin_alaraja" (50 %), the sentence is "enimmäkseen hyvä
      näkyvyys".
   b. If rain coverage ≤ 10 %, no sentence is emitted even if the form
      would be snow.
   c. Otherwise (i.e. rain is extensive and/or long-lasting), report
      the phenomena and append:
      * If snow is present, report the phenomena and add the phrase
        "muuten hyvä näkyvyys" after the comma.
      * If other rain phenomena, report them and continue
        "muuten hyvä näkyvyys".
4. If the period has fog (with or without rain), do not add a
   visibility phrase.
5. Time phrases in visibility forecasts:
   a. If rain ≤ 6 h (no! continuous rain), use the same time phrases as
      the land forecast. Remember fog too.
   b. If rain period ≥ 0.7 × forecast length, report only the rain
      event.
   c. If the rain length is between the above points (6 h < rain <
      0.7 × forecast length), use coarser time phrases:

      > Yöllä 21 – 06, Aamulla 05 – 10, Päivällä 10 – 18, Illalla 17 –
      > 22, and Yöllä ja aamulla, Aamulla ja päivällä, Päivällä ja
      > illalla

### Poor-visibility definition

"Huono näkyvyys" is chosen when:

a. precipitation form = snow / sleet / freezing, type = shower or
   continuous (i.e. all), intensity > heikko or area maximum intensity
   is over at least 3 h > heikko, **OR**
b. (precipitation form = water / sleet / drizzle, type = shower,
   intensity ≥ runsas) — **removed**
c. **AND** precipitation coverage ≥ monin_paikoin_alaraja (50 %), **AND**
d. **AND** rain duration > 50 % of the period length (or another
   existing long-duration measure).

### Visibility phrases

* "Hyvä näkyvyys"
* "ja huono näkyvyys"
* "enimmäkseen hyvä näkyvyys"
* "muuten hyvä näkyvyys"

### Visibility configuration parameters

In the marine forecast the "monin paikoin" phrase is replaced by
"paikoin" — set `in_some_places_upper_limit` to 90 (instead of the
default 50); this causes the "paikoin" phrase to be used in the
10…90 % range.

For press products the `short_text_mode` parameter has been introduced
to trim generated text. If `short_text_mode = true`, visibility-
degrading phrases are not emitted, except **"Hyvä näkyvyys"**.

| Parameter | Meaning | Value (default) |
| --- | --- | --- |
| `in_some_places_upper_limit` | Upper limit for "paikoin" phrase use | 90 (50) |
| `short_text_mode` | If `true`, do not emit visibility-degrading phrases (except "Hyvä näkyvyys") | false (false) |

Other parameters are the same as in the land forecast — see
[Configuration parameters](#configuration-parameters) below.

## Cloudiness

### Reporting cloudiness

Cloudiness is reported either as a change or as a total cloudiness
class.

### Cloudiness classes

Total-cloudiness classification, following Lasse Winberg's scale
(YLE's limits in parentheses):

| Total cloudiness (%) (YLE's) | Word |
| --- | --- |
| 0 – 9.9 (19) | selkeää |
| 9.9 (19) – 35 (39) | melkein selkeää |
| 35 (39) – 65 (79) | puolipilvistä |
| 65 (79) – 85 (91) | verrattain pilvistä |
| 85 (91) – 110 (110) | pilvistä |

Additionally, the phrase **"sää vaihtelee puolipilvisestä pilviseen"**
is used when the minimum cloudiness falls in 35 – 65 and the maximum
in 85 – 110.

### Cloudiness change

Weather can become either cloudy or clear:

> "sää selkenee" — used when cloudiness is at least 65 % (verrattain
> pilvistä) at the start and at most 20 % at the end.

> "sää pilvistyy" — used when cloudiness is at most 20 % at the start
> and at least 65 % (verrattain pilvistä) at the end.

### Inland and coast distinction

Per Anssi Vähämäki, the area is split into coast and inland only
rarely. Temperature is the primary parameter driving the split.
Cloudiness is reported separately for coast and inland only when one is
fully cloudless and the other very cloudy.

Exact limits: fully cloudless < 5.0 %; very cloudy > 70 %.

### Cloudiness phrases

* selkeää
* melkein selkeää
* puolipilvistä
* verrattain pilvistä
* pilvistä
* sää vaihtelee puolipilvisestä pilviseen

## Thunder

### Thunder-area extent

Thunder-area extent is the maximum over the hourly time series. This
value is used to pick the thunder phrase (see table below).

### Thunder probability

Thunder probability is the maximum over the hourly time series. This
value picks the thunder phrase.

### Reporting thunder

Thunder is always reported as a supplementary addition (with
precipitation). The thunder probability is inspected, and the sentence
is formed using the thunder-area's extent and the probability maximum
as follows:

| Extent (%) × Probability (%) | 20 ≤ tn ≤ 30 | 30 ≤ tn ≤ 60 | 60 ≤ tn ≤ 100 |
| --- | --- | --- | --- |
| summer: 5 ≥ laajuus < 30 | paikoin voi myös ukkostaa | paikoin myös ukkostaa | todennäköisesti myös ukkostaa |
| winter: 10 ≥ laajuus < 30 | paikoin voi myös ukkostaa | paikoin myös ukkostaa | todennäköisesti myös ukkostaa |
| laajuus ≥ 30 | mahdollisesti myös ukkostaa | myös ukkostaa esiintyy | todennäköisesti myös ukkostaa |

## Configuration parameters

### Freezing-rain threshold

| Parameter | Default | Meaning |
| --- | --- | --- |
| `freezing_rain_limit` | 10 | Share of precipitation that must be freezing (%) to trigger a warning |

### Cloudiness-related parameters

| Parameter | Default upper bound (%) | Phrase |
| --- | --- | --- |
| `clear_sky_limit` | 9.9 | selkeää |
| `almost_clear_sky_limit` | 35 | melkein selkeää |
| `partly_cloudy_limit` | 65 | puolipilvistä |
| `mostly_cloudy_lower_limit` | 85 | verrattain pilvistä |

### Precipitation-intensity parameters

| Parameter | Default (mm/h) | Meaning |
| --- | --- | --- |
| `dry_weather_limit_water` | 0.04 | Threshold between fair and water rain |
| `dry_weather_limit_drizzle` | 0.02 | Threshold between fair and drizzle |
| `dry_weather_limit_sleet` | 0.02 | Threshold between fair and sleet |
| `dry_weather_limit_snow` | 0.02 | Threshold between fair and snow |
| `weak_precipitation_limit_water` | 0.4 | Threshold for "heikko" vs. "kohtalainen" water |
| `weak_precipitation_limit_sleet` | 0.4 | Threshold for sleet |
| `weak_precipitation_limit_snow` | 0.4 | Threshold for snow |
| `hard_precipitation_limit_water` | 2.0 | Threshold for "runsas" water |
| `hard_precipitation_limit_sleet` | 1.7 | Threshold for "runsas" sleet |
| `hard_precipitation_limit_snow` | 1.5 | Threshold for "runsas" snow |
| `rainstorm_limit` | 7.0 | Threshold for "rankka" rain |

### Precipitation-extent parameters

| Parameter | Default (%) | Phrase: bound |
| --- | --- | --- |
| `mostly_dry_weather_limit` | 10 | enimmäkseen poutaa: upper |
| `in_some_places_lower_limit` | 10 | paikoin: lower |
| `in_some_places_upper_limit` | 50 | paikoin: upper |
| `in_many_places_lower_limit` | 50 | monin paikoin: lower |
| `in_many_places_upper_limit` | 90 | monin paikoin: upper |

### Visibility-specific parameters (marine only)

| Parameter | Default | Meaning |
| --- | --- | --- |
| `in_some_places_upper_limit` | 50 (marine override: 90) | See [Visibility configuration parameters](#visibility-configuration-parameters) |
| `short_text_mode` | false | If `true`, suppress visibility-degrading phrases except "Hyvä näkyvyys" |

## Meteorologist feedback

The story has been refined over several rounds of meteorologist
feedback. Confirm that this feedback has been taken into account:

* Feedback round 1 (linked in the original spec)
* Feedback round 2 (linked in the original spec)

## Special considerations

### "Continues" phrases

The phrases "sää jatkuu epävakaisena", "sää jatkuu sateisena", and
"sää jatkuu pilvisenä" are not used. Jouko Korhonen (2016-04-10): in a
2-day forecast, "jatkuu" is always questionable — should the forecast
sometimes take a stance on the preceding time? Silly phrasings emerge
easily.

### Showers

Jouko Korhonen raised the question of how showers should be edited.
This must absolutely be agreed with the editors. Jouko's email:

> "When a shower has been edited more intensely on the map, the
> following might work:
> ~~if a shower has been edited to last > 6 h or over a wide area of
> 9 h, the phrase could be e.g. occasional water-, sleet-, or snow-
> shower.~~ In that case in a rain phrase a number indicating
> intensity would still be needed in connection with the shower. If
> drizzle and shower at the same time, we would always say —
> occasional drizzle or freezing drizzle. If a shower's rain exceeds
> freezing, then real thunder would generally be the only word if
> showers are otherwise predicted."

And more:

> "If a shower-type rain has been edited > 6 or 9 hours consecutively
> and over a wide area, the text would be: Enimmäkseen pilvistä ja
> ajoittain satunnaisesti (hieman) vettä/lunta. Ajoittain satunnaisesti
> (hieman) vettä/lunta, but also with breaks in the cloudiness. The
> word rakoilla can here be accepted even up to 50 % of the sky,
> provided the precipitation is forecast."

### Short-duration rain

Anssi Vähämäki: If between morning and evening rains there is e.g. a
span of **less than 3 hours of relatively clear weather**, this could
be commented on (by day possibly clear OR by day clear in some places
… by morning water- or sleet-showers, by day (locally, in some places…)
clear. In the evening there could be light water-showers..). Similarly,
in evening showers we would need to think about using the "rakoilla"
word again, when you actually have to get phrases. If in this example
the morning and evening rain span has a relatively clear spell of 3
hours OR MORE, this could be commented on (during the day clear OR by
morning water- or sleet-showers, by day (locally, in some places…)
clear. By evening light water-showers…).

## TODO (from 2010-12-10)

* Testing the effect of various parameter values (see
  [Configuration parameters](#configuration-parameters))
* Regression tests
* Pyry (blizzard)
* Possibility of wind
* Rain intensity + accumulation: when it rains long enough, snow can
  accumulate heavily, so the word "heikko" cannot be used. Example:
  Kymenlaakso 2010-12-10 07:00 – 2010-12-11 06:00:

  > Text: "Paikoin lumikuuroja. Yöllä sää on puolipilvinen ja poutainen"

  ```
  time                intensit. laajuus olomuoto   kuurojen osuus
  10.12.2010 07:00:00: 0.946   100.0   snow=100   showers= 88
  10.12.2010 08:00:00: 1.032   88.5    snow=100   showers= 88
  10.12.2010 09:00:00: 1.016   84.6    snow=100   showers= 88
  10.12.2010 10:00:00: 0.956   88.5    snow=100   showers= 88
  10.12.2010 11:00:00: 0.611   84.6    snow=100   showers= 88
  10.12.2010 12:00:00: 0.372   76.9    snow=100   showers= 86
  10.12.2010 13:00:00: 0.242   61.5    snow=100   showers= 82
  10.12.2010 14:00:00: 0.195   34.6    snow=100   showers= 70
  10.12.2010 15:00:00: 0.165   30.8    snow=100
  10.12.2010 16:00:00: 0.143   23.1    snow=100
  10.12.2010 17:00:00: 0.084   19.2    snow=100
  10.12.2010 18:00:00: 0.053   23.1    snow=100   showers= 17
  ...
  ```

  The strong snow onset at the start, and the extent, ought to come
  out in the text. The problem is the rain period is handled as one
  whole and the intensity and rain-area extent's big-part changes are
  not considered.

## See also

* [`weather_overview`](weather_overview.md) — the older flagship weather
  story, built on `rain_oneday` / `rain_twoday` case tables
* [`weather_shortoverview`](weather_shortoverview.md) — condensed
  multi-day variant
* [`cloudiness_overview`](../cloudiness/cloudiness_overview.md) —
  cloudiness-only story
* [`weather_thunderprobability`](weather_thunderprobability.md) —
  thunder-only story
