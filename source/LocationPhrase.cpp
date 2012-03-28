// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::LocationPhrase
 */
// ======================================================================
/*!
 * \class TextGen::LocationPhrase
 * \brief Representation of a location name
 */
// ======================================================================

#include "LocationPhrase.h"
#include "Dictionary.h"
#include "TextFormatter.h"
#include "TextFormatterTools.h"
#include <newbase/NFmiStringTools.h>
#include <boost/locale.hpp>

using namespace std;
using namespace boost;

namespace TextGen
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  LocationPhrase::~LocationPhrase()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theLocation The location name
   */
  // ----------------------------------------------------------------------

  LocationPhrase::LocationPhrase(const std::string & theLocation)
	: itsLocation(theLocation)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   */
  // ----------------------------------------------------------------------

  shared_ptr<Glyph> LocationPhrase::clone() const
  {
	shared_ptr<Glyph> ret(new LocationPhrase(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using the given dictionary
   *
   * If there is no translation, the location name is returned back
   * properly capitalized. That includes capitalizing each word
   * in the location name separated either by spaces or by hyphens.
   *
   * \param theDictionary The dictionary to realize with
   * \return The realized string
   */
  // ----------------------------------------------------------------------

  std::string LocationPhrase::realize(const Dictionary & theDictionary) const
  {
	using namespace boost::locale;
	using namespace boost::locale::boundary;

	if(theDictionary.contains(itsLocation))
	  return theDictionary.find(itsLocation);

	generator gen;
	return to_title(itsLocation);

#if 0
	// Capitalize space-separated words
	vector<string> words = NFmiStringTools::Split(itsLocation," ");

	for_each(words.begin(),words.end(),TextFormatterTools::capitalize);
	string name = NFmiStringTools::Join(words," ");

	// Capitalize hyphen-separated words
	words = NFmiStringTools::Split(name,"-");
	for_each(words.begin(),words.end(),TextFormatterTools::capitalize);
	name = NFmiStringTools::Join(words,"-");
	return name;
#endif

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Realize using the given text formatter
   *
   * \param theFormatter The formatter
   * \return The formatter string
   */
  // ----------------------------------------------------------------------

  std::string LocationPhrase::realize(const TextFormatter & theFormatter) const
  {
	return theFormatter.visit(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Returns false since location phrase is not a separator
   */
  // ----------------------------------------------------------------------

  bool LocationPhrase::isDelimiter() const
  {
	return false;
  }

} // namespace TextGen

// ======================================================================
