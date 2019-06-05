#ifndef BST_UNITS_HPP_
#define BST_UNITS_HPP_

//==============================================================================

#include "bits/detail.hpp"
#include "bits/fwd.hpp"
#include "bits/meta.hpp"

#include <ratio>
#include <type_traits>

//==============================================================================
namespace units {

template <typename... Params>
using derived_unit = typename detail::parse_derived_unit<Params...>::type;

//------------------------------------------------------------------------------

template <typename T, typename Scale, typename UnitList> class value_impl {
  T val;

public:
  using value_type = T;
  using scale = Scale;
  using base_units = UnitList;

  template <typename Other>
  static constexpr auto convertible_with =
      std::is_same_v<base_units, typename Other::base_units>;

  explicit constexpr value_impl(value_type const &v) : val{v} {}

  constexpr auto get() const { return val; }

  template <typename Other,
            typename = std::enable_if_t<convertible_with<Other>>>
  explicit constexpr operator Other() const {
    using scale_to_base = scale;
    using scale_from_base = typename meta::recip<typename Other::scale>::type;
    using scale_to_other = std::ratio_multiply<scale_to_base, scale_from_base>;

    return Other{val * scale_to_other::num / scale_to_other::den};
  }

  explicit constexpr operator value_type() const { return val; }
};

//------------------------------------------------------------------------------

template <
    typename Value1, typename Value2,
    typename = std::enable_if_t<Value1::template convertible_with<Value2>>>
constexpr auto operator+(Value1 const &v1, Value2 const &v2) {
  return Value1{v1.get() + static_cast<Value1>(v2).get()};
}

//------------------------------------------------------------------------------

template <
    typename Value1, typename Value2,
    typename = std::enable_if_t<Value1::template convertible_with<Value2>>>
constexpr auto operator-(Value1 const &v1, Value2 const &v2) {
  return Value1{v1.get() - static_cast<Value1>(v2).get()};
}

//------------------------------------------------------------------------------

template <typename Value1, typename Value2>
constexpr auto operator*(Value1 const &v1, Value2 const &v2) {
  using unit_list = typename meta::type_list_remove_if<
      detail::is_power_zero,
      typename meta::type_list_merge_with<
          detail::comp, detail::merge_add, typename Value1::base_units,
          typename Value2::base_units>::type>::type;

  using value_type = std::common_type_t<typename Value1::value_type,
                                        typename Value2::value_type>;

  using scale_to_base = typename Value1::scale;
  using scale_from_base = typename meta::recip<typename Value2::scale>::type;
  using scale = std::ratio_multiply<scale_to_base, scale_from_base>;

  return value_impl<value_type, scale, unit_list>(
      static_cast<value_type>(v1.get()) * static_cast<value_type>(v2.get()));
}

//------------------------------------------------------------------------------

template <typename Value1, typename Value2>
constexpr auto operator/(Value1 const &v1, Value2 const &v2) {
  using inverted_units =
      typename meta::type_list_map<detail::invert_power,
                                   typename Value2::base_units>::type;

  using unit_list = typename meta::type_list_remove_if<
      detail::is_power_zero,
      typename meta::type_list_merge_with<detail::comp, detail::merge_add,
                                          typename Value1::base_units,
                                          inverted_units>::type>::type;

  using value_type = std::common_type_t<typename Value1::value_type,
                                        typename Value2::value_type>;

  using scale_to_base = typename Value1::scale;
  using scale_from_base = typename meta::recip<typename Value2::scale>::type;
  using scale = std::ratio_multiply<scale_to_base, scale_from_base>;

  return value_impl<value_type, scale, unit_list>(
      static_cast<value_type>(v1.get()) / static_cast<value_type>(v2.get()));
}

//------------------------------------------------------------------------------

template <typename T, typename Unit>
using value = value_impl<T, typename detail::get_scale<Unit>::type,
                         typename detail::get_base_unit_list<Unit>::type>;

} // namespace units
//==============================================================================

#endif
