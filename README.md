units
=====

Using C++ templates to track dimensional metadata

Basic Types
-----------

### base_unit<int tag>

A base unit for the system being defined. Tags must be unique within
the system.

### scaled_unit<Ratio scale, Unit unit>

A scaled version of an underlying unit. e.g. kilograms.

### derived_unit<Param params\...>

A derived unit, defined as the product of a list of units, and their
respective powers. e.g. metre per second.

### quantity<typename T, Unit unit>

A magnitude of type T, paired with it's unit. e.g. 4.5 seconds.

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
