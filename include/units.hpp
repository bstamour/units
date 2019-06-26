#ifndef BST_UNITS_HPP_
#define BST_UNITS_HPP_

//==============================================================================

#include "bits/detail.hpp"
#include "bits/meta.hpp"
#include "units_fwd.hpp"

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

  template <typename U, typename S, typename UL>
  explicit constexpr operator basic_quantity<U, S, UL>() const {
    using other_type = basic_quantity<U, S, UL>;

    static_assert(convertible_with<other_type>, "Units are not convertible");

    using scale_to_base = scale;
    using scale_from_base = typename meta::recip<S>::type;
    using scale_to_other = std::ratio_multiply<scale_to_base, scale_from_base>;

    return other_type{val * scale_to_other::num / scale_to_other::den};
  }

  explicit constexpr operator value_type() const { return val; }
};

//------------------------------------------------------------------------------

template <typename T1, typename Scale1, typename UL1, typename T2,
          typename Scale2, typename UL2>
constexpr auto operator+(basic_quantity<T1, Scale1, UL1> const &v1,
                         basic_quantity<T2, Scale2, UL2> const &v2) {
  using value_1 = basic_quantity<T1, Scale1, UL1>;
  using value_2 = basic_quantity<T2, Scale2, UL2>;

  static_assert(value_1::template convertible_with<value_2>,
                "Units are not compatible for addition");

  if constexpr (std::ratio_less_v<Scale1, Scale2>)
    return value_1{v1.get() + static_cast<value_1>(v2).get()};
  else
    return value_2{static_cast<value_2>(v1).get() + v2.get()};
}

//------------------------------------------------------------------------------

template <typename T1, typename Scale1, typename UL1, typename T2,
          typename Scale2, typename UL2>
constexpr auto operator-(basic_quantity<T1, Scale1, UL1> const &v1,
                         basic_quantity<T2, Scale2, UL2> const &v2) {
  using value_1 = basic_quantity<T1, Scale1, UL1>;
  using value_2 = basic_quantity<T2, Scale2, UL2>;

  static_assert(value_1::template convertible_with<value_2>,
                "Units are not compatible for subtraction");

  if constexpr (std::ratio_less_v<Scale1, Scale2>)
    return value_1{v1.get() - static_cast<value_1>(v2).get()};
  else
    return value_2{static_cast<value_2>(v1).get() - v2.get()};
}

//------------------------------------------------------------------------------

template <typename T1, typename Scale1, typename UL1, typename T2,
          typename Scale2, typename UL2>
constexpr auto operator*(basic_quantity<T1, Scale1, UL1> const &v1,
                         basic_quantity<T2, Scale2, UL2> const &v2) {
  using unit_list = typename meta::type_list_remove_if<
      detail::is_power_zero,
      typename meta::type_list_merge_with<detail::comp, detail::merge_add, UL1,
                                          UL2>::type>::type;

  using value_type = std::common_type_t<T1, T2>;

  using scale_to_base = Scale1;
  using scale_from_base = typename meta::recip<Scale2>::type;
  using scale = std::ratio_multiply<scale_to_base, scale_from_base>;

  return basic_quantity<value_type, scale, unit_list>(
      static_cast<value_type>(v1.get()) * static_cast<value_type>(v2.get()));
}

//------------------------------------------------------------------------------

template <typename T1, typename Scale1, typename UL1, typename T2,
          typename Scale2, typename UL2>
constexpr auto operator/(basic_quantity<T1, Scale1, UL1> const &v1,
                         basic_quantity<T2, Scale2, UL2> const &v2) {
  using inverted_units =
      typename meta::type_list_map<detail::invert_power, UL2>::type;

  using unit_list = typename meta::type_list_remove_if<
      detail::is_power_zero,
      typename meta::type_list_merge_with<detail::comp, detail::merge_add, UL1,
                                          inverted_units>::type>::type;

  using value_type = std::common_type_t<T1, T2>;

  using scale_to_base = Scale1;
  using scale_from_base = typename meta::recip<Scale2>::type;
  using scale = std::ratio_multiply<scale_to_base, scale_from_base>;

  return basic_quantity<value_type, scale, unit_list>(
      static_cast<value_type>(v1.get()) / static_cast<value_type>(v2.get()));
}

//------------------------------------------------------------------------------

template <typename To, typename T> constexpr auto unit_cast(T const &x) {
  return static_cast<quantity<typename T::value_type, To>>(x);
}

//------------------------------------------------------------------------------

template <typename Unit, typename T> constexpr auto quantity_of(T const &x) {
  return quantity<T, Unit>{x};
}

} // namespace units
//==============================================================================

#endif
