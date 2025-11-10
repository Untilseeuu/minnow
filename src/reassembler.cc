#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Handle empty data
  if ( data.empty() ) {
    if ( is_last_substring ) {
      eof_received_ = true;
      eof_index_ = first_index;
      if ( next_index_ >= eof_index_ ) {
        output_.writer().close();
      }
    }
    return;
  }

  // Record EOF information
  if ( is_last_substring ) {
    eof_received_ = true;
    eof_index_ = first_index + data.length();
  }

  uint64_t last_index = first_index + data.length();
  uint64_t first_unacceptable = next_index_ + output_.writer().available_capacity();

  // Discard data completely before next_index_ or beyond capacity
  if ( last_index <= next_index_ || first_index >= first_unacceptable ) {
    if ( eof_received_ && next_index_ >= eof_index_ ) {
      output_.writer().close();
    }
    return;
  }

  // Trim data to fit within acceptable range [next_index_, first_unacceptable)
  if ( first_index < next_index_ ) {
    uint64_t trim_amount = next_index_ - first_index;
    data = data.substr( trim_amount );
    first_index = next_index_;
  }

  if ( last_index > first_unacceptable ) {
    data = data.substr( 0, first_unacceptable - first_index );
    last_index = first_unacceptable;
  }

  // Check if new data is completely covered by existing segments
  for ( const auto& seg : unassembled_ ) {
    uint64_t seg_first = seg.first;
    uint64_t seg_last = seg_first + seg.second.length();
    if ( seg_first <= first_index && seg_last >= last_index ) {
      // New data is completely covered, nothing to do
      if ( eof_received_ && next_index_ >= eof_index_ ) {
        output_.writer().close();
      }
      return;
    }
  }

  // Remove all segments that overlap with [first_index, last_index)
  auto it = unassembled_.begin();
  while ( it != unassembled_.end() ) {
    uint64_t seg_first = it->first;
    uint64_t seg_last = seg_first + it->second.length();

    // Check if there's any overlap
    if ( seg_first < last_index && seg_last > first_index ) {
      // There's overlap - we need to handle it

      // If existing segment extends before our new data
      if ( seg_first < first_index ) {
        // Keep the non-overlapping prefix
        string prefix = it->second.substr( 0, first_index - seg_first );
        unassembled_bytes_ -= it->second.length();
        it->second = prefix;
        unassembled_bytes_ += prefix.length();
        it++;
      } else {
        // Existing segment starts at or after our new data's start
        // If existing segment extends beyond our new data
        if ( seg_last > last_index ) {
          // Keep the non-overlapping suffix
          string suffix = it->second.substr( last_index - seg_first );
          unassembled_bytes_ -= it->second.length();
          // Remove the old entry and insert the suffix at the new position
          it = unassembled_.erase( it );
          unassembled_[last_index] = suffix;
          unassembled_bytes_ += suffix.length();
        } else {
          // Existing segment is completely covered by new data
          unassembled_bytes_ -= it->second.length();
          it = unassembled_.erase( it );
        }
      }
    } else {
      it++;
    }
  }

  // Insert the new segment
  if ( !data.empty() ) {
    unassembled_[first_index] = data;
    unassembled_bytes_ += data.length();
  }

  // Try to write as much as possible to the output stream
  while ( !unassembled_.empty() && unassembled_.begin()->first == next_index_ ) {
    auto& segment = unassembled_.begin()->second;
    output_.writer().push( segment );
    next_index_ += segment.length();
    unassembled_bytes_ -= segment.length();
    unassembled_.erase( unassembled_.begin() );
  }

  // Close the stream if we've received and written all data
  if ( eof_received_ && next_index_ >= eof_index_ ) {
    output_.writer().close();
  }
}

uint64_t Reassembler::count_bytes_pending() const
{
  return unassembled_bytes_;
}
