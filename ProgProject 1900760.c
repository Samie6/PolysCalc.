#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//structures to define terms of both parts of the division, including length and coefficients

typedef struct {
    int numerator;
    int denominator;
} fraction;

typedef struct {
    int length;
    fraction* coefficients;
} polynomial;



//Function to find greatest common factor

int GCF(int x, int y) {
    int z = x % y;
    if (z > 0) {
        return GCF(y, z);
    }
    return y;
}


//Function to find least common multiple

int LCM(int x, int y) {
    if (x < y) {
        return x * y / GCF(y, x);
    }
    return x * y / GCF(x, y);
}

// Setting numerator and denominator of the division, to be used in the main operation

void setFractions(fraction* fraction, int numerator, int denominator, int reduce) {
    if (reduce) {
        int fraction_GCF = GCF(abs(numerator), denominator);
        numerator /= fraction_GCF;
        denominator /= fraction_GCF;
    }
    fraction->numerator = numerator;
    fraction->denominator = denominator;
}

//Function to deallocate or 'free' the memory so we can add the following coefficients of the other terms

void freePolys(polynomial* pn) {
    free(pn->coefficients);
}

// Setting or 'saving' of the new full polynomial to be operated on, also used with 'copyPolynomial' function

int newPoly(polynomial* pn, int length) {
    int i;
    pn->length = length;
    pn->coefficients = malloc(sizeof(fraction) * (size_t)length);
    for (i = 0; i < length; i++) {
        setFractions(pn->coefficients + i, 0, 1, 0);
    }
    return 1;
}

//The main operation consists of division, multiplication, subtraction, addition and decreasing
//all defined in the following functions, then explained in the main operation in 'dividePolynomial' function

void decreasePolynomial(polynomial* pn) {
    int i;
    for (i = pn->length - 1; i > 0 && pn->coefficients[i].numerator == 0; i--, pn->length--);
}


void printFraction(fraction* fraction) {
    printf("%d", fraction->numerator);
    if (fraction->numerator != 0 && fraction->denominator > 1 && abs(fraction->numerator) != fraction->denominator) {
        printf("/%d", fraction->denominator);
    }
}

int scanFraction(fraction* fraction) {
    if (scanf("%d", &fraction->numerator) != 1) {
        printf("Invalid fraction numerator\n");
    }
    if (scanf("/%d", &fraction->denominator) == 1) {
        printf("Invalid fraction denominator\n");
    }

    return 1;
}

//Function to input the polynomials' length from the user then
//it calls scanFraction function to input coefficients of the polynomials' numerator and denominator
int scanPolynomial(polynomial* pn) {

    int length, x;
    if (scanf("%d", &length) != 1 || length < 1) {
        printf("Invalid\n");
        return 0;
    }

    if (length == 1) {
        printf("Please enter a polynomial length greater than 1\n");
        return 0;
    }

    if (!newPoly(pn, length)) {
        return 0;
    }
    for (x = 0; x < length; x++) {
        printf("Please Enter Coefficient Of X^%d \n", x);
        if (!scanFraction(pn->coefficients + x)) {
            freePolys(pn);
            return 0;
        }
    }
    //This if loop is to make sure neither the dividend nor divisor are equal to zero
    if (length > 0 && pn->coefficients[length - 1].numerator == 0) {
        printf("Invalid\n");
        return 0;
    }
    return 1;
}




int copyPolynomial(polynomial* from, polynomial* to) {
    int i;
    if (!newPoly(to, from->length)) {
        return 0;
    }
    for (i = 0; i < from->length; i++) {
        to->coefficients[i] = from->coefficients[i];
    }
    return 1;
}


void printPolynomial(const char* name, polynomial* pn) {
    int i;
    printf("%s = ", name);
    for (i = pn->length - 1; i > 0; i--) {
        if (i < pn->length - 1 && pn->coefficients[i].numerator > 0) {
            printf("+");
        }
        if (pn->coefficients[i].numerator != 0) {
            if (abs(pn->coefficients[i].numerator) != pn->coefficients[i].denominator) {
                printFraction(pn->coefficients + i);
                printf("*");
            }
            printf("x");
            if (i > 1) {
                printf("^%d", i);
            }
        }
    }
    if (pn->length == 1 || pn->coefficients[0].numerator != 0) {
        if (pn->length > 1 && pn->coefficients[0].numerator > 0) {
            printf("+");
        }
        printFraction(pn->coefficients);
    }
    puts("");
    fflush(stdout);
}





void multiplyFractions(fraction* a, fraction* b, fraction* result) {
    setFractions(result, a->numerator * b->numerator, a->denominator * b->denominator, 1);
}

void divideFractions(fraction* dividend, fraction* divisor, fraction* result) {
    fraction divisor_inverse;
    if (divisor->numerator < 0) {
        setFractions(&divisor_inverse, -divisor->denominator, abs(divisor->numerator), 0);
    }
    else {
        setFractions(&divisor_inverse, divisor->denominator, divisor->numerator, 0);
    }
    multiplyFractions(dividend, &divisor_inverse, result);
}

void addFractions(fraction* a, fraction* b, fraction* result) {
    int denominators_LCM = LCM(a->denominator, b->denominator), numerators_addition = a->numerator * denominators_LCM / a->denominator + b->numerator * denominators_LCM / b->denominator;
    setFractions(result, numerators_addition, denominators_LCM, 1);
}


void subtractFractions(fraction* a, fraction* b, fraction* result) {
    int denominators_LCM = LCM(a->denominator, b->denominator), numerators_subtraction = a->numerator * denominators_LCM / a->denominator - b->numerator * denominators_LCM / b->denominator;
    setFractions(result, numerators_subtraction, denominators_LCM, 1);
}


int dividePolynomial(polynomial* dividend, polynomial* divisor, polynomial* quotient, polynomial* remainder) {
    if (divisor->length == 1 && divisor->coefficients[0].numerator == 0) {
        return 0;
    }
    if (!newPoly(quotient, dividend->length)) {
        return 0;
    }
    if (!copyPolynomial(dividend, remainder)) {
        freePolys(quotient);
        return 0;
    }

    //This loop is the main mathematical operation to divide the polynomials, a long division operation
    //It calls the divideFractions function to continue the operation
    //It also calls the separate functions of the addition, subtractions, multiplication and decreasing of the polynomials' terms
    while (divisor->length <= remainder->length) {
        int offset, i;
        fraction division, product;
        divideFractions(remainder->coefficients + remainder->length - 1, divisor->coefficients + divisor->length - 1, &division);
        offset = remainder->length - divisor->length;
        addFractions(quotient->coefficients + offset, &division, quotient->coefficients + offset);
        for (i = 0; i < divisor->length; i++) {
            multiplyFractions(divisor->coefficients + i, &division, &product);
            subtractFractions(remainder->coefficients + i + offset, &product, remainder->coefficients + i + offset);
        }
        decreasePolynomial(remainder);
    }
    decreasePolynomial(quotient);
    return 1;
}


int main(void) {
    polynomial dividend, divisor, quotient, remainder;
    printf("Please Enter Dividend Polynomial Length\n");
    if (!scanPolynomial(&dividend)) {
        return 0;
    }
    printf("Please Enter Divisor Polynomial Length\n");
    if (!scanPolynomial(&divisor)) {
        return 0;
    }
    if (!dividePolynomial(&dividend, &divisor, &quotient, &remainder)) {
        freePolys(&divisor);
        freePolys(&dividend);
        return 0;
    }

    printPolynomial("quotient", &quotient);
    printPolynomial("remainder", &remainder);
    return 1;
}
