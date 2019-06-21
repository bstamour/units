//==============================================================================

#include "bits/meta.hpp"

#include <ratio>
#include <type_traits>

#include <iostream>

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
  using shift = Shift;
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

template <typename TagBody> struct unit_tag { using tag = TagBody; };

template <typename Unit> struct affine_tag_body { using unit = Unit; };

template <typename Scale, typename PowerPairList> struct rational_tag_body {
  using scale = Scale;
  using unit_list = PowerPairList;
};

template <typename UnitTag> struct is_affine_tag : std::false_type {};

template <typename Unit>
struct is_affine_tag<affine_tag_body<Unit>> : std::true_type {};

//------------------------------------------------------------------------------

template <typename P1, typename P2> struct comp {
  static const bool value = P1::unit::tag < P2::unit::tag;
};

template <typename P1, typename P2> struct merge_add {
  using type = unit_power_pair<typename P1::unit, P1::power + P2::power>;
};

template <typename P1, typename P2> struct merge_sub {
  using type = unit_power_pair<typename P1::unit, P1::power - P2::power>;
};

template <typename P> struct invert_power {
  using type = unit_power_pair<typename P::unit, -1 * P::power>;
};

template <typename P> struct is_power_zero {
  static const bool value = P::power == 0;
};

//------------------------------------------------------------------------------

template <typename Unit> struct flatten_and_scale;

template <int tag> struct flatten_and_scale<base_unit<tag>> {
  using base_unit_list = meta::type_list<unit_power_pair<base_unit<tag>, 1>>;

  using ratio = std::ratio<1, 1>;
};

template <typename Scale, typename Unit>
struct flatten_and_scale<scaled_unit<Unit, Scale>> {
  using base_unit_list = typename flatten_and_scale<Unit>::base_unit_list;

  using ratio =
      std::ratio_multiply<Scale, typename flatten_and_scale<Unit>::ratio>;
};

template <> struct flatten_and_scale<parsed_product_unit<>> {
  using base_unit_list = meta::type_list<>;

  using ratio = std::ratio<1, 1>;
};

template <typename Pair, typename... Pairs>
struct flatten_and_scale<parsed_product_unit<Pair, Pairs...>> {
  using pair_units =
      typename flatten_and_scale<typename Pair::unit>::base_unit_list;

  template <typename P> struct op {
    using type = unit_power_pair<typename P::unit, P::power * Pair::power>;
  };

  // Transform and sort Pair's units.
  using updated_units = typename meta::sort_type_list<
      comp, typename meta::type_list_map<op, pair_units>::type>::type;

  // Fetch and sort the remaining items.
  using rest_of_units = typename meta::sort_type_list<
      comp, typename flatten_and_scale<
                parsed_product_unit<Pairs...>>::base_unit_list>::type;

  // Merge them together.
  using base_unit_list = typename meta::type_list_remove_if<
      is_power_zero,
      typename meta::type_list_merge_with<comp, merge_add, updated_units,
                                          rest_of_units>::type>::type;

  using ratio = std::ratio_multiply<
      typename meta::ratio_power<
          typename flatten_and_scale<typename Pair::unit>::ratio,
          Pair::power>::type,
      typename flatten_and_scale<parsed_product_unit<Pairs...>>::ratio>;
};

template <typename... Values>
struct flatten_and_scale<product_unit<Values...>> {
  using impl = flatten_and_scale<
      typename parse_product_unit<product_unit<Values...>>::type>;

  using base_unit_list = typename impl::base_unit_list;
  using ratio = typename impl::ratio;
};

//---

template <typename Unit> struct make_affine_unit_tag {
  using type = affine_tag_body<Unit>;
};

template <typename Unit> struct make_rational_unit_tag {
  using impl = flatten_and_scale<Unit>;

  using type =
      rational_tag_body<typename impl::ratio, typename impl::base_unit_list>;
};

template <typename Unit>
struct make_unit_tag
    : std::conditional_t<is_affine_unit_v<Unit>, make_affine_unit_tag<Unit>,
                         make_rational_unit_tag<Unit>> {};

} // namespace units::detail
//==============================================================================

