// ======================================================================
/*!
 * \file
 * \brief Interface of class MessageLogger
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

#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include "MessageLoggerStream.h"

#include <stdexcept>

namespace TextGen
{
  class Glyph;
}

class MessageLogger : public MessageLoggerStream<>
{

public:

  ~MessageLogger();
  MessageLogger(const std::string & theFunction);

  virtual void onNewMessage(const string_type & theMessage);

  static void open(const std::string & theFilename);
  static void indent(char theChar) { itsIndentChar = theChar; }
  static void indentstep(unsigned int theStep) { itsIndentStep = theStep; }
  static void timestamp(bool theFlag) { itsTimeStampOn = theFlag; }

  MessageLogger & operator<<(const TextGen::Glyph & theGlyph);

private:

  MessageLogger();
  MessageLogger(const MessageLogger & theLogger);
  MessageLogger & operator=(const MessageLogger & theLogger);

  std::string itsFunction;
  static unsigned long itsDepth;
  static std::ostream * itsOutput;
  static char itsIndentChar;
  static unsigned int itsIndentStep;
  static bool itsTimeStampOn;

}; // MessageLogger

#endif // MESSAGELOGGER_H

// ======================================================================
