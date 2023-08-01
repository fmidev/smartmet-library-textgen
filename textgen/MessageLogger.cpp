// ======================================================================
/*!
 * \file
 * \brief Implementation of class MessageLogger
 */
// ======================================================================
/*!
 * \class MessageLogger
 *
 * \brief Generic debugging message logger
 *
 * Sample use in a main program:
 * \code
 * int main()
 * {
 *   MessageLogger::open("my.log");
 *   MessageLogger log("main");
 *   log << "Starting the work" << std::endl;
 *   ...
 *
 * }
 * \endcode
 *
 * Sample use in a function:
 * \code
 * void myfunction()
 * {
 *   MessageLogger log("myfunction()");
 *   log << "calculating some result " << 10 << std::endl;
 * }
 * \endcode

 */
// ======================================================================

#include "MessageLogger.h"
#include "DebugTextFormatter.h"
#include <newbase/NFmiTime.h>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <utility>

using namespace std;

// TextGen is historical code from about 2002, and was not designed
// for multithreading. This is one way to avert multithreading issues
// without massive refactoring of code.

thread_local unsigned long sDepth = 0;
thread_local ofstream* sOutputFile = nullptr;
thread_local ostringstream* sOutputStream = nullptr;
thread_local char sIndentChar = ' ';
thread_local unsigned int sIndentStep = 2;
thread_local bool sTimeStampOn = false;
thread_local TextGen::DebugTextFormatter sFormatter;

namespace
{
// ----------------------------------------------------------------------
/*!
 * \brief Output timestamp if flag is true
 */
// ----------------------------------------------------------------------

void output_timestamp(bool theFlag)
{
  if (theFlag)
  {
    NFmiTime now;

    if (sOutputFile != nullptr)
      *sOutputFile << now.ToStr(kYYYYMMDDHHMMSS).CharPtr() << ' ';
    if (sOutputStream != nullptr)
      *sOutputStream << now.ToStr(kYYYYMMDDHHMMSS).CharPtr() << ' ';
  }
}
}  // namespace

// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

MessageLogger::~MessageLogger()
{
  --sDepth;
  output_timestamp(sTimeStampOn);

  if (sOutputFile != nullptr)
    *sOutputFile << string(sIndentStep * sDepth, sIndentChar) << "[Leaving " << itsFunction << ']'
                 << endl;

  if (sOutputStream != nullptr)
    *sOutputStream << string(sIndentStep * sDepth, sIndentChar) << "[Leaving " << itsFunction << ']'
                   << endl;
}

// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * \param theFunction The function name
 */
// ----------------------------------------------------------------------

MessageLogger::MessageLogger(string theFunction) : itsFunction(std::move(theFunction))
{
  output_timestamp(sTimeStampOn);

  if (sOutputFile != nullptr)
    *sOutputFile << string(sIndentStep * sDepth, sIndentChar) << "[Entering " << itsFunction << ']'
                 << endl;
  if (sOutputStream != nullptr)
    *sOutputStream << string(sIndentStep * sDepth, sIndentChar) << "[Entering " << itsFunction
                   << ']' << endl;
  ++sDepth;
}

// ----------------------------------------------------------------------
/*!
 * \brief Write a new message when flush occurs
 *
 * \param theMessage The message to write
 */
// ----------------------------------------------------------------------

void MessageLogger::onNewMessage(const string_type& theMessage)
{
  output_timestamp(sTimeStampOn);

  if (sOutputFile != nullptr)
    *sOutputFile << string(sIndentStep * sDepth, sIndentChar) << theMessage;
  if (sOutputStream != nullptr)
    *sOutputStream << string(sIndentStep * sDepth, sIndentChar) << theMessage;
}

// ----------------------------------------------------------------------
/*!
 * \brief Open a messagelog output stream
 *
 * \param theFilename The filename for the log
 */
// ----------------------------------------------------------------------

void MessageLogger::open(const string& theFilename)
{
  delete sOutputFile;
  sOutputFile = nullptr;

  if (theFilename.empty())
    return;

  sOutputFile = new ofstream(theFilename.c_str(), ios::out);
  if (!(*sOutputFile))
    throw std::runtime_error("MessageLogger failed to open '" + theFilename + "' for writing");
}

// ----------------------------------------------------------------------
/*!
 * \brief Open a messagelog output stream
 *
 * \param theFilename The filename for the log
 */
// ----------------------------------------------------------------------

void MessageLogger::open()
{
  if (sOutputStream == nullptr)
    sOutputStream = new ostringstream();
  else
    sOutputStream->str("");
}

void MessageLogger::indent(char theChar)
{
  sIndentChar = theChar;
}

void MessageLogger::indentstep(unsigned int theStep)
{
  sIndentStep = theStep;
}

void MessageLogger::timestamp(bool theFlag)
{
  sTimeStampOn = theFlag;
}

std::string MessageLogger::str() const
{
  if (sOutputStream != nullptr)
    return sOutputStream->str();
  return {};
}

// ----------------------------------------------------------------------
/*!
 * \brief Convenience operator for textgen library
 *
 * This really should not be here, it couples the generic MessageLogger
 * class to the DebugTextFormatter and Glyph classes. However, any
 * other solution seems inconvenient for the user.
 *
 */
// ----------------------------------------------------------------------

MessageLogger& MessageLogger::operator<<(const TextGen::Glyph& theGlyph)
{
  if (sOutputFile != nullptr)
    *sOutputFile << "Return: " << sFormatter.format(theGlyph) << endl;
  if (sOutputStream != nullptr)
    *sOutputStream << "Return: " << sFormatter.format(theGlyph) << endl;
  return *this;
}

// ======================================================================
