units
=====

This library provides the building blocks for constructing custom unit systems. The resulting systems then provide
type-safe dimensional analysis for runtime quantities. For example, a unit of length cannot be added to a unit
of time. However they can be safely divided, resulting in a unit of length over time (a velocity).

So far the basic arithmetic operators (addition, subtraction, multiplication, division) are provided.

Basic Types
-----------

### base_unit\<int tag\>

A base unit for the system being defined. Tags must be unique within
the system.

### scaled_unit\<Ratio scale, Unit unit\>

A scaled version of an underlying unit. e.g. kilograms.

### derived_unit<Param params\...>

A derived unit, defined as the product of a list of units, and their
respective powers. e.g. metre per second.

### quantity<typename T, Unit unit>

A magnitude of type T, paired with it's unit. e.g. 4.5 seconds.

Basic Functions
---------------

### quantity_of\<Unit unit\>(value)

Create an object of type quantity\<decltype(value), Unit\>. The representation type T is automatically computed from the
type of the input. So e.g. quaitity_of\<kilometre\>(4.5) will give you double precision, while quantity_of\<metre\>(0.5f) will give you single precision.

### unit_cast\<Unit To\>(Quantity from)

Cast the quantity to a quantity of a new unit type. Fails to compile if the unit types are incompatible with each other (e.g. you cannot cast a quantity of type to a quantity of length.)

Example System
--------------

```C++
struct si {
	using second = base_unit<0>;
	using metre = base_unit<1>;

	// More base units...

	using kilometre = scaled_unit<std::ratio<1000, 1>, metre>;

	// More scaled units...

	using metre_per_second = derived_unit<metre, exp<second, -1>>;

	// More derived units...
};
```

Usage Example
-------------

```C++
void f()
{
	auto dist = quantity_of<si::metre>(4.5);
	auto time = quantity_of<si::second>(2.0);

	// Compute a value of metres per second.
	auto velocity = dist / time;

	// Scale a quantity.
	auto in_km = unit_cast<si::kilometre>(dist);

	// BAD! Compiler errir!
	auto nonsense = in_km + time;
}
```

More Examples
-------------

1. SI units: https://github.com/bstamour/units/blob/master/examples/si.cpp
2. CGS: https://github.com/bstamour/units/blob/master/examples/cgs.cpp

Limitations
-----------

So far there is no way to represent an inter-system conversion. But down the road, such conversions, e.g. metres to imperial feet, will be able to be represented.

Also, there is no support for affine (or pure interval) units, such as Degrees Celcius. Use Kelvin.
