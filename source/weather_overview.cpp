// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::overview
 */
// ======================================================================

#include "WeatherStory.h"
#include "CloudinessStory.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "Sentence.h"
#include "TimeTools.h"

#include <vector>

using namespace WeatherAnalysis;
using namespace std;

namespace
{
  
  // ----------------------------------------------------------------------
  /*!
   * \brief A structure for storing one day rain dispatch information
   *
   * The phrases are time phrases to be passed onto the story generating
   * subroutines. Either one or both may be empty strings.
   */
  // ----------------------------------------------------------------------

  struct one_day_data
  {
	int index;
	const char * phrase1;
	const char * phrase2;
  };
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Second table from page \ref page_rain_oneday
   *
   * All 36 unique one day rains mapped onto 4 different reduced
   * cases with two time phrase parameters
   */
  // ----------------------------------------------------------------------

  one_day_data one_day_cases[37] =
	{
	  { 0, "", ""},
	  { 2, "aamulla", "aamup‰iv‰st‰ alkaen" },
	  { 2, "aamulla", "keskip‰iv‰st‰ alkaen" },
	  { 3, "aamup‰iv‰ll‰", "iltap‰iv‰st‰ alkaen" },
	  { 3, "aamup‰iv‰st‰ alkaen", "illalla" },
	  { 1, "aamup‰iv‰st‰ alkaen", "" },
	  { 3, "aamusta alkaen", "illalla" },
	  { 3, "aamusta alkaen", "illasta alkaen" },
	  { 2, "aamusta alkaen", "iltap‰iv‰st‰ alkaen" },
	  { 3, "aamusta alkaen", "iltap‰iv‰st‰ alkaen" },
	  { 1, "aamusta alkaen", "" },
	  { 2, "aamuyˆll‰", "p‰iv‰ll‰" },
	  { 4, "aamulla", "" },
	  { 4, "aamup‰iv‰ll‰", "" },
	  { 4, "illalla", "" },
	  { 4, "illasta alkaen", "" },
	  { 4, "iltap‰iv‰ll‰", "" },
	  { 4, "iltap‰iv‰st‰ alkaen", "" },
	  { 4, "iltayˆll‰", "" },
	  { 4, "keskip‰iv‰ll‰", "" },
	  { 4, "p‰iv‰ll‰", "" },
	  { 1, "iltap‰iv‰ll‰", "" },
	  { 1, "iltap‰iv‰st‰ alkaen", "" },
	  { 3, "keskip‰iv‰st‰ alkaen", "illalla" },
	  { 1, "keskip‰iv‰st‰ alkaen", "" },
	  { 2, "keskiyˆll‰", "p‰iv‰ll‰" },
	  { 2, "", "aamup‰iv‰st‰ alkaen" },
	  { 3, "", "illalla" },
	  { 3, "", "iltap‰iv‰st‰ alkaen" },
	  { 2, "", "keskip‰iv‰st‰ alkaen" },
	  { 1, "", "" },
	  { 3, "p‰iv‰ll‰", "iltap‰iv‰st‰ alkaen" },
	  { 1, "p‰iv‰ll‰", "" },
	  { 2, "yˆll‰", "aamup‰iv‰st‰ alkaen" },
	  { 2, "yˆll‰", "aamusta alkaen" },
	  { 2, "yˆll‰", "keskip‰iv‰st‰ alkaen" },
	  { 2, "yˆll‰", "p‰iv‰ll‰" }
	};

  // ----------------------------------------------------------------------
  /*!
   * \brief Table from page \ref page_rain_oneday
   *
   * All possible one day rains mapped onto 36 unique cases
   */
  // ----------------------------------------------------------------------

