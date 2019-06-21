//==============================================================================

#include <units.hpp>

#include <iostream>
#include <type_traits>

//------------------------------------------------------------------------------

namespace cgs_system {

using namespace units;

//------------------------------------------------------------------------------

struct cgs {

  // Base units.

  using centimetre = base_unit<0>;
  using gram = base_unit<1>;
  using second = base_unit<2>;

  // Derived units.

  using centimetre_per_second = derived_unit<centimetre, exp<second, -1>>;
  using gal = derived_unit<centimetre, exp<second, -2>>;
  using dyne = derived_unit<gram, centimetre, exp<second, -2>>;
  using erg = derived_unit<gram, exp<centimetre, 2>, exp<second, -2>>;
  using erg_per_second = derived_unit<erg, exp<second, -1>>;
  using barye = derived_unit<gram, exp<centimetre, -1>, exp<second, -2>>;
  using poise = derived_unit<gram, exp<centimetre, -1>, exp<second, -1>>;
  using stokes = derived_unit<exp<centimetre, 2>, exp<second, -1>>;
  using kayser = derived_unit<exp<centimetre, -1>>;
};

} // namespace cgs_system
//------------------------------------------------------------------------------

template <typename T> void print_type(T &) {
  static_assert(std::is_same_v<T, int>);
}

int main() {
  using namespace cgs_system;

  auto x = quantity_of<cgs::gram>> (4.0);
  auto y = quantity_of<cgs::second>(10.0);

  auto z = x / y;

  std::cout << z.get() << std::endl;

  //  print_type(z);
}

//==============================================================================
