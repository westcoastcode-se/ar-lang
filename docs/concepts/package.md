# [Concepts](../concepts.md) > Package

## Circular References

It's not allowed to have circular references between two packages:

```
package package1

import package2

// ....
```

```
package package2

import package1

// ....
```

The example above will result in an error since both `package1` and `package2` tries to import each other
