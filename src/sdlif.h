/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#ifndef SDLIF_H
#define SDLIF_H

#include <SDL.h>
#include <SDL_events.h>
#include <rendering.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef double (* t_color_map_cb) (
  const void*  p_color_map_params,
  const double iteration,
  const double max_iterations );


typedef struct {
  double                center;
  double                shift;
  double                alpha;
  double                beta;
} t_exp_color_params;


typedef struct {
  SDL_Window*           window;
  SDL_Renderer*         renderer;
  pthread_t             gui_thread;
  t_parman_threads*     p_threads;
  int                   nr_threads;
  t_color_map_cb        p_color_map_cb;
  void*                 p_color_params;
  int                   done;
} t_gui;


typedef struct {
  double                min_x;
  double                min_y;
  double                width;
  double                height;
} t_coord;


void release_gui( t_gui* p );
t_gui* create_gui( const int nr_threads );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef SDLIF_H */
