// ======================================================================
/*!
 * \file
 * \brief Implementation of class TextGenError
 */
// ======================================================================

#include "TextGenError.h"

namespace TextGen
{
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theError The error message
   */
  // ----------------------------------------------------------------------

  TextGenError::TextGenError(const std::string & theError)
	: itsError(theError)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Returns the error message
   *
   * \return The error message
   */
  // ----------------------------------------------------------------------

  const char * TextGenError::what(void) const throw()
  {
	return itsError.c_str();
  }

} // namespace TextGen

// ======================================================================
