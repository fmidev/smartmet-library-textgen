// ======================================================================
/*!
 * \file
 * \brief Implementation of class MessageLogger
 */
// ======================================================================

#include "MessageLogger.h"
#include <fstream>
#include <iomanip>

using namespace std;

unsigned long MessageLogger::itsDepth = 0;
ostream * MessageLogger::itsOutput = 0;
char MessageLogger::itsIndentChar = ' ';
unsigned int MessageLogger::itsIndentStep = 2;
bool MessageLogger::itsTimeStampOn = false;

namespace
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Output timestamp if flag is true
   */
  // ----------------------------------------------------------------------

  void output_timestamp(bool theFlag, ostream * theOutput)
  {
	if(theOutput != 0 && theFlag)
	  {
		time_t t;
		static_cast<void>(time(&t));
		struct tm * loc = localtime(&t);

		*theOutput << setfill('0')
				   << setw(2) << loc->tm_hour
				   << ':'
				   << setw(2) << loc->tm_min
				   << ':'
				   << setw(2) << loc->tm_sec
				   << ' '
				   << setw(2) << loc->tm_mday
				   << '.'
				   << setw(2) << loc->tm_mon+1
				   << '.'
				   << setw(4) << loc->tm_year+1900
				   << ' ';
	  }
  }
						
}

// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

MessageLogger::~MessageLogger()
{
  --itsDepth;
  output_timestamp(itsTimeStampOn, itsOutput);
  if(itsOutput != 0)
	*itsOutput << string(itsIndentStep*itsDepth,itsIndentChar)
			   << "[Leaving "
			   << itsFunction
			   << ']'
			   << endl;
}

// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 *
 * \param theFunction The function name
 */
// ----------------------------------------------------------------------

MessageLogger::MessageLogger(const string & theFunction)
  : itsFunction(theFunction)
{
  output_timestamp(itsTimeStampOn, itsOutput);

  if(itsOutput != 0)
	*itsOutput << string(itsIndentStep*itsDepth,itsIndentChar)
			   << "[Entering "
			   << itsFunction
			   << ']'
			   << endl;
  ++itsDepth;
}

// ----------------------------------------------------------------------
/*!
 * \brief Write a new message when flush occurs
 *
 * \param theMessage The message to write
 */
// ----------------------------------------------------------------------

void MessageLogger::onNewMessage(const string_type & theMessage)
{
  output_timestamp(itsTimeStampOn, itsOutput);

  if(itsOutput != 0)
	*itsOutput << string(itsIndentStep*itsDepth,itsIndentChar)
			   << theMessage;
}

// ----------------------------------------------------------------------
/*!
 * \brief Open a messagelog output stream
 *
 * \param theFilename The filename for the log
 */
// ----------------------------------------------------------------------

void MessageLogger::open(const string & theFilename)
{
  delete itsOutput;
  itsOutput = 0;

  if(theFilename.empty())
	return;

  itsOutput = new ofstream(theFilename.c_str(), ios::out);
  if(itsOutput == 0)
	throw std::runtime_error("MessageLogger could not allocate a new output stream");
  if(!(*itsOutput))
	throw std::runtime_error("MessageLogger failed to open '"+theFilename+"' for writing");
  
}

// ======================================================================
