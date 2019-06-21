#ifndef BST_UNITS_BITS_DETAIL_
#define BST_UNITS_BITS_DETAIL_

#include "meta.hpp"
#include "units_fwd.hpp"

#include <ratio>

//==============================================================================
namespace units::detail {

template <typename... Pairs>
using derived_unit_impl = meta::type_list<Pairs...>;

template <typename Unit, int P> struct unit_power_pair {
  using unit = Unit;
  static const int power = P;
};

//------------------------------------------------------------------------------

template <typename... Params> struct parse_derived_unit;

template <> struct parse_derived_unit<> { using type = derived_unit_impl<>; };

template <typename Unit, int P, typename... Params>
struct parse_derived_unit<exp<Unit, P>, Params...> {
  using type = typename meta::type_list_append<
      derived_unit_impl<unit_power_pair<Unit, P>>,
      typename parse_derived_unit<Params...>::type>::type;
};

template <typename Param, typename... Params>
struct parse_derived_unit<Param, Params...> {
  using type = typename meta::type_list_append<
      derived_unit_impl<unit_power_pair<Param, 1>>,
      typename parse_derived_unit<Params...>::type>::type;
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

template <typename Unit> struct flatten_and_scale;

template <int tag> struct flatten_and_scale<base_unit<tag>> {
  using base_unit_list = meta::type_list<unit_power_pair<base_unit<tag>, 1>>;

  using ratio = std::ratio<1, 1>;
};

template <typename Scale, typename Unit>
struct flatten_and_scale<scaled_unit<Scale, Unit>> {
  using base_unit_list = typename flatten_and_scale<Unit>::base_unit_list;

  using ratio =
      std::ratio_multiply<Scale, typename flatten_and_scale<Unit>::ratio>;
};

template <> struct flatten_and_scale<derived_unit_impl<>> {
  using base_unit_list = meta::type_list<>;

  using ratio = std::ratio<1, 1>;
};

template <typename Pair, typename... Pairs>
struct flatten_and_scale<derived_unit_impl<Pair, Pairs...>> {
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
                derived_unit_impl<Pairs...>>::base_unit_list>::type;

  // Merge them together.
  using base_unit_list = typename meta::type_list_remove_if<
      is_power_zero,
      typename meta::type_list_merge_with<comp, merge_add, updated_units,
                                          rest_of_units>::type>::type;

  using ratio = std::ratio_multiply<
      typename meta::ratio_power<
          typename flatten_and_scale<typename Pair::unit>::ratio,
          Pair::power>::type,
      typename flatten_and_scale<derived_unit_impl<Pairs...>>::ratio>;
};

template <typename... Values> struct flatten_and_scale<derived_unit<Values...>>
    : flatten_and_scale<typename parse_derived_unit<Values...>::type>
{
};

//------------------------------------------------------------------------------

template <typename Unit> struct get_base_unit_list {
  using type = typename flatten_and_scale<Unit>::base_unit_list;
};

template <typename Unit> struct get_scale {
  using type = typename flatten_and_scale<Unit>::ratio;
};


} // namespace units::detail
//==============================================================================

#endif
