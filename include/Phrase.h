// ======================================================================
/*!
 * \file
 * \brief Interface of class Phrase
 */
// ======================================================================
/*!
 * \class TextGen::Phrase
 * \brief A phrase is some constituent of a sentence.
 *
 * The Phrase class defines the interface for all constituents
 * of a sentence, for example a Word and a Number.
 *
 */
// ======================================================================

#ifndef IMAGINE_PHRASE_H
#define IMAGINE_PHRASE_H

#include <string>

namespace TextGen
{
  class Dictionary;

  class Phrase
  {
  public:

	virtual ~Phrase() { }
	virtual std::string realize() const = 0;
	virtual std::string realize(const Dictionary & theDictionary) const = 0;

  protected:

	Phrase() { }

  }; // class Phrase

} // namespace TextGen

#endif // IMAGINE_PHRASE_H

// ======================================================================
