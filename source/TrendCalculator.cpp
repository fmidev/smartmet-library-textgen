// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::TrendCalculator
 */
// ======================================================================

#include "TrendCalculator.h"
#include "DefaultAcceptor.h"

#include "NFmiGlobals.h"

using namespace boost;

namespace WeatherAnalysis
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------

  TrendCalculator::TrendCalculator()
	: itsAcceptor(new DefaultAcceptor())
	, itsCounter(0)
	, itsStartValue(kFloatMissing)
	, itsEndValue(kFloatMissing)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Integrate a new value
   *
   * \param theValue
   */
  // ----------------------------------------------------------------------

  void TrendCalculator::operator()(float theValue)
  {
	if(itsAcceptor->accept(theValue))
	  {
		if(itsCounter==0)
		  itsStartValue = theValue;
		else
		  itsEndValue = theValue;
		++itsCounter;
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the integrated value
   *
   * \return The integrated trend value
   */
  // ----------------------------------------------------------------------

  float TrendCalculator::operator()() const
  {
	if(itsCounter<2 ||
	   itsStartValue==kFloatMissing ||
	   itsEndValue==kFloatMissing)
	  return kFloatMissing;
	else
	  return (itsEndValue-itsStartValue);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Set the internal acceptor
   *
   * \param theAcceptor The acceptor to be used
   */
  // ----------------------------------------------------------------------

  void TrendCalculator::acceptor(const Acceptor & theAcceptor)
  {
	itsAcceptor = shared_ptr<Acceptor>(theAcceptor.clone());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<Calculator> TrendCalculator::clone() const
  {
	return boost::shared_ptr<Calculator>(new TrendCalculator(*this));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Reset
   */
  // ----------------------------------------------------------------------

  void TrendCalculator::reset()
  {
	itsCounter = 0;
	itsStartValue = kFloatMissing;
	itsEndValue = kFloatMissing;
  }

} // namespace WeatherAnalysis

// ======================================================================