  int one_day_forecasts[301] =
	{
	  0,	// empty place holder
	  25,	// 1. Keskiyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  25,	// 2. Keskiyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  25,	// 3. Keskiyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 4. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 5. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 6. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  12,	// 7. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 8. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 9. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  13,	// 10. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  13,	// 11. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  19,	// 12. [Enimm‰kseen selke‰‰], keskip‰iv‰ll‰ [paikoin] [sadetta]
	  19,	// 13. [Enimm‰kseen selke‰‰], keskip‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 14. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 15. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 16. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 17. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 18. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  14,	// 19. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 20. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 21. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 22. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  18,	// 23. [Enimm‰kseen selke‰‰], iltayˆll‰ [paikoin] [sadetta]
	  18,	// 24. [Enimm‰kseen selke‰‰], iltayˆll‰ [paikoin] [sadetta]
	  25,	// 25. Keskiyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  25,	// 26. Keskiyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 27. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 28. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 29. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  12,	// 30. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 31. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 32. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 33. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  13,	// 34. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  13,	// 35. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  19,	// 36. [Enimm‰kseen selke‰‰], keskip‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 37. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 38. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 39. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 40. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 41. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  14,	// 42. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 43. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 44. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 45. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  18,	// 46. [Enimm‰kseen selke‰‰], iltayˆll‰ [paikoin] [sadetta]
	  18,	// 47. [Enimm‰kseen selke‰‰], iltayˆll‰ [paikoin] [sadetta]
	  25,	// 48. Keskiyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 49. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 50. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 51. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  12,	// 52. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 53. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 54. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 55. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  13,	// 56. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  13,	// 57. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  13,	// 58. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  19,	// 59. [Enimm‰kseen selke‰‰], keskip‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 60. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 61. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 62. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 63. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 64. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  14,	// 65. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 66. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 67. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 68. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  18,	// 69. [Enimm‰kseen selke‰‰], iltayˆll‰ [paikoin] [sadetta]
	  25,	// 70. Keskiyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 71. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 72. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 73. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  12,	// 74. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 75. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 76. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 77. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  13,	// 78. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  13,	// 79. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  20,	// 80. [Enimm‰kseen selke‰‰], p‰iv‰ll‰ [paikoin] [sadetta]
	  20,	// 81. [Enimm‰kseen selke‰‰], p‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 82. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 83. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 84. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 85. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  14,	// 86. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 87. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 88. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 89. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  15,	// 90. [Enimm‰kseen selke‰‰], illasta alkaen [paikoin] [sadetta]
	  36,	// 91. Yˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  36,	// 92. Yˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  11,	// 93. Aamuyˆll‰ [paikoin] [sadetta], p‰iv‰ll‰ [enimm‰kseen selke‰‰] ja poutaa
	  12,	// 94. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 95. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 96. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  12,	// 97. [Enimm‰kseen selke‰‰], aamulla [paikoin] [sadetta]
	  13,	// 98. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  13,	// 99. [Enimm‰kseen selke‰‰], aamup‰iv‰ll‰ [paikoin] [sadetta]
	  20,	// 100. [Enimm‰kseen selke‰‰], p‰iv‰ll‰ [paikoin] [sadetta]
	  20,	// 101. [Enimm‰kseen selke‰‰], p‰iv‰ll‰ [paikoin] [sadetta]
	  20,	// 102. [Enimm‰kseen selke‰‰], p‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 103. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 104. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  16,	// 105. [Enimm‰kseen selke‰‰], iltap‰iv‰ll‰ [paikoin] [sadetta]
	  17,	// 106. [Enimm‰kseen selke‰‰], iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  17,	// 107. [Enimm‰kseen selke‰‰], iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  14,	// 108. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  15,	// 109. [Enimm‰kseen selke‰‰], illasta alkaen [paikoin] [sadetta]
	  15,	// 110. [Enimm‰kseen selke‰‰], illasta alkaen [paikoin] [sadetta]
	  34,	// 111. Yˆll‰ [paikoin] [sadetta], aamusta alkaen [enimm‰kseen selke‰‰] ja poutaa
	  34,	// 112. Yˆll‰ [paikoin] [sadetta], aamusta alkaen [enimm‰kseen selke‰‰] ja poutaa
	  34,	// 113. Yˆll‰ [paikoin] [sadetta], aamusta alkaen [enimm‰kseen selke‰‰] ja poutaa
	  1,	// 114. Aamulla [paikoin] [sadetta], aamup‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  1,	// 115. Aamulla [paikoin] [sadetta], aamup‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  2,	// 116. Aamulla [paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  2,	// 117. Aamulla [paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  3,	// 118. Aamup‰iv‰ll‰ [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  31,	// 119. P‰iv‰ll‰ [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  31,	// 120. P‰iv‰ll‰ [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  32,	// 121. P‰iv‰ll‰ [paikoin] [sadetta]
	  32,	// 122. P‰iv‰ll‰ [paikoin] [sadetta]
	  21,	// 123. Iltap‰iv‰ll‰ [paikoin] [sadetta]
	  21,	// 124. Iltap‰iv‰ll‰ [paikoin] [sadetta]
	  22,	// 125. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 126. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 127. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  15,	// 128. [Enimm‰kseen selke‰‰], illasta alkaen [paikoin] [sadetta]
	  15,	// 129. [Enimm‰kseen selke‰‰], illasta alkaen [paikoin] [sadetta]
	  34,	// 130. Yˆll‰ [paikoin] [sadetta], aamusta alkaen [enimm‰kseen selke‰‰] ja poutaa
	  34,	// 131. Yˆll‰ [paikoin] [sadetta], aamusta alkaen [enimm‰kseen selke‰‰] ja poutaa
	  34,	// 132. Yˆll‰ [paikoin] [sadetta], aamusta alkaen [enimm‰kseen selke‰‰] ja poutaa
	  26,	// 133. [Paikoin] [sadetta], aamup‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  2,	// 134. Aamulla [paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  2,	// 135. Aamulla [paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  8,	// 136. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  8,	// 137. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  32,	// 138. P‰iv‰ll‰ [paikoin] [sadetta]
	  32,	// 139. P‰iv‰ll‰ [paikoin] [sadetta]
	  32,	// 140. P‰iv‰ll‰ [paikoin] [sadetta]
	  23,	// 141. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  23,	// 142. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  22,	// 143. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 144. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 145. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  14,	// 146. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  14,	// 147. [Enimm‰kseen selke‰‰], illalla [paikoin] [sadetta]
	  34,	// 148. Yˆll‰ [paikoin] [sadetta], aamusta alkaen [enimm‰kseen selke‰‰] ja poutaa
	  33,	// 149. Yˆll‰ [paikoin] [sadetta], aamup‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  33,	// 150. Yˆll‰ [paikoin] [sadetta], aamup‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  35,	// 151. Yˆll‰ [paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  29,	// 152. [Paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  9,	// 153. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  9,	// 154. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  9,	// 155. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  9,	// 156. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  4,	// 157. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 158. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  23,	// 159. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  24,	// 160. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 161. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 162. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 163. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 164. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  26,	// 165. [Paikoin] [sadetta], aamup‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  26,	// 166. [Paikoin] [sadetta], aamup‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  29,	// 167. [Paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  29,	// 168. [Paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  28,	// 169. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  9,	// 170. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  9,	// 171. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  9,	// 172. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  7,	// 173. Aamusta alkaen [paikoin] [sadetta], illasta alkaen poutaa
	  4,	// 174. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 175. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  24,	// 176. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  24,	// 177. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 178. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 179. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 180. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  26,	// 181. [Paikoin] [sadetta], aamup‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  29,	// 182. [Paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  29,	// 183. [Paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  28,	// 184. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 185. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  9,	// 186. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  6,	// 187. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 188. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 189. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 190. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  5,	// 191. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta]
	  24,	// 192. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  24,	// 193. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 194. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 195. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  29,	// 196. [Paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰],  poutaa
	  29,	// 197. [Paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  28,	// 198. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 199. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 200. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  9,	// 201. Aamusta alkaen [paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  6,	// 202. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 203. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 204. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 205. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta], illalla poutaa
	  5,	// 206. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta]
	  24,	// 207. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  24,	// 208. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  22,	// 209. Iltap‰iv‰st‰ alkaen [paikoin] [sadetta]
	  29,	// 210. [Paikoin] [sadetta], keskip‰iv‰st‰ alkaen [enimm‰kseen selke‰‰] ja poutaa
	  28,	// 211. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 212. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 213. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 214. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  6,	// 215. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 216. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 217. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 218. Aamusta alkaen [paikoin] [sadetta]
	  5,	// 219. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta]
	  5,	// 220. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta]
	  24,	// 221. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  24,	// 222. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  28,	// 223. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 224. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 225. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 226. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  27,	// 227. [Paikoin] [sadetta], illalla poutaa
	  6,	// 228. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 229. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 230. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 231. Aamusta alkaen [paikoin] [sadetta]
	  5,	// 232. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta]
	  5,	// 233. Aamup‰iv‰st‰ alkaen [paikoin] [sadetta]
	  24,	// 234. Keskip‰iv‰st‰ alkaen [paikoin] [sadetta]
	  28,	// 235. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 236. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 237. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  27,	// 238. [Paikoin] [sadetta], illalla poutaa
	  27,	// 239. [Paikoin] [sadetta], illalla poutaa
	  6,	// 240. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 241. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 242. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 243. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 244. Aamusta alkaen [paikoin] [sadetta]
	  30,	// 245. [Paikoin] [sadetta]
	  28,	// 246. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  28,	// 247. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  27,	// 248. [Paikoin] [sadetta], illalla poutaa
	  27,	// 249. [Paikoin] [sadetta], illalla poutaa
	  27,	// 250. [Paikoin] [sadetta], illalla poutaa
	  10,	// 251. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 252. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 253. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 254. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 255. Aamusta alkaen [paikoin] [sadetta]
	  28,	// 256. [Paikoin] [sadetta], iltap‰iv‰st‰ alkaen poutaa
	  27,	// 257. [Paikoin] [sadetta], illalla poutaa
	  27,	// 258. [Paikoin] [sadetta], illalla poutaa
	  27,	// 259. [Paikoin] [sadetta], illalla poutaa
	  30,	// 260. [Paikoin] [sadetta]
	  10,	// 261. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 262. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 263. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 264. Aamusta alkaen [paikoin] [sadetta]
	  27,	// 265. [Paikoin] [sadetta], illalla poutaa
	  27,	// 266. [Paikoin] [sadetta], illalla poutaa
	  27,	// 267. [Paikoin] [sadetta], illalla poutaa
	  30,	// 268. [Paikoin] [sadetta]
	  30,	// 269. [Paikoin] [sadetta]
	  10,	// 270. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 271. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 272. Aamusta alkaen [paikoin] [sadetta]
	  27,	// 273. [Paikoin] [sadetta], illalla poutaa
	  27,	// 274. [Paikoin] [sadetta], illalla poutaa
	  30,	// 275. [Paikoin] [sadetta]
	  30,	// 276. [Paikoin] [sadetta]
	  30,	// 277. [Paikoin] [sadetta]
	  10,	// 278. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 279. Aamusta alkaen [paikoin] [sadetta]
	  27,	// 280. [Paikoin] [sadetta], illalla poutaa
	  30,	// 281. [Paikoin] [sadetta]
	  30,	// 282. [Paikoin] [sadetta]
	  30,	// 283. [Paikoin] [sadetta]
	  30,	// 284. [Paikoin] [sadetta]
	  10,	// 285. Aamusta alkaen [paikoin] [sadetta]
	  30,	// 286. [Paikoin] [sadetta]
	  30,	// 287. [Paikoin] [sadetta]
	  30,	// 288. [Paikoin] [sadetta]
	  30,	// 289. [Paikoin] [sadetta]
	  30,	// 290. [Paikoin] [sadetta]
	  30,	// 291. [Paikoin] [sadetta]
	  30,	// 292. [Paikoin] [sadetta]
	  30,	// 293. [Paikoin] [sadetta]
	  30,	// 294. [Paikoin] [sadetta]
	  30,	// 295. [Paikoin] [sadetta]
	  30,	// 296. [Paikoin] [sadetta]
	  30,	// 297. [Paikoin] [sadetta]
	  30,	// 298. [Paikoin] [sadetta]
	  30,	// 299. [Paikoin] [sadetta]
	  30	// 300. [Paikoin] [sadetta]
	};

