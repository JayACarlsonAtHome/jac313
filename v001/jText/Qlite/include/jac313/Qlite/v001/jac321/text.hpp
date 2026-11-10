// Vendored from jac321 80445ad907d9, v001/include/jac321/text.hpp, by tools/vendor.sh.
// Do not edit here: change jac321, then vendor again. Namespace: jac313::Qlite::v001::jac321.
// The JAC_* macros are left out of this copy; call check<Policy>(...) directly.
// jac321 compile-time text: small helpers that build names at compile time,
// so a predicate's name can carry its template arguments ("in range 0..100").
//
//   text::lit<"greater than ">()   -> std::array<char, 13>   (no '\0')
//   text::int_chars<-42>()         -> std::array<char, 3>    "-42"
//   text::join(a, b, ...)          -> std::array<char, N+1>  with '\0' at the end
//   text::join_no_end(a, b, ...)   -> std::array<char, N>    (a piece again)
//   text::name_chars<Pred>()       -> Pred::name as a piece
//
// Keep the joined result in an inline constexpr variable (like prefix_int_v) so its
// .data() is a pointer to static storage that is valid as a constant.
#ifndef QLITE_V001_JAC321_TEXT_HPP
#define QLITE_V001_JAC321_TEXT_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>

namespace jac313::Qlite::v001::jac321::text {

// A string literal usable as a template argument.
template <std::size_t N>
struct Lit {
	char s[N]{};
	constexpr Lit(const char (&a)[N]) noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
			s[i] = a[i];
	}
	static constexpr std::size_t size = N - 1; // without the '\0'
};

template <Lit L>
constexpr auto
lit() noexcept
{
	std::array<char, L.size> out{};
	for (std::size_t i = 0; i < L.size; ++i)
		out[i] = L.s[i];
	return out;
}

template <class T>
concept Integer = std::integral<T> && !std::same_as<T, bool>;

// The size of a number without its sign, computed so the lowest value (INT_MIN)
// does not overflow.
template <Integer auto V>
constexpr auto
magnitude() noexcept
{
	using U = std::make_unsigned_t<decltype(V)>;
	return V < 0 ? static_cast<U>(U{0} - static_cast<U>(V)) : static_cast<U>(V);
}

template <Integer auto V>
constexpr std::size_t
int_width() noexcept
{
	auto        m = magnitude<V>();
	std::size_t w = 1;
	while (m >= 10) {
		m /= 10;
		++w;
	}
	return V < 0 ? w + 1 : w;
}

template <Integer auto V>
constexpr auto
int_chars() noexcept
{
	std::array<char, int_width<V>()> out{};
	auto        m = magnitude<V>();
	std::size_t i = out.size();
	do {
		out[--i] = static_cast<char>('0' + static_cast<int>(m % 10));
		m /= 10;
	} while (m != 0);
	if (V < 0)
		out[0] = '-';
	return out;
}

// The pieces back to back, without a '\0' (to use as a piece again).
template <std::size_t... N>
constexpr auto
join_no_end(const std::array<char, N>&... parts) noexcept
{
	std::array<char, (N + ... + 0)> out{};
	std::size_t i = 0;
	((
	     [&] {
		     for (char c : parts)
			     out[i++] = c;
	     }()),
	 ...);
	return out;
}

// The pieces back to back with a '\0' at the end (to use as a name).
template <std::size_t... N>
constexpr auto
join(const std::array<char, N>&... parts) noexcept
{
	return join_no_end(parts..., std::array<char, 1>{'\0'});
}

constexpr std::size_t
length(const char* s) noexcept
{
	std::size_t n = 0;
	while (s[n] != '\0')
		++n;
	return n;
}

// A predicate's name (any type with a constant const char* name) as a piece.
template <class P>
constexpr auto
name_chars() noexcept
{
	std::array<char, length(P::name)> out{};
	for (std::size_t i = 0; i < out.size(); ++i)
		out[i] = P::name[i];
	return out;
}

// "<Prefix><V>" with a '\0', in static storage.
template <Lit Prefix, Integer auto V>
inline constexpr auto prefix_int_v = join(lit<Prefix>(), int_chars<V>());

} // namespace jac313::Qlite::v001::jac321::text

#endif
