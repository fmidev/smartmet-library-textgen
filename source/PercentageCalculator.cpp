// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::PercentageCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::PercentageCalculator
 *
 * \brief Percentage intergator
 *
 */
// ======================================================================

#include "PercentageCalculator.h"
#include "DefaultAcceptor.h"
#include "NullAcceptor.h"

#include <iostream>
#include <newbase/NFmiGlobals.h>

using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  PercentageCalculator::~PercentageCalculator()
  {
	delete itsCondition;
	delete itsAcceptor;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  PercentageCalculator::PercentageCalculator()
	: itsCondition(new NullAcceptor)
	, itsAcceptor(new DefaultAcceptor)
	, itsCounter(0)
	, itsTotalCounter(0)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Copy constructor
   */
  // ----------------------------------------------------------------------

  PercentageCalculator::PercentageCalculator(const PercentageCalculator & other)
	: itsCondition(other.itsCondition->clone())
	, itsAcceptor(other.itsAcceptor->clone())
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

  void PercentageCalculator::operator()(float theValue)
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
   * \return The integrated percentage value in the range 0-100
   */
  // ----------------------------------------------------------------------

  float PercentageCalculator::operator()() const
  {
	if(itsTotalCounter==0)
	  return kFloatMissing;
	else
	  return 100*static_cast<float>(itsCounter)/itsTotalCounter;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Set the internal acceptor
   *
   * \param theAcceptor The acceptor to be used
   */
  // ----------------------------------------------------------------------

  void PercentageCalculator::acceptor(const Acceptor & theAcceptor)
  {
	itsAcceptor = theAcceptor.clone();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set the actual percentage calculator condition
   *
   * \param theCondition The condition to be used
   */
  // ----------------------------------------------------------------------

  void PercentageCalculator::condition(const Acceptor & theCondition)
  {
	itsCondition = theCondition.clone();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  Calculator * PercentageCalculator::clone() const
  {
	return new PercentageCalculator(*this);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Reset
   */
  // ----------------------------------------------------------------------

  void PercentageCalculator::reset()
  {
	itsCounter = 0;
	itsTotalCounter = 0;
  }

} // namespace WeatherAnalysis

// ======================================================================
