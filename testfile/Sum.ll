define int Sum(int a[], int b[], int n)
    c = 0;
start:
    i = phi(0, 0, i_inc, calc);
    sum = phi(c, 0, temp, calc);
    cond = i >= n;
    br cond ret calc;

calc:
    ai = load(a, i);
    temp = sum + ai;
    store(b, i, temp);
    i_inc = i + 1;
    br start;

ret:
    num = n - 1;
    res = load(b, num);
    return res;
