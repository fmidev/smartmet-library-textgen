// ======================================================================
/*!
 * \file
 * \brief Interface of namespace TextGen::UnitFactory
 */
// ======================================================================

#pragma once

#include <memory>

namespace TextGen
{
class Sentence;

enum Units
{
  DegreesCelsius,
  MetersPerSecond,
  Millimeters,
  Percent,
  HectoPascal,
  Meters,
};

namespace UnitFactory
{
std::shared_ptr<Sentence> create(Units theUnit);
std::shared_ptr<Sentence> create(Units theUnit, int value);
std::shared_ptr<Sentence> create_unit(Units theUnit, int value, bool isInterval = false);

}  // namespace UnitFactory
}  // namespace TextGen

// ======================================================================
