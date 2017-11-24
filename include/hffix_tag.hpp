#ifndef HFFIX_TAG_HPP
#define HFFIX_TAG_HPP

#include <iostream>
#include <type_traits> // for enable_if

namespace hffix {

/*!
 * \brief a FIX tag
 *
 * The tag_t type represent a FIX tag value. It can be constructed from an
 * int value and can be converted to an integer value. It's main purpose is to
 * provide additional type safety to distinguish tags from any other int.
*/
class tag_t {
public:

    constexpr explicit tag_t(int tag) : m_tag_value(tag) {}
    constexpr explicit operator int() const { return m_tag_value; }

    constexpr tag_t(tag_t const& tag) = default;
    tag_t& operator=(tag_t const& tag) = default;

    friend constexpr bool operator==(tag_t const lhs, tag_t const rhs)
    { return lhs.m_tag_value == rhs.m_tag_value; }

    friend constexpr bool operator!=(tag_t const lhs, tag_t const rhs)
    { return lhs.m_tag_value != rhs.m_tag_value; }

    // this is only here to support using tags as keys in associative containers
    friend constexpr bool operator<(tag_t const lhs, tag_t const rhs)
    { return lhs.m_tag_value < rhs.m_tag_value; }

    friend std::ostream& operator<<(std::ostream& os, tag_t const t)
    { return os << t.m_tag_value; }

private:
    int m_tag_value;
};

}

#endif

