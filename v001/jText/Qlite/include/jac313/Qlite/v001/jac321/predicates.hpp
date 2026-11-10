// Vendored from jac321 80445ad907d9, v001/include/jac321/predicates.hpp, by tools/vendor.sh.
// Do not edit here: change jac321, then vendor again. Namespace: jac313::Qlite::v001::jac321.
// The JAC_* macros are left out of this copy; call check<Policy>(...) directly.
// jac321 predicates for Refined: a type with a compile-time name and a static
// holds(const T&). Names carry their template arguments ("at least 3") and
// combinations ("in range 0..100 and not greater than 50").
//
// Comparisons between integers use std::cmp_*, so signed and unsigned values
// compare by value: GreaterThan<-1>::holds(0u) is true.
#ifndef QLITE_V001_JAC321_PREDICATES_HPP
#define QLITE_V001_JAC321_PREDICATES_HPP

#include "text.hpp"

#include <type_traits>
#include <utility>

namespace jac313::Qlite::v001::jac321 {

namespace detail {

template <class A, class B>
constexpr bool
less(const A& a, const B& b)
{
	if constexpr (std::is_integral_v<A> && std::is_integral_v<B>)
		return std::cmp_less(a, b);
	else
		return a < b;
}

template <class A, class B>
constexpr bool
less_equal(const A& a, const B& b)
{
	return !less(b, a);
}

} // namespace detail

// ---- single conditions ----

struct NonEmpty {
	static constexpr const char* name = "non-empty";
	template <class C>
	static constexpr bool holds(const C& c) { return !c.empty(); }
};

// Pointers, smart pointers, std::function, handles: anything that tests as bool.
struct NotNull {
	static constexpr const char* name = "not null";
	template <class P>
	static constexpr bool holds(const P& p) { return static_cast<bool>(p); }
};

struct Positive {
	static constexpr const char* name = "positive";
	template <class N>
	static constexpr bool holds(const N& n) { return detail::less(N{}, n); }
};

struct NonNegative {
	static constexpr const char* name = "non-negative";
	template <class N>
	static constexpr bool holds(const N& n) { return detail::less_equal(N{}, n); }
};

template <text::Integer auto Min>
struct AtLeast {
	static constexpr const char* name = text::prefix_int_v<"at least ", Min>.data();
	template <class N>
	static constexpr bool holds(const N& n) { return detail::less_equal(Min, n); }
};

template <text::Integer auto Max>
struct AtMost {
	static constexpr const char* name = text::prefix_int_v<"at most ", Max>.data();
	template <class N>
	static constexpr bool holds(const N& n) { return detail::less_equal(n, Max); }
};

template <text::Integer auto Min>
struct GreaterThan {
	static constexpr const char* name = text::prefix_int_v<"greater than ", Min>.data();
	template <class N>
	static constexpr bool holds(const N& n) { return detail::less(Min, n); }
};

template <text::Integer auto Lo, text::Integer auto Hi>
inline constexpr auto in_range_name_v = text::join(
    text::lit<"in range ">(), text::int_chars<Lo>(), text::lit<"..">(), text::int_chars<Hi>());

template <text::Integer auto Lo, text::Integer auto Hi>
struct InRange {
	static_assert(detail::less_equal(Lo, Hi), "InRange: Lo must not be above Hi");
	static constexpr const char* name = in_range_name_v<Lo, Hi>.data();
	template <class N>
	static constexpr bool holds(const N& n)
	{
		return detail::less_equal(Lo, n) && detail::less_equal(n, Hi);
	}
};

// ---- combinations ----

// Compound predicates (And, Or) get parentheses when they are inside another one.
template <class P>
inline constexpr bool is_compound_v = false;

template <class P>
inline constexpr auto operand_chars_v = [] {
	if constexpr (is_compound_v<P>)
		return text::join_no_end(text::lit<"(">(), text::name_chars<P>(), text::lit<")">());
	else
		return text::name_chars<P>();
}();

template <class A, class B>
inline constexpr auto and_name_v =
    text::join(operand_chars_v<A>, text::lit<" and ">(), operand_chars_v<B>);

template <class A, class B>
inline constexpr auto or_name_v =
    text::join(operand_chars_v<A>, text::lit<" or ">(), operand_chars_v<B>);

template <class A>
inline constexpr auto not_name_v = text::join(text::lit<"not ">(), operand_chars_v<A>);

template <class A, class B>
struct And {
	static constexpr const char* name = and_name_v<A, B>.data();
	template <class T>
	static constexpr bool holds(const T& v) { return A::holds(v) && B::holds(v); }
};

template <class A, class B>
struct Or {
	static constexpr const char* name = or_name_v<A, B>.data();
	template <class T>
	static constexpr bool holds(const T& v) { return A::holds(v) || B::holds(v); }
};

template <class A>
struct Not {
	static constexpr const char* name = not_name_v<A>.data();
	template <class T>
	static constexpr bool holds(const T& v) { return !A::holds(v); }
};

template <class A, class B>
inline constexpr bool is_compound_v<And<A, B>> = true;
template <class A, class B>
inline constexpr bool is_compound_v<Or<A, B>> = true;

} // namespace jac313::Qlite::v001::jac321

#endif
