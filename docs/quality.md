# Quality

## Introduction

A quality-measuring number can be associated with each analyzed result. This page explains the intended purpose of the quality value, how it is computed for different parameters and functions, and how it can be tuned for different purposes.

## Purpose

The quality value has the following purposes:

1. Area-based forecasts can be automatically split into smaller parts if the computed results have poor quality.
1. Time-based forecasts can be automatically split into shorter intervals if the computed results have poor quality.

For example, Finland can be further split into Southern, Central, and Northern Finland. A multi-day period can in turn be split into individual days, and an individual day further into day and night, etc.

## Computing quality

Quality is defined mainly as an area-based quantity; integration over time is not considered to have any effect. Quality is measured directly as an error estimate, so the unit of quality depends on the quantity. Common to all quality values is that 0 means an exact result.

## Interpretation for different functions

Most analyses performed by the text generator are a combination of area and time integrations, with the latter performed first. All quality is considered to apply to the area-based integration, i.e. the error estimate of the point integration is always 0.

For the following functions the error estimate is always 0:

1. `Maximum`
1. `Minimum`
1. `Sum`
1. `StandardDeviation`
1. `Percentage`
1. `Count`
1. `Trend`
1. `Change`

Only `Mean` remains.

For the `Mean` function the error estimate is the area-based standard deviation.

Note, however, that when generating text, `Trend` is a good quality indicator for the temporal `Change` function.
