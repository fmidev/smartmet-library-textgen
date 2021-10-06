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
  ~MessageLogger() override;
  MessageLogger(const std::string& theFunction);
  void onNewMessage(const string_type& theMessage) override;
  std::string str() const;
  MessageLogger& operator<<(const TextGen::Glyph& theGlyph);

  static void open(const std::string& theFilename);
  static void open();
  static void indent(char theChar);
  static void indentstep(unsigned int theStep);
  static void timestamp(bool theFlag);

 private:
  MessageLogger() = delete;
  MessageLogger(const MessageLogger& theLogger) = delete;
  MessageLogger& operator=(const MessageLogger& theLogger) = delete;

  std::string itsFunction;

};  // MessageLogger

// ======================================================================
