#include <vector>
#include <string>
#include <algorithm>

namespace myers {

struct Record {
    char op;
    char symbol;
};

template <typename R = Record>
std::vector<R> compare(const std::string& a, const std::string& b) {
    size_t m = a.length();
    size_t n = b.length();

    size_t offset = std::max(m, n);
    std::vector<int> v(2 * offset + 1, 0);
    
    int x = 0, y = 0;
    
    for (int d = 0; d <= m + n; ++d) {
        for (int k = -d; k <= d; k += 2) {
            if (k == -d || (k != d && v[offset + k - 1] < v[offset + k + 1])) {
                // down, delete from a
                x = v[offset + k + 1];
            } else {
                // right, insert from b
                x = v[offset + k - 1] + 1;
            }
            y = x - k;

            if (x < m && y < n && a[x] == b[y]) {
                // match, move diagonally
                ++x; 
                ++y;
            }
            v[offset+k] = x;

            if (x >= m && y >= n) {
                std::vector<Record> records;

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
                        if (v[offset + k - 1] < v[offset + k + 1]) {
                            records.push_back({'-', a[x - 1]});
                            --x;
                        } else {
                            records.push_back({'+', b[y - 1]});
                            --y;
                        }
                    }
                }
                reverse(records.begin(), records.end());
                return records;
            }
        }
    }
    
    return {};
}

}