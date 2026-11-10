// Vendored from jac321 80445ad907d9, v001/include/jac321/contract.hpp, by tools/vendor.sh.
// Do not edit here: change jac321, then vendor again. Namespace: Qlite::v001::jac321.
// The JAC_* macros are left out of this copy; call check<Policy>(...) directly.
// jac321 library contracts: no compiler contract support needed.
//
// One check function, a violation record, and the violation POLICY as a template
// parameter (a type, not a function pointer: no type erasure, chosen at compile
// time per call site).
//
//
// The condition is passed as a lambda, so a policy that does not evaluate
// (Ignore) skips it entirely, side effects included.
//
// In constant evaluation a failed check is a compile error, so a constexpr or
// consteval caller that breaks a contract does not build.
#ifndef QLITE_V001_JAC321_CONTRACT_HPP
#define QLITE_V001_JAC321_CONTRACT_HPP

#include <concepts>
#include <cstdio>
#include <cstdlib>
#include <source_location>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace Qlite::v001::jac321 {

enum class Kind { pre, post, invariant, assertion };

constexpr const char*
kind_name(Kind k) noexcept
{
	switch (k) {
	case Kind::pre:       return "precondition";
	case Kind::post:      return "postcondition";
	case Kind::invariant: return "invariant";
	case Kind::assertion: return "assertion";
	}
	return "contract";
}

struct Violation {
	Kind                 kind;
	const char*          expr;
	std::source_location loc;
};

inline std::string
describe(const Violation& v)
{
	return std::string("jac321 ") + kind_name(v.kind) + " violated: (" + v.expr + ") at "
	       + v.loc.file_name() + ":" + std::to_string(v.loc.line()) + " in "
	       + v.loc.function_name();
}

inline void
report(const Violation& v) noexcept
{
	std::fprintf(stderr, "jac321 %s violated: (%s) at %s:%u in %s\n", kind_name(v.kind),
	             v.expr, v.loc.file_name(), static_cast<unsigned>(v.loc.line()),
	             v.loc.function_name());
}

class ContractError : public std::logic_error {
public:
	explicit ContractError(const Violation& v) : std::logic_error(describe(v)), violation_(v) {}
	const Violation& violation() const noexcept { return violation_; }

private:
	Violation violation_;
};

// ---- policies: evaluate, stops, and static on_violation(const Violation&) ----

struct Abort {
	static constexpr bool evaluate = true;
	static constexpr bool stops    = true;
	[[noreturn]] static void on_violation(const Violation& v) noexcept
	{
		report(v);
		std::abort();
	}
};

struct Throw {
	static constexpr bool evaluate = true;
	static constexpr bool stops    = true;
	[[noreturn]] static void on_violation(const Violation& v) { throw ContractError(v); }
};

struct ReportContinue {
	static constexpr bool evaluate = true;
	static constexpr bool stops    = false;
	static void on_violation(const Violation& v) noexcept { report(v); }
};

// Ignore never evaluates the condition: no cost, no side effects, and no
// compile-time check either.
struct Ignore {
	static constexpr bool evaluate = false;
	static constexpr bool stops    = false;
	static void on_violation(const Violation&) noexcept {}
};

// BuildOff: what a build-wide "contracts off" constant selects (never chosen at
// a call site). Like Ignore it evaluates nothing; unlike Ignore, Refined and Index
// accept it, because the whole build, not one call, decided to drop the checks.
struct BuildOff {
	static constexpr bool evaluate  = false;
	static constexpr bool stops     = false;
	static constexpr bool build_off = true;
	static void on_violation(const Violation&) noexcept {}
};

// evaluate: the condition is checked at all (Ignore: no).
// stops:    a violation never returns normally (Abort, Throw: yes).
template <class P>
concept Policy = requires(const Violation& v) {
	{ P::evaluate } -> std::convertible_to<bool>;
	{ P::stops } -> std::convertible_to<bool>;
	P::on_violation(v);
};

// A policy under which code after a check can rely on the condition.
template <class P>
concept StoppingPolicy = Policy<P> && P::evaluate && P::stops;

template <class P>
concept BuildOffPolicy = Policy<P> && requires { requires P::build_off; };

// What Refined and Index take: checks that stop, or the build switched them off.
template <class P>
concept RefinedPolicy = StoppingPolicy<P> || BuildOffPolicy<P>;

// The condition: a callable returning bool, called only if the policy evaluates.
template <class C>
concept Condition = std::is_invocable_r_v<bool, C&>;

// Not constexpr on purpose: reaching it during constant evaluation is the
// compile error, and its name is what the compiler prints.
inline void
contract_violated_during_constant_evaluation() noexcept
{}

template <Policy P = Abort, Condition C>
constexpr void
check(Kind k, C&& cond, const char* expr,
      std::source_location loc = std::source_location::current())
{
	if constexpr (P::evaluate) {
		if (cond())
			return;
		if consteval {
			contract_violated_during_constant_evaluation();
		} else {
			P::on_violation(Violation{k, expr, loc});
		}
	}
}

} // namespace Qlite::v001::jac321


#endif
