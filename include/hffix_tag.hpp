#ifndef HFFIX_TAG_HPP
#define HFFIX_TAG_HPP

#include <iosream>
#include <type_traits> // for enable_if

namespace hffix {
namespace detail {

// tag types for indicating which parameter type FIX tags accept
struct string_tag;
struct number_tag; // both integers and decimal values
struct bool_tag;
struct character_tag;
struct timestamp_tag;

} // namespace detail

/*!
 * \brief a FIX tag
 *
 * The tag_t type represent a FIX tag value. It can be constructed from an
 * int value and can be converted to an integer value. It's main purpose is to
 * provide additional type safety for building FIX messages.
*/
template <typename ParameterType = void>
class tag_t {
   friend class tag_t<void>;
public:

    constexpr explicit tag_t(int tag) : m_tag_value(tag) {}
    constexpr explicit operator int() const { return m_tag_value; }

    constexpr tag_t(tag_t const& tag) = default;
    tag_t& operator=(tag_t const& tag) = default;

    template <typename A, typename B>
    friend constexpr bool operator==(tag_t<A> const lhs, tag_t<B> const rhs);

    template <typename A, typename B>
    friend constexpr bool operator!=(tag_t<A> const lhs, tag_t<B> const rhs);

    // this is only here to support using tags as keys in associative containers
    template <typename A, typename B>
    friend constexpr bool operator<(tag_t<A> const lhs, tag_t<B> const rhs);

    friend std::ostream& operator<<(std::ostream& os, tag_t const t)
    { return os << t.m_tag_value; }

private:
    int m_tag_value;
};

template <>
class tag_t<void> {
   template <typename A> friend class tag_t;
public:

    constexpr explicit tag_t(int tag) : m_tag_value(tag) {}
    constexpr explicit operator int() const { return m_tag_value; }

    // implicit conversion to and from types tags
    template <typename A>
    constexpr tag_t(tag_t<A> const& tag) : m_tag_value(tag.m_tag_value) {}

    template <typename A>
    tag_t& operator=(tag_t<A> const& tag) { m_tag_value = tag.m_tag_value; return *this; }

    template <typename A, typename B>
    friend constexpr bool operator==(tag_t<A> const lhs, tag_t<B> const rhs);

    template <typename A, typename B>
    friend constexpr bool operator!=(tag_t<A> const lhs, tag_t<B> const rhs);

    // this is only here to support using tags as keys in associative containers
    template <typename A, typename B>
    friend constexpr bool operator<(tag_t<A> const lhs, tag_t<B> const rhs);

    friend std::ostream& operator<<(std::ostream& os, tag_t const t)
    { return os << t.m_tag_value; }

private:
    int m_tag_value;
};

template <typename A, typename B>
constexpr bool operator==(tag_t<A> const lhs, tag_t<B> const rhs)
{ return lhs.m_tag_value == rhs.m_tag_value; }

template <typename A, typename B>
constexpr bool operator!=(tag_t<A> const lhs, tag_t<B> const rhs)
{ return lhs.m_tag_value != rhs.m_tag_value; }

// this is only here to support using tags as keys in associative containers
template <typename A, typename B>
constexpr bool operator<(tag_t<A> const lhs, tag_t<B> const rhs)
{ return lhs.m_tag_value < rhs.m_tag_value; }

}

#endif

