// ======================================================================
/*!
 * \file
 * \brief Implementation of class WeatherAnalysis::ModMeanCalculator
 */
// ======================================================================

#include "ModMeanCalculator.h"
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

  ModMeanCalculator::ModMeanCalculator(int theModulo)
	: itsAcceptor(new DefaultAcceptor())
	, itsModulo(theModulo)
	, itsSum1(0)
	, itsSum2(0)
	, itsCounter1(0)
	, itsCounter2(0)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Integrate a new value
   *
   * \param theValue
   */
  // ----------------------------------------------------------------------

  void ModMeanCalculator::operator()(float theValue)
  {
	if(itsAcceptor->accept(theValue))
	  {
		if(theValue < itsModulo)
		  {
			itsCounter1++;
			itsSum1 += theValue;
		  }
		else
		  {
			itsCounter2++;
			itsSum2 += theValue;
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
	if(itsCounter1+itsCounter2==0)
	  return kFloatMissing;
	if(itsCounter1==0)
	  return itsSum2/itsCounter2;
	if(itsCounter2==0)
	  return itsSum1/itsCounter1;

	const float mean1 = itsSum1/itsCounter1;
	const float mean2 = itsSum2/itsCounter2;

	if(mean2-mean1 < itsModulo/2)
	  return (itsSum1+itsSum2)/(itsCounter1+itsCounter2);

	const float mean = (itsSum2+itsModulo*itsCounter2+itsSum1)/(itsCounter1+itsCounter2);

	return (mean<itsModulo ? mean : mean-itsModulo);

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
	itsCounter1 = 0;
	itsCounter2 = 0;
	itsSum1 = 0;
	itsSum2 = 0;
  }

} // namespace WeatherAnalysis

// ======================================================================
