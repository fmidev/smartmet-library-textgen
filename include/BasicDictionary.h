// ======================================================================
/*!
 * \file
 * \brief Interface of the BasicDictionary class
 */
// ======================================================================
/*!
 * \class textgen::BasicDictionary
 *
 * \brief Provides dictionary services
 *
 * The responsibility of the BasicDictionary class is to provide natural
 * language text for the given keyword, and to accept inserting
 * new keyword-text pairs.
 *
 * The basic dictionary does not have a real initialization method.
 * Instead, the user is expected to fill the dictionary by using
 * the insert command.
 *
 * Sample usage:
 * \code
 * using namespace textgen;
 *
 * BasicDictionary finnish;
 * finnish.insert("good morning","huomenta");
 * finnish.insert("good night","hyvää yötä");
 *
 * cout << finnish.find("good morning") << endl;
 * cout << finnish.find("good night") << endl;
 *
 * if(finnish.contains("good night"))
 *    cout << finnish.find("good night") << endl;
 * \endcode
 *
 * Note that find throws if the given keyword does not exist.
 *
 */
// ----------------------------------------------------------------------

#ifndef TEXTGEN_BASICDICTIONARY_H
#define TEXTGEN_BASICDICTIONARY_H

#include "Dictionary.h"

#include <map>
#include <string>

namespace textgen
{
  class BasicDictionary : public Dictionary
  {
  public:

	// Compiler generated:
	//
	// BasicDictionary();
	// BasicDictionary(const BasicDictionary & theDict);
	// BasicDictionary & operator=(const BasicDictionary & theDict);

	virtual ~BasicDictionary() { }
	virtual void init(const std::string & theLanguage) { }
	virtual bool contains(const std::string & theKey) const;
	virtual const std::string & find(const std::string & theKey) const;
	virtual void insert(const std::string & theKey, const std::string & thePhrase);

  private:

	typedef std::map<std::string,std::string> StorageType;
	StorageType itsData;

  }; // class BasicDictionary

} // namespace textgen

#endif // TEXTGEN_BASICDICTIONARY_H

// ======================================================================
