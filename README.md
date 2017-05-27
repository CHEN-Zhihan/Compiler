Name: CHEN, Zhihan

UID: 3035142261

# Compilation: 
`make`
# Execution: 
`./c6 target > outfile`

`./cvm outfile`

# Language specification:

- function definition starts with a def indicator.

 
   `def add(a, b) {
       return a + b;
   }`

- geti, gets, getc are functions that return a user input

    `a = geti();`

- When a function accepts an array as a parameter, it must indicate the dimension of that array. For multi-dimensional array, it must indicate the size of each dimension starting from the second dimension. Arrays are passed by reference. Others are passed by value. 


    ```
    def f(x[][5], i, j) {
        x[i][j] = 1;
    }

    array y[3][5] = 0;
    array z[4][7] = 0;
    f(y, 1, 2);
    // f(z, 1, 2); error
    puti(y[1][2]) // prints 1;
    // f(y[1], 2, 3); error
    // f(y[1][2], 2, 3); error
    ```

- increment/decrement operation supported

    ```
    a = 1;
    puti(a++); // prints 1;
    puti(a); // prints 2;
    puti(++a); // prints 3
    puti(a); //prints 3`
    ```
    similar for decremental operation;

- Array declaration supports operation on constants only.
    The result must be positive. Constant folding adopted in array declaration.

    ```
    array x[1 + 2][5 * 3]; // equivalent to array x[3][15];
    // array x[2 / 0]; error
    // array x[-5]; error
    // i = 1; array x[i]; error

- Chained assignment supported, assignment is an expression instead of a statement;


    ```
    a = b = 2;
    puti(a); // prints 2;
    puti(b); // prints 2;
    if (a = 1) {
        puti(a); // prints 1;
    }
    ```
- boolean constant true and false supported. Negation operator supported. No guarantee when the operand is not of boolean type.

    ```
    a = false;
    if (!a) {
        puts("success") // prints success.
    }
    ```

- Scopes adopted. in an assigning operation, if the target variable is defined in the related scope, it is assigned a value. If it is not defined, it is defined within that scope.

    ```
    a = 1;
    c = 5;
    if (a == 1) {
        b = 2;
        c = a + b;
        puti(b); // prints 2
        puti(c); // prints 3
    } else {
        b = 3;
        c = a + b;
        puti(b); // prints 3
        puti(c); // prints 4
    }
    // puti(b); error
    puti(c); // prints 3
    ```
- The addresses of variables defined in one scope can be reused once the program exits that scope. It uses first fit first algorithm to find the proper address. While a variable cannot be assigned to an address space that is part of previously used and part of newly declared.
    
    Example 1:

    ```
    i = 1;
    if (i > 1) {
        b = 2;
    }

    c = 3;
    ```

    Code generated:
    ```
        push	sp
        push	2
        add
        pop	sp
        push	1
        pop	sb[0]
        push	sb[0]
        push	1
        compGT
        j0	L000
        push	2
        pop	sb[1]
    L000:
        push	3
        pop	sb[1]
        end
    ```
    Note that 3 variables are defined but 2 addresses are used.

    Example 2:

    ```
    i = 1;
    if (i > 1) {
        b = 2;
    } else {
        c = 3;
    }
    d = 4;
    e = 5;
    ```
    Code generated:

    ```
    push	sp
        push	3
        add
        pop	sp
        push	1
        pop	sb[0]
        push	sb[0]
        push	1
        compGT
        j0	L000
        push	2
        pop	sb[1]
        jmp	L001
    L000:
        push	3
        pop	sb[1]
    L001:
        push	4
        pop	sb[1]
        push	5
        pop	sb[2]
        end
    ```

    Note that 5 variables are defined but only 3 addresses allocated.

    Example 3:

    ```
    array x[5];
    if (geti() == 4) {
        array y[4];
    } else {
        array z[3];
        k = 1;
    }
    array k[5];
    ```

    Code generated:

    ```
        push	sp
        push	14
        add
        pop	sp
        geti
        push	4
        compEQ
        j0	L000
        jmp	L001
    L000:
        push	1
        pop	sb[8]
    L001:
        end
    ```

    Note that array z and scalar k share memory with array y. While array k declares its own new memory.


- simple variable, function and array checking adopted.


    - Invalid operation on array

        ```
        array x[5][6];
        if (x == 3) { // error
            ;
        }
        puti(x[2]); // error
        x++;  //error
        ```
    
    - Invalid assignment on array

        ```
        array x[5][6];
        x = 3; // error
        x[5] = 3; // error
        ```

    - return in global scope

        ```
        return 5;
        ```
    - redefinition of array

        ```
        x = 5;
        array x[3];

        array y[2];
        array y[3];
        ```

    - @ in global scope

        ```
        @x = 5;
        ```
    
    - function inside function

        ```
        def a() {
            def b() {
                ;
            }
        }
        ```
    - redefinition of function

        ```
        def a() {
            ;
        }

        def a() {
            ;
        }
        ```
        ```
        def y() {
            ;
        }
        y = 5;
        ```
    
    - duplicate parameter name

        ```
        def a(x, x) {
            ;
        }
        ```

    - Invalid argument for function

        ```
        def a(x[][]) {
            ;
        }
        array x[5][4];
        a(x[2]);
        a(5);
        a(x[3][4]);
        ```

    - invalid number of argument for function

        ```
        def a(x, y) {
            ;
        }
        a(5);
        a(6, 7, 7);
        geti(a);
        ```

    - Call to undefined function
        ```
        a(5);
        ```

    - undefined reference to variable

        ```
        def f() {
            if (geti() == 4) {
                x = 5;
                puti(x);
            }
            puti(x);
        }
        f();
        ```

        ```
        def f() {
            puti(@a);
        }
        f();
        ```

# project
    tic-tac-toe is implemented with a minimax AI. Player 
    is expected to lose or have a draw but never win.