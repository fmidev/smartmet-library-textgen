# Analysis functions

1. [Introduction](#funktio_intro)
1. [Mean](#mean)
1. [Maximum](#maximum)
1. [Minimum](#minimum)
1. [Sum](#sum)
1. [Count](#count)
1. [Percentage](#percentage)
1. [Trend](#trend)

<a id="funktio_intro"></a>
## Introduction

The `TextGen::WeatherFunction` enumeration, defined in the `WeatherFunction.h` file, lists the functions recognised by the weather analyser.

<a id="mean"></a>
## "Mean"

<a id="maximum"></a>
## "Maximum"

<a id="minimum"></a>
## "Minimum"

<a id="sum"></a>
## "Sum"

<a id="count"></a>
## "Count"

<a id="percentage"></a>
## "Percentage"

<a id="trend"></a>
## "Trend"

The `Trend` function is intended to be used only with respect to time; the analyser does not check this at runtime.

The function's value is the difference between the last and first value in the time series. The function's quality — which here is an especially essential part of the result — is the percentage of the time series in which the value changes in the same direction as the final difference. Under this definition, the quality of a monotonically increasing or decreasing time series is always 1.

If the time series rises or falls for most of the time, the quality can be assumed to be clearly over 0.5, which allows us to speak of a trend in the time series.
