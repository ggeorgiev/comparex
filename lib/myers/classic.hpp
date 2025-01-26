#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "lib/profile/noop.hpp"
#include "record.hpp"

namespace myers {

template <typename R = Record, typename M = profile::Noop, typename S = uint32_t>
class Classic {
private:
  using size_type = S;
  M memory_tracker;

  /**
   * Reconstruct the diff path by walking backwards through the track.
   *
   * @param a          First string
   * @param b          Second string
   * @param track      Recorded wavefronts for each iteration
   * @return           A list of edit operations (records) from start to finish
   */
  std::vector<R> backtrack(const std::string &a,
                           const std::string &b,
                           std::vector<std::vector<size_type>> &track) {

    std::vector<size_type> wavefront = track.back();
    track.pop_back();

    auto x = static_cast<size_type>(a.size());
    auto y = static_cast<size_type>(b.size());
    // We start from the waveIndex that corresponds to (k = x - y). This is
    // simply the farthest diagonal in the last iteration, though the precise
    // waveIndex might shift depending on how you store wavefronts.
    auto waveIndex = 2 * x + 1;

    std::vector<R> records;
    // Walk backwards while we still have characters to match/insert/delete
    while (x > 0 || y > 0) {
      if (x == 0) {
        // Nothing left in 'a', must have come from inserting into 'a'
        records.push_back({'+', b.at(y - 1)});
        --y;
      } else if (y == 0) {
        // Nothing left in 'b', must have come from deleting from 'a'
        records.push_back({'-', a.at(x - 1)});
        --x;
      } else if (a.at(x - 1) == b.at(y - 1)) {
        // Characters matched, so this is a diagonal move
        records.push_back({' ', a.at(x - 1)});
        --x;
        --y;
      } else {
        // Mismatch: either a deletion from 'a' or an insertion from 'b'
        if (wavefront.at(waveIndex - 1) > wavefront.at(waveIndex + 1)) {
          // Down: came from the "delete" branch
          records.push_back({'-', a.at(x - 1)});
          --x;
          --waveIndex;
        } else {
          // Right: came from the "insert" branch
          records.push_back({'+', b.at(y - 1)});
          --y;
          ++waveIndex;
        }
        // After deciding direction, jump to the previous wavefront
        wavefront = track.back();
        track.pop_back();
      }
    }
    // The reconstruction is backwards; flip it to get the final sequence
    std::reverse(records.begin(), records.end());
    return records;
  }

public:
  size_t memory() { return memory_tracker.number(); }

  /**
   * Compute the difference between two strings @p a and @p b.
   * Return the list of edit operations needed to transform @p a into @p b.
   */
  std::vector<R> compare(const std::string &a, const std::string &b) {
    auto m = static_cast<size_type>(a.size());
    auto n = static_cast<size_type>(b.size());

    // Phases ensures we can index wavefront by (phases + k - d)
    auto phases = m + n + 1;

    // Single wavefront array for the current iteration
    // Size of wavefront = 2 * phases + 1 is enough to store all possible diagonals
    std::vector<size_type> wavefront(2 * phases + 1, 0);
    memory_tracker += sizeof(size_type) * wavefront.size();

    // Keep track of each iteration’s wavefront for backtracking
    std::vector<std::vector<size_type>> track;
    track.reserve(phases); // optional: reduce re-allocations

    // At most m+n "phases" in the classic Myers
    for (size_type d = 0; d < phases; ++d) {
      auto d2 = d * 2;
      for (size_type k = 0; k <= d2; k += 2) {
        auto waveIndex = phases + k - d;

        // Decide whether we come from "down" (delete) or "right" (insert)
        size_type x = (k == 0 ||
                 (k != d2 && wavefront[waveIndex - 1] < wavefront[waveIndex + 1]))
                    ? wavefront[waveIndex + 1]
                    : wavefront[waveIndex - 1] + 1;
        size_type y = x + d - k;

        // Follow diagonals as far as possible
        for (;x < m && y < n && a[x] == b[y]; ++x, ++y);

        // Store the furthest x in the wavefront
        wavefront[waveIndex] = x;

        // Early exit: if we've aligned both strings fully, backtrack
        if (x >= m && y >= n) {
          d = phases;
          break;
        }
      }

      // Save the current wavefront for backtracking
      track.push_back(wavefront);
      memory_tracker += sizeof(int) * wavefront.size();
    }

    // Backtrack to build the edits
    return backtrack(a, b, track);
  }
};

} // namespace myers
