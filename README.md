## Abstract

In this task the requirements was to create a c++ program that calculate and print the truth table for a given logic expression that uses `or`, `and`, and `not` logic operators as well as print all variable values making the expression satisfiable. the program was also required to be able to evaluate whether an expression and a hypothetical simplified expression are equivalent or not. we were asked to make the program as generic as possible.

Fulfilling the requirements, i managed to create the c++ `LogicExpression` class. Taking a string representing a logical expression using any number or variation of alphabetical characters (up to 52 variables using lower and upper case letters) and bitwise operators `&`,`|`, and `~`, the class evaluates the truth table of the given expression as well as manages to compare whether it is logically equivalent to any other given expression or not, regardless of the premise as to whether the other expression is a hypothetical simplification of it or not.

## Structure

the main properties of the class consists of the following:

* `variables`: an array storing all variables of the expression
* `permutations`: a 2D array storing all possible binary permutations based on the number of the variables generated using a recursive function
* `outputs`: an array storing the output of the expression where the *i*-th output corresponds to the *i*-th permutation and under the current order of variables in the `variables` array
* `var_map`: a map that stores which column which variable belongs in the truth table and `permutations` array.

![structure](https://github.com/user-attachments/assets/9adeff78-1e08-4606-94ca-b72f0879851f)

## Algorithms


### string evaluation
the string expression is first converted to and stored as a [postfix notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation) for postfix evaluation, then for each binary permutation, each variable is substituted with its corresponding value (described in `var_map`) in the current permutation.

### expressions equivalence
before getting to the algorithm and rationale of how the comparison works, i want to highlight a significant observation about truth tables in relation with redundant variables. that is whenever a variable is redundant, i.e its value has no effect over the expression output,  and this (or these) variables when they are all assigned to the most significant bits in an ascendingly ordered truth table permutations, a pattern in the output get periodically repeated each `2^n` rows, where `n` denotes the number of non-redundant variables.

quick example:

> LogicExpression("(c&~b|~c&b)&(a|~a)").printTruthTable();

```
a | b | c | Expr 
----------------
0 | 0 | 0 | 0
0 | 0 | 1 | 1
0 | 1 | 0 | 1
0 | 1 | 1 | 0
1 | 0 | 0 | 0
1 | 0 | 1 | 1
1 | 1 | 0 | 1
1 | 1 | 1 | 0

```

using this fact, we can know whether two expressions are equivalent or not:
```
if both expressions have no shared variables:
  if both of them being equally either tautology or unsatisfiable:
    then they are equivalent
  else:
    they are not equivalent

for each expression:
  if the expression has more variables than the shared variables between both expressions
  and any of the unshared variables is not redundant:
    then they are not equivalent

finally, if they share variables and all the unshared variables are redundant:
  make the shared variables occupy the least significant bits in the truth table
  for every output of all rows from 1 to 2^(number of shared variables):
    if output of expression A != output of expression B:
      they are not equivalent

if they are not shown unequivalent so far:
  then they are equivalent

```

Program worst runtime case: O(m2^n), where `m` denotes the string expression length, and `n` denotes the number of variables.
