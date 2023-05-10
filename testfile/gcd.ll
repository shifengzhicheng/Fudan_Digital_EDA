define int gcd(int a, int b)
    c = a;
    d = b;

start:
    a1 = phi(c, 0, divisor, cal);
    b1 = phi(d, 0, remainder, cal);
    a_LE_b = a1 >= b1;
    br a_LE_b cal exchange;
	
cal:
    divisor = b1;
    remainder = a1 - b1;
    cond = remainder == 0;
    br cond ret start;
	
exchange:
    temp = a1;
    a1 = b1;
    b1 = temp;
    br cal;
		
ret:
    return divisor;