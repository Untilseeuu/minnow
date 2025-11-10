#pragma once

#include "byte_stream.hh"

#include <map>
#include <string>

class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output )
    : output_( std::move( output ) ), next_index_( 0 ), unassembled_(), unassembled_bytes_( 0 ), eof_received_( false ), eof_index_( 0 )
  {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until they can be written.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  uint64_t count_bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }

private:
  ByteStream output_; // the Reassembler writes to this ByteStream

  // Private members to track reassembly state
  uint64_t next_index_ { 0 };                   // Next byte index expected to write
  std::map<uint64_t, std::string> unassembled_; // Buffer for out-of-order substrings
  uint64_t unassembled_bytes_ { 0 };            // Count of bytes in unassembled buffer
  bool eof_received_ { false };                 // Whether we've seen the last substring
  uint64_t eof_index_ { 0 };                    // Index where EOF should be
};
