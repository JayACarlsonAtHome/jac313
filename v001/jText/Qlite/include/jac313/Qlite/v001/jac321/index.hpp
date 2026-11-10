// Vendored from jac321 80445ad907d9, v001/include/jac321/index.hpp, by tools/vendor.sh.
// Do not edit here: change jac321, then vendor again. Namespace: jac313::Qlite::v001::jac321.
// The JAC_* macros are left out of this copy; call check<Policy>(...) directly.
// jac321 run-time bounded index: checked against a count known only at run time.
//
//   using ColumnIndex = jac321::Index<struct ColumnTag>;      // 0 <= i < count
//   using BindIndex   = jac321::Index<struct BindTag, 1>;     // 1 <= i <= count
//
//   ColumnIndex col{c, column_count()};   // checked once, here
//   int read(ColumnIndex col);            // needs no precondition
//
// The Tag makes each kind of index its own type, so a column index can not be
// passed where a bind index is wanted, and a 0-based one never mixes with a
// 1-based one.
//
// FirstIndexes<IndexT, N> checks count >= N once and then hands out the first N
// indexes with no more checks (one check for a row of N columns, not N).
//
// The limit: the guarantee is "in range when it was made". If the thing it
// indexes shrinks afterwards, the index can be out of range. Make it close to
// where it is used.
#ifndef QLITE_V001_JAC321_INDEX_HPP
#define QLITE_V001_JAC321_INDEX_HPP

#include "contract.hpp"
#include "predicates.hpp"
#include "text.hpp"

#include <cstddef>
#include <limits>
#include <source_location>

namespace jac313::Qlite::v001::jac321 {

template <text::Integer auto Base>
inline constexpr auto index_name_v = [] {
	if constexpr (Base == 0)
		return text::join(text::lit<"0 <= index < count">());
	else
		return text::join(text::int_chars<Base>(), text::lit<" <= index < ">(),
		                  text::int_chars<Base>(), text::lit<" + count">());
}();

template <class IndexT, std::size_t N>
class FirstIndexes;

template <class Tag, text::Integer auto Base = 0, RefinedPolicy P = Abort>
class Index {
	static_assert(Base >= 0, "Index: Base must not be negative");

public:
	using value_type = decltype(Base);
	using tag        = Tag;
	using policy     = P;
	static constexpr value_type base = Base;

private:
	template <class, std::size_t>
	friend class FirstIndexes;

	// Only FirstIndexes uses this: its one check already covers the value.
	struct Unchecked {};
	constexpr Index(Unchecked, value_type i) noexcept : value_(i) {}

public:
	static constexpr const char* name = index_name_v<Base>.data();

	// count: how many there are (any integer type; compared by value).
	template <text::Integer C>
	constexpr Index(value_type i, C count,
	                std::source_location loc = std::source_location::current())
	    : value_(i)
	{
		check<P>(
		    Kind::invariant,
		    [&] {
			    // i - Base is only computed once i >= Base, so it can not overflow.
			    return detail::less_equal(Base, i) && detail::less(i - Base, count);
		    },
		    name, loc);
	}

	constexpr value_type get() const noexcept { return value_; }
	constexpr operator value_type() const noexcept { return value_; }

private:
	value_type value_;
};

// One check that count >= N, then indexes 0..N-1 (Base..Base+N-1) with no more
// checks: at<I>() is proven by the compile-time I < N.
//
//   FirstIndexes<ColumnIndex, 3> cols{column_count()};   // one check
//   read(cols.at<0>()); read(cols.at<1>()); read(cols.at<2>());
template <class IndexT, std::size_t N>
class FirstIndexes {
	using value_type = typename IndexT::value_type;
	using policy     = typename IndexT::policy;
	static_assert(N <= static_cast<std::size_t>(std::numeric_limits<value_type>::max()),
	              "FirstIndexes: N does not fit the index type");

public:
	static constexpr const char* name = text::prefix_int_v<"count >= ", N>.data();

	template <text::Integer C>
	constexpr explicit FirstIndexes(C count,
	                                std::source_location loc = std::source_location::current())
	{
		check<policy>(Kind::invariant, [&] { return !detail::less(count, N); }, name, loc);
	}

	template <std::size_t I>
		requires(I < N)
	constexpr IndexT at() const noexcept
	{
		return IndexT{typename IndexT::Unchecked{},
		              static_cast<value_type>(IndexT::base + static_cast<value_type>(I))};
	}
};

} // namespace jac313::Qlite::v001::jac321

#endif
