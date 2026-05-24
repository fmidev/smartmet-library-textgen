// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGen::GlyphContainer
 */
// ======================================================================
/*!
 * \class TextGen::GlyphContainer
 *
 * \brief A generic text glyph interface
 *
 */
// ======================================================================

#include "GlyphContainer.h"

#include <macgyver/Exception.h>

using namespace std;

namespace TextGen
{

GlyphContainer::~GlyphContainer() = default;

// ----------------------------------------------------------------------
/*!
 * \brief Return the size of the container
 *
 * \return The size (number of non-recursive glyphs)
 */
// ----------------------------------------------------------------------

GlyphContainer::size_type GlyphContainer::size() const
{
  return itsData.size();
}
// ----------------------------------------------------------------------
/*!
 * \brief Test if the container is empty
 *
 * \return True if the container is empty
 */
// ----------------------------------------------------------------------

bool GlyphContainer::empty() const
{
  return itsData.empty();
}
// ----------------------------------------------------------------------
/*!
 * \brief Clear the container
 */
// ----------------------------------------------------------------------

void GlyphContainer::clear()
{
  try
  {
    itsData.clear();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Append a new glyph to the container
 *
 * \param theGlyph The glyph to append
 */
// ----------------------------------------------------------------------

void GlyphContainer::push_back(const Glyph& theGlyph)
{
  try
  {
    itsData.push_back(theGlyph.clone());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Append a new glyph to the container
 *
 * \param theGlyph The glyph to append
 */
// ----------------------------------------------------------------------

void GlyphContainer::push_back(const_reference theGlyph)
{
  try
  {
    itsData.push_back(theGlyph);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Insert a new glyph to the container
 *
 * \param theGlyph The glyph to insert
 */
// ----------------------------------------------------------------------

void GlyphContainer::push_front(const Glyph& theGlyph)
{
  try
  {
    itsData.push_front(theGlyph.clone());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Insert a new glyph to the container
 *
 * \param theGlyph The glyph to insert
 */
// ----------------------------------------------------------------------

void GlyphContainer::push_front(const_reference theGlyph)
{
  try
  {
    itsData.push_front(theGlyph);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the begin iterator (const)
 *
 * \return The begin iterator
 */
// ----------------------------------------------------------------------

GlyphContainer::const_iterator GlyphContainer::begin() const
{
  try
  {
    return itsData.begin();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the end iterator (const)
 *
 * \return The end iterator
 */
// ----------------------------------------------------------------------

GlyphContainer::const_iterator GlyphContainer::end() const
{
  try
  {
    return itsData.end();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the begin iterator (non-const)
 *
 * \return The begin iterator
 */
// ----------------------------------------------------------------------

GlyphContainer::iterator GlyphContainer::begin()
{
  try
  {
    return itsData.begin();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the end iterator (non-const)
 *
 * \return The end iterator
 */
// ----------------------------------------------------------------------

GlyphContainer::iterator GlyphContainer::end()
{
  try
  {
    return itsData.end();
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed");
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the front element
 *
 * \return The front element
 */
// ----------------------------------------------------------------------

GlyphContainer::const_reference GlyphContainer::front() const
{
  return itsData.front();
}
// ----------------------------------------------------------------------
/*!
 * \brief Return the back element
 *
 * \return The back element
 */
// ----------------------------------------------------------------------

GlyphContainer::const_reference GlyphContainer::back() const
{
  return itsData.back();
}
}  // namespace TextGen

// ======================================================================
