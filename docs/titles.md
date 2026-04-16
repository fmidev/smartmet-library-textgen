# Headers

1. [HeaderFactory namespace](#headerfactory)
   1. [header "none"](header_none.md)
   1. [header "until"](header_until.md)
   1. [header "clock_range"](header_clock_range.md)
   1. [header "from_until"](header_from_until.md)
   1. [header "several_days"](header_several_days.md)
   1. [header "morning"](header_morning.md)
   1. [header "forenoon"](header_forenoon.md)
   1. [header "afternoon"](header_afternoon.md)
   1. [header "evening"](header_evening.md)
   1. [header "report_time"](header_report_time.md)
   1. [header "report_area"](header_report_area.md)
   1. [header "report_location"](header_report_location.md)

<a id="headerfactory"></a>
## HeaderFactory namespace

The `TextGen::HeaderFactory` namespace contains a `create` function that can be used to create named headers. The function is given the area name, the weather period, and the header type. The return value is a `TextGen::Header` object.