//==============================================================================
namespace units {

template <typename T, typename UnitTag> class quantity {
public:
  using value_type = T;
  using tag_type = UnitTag;

  static const bool is_affine = detail::is_affine_tag<tag_type>::value;
  static const bool is_rational = !is_affine;

  explicit constexpr quantity(value_type const &v) : value_{v} {}
  constexpr auto get() const { return value_; }

private:
  value_type value_;
};

//------------------------------------------------------------------------------

// Base case.

template <typename Q1, typename Q2, typename = void>
struct quantity_convertible : std::false_type {};

// Both affine.

template <typename Q1, typename Q2>
struct quantity_convertible<
    Q1, Q2,
    std::enable_if_t<Q1::is_affine && Q2::is_affine &&
                     std::is_same_v<typename Q1::tag_type::unit::base_unit,
                                    typename Q2::tag_type::unit::base_unit>>>
    : std::true_type {};

// First affine.

template <typename Q1, typename Q2>
struct quantity_convertible<
    Q1, Q2,
    std::enable_if_t<
        Q1::is_affine && Q2::is_rational &&
        meta::type_list_length<typename Q2::tag_type::unit_list>::value == 1 &&
        std::is_same_v<typename Q1::tag_type::unit::base_unit,
                       typename meta::type_list_head<
                           typename Q2::tag_type::unit_list>::type::unit>>>
    : std::true_type {};

// Second affine.

template <typename Q1, typename Q2>
struct quantity_convertible<Q1, Q2,
                            std::enable_if_t<Q1::is_rational && Q2::is_affine>>
    : quantity_convertible<Q2, Q1> {};

// Both rational.

template <typename Q1, typename Q2>
struct quantity_convertible<
    Q1, Q2,
    std::enable_if_t<Q1::is_rational && Q2::is_rational &&
                     std::is_same_v<typename Q1::tag_type::unit_list,
                                    typename Q2::tag_type::unit_list>>>
    : std::true_type {};

template <typename Q1, typename Q2>
constexpr bool quantity_convertible_v = quantity_convertible<Q1, Q2>::value;

//------------------------------------------------------------------------------

template <typename Unit, typename T> constexpr auto quantity_of(T const &x) {
  using tag_type = typename detail::make_unit_tag<Unit>::type;
  return quantity<T, tag_type>{x};
}

//------------------------------------------------------------------------------

template <typename ToUnit, typename FromUnit,
          typename Q = quantity<typename FromUnit::value_type,
                                typename detail::make_unit_tag<ToUnit>::type>,
          typename = std::enable_if_t<quantity_convertible_v<FromUnit, Q>>>
constexpr auto unit_cast(FromUnit const &u) {
  using value_type = typename FromUnit::value_type;
  using tag_type = typename detail::make_unit_tag<ToUnit>::type;

  if constexpr (FromUnit::is_affine || is_affine_unit_v<ToUnit>) {
    auto x = u.get();

    if constexpr (FromUnit::is_affine) {
      using shift_to_type = typename FromUnit::tag_type::unit::shift;
      using scale_to_type =
          typename meta::recip<typename FromUnit::tag_type::unit::scale>::type;


      // Convert down to the base.

      x += (static_cast<value_type>(shift_to_type::num) /
            static_cast<value_type>(shift_to_type::den));

      x *= static_cast<value_type>(scale_to_type::num);
      x /= static_cast<value_type>(scale_to_type::den);
    }

    if constexpr (is_affine_unit_v<ToUnit>) {
      // Convert up to the new affine type.
      using shift_from_type = typename ToUnit::shift;
      using scale_from_type = typename ToUnit::scale;


      x *= static_cast<value_type>(scale_from_type::num);
      x /= static_cast<value_type>(scale_from_type::den);

      x -= (static_cast<value_type>(shift_from_type::num) /
            static_cast<value_type>(shift_from_type::den));
    }

    return quantity<value_type, tag_type>{x};
  } else {
    // Both rational and convertible.
    return u;
  }
}

} // namespace units
//==============================================================================

struct si {
  using second = units::base_unit<0>;
  using metre = units::base_unit<1>;
  using kelvin = units::base_unit<2>;

  using kilometre = units::scaled_unit<metre, std::ratio<1000, 1>>;

  using degrees_celcius =
      units::interval_unit<kelvin, std::ratio<1, 1>, std::ratio<27315, 100>>;

  using degrees_far =
      units::interval_unit<kelvin, std::ratio<9, 5>, std::ratio<45967, 100>>;

  using metre_per_second = units::product_unit<metre, units::p<second, -1>>;
};

int main() {
  auto x = units::quantity_of<si::kelvin>(0.0);

  auto z = units::unit_cast<si::degrees_celcius>(x);

  std::cout << z.get() << std::endl;

  //  units::meta::print_type<decltype(z)>();
}

//==============================================================================
