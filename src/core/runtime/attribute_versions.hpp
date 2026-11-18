#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace JX3DPS::runtime {
using AttributeVersionId = std::uint32_t;

// Fight-local immutable values. Reserve once per worker; never grow in a fight.
template <class T> class AttributeVersions {
public:
    void Reserve(std::size_t capacity) {
        if (!capacity || capacity >= std::numeric_limits<AttributeVersionId>::max())
            throw std::invalid_argument("invalid attribute version capacity");
        values_.reserve(capacity);
    }
    void Clear() { values_.clear(); }
    AttributeVersionId Append(T value) {
        if (values_.size() == values_.capacity())
            throw std::length_error("attribute version capacity exhausted");
        const auto id = static_cast<AttributeVersionId>(values_.size());
        values_.push_back(std::move(value));
        return id;
    }
    const T &operator[](AttributeVersionId id) const { return values_[id]; }
    std::size_t Size() const { return values_.size(); }
    std::size_t Capacity() const { return values_.capacity(); }
    auto begin() const { return values_.begin(); }
    auto end() const { return values_.end(); }
private:
    std::vector<T> values_;
};
}
