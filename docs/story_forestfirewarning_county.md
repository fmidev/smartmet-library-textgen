# Story "forestfirewarning_county"

The `forestfirewarning_county` story takes a different form:

1. "Metsäpalovaroitus [on/ei ole] voimassa."

In Swedish:

1. "Varning för skogsbrand [är/är inte] i kraft."

In English:

1. "Forest fire warning [is/is not] in effect."

The warning is area-specific. For example, if the area has the code name "imatra" — defined as a polygon or a point — the code number of the relevant warning area is read from the variable:

```
qdtext::forestfirewarning::areacodes::imatra = 9
```

The code numbers currently in use are:

```
1. Regions:
1 Uusimaa
2 Varsinais-Suomi
3 Itä-Uusimaa
4 Satakunta
5 Kanta-Häme
6 Pirkanmaa
7 Päijät-Häme
8 Kymenlaakso
9 Etelä-Karjala
10 Etelä-Savo
11 Pohjois-Savo
12 Pohjois-Karjala
13 Keski-Suomi
14 Etelä-Pohjanmaa
15 Pohjanmaa
16 Keski-Pohjanmaa
17 Pohjois-Pohjanmaa
18 Kainuu
19 Lappi
20 Ahvenanmaa
1. Municipalities of Pohjois-Pohjanmaa:
21 Eteläinen Pohjois-Pohjanmaa
22 Pudasjärvi
23 Taivalkoski
24 Kuusamo
1. Municipalities of Lappi:
25 Kemi
26 Tornio
27 Keminmaa
28 Tervola
29 Simo
30 Ranua
31 Posio
32 Salla
33 Kemijärvi
34 Rovaniemen mlk
35 Rovaniemi
36 Ylitornio
37 Pello
38 Kolari
39 Muonio
40 Kittilä
41 Sodankylä
42 Pelkosenniemi
43 Savukoski
44 Inari
45 Utsjoki
46 Enontekiö
```

Forest fire warnings are read from the directory:

```
qdtext::forestfirewarning::directory
```

A suitable current value is:

```
/data/pal/txt/varo/palo/
```

The directory is expected to contain a file named:

```
YYYYMMDD.palot_koodina
```

where `YYYYMMDD` is either the current day's timestamp, or — if the file is not found — the previous day's timestamp. This allows the text to function correctly around the day change.

The expected file structure looks like this:

```
200505240921
20 2
2 0
4 2
1 0
3 0
5 0
7 0
8 0
9 0
6 0
13 0
10 0
11 0
12 0
14 2
15 2
16 0
17 0
18 0
19 0
21 0
22 0
23 0
24 0
25 0
26 0
27 0
28 0
29 0
30 0
31 0
32 0
33 0
34 0
35 0
36 0
37 0
38 0
39 0
40 0
41 0
42 0
43 0
44 0
45 0
46 0
```

After the first line, all warning areas are listed together with the active forest fire warning level. The levels are:

* 0 = no warnings in effect
* 1 = risk of grass fires is high
* 2 = forest fire warning is in effect / will be issued today at 12:00

The phrase "metsäpalovaroitus on voimassa" is chosen only and exclusively for code 2.

Phrases required by the story:

* "metsäpaloindeksi on voimassa"
* "metsäpaloindeksi ei ole voimassa"
