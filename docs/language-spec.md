# Language Specification

## Go++

A component written OberonScript

```
package dp

import game
import math

type PlayerComponent struct {
	Component
	Velocity math.Vec3
}

func (p *PlayerComponent) Update() {
	pos := p.Component.GetPosition()
	p.Component.SetPosition(pos + p.Velocity * game.DeltaTime)
}
```

Invoke Using C++

```cpp

// Create a new VM and load binary data
vm_process* vm = vm_open();
// ...
// Create a new thread used to execute bytecode with
vm_thread* thread = arThread_new(vm);

// Search for the PlayerComponent and it's update method
vm_package* package = vm_get_package(vm, "dp");
vm_package_type* type = vm_package_get_type(package, "PlayerComponent");
vm_type_function* func = vm_type_get_function(type, "Update");

vm_push_this(this);
vm_thread_func_invoke(func);
```