// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::StandardDeviationCalculator
 */
// ======================================================================

#include "StandardDeviationCalculator.h"
#include "DefaultAcceptor.h"

#include "NFmiGlobals.h"

#include <cmath>

using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  StandardDeviationCalculator::StandardDeviationCalculator()
	: itsAcceptor(new DefaultAcceptor())
	, itsCounter(0)
	, itsSum(0)
	, itsSquaredSum(0)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Integrate a new value
   *
   * \param theValue
   */
  // ----------------------------------------------------------------------

  void StandardDeviationCalculator::operator()(float theValue)
  {
	if(itsAcceptor->accept(theValue))
	  {
		++itsCounter;
		itsSum += theValue;
		itsSquaredSum += theValue*theValue;
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the integrated value
   *
   * \return The integrated sum value
   */
  // ----------------------------------------------------------------------

  float StandardDeviationCalculator::operator()() const
  {
	if(itsCounter<2)
	  return kFloatMissing;
	else
	  return sqrt((itsSquaredSum-itsSum*itsSum/itsCounter)/(itsCounter-1));
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Set the internal acceptor
   *
   * \param theAcceptor The acceptor to be used
   */
  // ----------------------------------------------------------------------

  void StandardDeviationCalculator::acceptor(const Acceptor & theAcceptor)
  {
	itsAcceptor = shared_ptr<Acceptor>(theAcceptor.clone());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<Calculator> StandardDeviationCalculator::clone() const
  {
	return boost::shared_ptr<Calculator>(new StandardDeviationCalculator(*this));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Reset
   */
  // ----------------------------------------------------------------------

  void StandardDeviationCalculator::reset()
  {
	itsCounter = 0;
	itsSum = 0;
	itsSquaredSum = 0;
  }

} // namespace WeatherAnalysis

// ======================================================================
