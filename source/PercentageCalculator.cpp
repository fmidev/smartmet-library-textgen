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

#include "NFmiGlobals.h"

using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  PercentageCalculator::PercentageCalculator()
	: itsAcceptor(new DefaultAcceptor())
	, itsCondition(new NullAcceptor())
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
	itsAcceptor = shared_ptr<Acceptor>(theAcceptor.clone());
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
	itsCondition = shared_ptr<Acceptor>(theCondition.clone());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<Calculator> PercentageCalculator::clone() const
  {
	return boost::shared_ptr<Calculator>(new PercentageCalculator(*this));
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
