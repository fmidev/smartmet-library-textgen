// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace TextGen::NumberFactory
 */
// ======================================================================

#include "NumberFactory.h"
#include "Number.h"
#include "NumberRange.h"
#include "Settings.h"
#include "Sentence.h"
#include "TextGenError.h"

using namespace std;
using namespace boost;

namespace TextGen
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Check number validity for Sonera
   *
   * Throws if the number is out of range -100...1000
   *
   * \param theNumber The number to test
   */
  // ----------------------------------------------------------------------

  void sonera_check(int theNumber)
  {
	if(theNumber < -100 || theNumber > 100)
	  throw TextGenError("Numberformatter 'sonera' supports only numbers -100...100");
  }

  namespace NumberFactory
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Create an integer
	 *
	 * \param theNumber The integer
	 * \return The glyph
	 */
	// ----------------------------------------------------------------------
	
	boost::shared_ptr<Glyph> create(int theNumber)
	{
	  const string formatter = Settings::optional_string("textgen::numberformatter","default");
	  
	  if(formatter == "default")
		return shared_ptr<Number<int> >(new Number<int>(theNumber));
	  if(formatter == "sonera")
		{
		  sonera_check(theNumber);
		  shared_ptr<Sentence> s(new Sentence);
		  if(theNumber < 0)
			*s << "miinus" << -theNumber;
		  else
			*s << theNumber;
		  return s;
		}
	  
	  throw TextGenError("Numberformatter '"+formatter+"' is unknown");
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Create an integer range
	 *
	 * \param theLoLimit The lower limit
	 * \param theHiLimit The upper limit
	 * \return The glyph
	 */
	// ----------------------------------------------------------------------
	
	boost::shared_ptr<Glyph> create(int theLoLimit, int theHiLimit)
	{
	  if(theLoLimit == theHiLimit)
		return create(theLoLimit);
	  
	  const string formatter = Settings::optional_string("textgen::numberformatter","default");
	  
	  if(formatter == "default")
		{
		  typedef NumberRange<Number<int> > IntRange;
		  return shared_ptr<IntRange>(new IntRange(theLoLimit,theHiLimit));
		}
	  
	  if(formatter == "sonera")
		{
		  sonera_check(theLoLimit);
		  sonera_check(theHiLimit);
		  shared_ptr<Sentence> s(new Sentence);
		  if(theLoLimit < 0)
			*s << "miinus" << -theLoLimit;
		  else
			*s << theLoLimit;
		  if(theHiLimit < 0)
			*s << "miinus" << -theHiLimit;
		  else
			*s << theHiLimit;
		  return s;
		}
	  
	  throw TextGenError("Numberformatter '"+formatter+"' is unknown");
	}
	

  } // namespace NumberFactory
} // namespace TextGen
  
// ======================================================================
  
