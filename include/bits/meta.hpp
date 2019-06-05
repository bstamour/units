#ifndef BST_UNITS_BITS_META_
#define BST_UNITS_BITS_META_

#include <type_traits>
#include <ratio>

//==============================================================================
namespace units::meta {

template <typename... Ts> struct type_list;

//------------------------------------------------------------------------------

template <typename List1, typename List2> struct type_list_append;

template <typename... Items1, typename... Items2>
struct type_list_append<type_list<Items1...>, type_list<Items2...>> {
  using type = type_list<Items1..., Items2...>;
};

template <template <typename> typename Op, typename List> struct type_list_map;

template <template <typename> typename Op>
struct type_list_map<Op, type_list<>> {
  using type = type_list<>;
};

template <template <typename> typename Op, typename T, typename... Ts>
struct type_list_map<Op, type_list<T, Ts...>> {
  using type = typename type_list_append<
      type_list<typename Op<T>::type>,
      typename type_list_map<Op, type_list<Ts...>>::type>::type;
};

//------------------------------------------------------------------------------

template <template <typename, typename> typename Comp, typename T,
          typename List>
struct type_list_insert;

template <template <typename, typename> typename Comp, typename T>
struct type_list_insert<Comp, T, type_list<>> {
  using type = type_list<T>;
};

template <template <typename, typename> typename Comp, typename T, typename U,
          typename... Us>
struct type_list_insert<Comp, T, type_list<U, Us...>> {
  using type = std::conditional_t<
      Comp<T, U>::value,

      // T < U
      typename type_list_append<type_list<T>, type_list<U, Us...>>::type,

      // ! (T < U)
      typename type_list_append<
          type_list<U>,
          typename type_list_insert<Comp, T, type_list<Us...>>::type>::type>;
};

template <template <typename, typename> typename Comp, typename List>
struct sort_type_list;

template <template <typename, typename> typename Comp>
struct sort_type_list<Comp, type_list<>> {
  using type = type_list<>;
};

template <template <typename, typename> typename Comp, typename T,
          typename... Ts>
struct sort_type_list<Comp, type_list<T, Ts...>> {
  using type = typename type_list_insert<
      Comp, T, typename sort_type_list<Comp, type_list<Ts...>>::type>::type;
};

//------------------------------------------------------------------------------

template <template <typename, typename> typename Comp,
          template <typename, typename> typename Merger, typename List1,
          typename List2>
struct type_list_merge_with;

template <template <typename, typename> typename Comp,
          template <typename, typename> typename Merger>
struct type_list_merge_with<Comp, Merger, type_list<>, type_list<>> {
  using type = type_list<>;
};

template <template <typename, typename> typename Comp,
          template <typename, typename> typename Merger, typename T,
          typename... Ts>
struct type_list_merge_with<Comp, Merger, type_list<>, type_list<T, Ts...>> {
  using type = type_list<T, Ts...>;
};

template <template <typename, typename> typename Comp,
          template <typename, typename> typename Merger, typename T,
          typename... Ts>
struct type_list_merge_with<Comp, Merger, type_list<T, Ts...>, type_list<>> {
  using type = type_list<T, Ts...>;
};

template <template <typename, typename> typename Comp,
          template <typename, typename> typename Merger, typename T,
          typename... Ts, typename U, typename... Us>
struct type_list_merge_with<Comp, Merger, type_list<T, Ts...>,
                            type_list<U, Us...>> {
  using type = std::conditional_t<
      Comp<T, U>::value,

      // T < U
      typename type_list_append<
          type_list<T>,
          typename type_list_merge_with<Comp, Merger, type_list<Ts...>,
                                        type_list<U, Us...>>::type>::type,

      std::conditional_t<
          Comp<U, T>::value,

          // U < T
          typename type_list_append<
              type_list<U>,
              typename type_list_merge_with<Comp, Merger, type_list<T, Ts...>,
                                            type_list<Us...>>::type>::type,

          // T == U
          typename type_list_append<
              type_list<typename Merger<T, U>::type>,
              typename type_list_merge_with<Comp, Merger, type_list<Ts...>,
                                            type_list<Us...>>::type>::type>>;
};

//------------------------------------------------------------------------------

template <template <typename> typename Pred, typename List>
struct type_list_remove_if;

template <template <typename> typename Pred>
struct type_list_remove_if<Pred, type_list<>> {
  using type = type_list<>;
};

template <template <typename> typename Pred, typename T, typename... Ts>
struct type_list_remove_if<Pred, type_list<T, Ts...>> {
  using type = std::conditional_t<
      Pred<T>::value,

      // Remove!
      typename type_list_remove_if<Pred, type_list<Ts...>>::type,

      // Keep!
      typename type_list_append<
          type_list<T>,
          typename type_list_remove_if<Pred, type_list<Ts...>>::type>::type>;
};

//------------------------------------------------------------------------------

template <typename Ratio> struct recip {
  using type = std::ratio<Ratio::den, Ratio::num>;
};

//------------------------------------------------------------------------------

template <typename Ratio, int Power> struct ratio_power_impl {
  using type =
      std::ratio_multiply<Ratio,
                          typename ratio_power_impl<Ratio, Power - 1>::type>;
};

template <typename Ratio> struct ratio_power_impl<Ratio, 0> {
  using type = std::ratio<1, 1>;
};

template <typename Ratio, int Power, bool is_positive> struct ratio_power_safe {
  using type = typename ratio_power_impl<Ratio, Power>::type;
};

template <typename Ratio, int Power>
struct ratio_power_safe<Ratio, Power, false> {
  using type = typename ratio_power_impl<std::ratio<Ratio::den, Ratio::num>,
                                         -1 * Power>::type;
};

template <typename Ratio, int Power> struct ratio_power {
  using type = typename ratio_power_safe<Ratio, Power, (Power >= 0)>::type;
};

} // namespace units::meta
//==============================================================================

#endif
