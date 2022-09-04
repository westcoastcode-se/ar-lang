# [Concepts](../concepts.md) > Constant

Constants are evaluated during compile time instead of runtime.

## Variables

Declare a constant variable can look like these:

```
const MyConst1 = 12345;
const MyConst2 int8 = 10;
const MyConst3 = "Hello World";
const MyConst4 = MyConst2 + MyConst1;
```

## Functions

You can also declare constant functions

```
import System;

const Fib(n int32) int32 {
	if n <= 0 {
		return 0;
	} 
	else if n == 1 {
		return 1;
	}
	t1, t2 := 0, 1;
	next_term := t1 + t2;
	for i := 3; i < n; ++i {
		t1 = t2;
		t2 = next_term;
		next_term = t1 + t2;
	}
	return next_term;
}

func Main() {
	Println("Fib: %d", Fib(5));
}
```

In the example above, the compiler will replace `Fib(5)` with `5`. 

You can also call a constant function with a non-constant variable, but in those cases the 
function is called in runtime instead of compile time as if it's a normal function.

## Generics

You can also define a constant function as a generic constant function:

```
const Add<T1, T2>(lhs T1, rhs T2) -> lhs + rhs;
```