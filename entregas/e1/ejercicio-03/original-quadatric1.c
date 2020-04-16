#include <stdio.h>
#include <math.h>

#define A 1.0
#define B -4.0000000
#define C 3.9999999

void dbl_solve(double a, double b, double c);
void flt_solve(float a, float b, float c);

int main(void)
{
    float fa = A;
    float fb = B;
    float fc = C;
    double da = A;
    double db = B;
    double dc = C;
    flt_solve(fa, fb, fc);
    dbl_solve(da, db, dc);
    return 0;
}

void dbl_solve(double a, double b, double c)
{
    double d = pow(b,2) - 4.0*a*c;      // d  = 4^2 - 4*1*3.99.. = 0, 000 000 399 999 999
    double sd = sqrt(d);                // sd = 0.00063245553
    double r1 = (-b + sd) / (2.0*a);    // r1 = (4 + sd) / 2 = 2.00031622776
    double r2 = (-b - sd) / (2.0*a);    // r2 = (4 - sd) / 2 = 1.99968377224
    printf("Soluciones Double: %.5f\t%.5f\n", r1, r2);
}

void flt_solve(float a, float b, float c)
{
    float d = pow(b,2) - 4.0*a*c;       // d  = 4^2 - 4*1*4 = 0
    float sd = sqrt(d);                 // sd = 0
    float r1 = (-b + sd) / (2.0*a);     // r1 = ((4 + 0) / 2) = 2
    float r2 = (-b - sd) / (2.0*a);     // r2 = ((4 - 0) / 2) = 2
    printf("Soluciones Float: %.5f\t%.5f\n", r1, r2);
}   
