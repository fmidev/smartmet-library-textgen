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
  ~GlyphContainer() override;
  GlyphContainer() = default;
  std::shared_ptr<Glyph> clone() const override = 0;
  std::string realize(const Dictionary& theDictionary) const override = 0;
  std::string realize(const TextFormatter& theFormatter) const override = 0;
  bool isDelimiter() const override = 0;

  using value_type = std::shared_ptr<Glyph>;
  using const_reference = const value_type&;
  using reference = value_type&;
  using storage_type = std::list<value_type>;
  using size_type = storage_type::size_type;
  using difference_type = storage_type::difference_type;
  using const_iterator = storage_type::const_iterator;
  using iterator = storage_type::iterator;

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
