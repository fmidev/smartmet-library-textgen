// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace DictionaryFactory
 */
// ======================================================================

#include "DictionaryFactory.h"
#include "NullDictionary.h"
#include "BasicDictionary.h"
#include "MySQLDictionary.h"
#include "TextGenError.h"

namespace textgen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Create a dictionary of given type
   *
   * \param theType The type of the dictionary (null|basic|mysql)
   * \return An uninitialized dictionary
   */
  // ----------------------------------------------------------------------

  std::auto_ptr<Dictionary> DictionaryFactory::create(const std::string & theType)
  {
	if(theType == "null")
	  return std::auto_ptr<Dictionary>(new NullDictionary());
	if(theType == "basic")
	  return std::auto_ptr<Dictionary>(new BasicDictionary());
	if(theType == "mysql")
	  return std::auto_ptr<Dictionary>(new MySQLDictionary());
	throw TextGenError("Error: Unknown dictionary type "+theType);
  }
 
} // namespace textgen

// ======================================================================
