// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::DictionaryFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::DictionaryFactory
 *
 * \brief Dictionary creation services
 *
 * The responsibility of the DictionaryFactory namespace is to provide
 * dictionary creation services.
 *
 * For example,
 * \code
 * boost::shared_ptr<Dictionary> dict1 = DictionaryFactory::create("null");
 * boost::shared_ptr<Dictionary> dict2 = DictionaryFactory::create("basic");
 * boost::shared_ptr<Dictionary> dict3 = DictionaryFactory::create("mysql");
 * boost::shared_ptr<Dictionary> dict3 = DictionaryFactory::create("file");
 * \endcode
 *
 */
// ======================================================================

#include "DictionaryFactory.h"
#include "NullDictionary.h"
#include "BasicDictionary.h"
#include "FileDictionary.h"
#include "MySQLDictionary.h"
#include "MySQLDictionaries.h"
#include "TextGenError.h"

using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Create a dictionary of given type
   *
   * \param theType The type of the dictionary (null|basic|mysql)
   * \return An uninitialized dictionary
   */
  // ----------------------------------------------------------------------

  shared_ptr<Dictionary> DictionaryFactory::create(const std::string & theType)
  {
	if(theType == "null")
	  return shared_ptr<Dictionary>(new NullDictionary());
	if(theType == "basic")
	  return shared_ptr<Dictionary>(new BasicDictionary());
	if(theType == "file")
	  return shared_ptr<Dictionary>(new FileDictionary());
	if(theType == "mysql")
	  return shared_ptr<Dictionary>(new MySQLDictionary());
	if(theType == "multimysql")
	  return shared_ptr<Dictionary>(new MySQLDictionaries());
	  
	throw TextGenError("Error: Unknown dictionary type "+theType);
  }
 
} // namespace TextGen

// ======================================================================
