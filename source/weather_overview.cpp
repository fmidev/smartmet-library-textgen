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
	  { 2, "aamulla", "aamupäivästä alkaen" },
	  { 2, "aamulla", "keskipäivästä alkaen" },
	  { 3, "aamupäivällä", "iltapäivästä alkaen" },
	  { 3, "aamupäivästä alkaen", "illalla" },
	  { 1, "aamupäivästä alkaen", "" },
	  { 3, "aamusta alkaen", "illalla" },
	  { 3, "aamusta alkaen", "illasta alkaen" },
	  { 2, "aamusta alkaen", "iltapäivästä alkaen" },
	  { 3, "aamusta alkaen", "iltapäivästä alkaen" },
	  { 1, "aamusta alkaen", "" },
	  { 2, "aamuyöllä", "päivällä" },
	  { 4, "aamulla", "" },
	  { 4, "aamupäivällä", "" },
	  { 4, "illalla", "" },
	  { 4, "illasta alkaen", "" },
	  { 4, "iltapäivällä", "" },
	  { 4, "iltapäivästä alkaen", "" },
	  { 4, "iltayöllä", "" },
	  { 4, "keskipäivällä", "" },
	  { 4, "päivällä", "" },
	  { 1, "iltapäivällä", "" },
	  { 1, "iltapäivästä alkaen", "" },
	  { 3, "keskipäivästä alkaen", "illalla" },
	  { 1, "keskipäivästä alkaen", "" },
	  { 2, "keskiyöllä", "päivällä" },
	  { 2, "", "aamupäivästä alkaen" },
	  { 3, "", "illalla" },
	  { 3, "", "iltapäivästä alkaen" },
	  { 2, "", "keskipäivästä alkaen" },
	  { 1, "", "" },
	  { 3, "päivällä", "iltapäivästä alkaen" },
	  { 1, "päivällä", "" },
	  { 2, "yöllä", "aamupäivästä alkaen" },
	  { 2, "yöllä", "aamusta alkaen" },
	  { 2, "yöllä", "keskipäivästä alkaen" },
	  { 2, "yöllä", "päivällä" }
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
	  25,	// 1. Keskiyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  25,	// 2. Keskiyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  25,	// 3. Keskiyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 4. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 5. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 6. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  12,	// 7. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 8. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 9. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  13,	// 10. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  13,	// 11. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  19,	// 12. [Enimmäkseen selkeää], keskipäivällä [paikoin] [sadetta]
	  19,	// 13. [Enimmäkseen selkeää], keskipäivällä [paikoin] [sadetta]
	  16,	// 14. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 15. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 16. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 17. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 18. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  14,	// 19. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 20. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 21. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 22. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  18,	// 23. [Enimmäkseen selkeää], iltayöllä [paikoin] [sadetta]
	  18,	// 24. [Enimmäkseen selkeää], iltayöllä [paikoin] [sadetta]
	  25,	// 25. Keskiyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  25,	// 26. Keskiyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 27. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 28. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 29. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  12,	// 30. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 31. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 32. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 33. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  13,	// 34. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  13,	// 35. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  19,	// 36. [Enimmäkseen selkeää], keskipäivällä [paikoin] [sadetta]
	  16,	// 37. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 38. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 39. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 40. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 41. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  14,	// 42. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 43. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 44. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 45. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  18,	// 46. [Enimmäkseen selkeää], iltayöllä [paikoin] [sadetta]
	  18,	// 47. [Enimmäkseen selkeää], iltayöllä [paikoin] [sadetta]
	  25,	// 48. Keskiyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 49. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 50. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 51. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  12,	// 52. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 53. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 54. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 55. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  13,	// 56. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  13,	// 57. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  13,	// 58. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  19,	// 59. [Enimmäkseen selkeää], keskipäivällä [paikoin] [sadetta]
	  16,	// 60. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 61. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 62. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 63. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 64. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  14,	// 65. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 66. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 67. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 68. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  18,	// 69. [Enimmäkseen selkeää], iltayöllä [paikoin] [sadetta]
	  25,	// 70. Keskiyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 71. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 72. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 73. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  12,	// 74. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 75. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 76. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 77. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  13,	// 78. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  13,	// 79. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  20,	// 80. [Enimmäkseen selkeää], päivällä [paikoin] [sadetta]
	  20,	// 81. [Enimmäkseen selkeää], päivällä [paikoin] [sadetta]
	  16,	// 82. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 83. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 84. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 85. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  14,	// 86. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 87. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 88. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 89. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  15,	// 90. [Enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  36,	// 91. Yöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  36,	// 92. Yöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  11,	// 93. Aamuyöllä [paikoin] [sadetta], päivällä [enimmäkseen selkeää] ja poutaa
	  12,	// 94. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 95. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 96. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  12,	// 97. [Enimmäkseen selkeää], aamulla [paikoin] [sadetta]
	  13,	// 98. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  13,	// 99. [Enimmäkseen selkeää], aamupäivällä [paikoin] [sadetta]
	  20,	// 100. [Enimmäkseen selkeää], päivällä [paikoin] [sadetta]
	  20,	// 101. [Enimmäkseen selkeää], päivällä [paikoin] [sadetta]
	  20,	// 102. [Enimmäkseen selkeää], päivällä [paikoin] [sadetta]
	  16,	// 103. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 104. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  16,	// 105. [Enimmäkseen selkeää], iltapäivällä [paikoin] [sadetta]
	  17,	// 106. [Enimmäkseen selkeää], iltapäivästä alkaen [paikoin] [sadetta]
	  17,	// 107. [Enimmäkseen selkeää], iltapäivästä alkaen [paikoin] [sadetta]
	  14,	// 108. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  15,	// 109. [Enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  15,	// 110. [Enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  34,	// 111. Yöllä [paikoin] [sadetta], aamusta alkaen [enimmäkseen selkeää] ja poutaa
	  34,	// 112. Yöllä [paikoin] [sadetta], aamusta alkaen [enimmäkseen selkeää] ja poutaa
	  34,	// 113. Yöllä [paikoin] [sadetta], aamusta alkaen [enimmäkseen selkeää] ja poutaa
	  1,	// 114. Aamulla [paikoin] [sadetta], aamupäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  1,	// 115. Aamulla [paikoin] [sadetta], aamupäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  2,	// 116. Aamulla [paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  2,	// 117. Aamulla [paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  3,	// 118. Aamupäivällä [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  31,	// 119. Päivällä [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  31,	// 120. Päivällä [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  32,	// 121. Päivällä [paikoin] [sadetta]
	  32,	// 122. Päivällä [paikoin] [sadetta]
	  21,	// 123. Iltapäivällä [paikoin] [sadetta]
	  21,	// 124. Iltapäivällä [paikoin] [sadetta]
	  22,	// 125. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 126. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 127. Iltapäivästä alkaen [paikoin] [sadetta]
	  15,	// 128. [Enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  15,	// 129. [Enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  34,	// 130. Yöllä [paikoin] [sadetta], aamusta alkaen [enimmäkseen selkeää] ja poutaa
	  34,	// 131. Yöllä [paikoin] [sadetta], aamusta alkaen [enimmäkseen selkeää] ja poutaa
	  34,	// 132. Yöllä [paikoin] [sadetta], aamusta alkaen [enimmäkseen selkeää] ja poutaa
	  26,	// 133. [Paikoin] [sadetta], aamupäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  2,	// 134. Aamulla [paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  2,	// 135. Aamulla [paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  8,	// 136. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  8,	// 137. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  32,	// 138. Päivällä [paikoin] [sadetta]
	  32,	// 139. Päivällä [paikoin] [sadetta]
	  32,	// 140. Päivällä [paikoin] [sadetta]
	  23,	// 141. Keskipäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  23,	// 142. Keskipäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  22,	// 143. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 144. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 145. Iltapäivästä alkaen [paikoin] [sadetta]
	  14,	// 146. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  14,	// 147. [Enimmäkseen selkeää], illalla [paikoin] [sadetta]
	  34,	// 148. Yöllä [paikoin] [sadetta], aamusta alkaen [enimmäkseen selkeää] ja poutaa
	  33,	// 149. Yöllä [paikoin] [sadetta], aamupäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  33,	// 150. Yöllä [paikoin] [sadetta], aamupäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  35,	// 151. Yöllä [paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  29,	// 152. [Paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  9,	// 153. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  9,	// 154. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  9,	// 155. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  9,	// 156. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  4,	// 157. Aamupäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 158. Aamupäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  23,	// 159. Keskipäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  24,	// 160. Keskipäivästä alkaen [paikoin] [sadetta]
	  22,	// 161. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 162. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 163. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 164. Iltapäivästä alkaen [paikoin] [sadetta]
	  26,	// 165. [Paikoin] [sadetta], aamupäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  26,	// 166. [Paikoin] [sadetta], aamupäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  29,	// 167. [Paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  29,	// 168. [Paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  28,	// 169. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  9,	// 170. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  9,	// 171. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  9,	// 172. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  7,	// 173. Aamusta alkaen [paikoin] [sadetta], illasta alkaen poutaa
	  4,	// 174. Aamupäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 175. Aamupäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  24,	// 176. Keskipäivästä alkaen [paikoin] [sadetta]
	  24,	// 177. Keskipäivästä alkaen [paikoin] [sadetta]
	  22,	// 178. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 179. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 180. Iltapäivästä alkaen [paikoin] [sadetta]
	  26,	// 181. [Paikoin] [sadetta], aamupäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  29,	// 182. [Paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  29,	// 183. [Paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  28,	// 184. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 185. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  9,	// 186. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  6,	// 187. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 188. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 189. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 190. Aamupäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  5,	// 191. Aamupäivästä alkaen [paikoin] [sadetta]
	  24,	// 192. Keskipäivästä alkaen [paikoin] [sadetta]
	  24,	// 193. Keskipäivästä alkaen [paikoin] [sadetta]
	  22,	// 194. Iltapäivästä alkaen [paikoin] [sadetta]
	  22,	// 195. Iltapäivästä alkaen [paikoin] [sadetta]
	  29,	// 196. [Paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää],  poutaa
	  29,	// 197. [Paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  28,	// 198. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 199. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 200. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  9,	// 201. Aamusta alkaen [paikoin] [sadetta], iltapäivästä alkaen poutaa
	  6,	// 202. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 203. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 204. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  4,	// 205. Aamupäivästä alkaen [paikoin] [sadetta], illalla poutaa
	  5,	// 206. Aamupäivästä alkaen [paikoin] [sadetta]
	  24,	// 207. Keskipäivästä alkaen [paikoin] [sadetta]
	  24,	// 208. Keskipäivästä alkaen [paikoin] [sadetta]
	  22,	// 209. Iltapäivästä alkaen [paikoin] [sadetta]
	  29,	// 210. [Paikoin] [sadetta], keskipäivästä alkaen [enimmäkseen selkeää] ja poutaa
	  28,	// 211. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 212. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 213. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 214. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  6,	// 215. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 216. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 217. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 218. Aamusta alkaen [paikoin] [sadetta]
	  5,	// 219. Aamupäivästä alkaen [paikoin] [sadetta]
	  5,	// 220. Aamupäivästä alkaen [paikoin] [sadetta]
	  24,	// 221. Keskipäivästä alkaen [paikoin] [sadetta]
	  24,	// 222. Keskipäivästä alkaen [paikoin] [sadetta]
	  28,	// 223. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 224. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 225. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 226. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  27,	// 227. [Paikoin] [sadetta], illalla poutaa
	  6,	// 228. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  6,	// 229. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 230. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 231. Aamusta alkaen [paikoin] [sadetta]
	  5,	// 232. Aamupäivästä alkaen [paikoin] [sadetta]
	  5,	// 233. Aamupäivästä alkaen [paikoin] [sadetta]
	  24,	// 234. Keskipäivästä alkaen [paikoin] [sadetta]
	  28,	// 235. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 236. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 237. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  27,	// 238. [Paikoin] [sadetta], illalla poutaa
	  27,	// 239. [Paikoin] [sadetta], illalla poutaa
	  6,	// 240. Aamusta alkaen [paikoin] [sadetta], illalla poutaa
	  10,	// 241. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 242. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 243. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 244. Aamusta alkaen [paikoin] [sadetta]
	  30,	// 245. [Paikoin] [sadetta]
	  28,	// 246. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  28,	// 247. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
	  27,	// 248. [Paikoin] [sadetta], illalla poutaa
	  27,	// 249. [Paikoin] [sadetta], illalla poutaa
	  27,	// 250. [Paikoin] [sadetta], illalla poutaa
	  10,	// 251. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 252. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 253. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 254. Aamusta alkaen [paikoin] [sadetta]
	  10,	// 255. Aamusta alkaen [paikoin] [sadetta]
	  28,	// 256. [Paikoin] [sadetta], iltapäivästä alkaen poutaa
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
	  // 1. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 3, "aamupäivästä alkaen", "aamupäivästä alkaen" },
	  // 2. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  { 4, "aamupäivästä alkaen", "" },
	  // 3. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  { 2, "aamupäivästä alkaen", "iltapäivästä alkaen" },
	  // 4. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 3, "aamupäivästä alkaen", "keskipäivästä alkaen" },
	  // 5. Tänään aamupäivästä alkaen [paikoin] [sadetta]
	  { 1, "aamupäivästä alkaen", "" },
	  // 6. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  { 5, "aamupäivästä alkaen", "" },
	  // 7. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "aamupäivästä alkaen", "" },
	  // 8. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 3, "aamusta alkaen", "aamupäivästä alkaen" },
	  // 9. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  { 4, "aamusta alkaen", "" },
	  // 10. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  { 2, "aamusta alkaen", "iltapäivästä alkaen" },
	  // 11. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 3, "aamusta alkaen", "keskipäivästä alkaen" },
	  // 12. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  { 5, "aamusta alkaen", "" },
	  // 13. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "aamusta alkaen", "" },
	  // 14. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  { 5, "aamutöstä alkaen", "" },
	  // 15. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "aamuyöstä alkaen", "" },
	  // 16. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  { 7, "illalla", "yöllä" },
	  // 17. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 10, "illasta alkaen", "aamupäivästä alkaen" },
	  // 18. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  { 9, "illasta alkaen", "iltapäivästä alkaen" },
	  // 19. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 10, "illasta alkaen", "keskipäivästä alkaen" },
	  // 20. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  { 8, "illasta alkaen", "" },
	  // 21. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 11, "illasta alkaen", "" },
	  // 22. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 10, "iltayöstä alkaen", "aamupäivästä alkaen" },
	  // 23. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  { 9, "iltayöstä alkaen", "iltapäivästä alkaen" },
	  // 24. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 10, "iltayöstä alkaen", "keskipäivästä alkaen" },
	  // 25. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]
	  { 8, "iltayöstä alkaen", "" },
	  // 26. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 11, "iltayöstä alkaen", "" },
	  // 27. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  { 7, "yöllä", "" },
	  // 28. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 3, "iltapäivästä alkaen", "aamupäivästä alkaen" },
	  // 29. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  { 4, "iltapäivästä alkaen", "" },
	  // 30. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  { 2, "iltapäivästä alkaen", "iltapäivästä alkaen" },
	  // 31. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 3, "iltapäivästä alkaen", "keskipäivästä alkaen" },
	  // 32. Tänään iltapäivästä alkaen [paikoin] [sadetta]
	  { 1, "iltapäivästä alkaen", "" },
	  // 33. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  { 5, "iltapäivästä alkaen", "" },
	  // 34. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "iltapäivästä alkaen", "" },
	  // 35. Tänään ja huomenna [paikoin] [sadetta]
	  { 17, "", "" },
	  // 36. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 3, "keskipäivästä alkaen", "aamupäivästä alkaen" },
	  // 37. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  { 4, "keskipäivästä alkaen", "" },
	  // 38. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  { 2, "keskipäivästä alkaen", "iltapäivästä alkaen" },
	  // 39. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 3, "keskipäivästä alkaen", "keskipäivästä alkaen" },
	  // 40. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  { 1, "keskipäivästä alkaen", "" },
	  // 41. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  { 5, "keskipäivästä alkaen", "" },
	  // 42. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  { 6, "keskipäivästä alkaen", "" },
	  // 43. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 15, "aamupäivästä alkaen", "" },
	  // 44. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  { 15, "aamusta alkaen", "" },
	  // 45. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  { 16, "", "" },
	  // 46. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  { 13, "illalla", "" },
	  // 47. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  { 14, "iltapäivästä alkaen", "" },
	  // 48. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  { 15, "keskipäivästä alkaen", "" }
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
	  45, // 1. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 2. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 3. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 4. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  44, // 5. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  44, // 6. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  44, // 7. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  44, // 8. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  43, // 9. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  43, // 10. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 11. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 12. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  47, // 13. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 14. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 15. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 16. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  46, // 17. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 18. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 19. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 20. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  35, // 21. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 22. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 23. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 24. Tänään ja huomenna [paikoin] [sadetta]
	  45, // 25. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 26. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 27. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 28. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  44, // 29. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  44, // 30. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  44, // 31. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  44, // 32. Tänään [paikoin] [sadetta], huomenna aamusta alkaen poutaa, [enimmäkseen selkeää]
	  43, // 33. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  43, // 34. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 35. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 36. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  47, // 37. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 38. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 39. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 40. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  46, // 41. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 42. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 43. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 44. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  35, // 45. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 46. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 47. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 48. Tänään ja huomenna [paikoin] [sadetta]
	  45, // 49. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 50. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 51. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 52. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  14, // 53. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 54. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 55. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 56. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  43, // 57. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  43, // 58. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 59. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 60. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  47, // 61. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 62. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 63. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 64. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  46, // 65. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 66. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 67. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  46, // 68. Tänään [paikoin] [sadetta], huomenna illalla poutaa
	  35, // 69. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 70. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 71. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 72. Tänään ja huomenna [paikoin] [sadetta]
	  45, // 73. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 74. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 75. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 76. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  14, // 77. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 78. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 79. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 80. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  43, // 81. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  43, // 82. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 83. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 84. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  47, // 85. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 86. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 87. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 88. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  15, // 89. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 90. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 91. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 92. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 93. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 94. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 95. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 96. Tänään ja huomenna [paikoin] [sadetta]
	  45, // 97. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 98. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 99. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  45, // 100. Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  14, // 101. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 102. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 103. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  14, // 104. Tänään aamuyöstä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  43, // 105. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  43, // 106. Tänään [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 107. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  48, // 108. Tänään [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  47, // 109. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 110. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 111. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  47, // 112. Tänään [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  15, // 113. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 114. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 115. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 116. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 117. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  15, // 118. Tänään aamuyöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 119. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 120. Tänään ja huomenna [paikoin] [sadetta]
	  9, //  121. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  122. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  123. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  124. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  12, // 125. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 126. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 127. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 128. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  8, //  129. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  8, //  130. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  11, // 131. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  11, // 132. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  10, // 133. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 134. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 135. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 136. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  13, // 137. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 138. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 139. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 140. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 141. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 142. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 143. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 144. Tänään ja huomenna [paikoin] [sadetta]
	  9, //  145. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  146. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  147. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  148. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  12, // 149. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 150. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 151. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 152. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  8, //  153. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  8, //  154. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  11, // 155. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  11, // 156. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  10, // 157. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 158. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 159. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  13, // 160. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 161. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 162. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 163. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 164. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 165. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 166. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 167. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 168. Tänään ja huomenna [paikoin] [sadetta]
	  9, //  169. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  170. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  171. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  172. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  12, // 173. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 174. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 175. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 176. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  8, //  177. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  8, //  178. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  11, // 179. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  11, // 180. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  10, // 181. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 182. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 183. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 184. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  13, // 185. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 186. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 187. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 188. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 189. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 190. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 191. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 192. Tänään ja huomenna [paikoin] [sadetta]
	  9, //  193. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  194. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  195. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  9, //  196. Tänään aamusta alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  12, // 197. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 198. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 199. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  12, // 200. Tänään aamusta alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  8, //  201. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  8, //  202. Tänään aamusta alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  11, // 203. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  11, // 204. Tänään aamusta alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  10, // 205. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 206. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 207. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  10, // 208. Tänään aamusta alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  13, // 209. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 210. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 211. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 212. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 213. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  13, // 214. Tänään aamusta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  35, // 215. Tänään ja huomenna [paikoin] [sadetta]
	  35, // 216. Tänään ja huomenna [paikoin] [sadetta]
	  2, //  217. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  2, //  218. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  2, //  219. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  2, //  220. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  6, //  221. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  6, //  222. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  6, //  223. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  6, //  224. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  1, //  225. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  1, //  226. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  4, //  227. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  4, //  228. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  3, //  229. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  3, //  230. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  3, //  231. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  3, //  232. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  7, //  233. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  234. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  235. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  236. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  237. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  238. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  5, //  239. Tänään aamupäivästä alkaen [paikoin] [sadetta>
	  5, //  240. Tänään aamupäivästä alkaen [paikoin] [sadetta>
	  2, //  241. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  2, //  242. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  2, //  243. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  2, //  244. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  6, //  245. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  6, //  246. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  6, //  247. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  6, //  248. Tänään aamupäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  1, //  249. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  1, //  250. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  4, //  251. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  4, //  252. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  3, //  253. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  3, //  254. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  3, //  255. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  3, //  256. Tänään aamupäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  7, //  257. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  258. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  259. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  260. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  261. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  7, //  262. Tänään aamupäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  5, //  263. Tänään aamupäivästä alkaen [paikoin] [sadetta>
	  5, //  264. Tänään aamupäivästä alkaen [paikoin] [sadetta>
	  37, // 265. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  37, // 266. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  37, // 267. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  37, // 268. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  41, // 269. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  41, // 270. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  41, // 271. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  41, // 272. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  36, // 273. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  36, // 274. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  39, // 275. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  39, // 276. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  38, // 277. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  38, // 278. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  38, // 279. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  38, // 280. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  42, // 281. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 282. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 283. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 284. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 285. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 286. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  40, // 287. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  40, // 288. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  37, // 289. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  37, // 290. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  37, // 291. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  37, // 292. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  41, // 293. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  41, // 294. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  41, // 295. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  41, // 296. Tänään keskipäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  36, // 297. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  36, // 298. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  39, // 299. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  39, // 300. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  38, // 301. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  38, // 302. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  38, // 303. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  38, // 304. Tänään keskipäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  42, // 305. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 306. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 307. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 308. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 309. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  42, // 310. Tänään keskipäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  40, // 311. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  40, // 312. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  29, // 313. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 314. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 315. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 316. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  33, // 317. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 318. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 319. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 320. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  28, // 321. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  28, // 322. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 323. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 324. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  30, // 325. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 326. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 327. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 328. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  34, // 329. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 330. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 331. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 332. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 333. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 334. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  40, // 335. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  40, // 336. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  29, // 337. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 338. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 339. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 340. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  33, // 341. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 342. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 343. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 344. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  28, // 345. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  28, // 346. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 347. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 348. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  30, // 349. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 350. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 351. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 352. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  34, // 353. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 354. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 355. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 356. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 357. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 358. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  40, // 359. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  40, // 360. Tänään keskipäivästä alkaen [paikoin] [sadetta]
	  29, // 361. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 362. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 363. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 364. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  33, // 365. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 366. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 367. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 368. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  28, // 369. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  28, // 370. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 371. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 372. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  30, // 373. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 374. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 375. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 376. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  34, // 377. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 378. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 379. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 380. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 381. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 382. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  32, // 383. Tänään iltapäivästä alkaen [paikoin] [sadetta>
	  32, // 384. Tänään iltapäivästä alkaen [paikoin] [sadetta>
	  29, // 385. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 386. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 387. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 388. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  33, // 389. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 390. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 391. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 392. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  28, // 393. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  28, // 394. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 395. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 396. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  30, // 397. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 398. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 399. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 400. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  34, // 401. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 402. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 403. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 404. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 405. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 406. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  32, // 407. Tänään iltapäivästä alkaen [paikoin] [sadetta>
	  32, // 408. Tänään iltapäivästä alkaen [paikoin] [sadetta>
	  29, // 409. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 410. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 411. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  29, // 412. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
	  33, // 413. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 414. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 415. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  33, // 416. Tänään iltapäivästä alkaen tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
	  28, // 417. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  28, // 418. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 419. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  31, // 420. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  30, // 421. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 422. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 423. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  30, // 424. Tänään iltapäivästä alkaen [paikoin] [sadetta], huomenna iltapäivästä alkaen poutaa
	  34, // 425. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 426. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 427. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 428. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 429. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  34, // 430. Tänään iltapäivästä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  32, // 431. Tänään iltapäivästä alkaen [paikoin] [sadetta>
	  32, // 432. Tänään iltapäivästä alkaen [paikoin] [sadetta>
	  16, // 433. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 434. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 435. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 436. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 437. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 438. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 439. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 440. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  17, // 441. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  17, // 442. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  19, // 443. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  19, // 444. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  18, // 445. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 446. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 447. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 448. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  21, // 449. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 450. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 451. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 452. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 453. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 454. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  20, // 455. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  20, // 456. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  16, // 457. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 458. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 459. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 460. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 461. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 462. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 463. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 464. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  17, // 465. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  17, // 466. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  19, // 467. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  19, // 468. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  18, // 469. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 470. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 471. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 472. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  21, // 473. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 474. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 475. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 476. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 477. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 478. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  20, // 479. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  20, // 480. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  16, // 481. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 482. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 483. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 484. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 485. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 486. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 487. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 488. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  17, // 489. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  17, // 490. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  19, // 491. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  17, // 492. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  18, // 493. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 494. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 495. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 496. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  21, // 497. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 498. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 499. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 500. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 501. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 502. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  20, // 503. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  20, // 504. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  16, // 505. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 506. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 507. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 508. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 509. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 510. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 511. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  16, // 512. Tänään [enimmäkseen selkeää], illalla ja yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  17, // 513. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  17, // 514. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  19, // 515. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  19, // 516. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  18, // 517. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 518. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 519. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  18, // 520. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  21, // 521. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 522. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 523. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 524. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 525. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  21, // 526. Tänään [enimmäkseen selkeää], illasta alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  20, // 527. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  20, // 528. Tänään [enimmäkseen selkeää], illasta alkaen [paikoin] [sadetta]
	  27, // 529. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 530. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 531. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 532. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 533. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 534. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 535. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 536. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  22, // 537. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  22, // 538. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  24, // 539. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  24, // 540. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  23, // 541. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  23, // 542. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  23, // 543. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  23, // 544. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  26, // 545. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 546. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 547. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 548. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 549. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 550. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  25, // 551. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]
	  25, // 552. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]
	  27, // 553. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 554. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 555. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 556. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 557. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 558. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 559. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  27, // 560. Tänään [enimmäkseen selkeää], yöllä [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
	  22, // 561. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  22, // 562. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna aamupäivästä alkaen poutaa, [enimmäkseen selkeää]
	  24, // 563. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  24, // 564. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna keskipäivästä alkaen poutaa, [enimmäkseen selkeää]
	  23, // 565. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  23, // 566. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  23, // 567. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  23, // 568. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]. Huomenna iltapäivästä alkaen poutaa
	  26, // 569. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 570. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 571. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 572. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 573. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  26, // 574. Tänään [enimmäkseen selkeää], iltayöstä alkaen tiistai-iltaan asti [paikoin] [sadetta]
	  25, // 575. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]
	  25  // 576. Tänään [enimmäkseen selkeää], iltayöstä alkaen [paikoin] [sadetta]
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
   * \param theStartHour The start hour of the rain
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
   * \param theStartHour The start hour of the rain
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
   * \param theStartHour The start hour of the rain
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
   * \param theStartHour The start hour of the rain
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
		// [Aamulla] [paikoin] [sadetta], [aamulla] [enimmäkseen selkeää] ja poutaa
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
		// [Enimmäkseen selkeää], [aamulla] [paikoin] [sadetta]
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
		// Tänään [aamusta alkaen] [paikoin] [sadetta]
	  case 1:
		{
		  s1 << PeriodPhraseFactory::create("today", theVar, theForecastTime, day1)
			 << phrase1
			 << places_phrase(theSources,theArea,theRainPeriod,theVar,theDay)
			 << "sadetta";
		  break;
		}
		// Tänään [aamusta alkaen] [paikoin] [sadetta], huomenna [aamusta alkaen] poutaa
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
		// Tänään [aamusta alkaen] [paikoin] [sadetta], huomenna [aamusta alkaen] poutaa, [enimmäkseen selkeää]
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
		// Tänään [aamusta alkaen] [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
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
		// Tänään [aamusta alkaen] tiistai-aamuun asti [paikoin] [sadetta], tiistaina [enimmäkseen selkeää]
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
		// Tänään [aamusta alkaen] tiistai-iltaan asti [paikoin] [sadetta]
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
		// Tänään [enimmäkseen selkeää], [aamulla] ja [yöllä] [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
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
		// Tänään [enimmäkseen selkeää], [aamusta alkaen] [paikoin] [sadetta]
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
		// Tänään [enimmäkseen selkeää], [aamusta alkaen] [paikoin] [sadetta]. Huomenna [aamusta alkaen] poutaa
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
		// Tänään [enimmäkseen selkeää], [aamusta alkaen] [paikoin] [sadetta]. Huomenna [aamusta alkaen] poutaa, [enimmäkseen selkeää]
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
		// Tänään [enimmäkseen selkeää], [aamusta alkaen] tiistai-iltaan asti [paikoin] [sadetta]
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
		// Tänään [enimmäkseen selkeää], [aamulla] [paikoin] [sadetta]. Huomenna [enimmäkseen selkeää]
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
		// Tänään [paikoin] [sadetta], huomenna [aamulla] poutaa
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
		// Tänään [paikoin] [sadetta], huomenna [aamusta alkaen] poutaa
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
		// Tänään [paikoin] [sadetta], huomenna [aamusta alkaen] poutaa, [enimmäkseen selkeää]
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
		// Tänään [paikoin] [sadetta], huomenna [enimmäkseen selkeää]
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
		// Tänään ja huomenna [paikoin] [sadetta]
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
