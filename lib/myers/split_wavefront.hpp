#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "record.hpp"
#include "lib/profile/noop.hpp"

namespace myers {

template <typename R = Record,
          typename M = profile::Noop>
class SplitWavefront {
private:
    M memory_tracker;

public:
    size_t memory() {
        return memory_tracker.number();
    }

    std::vector<R> compare(const std::string& a, const std::string& b) {
        int m = a.length();
        int n = b.length();

        int offset = m+n;
        std::vector<int> wavefront(2 * offset + 1, 0);
        std::vector<std::vector<int>> track;

        memory_tracker += sizeof(int) * wavefront.size();
        
        for (int d = 0; d <= m + n; ++d) {
            for (int k = -d; k <= d; k += 2) {
                int waveIndex = offset + k;

                int x;
                if (k == -d || (k != d && wavefront[waveIndex - 1] < wavefront[waveIndex + 1])) {
                    // down, delete from a
                    x = wavefront[waveIndex + 1];
                } else {
                    // right, insert from b
                    x = wavefront[waveIndex - 1] + 1;
                }
                int y = x - k;

                while (x < m && y < n && a[x] == b[y]) {
                    // match, move diagonally
                    ++x; 
                    ++y;
                }
                wavefront[waveIndex] = x;

                if (x >= m && y >= n) {
                    std::vector<Record> records;

                    waveIndex = x - y + offset;

                    // We've found the path to the end
                    while (x > 0 || y > 0) {
                        if (x == 0) {
                            records.push_back({'+', b[y - 1]});
                            --y;
                        } else if (y == 0) {
                            records.push_back({'-', a[x - 1]});
                            --x;
                        } else if (a[x - 1] == b[y - 1]) {
                            records.push_back({' ', a[x - 1]});
                            --x;
                            --y;
                        } else {
                            if (wavefront[waveIndex - 1] > wavefront[waveIndex + 1]) {
                                records.push_back({'-', a[x - 1]});
                                --x;
                                --waveIndex;
                            } else {
                                records.push_back({'+', b[y - 1]});
                                --y;
                                ++waveIndex;
                            }
                            wavefront = track.back();
                            track.pop_back();

                        }
                    }
                    reverse(records.begin(), records.end());
                    return records;
                }
            }
            track.push_back(wavefront);
            memory_tracker += sizeof(int) * wavefront.size();
        }
        
        return {};
    }
};

}