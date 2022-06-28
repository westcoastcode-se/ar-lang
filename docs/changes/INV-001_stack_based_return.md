# Changes > INV-001_stack_based_return

When calling a function, we have to allocate memory for the return value before pushing arguments. Investigate if it's possible
to rely on the callee to push the return values instead of "setting" the value in a return value slot.

We want to change:

``` 
fn Inner() (int32) {
	ldc_i4 10
	str 0
	ret
}

fn Outer() () {
	// ...
	allocs int32
	call Inner()(int32)
	// ...
}
```

into

``` 
fn Inner() (int32) {
	ldc_i4 10
	ret
}

fn Outer() () {
	// ...
	call Inner()(int32)
	// value is now at the top of the stack here
	//...
}
```