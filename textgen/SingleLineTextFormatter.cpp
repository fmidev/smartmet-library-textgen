#include "SingleLineTextFormatter.h"
#include "Document.h"
#include "TextFormatterTools.h"
#include <macgyver/Exception.h>

using namespace std;

namespace TextGen
{
string SingleLineTextFormatter::visit(const Document& theDocument) const
{
  try
  {
    string ret = TextFormatterTools::realize(theDocument.begin(), theDocument.end(), *this, " ", "");
    ret += '\n';
    return ret;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen
