/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#ifndef COLORMAP_H
#define COLORMAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int r;
  int g;
  int b;
} t_rgb;


void release_colormap( t_rgb* p);

t_rgb* create_colormap( const int nr_support_points, const double* x, const t_rgb* support_points, const int size );

t_rgb* create_default_colormap( const int size );

#ifdef __cplusplus
}
#endif

#endif /* COLORMAP_H */
