// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGenError
 */
// ======================================================================
/*!
 * \class textgen::TextGenError
 *
 * \brief The generic exception thrown by textgen library
 *
 */
// ======================================================================

#ifndef TEXTGEN_TEXTGENERROR_H
#define TEXTGEN_TEXTGENERROR_H

#include <stdexcept>
#include <string>

namespace textgen
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
} // namespace textgen

#endif // TEXTGEN_TEXTGENERROR_H

// ======================================================================
