define int sum(int a[], int n)
    c = 0;
start:
    i = phi(0, 0, i_inc, calc);
    sum = phi(c, 0, temp, calc);
    cond = i >= n;
    br cond ret calc;

calc:
    ai = load(a, i);
    temp = i - 1;
    sum_c = load(b, temp);
    sum_c_a = sum_c + ai;
	store(b, i, sum_c_a);
    i_inc = i + 1;
    br start;

ret:
    return sum;