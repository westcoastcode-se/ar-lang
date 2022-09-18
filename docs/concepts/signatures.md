# [Concepts](../concepts.md) > Signatures

Signatures have the following characteristics:

1. All symbols have a signature
1. Signatures must be application unique
1. Two different types of symbols are now allowed to have the same signature

## Package

Given that we have two packages: `WestCoastCode` and `Engine`. The `Engine` package have `WestCoastCode` as parent, then:

1. The signature is: `WestCoastCode.Engine`
1. The local signature is `Engine`

## Function

Given a package `Calculator` that have a function `Add` that takes two `int32` arguments and return an `int32` argument, then:

1. The signature is `Calculator#Add(int32,int32)(int32)`
1. The local signature is `Add(int32,int32)(int32)

Please note the `#` character, which separates the package signature and the function's local signature
