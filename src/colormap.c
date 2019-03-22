/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <log.h>
#include <colormap.h>
#include <cubic_interpol.h>

void release_colormap( t_rgb* p)
{
  free( p );
}

t_rgb* create_colormap( const int nr_support_points, const double* x, const t_rgb* support_points, const int size )
{
  t_rgb* p;
  int i, ci, s;

  const int n = nr_support_points;
  double a[n];
  double b[n], c[n], d[n];
  int* p_col;
  double xk, yk;

  p = malloc( sizeof(t_rgb) * size );
  if( p == NULL ) {
    log_error("%s, %d: out of memory error!\n", __func__, __LINE__ );
    return NULL;
  }

  for( ci=0; ci < 3; ++ci ) /* color index loop */
  {
    /* copy support points for color */
    for( s=0; s<n; ++s ) {
      p_col = (int *) &support_points[s];
      p_col += ci;

      a[s] = (double)(*p_col);
    }

    /* determine coefficients of spline function */
    cubic_split_coef( n, x, a, b, c, d );

    for( i=0; i < size-1; ++ i) {
      p_col = (int *) &p[i];
      p_col += ci;

      xk = (double)i / (double)size;
      yk = cubic_split_fun( xk, n, x, a, b, c, d );
      *p_col = (int) ((yk >= 0.0) ? yk : 0.0 );
    }

    p[size-1].r = 0; p[size-1].g = 0; p[size-1].b = 0;
  }

  return p;
}

static int test()
{
  const int n = 5;
  const double x[] = { 0.0, 0.16, 0.42, 0.6425, 0.8575 };
  const double a[] = { 0, 32, 237, 255, 0 };
  double b[n], c[n], d[n], xk;
  int i;

  cubic_split_coef( n, x, a, b, c,d );

  printf("_______________ %s, %d __________________\n", __func__, __LINE__ );

  printf("coefficients:\n");
  printf("%2s %8s %8s %8s %8s\n", "i", "ai", "bi", "ci", "di");
  for (i = 0; i < n-1; ++i)
    printf("%2d %8.2f %8.2f %8.2f %8.2f\n", i, a[i], b[i], c[i], d[i]);

  printf("check support points:\n");
  for( i=0; i < 5; ++i ) {
    xk = x[i];
    printf( "%10lf -> %10lf\n", xk, cubic_split_fun( xk, n, x, a, b, c, d ) );
  }

  printf("interpolation:\n");
  for( xk=0; xk<1.0; xk+=0.1 ) {
    printf( "%10lf -> %10lf\n", xk, cubic_split_fun( xk, n, x, a, b, c, d ) );
  }

  return 0;
}

/*
 * taken from: https://stackoverflow.com/a/25816111
 */
t_rgb* create_default_colormap( const int size )
{
  const double x[] = { 0.0, 0.16, 0.42, 0.6425, 0.8575, 1.0 };
  const int nr_support_points = sizeof(x) / sizeof(double);
  const t_rgb support_points[] = {
    { 0,   7,   100 },
    { 32,  107, 203 },
    { 237, 255, 255 },
    { 255, 170, 0 },
    { 0,   2,   0 },
    { 0,   0,   0 }
  };

  return create_colormap( nr_support_points, x, support_points, size );
}
