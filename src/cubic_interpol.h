/* cubic spline interpolation

   from: https://gist.github.com/svdamani/1015c5c4b673c3297309
         Numerical Analysis 9th ed - Burden, Faires (Ch. 3 Natural Cubic Spline, Pg. 149)
*/

#ifndef CUBIC_SPLINE_H
#define CUBIC_SPLINE_H

#ifdef __cplusplus
extern "C" {
#endif

int cubic_split_coef( const int ni, const double* x, const double* a, double* b, double* c, double* d );
double cubic_split_fun( const double x, const int n, const double* xs, const double* a, const double* b, const double* c, const double* d );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef CUBIC_SPLINE_H */
