#include "SingleLineTextFormatter.h"
#include "Document.h"
#include "TextFormatterTools.h"

using namespace std;
using namespace boost;

namespace TextGen
{
string SingleLineTextFormatter::visit(const Document& theDocument) const
{
  string ret = TextFormatterTools::realize(theDocument.begin(), theDocument.end(), *this, " ", "");
  ret += '\n';
  return ret;
}
}  // namespace TextGen
