// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::UnitFactory
 */
// ======================================================================
/*!
 * \namespace TextGen::UnitFactory
 *
 * \brief Provides unit phrase creation services.
 *
 * The purpose of the factory is to decouple the formatting
 * of units from their creation. The desired formatting is
 * specified by a global Settings variable for each unit.
 *
 * Note that we return always on purpose a Sentence so that
 * we may append the phrase to a sentence.
 *
 * The variables controlling each unit are listed below.
 * Note that the default is always SI.
 *
 * <table>
 * <tr>
 * <th>Unit</th>
 * <th>Variable</th>
 * <th>Values</th>
 * </tr>
 * <tr>
 * <td>DegreesCelsius</td>
 * <td>textgen::units::celsius::format</td>
 * <td>phrase/SI</td>
 * </tr>
 * <tr>
 * <td>MetersPerSecond</td>
 * <td>textgen::units::meterspersecond::format</td>
 * <td>phrase/SI</td>
 * </tr>
 * <td>Millimeters</td>
 * <td>textgen::units::millimeters::format</td>
 * <td>phrase/SI</td>
 * </tr>
 * <tr>
 * <td>Percent</td>
 * <td>textgen::units::percent::format</td>
 * <td>phrase/SI</td>
 * </tr>
 * </table>
 */
// ======================================================================

#ifndef TEXTGEN_UNITFACTORY_H
#define TEXTGEN_UNITFACTORY_H

#include "boost/shared_ptr.hpp"

namespace TextGen
{
  class Sentence;

  enum Units
	{
	  DegreesCelsius,
	  MetersPerSecond,
	  Millimeters,
	  Percent
	};

  namespace UnitFactory
  {

	boost::shared_ptr<Sentence> create(Units theUnit);

  } // namespace NumberFactory
} // namespace TextGen

#endif // TEXTGEN_UNITFACTORY_H

// ======================================================================
