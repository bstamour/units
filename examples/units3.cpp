

#include "bits/meta.hpp"

#include <ratio>
#include <type_traits>

//==============================================================================
namespace units {

//------------------------------------------------------------------------------
// Forward declarations of types and concepts.

// Concepts.

template <typename Unit> struct is_unit;
template <typename Unit> struct is_affine_unit;
template <typename Unit> struct is_rational_unit;
template <typename Unit> struct is_base_unit;
template <typename Unit> struct is_derived_unit;

template <typename U> constexpr auto is_unit_v = is_unit<U>::value;
template <typename U>
constexpr auto is_affine_unit_v = is_affine_unit<U>::value;
template <typename U>
constexpr auto is_rational_unit_v = is_rational_unit<U>::value;
template <typename U> constexpr auto is_base_unit_v = is_base_unit<U>::value;
template <typename U>
constexpr auto is_derived_unit_v = is_derived_unit<U>::value;

// Types.

template <int Tag> struct base_unit;
template <typename BaseUnit, typename Scale, typename Shift>
struct interval_unit;
template <typename RationalUnit, typename Scale> struct scaled_unit;
template <typename... Values> struct product_unit;

template <typename T, typename UnitTag> class quantity;

// Helper types.

// Useful for defining product_units, e.g. <p<metre, 2>, p<second, -1>>;
template <typename Unit, int Power> struct p;

} // namespace units
//==============================================================================

//==============================================================================
namespace units {

//------------------------------------------------------------------------------
// Concept definitions.

template <typename Unit>
struct is_unit : std::bool_constant<is_affine_unit<Unit>::value ||
                                    is_rational_unit<Unit>::value> {};

//---

template <typename Unit> struct is_affine_unit : std::false_type {};

template <typename B, typename S, typename H>
struct is_affine_unit<interval_unit<B, S, H>> : std::true_type {};

//---

template <typename Unit> struct is_rational_unit : std::false_type {};

template <int t> struct is_rational_unit<base_unit<t>> : std::true_type {};

template <typename R, typename S>
struct is_rational_unit<scaled_unit<R, S>> : std::true_type {};

template <typename... Vs>
struct is_rational_unit<product_unit<Vs...>> : std::true_type {};

//---

template <typename Unit> struct is_base_unit : std::false_type {};

template <int t> struct is_base_unit<base_unit<t>> : std::true_type {};

//---

template <typename Unit>
struct is_derived_unit
    : std::bool_constant<is_unit<Unit>::value && !is_base_unit<Unit>::value> {};

//------------------------------------------------------------------------------
// Unit type definitions.

template <int Tag> struct base_unit { static int const tag = Tag; };

template <typename BaseUnit, typename Scale, typename Shift>
struct interval_unit : meta::inherit_if<is_base_unit_v<BaseUnit>> {
  using base_unit = BaseUnit;
  using scale = Scale;
  using Shift = Shift;
};

template <typename RationalUnit, typename Scale>
struct scaled_unit : meta::inherit_if<is_rational_unit_v<RationalUnit>> {
  using unit = RationalUnit;
  using scale = Scale;
};

} // namespace units
//==============================================================================

//==============================================================================
namespace units::detail {

//------------------------------------------------------------------------------
// Internal helper types.

template <typename BaseUnit, int Power>
struct unit_power_pair : meta::inherit_if<is_base_unit_v<BaseUnit>> {
  using unit = BaseUnit;
  static int const power = Power;
};

template <typename... PowerPairs>
using parsed_product_unit = meta::type_list<PowerPairs...>;

//---

template <typename Unit> struct parse_product_unit;

template <> struct parse_product_unit<product_unit<>> {
  using type = parsed_product_unit<>;
};

template <typename Unit, int P, typename... Params>
struct parse_product_unit<product_unit<p<Unit, P>, Params...>> {
  using type = typename meta::type_list_append<
      parsed_product_unit<unit_power_pair<Unit, P>>,
      typename parse_product_unit<product_unit<Params...>>::type>::type;
};

template <typename Param, typename... Params>
struct parse_product_unit<product_unit<Param, Params...>> {
  using type = typename meta::type_list_append<
      parsed_product_unit<unit_power_pair<Param, 1>>,
      typename parse_product_unit<product_unit<Params...>>::type>::type;
};

//------------------------------------------------------------------------------
// unit_tag types are a unique representation of a unit.
//
// Affine units are represented directly, as they cannot be further combined.
// Rational units are represented as their unique combination of unit power
// pairs, along with a scaling factor (for e.g. kilometres).

template <typename TagBody> struct unit_tag;

template <typename Scale, typename PowerPairList> struct rational_tag_body;

//---

// TODO: make_unit_tag<Unit>

} // namespace units::detail
//==============================================================================

struct si {
  using second = units::base_unit<0>;
  using metre = units::base_unit<1>;
};

int main()
{
    using t = units::product_unit<
      si::metre, units::p<si::second, -1>>;

    using u = typename units::detail::parse_product_unit<t>::type;

    units::meta::print_type<u>();
}
