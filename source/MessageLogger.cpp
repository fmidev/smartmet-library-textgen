// ======================================================================
/*!
 * \file
 * \brief Implementation of class MessageLogger
 */
// ======================================================================

#include "MessageLogger.h"
#include <fstream>

using namespace std;

unsigned long MessageLogger::itsDepth = 0;
ostream * MessageLogger::itsOutput = 0;


// ----------------------------------------------------------------------
/*!
 * \brief Destructor
 */
// ----------------------------------------------------------------------

MessageLogger::~MessageLogger()
{
	*itsOutput << string(--itsDepth,'.')
			   << "[Entering "
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
  *itsOutput << string(itsDepth++,'.')
			 << "[Leaving "
			 << itsFunction
			 << ']'
			 << endl;
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
  itsOutput = new ofstream(theFilename.c_str(), ios::out);
  if(itsOutput == 0)
	throw std::runtime_error("MessageLogger could not allocate a new output stream");
  if(!(*itsOutput))
	throw std::runtime_error("MessageLogger failed to open '"+theFilename+"' for writing");
  
}

// ======================================================================
