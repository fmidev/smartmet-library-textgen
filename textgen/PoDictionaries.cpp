// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::PoDictionaries
 */
// ======================================================================
/*!
 * \class TextGen::PoDictionaries
 *
 * \brief Multi-language PO dictionary cache
 *
 * PoDictionaries is the multi-language version of PoDictionary: every call
 * to init() loads a new language from its .po file and caches it, and
 * changeLanguage() switches the active dictionary among the already-loaded
 * languages.
 *
 * Sample usage:
 * \code
 * using namespace TextGen;
 *
 * PoDictionaries dict;
 * dict.init("fi");
 * dict.init("en");
 *
 * cout << dict.find("good morning") << '\n';
 * dict.changeLanguage("fi");
 * cout << dict.find("good morning") << '\n';
 * \endcode
 *
 * Note that find throws if the given keyword does not exist.
 *
 * The .po directory is read from textgen::podictionaries via NFmiSettings.
 */
// ----------------------------------------------------------------------

#include "PoDictionaries.h"
#ifdef UNIX
#include "PoDictionary.h"
#include <macgyver/Exception.h>
#include <map>
#include <memory>

using namespace std;

namespace TextGen
{
class PoDictionaries::Pimple
{
 public:
  using storage_type = map<string, std::shared_ptr<PoDictionary>>;

  storage_type itsData;
  string itsLanguage;
  bool itsInitialized{false};

  storage_type::const_iterator itsCurrentDictionary;

};  // class Pimple

PoDictionaries::~PoDictionaries() = default;

PoDictionaries::PoDictionaries() : itsPimple(new Pimple())
{
  itsDictionaryId = "multipo";
}

const std::string& PoDictionaries::language() const
{
  return itsPimple->itsLanguage;
}

// ----------------------------------------------------------------------
/*!
 * \brief Initialize with given language
 *
 * Languages are loaded lazily and cached; switching back to an already
 * loaded language is essentially free.
 */
// ----------------------------------------------------------------------

void PoDictionaries::init(const std::string& theLanguage)
{
  if (theLanguage == itsPimple->itsLanguage)
    return;

  itsPimple->itsLanguage = theLanguage;

  itsPimple->itsCurrentDictionary = itsPimple->itsData.find(theLanguage);
  if (itsPimple->itsCurrentDictionary != itsPimple->itsData.end())
    return;

  std::shared_ptr<PoDictionary> dict(new PoDictionary);
  if (dict == nullptr)
    throw Fmi::Exception(BCP, "Failed to allocate a new PoDictionary");

  dict->init(theLanguage);

  itsPimple->itsData.insert(Pimple::storage_type::value_type(theLanguage, dict));
  itsPimple->itsCurrentDictionary = itsPimple->itsData.find(theLanguage);

  itsPimple->itsInitialized = true;
}

bool PoDictionaries::contains(const std::string& theKey) const
{
  if (!itsPimple->itsInitialized)
    throw Fmi::Exception(BCP, "Error: PoDictionaries::contains() called before init()");

  return (itsPimple->itsCurrentDictionary->second->contains(theKey));
}

std::string PoDictionaries::find(const std::string& theKey) const
{
  if (!itsPimple->itsInitialized)
    throw Fmi::Exception(BCP, "Error: PoDictionaries::find() called before init()");

  return itsPimple->itsCurrentDictionary->second->find(theKey);
}

void PoDictionaries::insert(const std::string& /*theKey*/, const std::string& /*thePhrase*/)
{
  throw Fmi::Exception(BCP, "Error: PoDictionaries::insert() is not allowed");
}

PoDictionaries::size_type PoDictionaries::size() const
{
  if (!itsPimple->itsInitialized)
    throw Fmi::Exception(BCP, "Error: PoDictionaries::size() called before init()");
  return itsPimple->itsCurrentDictionary->second->size();
}

bool PoDictionaries::empty() const
{
  if (!itsPimple->itsInitialized)
    throw Fmi::Exception(BCP, "Error: PoDictionaries::empty() called before init()");

  return itsPimple->itsCurrentDictionary->second->empty();
}

void PoDictionaries::changeLanguage(const std::string& theLanguage)
{
  if (itsPimple->itsData.find(theLanguage) == itsPimple->itsData.end())
    throw Fmi::Exception(BCP, "Error: The requested language not supported: " + theLanguage);

  itsPimple->itsLanguage = theLanguage;
  itsPimple->itsCurrentDictionary = itsPimple->itsData.find(theLanguage);
}

}  // namespace TextGen

// ======================================================================
#endif  // UNIX
