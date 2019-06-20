#include "bits/meta.hpp"

#include <ratio>
#include <type_traits>

//==============================================================================
namespace units {

//------------------------------------------------------------------------------
// Ratio units.

template <int Tag> struct base_unit; // e.g. Kelvin

template <typename BaseUnit, typename Scale>
struct scaled_unit; // e.g. kilometre

template <typename... Values> struct product_unit; // e.g. kilometres_per_hour

//---

template <typename Unit> struct is_ratio_unit : std::false_type {};
template <int Tag> struct is_ratio_unit<base_unit<Tag>> : std::true_type {};
template <typename BaseUnit, typename Scale>
struct is_ratio_unit<scaled_unit<BaseUnit, Scale>> : std::true_type {};
template <typename... Values>
struct is_ratio_unit<product_unit<Values...>> : std::true_type {};

template <typename Unit>
constexpr auto is_ratio_unit_v = is_ratio_unit<Unit>::value;

//------------------------------------------------------------------------------
// Affine units.

template <typename BaseUnit, int Shift>
struct interval_unit; // e.g. degrees celcius

//---

template <typename Unit> struct is_affine_unit : std::false_type {};
template <typename BaseUnit, int Shift>
struct is_affine_unit<interval_unit<BaseUnit, Shift>> : std::true_type {};

template <typename Unit>
constexpr auto is_affine_unit_v = is_affine_unit<Unit>::value;

//------------------------------------------------------------------------------
// Other

template <typename Unit, int N> struct ep;

} // namespace units
//==============================================================================


//==============================================================================
namespace units::detail {

//------------------------------------------------------------------------------
// Unit tags:
//
// Unambiguous tags to determine whether units can be combined.

template <typename Data> struct unit_tag;

// Data ::= interval_unit<...>
//       | tag_data<scale, type_list<unit_power_pair*>

template <typename Scale, typename UnitList> struct tag_data;

template <typename Unit, int Power> struct unit_power_pair {
  using unit = Unit;
  static const int power = Power;
};

template <typename... Pairs>
using parsed_product_unit = meta::type_list<Pairs...>;

//---

template <typename Tag> struct is_unit_tag_affine : std::false_type {};
template <typename Unit>
struct is_unit_tag_affine<unit_tag<Unit>> : is_affine_unit<Unit> {};

template <typename Tag>
constexpr auto is_unit_tag_affine_v = is_unit_tag_affine<Tag>::value;

//------------------------------------------------------------------------------

template <typename... Params> struct parse_product_unit; // TODO

template <> struct parse_product_unit<> { using type = parsed_product_unit<>; };

template <typename Unit, int P, typename... Params>
struct parse_product_unit<ep<Unit, P>, Params...> {
  using type = typename meta::type_list_append<
      parsed_product_unit<unit_power_pair<Unit, P>>,
      typename parse_product_unit<Params...>::type>::type;
};

template <typename Param, typename... Params>
struct parse_product_unit<Param, Params...> {
  using type = typename meta::type_list_append<
      parsed_product_unit<unit_power_pair<Param, 1>>,
      typename parse_product_unit<Params...>::type>::type;
};

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

template <typename Unit> struct make_unit_tag;

template <typename BaseUnit, int Shift>
struct make_unit_tag<interval_unit<BaseUnit, Shift>> {
  using type = unit_tag<interval_unit<BaseUnit, Shift>>;
};

template <int Tag> struct make_unit_tag<base_unit<Tag>> {
  using ratio = std::ratio<1, 1>;

  using base_unit_list = meta::type_list<unit_power_pair<base_unit<Tag>, 1>>;

  using type = unit_tag<tag_data<ratio, base_unit_list>>;
};

template <typename Unit, typename Scale>
struct make_unit_tag<scaled_unit<Unit, Scale>> {
  using ratio = std::ratio_multiply<Scale, typename make_unit_tag<Unit>::ratio>;

  using base_unit_list = typename make_unit_tag<Unit>::base_unit_list;

  using type = unit_tag<tag_data<ratio, base_unit_list>>;
};

template <> struct make_unit_tag<parsed_product_unit<>> {
  using ratio = std::ratio<1, 1>;

  using base_unit_list = meta::type_list<>;

  using type = unit_tag<tag_data<ratio, base_unit_list>>;
};

template <typename Pair, typename... Pairs>
struct make_unit_tag<parsed_product_unit<Pair, Pairs...>> {
  using ratio = std::ratio_multiply<
      typename meta::ratio_power<
          typename make_unit_tag<typename Pair::unit>::ratio,
          Pair::power>::type,
      typename make_unit_tag<parsed_product_unit<Pairs...>>::ratio>;

  using pair_units =
      typename make_unit_tag<typename Pair::unit>::base_unit_list;

  template <typename P> struct op {
    using type = unit_power_pair<typename P::unit, P::power * Pair::power>;
  };

  // Transform and sort Pair's units.
  using updated_units = typename meta::sort_type_list<
      comp, typename meta::type_list_map<op, pair_units>::type>::type;

  // Fetch and sort the remaining items.
  using rest_of_units = typename meta::sort_type_list<
      comp, typename make_unit_tag<
                parsed_product_pair<Pairs...>>::base_unit_list>::type;

  // Merge them together.
  using base_unit_list = typename meta::type_list_remove_if<
      is_power_zero,
      typename meta::type_list_merge_with<comp, merge_add, updated_units,
                                          rest_of_units>::type>::type;

  using type = unit_tag<tag_data<ratio, base_unit_list>>;
};

template <typename... Values> struct make_unit_tag<product_unit<Values...>> {
  using type = typename make_unit_tag<
      typename parse_product_unit<Values...>::type>::type;
};

template <typename Unit>
using make_unit_tag_t = typename make_unit_tag<Unit>::type;

//---

} // namespace units::detail
//==============================================================================
