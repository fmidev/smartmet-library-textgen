// ======================================================================
/*!
 * \file
 * \brief Interface of abstract class TextGen::Dictionary
 */
// ======================================================================

#pragma once

#include <calculator/TextGenError.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace TextGen
{
class Dictionary
{
 public:
  using size_type = unsigned long;

  // Compiler generated:
  //
  // Dictionary();
  // Dictionary(const Dictionary & theDict);
  // Dictionary & operator=(const Dictionary & theDict);

  virtual ~Dictionary() {}
  virtual void init(const std::string& theLanguage) = 0;
  virtual void changeLanguage(const std::string& theLanguage){};
  virtual const std::string& language() const = 0;
  virtual bool contains(const std::string& theKey) const = 0;
  virtual const std::string& find(const std::string& theKey) const = 0;
  virtual void insert(const std::string& theKey, const std::string& thePhrase) = 0;

  virtual void geoinit(void* theReactor) {}
  virtual bool geocontains(const std::string& theKey) const { return false; }
  virtual bool geocontains(const double& theLongitude,
                           const double& theLatitude,
                           const double& theMaxDistance) const
  {
    return false;
  }
  virtual std::string geofind(const std::string& theKey) const
  {
    throw TextGenError("Error: Dictionary::geofind(" + theKey + ") failed");
  }
  virtual std::string geofind(double theLongitude, double theLatitude, double theMaxDistance) const
  {
    std::stringstream ss;
    ss << "Error: Dictionary::geofind(" << theLongitude << ", " << theLatitude << ", "
       << theMaxDistance << ") failed";

    throw TextGenError(ss.str());
  }

  virtual size_type size() const = 0;
  virtual bool empty() const = 0;

  std::string getDictionaryId() const { return itsDictionaryId; }

 protected:
  std::string itsDictionaryId;

};  // class Dictionary

}  // namespace TextGen

// ======================================================================
