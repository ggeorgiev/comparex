#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <valarray>

#include "lib/profile/noop.hpp"
#include "record.hpp"

namespace myers {

template <typename Sq = std::string, typename R = Record, typename M = profile::Noop,
          typename Sz = uint32_t>
class Classic {
private:
  using sequence_type = Sq;
  using size_type = Sz;
  M memory_tracker;

  /**
   * Reconstruct the diff path by walking backwards through the track.
   *
   * @param a          First string
   * @param b          Second string
   * @param track      Recorded wavefronts for each iteration
   * @return           A list of edit operations (records) from start to finish
   */
  std::vector<R> backtrack(const sequence_type &a, const sequence_type &b,
                           std::vector<std::valarray<bool>> &track) {

    auto truns = track.rbegin();

    auto x = static_cast<size_type>(a.size());
    auto y = static_cast<size_type>(b.size());
    auto k = static_cast<size_type>(track.size() + x - y) / 2;

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
        if ((*truns)[k]) {
          records.push_back({'+', b.at(y - 1)});
          --y;
        } else {
          records.push_back({'-', a.at(x - 1)});
          --x;
          --k;
        }
        // After deciding direction, jump to the previous truns
        truns++;
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
  std::vector<R> compare(const sequence_type &a, const sequence_type &b) {
    auto m = static_cast<size_type>(a.size());
    auto n = static_cast<size_type>(b.size());

    // Phases ensures we can index wavefront by (phases + k - d)
    auto phases = m + n + 1;

    std::vector<size_type> wavefront(1, 0);
    std::vector<std::valarray<bool>> track;

    // At most m+n "phases" in the classic Myers
    for (size_type d = 1; d <= phases; ++d) {
      auto d2 = d * 2;
      std::valarray<bool> turns(d);
      wavefront.resize(wavefront.size() + 2, 1);
      for (size_type k = 1; k <= d2; k += 2) {
        auto up = wavefront[k + 1] = wavefront[k];
        auto left = wavefront[k - 1];
        size_type x = (turns[k/2] = up > left) ? up - 1 : left;
        size_type y = x + d - k;

        // Follow diagonals as far as possible
        for (; x < m && y < n && a[x] == b[y]; ++x, ++y)
          ;

        // Store the furthest x in the wavefront
        wavefront[k] = x + 1;

        // Early exit: if we've aligned both strings fully, backtrack
        if (x >= m && y >= n) {
          d = phases;
          break;
        }
      }

      // Save the current wavefront for backtracking
      memory_tracker += (turns.size() + 7) / 8;
      track.push_back(std::move(turns));
    }

    // Backtrack to build the edits
    return backtrack(a, b, track);
  }
};

} // namespace myers
