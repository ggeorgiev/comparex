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

        int offset = m + n;
        std::vector<int> wavefront(2 * offset + 1, 0);

        memory_tracker += sizeof(int) * wavefront.size();

        int x = 0, y = 0;

        for (int d = 0; d <= m + n; ++d) {

            for (int i = 0; i <= 2*d; i += 2) {
                int k = i - d;
                int waveIndex = offset + k;

                if (i == 0
                    || (i != 2*d
                        && wavefront[waveIndex - 1] < wavefront[waveIndex + 1])) {
                    // down, delete from a
                    x = wavefront[waveIndex + 1];
                } else {
                    // right, insert from b
                    x = wavefront[waveIndex - 1] + 1;
                }
                y = x - k;

                // Match (extend diagonal) if possible
                if (x < m && y < n && a[x] == b[y]) {
                    ++x;
                    ++y;
                }

                // Store the furthest reach for diagonal k
                wavefront[waveIndex] = x;

                // If we've reached the end, reconstruct
                if (x >= m && y >= n) {
                    std::vector<Record> records;

                    // Reconstruct the path
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
                            // Decide insert vs delete
                            // (same check as above, but in reverse order)
                            if (wavefront[waveIndex - 1] < wavefront[waveIndex + 1]) {
                                records.push_back({'-', a[x - 1]});
                                --x;
                            } else {
                                records.push_back({'+', b[y - 1]});
                                --y;
                            }
                        }
                    }

                    std::reverse(records.begin(), records.end());
                    return records;
                }
            }
        }

        // In case no edits found (empty strings, or fallback)
        return {};
    }
};

} // namespace myers