  // ----------------------------------------------------------------------
  /*!
   * \brief Calculates index for rain in the table at \ref page_rain_oneday
   *
   * If forecast length is N+1, the start index for that forecast length is
   * the arithmetic sum of 24+23+...+24-N+1. The sum is then (a1+an)*n/2
   * where a1=24, an=24-n+1. The final index is then obtained by adding
   * the starthour, plus one since the indices start at 1.
   *
   * For example, if the rain spans 4-6:
   *  -# n = (6-4)-1 = 1
   *  -# a1 = 24
   *  -# an = 24-n+1 = 24
   *  -# sn = ((a1+an)*n)/2 = ((24+24)*2)/2 = 24
   *  -# 24+4+1 = 29, the final result
   *
   * For example, if the rain spans 5-18:
   *  -# n = (18-5)-1 = 12
   *  -# a1 = 24
   *  -# an = 24-n+1 = 13
   *  -# sn = ((a1+an)*n)/2 = ((24+13)*12)/2 = 222
   *  -# 222+5+1 = 228, the final result
   * 
   *†\param theStartHour The start hour of the rain
   * \param theEndHour The end hour of the rain
   * \return The index for the rain
   */
  // ----------------------------------------------------------------------

  int one_day_rain_index(int theStartHour, int theEndHour)
  {
	if(theEndHour <= theStartHour)
	  throw runtime_error("Internal error in weather_overview: end hour must be greater than start hour");

	const int n = theEndHour-theStartHour-1;
	const int a1 = 24;
	const int an = 24-n+1;
	const int sn = ((a1+an)*n)/2;
	return (sn + theStartHour + 1);
  }

}

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generator story on a day with a single inclusive rain
   */
  // ----------------------------------------------------------------------

  Paragraph one_inclusive_rain(const NFmiTime & theForecastTime,
							   const AnalysisSources & theSources,
							   const WeatherArea & theArea,
							   const WeatherPeriod & thePeriod,
							   const string & theVar,
							   const WeatherPeriod & theRainPeriod)
  {
	Paragraph p;
	Sentence s;
	s << PeriodPhraseFactory::create("days",
									 theVar,
									 theForecastTime,
									 thePeriod);
	s << "ajoittain sateista";
	p << s;
	return p;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate story on a day with many inclusive rains
   */
  // ----------------------------------------------------------------------
   
  Sentence many_inclusive_rains(const NFmiTime & theForecastTime,
								const AnalysisSources & theSources,
								const WeatherArea & theArea,
								const WeatherPeriod & thePeriod,
								const string & theVar,
								const PrecipitationPeriodTools::RainPeriods & theRainPeriods)
  {
	Sentence s;
	s << PeriodPhraseFactory::create("days",
									 theVar,
									 theForecastTime,
									 thePeriod);
	s << "ajoittain sateista";
	return s;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate overview on weather
   *
   * \return The story
   *
   * \see page_weather_overview
   *
   * \todo Much missing
   */
  // ----------------------------------------------------------------------

  Paragraph WeatherStory::overview() const
  {
	MessageLogger log("WeatherStory::overview");

	using namespace PrecipitationPeriodTools;

	Paragraph paragraph;

	// we want the last day to extend up to midnight regardless
	// of the actual period length, otherwise we risk badly
	// formed forecasts when using full-day descriptions

	WeatherPeriod rainperiod(itsPeriod.localStartTime(),
							 TimeTools::dayEnd(itsPeriod.localEndTime()));

	RainPeriods rainperiods = PrecipitationPeriodTools::analyze(itsSources,
																itsArea,
																itsPeriod,
																itsVar);

	log << "Found " << rainperiods.size() << " rainy periods" << endl;

	{
	  for(RainPeriods::const_iterator it=rainperiods.begin();
		  it!=rainperiods.end();
		  it++)
		{
		  log << "Period: "
			  << it->localStartTime()
			  << " ... "
			  << it->localEndTime()
			  << endl;
		}
	}


	if(rainperiods.size() == 0)
	  {
		CloudinessStory story(itsForecastTime,
							  itsSources,
							  itsArea,
							  itsPeriod,
							  itsVar);
		paragraph << story.makeStory("cloudiness_overview");
		log << paragraph;
		return paragraph;
	  }

	// process each day separately

	HourPeriodGenerator generator(rainperiod, itsVar+"::day");

	const int n = generator.size();

	vector<RainPeriods> overlaps;
	vector<RainPeriods> inclusives;

	for(int day=1; day<=n; day++)
	  {
		WeatherPeriod period = generator.period(day);

		RainPeriods overlap = overlappingPeriods(rainperiods,period);
		RainPeriods inclusive = inclusivePeriods(rainperiods,period);

		overlaps.push_back(overlap);
		inclusives.push_back(inclusive);

	  }

	for(int day=1; day<=n; day++)
	  {
		const RainPeriods::size_type noverlap = overlaps[day-1].size();
		const RainPeriods::size_type ninclusive = inclusives[day-1].size();

		log << "Day " << day << " overlap   : " << noverlap << endl;
		log << "Day " << day << " inclusive : " << ninclusive << endl;

		if(ninclusive==noverlap) // all rains within the same day
		  {
			if(ninclusive==0)
			  {
				// find sequence of non-rainy days, report all at once
				int day2 = day;
				for(; day2<n; day2++)
				  {
					if(overlaps[day2].size()!=0 || inclusives[day2].size()!=0)
					  break;
				  }
				
				WeatherPeriod period(generator.period(day).localStartTime(),
									 generator.period(day2).localEndTime());
				
				if(day != day2)
				  log << "Cloudiness only for days " << day << '-' << day2 << endl;
				else
				  log << "Cloudiness only for day " << day << endl;
				
				CloudinessStory story(itsForecastTime,
									  itsSources,
									  itsArea,
									  period,
									  itsVar);
				
				paragraph << story.makeStory("cloudiness_overview");
				day = day2;
			  }
			
			else if(ninclusive==1)
			  {
				paragraph << one_inclusive_rain(itsForecastTime,
												itsSources,
												itsArea,
												generator.period(day),
												itsVar,
												inclusives[day-1].front());
			  }
			else
			  {
				paragraph << many_inclusive_rains(itsForecastTime,
												  itsSources,
												  itsArea,
												  generator.period(day),
												  itsVar,
												  inclusives[day-1]);
			  }
		  }
		else
		  {
			Sentence s;
			s << "ajoittain sateista";
			paragraph << s;
		  }
	  }

	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
