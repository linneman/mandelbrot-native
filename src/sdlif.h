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
#include <colormap.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  SDL_Window*           window;
  SDL_Renderer*         renderer;
  pthread_t             gui_thread;
  t_parman_threads*     p_threads;
  int                   nr_threads;
  int                   iterations;
  t_rgb*                p_rgb;
  int                   done;
} t_gui;


typedef struct {
  double                min_x;
  double                min_y;
  double                width;
  double                height;
} t_coord;


void release_gui( t_gui* p );
t_gui* create_gui( const int nr_threads, const int iterations );


#ifdef __cplusplus
}
#endif

#endif /* #ifndef SDLIF_H */
