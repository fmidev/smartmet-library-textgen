// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::Header
 */
// ======================================================================
/*!
 * \class TextGen::Header
 * \brief Representation of a header.
 *
 * The responsibility of the Header class is to store a sequence
 * of phrases and provide means for translating it by using a
 * dictionary.
 *
 */
// ======================================================================

#ifndef TEXTGEN_HEADER_H
#define TEXTGEN_HEADER_H

#include "Phrase.h"
#include <memory>
#include <string>

namespace TextGen
{
  class Dictionary;

  class Header
  {
  public:

	~Header();
	Header();
	Header(const Header & theHeader);
	Header(const Phrase & thePhrase);
	Header(const std::string & thePhrase);
	Header(int theValue);
	Header & operator=(const Header & theHeader);

	void swap(Header & theHeader);

	Header & operator<<(const Header & theHeader);
	Header & operator<<(const Phrase & thePhrase);
	Header & operator<<(const std::string & thePhrase);
	Header & operator<<(int theValue);

	std::string realize() const;
	std::string realize(const Dictionary & theDictionary) const;

	bool empty() const;
	size_t size() const;

  private:

	class Pimple;
	std::auto_ptr<Pimple> itsPimple;

  }; // class Header

  // Free operators

  void swap(Header & theLhs, Header & theRhs);


} // namespace TextGen

#endif // TEXTGEN_HEADER_H

// ======================================================================
