#include "regression/tframe.h"
#include "QueryDataIntegrator.h"

using namespace std;

namespace QueryDataIntegratorTest
{

  // ----------------------------------------------------------------------
  /*!
   * \brief Test QueryDataIntegrator::integrate
   */
  // ----------------------------------------------------------------------

  void integrate_area()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test QueryDataIntegrator::integrate
   */
  // ----------------------------------------------------------------------

  void integrate_point_time()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test QueryDataIntegrator::integrate
   */
  // ----------------------------------------------------------------------

  void integrate_area_time()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test QueryDataIntegrator::integrate
   */
  // ----------------------------------------------------------------------

  void integrate_point_time_subtime()
  {
	TEST_NOT_IMPLEMENTED();
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Test QueryDataIntegrator::integrate
   */
  // ----------------------------------------------------------------------

  void integrate_area_time_subtime()
  {
	TEST_NOT_IMPLEMENTED();
  }

  //! The actual test driver
  class tests : public tframe::tests
  {
	//! Overridden message separator
	virtual const char * const error_message_prefix() const
	{
	  return "\n\t";
	}

	//! Main test suite
	void test(void)
	{
	  TEST(integrate_area);
	  TEST(integrate_point_time);
	  TEST(integrate_area_time);
	  TEST(integrate_point_time_subtime);
	  TEST(integrate_area_time_subtime);
	  
	}

  }; // class tests

} // namespace QueryDataIntegratorTest


int main(void)
{
  cout << endl
	   << "QueryDataIntegrator tester" << endl
	   << "=========================" << endl;
  QueryDataIntegratorTest::tests t;
  return t.run();
}
