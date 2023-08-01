// ======================================================================
/*!
 * \file
 * \brief Interface of class MessageLoggerStream
 */
// ======================================================================

#pragma once

#include <cassert>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>

#ifndef UNIX
#pragma warning(disable : 4512)  // poistaa VC++ k‰‰nt‰j‰n C4512 "assignment operator could not be
                                 // generated" varoituksen
#endif

// forward declaration(s)

template <class char_type = char, class CharTraits = std::char_traits<char_type> >
class MessageLoggerStream;

template <class char_type = char, class CharTraits = std::char_traits<char_type> >
class MessageLoggerStreambuf : public std::basic_streambuf<char_type, CharTraits>
{
 private:
  friend class MessageLoggerStream<char, CharTraits>;

  using streambuf_type = std::basic_streambuf<char_type, CharTraits>;
  using ostream_type = MessageLoggerStream<char_type, CharTraits>;
  using base_class = std::basic_streambuf<char_type, CharTraits>;

  using int_type = typename CharTraits::int_type;
  using pos_type = typename CharTraits::pos_type;
  using off_type = typename CharTraits::off_type;

  enum
  {
    _BADOFF = -1  // bad offset - for positioning functions
  };

 protected:
  // only for output, not for input
  int_type pbackfail(int_type /*unused*/ = CharTraits::eof()) override
  {
    throw std::runtime_error("MessageLoggerStream::pbackfail not available");
    return 0;
  }

  // only for output, not for input
  std::streamsize showmanyc() override
  {
    throw std::runtime_error("MessageLoggerStream::showmanyc not available");
    return 0;
  }

  // only for output, not for input
  int_type underflow() override
  {
    throw std::runtime_error("MessageLoggerStream::underflow not available");
    return 0;
  }

  // only for output, not for input
  int_type uflow() override
  {
    throw std::runtime_error("MessageLoggerStream::uflow not available");
    return 0;
  }

  // only for output, not for input
  std::streamsize xsgetn(char_type* /* _S */, std::streamsize /* _N */) override
  {
    throw std::runtime_error("MessageLoggerStream::xsgetn not available");
    return 0;
  }

  // we don't allow positioning
  pos_type seekoff(off_type /*unused*/,
                   std::ios_base::seekdir /*unused*/,
                   std::ios_base::openmode /*unused*/ = std::ios_base::in |
                                                        std::ios_base::out) override
  {
    throw std::runtime_error("MessageLoggerStream::seekoff not available");
    return 0;
  }

  // we don't allow positioning
  pos_type seekpos(pos_type /*unused*/,
                   std::ios_base::openmode /*unused*/ = std::ios_base::in |
                                                        std::ios_base::out) override
  {
    throw std::runtime_error("MessageLoggerStream::seekpos not available");
    return 0;
  }

  // output functions

  // called to write out from the internal buffer, into the external buffer
  int sync() override
  {
    itsOwnerStream->onNewMessage(GetStreamBuffer().str());
    itsStreamBuffer.reset(new StringStream);
    return 0;
  }

  streambuf_type* setbuf(char_type* buffer, std::streamsize n) override
  {
    // note: this function MUST be called
    // before working with this stream buffer

    // we don't use a buffer - we forward everything
    assert(buffer == nullptr && n == 0);
    this->setp(nullptr, nullptr);

    return this;
  }

  // write the characters from the buffer
  // to their real destination
  int_type overflow(int_type nChar = CharTraits::eof()) override
  {
    if (CharTraits::not_eof(nChar))
    {
      GetStreamBuffer() << char_type(nChar);
    }
    return CharTraits::not_eof(nChar);
  }

  std::streamsize xsputn(const char_type* S, std::streamsize N) override
  {
    GetStreamBuffer().write(S, N);
    return N;
  }

 public:
  MessageLoggerStreambuf() : itsStreamBuffer(new StringStream) {}

 private:
  using StringStream = std::basic_ostringstream<char_type>;

  StringStream& GetStreamBuffer()
  {
    // allocate the buffer first !!!
    assert(itsStreamBuffer.get());
    return *itsStreamBuffer;
  }

  // holds the Message, until it's flushed
  std::unique_ptr<StringStream> itsStreamBuffer;

  // the Message Handler Stream - where we write into
  ostream_type* itsOwnerStream;
};

template <class char_type, class CharTraits>
class MessageLoggerStream : public std::basic_ostream<char_type, CharTraits>
{
  using handler_streambuf_type = MessageLoggerStreambuf<char_type, CharTraits>;
  friend class MessageLoggerStreambuf<char_type, CharTraits>;

  using base_class = std::basic_ostream<char_type, CharTraits>;

 protected:
  using string_type = std::basic_string<char_type>;

  MessageLoggerStream() : base_class(nullptr), itsStreamBuf()
  {
    itsStreamBuf.itsOwnerStream = this;
    this->init(&itsStreamBuf);
    itsStreamBuf.pubsetbuf(nullptr, 0);
  }

  ~MessageLoggerStream() override = default;

  virtual void onNewMessage(const string_type& strNewMessage) = 0;

 public:
  // our stream buffer
  handler_streambuf_type itsStreamBuf;
};

// ======================================================================
