# Support IR format:

1. function define:
```c++   
define int foo(int a, int b); return value could be int/void
// 支持定义函数，返回值可以是int/void
```

2. operators:
```c++
// support arrays: 
int a[100]; define int foo(int a[], int b[], int N); 
// load: 
load a value from array. a[10] cannot be used directly. load(a, 10);
// store: 
save a value to array. store(a, 10, c) -> a[10] = c;
=: 
   assign value to a variable, follows the static single assignment rule
+: 
   c = a + b;
-:
   c = a - b; 
*:
   c = a * b;
/:
   c = a / b;
==:
   cond = a == b;
<:
   cond = a < b;
>: 
   cond = a > b;
>=:
   cond = a >= b;
<=:
   cond = a <=b; 
br:
   br label or br cond label1 label2
phi: 
   phi function, select the right value from different blocks, according to SSA rule
   phi(value1, block_label1, value2, block_label2, ...); The default block label from the definition of the function is 0.
return:
   return a value
```

3. examples
```c++
define int dotprod(int a[], int b[], int n)
    c = 0;

start:
    i = phi(0, 0, i_inc, calc);
    cond = i >= n;
    br cond ret calc

calc:
    ai = load(a, i);
    bi = load(b, i);
    cl = phi(c, 0, cr, start);
    ci = ai * bi;
    cr = cl + ci;
    i_inc = i + 1;
    br start;

ret:
    cf = phi(0, c, start, cr);
    return cf;
```
