/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#ifndef RENDERING_H
#define RENDERING_H

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int                   res_x;;
  int                   res_y;;
  double                init_x;
  double                init_y;
  double                step_x;
  double                step_y;
  int                   iterations;
  int*                  grid;
} t_parman_data;


typedef struct {
  t_parman_data*        p_data;
  int                   start_idx;
  int                   elements;
  int                   stop;
  int                   done;
} t_parman_thread_state;


typedef struct {
  pthread_t             renderer;
  t_parman_thread_state state;
} t_parman_thread;


typedef struct {
  t_parman_thread*      thread;
  int                   nr_threads;
} t_parman_threads;


void release_parman_data( t_parman_data* p );
t_parman_data* create_parman_data( const int res_x,
                                 const int res_y,
                                 const double min_x,
                                 const double min_y,
                                 const double width,
                                 const double height,
                                 const int iterations );
void print_mandel( const t_parman_data* p );
void release_rendering( t_parman_threads* p );
t_parman_threads* start_rendering( t_parman_data* p_data, const int nr_threads );

t_parman_data* get_image_data( t_parman_threads* p_parman_threads );
void release_image( t_parman_threads* p_parman_threads );
t_parman_threads* render_image( const int res_x,
                                const int res_y,
                                const double min_x,
                                const double min_y,
                                const double width,
                                const double height,
                                const int iterations,
                                const int nr_threads );

int has_rendering_completed( t_parman_threads* p);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef RENDERING_H */
