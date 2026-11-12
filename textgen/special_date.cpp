// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::SpecialStory::date
 */
// ======================================================================

#include "MessageLogger.h"
#include "Paragraph.h"
#include "SpecialStory.h"
#include "TimePeriod.h"
#include <boost/locale.hpp>
#include <calculator/WeatherPeriod.h>
#include <macgyver/Exception.h>
#include <string>

using namespace TextGen;
using namespace std;
using namespace boost::locale::as;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on text
 *
 * Variables:
 *
 *     value = "Text to be inserted into paragraph as is."
 *     value = @filename
 *     value = @filename.php
 *
 * Product specific variables are possible:
 *
 *     value::en_html = "<underline>Text to be underlined.</underline>"
 *
 * \return The story
 */
// ----------------------------------------------------------------------

Paragraph SpecialStory::date() const
{
  try
  {
    MessageLogger log("SpecialStory::date");

    Paragraph paragraph;

    paragraph << TimePeriod(itsPeriod);

    return paragraph;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
