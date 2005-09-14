// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::StoryTag
 */
// ======================================================================
/*!
 * \class TextGen::StoryTag
 * \brief Represents a non-realizable story name tag for formatter
 */
// ======================================================================

#include "StoryTag.h"
#include "Dictionary.h"
#include "TextFormatter.h"
#include "TextGenError.h"

#include <boost/shared_ptr.hpp>

#include <sstream>

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  StoryTag::~StoryTag()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  StoryTag::StoryTag(const std::string & theName)
	: itsName(theName)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return a clone
   */
  // ----------------------------------------------------------------------

  shared_ptr<Glyph> StoryTag::clone() const
  {
	shared_ptr<Glyph> ret(new StoryTag(*this));
	return ret;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the StoryTag
   *
   * \param theDictionary The dictionary to be used
   * \return The text
   */
  // ----------------------------------------------------------------------

  const std::string StoryTag::realize(const Dictionary & theDictionary) const
  {
	return itsName;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the text for the number
   *
   * \param theFormatter The formatter
   * \return The text
   */
  // ----------------------------------------------------------------------

  const std::string StoryTag::realize(const TextFormatter & theFormatter) const
  {
	return theFormatter.visit(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Returns false since StoryTag is not a separator
   */
  // ----------------------------------------------------------------------

  bool StoryTag::isDelimiter() const
  {
	return false;
  }

} // namespace TextGen

// ======================================================================
