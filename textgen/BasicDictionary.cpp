// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::BasicDictionary
 */
// ======================================================================

#include "BasicDictionary.h"
#include <macgyver/Exception.h>

namespace TextGen
{
// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 *
 * The destructor does nothing special.
 */
// ----------------------------------------------------------------------

BasicDictionary::~BasicDictionary() = default;
// ----------------------------------------------------------------------
/*!
 * \brief Initialize with given language
 *
 * Initializing BasicDictionary does nothing, the method is defined
 * only to satisfy the abstract Dictionary interface. It is expected
 * that the user fills the BasicDictionary by himself using the insert
 * method.
 *
 * The method is not inlined since the due to polymorphic usage
 * the compiler is unlikely to see the empty implementation.
 */
// ----------------------------------------------------------------------

void BasicDictionary::init(const std::string& theLanguage)
{
  try
  {
    itsLanguage = theLanguage;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("language", theLanguage);
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the language
 *
 * An empty string is returned if no language has been initialized.
 *
 * \return The language
 */
// ----------------------------------------------------------------------

const std::string& BasicDictionary::language() const
{
  try
  {
    return itsLanguage;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Test if the given phrase is in the dictionary
 *
 * \param theKey The key of the phrase
 * \return True if the phrase is in the dictionary.
 */
// ----------------------------------------------------------------------

bool BasicDictionary::contains(const std::string& theKey) const
{
  try
  {
    return (itsData.find(theKey) != itsData.end());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("key", theKey);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Return the phrase for the given key.
 *
 * This will throw a TextGenError if the phrase is not in
 * the dictionary.
 *
 * \param theKey The key of the phrase
 * \return The phrase
 */
// ----------------------------------------------------------------------

std::string BasicDictionary::find(const std::string& theKey) const
{
  try
  {
    auto it = itsData.find(theKey);
    if (it != itsData.end())
      return it->second;
    throw Fmi::Exception(BCP, "Dictionary does not contain phrase '" + theKey + "'");
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("key", theKey);
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Insert a new phrase into the dictionary
 *
 * Note that nothing will occur if the phrase key already exists in
 * the dictionary. The user is expected to use contains before
 * insert if such checking is desired.
 *
 * \param theKey The key of the phrase
 * \param thePhrase The phrase
 */
// ----------------------------------------------------------------------

void BasicDictionary::insert(const std::string& theKey, const std::string& thePhrase)
{
  try
  {
    itsData.insert(StorageType::value_type(theKey, thePhrase));
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed").addParameter("key", theKey).addParameter("phrase", thePhrase);
  }
}

// ----------------------------------------------------------------------
/*!
 * Return the size of the dictionary
 *
 * \return The size of the dictionary
 */
// ----------------------------------------------------------------------

BasicDictionary::size_type BasicDictionary::size() const
{
  try
  {
    return itsData.size();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * Test if the dictionary is empty
 *
 * \return True if the dictionary is empty
 */
// ----------------------------------------------------------------------

bool BasicDictionary::empty() const
{
  try
  {
    return itsData.empty();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
}  // namespace TextGen

// ======================================================================
