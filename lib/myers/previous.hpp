#pragma once

#include <algorithm>
#include <cstdint>
#include <list>
#include <string>
#include <valarray>
#include <vector>

#include "lib/profile/noop.hpp"
#include "record.hpp"

namespace myers {

template <typename Sq = std::string, typename R = Record, typename M = profile::Noop,
          typename Sz = uint32_t>
class Previous {
private:
  using sequence_type = Sq;
  using size_type = Sz;
  M memory_tracker;

  std::vector<R> backtrack(const sequence_type &a, const sequence_type &b,
                           std::vector<std::valarray<bool>> &track) {
    auto truns = track.rbegin();
    auto x = static_cast<size_type>(a.size());
    auto y = static_cast<size_type>(b.size());
    auto k = static_cast<size_type>(track.size() + x - y) / 2;

    std::vector<R> records;
    while (x > 0 || y > 0) {
      if (x == 0) {
        records.push_back({'+', b.at(y - 1)});
        --y;
      } else if (y == 0) {
        records.push_back({'-', a.at(x - 1)});
        --x;
      } else if (a.at(x - 1) == b.at(y - 1)) {
        records.push_back({' ', a.at(x - 1)});
        --x;
        --y;
      } else {
        if ((*truns)[k]) {
          records.push_back({'+', b.at(y - 1)});
          --y;
        } else {
          records.push_back({'-', a.at(x - 1)});
          --x;
          --k;
        }
        truns++;
      }
    }
    std::reverse(records.begin(), records.end());
    return records;
  }

public:
  size_t memory() { return memory_tracker.number(); }

  std::vector<R> compare(const sequence_type &a, const sequence_type &b) {
    auto m = static_cast<size_type>(a.size());
    auto n = static_cast<size_type>(b.size());
    auto phases = m + n + 1;

    std::vector<std::valarray<bool>> track;

    std::vector<size_type> wavefront(1, 0);
    std::vector<size_type> echelon(1, 0);
    echelon.push_back(1);

    for (size_type d = 0; d < phases; ++d) {
      std::valarray<bool> turns(d + 1);
      wavefront.resize(echelon.size() + 1, 1);

      size_type k_start = (d < n) ? 0 : d - n;
      size_type k_end = (d < m) ? d : m;

      for (size_type k = k_start, k2 = k_start * 2; k <= k_end; k2 += 2) {
        size_type left = echelon[k];
        size_type up = echelon[k + 1];
        size_type x = (turns[k] = up > left) ? up - 1 : left;
        size_type y = x + d - k2;

        // Follow diagonals as far as possible
        for (; x < m && y < n && a[x] == b[y]; ++x, ++y)
          ;

        wavefront[++k] = x + 1;

        if (x >= m && y >= n) {
          d = phases; // Force outer loop exit
          break;
        }
      }

      memory_tracker += (turns.size() + 7) / 8;
      track.push_back(std::move(turns));
      std::swap(echelon, wavefront);
    }

    return backtrack(a, b, track);
  }
};

} // namespace myers