define int gcd(int a, int b)
    c = a;
    d = b;

start:
    a1 = phi(c, 0, divisor, cal);
    b1 = phi(d, 0, remainder, cal);
    a_LE_b = a1 >= b1;
    br a_LE_b cal exchange;
	
cal:
    divisor = phi(b1, start, a1, exchange);
    larger = phi(a1, start, b1, exchange);
    remainder = larger - divisor;
    cond = remainder == 0;
    br cond ret start;
	
exchange:
    br cal;
		
ret:
    return divisor;
