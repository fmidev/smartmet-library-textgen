// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace MathTools
 */
// ======================================================================
/*!
 * \namespace MathTools
 *
 * \brief Various mathematical utitity functions
 *
 */
// ======================================================================

#ifndef MATHTOOLS_H
#define MATHTOOLS_H

namespace MathTools
{

  int to_precision(float theValue, int thePrecision);

  // ----------------------------------------------------------------------
  /*!
   * \brief Return mean of two arguments
   */
  // ----------------------------------------------------------------------

  inline
  double mean(double arg1, double arg2)
  {
	return (arg1+arg2)/2;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return mean of three arguments
   */
  // ----------------------------------------------------------------------

  inline
  double mean(double arg1, double arg2, double arg3)
  {
	return (arg1+arg2+arg3)/3;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return mean of four arguments
   */
  // ----------------------------------------------------------------------

  inline
  double mean(double arg1, double arg2, double arg3, double arg4)
  {
	return (arg1+arg2+arg3+arg4)/4;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return smallest of two arguments
   */
  // ----------------------------------------------------------------------

  template <typename T>
  const T & min(const T & arg1, const T & arg2)
  {
	return (arg1 < arg2 ? arg1 : arg2);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return largest of two arguments
   */
  // ----------------------------------------------------------------------

  template <typename T>
  const T & max(const T & arg1, const T & arg2)
  {
	return (arg1 < arg2 ? arg2 : arg1);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return smallest of three arguments
   */
  // ----------------------------------------------------------------------

  template <typename T>
  const T & min(const T & arg1, const T & arg2, const T & arg3)
  {
	return min(min(arg1,arg2),arg3);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return largest of three arguments
   */
  // ----------------------------------------------------------------------

  template <typename T>
  const T & max(const T & arg1, const T & arg2, const T & arg3)
  {
	return max(max(arg1,arg2),arg3);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return smallest of four arguments
   */
  // ----------------------------------------------------------------------

  template <typename T>
  const T & min(const T & arg1, const T & arg2, const T & arg3, const T & arg4)
  {
	return min(min(arg1,arg2),min(arg3,arg4));
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return largest of three arguments
   */
  // ----------------------------------------------------------------------

  template <typename T>
  const T & max(const T & arg1, const T & arg2, const T & arg3, const T & arg4)
  {
	return max(max(arg1,arg2),max(arg3,arg4));
  }

} // namespace MathTools

#endif // MATHTOOLS_H

// ======================================================================
