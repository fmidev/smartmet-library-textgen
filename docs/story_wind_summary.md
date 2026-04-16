# Story "wind_summary"

## Story form

The story describes a wind forecast for a period of three or more days that is not at the very beginning of the overall forecast. Typically the period starts on the third or fourth forecast day. The story is best suited to sea areas.

Possible base sentences:

1. Enimmäkseen [kohtalaista lounaistuulta].
1. Enimmäkseen [kohtalaista], ajoittain [navakkaa lounaistuulta]

For possible strong wind, an additional sentence is added as follows:

1. Mahdollisesti [kovaa tuulta] [maanantaina]
1. Mahdollisesti [kovaa tuulta] [maanantaina] ja [tiistaina]  (2 consecutive days)
1. Mahdollisesti [kovaa tuulta] jakson alkupuolella.
1. Mahdollisesti [kovaa tuulta] jakson keskivaiheilla.
1. Mahdollisesti [kovaa tuulta] jakson loppupuolella.
1. Mahdollisesti [kovaa tuulta]

Here [kovaa tuulta] expands to one of the alternative forms:

1. "kovaa tuulta" (strong wind)
1. "kovaa tuulta tai myrskyä" (strong wind or storm)
1. "myrskyä" (storm)

## Algorithms

1. Compute the percentage of each wind class. In the initial stage, strong wind and storm are handled as a single class.
1. Find the most common class X.
1. Find the largest class Y that occurs at least the desired minimum amount (`minimum_percentage`).
1. If Y <= X, table 1 is used; otherwise table 2.

| Table 1 — X | Forecast |
| --- | --- |
| tyyni | Enimmäkseen tyyntä |
| kohtalainen | Enimmäkseen [kohtalaista lounaistuulta] |
| navakka | Enimmäkseen [navakkaa lounaistuulta] |
| kova/myrsky | [Mahdollisesti kovaa tuulta] |

| Table 2 — X | Y | Forecast |
| --- | --- | --- |
| tyyni | kohtalainen | Enimmäkseen tyyntä, ajoittain [kohtalaista tuulta] |
|  | navakka | Enimmäkseen tyyntä, ajoittain [navakkaa tuulta] |
|  | kova/myrsky | Enimmäkseen tyyntä. [Mahdollisesti kovaa tuulta] |
| kohtalainen | navakka | Enimmäkseen kohtalaista, ajoittain navakkaa [lounaistuulta] |
|  | kova/myrsky | Enimmäkseen [kohtalaista lounaistuulta]. [Mahdollisesti kovaa tuulta] |
| navakka | kova/myrsky | Enimmäkseen [navakkaa lounaistuulta]. [Mahdollisesti kovaa tuulta] |

For possible strong wind, an additional sentence is produced as follows:

1. If strong wind is forecast for only one day:
** "Mahdollisesti [kovaa tuulta] [maanantaina]"
1. If strong wind is forecast for only two consecutive days:
** "Mahdollisesti [kovaa tuulta] [maanantaina] ja [tiistaina]"
1. If strong wind is forecast for only the beginning of the period:
** "Mahdollisesti [kovaa tuulta] jakson alkupuolella"
1. If strong wind is forecast for only the end of the period:
** "Mahdollisesti [kovaa tuulta] jakson loppupuolella."
1. If strong wind is forecast for only the middle of the period:
** "Mahdollisesti [kovaa tuulta] jakson keskivaiheilla."
1. Otherwise:
** Mahdollisesti [kovaa tuulta]

Here the phrase "kovaa tuulta" is expanded as follows:

1. If the maximum wind is over `storm_limit`, "myrskyä"
1. If the maximum wind is over `storm_maybe_limit`, "kovaa tuulta tai myrskyä"
1. Otherwise, "kovaa tuulta"

Variables used:

```
textgen::[section]::story::wind_summary::minimum_percentage = [0-X] (=5)
textgen::[section]::story::wind_summary::storm_limit       = [0-X] (=21)
textgen::[section]::story::wind_summary::storm_maybe_limit = [0-X] (=19)
```

## Wind direction

The wind direction for each time interval is obtained by computing the mean direction over time and space. Based on the direction's quality index, the direction's accuracy is first weakened to "N-puoleista tuulta" (approximately N) and then to "suunnaltaan vaihtelevaa tuulta" (variable).

The required quality (direction's standard deviation) is controlled by:

```
textgen::[section]::story::wind_summary::direction::accurate (= 22.5)
textgen::[section]::story::wind_summary::direction::variable (= 45)
```

Defaults are given in parentheses.

The direction phrase is chosen as follows:

1. If quality >= `accurate`, "N-tuulta"
1. If quality >= `variable`, "N-puoleista tuulta"
1. Otherwise the direction is not mentioned

Note in particular that overview-type forecasts do not mention variable-direction wind.

## Phrases required

Phrases required:

* "mahdollisesti"
* "enimmäkseen"
* "ajoittain"
* "ja"
* "tyyntä"
* "heikkoa"
* "kohtalaista"
* "kovaa"
* "myrskyä"
* "tuulta"
* "jakson alkupuolella"
* "jakson loppupuolella"
* "jakson keskivaiheilla"
* "N-tuulta"
* "N-na"
