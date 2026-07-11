

## Context

filter factories need be stored in lookup map..

## Decision
storage boundary vs defenition boundary: std::function is right when container need to hold heterogeneous callables uniformily ( eg. a factory map that later support lambdas, plugin loaded functions). Indvidual factory *functions* themselves dont need to be a std::function - a plain function decays to a a function pointer automatically and converts implicitily at the point of insertion. Type erasure only needs to happen once, at insertion, not at defenition.

## consequences
type erasure cost paid once, at map insertion, not per factory

---

also:
function-local static const map = lazy initialized on first call, thread safe since c++11 (meyer's singleton pattern). only built once.

---
