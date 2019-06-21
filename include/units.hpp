#ifndef BST_UNITS_HPP_
#define BST_UNITS_HPP_

//==============================================================================

#include "bits/detail.hpp"
#include "bits/units_fwd.hpp"
#include "bits/meta.hpp"

#include <ratio>
#include <type_traits>

//==============================================================================
namespace units {

//------------------------------------------------------------------------------

template <typename T, typename Scale, typename UnitList> class basic_quantity {
  T val;

public:
  using value_type = T;
  using scale = Scale;
  using base_units = UnitList;

  template <typename Other>
  static constexpr auto convertible_with =
      std::is_same_v<base_units, typename Other::base_units>;

  explicit constexpr basic_quantity(value_type const &v) : val{v} {}

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
  using scale1 = typename Value1::scale;
  using scale2 = typename Value2::scale;

  if constexpr (std::ratio_less_v<scale1, scale2>)
    return Value1{v1.get() + static_cast<Value1>(v2).get()};
  else
    return Value2{static_cast<Value2>(v1).get() + v2.get()};
}

//------------------------------------------------------------------------------

template <
    typename Value1, typename Value2,
    typename = std::enable_if_t<Value1::template convertible_with<Value2>>>
constexpr auto operator-(Value1 const &v1, Value2 const &v2) {
  using scale1 = typename Value1::scale;
  using scale2 = typename Value2::scale;

  if constexpr (std::ratio_less_v<scale1, scale2>)
    return Value1{v1.get() - static_cast<Value1>(v2).get()};
  else
    return Value2{static_cast<Value2>(v1).get() - v2.get()};
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

  return basic_quantity<value_type, scale, unit_list>(
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

  return basic_quantity<value_type, scale, unit_list>(
      static_cast<value_type>(v1.get()) / static_cast<value_type>(v2.get()));
}

//------------------------------------------------------------------------------

template <typename T, typename Unit>
using quantity =
    basic_quantity<T, typename detail::get_scale<Unit>::type,
                   typename detail::get_base_unit_list<Unit>::type>;

//------------------------------------------------------------------------------

template <typename To, typename T> constexpr auto unit_cast(T const &x) {
  return static_cast<quantity<typename T::value_type, To>>(x);
}

//------------------------------------------------------------------------------

template <typename Unit, typename T> constexpr auto quantity_of(T const& x) {
  return quantity<T, Unit>{x};
}

} // namespace units
//==============================================================================

#endif
