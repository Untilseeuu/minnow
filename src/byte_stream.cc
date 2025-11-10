#include "byte_stream.hh"
template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused **/){}
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

// Push data to stream, but only as much as available capacity allows.
void Writer::push( string data )
{
  // Your code here (and in each method below)
  //debug( "Writer::push({}) not yet implemented", data );
  uint64_t len = data.length();
  if ( len > capacity_ - buffer.size() ) {
    len = capacity_ - buffer.size();
  }
  for ( uint64_t i = 0; i < len; ++i ) {
    buffer.push_back( data[i] );
    total_bytes_pushed++;
  }
  return;
}

// Signal that the stream has reached its ending. Nothing more will be written.
void Writer::close()
{
  //debug( "Writer::close() not yet implemented" );
  closed = true;
}

// Has the stream been closed?
bool Writer::is_closed() const
{
  //debug( "Writer::is_closed() not yet implemented" );
  return closed; // Your code here.
}

// How many bytes can be pushed to the stream right now?
uint64_t Writer::available_capacity() const
{
  //debug( "Writer::available_capacity() not yet implemented" );
  //return {}; // Your code here.
  return capacity_ - buffer.size();
}

// Total number of bytes cumulatively pushed to the stream
uint64_t Writer::bytes_pushed() const
{
  // debug( "Writer::bytes_pushed() not yet implemented" );
  // return {}; // Your code here.
  return total_bytes_pushed;
}

// Peek at the next bytes in the buffer -- ideally as many as possible.
// It's not required to return a string_view of the *whole* buffer, but
// if the peeked string_view is only one byte at a time, it will probably force
// the caller to do a lot of extra work.
string_view Reader::peek() const
{
  // debug( "Reader::peek() not yet implemented" );
  // return {}; // Your code here.
  if ( !buffer.empty() ) {
    return std::string_view( &buffer.front(), 1 ); // 返回deque的front元素的string_view
  }
  return std::string_view(); // 返回一个默认构造的string_view（空的）
}

// Remove `len` bytes from the buffer.
void Reader::pop( uint64_t len )
{
  // debug( "Reader::pop({}) not yet implemented", len );
  if ( buffer.size() < len ) {
    len = buffer.size();
  }
  for ( uint64_t i = 0; i < len; ++i ) {
    buffer.pop_front();
    total_bytes_poped++;
  }
}

// Is the stream finished (closed and fully popped)?
bool Reader::is_finished() const
{
  // debug( "Reader::is_finished() not yet implemented" );
  // return {}; // Your code here.
  return closed && buffer.empty();
}

// Number of bytes currently buffered (pushed and not popped)
uint64_t Reader::bytes_buffered() const
{
  // debug( "Reader::bytes_buffered() not yet implemented" );
  // return {}; // Your code here.
  return buffer.size();
}

// Total number of bytes cumulatively popped from stream
uint64_t Reader::bytes_popped() const
{
  // debug( "Reader::bytes_popped() not yet implemented" );
  // return {}; // Your code here.
  return total_bytes_poped;
}
