// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::ModMeanCalculator
 */
// ======================================================================

#include "ModMeanCalculator.h"
#include "DefaultAcceptor.h"

#include "NFmiGlobals.h"

#include <cmath>

using namespace boost;
using namespace std;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  ModMeanCalculator::ModMeanCalculator(int theModulo)
	: itsAcceptor(new DefaultAcceptor())
	, itsModulo(theModulo)
	, itsCounter(0)
	, itsSum(0)
	, itsPreviousDirection(kFloatMissing)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Integrate a new value
   *
   * Uses the Mitsuta algorithm.
   *
   * \param theValue
   */
  // ----------------------------------------------------------------------

  void ModMeanCalculator::operator()(float theValue)
  {
	if(itsAcceptor->accept(theValue))
	  {
		++itsCounter;
		if(itsCounter==1)
		  {
			itsSum = theValue;
			itsPreviousDirection = theValue;
		  }
		else
		  {
			const float diff = theValue - itsPreviousDirection;
			float dir = itsPreviousDirection + diff;
			if(diff < -itsModulo/2)
			  dir += itsModulo;
			else if(diff > itsModulo/2)
			  dir -= itsModulo;
			itsSum += dir;

			itsPreviousDirection = dir;
		  }
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the integrated value
   *
   * \return The integrated mean value
   */
  // ----------------------------------------------------------------------

  float ModMeanCalculator::operator()() const
  {
	if(itsCounter==0)
	  return kFloatMissing;

	// Floats do not support %, it is an integer operator
	float mean = itsSum/itsCounter;
	mean -= itsModulo*floor(mean/itsModulo);
	return mean;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Set the internal acceptor
   *
   * \param theAcceptor The acceptor to be used
   */
  // ----------------------------------------------------------------------

  void ModMeanCalculator::acceptor(const Acceptor & theAcceptor)
  {
	itsAcceptor = shared_ptr<Acceptor>(theAcceptor.clone());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<Calculator> ModMeanCalculator::clone() const
  {
	return boost::shared_ptr<Calculator>(new ModMeanCalculator(*this));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Reset
   */
  // ----------------------------------------------------------------------

  void ModMeanCalculator::reset()
  {
	itsCounter = 0;
	itsSum = 0;
	itsPreviousDirection = kFloatMissing;
  }

} // namespace WeatherAnalysis

// ======================================================================
