// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::DictionaryFactory
 */
// ======================================================================
#ifdef _MSC_VER
#pragma warning(disable : 4786) // poistaa n kpl VC++ k��nt�j�n varoitusta (liian pitk� nimi >255 merkki� joka johtuu 'puretuista' STL-template nimist�)
#endif

#include "DictionaryFactory.h"
#include "NullDictionary.h"
#include "BasicDictionary.h"
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
	if(theType == "mysql")
	  return shared_ptr<Dictionary>(new MySQLDictionary());
	if(theType == "multimysql")
	  return shared_ptr<Dictionary>(new MySQLDictionaries());
	  
	throw TextGenError("Error: Unknown dictionary type "+theType);
  }
 
} // namespace TextGen

// ======================================================================
