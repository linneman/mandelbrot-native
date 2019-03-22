/* cubic spline interpolation

   from: https://gist.github.com/svdamani/1015c5c4b673c3297309
         Numerical Analysis 9th ed - Burden, Faires (Ch. 3 Natural Cubic Spline, Pg. 149)
*/

#include <stdio.h>
#include <math.h>

int cubic_split_coef( const int ni, const double* x, const double* a, double* b, double* c, double* d )
{
    int n, i, j;

    /** Step 0 */
    n = ni-1;
    double h[n], A[n], l[n + 1];
    double u[n + 1], z[n + 1];

    /** Step 1 */
    for (i = 0; i <= n - 1; ++i) h[i] = x[i + 1] - x[i];

    /** Step 2 */
    for (i = 1; i <= n - 1; ++i)
        A[i] = 3 * (a[i + 1] - a[i]) / h[i] - 3 * (a[i] - a[i - 1]) / h[i - 1];

    /** Step 3 */
    l[0] = 1;
    u[0] = 0;
    z[0] = 0;

    /** Step 4 */
    for (i = 1; i <= n - 1; ++i) {
        l[i] = 2 * (x[i + 1] - x[i - 1]) - h[i - 1] * u[i - 1];
        u[i] = h[i] / l[i];
        z[i] = (A[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    /** Step 5 */
    l[n] = 1;
    z[n] = 0;
    c[n] = 0;

    /** Step 6 */
    for (j = n - 1; j >= 0; --j) {
        c[j] = z[j] - u[j] * c[j + 1];
        b[j] = (a[j + 1] - a[j]) / h[j] - h[j] * (c[j + 1] + 2 * c[j]) / 3;
        d[j] = (c[j + 1] - c[j]) / (3 * h[j]);
    }

    return 0;
}


double cubic_split_fun( const double x, const int n, const double* xs, const double* a, const double* b, const double* c, const double* d )
{
  double x0 = -1;
  int i;

  for( i=n-2; i >= 0; --i ) {
    if( x >= xs[i] ) {
      x0 = xs[i];
      break;
    }
  }

  if( x0 < 0 ) {
    x0 = 0; /* not found */
  }

  return( a[i]  +  b[i] * (x-x0)  +  c[i] * pow( x-x0, 2.0 )  +  d[i] * pow(x-x0, 3.0) );
}


#if(0)

int main()
{
  const int n = 4;
  const double x[] = { 0, 1, 2, 3 };
  const double a[] = { 0, 1, 4, 9 };
  double b[n], c[n], d[n], xk;
  int i;

  cubic_split_coef( n, x, a, b, c,d );

  /** Step 7 */
  printf("coefficients:\n");
  printf("%2s %8s %8s %8s %8s\n", "i", "ai", "bi", "ci", "di");
  for (i = 0; i < n-1; ++i)
    printf("%2d %8.2f %8.2f %8.2f %8.2f\n", i, a[i], b[i], c[i], d[i]);


  printf("interpolation:\n");
  for( xk=0; xk<3.0; xk+=0.25 ) {
    printf( "%10lf -> %10lf\n", xk, cubic_split_fun( xk, n, x, a, b, c, d ) );
  }

  return 0;
}

#endif
