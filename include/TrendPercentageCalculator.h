// ======================================================================
/*!
 * \file
 * \brief Interface of class WeatherAnalysis::TrendPercentageCalculator
 */
// ======================================================================
/*!
 * \class WeatherAnalysis::TrendPercentageCalculator
 *
 * \brief TrendPercentage intergator
 *
 */
// ======================================================================

#ifndef WEATHERANALYSIS_TRENDPERCENTAGECALCULATOR_H
#define WEATHERANALYSIS_TRENDPERCENTAGECALCULATOR_H

#include "Calculator.h"
#include "Acceptor.h"
#include "DefaultAcceptor.h"

#include "NFmiGlobals.h"
#include "boost/shared_ptr.hpp"

namespace WeatherAnalysis
{
  template <typename T>
  class TrendPercentageCalculator : public Calculator
  {
  public:

	typedef T Comparator;

	TrendPercentageCalculator();
	virtual ~TrendPercentageCalculator() { }
	virtual void operator()(float theValue);
	virtual float operator()() const;
	virtual void acceptor(const Acceptor & theAcceptor);
	virtual boost::shared_ptr<Calculator> clone() const;
	virtual void reset();

  private:

	boost::shared_ptr<Acceptor> itsAcceptor;
	
	long itsTotalCounter;
	long itsCounter;
	float itsLastValue;

  }; // class TrendPercentageCalculator

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   */
  // ----------------------------------------------------------------------
  
  template <typename T>
  TrendPercentageCalculator<T>::TrendPercentageCalculator()
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

  template <typename T>
  void TrendPercentageCalculator<T>::operator()(float theValue)
  {
	if(itsAcceptor->accept(theValue))
	  {
		if(itsTotalCounter==0)
		  itsLastValue = theValue;
		else if(Comparator(theValue,itsLastValue))
		  ++itsCounter;
		itsTotalCounter++;
		itsLastValue = theValue;
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return the integrated value
   *
   * \return The integrated trendPercentage value
   */
  // ----------------------------------------------------------------------

  template <typename T>
  float TrendPercentageCalculator<T>::operator()() const
  {
	if(itsTotalCounter < 2)
	  return kFloatMissing;
	
	// TotalCounter counts numbers, Counter pairs. Hence the minus 1
	return static_cast<float>(itsCounter)/(itsTotalCounter-1);
  }
  
  // ----------------------------------------------------------------------
  /*!
   * \brief Set the internal acceptor
   *
   * \param theAcceptor The acceptor to be used
   */
  // ----------------------------------------------------------------------

  template <typename T>
  void TrendPercentageCalculator<T>::acceptor(const Acceptor & theAcceptor)
  {
	itsAcceptor = shared_ptr<Acceptor>(theAcceptor.clone());
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Clone
   */
  // ----------------------------------------------------------------------

  template <typename T> 
  boost::shared_ptr<Calculator> TrendPercentageCalculator<T>::clone() const
  {
	return boost::shared_ptr<Calculator>(new TrendPercentageCalculator(*this));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Reset
   */
  // ----------------------------------------------------------------------

  template <typename T>
  void TrendPercentageCalculator<T>::reset()
  {
	itsCounter = 0;
	itsStartValue = kFloatMissing;
	itsEndValue = kFloatMissing;
  }

} // namespace WeatherAnalysis

#endif // WEATHERANALYSIS_TRENDPERCENTAGECALCULATOR_H

// ======================================================================
