// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PoDictionary
 */
// ======================================================================
/*!
 * \class TextGen::PoDictionary
 *
 * \brief Provides dictionary services from a gettext .po file
 *
 * The responsibility of the PoDictionary class is to provide natural
 * language text for the given keyword, loaded from a single gettext
 * .po file per language.
 *
 * Sample usage:
 * \code
 * using namespace TextGen;
 *
 * PoDictionary english;
 * english.init("en");
 *
 * cout << english.find("1-aamusta") << '\n';
 * \endcode
 *
 * Note that find throws if the given keyword does not exist.
 *
 * The .po file location is read from textgen::podictionaries, which must
 * name a directory containing `<language>.po` files.
 *
 * The dictionary can be initialized multiple times. Each init erases the
 * language initialized earlier.
 */
// ----------------------------------------------------------------------

#include "PoDictionary.h"
#include <calculator/Settings.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiFileSystem.h>

#include <fstream>
#include <map>

namespace TextGen
{
namespace
{
// ----------------------------------------------------------------------
/*!
 * \brief Strip a trailing \r left over from CRLF files
 */
// ----------------------------------------------------------------------
void rstrip_cr(std::string& line)
{
  if (!line.empty() && line.back() == '\r')
    line.pop_back();
}

// ----------------------------------------------------------------------
/*!
 * \brief Unquote and unescape a single-line gettext string literal
 *
 * Expects a string that starts and ends with a double quote. Recognized
 * escape sequences: \\, \", \n, \t, \r. Unknown escapes are preserved
 * verbatim (minus the backslash).
 */
// ----------------------------------------------------------------------
std::string po_unquote(const std::string& theLine, const std::string& theFile, int theLineno)
{
  if (theLine.size() < 2 || theLine.front() != '"' || theLine.back() != '"')
    throw Fmi::Exception(
        BCP, theFile + ":" + std::to_string(theLineno) + ": expected a quoted string");

  std::string result;
  result.reserve(theLine.size() - 2);
  for (std::size_t i = 1; i + 1 < theLine.size(); ++i)
  {
    char c = theLine[i];
    if (c != '\\')
    {
      result += c;
      continue;
    }
    if (i + 2 >= theLine.size())
      throw Fmi::Exception(BCP,
                           theFile + ":" + std::to_string(theLineno) +
                               ": dangling backslash in quoted string");
    char next = theLine[++i];
    switch (next)
    {
      case 'n':
        result += '\n';
        break;
      case 't':
        result += '\t';
        break;
      case 'r':
        result += '\r';
        break;
      case '\\':
        result += '\\';
        break;
      case '"':
        result += '"';
        break;
      default:
        result += next;
        break;
    }
  }
  return result;
}

constexpr bool starts_with(const std::string& s, const char* prefix)
{
  return s.compare(0, std::char_traits<char>::length(prefix), prefix) == 0;
}

}  // namespace

// ----------------------------------------------------------------------
/*!
 * \brief Implementation hiding pimple
 */
// ----------------------------------------------------------------------

class PoDictionary::Pimple
{
 public:
  Pimple() = default;
  using StorageType = std::map<std::string, std::string>;
  using value_type = StorageType::value_type;

  bool itsInitialized{false};
  std::string itsLanguage;
  StorageType itsData;

};  // class Pimple

PoDictionary::~PoDictionary() = default;

PoDictionary::PoDictionary() : itsPimple(new Pimple())
{
  try
  {
    itsDictionaryId = "po";
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

const std::string& PoDictionary::language() const
{
  try
  {
    return itsPimple->itsLanguage;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Initialize with given language
 *
 * Loads `<textgen::podictionaries>/<theLanguage>.po` into memory. Any
 * previously loaded language is discarded.
 */
// ----------------------------------------------------------------------

void PoDictionary::init(const std::string& theLanguage)
{
  try
  {
    itsPimple->itsLanguage = theLanguage;
    itsPimple->itsInitialized = false;
    itsPimple->itsData.clear();

    std::string database = Settings::require_string("textgen::podictionaries");
    std::string filename = database + '/' + theLanguage + ".po";

    if (!NFmiFileSystem::FileExists(filename))
      throw Fmi::Exception(BCP, "Error: Could not find dictionary '" + filename + "'");

    std::ifstream in(filename.c_str());
    if (!in)
      throw Fmi::Exception(BCP, "Error: Could not open dictionary '" + filename + "' for reading");

    // A gettext .po file is a sequence of entries separated by blank lines.
    // Each entry has a msgid and a msgstr, either of which may be spread
    // across multiple "..." continuation lines. Comment lines start with '#'.
    // The header entry has an empty msgid and is skipped here.

    enum class State
    {
      None,
      Msgid,
      Msgstr
    } state = State::None;
    std::string msgid;
    std::string msgstr;

    auto commit = [&]()
    {
      if (state != State::None && !msgid.empty())
        itsPimple->itsData.insert(Pimple::value_type(msgid, msgstr));
      msgid.clear();
      msgstr.clear();
      state = State::None;
    };

    std::string line;
    int lineno = 0;
    while (std::getline(in, line))
    {
      ++lineno;
      rstrip_cr(line);

      if (line.empty())
      {
        commit();
        continue;
      }
      if (line[0] == '#')
        continue;

      if (starts_with(line, "msgid \""))
      {
        commit();
        msgid = po_unquote(line.substr(6), filename, lineno);
        state = State::Msgid;
      }
      else if (starts_with(line, "msgstr \""))
      {
        msgstr = po_unquote(line.substr(7), filename, lineno);
        state = State::Msgstr;
      }
      else if (line[0] == '"')
      {
        std::string piece = po_unquote(line, filename, lineno);
        if (state == State::Msgid)
          msgid += piece;
        else if (state == State::Msgstr)
          msgstr += piece;
        else
          throw Fmi::Exception(BCP,
                               filename + ":" + std::to_string(lineno) +
                                   ": continuation string outside msgid/msgstr");
      }
      else
      {
        throw Fmi::Exception(
            BCP, filename + ":" + std::to_string(lineno) + ": unexpected line '" + line + "'");
      }
    }
    commit();

    itsPimple->itsInitialized = true;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("language", theLanguage);
  }
}

bool PoDictionary::contains(const std::string& theKey) const
{
  try
  {
    return (itsPimple->itsData.find(theKey) != itsPimple->itsData.end());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("key", theKey);
  }
}

std::string PoDictionary::find(const std::string& theKey) const
{
  try
  {
    if (!itsPimple->itsInitialized)
      throw Fmi::Exception(BCP, "Error: PoDictionary::find() called before init()");
    auto it = itsPimple->itsData.find(theKey);

    if (it != itsPimple->itsData.end())
      return it->second;
    throw Fmi::Exception(
        BCP,
        "Error: PoDictionary::find(" + theKey + ") failed in language " + itsPimple->itsLanguage);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("key", theKey);
  }
}

void PoDictionary::insert(const std::string& /*theKey*/, const std::string& /*thePhrase*/)
{
  try
  {
    throw Fmi::Exception(BCP, "Error: PoDictionary::insert() is not allowed");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

PoDictionary::size_type PoDictionary::size() const
{
  try
  {
    return itsPimple->itsData.size();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

bool PoDictionary::empty() const
{
  try
  {
    return itsPimple->itsData.empty();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}

void PoDictionary::changeLanguage(const std::string& theLanguage)
{
  try
  {
    init(theLanguage);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("language", theLanguage);
  }
}

}  // namespace TextGen

// ======================================================================
