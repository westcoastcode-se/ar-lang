# [Concepts](../concepts.md) > Method

Methods have a signature that looks like this:

```
func [scope] MethodName(args) (returns) {
	// Body
}
```

Assuming that you have a base class called `Animal` and then a more specialized class `Dog` declared as:

```
virtual class Animal {
	Name string;
}

class Dog : Animal {
	Friendliness int32;
}
```

Then you can add a method to the `Animal` class like this:

```
func [a *Animal] GetName() string {
	return a.Name;
}
```

## Virtual Methods

You can also make virtual methods that you can override in other classes.

Example:

```
virtual func [a *Animal] Pet() {
	Println("Petting %s", a.Name);
}

func [d *Dog] Pet() {
	Println("Petting %s with Friendliness %d", d.Animal, d.Friendliness);
}
```

## Final methods

If you mark a method as `final` then that method is not allowed to be overriden anymore. This is, in most cases, 
only useful as a clean-code concept. But the compiler can, in some cases, call the function without looking it up in the
virtual method table as an optimization. 

Example:

```
final func [d *Dog] Pet() {
	// ...
}
```
