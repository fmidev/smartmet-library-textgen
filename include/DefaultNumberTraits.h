// ======================================================================
/*!
 * \file
 * \brief Interface of class TextGen::DefaultNumberTraits
 */
// ======================================================================
/*!
 * \class TextGen::DefaultNumberTraits
 * \brief Default tostring conversions for various numbers
 *
 * The default number traits provide a convert command for
 * converting any object with a << operator to a string.
 * The traits are specialized for some number types to
 * avoid excessive precision in generated text.
 *
 */
// ======================================================================

#include <iomanip>
#include <sstream>
#include <string>

namespace TextGen
{
  template <typename Value>
  struct DefaultNumberTraits
  {
	static std::string tostring(const Value & theValue)
	{
	  std::ostringstream os;
	  os << theValue;
	  return os.str();
	}
  };

  //! Default traits for float type
  template <>
  struct DefaultNumberTraits<float>
  {
	static std::string tostring(float theValue)
	{
	  std::ostringstream os;
#ifdef OLDGCC
	  os << std::setprecision(1) << theValue;
#else
	  os << std::fixed << std::setprecision(1) << theValue;
#endif
	  std::string ret = os.str();
	  if(ret.size() > 2)
		if(ret.substr(ret.size()-2,2) == ".0")
		  return ret.substr(0,ret.size()-2);
	  return ret;
	}
  };

  //! Default traits for double type
  template <>
  struct DefaultNumberTraits<double>
  {
	static std::string tostring(double theValue)
	{
	  std::ostringstream os;
#ifdef OLDGCC
	  os << std::setprecision(1) << theValue;
#else
	  os << std::fixed << std::setprecision(1) << theValue;
#endif
	  std::string ret = os.str();
	  if(ret.size() > 2)
		if(ret.substr(ret.size()-2,2) == ".0")
		  return ret.substr(0,ret.size()-2);
	  return ret;
	}
  };

} // namespace TextGen

// ======================================================================
