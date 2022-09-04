# Examples

Source Code examples to give a feeling on how the language works

## Hello World

```
package Test;

import System;

func Main() {
	Printf("Hello World");
}
```

## Function

```
package Test;

import System;

func ReturnMultiples() (int32, int32) {
	return 10, 20;
}

func Main() {
	val1, val2 := ReturnMultiple();
	Printf("val1 %d, val2 %d", val1, val2);
}
```

## Classes

```
package Test;

import System;

class Animal {
	Name string;
}

func [a *Animal] Pet() {
	Println("Petting %s", a.Name);
}
```

## Class Inheritence

You can inherit methods that's marked as virtual or abstract

```
package Test

import System;

class Animal {}

abstract func [a *Animal] Pet();

virtual func [a *Animal] Name() string {
	return "Unknown";
}

class Dog : Animal {}

func [d *Dog] Pet() {
	Println("Petting Dog with name %s", d.Name());
}

func [d *Dog] Name() string {
	return "Fido";
}

func Main() {
	dog := new Dog();
	dog.Pet();
	var animal Animal = dog;
	animal.Pet();
}
```

## Templates

```
class Array<T, Count int32> {
	items [Count]T;
}

func [a *Array<T, Count>] Get(index int32) &T {
	return a.items[index];
}

```

## Constants

Constant values and functions are evaluated during compile time. Constants are only allowed to refer to other constant
values and functions

```
const Value = 10

const ConstFunc(input int32) int32 {
	return input * 10;
}

const ConstFuncWithLoop(init int32, count int32) int32 {
	result := init;
	for i := 0; i < count; ++i {
		result *= result;
	}
	return result;
}
```
