// ======================================================================
/*!
 * \file
 * \brief Interface of class MessageLogger
 */
// ======================================================================

#pragma once

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
  MessageLogger(const std::string& theFunction);
  virtual void onNewMessage(const string_type& theMessage);
  std::string str() const;
  MessageLogger& operator<<(const TextGen::Glyph& theGlyph);

  static void open(const std::string& theFilename);
  static void open();
  static void indent(char theChar);
  static void indentstep(unsigned int theStep);
  static void timestamp(bool theFlag);

 private:
  MessageLogger();
  MessageLogger(const MessageLogger& theLogger);
  MessageLogger& operator=(const MessageLogger& theLogger);

  std::string itsFunction;

};  // MessageLogger

// ======================================================================
