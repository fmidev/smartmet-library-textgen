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
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	boost::shared_ptr<Sentence> create(int theNumber)
	{
	  const string formatter = Settings::optional_string("textgen::numberformatter","default");

	  shared_ptr<Sentence> sentence(new Sentence);
	  
	  if(formatter == "default")
		*sentence << Number<int>(theNumber);

	  else if(formatter == "sonera")
		{
		  sonera_check(theNumber);
		  if(theNumber < 0)
			*sentence << "miinus" << Number<int>(-theNumber);
		  else
			*sentence << Number<int>(theNumber);
		}
	  else
		throw TextGenError("Numberformatter '"+formatter+"' is unknown");

	  return sentence;
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Create an integer range
	 *
	 * \param theLoLimit The lower limit
	 * \param theHiLimit The upper limit
	 * \return The sentence
	 */
	// ----------------------------------------------------------------------
	
	boost::shared_ptr<Sentence> create(int theLoLimit, int theHiLimit)
	{
	  if(theLoLimit == theHiLimit)
		return create(theLoLimit);
	  
	  const string formatter = Settings::optional_string("textgen::numberformatter","default");
	  
	  shared_ptr<Sentence> sentence(new Sentence);

	  if(formatter == "default")
		{
		  typedef NumberRange<Number<int> > IntRange;
		  *sentence << IntRange(theLoLimit,theHiLimit);
		}
	  
	  else if(formatter == "sonera")
		{
		  *sentence << *create(theLoLimit)
					<< "viiva"
					<< *create(theHiLimit);
		}
	  else
		throw TextGenError("Numberformatter '"+formatter+"' is unknown");

	  return sentence;

	}
	

  } // namespace NumberFactory
} // namespace TextGen
  
// ======================================================================
  