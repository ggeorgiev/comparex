#pragma once

namespace profile {

class Noop {
public:
    Noop() = default;
    ~Noop() = default;
    Noop(const Noop&) = default;
    Noop(Noop&&) noexcept = default;
    Noop& operator=(const Noop&) = default;
    Noop& operator=(Noop&&) noexcept = default;

    void operator+=(size_t n) {
    }
};

} // namespace profile
