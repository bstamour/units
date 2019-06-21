#ifndef BST_UNITS_BITS_FWD_
#define BST_UNITS_BITS_FWD_

//==============================================================================
namespace units {

template <int Tag> struct base_unit { static const int tag = Tag; };
template <typename Scale, typename Unit> struct scaled_unit;
template <typename... Params> struct derived_unit;

template <typename Unit, int P> struct exp;

} // namespace units
//==============================================================================

#endif
