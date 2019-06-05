//==============================================================================

#include <units.hpp>

#include <ratio>
#include <type_traits>

//------------------------------------------------------------------------------

namespace si_system {

using namespace units;

//------------------------------------------------------------------------------

struct si {

  // Base units.

  using second = base_unit<0>;
  using metre = base_unit<1>;
  using kilogram = base_unit<2>;
  using kelvin = base_unit<3>;
  using ampere = base_unit<4>;
  using mole = base_unit<5>;
  using candela = base_unit<6>;

  // Dimensionless derived units.

  using radian = derived_unit<metre, exp<metre, -1>>;
  using steradian = derived_unit<exp<metre, 2>, exp<metre, -2>>;

  // Derived units.

  using hertz = derived_unit<exp<second, -1>>;
  using newton = derived_unit<kilogram, metre, exp<second, -2>>;
  using pascal = derived_unit<newton, exp<metre, -2>>;
  using joule = derived_unit<kilogram, exp<metre, 2>, exp<second, -2>>;
  using watt = derived_unit<joule, exp<second, -1>>;
  using coulomb = derived_unit<second, ampere>;
  using volt =
      derived_unit<kilogram, exp<metre, 2>, exp<second, -3>, exp<ampere, -1>>;
  using farad = derived_unit<coulomb, exp<volt, -1>>;
  using ohm = derived_unit<volt, exp<ampere, -1>>;
  using siemens = derived_unit<exp<ohm, -1>>;
  using weber = derived_unit<volt, second>;
  using tesla = derived_unit<weber, exp<metre, -2>>;
  using henry = derived_unit<weber, exp<ampere, -1>>;
  using lumen = derived_unit<candela, steradian>;
  using lux = derived_unit<lumen, exp<metre, -2>>;
  using becquerel = derived_unit<exp<second, -1>>;
  using gray = derived_unit<joule, exp<kilogram, -1>>;
  using sievert = derived_unit<joule, exp<kilogram, -1>>;
  using katal = derived_unit<mole, exp<second, -1>>;

  // Example coherent derived units in terms of base units.

  using square_metre = derived_unit<exp<metre, 2>>;
  using cubic_metre = derived_unit<exp<metre, 3>>;
  using metre_per_second = derived_unit<metre, exp<second, -1>>;
  using metre_per_second_squared = derived_unit<metre, exp<second, -2>>;
  using reciprocal_metre = derived_unit<exp<metre, -1>>;
  using kilogram_per_cubic_metre = derived_unit<kilogram, exp<metre, -3>>;
  using kilogram_per_square_metre = derived_unit<kilogram, exp<metre, -2>>;
  using cubic_metre_per_kilogram =
      derived_unit<exp<metre, 3>, exp<kilogram, -1>>;
  using ampere_per_square_metre = derived_unit<ampere, exp<metre, -2>>;
  using ampere_per_metre = derived_unit<ampere, exp<metre, -1>>;
  using mole_per_cubic_metre = derived_unit<mole, exp<metre, -3>>;
  using candela_per_square_metre = derived_unit<candela, exp<metre, -2>>;

  // Example derived units with special names.

  using pascal_second = derived_unit<pascal, second>;
  using newton_metre = derived_unit<newton, metre>;
  using newton_per_metre = derived_unit<newton, exp<metre, -1>>;
  using radian_per_second = derived_unit<radian, exp<second, -1>>;
  using radian_per_second_squared = derived_unit<radian, exp<second, -2>>;
  using watt_per_square_metre = derived_unit<watt, exp<metre, -2>>;
  using joule_per_kelvin = derived_unit<joule, exp<kelvin, -1>>;
  using joule_per_kilogram_kelvin =
      derived_unit<joule, exp<kilogram, -1>, exp<kelvin, -1>>;
  using joule_per_kilogram = derived_unit<joule, exp<kilogram, -1>>;
  using watt_per_metre_kelvin =
      derived_unit<watt, exp<metre, -1>, exp<kelvin, -1>>;
  using joule_per_cubic_metre = derived_unit<joule, exp<metre, -3>>;
  using volt_per_metre = derived_unit<volt, exp<metre, -1>>;
  using coulomb_per_cubic_metre = derived_unit<coulomb, exp<metre, -3>>;
  using coulomb_per_square_metre = derived_unit<coulomb, exp<metre, -2>>;
  using farad_per_metre = derived_unit<farad, exp<metre, -1>>;
  using henry_per_metre = derived_unit<henry, exp<metre, -1>>;
  using joule_per_mole = derived_unit<joule, exp<mole, -1>>;
  using joule_per_mole_kelvin =
      derived_unit<joule, exp<mole, -1>, exp<kelvin, -1>>;
  using coulomb_per_kilogram = derived_unit<coulomb, exp<kilogram, -1>>;
  using gray_per_second = derived_unit<gray, exp<second, -1>>;
  using watt_per_steradian = derived_unit<watt, exp<steradian, -1>>;
  using watt_per_square_metre_steradian =
      derived_unit<watt, exp<metre, -2>, exp<steradian, -1>>;
  using katal_per_cubic_metre = derived_unit<katal, exp<metre, -3>>;

  // Handy wrappers for scaling units. e.g. mega<watt>

  template <typename Unit> using exa = scaled_unit<std::exa, Unit>;
  template <typename Unit> using peta = scaled_unit<std::peta, Unit>;
  template <typename Unit> using tera = scaled_unit<std::tera, Unit>;
  template <typename Unit> using giga = scaled_unit<std::giga, Unit>;
  template <typename Unit> using mega = scaled_unit<std::mega, Unit>;
  template <typename Unit> using kilo = scaled_unit<std::kilo, Unit>;
  template <typename Unit> using hecto = scaled_unit<std::hecto, Unit>;
  template <typename Unit> using deca = scaled_unit<std::deca, Unit>;
  template <typename Unit> using deci = scaled_unit<std::deci, Unit>;
  template <typename Unit> using centi = scaled_unit<std::centi, Unit>;
  template <typename Unit> using milli = scaled_unit<std::milli, Unit>;
  template <typename Unit> using micro = scaled_unit<std::micro, Unit>;
  template <typename Unit> using nano = scaled_unit<std::nano, Unit>;
  template <typename Unit> using pico = scaled_unit<std::pico, Unit>;
  template <typename Unit> using femto = scaled_unit<std::femto, Unit>;
  template <typename Unit> using atto = scaled_unit<std::atto, Unit>;

  // More scaled units. Some of these are technically not SI units.

  using minute = scaled_unit<std::ratio<60, 1>, second>;
  using hour = scaled_unit<std::ratio<60, 1>, minute>;
  using day = scaled_unit<std::ratio<24, 1>, hour>;

  using astronomical_unit = scaled_unit<std::ratio<149597870700, 1>, metre>;

  using hectare = scaled_unit<std::ratio<10000, 1>, square_metre>;

  using litre = scaled_unit<std::ratio<1, 1000>, cubic_metre>;

  using tonne = scaled_unit<std::ratio<1000, 1>, kilogram>;
  using metric_ton = tonne;
};

}
//------------------------------------------------------------------------------

template <typename T> void print_type() {
  static_assert(std::is_same_v<T, int>);
}

int main() {
  using namespace si_system;

  constexpr value<double, si::astronomical_unit> dist{4.0};
  print_type<decltype(dist)>();
}

//==============================================================================
