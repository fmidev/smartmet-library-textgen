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
 *   log << "Starting the work\n";
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

#include <iostream>
#include <string>
#include <stdexcept>

class MessageLogger
{

public:

  ~MessageLogger();
  MessageLogger(const std::string & theFunction);

  template <typename T>
  MessageLogger & operator<<(const T & theObject)
  {
	if(itsOutput == 0)
	  throw std::runtime_error("MessageLogger not opened");
	*itsOutput << std::string(itsDepth,'.')
			   << theObject;
  }

  static void open(const std::string & theFilename);

private:

  MessageLogger();
  MessageLogger(const MessageLogger & theLogger);
  MessageLogger & operator=(const MessageLogger & theLogger);

  std::string itsFunction;

  static unsigned long itsDepth;
  static std::ostream * itsOutput;

}; // MessageLogger

#endif // MESSAGELOGGER_H

// ======================================================================
