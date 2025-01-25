#include <cstddef> 

namespace profile {

class Track {
private:
    size_t m_number = 0;
public:
    Track() = default;
    ~Track() = default;
    Track(const Track&) = default;
    Track(Track&&) noexcept = default;
    Track& operator=(const Track&) = default;
    Track& operator=(Track&&) noexcept = default;

    void operator+=(size_t n) {
        m_number += n;
    };

    size_t number() {
        return m_number;
    }
};

}