// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::WeatherStory::overview
 */
// ======================================================================

#include "WeatherStory.h"
#include "CloudinessStory.h"
#include "CloudinessStoryTools.h"
#include "Delimiter.h"
#include "GridForecaster.h"
#include "HourPeriodGenerator.h"
#include "MessageLogger.h"
#include "Paragraph.h"
#include "PeriodPhraseFactory.h"
#include "PrecipitationPeriodTools.h"
#include "PrecipitationStoryTools.h"
#include "RangeAcceptor.h"
#include "Sentence.h"
#include "Settings.h"
#include "TextGenError.h"
#include "TimeTools.h"
#include "WeatherResult.h"
#include "WeekdayTools.h"

#include "boost/lexical_cast.hpp"
#include <vector>

using namespace WeatherAnalysis;
using namespace boost;
using namespace std;

namespace
{

  // ----------------------------------------------------------------------
  /*!
   * \brief A structure for storing rain dispatch information
   *
   * The phrases are time phrases to be passed onto the story generating
   * subroutines. Either one or both may be empty strings.
   */
  // ----------------------------------------------------------------------

  struct rain_dispatch_data
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

  rain_dispatch_data one_day_cases[37] =
	{
	  { 0, "", ""},
	  { 2, "aamulla", "aamup�iv�st� alkaen" },
	  { 2, "aamulla", "keskip�iv�st� alkaen" },
	  { 3, "aamup�iv�ll�", "iltap�iv�st� alkaen" },
	  { 3, "aamup�iv�st� alkaen", "illalla" },
	  { 1, "aamup�iv�st� alkaen", "" },
	  { 3, "aamusta alkaen", "illalla" },
	  { 3, "aamusta alkaen", "illasta alkaen" },
	  { 2, "aamusta alkaen", "iltap�iv�st� alkaen" },
	  { 3, "aamusta alkaen", "iltap�iv�st� alkaen" },
	  { 1, "aamusta alkaen", "" },
	  { 2, "aamuy�ll�", "p�iv�ll�" },
	  { 4, "aamulla", "" },
	  { 4, "aamup�iv�ll�", "" },
	  { 4, "illalla", "" },
	  { 4, "illasta alkaen", "" },
	  { 4, "iltap�iv�ll�", "" },
	  { 4, "iltap�iv�st� alkaen", "" },
	  { 4, "iltay�ll�", "" },
	  { 4, "keskip�iv�ll�", "" },
	  { 4, "p�iv�ll�", "" },
	  { 1, "iltap�iv�ll�", "" },
	  { 1, "iltap�iv�st� alkaen", "" },
	  { 3, "keskip�iv�st� alkaen", "illalla" },
	  { 1, "keskip�iv�st� alkaen", "" },
	  { 2, "keskiy�ll�", "p�iv�ll�" },
	  { 2, "", "aamup�iv�st� alkaen" },
	  { 3, "", "illalla" },
	  { 3, "", "iltap�iv�st� alkaen" },
	  { 2, "", "keskip�iv�st� alkaen" },
	  { 1, "", "" },
	  { 3, "p�iv�ll�", "iltap�iv�st� alkaen" },
	  { 1, "p�iv�ll�", "" },
	  { 2, "y�ll�", "aamup�iv�st� alkaen" },
	  { 2, "y�ll�", "aamusta alkaen" },
	  { 2, "y�ll�", "keskip�iv�st� alkaen" },
	  { 2, "y�ll�", "p�iv�ll�" }
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
	  25,	// 1. Keskiy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  25,	// 2. Keskiy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  25,	// 3. Keskiy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 4. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 5. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 6. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  12,	// 7. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 8. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 9. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  13,	// 10. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  13,	// 11. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  19,	// 12. [Enimm�kseen selke��], keskip�iv�ll� [paikoin] [sadetta]
	  19,	// 13. [Enimm�kseen selke��], keskip�iv�ll� [paikoin] [sadetta]
	  16,	// 14. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 15. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 16. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 17. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 18. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  14,	// 19. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 20. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 21. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 22. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  18,	// 23. [Enimm�kseen selke��], iltay�ll� [paikoin] [sadetta]
	  18,	// 24. [Enimm�kseen selke��], iltay�ll� [paikoin] [sadetta]
	  25,	// 25. Keskiy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  25,	// 26. Keskiy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 27. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 28. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 29. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  12,	// 30. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 31. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 32. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 33. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  13,	// 34. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  13,	// 35. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  19,	// 36. [Enimm�kseen selke��], keskip�iv�ll� [paikoin] [sadetta]
	  16,	// 37. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 38. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 39. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 40. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 41. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  14,	// 42. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 43. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 44. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 45. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  18,	// 46. [Enimm�kseen selke��], iltay�ll� [paikoin] [sadetta]
	  18,	// 47. [Enimm�kseen selke��], iltay�ll� [paikoin] [sadetta]
	  25,	// 48. Keskiy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 49. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 50. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 51. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  12,	// 52. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 53. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 54. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 55. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  13,	// 56. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  13,	// 57. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  13,	// 58. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  19,	// 59. [Enimm�kseen selke��], keskip�iv�ll� [paikoin] [sadetta]
	  16,	// 60. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 61. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 62. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 63. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 64. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  14,	// 65. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 66. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 67. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 68. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  18,	// 69. [Enimm�kseen selke��], iltay�ll� [paikoin] [sadetta]
	  25,	// 70. Keskiy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 71. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 72. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 73. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  12,	// 74. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 75. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 76. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 77. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  13,	// 78. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  13,	// 79. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  20,	// 80. [Enimm�kseen selke��], p�iv�ll� [paikoin] [sadetta]
	  20,	// 81. [Enimm�kseen selke��], p�iv�ll� [paikoin] [sadetta]
	  16,	// 82. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 83. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 84. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 85. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  14,	// 86. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 87. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 88. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 89. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  15,	// 90. [Enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  36,	// 91. Y�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  36,	// 92. Y�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  11,	// 93. Aamuy�ll� [paikoin] [sadetta], p�iv�ll� [enimm�kseen selke��] ja poutaa
	  12,	// 94. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 95. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 96. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  12,	// 97. [Enimm�kseen selke��], aamulla [paikoin] [sadetta]
	  13,	// 98. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  13,	// 99. [Enimm�kseen selke��], aamup�iv�ll� [paikoin] [sadetta]
	  20,	// 100. [Enimm�kseen selke��], p�iv�ll� [paikoin] [sadetta]
	  20,	// 101. [Enimm�kseen selke��], p�iv�ll� [paikoin] [sadetta]
	  20,	// 102. [Enimm�kseen selke��], p�iv�ll� [paikoin] [sadetta]
	  16,	// 103. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 104. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  16,	// 105. [Enimm�kseen selke��], iltap�iv�ll� [paikoin] [sadetta]
	  17,	// 106. [Enimm�kseen selke��], iltap�iv�st� alkaen [paikoin] [sadetta]
	  17,	// 107. [Enimm�kseen selke��], iltap�iv�st� alkaen [paikoin] [sadetta]
	  14,	// 108. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  15,	// 109. [Enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  15,	// 110. [Enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  34,	// 111. Y�ll� [paikoin] [sadetta], aamusta alkaen [enimm�kseen selke��] ja poutaa
	  34,	// 112. Y�ll� [paikoin] [sadetta], aamusta alkaen [enimm�kseen selke��] ja poutaa
	  34,	// 113. Y�ll� [paikoin] [sadetta], aamusta alkaen [enimm�kseen selke��] ja poutaa
	  1,	// 114. Aamulla [paikoin] [sadetta], aamup�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  1,	// 115. Aamulla [paikoin] [sadetta], aamup�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  2,	// 116. Aamulla [paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  2,	// 117. Aamulla [paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  3,	// 118. Aamup�iv�ll� [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  31,	// 119. P�iv�ll� [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  31,	// 120. P�iv�ll� [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  32,	// 121. P�iv�ll� [paikoin] [sadetta]
	  32,	// 122. P�iv�ll� [paikoin] [sadetta]
	  21,	// 123. Iltap�iv�ll� [paikoin] [sadetta]
	  21,	// 124. Iltap�iv�ll� [paikoin] [sadetta]
	  22,	// 125. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 126. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 127. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  15,	// 128. [Enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  15,	// 129. [Enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  34,	// 130. Y�ll� [paikoin] [sadetta], aamusta alkaen [enimm�kseen selke��] ja poutaa
	  34,	// 131. Y�ll� [paikoin] [sadetta], aamusta alkaen [enimm�kseen selke��] ja poutaa
	  34,	// 132. Y�ll� [paikoin] [sadetta], aamusta alkaen [enimm�kseen selke��] ja poutaa
	  26,	// 133. [Paikoin] [sadetta], aamup�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  2,	// 134. Aamulla [paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  2,	// 135. Aamulla [paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  8,	// 136. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  8,	// 137. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  32,	// 138. P�iv�ll� [paikoin] [sadetta]
	  32,	// 139. P�iv�ll� [paikoin] [sadetta]
	  32,	// 140. P�iv�ll� [paikoin] [sadetta]
	  23,	// 141. Keskip�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  23,	// 142. Keskip�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  22,	// 143. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 144. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 145. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  14,	// 146. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  14,	// 147. [Enimm�kseen selke��], illalla [paikoin] [sadetta]
	  34,	// 148. Y�ll� [paikoin] [sadetta], aamusta alkaen [enimm�kseen selke��] ja poutaa
	  33,	// 149. Y�ll� [paikoin] [sadetta], aamup�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  33,	// 150. Y�ll� [paikoin] [sadetta], aamup�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  35,	// 151. Y�ll� [paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  29,	// 152. [Paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  9,	// 153. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  9,	// 154. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  9,	// 155. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  9,	// 156. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  4,	// 157. Aamup�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 158. Aamup�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  23,	// 159. Keskip�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  24,	// 160. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 161. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 162. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 163. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 164. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  26,	// 165. [Paikoin] [sadetta], aamup�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  26,	// 166. [Paikoin] [sadetta], aamup�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  29,	// 167. [Paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  29,	// 168. [Paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  28,	// 169. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  9,	// 170. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  9,	// 171. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  9,	// 172. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  7,	// 173. Aamusta alkaen [paikoin] [sadetta], illasta alkaen poutaa
	  4,	// 174. Aamup�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 175. Aamup�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  24,	// 176. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  24,	// 177. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 178. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 179. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 180. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  26,	// 181. [Paikoin] [sadetta], aamup�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  29,	// 182. [Paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  29,	// 183. [Paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  28,	// 184. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 185. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  9,	// 186. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  6,	// 187. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 188. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 189. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 190. Aamup�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  5,	// 191. Aamup�iv�st� alkaen [paikoin] [sadetta]
	  24,	// 192. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  24,	// 193. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 194. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 195. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  29,	// 196. [Paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��],  poutaa
	  29,	// 197. [Paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  28,	// 198. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 199. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 200. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  9,	// 201. Aamusta alkaen [paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  6,	// 202. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 203. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 204. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 205. Aamup�iv�st� alkaen [paikoin] [sadetta], illalla poutaa
	  5,	// 206. Aamup�iv�st� alkaen [paikoin] [sadetta]
	  24,	// 207. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  24,	// 208. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  22,	// 209. Iltap�iv�st� alkaen [paikoin] [sadetta]
	  29,	// 210. [Paikoin] [sadetta], keskip�iv�st� alkaen [enimm�kseen selke��] ja poutaa
	  28,	// 211. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 212. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 213. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 214. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  6,	// 215. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 216. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 217. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 218. Aamusta alkaen [paikoin] [sadetta]
	  5,	// 219. Aamup�iv�st� alkaen [paikoin] [sadetta]
	  5,	// 220. Aamup�iv�st� alkaen [paikoin] [sadetta]
	  24,	// 221. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  24,	// 222. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  28,	// 223. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 224. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 225. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 226. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  27,	// 227. [Paikoin] [sadetta], illalla poutaa
	  6,	// 228. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 229. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 230. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 231. Aamusta alkaen [paikoin] [sadetta]
	  5,	// 232. Aamup�iv�st� alkaen [paikoin] [sadetta]
	  5,	// 233. Aamup�iv�st� alkaen [paikoin] [sadetta]
	  24,	// 234. Keskip�iv�st� alkaen [paikoin] [sadetta]
	  28,	// 235. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 236. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 237. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  27,	// 238. [Paikoin] [sadetta], illalla poutaa
	  27,	// 239. [Paikoin] [sadetta], illalla poutaa
	  6,	// 240. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 241. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 242. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 243. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 244. Aamusta alkaen [paikoin] [sadetta]
	  30,	// 245. [Paikoin] [sadetta]
	  28,	// 246. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  28,	// 247. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
	  27,	// 248. [Paikoin] [sadetta], illalla poutaa
	  27,	// 249. [Paikoin] [sadetta], illalla poutaa
	  27,	// 250. [Paikoin] [sadetta], illalla poutaa
	  10,	// 251. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 252. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 253. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 254. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 255. Aamusta alkaen [paikoin] [sadetta]
	  28,	// 256. [Paikoin] [sadetta], iltap�iv�st� alkaen poutaa
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
   * \brief Second table from page \ref page_rain_twoday
   *
   * All 48 unique one day rains mapped onto 17 different reduced
   * cases with two time phrase parameters
   */
  // ----------------------------------------------------------------------

  rain_dispatch_data two_day_cases[49] =
	{
	  // empty placeholder to get indices to start from 1
	  { 0, "", ""},
	  // 1. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 3, "aamup�iv�st� alkaen", "aamup�iv�st� alkaen" },
	  // 2. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  { 4, "aamup�iv�st� alkaen", "" },
	  // 3. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  { 2, "aamup�iv�st� alkaen", "iltap�iv�st� alkaen" },
	  // 4. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 3, "aamup�iv�st� alkaen", "keskip�iv�st� alkaen" },
	  // 5. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta]
	  { 1, "aamup�iv�st� alkaen", "" },
	  // 6. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  { 5, "aamup�iv�st� alkaen", "" },
	  // 7. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "aamup�iv�st� alkaen", "" },
	  // 8. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 3, "aamusta alkaen", "aamup�iv�st� alkaen" },
	  // 9. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  { 4, "aamusta alkaen", "" },
	  // 10. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  { 2, "aamusta alkaen", "iltap�iv�st� alkaen" },
	  // 11. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 3, "aamusta alkaen", "keskip�iv�st� alkaen" },
	  // 12. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  { 5, "aamusta alkaen", "" },
	  // 13. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "aamusta alkaen", "" },
	  // 14. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  { 5, "aamut�st� alkaen", "" },
	  // 15. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "aamuy�st� alkaen", "" },
	  // 16. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  { 7, "illalla", "y�ll�" },
	  // 17. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 10, "illasta alkaen", "aamup�iv�st� alkaen" },
	  // 18. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  { 9, "illasta alkaen", "iltap�iv�st� alkaen" },
	  // 19. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 10, "illasta alkaen", "keskip�iv�st� alkaen" },
	  // 20. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  { 8, "illasta alkaen", "" },
	  // 21. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 11, "illasta alkaen", "" },
	  // 22. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 10, "iltay�st� alkaen", "aamup�iv�st� alkaen" },
	  // 23. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  { 9, "iltay�st� alkaen", "iltap�iv�st� alkaen" },
	  // 24. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 10, "iltay�st� alkaen", "keskip�iv�st� alkaen" },
	  // 25. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]
	  { 8, "iltay�st� alkaen", "" },
	  // 26. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 11, "iltay�st� alkaen", "" },
	  // 27. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  { 7, "y�ll�", "" },
	  // 28. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 3, "iltap�iv�st� alkaen", "aamup�iv�st� alkaen" },
	  // 29. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  { 4, "iltap�iv�st� alkaen", "" },
	  // 30. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  { 2, "iltap�iv�st� alkaen", "iltap�iv�st� alkaen" },
	  // 31. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 3, "iltap�iv�st� alkaen", "keskip�iv�st� alkaen" },
	  // 32. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta]
	  { 1, "iltap�iv�st� alkaen", "" },
	  // 33. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  { 5, "iltap�iv�st� alkaen", "" },
	  // 34. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "iltap�iv�st� alkaen", "" },
	  // 35. T�n��n ja huomenna [paikoin] [sadetta]
	  { 17, "", "" },
	  // 36. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 3, "keskip�iv�st� alkaen", "aamup�iv�st� alkaen" },
	  // 37. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  { 4, "keskip�iv�st� alkaen", "" },
	  // 38. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  { 2, "keskip�iv�st� alkaen", "iltap�iv�st� alkaen" },
	  // 39. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 3, "keskip�iv�st� alkaen", "keskip�iv�st� alkaen" },
	  // 40. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  { 1, "keskip�iv�st� alkaen", "" },
	  // 41. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  { 5, "keskip�iv�st� alkaen", "" },
	  // 42. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "keskip�iv�st� alkaen", "" },
	  // 43. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 15, "aamup�iv�st� alkaen", "" },
	  // 44. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  { 15, "aamusta alkaen", "" },
	  // 45. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  { 16, "", "" },
	  // 46. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  { 13, "illalla", "" },
	  // 47. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  { 14, "iltap�iv�st� alkaen", "" },
	  // 48. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  { 15, "keskip�iv�st� alkaen", "" }
	};

  // ----------------------------------------------------------------------
  /*!
   * \brief Table from page \ref page_rain_twoday
   *
   * All possible one day rains mapped onto 48 unique cases
   */
  // ----------------------------------------------------------------------

  int two_day_forecasts[577] =
	{
	  0,  // place holder to get indices to start from 1
	  45, // 1. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 2. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 3. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 4. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  44, // 5. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  44, // 6. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  44, // 7. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  44, // 8. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  43, // 9. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  43, // 10. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 11. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 12. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  47, // 13. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 14. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 15. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 16. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  46, // 17. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 18. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 19. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 20. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  35, // 21. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 22. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 23. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 24. T�n��n ja huomenna [paikoin] [sadetta]
	  45, // 25. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 26. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 27. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 28. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  44, // 29. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  44, // 30. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  44, // 31. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  44, // 32. T�n��n [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimm�kseen selke��]
	  43, // 33. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  43, // 34. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 35. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 36. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  47, // 37. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 38. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 39. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 40. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  46, // 41. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 42. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 43. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 44. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  35, // 45. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 46. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 47. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 48. T�n��n ja huomenna [paikoin] [sadetta]
	  45, // 49. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 50. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 51. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 52. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  14, // 53. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 54. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 55. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 56. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  43, // 57. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  43, // 58. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 59. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 60. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  47, // 61. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 62. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 63. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 64. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  46, // 65. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 66. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 67. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 68. T�n��n [paikoin] [sadetta], huomenna illalla poutaa
	  35, // 69. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 70. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 71. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 72. T�n��n ja huomenna [paikoin] [sadetta]
	  45, // 73. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 74. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 75. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 76. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  14, // 77. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 78. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 79. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 80. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  43, // 81. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  43, // 82. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 83. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 84. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  47, // 85. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 86. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 87. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 88. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  15, // 89. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 90. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 91. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 92. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 93. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 94. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 95. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 96. T�n��n ja huomenna [paikoin] [sadetta]
	  45, // 97. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 98. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 99. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  45, // 100. T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  14, // 101. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 102. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 103. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  14, // 104. T�n��n aamuy�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  43, // 105. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  43, // 106. T�n��n [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 107. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  48, // 108. T�n��n [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  47, // 109. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 110. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 111. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  47, // 112. T�n��n [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  15, // 113. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 114. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 115. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 116. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 117. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 118. T�n��n aamuy�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 119. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 120. T�n��n ja huomenna [paikoin] [sadetta]
	  9, //  121. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  122. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  123. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  124. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  12, // 125. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 126. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 127. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 128. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  8, //  129. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  8, //  130. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  11, // 131. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  11, // 132. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  10, // 133. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 134. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 135. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 136. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  13, // 137. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 138. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 139. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 140. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 141. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 142. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 143. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 144. T�n��n ja huomenna [paikoin] [sadetta]
	  9, //  145. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  146. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  147. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  148. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  12, // 149. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 150. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 151. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 152. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  8, //  153. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  8, //  154. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  11, // 155. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  11, // 156. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  10, // 157. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 158. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 159. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  13, // 160. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 161. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 162. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 163. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 164. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 165. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 166. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 167. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 168. T�n��n ja huomenna [paikoin] [sadetta]
	  9, //  169. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  170. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  171. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  172. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  12, // 173. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 174. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 175. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 176. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  8, //  177. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  8, //  178. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  11, // 179. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  11, // 180. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  10, // 181. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 182. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 183. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 184. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  13, // 185. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 186. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 187. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 188. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 189. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 190. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 191. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 192. T�n��n ja huomenna [paikoin] [sadetta]
	  9, //  193. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  194. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  195. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  9, //  196. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  12, // 197. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 198. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 199. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  12, // 200. T�n��n aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  8, //  201. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  8, //  202. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  11, // 203. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  11, // 204. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  10, // 205. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 206. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 207. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  10, // 208. T�n��n aamusta alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  13, // 209. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 210. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 211. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 212. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 213. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 214. T�n��n aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 215. T�n��n ja huomenna [paikoin] [sadetta]
	  35, // 216. T�n��n ja huomenna [paikoin] [sadetta]
	  2, //  217. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  2, //  218. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  2, //  219. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  2, //  220. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  6, //  221. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  6, //  222. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  6, //  223. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  6, //  224. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  1, //  225. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  1, //  226. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  4, //  227. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  4, //  228. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  3, //  229. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  3, //  230. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  3, //  231. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  3, //  232. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  7, //  233. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  234. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  235. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  236. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  237. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  238. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  5, //  239. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta>
	  5, //  240. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta>
	  2, //  241. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  2, //  242. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  2, //  243. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  2, //  244. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  6, //  245. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  6, //  246. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  6, //  247. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  6, //  248. T�n��n aamup�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  1, //  249. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  1, //  250. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  4, //  251. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  4, //  252. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  3, //  253. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  3, //  254. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  3, //  255. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  3, //  256. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  7, //  257. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  258. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  259. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  260. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  261. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  262. T�n��n aamup�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  5, //  263. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta>
	  5, //  264. T�n��n aamup�iv�st� alkaen [paikoin] [sadetta>
	  37, // 265. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  37, // 266. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  37, // 267. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  37, // 268. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  41, // 269. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  41, // 270. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  41, // 271. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  41, // 272. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  36, // 273. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  36, // 274. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  39, // 275. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  39, // 276. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  38, // 277. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  38, // 278. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  38, // 279. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  38, // 280. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  42, // 281. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 282. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 283. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 284. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 285. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 286. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  40, // 287. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  40, // 288. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  37, // 289. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  37, // 290. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  37, // 291. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  37, // 292. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  41, // 293. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  41, // 294. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  41, // 295. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  41, // 296. T�n��n keskip�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  36, // 297. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  36, // 298. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  39, // 299. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  39, // 300. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  38, // 301. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  38, // 302. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  38, // 303. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  38, // 304. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  42, // 305. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 306. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 307. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 308. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 309. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 310. T�n��n keskip�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  40, // 311. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  40, // 312. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  29, // 313. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 314. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 315. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 316. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  33, // 317. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 318. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 319. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 320. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  28, // 321. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  28, // 322. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 323. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 324. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  30, // 325. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 326. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 327. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 328. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  34, // 329. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 330. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 331. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 332. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 333. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 334. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  40, // 335. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  40, // 336. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  29, // 337. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 338. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 339. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 340. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  33, // 341. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 342. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 343. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 344. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  28, // 345. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  28, // 346. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 347. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 348. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  30, // 349. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 350. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 351. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 352. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  34, // 353. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 354. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 355. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 356. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 357. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 358. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  40, // 359. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  40, // 360. T�n��n keskip�iv�st� alkaen [paikoin] [sadetta]
	  29, // 361. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 362. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 363. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 364. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  33, // 365. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 366. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 367. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 368. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  28, // 369. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  28, // 370. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 371. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 372. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  30, // 373. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 374. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 375. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 376. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  34, // 377. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 378. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 379. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 380. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 381. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 382. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  32, // 383. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta>
	  32, // 384. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta>
	  29, // 385. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 386. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 387. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 388. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  33, // 389. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 390. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 391. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 392. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  28, // 393. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  28, // 394. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 395. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 396. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  30, // 397. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 398. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 399. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 400. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  34, // 401. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 402. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 403. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 404. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 405. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 406. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  32, // 407. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta>
	  32, // 408. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta>
	  29, // 409. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 410. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 411. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  29, // 412. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  33, // 413. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 414. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 415. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  33, // 416. T�n��n iltap�iv�st� alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  28, // 417. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  28, // 418. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 419. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  31, // 420. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  30, // 421. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 422. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 423. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  30, // 424. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta], huomenna iltap�iv�st� alkaen poutaa
	  34, // 425. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 426. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 427. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 428. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 429. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 430. T�n��n iltap�iv�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  32, // 431. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta>
	  32, // 432. T�n��n iltap�iv�st� alkaen [paikoin] [sadetta>
	  16, // 433. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 434. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 435. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 436. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 437. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 438. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 439. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 440. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  17, // 441. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  17, // 442. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  19, // 443. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  19, // 444. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  18, // 445. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 446. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 447. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 448. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  21, // 449. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 450. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 451. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 452. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 453. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 454. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  20, // 455. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  20, // 456. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  16, // 457. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 458. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 459. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 460. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 461. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 462. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 463. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 464. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  17, // 465. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  17, // 466. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  19, // 467. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  19, // 468. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  18, // 469. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 470. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 471. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 472. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  21, // 473. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 474. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 475. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 476. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 477. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 478. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  20, // 479. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  20, // 480. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  16, // 481. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 482. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 483. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 484. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 485. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 486. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 487. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 488. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  17, // 489. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  17, // 490. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  19, // 491. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  17, // 492. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  18, // 493. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 494. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 495. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 496. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  21, // 497. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 498. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 499. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 500. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 501. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 502. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  20, // 503. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  20, // 504. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  16, // 505. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 506. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 507. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 508. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 509. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 510. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 511. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  16, // 512. T�n��n [enimm�kseen selke��], illalla ja y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  17, // 513. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  17, // 514. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  19, // 515. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  19, // 516. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  18, // 517. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 518. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 519. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  18, // 520. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  21, // 521. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 522. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 523. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 524. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 525. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 526. T�n��n [enimm�kseen selke��], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  20, // 527. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  20, // 528. T�n��n [enimm�kseen selke��], illasta alkaen [paikoin] [sadetta]
	  27, // 529. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 530. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 531. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 532. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 533. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 534. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 535. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 536. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  22, // 537. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  22, // 538. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  24, // 539. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  24, // 540. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  23, // 541. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  23, // 542. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  23, // 543. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  23, // 544. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  26, // 545. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 546. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 547. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 548. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 549. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 550. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  25, // 551. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]
	  25, // 552. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]
	  27, // 553. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 554. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 555. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 556. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 557. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 558. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 559. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  27, // 560. T�n��n [enimm�kseen selke��], y�ll� [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  22, // 561. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  22, // 562. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna aamup�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  24, // 563. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  24, // 564. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna keskip�iv�st� alkaen poutaa, [enimm�kseen selke��]
	  23, // 565. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  23, // 566. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  23, // 567. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  23, // 568. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]. Huomenna iltap�iv�st� alkaen poutaa
	  26, // 569. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 570. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 571. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 572. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 573. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 574. T�n��n [enimm�kseen selke��], iltay�st� alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  25, // 575. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]
	  25  // 576. T�n��n [enimm�kseen selke��], iltay�st� alkaen [paikoin] [sadetta]
	};

  // ----------------------------------------------------------------------
  /*!
   * \brief Calculates index for rain in the first table at \ref page_rain_oneday
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
   *�\param theStartHour The start hour of the rain
   * \param theEndHour The end hour of the rain
   * \return The index for the rain
   */
  // ----------------------------------------------------------------------

  int one_day_rain_index(int theStartHour, int theEndHour)
  {
	if(theEndHour <= theStartHour)
	  throw TextGen::TextGenError("Internal error in weather_overview: end hour must be greater than start hour");

	const int n = theEndHour-theStartHour-1;
	const int a1 = 24;
	const int an = 24-n+1;
	const int sn = ((a1+an)*n)/2;
	return (sn + theStartHour + 1);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Calculates index for rain in the second table at \ref page_rain_oneday
   *
   *�\param theStartHour The start hour of the rain
   * \param theEndHour The end hour of the rain
   * \return The index for the rain
   */
  // ----------------------------------------------------------------------

  int one_day_rain_unique_index(int theStartHour, int theEndHour)
  {
	const int tmp = one_day_rain_index(theStartHour,theEndHour);
	return one_day_forecasts[tmp];
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Calculates index for rain in the first table at \ref page_rain_twoday
   *
   *�\param theStartHour The start hour of the rain
   * \param theEndHour The end hour of the rain
   * \return The index for the rain
   */
  // ----------------------------------------------------------------------

  int two_day_rain_index(int theStartHour, int theEndHour)
  {
	return (theStartHour*24 + theEndHour);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Calculates index for rain in the second table at \ref page_rain_twoday
   *
   *�\param theStartHour The start hour of the rain
   * \param theEndHour The end hour of the rain
   * \return The index for the rain
   */
  // ----------------------------------------------------------------------

  int two_day_rain_unique_index(int theStartHour, int theEndHour)
  {
	const int tmp = two_day_rain_index(theStartHour,theEndHour);
	return two_day_forecasts[tmp];
  }


}

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Generate the "sadetta" phrases
   *
   * \param theSources The analysis sources
   * \param theArea The area to be analyzed
   * \param thePeriod The rainy period to be analyzed
   * \param theVar The control variable
   * \param theDay The day in question
   * \return The phrase
   */
  // ----------------------------------------------------------------------

  Sentence rain_phrase(const AnalysisSources & theSources,
					   const WeatherArea & theArea,
					   const WeatherPeriod & thePeriod,
					   const string & theVar,
					   int theDay)
  {
	Sentence s;
	s << "sadetta";
	return s;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generator story on a day with a single inclusive rain
   */
  // ----------------------------------------------------------------------

  Sentence one_inclusive_rain(const NFmiTime & theForecastTime,
							  const AnalysisSources & theSources,
							  const WeatherArea & theArea,
							  const WeatherPeriod & thePeriod,
							  const string & theVar,
							  const WeatherPeriod & theRainPeriod,
							  int theDay)
  {
	using namespace CloudinessStoryTools;
	using namespace PrecipitationStoryTools;

	Sentence s;
	s << PeriodPhraseFactory::create("days",
									 theVar,
									 theForecastTime,
									 thePeriod);

	// start & end times of the rain
	int rainstarthour = theRainPeriod.localStartTime().GetHour();
	int rainendhour = theRainPeriod.localEndTime().GetHour() + 1;
	if(rainendhour==1) rainendhour = 24;

	// mapping onto the four basic cases
	const int idx = one_day_rain_unique_index(rainstarthour,rainendhour);
	const int phrase = one_day_cases[idx].index;

	switch(phrase)
	  {
		// [Aamulla] [paikoin] [sadetta]
	  case 1:
		{
		  s << one_day_cases[idx].phrase1;
		  s << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay);
		  s << rain_phrase(theSources,theArea,theRainPeriod,theVar,theDay);
		  break;
		}
		// [Aamulla] [paikoin] [sadetta], [aamulla] [enimm�kseen selke��] ja poutaa
	  case 2:
		{
		  WeatherPeriod cperiod(theRainPeriod.localEndTime(),
								thePeriod.localEndTime());
		  s << one_day_cases[idx].phrase1;
		  s << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay);
		  s << rain_phrase(theSources,theArea,theRainPeriod,theVar,theDay);
		  s << Delimiter(",");
		  s << one_day_cases[idx].phrase2;
		  s << cloudiness_phrase(theSources,theArea,cperiod,theVar,theDay);
		  s << "ja" << "poutaa";
		  break;
		}
		// [Aamulla] [paikoin] [sadetta], [aamulla] poutaa
	  case 3:
		{
		  s << one_day_cases[idx].phrase1;
		  s << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay);
		  s << rain_phrase(theSources,theArea,theRainPeriod,theVar,theDay);
		  s << Delimiter(",");
		  s << one_day_cases[idx].phrase2;
		  s << "poutaa";
		  break;
		}
		// [Enimm�kseen selke��], [aamulla] [paikoin] [sadetta]
	  case 4:
		{
		  s << cloudiness_phrase(theSources,theArea,thePeriod,theVar,theDay);
		  s << Delimiter(",");
		  s << one_day_cases[idx].phrase1;
		  s << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay);
		  s << rain_phrase(theSources,theArea,theRainPeriod,theVar,theDay);
		  break;
		}
	  default:
		throw TextGenError("Internal error in weather_overview::one_inclusive_rain");
	  }
	return s;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Generator story on two days with one inclusive rain
   */
  // ----------------------------------------------------------------------

  Paragraph one_twoday_inclusive_rain(const NFmiTime & theForecastTime,
									  const AnalysisSources & theSources,
									  const WeatherArea & theArea,
									  const WeatherPeriod & thePeriod,
									  const string & theVar,
									  const WeatherPeriod & theRainPeriod,
									  int theDay)
  {
	using CloudinessStoryTools::cloudiness_phrase;
	using PrecipitationStoryTools::places_phrase;

	// start & end times of the rain
	int rainstarthour = theRainPeriod.localStartTime().GetHour();
	int rainendhour = theRainPeriod.localEndTime().GetHour() + 1;
	if(rainendhour==1) rainendhour = 24;

	// mapping onto the seventeen basic cases
	const int idx = two_day_rain_unique_index(rainstarthour,rainendhour);
	const int phrase = two_day_cases[idx].index;
	const char * phrase1 = two_day_cases[idx].phrase1;
	const char * phrase2 = two_day_cases[idx].phrase2;

	// days 1 and 2
	const WeatherPeriod day1(TimeTools::dayStart(theRainPeriod.localStartTime()),
							 TimeTools::dayEnd(theRainPeriod.localStartTime()));
	const WeatherPeriod day2(TimeTools::nextDay(day1.localStartTime()),
							 TimeTools::nextDay(day1.localEndTime()));

	// the period before the rain
	const NFmiTime before_rain_start(TimeTools::dayStart(theRainPeriod.localStartTime()));
	const NFmiTime before_rain_end(theRainPeriod.localStartTime());
	const WeatherPeriod before_rain(before_rain_start,before_rain_end);

	// the period after the rain
	const NFmiTime after_rain_start(TimeTools::addHours(theRainPeriod.localEndTime(),1));
	const NFmiTime after_rain_end(TimeTools::dayEnd(after_rain_start));
	const WeatherPeriod after_rain(after_rain_start,after_rain_end);

	Paragraph paragraph;
	Sentence s1;
	Sentence s2;
	switch(phrase)
	  {
		// T�n��n [aamusta alkaen] [paikoin] [sadetta]
	  case 1:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  break;
		}
		// T�n��n [aamusta alkaen] [paikoin] [sadetta], huomenna [aamusta alkaen] poutaa
	  case 2:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta"
			 << Delimiter(",")
			 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << phrase2
			 << "poutaa";
		  break;
		}
		// T�n��n [aamusta alkaen] [paikoin] [sadetta], huomenna [aamusta alkaen] poutaa, [enimm�kseen selke��]
	  case 3:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta"
			 << Delimiter(",")
			 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << phrase2
			 << "poutaa"
			 << Delimiter(",")
			 << cloudiness_phrase(theSources,theArea,after_rain,theVar,theDay+1);
		  break;
		}
		// T�n��n [aamusta alkaen] [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  case 4:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta"
			 << Delimiter(",")
			 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << cloudiness_phrase(theSources,theArea,after_rain,theVar,theDay+1);
		  break;
		}
		// T�n��n [aamusta alkaen] tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimm�kseen selke��]
	  case 5:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << phrase1
			 << WeekdayTools::until_weekday_morning(day2.localStartTime())
			 << "asti"
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta"
			 << Delimiter(",")
			 << WeekdayTools::on_weekday(day2.localStartTime())
			 << cloudiness_phrase(theSources,theArea,after_rain,theVar,theDay+1);
		  break;
		}
		// T�n��n [aamusta alkaen] tiistai-iltaan asti [paikoin] [sadetta]
	  case 6:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << phrase1
			 << WeekdayTools::until_weekday_morning(day2.localStartTime())
			 << "asti"
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  break;
		}
		// T�n��n [enimm�kseen selke��], [aamulla] ja [y�ll�] [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  case 7:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << cloudiness_phrase(theSources,theArea,before_rain,theVar,theDay)
			 << Delimiter(",")
			 << phrase1;
		  if(phrase2 != "")
			s1 << "ja" << phrase2;
		  s1 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";

		  s2 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << cloudiness_phrase(theSources,theArea,after_rain,theVar,theDay+1);
		  break;
		}
		// T�n��n [enimm�kseen selke��], [aamusta alkaen] [paikoin] [sadetta]
	  case 8:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << cloudiness_phrase(theSources,theArea,before_rain,theVar,theDay)
			 << Delimiter(",")
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  break;
		}
		// T�n��n [enimm�kseen selke��], [aamusta alkaen] [paikoin] [sadetta]. Huomenna [aamusta alkaen] poutaa
	  case 9:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << cloudiness_phrase(theSources,theArea,before_rain,theVar,theDay)
			 << Delimiter(",")
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  s2 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << phrase2
			 << "poutaa";
		  break;
		}
		// T�n��n [enimm�kseen selke��], [aamusta alkaen] [paikoin] [sadetta]. Huomenna [aamusta alkaen] poutaa, [enimm�kseen selke��]
	  case 10:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << cloudiness_phrase(theSources,theArea,before_rain,theVar,theDay)
			 << Delimiter(",")
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  s2 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << phrase2
			 << "poutaa"
			 << Delimiter(",")
			 << cloudiness_phrase(theSources,theArea,after_rain,theVar,theDay+1);
		  break;
		}
		// T�n��n [enimm�kseen selke��], [aamusta alkaen] tiistai-iltaan asti [paikoin] [sadetta]
	  case 11:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << cloudiness_phrase(theSources,theArea,before_rain,theVar,theDay)
			 << Delimiter(",")
			 << phrase1
			 << WeekdayTools::until_weekday_evening(day2.localStartTime())
			 << "asti"
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  break;
		}
		// T�n��n [enimm�kseen selke��], [aamulla] [paikoin] [sadetta]. Huomenna [enimm�kseen selke��]
	  case 12:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << cloudiness_phrase(theSources,theArea,before_rain,theVar,theDay)
			 << Delimiter(",")
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  s2 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << cloudiness_phrase(theSources,theArea,after_rain,theVar,theDay+1);
		  break;
		}
		// T�n��n [paikoin] [sadetta], huomenna [aamulla] poutaa
	  case 13:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta"
			 << Delimiter(",")
			 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << phrase2
			 << "poutaa";
		  break;
		}
		// T�n��n [paikoin] [sadetta], huomenna [aamusta alkaen] poutaa
	  case 14:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta"
			 << Delimiter(",")
			 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << phrase2
			 << "poutaa";
		  break;
		}
		// T�n��n [paikoin] [sadetta], huomenna [aamusta alkaen] poutaa, [enimm�kseen selke��]
	  case 15:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta"
			 << Delimiter(",")
			 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << phrase2
			 << "poutaa"
			 << Delimiter(",")
			 << cloudiness_phrase(theSources,theArea,after_rain,theVar,theDay+1);
		  break;
		}
		// T�n��n [paikoin] [sadetta], huomenna [enimm�kseen selke��]
	  case 16:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay+1)
			 << "sadetta"
			 << Delimiter(",")
			 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << phrase2
			 << cloudiness_phrase(theSources,theArea,after_rain,theVar,theDay+1);
		  break;
		}
		// T�n��n ja huomenna [paikoin] [sadetta]
	  case 17:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << "ja"
			 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day2)
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  break;
		}
	  default:
		throw TextGenError("Internal error in weather_overview::one_twoday_inclusive_rain");
	  }

	paragraph << s1 << s2;
	return paragraph;
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
	// formed forecasts when using full-day descriptions. If the end
	// hour is too early, we ignore the last day fully

	const NFmiTime endtime = (itsPeriod.localEndTime().GetHour() < 12 ?
							  TimeTools::dayStart(itsPeriod.localEndTime()) :
							  TimeTools::dayEnd(itsPeriod.localEndTime()));

	WeatherPeriod rainperiod(itsPeriod.localStartTime(), endtime);

	RainPeriods rainperiods = PrecipitationPeriodTools::analyze(itsSources,
																itsArea,
																rainperiod,
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

	// process sequences of similar days

	HourPeriodGenerator generator(rainperiod, itsVar+"::day");

	const int n = generator.size();

	RainPeriods dummy;
	vector<RainPeriods> overlaps;
	vector<RainPeriods> inclusives;
	overlaps.push_back(dummy);
	inclusives.push_back(dummy);

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
		const RainPeriods::size_type noverlap = overlaps[day].size();
		const RainPeriods::size_type ninclusive = inclusives[day].size();

		log << "Day " << day << " overlap   : " << noverlap << endl;
		log << "Day " << day << " inclusive : " << ninclusive << endl;

		if(noverlap==0)
		  {
			// find sequence of non-rainy days, report all at once
			int day2 = day;
			for(; day2<n; day2++)
			  {
				if(overlaps[day2+1].size()!=0 || inclusives[day2+1].size()!=0)
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

		else if(ninclusive==1 && noverlap==1)
		  {
			paragraph << one_inclusive_rain(itsForecastTime,
											itsSources,
											itsArea,
											generator.period(day),
											itsVar,
											inclusives[day].front(),
											day);
		  }
		else if(ninclusive==noverlap)
		  {
				paragraph << many_inclusive_rains(itsForecastTime,
												  itsSources,
												  itsArea,
												  generator.period(day),
												  itsVar,
												  inclusives[day]);
		  }
		else if(ninclusive==0 && noverlap==1 &&
				overlaps[day+1].size()==1 && inclusives[day+1].size()==0)
		  {
			WeatherPeriod period(generator.period(day).localStartTime(),
								 generator.period(day+1).localEndTime());
			paragraph << one_twoday_inclusive_rain(itsForecastTime,
												   itsSources,
												   itsArea,
												   period,
												   itsVar,
												   inclusives[day].front(),
												   day);
			day++;
		  }
		else
		  {
			// seek end of rainy days
			// find sequence of non-rainy days, report all at once
			int day2 = day;
			for(; day2<n; day2++)
			  {
				// found end if there is a non-rainy day
				if(overlaps[day2+1].size()==0)
				  break;
				// found end if there is a 1-rain inclusive day
				if(overlaps[day2+1].size()==1 && inclusives[day2+1].size()==1)
				  break;
			  }
			
			WeatherPeriod period(generator.period(day).localStartTime(),
								 generator.period(day2).localEndTime());
			
			Sentence s;
			if(day==day2)
			  {
				s << PeriodPhraseFactory::create("today", itsVar, itsForecastTime, period)
				  << "ajoittain sateista";
			  }
			else
			  {
				s << WeekdayTools::from_weekday(period.localStartTime())
				  << "ajoittain sateista";
			  }
			paragraph << s;
			day = day2;
		  }
	  }
	
	log << paragraph;
	return paragraph;
  }


} // namespace TextGen

// ======================================================================
