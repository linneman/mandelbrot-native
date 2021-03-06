/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#include <pthread.h>
#include <unistd.h>
#include <sdlif.h>
#include <log.h>
#include <math.h>


static void plot_mandel( SDL_Renderer* renderer, const t_gui *p_gui )
{
  const t_parman_data* p = get_image_data( p_gui->p_threads );
  int x, y, color_index;
  int max_grid_val = 0;
  t_rgb* p_rgb;

  for( y = 0; y < p->res_y; ++y ) {
    for( x = 0; x < p->res_x; ++x ) {
      color_index = p->grid[ y * p->res_x + x];
      p_rgb = & p_gui->p_rgb[ color_index ];

      SDL_SetRenderDrawColor(renderer, p_rgb->r, p_rgb->g, p_rgb->b, SDL_ALPHA_OPAQUE);
      SDL_RenderDrawPoint( renderer, x, p->res_y - y );
    }
  }
}


static void* gui_thread( void* _p )
{
  t_gui* p = (t_gui*)_p;
  int res_x = 600;
  int res_y = 600;
  int iterations = p->iterations;
  SDL_bool done = SDL_FALSE;
  SDL_Event event;
  SDL_Rect selection = { .x=10, .y=10, .w=100, .h=100 };
  int errorcode;
  long cnt = 0L;
  int drawSelection = 0, update = 0;
  const int size_undo_stack = 100;
  t_coord undo_stack[size_undo_stack];
  int top_undo_stack = 0;
  long double upd_min_x, upd_min_y, upd_width, upd_height;

  if( SDL_CreateWindowAndRenderer( res_x, res_y, SDL_WINDOW_RESIZABLE, & p->window, & p->renderer) != 0) {
    log_error("%s,%d: could not initialize window and renderer error: %s!\n", __func__, __LINE__, SDL_GetError() );
  }

  SDL_RenderClear( p->renderer );

  while (!done) {

    if( cnt == 0L ) {
      p->p_threads = render_image( res_x, res_y,
                                   -2 /* min_x */,
                                   -1.25 /* min_y */,
                                   2.5 /* width */,
                                   2.5 /* height */,
                                   iterations, p->nr_threads );
      update  = 1;
      if( p->p_threads == NULL ) {
        log_error("%s, %d: could create renderer error!\n", __func__, __LINE__ );
        SDL_DestroyRenderer(p->renderer);
        SDL_DestroyWindow(p->window);
        SDL_Quit();
        return NULL;
      }
    }


    if( drawSelection || update ) {
      plot_mandel( p->renderer, p );
      if( drawSelection ) {
        SDL_SetRenderDrawColor( p->renderer, 0xff, 0, 0, 100 );
        SDL_SetRenderDrawBlendMode( p->renderer, SDL_BLENDMODE_BLEND );
        SDL_RenderFillRect( p->renderer, &selection );
      }
      SDL_RenderPresent( p->renderer );

      if( has_rendering_completed( p->p_threads ) ) {
        /* ensure final image update when rendering has been completed and no dragging operation is pending */
        if( ! drawSelection ) {
          SDL_RenderClear( p->renderer );
          plot_mandel( p->renderer, p );
          SDL_RenderPresent( p->renderer );
        }
        update = 0;
      }
    }


    while (SDL_PollEvent(&event)) {

      switch( event.type ) {

      case SDL_QUIT:
        done = SDL_TRUE;
        break;

      case SDL_MOUSEBUTTONDOWN:
        if( SDL_GetWindowID(p->window) == event.button.windowID ) {
          if( event.button.button == SDL_BUTTON_LEFT ) {
            selection.x = event.button.x;
            selection.y = event.button.y;
            selection.w = 1;
            selection.h = 1;
            drawSelection = 1;
          }
        }
        break;

      case SDL_MOUSEMOTION:
        if( SDL_GetWindowID(p->window) == event.motion.windowID ) {
          if( event.button.button == SDL_BUTTON_LEFT ) {
            selection.w = event.motion.x - selection.x;
            selection.h = selection.w * res_y / res_x;
          }
        }
        break;

      case SDL_MOUSEBUTTONUP:
        if( SDL_GetWindowID(p->window) == event.button.windowID ) {
          if( event.button.button == SDL_BUTTON_LEFT ) {
            t_parman_data* p_data = get_image_data( p->p_threads );
            drawSelection = 0;
            update = 1;

            upd_min_x  = p_data->init_x + selection.x * p_data->step_x;
            upd_min_y  = p_data->init_y + selection.y * p_data->step_y;
            upd_width  = selection.w * p_data->step_x;
            upd_height = selection.h * p_data->step_y;

            undo_stack[top_undo_stack].min_x  = p_data->init_x;
            undo_stack[top_undo_stack].min_y  = p_data->init_y;
            undo_stack[top_undo_stack].width  = res_x * p_data->step_x;
            undo_stack[top_undo_stack].height = res_y * p_data->step_y;

            if( top_undo_stack < size_undo_stack )
              ++top_undo_stack;

          } else if( event.button.button == SDL_BUTTON_RIGHT ) {
            if( --top_undo_stack < 0 ) {
              top_undo_stack = 0;
              break;
            }
            else {
              upd_min_x = undo_stack[top_undo_stack].min_x;
              upd_min_y = undo_stack[top_undo_stack].min_y;
              upd_width = undo_stack[top_undo_stack].width;
              // upd_height = undo_stack[top_undo_stack].height;
              upd_height = undo_stack[top_undo_stack].width * res_y / res_x;
              update = 1;
            }
          } else {
            break;
          }

          release_image( p->p_threads );
          p->p_threads = render_image( res_x, res_y, upd_min_x, upd_min_y, upd_width, upd_height,
                                       iterations, p->nr_threads );
          if( p->p_threads == NULL ) {
            log_error("%s, %d: could create renderer error!\n", __func__, __LINE__ );
            SDL_DestroyRenderer(p->renderer);
            SDL_DestroyWindow(p->window);
            SDL_Quit();
            return NULL;
          }
        }
        break;

      case SDL_WINDOWEVENT:
        if( event.window.event == SDL_WINDOWEVENT_RESIZED ) {
          t_parman_data* p_data = get_image_data( p->p_threads );
          res_x = event.window.data1;
          res_y = event.window.data2;
          if( res_y > res_x ) {
            upd_width  = p_data->res_x * p_data->step_x;
            upd_height = upd_width * res_y / res_x;
          } else {
            upd_height = p_data->res_y * p_data->step_y;
            upd_width  = upd_height * res_x / res_y;
          }
          release_image( p->p_threads );
          p->p_threads = render_image( res_x, res_y, p_data->init_x, p_data->init_y, upd_width, upd_height,
                                       iterations, p->nr_threads );
          update = 1;
          if( p->p_threads == NULL ) {
            log_error("%s, %d: could create renderer error!\n", __func__, __LINE__ );
            SDL_DestroyRenderer(p->renderer);
            SDL_DestroyWindow(p->window);
            SDL_Quit();
            return NULL;
          }
        }
        break;
      }
    }

    SDL_Delay( 100 );
    ++cnt;
  }

  release_image( p->p_threads );
  SDL_DestroyRenderer(p->renderer);
  SDL_DestroyWindow(p->window);
  SDL_Quit();

  p->done = 1;
  return NULL;
}

