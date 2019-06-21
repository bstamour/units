#ifndef BST_UNITS_BITS_FWD_
#define BST_UNITS_BITS_FWD_

//==============================================================================
namespace units {

namespace detail {
template <typename Unit> struct get_scale;
template <typename Unit> struct get_base_unit_list;
}

template <int Tag> struct base_unit { static const int tag = Tag; };
template <typename Scale, typename Unit> struct scaled_unit;
template <typename... Params> struct derived_unit;

template <typename T, typename Scale, typename UnitList> class basic_quantity;

template <typename Unit, int P> struct exp;

//---

template <typename T, typename Unit>
using quantity =
    basic_quantity<T, typename detail::get_scale<Unit>::type,
                   typename detail::get_base_unit_list<Unit>::type>;

} // namespace units
//==============================================================================

#endif
