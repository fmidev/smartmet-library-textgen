// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::CountCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::CountCalculator
 *
 * \brief Count intergator
 *
 */
// ======================================================================

#include "CountCalculator.h"
#include "DefaultAcceptor.h"
#include "NullAcceptor.h"

#include <newbase/NFmiGlobals.h>

using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  CountCalculator::~CountCalculator()
  {
	delete itsCondition;
	delete itsAcceptor;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  CountCalculator::CountCalculator()
	: itsAcceptor(new DefaultAcceptor)
	, itsCondition(new NullAcceptor)
	, itsCounter(0)
	, itsTotalCounter(0)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Copy constructor
   */
  // ----------------------------------------------------------------------

  CountCalculator::CountCalculator(const CountCalculator & other)
	: itsAcceptor(other.itsAcceptor->clone())
	, itsCondition(other.itsCondition->clone())
	, itsCounter(0)
	, itsTotalCounter(0)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Integrate a new value
   *
   * \param theValue
   */
  // ----------------------------------------------------------------------

  void CountCalculator::operator()(float theValue)
  {
	if(itsAcceptor->accept(theValue))
	  {
		++itsTotalCounter;
		if(itsCondition->accept(theValue))
		  ++itsCounter;
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the integrated value
   *
   * \return The integrated count value in the range 0-100
   */
  // ----------------------------------------------------------------------

  float CountCalculator::operator()() const
  {
	if(itsTotalCounter==0)
	  return kFloatMissing;
	else
	  return itsCounter;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Set the internal acceptor
   *
   * \param theAcceptor The acceptor to be used
   */
  // ----------------------------------------------------------------------

  void CountCalculator::acceptor(const Acceptor & theAcceptor)
  {
	itsAcceptor = theAcceptor.clone();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the actual count calculator condition
   *
   * \param theCondition The condition to be used
   */
  // ----------------------------------------------------------------------

  void CountCalculator::condition(const Acceptor & theCondition)
  {
	itsCondition = theCondition.clone();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  Calculator * CountCalculator::clone() const
  {
	return new CountCalculator(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Reset
   */
  // ----------------------------------------------------------------------

  void CountCalculator::reset()
  {
	itsCounter = 0;
	itsTotalCounter = 0;
  }

} // namespace WeatherAnalysis

// ======================================================================