void release_gui( t_gui* p )
{
  release_colormap( p->p_rgb );
  free( p );
}

t_gui* create_gui( const int nr_threads, const int iterations )
{
  t_gui* p;
  int retcode;
  const int exp_color_map = 1;

  if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
    log_error("%s,%d: could not initialize sdl graphics driver error %s!\n", __func__, __LINE__,
              SDL_GetError() );
    return NULL;
  }

  p = malloc( sizeof(t_gui) );
  if( p == NULL ) {
    log_error("%s, %d: out of memory error!\n", __func__, __LINE__ );
    return NULL;
  }
  memset( p, 0, sizeof(t_gui) );
  p->nr_threads = nr_threads;
  p->iterations = iterations;

  p->p_rgb = create_default_colormap( iterations + 1 );
  if( p->p_rgb == NULL ) {
    log_error("%s, %d: out of memory error!\n", __func__, __LINE__ );
    free( p );
    return NULL;
  }

#ifdef __APPLE__
  gui_thread( p );
#else
  retcode = pthread_create( & p->gui_thread, NULL, gui_thread, p );
  if( ! retcode ) {
    retcode = pthread_detach( p->gui_thread );
  }
#endif

  if( retcode ) {
    log_error( "%s, %d: could not create gui thread error!\n", __func__, __LINE__ );
    release_colormap( p->p_rgb );
    free( p );
    return NULL;
  }

  return p;
}
