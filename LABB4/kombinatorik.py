def combinatorial(n):
    result = 1;

    while(n!=0):
        n-=1;
        y=n
        x = result;
        while(y!=0):
            result = result + x
            y-=1;
    print(result);

combinatorial(2);
combinatorial(0);