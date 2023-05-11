define int dotprod(int a[], int b[], int n)
    c = 0;
    br start;

start:
    i = phi(0, 0, i_inc, calc);
    cl = phi(c, 0, cr, calc);
    cond = i >= n;
    br cond ret calc;

calc:
    ai = load(a, i);
    bi = load(b, i);
    ci = ai * bi;
    cr = cl + ci;
    i_inc = i + 1;
    br start;

ret:
    return cl;
