// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::SumCalculator
 */
// ======================================================================

#include "SumCalculator.h"
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

  SumCalculator::SumCalculator()
	: itsAcceptor(new DefaultAcceptor())
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Integrate a new value
   *
   * \param theValue
   */
  // ----------------------------------------------------------------------

  void SumCalculator::operator()(float theValue)
  {
	if(itsAcceptor->accept(theValue))
	  {
		++itsCounter;
		itsSum += theValue;
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the integrated value
   *
   * \return The integrated sum value
   */
  // ----------------------------------------------------------------------

  float SumCalculator::operator()() const
  {
	if(itsCounter==0)
	  return kFloatMissing;
	else
	  return itsSum;
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Set the internal acceptor
   *
   * \param theAcceptor The acceptor to be used
   */
  // ----------------------------------------------------------------------

  void SumCalculator::acceptor(const Acceptor & theAcceptor)
  {
	itsAcceptor = shared_ptr<Acceptor>(theAcceptor.clone());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  boost::shared_ptr<Calculator> SumCalculator::clone() const
  {
	return boost::shared_ptr<Calculator>(new SumCalculator(*this));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Reset
   */
  // ----------------------------------------------------------------------

  void SumCalculator::reset()
  {
	itsCounter = 0;
	itsSum = 0;
  }

} // namespace WeatherAnalysis

// ======================================================================
