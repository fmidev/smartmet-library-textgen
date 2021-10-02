// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::GlyphContainer
 */
// ======================================================================

#pragma once

#include "Glyph.h"
#include <list>

namespace TextGen
{
class GlyphContainer : public Glyph
{
 public:
  ~GlyphContainer() override {}
  GlyphContainer() {}
  boost::shared_ptr<Glyph> clone() const override = 0;
  std::string realize(const Dictionary& theDictionary) const override = 0;
  std::string realize(const TextFormatter& theFormatter) const override = 0;
  bool isDelimiter() const override = 0;

  typedef boost::shared_ptr<Glyph> value_type;
  typedef const value_type& const_reference;
  typedef value_type& reference;
  typedef std::list<value_type> storage_type;
  typedef storage_type::size_type size_type;
  typedef storage_type::difference_type difference_type;
  typedef storage_type::const_iterator const_iterator;
  typedef storage_type::iterator iterator;

  size_type size() const;
  bool empty() const;
  void clear();

  void push_back(const Glyph& theGlyph);
  void push_back(const_reference theGlyph);
  void push_front(const Glyph& theGlyph);
  void push_front(const_reference theGlyph);

  const_iterator begin() const;
  const_iterator end() const;

  iterator begin();
  iterator end();

  const_reference front() const;
  const_reference back() const;

 protected:
  storage_type itsData;

};  // class GlyphContainer
}  // namespace TextGen

// ======================================================================
