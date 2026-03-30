// ======================================================================
/*!
 * \file
 * \brief Implementation of method TextGen::SpecialStory::none
 */
// ======================================================================

#include "MessageLogger.h"
#include "Paragraph.h"
#include "SpecialStory.h"
#include <macgyver/Exception.h>

using namespace TextGen;
using namespace std;

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Generate story on none
 *
 * \return The story
 */
// ----------------------------------------------------------------------

Paragraph SpecialStory::none() const
{
  try
  {
    MessageLogger log("SpecialStory::none");

    Paragraph paragraph;
    return paragraph;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

}  // namespace TextGen

// ======================================================================
