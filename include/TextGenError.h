// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGenError
 */
// ======================================================================
/*!
 * \class TextGen::TextGenError
 *
 * \brief The generic exception thrown by TextGen library
 *
 */
// ======================================================================

#ifndef TEXTGEN_TEXTGENERROR_H
#define TEXTGEN_TEXTGENERROR_H

#include <stdexcept>
#include <string>

namespace TextGen
{
  class TextGenError : public std::exception
  {
  public:
	~TextGenError() throw() { }
	TextGenError(const std::string & theError);
	virtual const char * what(void) const throw();
  private:
	std::string itsError;

  }; // class TextGenError
} // namespace TextGen

#endif // TEXTGEN_TEXTGENERROR_H

// ======================================================================
