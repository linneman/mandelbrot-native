/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <rendering.h>
#include <log.h>

#define SQUARE(x)     ( (x)*(x) )

void release_parman_data( t_parman_data* p )
{
  if( p ) {
    if( p->grid ) {
      free( p->grid );
    }
    free( p );
  }
}

t_parman_data* create_parman_data( const int res_x,
                                   const int res_y,
                                   const double min_x,
                                   const double min_y,
                                   const double width,
                                   const double height,
                                   const int iterations )
{
  t_parman_data* p = malloc( sizeof( t_parman_data ) );
  const long grid_elements = (long) res_x * (long) res_y;

  if( p == NULL ) {
    log_error(  "%s,%d: out of memory error!\n", __func__, __LINE__ );
    return NULL;
  }
  p->grid = malloc( sizeof(int) * grid_elements );

  if( p->grid == NULL ) {
    log_error(  "%s,%d: out of memory error!\n", __func__, __LINE__ );
    release_parman_data( p );
    return NULL;
  }

  memset( p->grid, 0, sizeof(int) * grid_elements );
  p->res_x = res_x;
  p->res_y = res_y;

  p->init_x = min_x;
  p->init_y = min_y;
  p->step_x = width / (double)res_x;
  p->step_y = height / (double)res_y;
  p->iterations = iterations;

  return p;
}

static void* render_mandel( void* pa )
{
  t_parman_thread_state* p_thread_state = (t_parman_thread_state *)pa;
  t_parman_data* p_data = p_thread_state->p_data;
  const int start_idx = p_thread_state->start_idx;
  const int elements = p_thread_state->elements;

  int idx_x = start_idx % p_data->res_x;
  int idx_y = start_idx / p_data->res_x;
  int i, iter;
  double z, zi ,c, ci, temp;
  const int max_iter = p_data->iterations;
  long invoc_cnt = 0L;
  static int thread_nr = 0;

  ++thread_nr;

  for( i=start_idx; i < start_idx + elements; ++i )
  {
    z  = 0; zi = 0;
    c  = p_data->init_x  +  idx_x * p_data->step_x;
    ci = p_data->init_y  +  (p_data->res_y - idx_y) * p_data->step_y;
    iter = 0;

    do {
      temp = z * z - zi * zi + c;
      zi = 2 * z * zi + ci;
      z = temp;

      if( p_thread_state->stop )
        goto stop;
    } while( (SQUARE(z) + SQUARE(zi)) < 4.0 && ++iter < max_iter );

    p_data->grid[i] = iter;

    if( ++idx_x >= p_data->res_x ) {
      idx_x = 0;
      if( ++idx_y >= p_data->res_y ) {
        log_error("%s,%d: out of bound exceptions error!\n", __func__, __LINE__ );
        return NULL;
      }
    }
  }

stop:
  p_thread_state->done = 1;
  return p_data;
}


void print_mandel( const t_parman_data* p )
{
  int x, y;
  const char pixels[] = { ' ', '.', 'o', 'x', '*' };
  const int max_pixels = sizeof( pixels ) / sizeof( char) - 1;
  int pixel_idx;

  /* clear screen (requires ansi terminal) */
  printf("\e[1;1H\e[2J");

  for( y = 0; y < p->res_y; ++y ) {
    for( x = 0; x < p->res_x; ++x ) {
      pixel_idx = p->grid[ y * p->res_x + x] * max_pixels / p->iterations;
      putchar( pixels[ pixel_idx ] );
    }
    putchar('\n');
  }
}

void release_rendering( t_parman_threads* p )
{
  int i, all_done;

  for( i=0; i < p->nr_threads; ++i ) {
    if( ! p->thread[i].state.done ) {
      p->thread[i].state.stop = 1;
    }
  }

  do {
    for(i=0, all_done = 1; i < p->nr_threads; ++i ) {
      all_done = all_done && p->thread[i].state.done;
    }
    usleep( 1000 );
  } while( !all_done );

  free( p->thread );
  free( p );
}

t_parman_threads* start_rendering( t_parman_data* p_data, const int nr_threads )
{
  t_parman_threads*      p;
  t_parman_thread_state* p_thread_state;
  const int tot_elements = p_data->res_x * p_data->res_y;
  int retcode, i, j;

  p = malloc( sizeof( t_parman_threads ) );
  if( p == NULL ) {
    log_error("%s,%d: out of memory error!\n", __func__, __LINE__ );
    return NULL;
  }
  memset( p, 0, sizeof(t_parman_threads) );

  p->thread = malloc( nr_threads * sizeof( t_parman_thread ) );
  if( p->thread == NULL ) {
    log_error("%s,%d: out of memory error!\n", __func__, __LINE__ );
    free( p );
    return NULL;
  }
  memset( p->thread, 0, nr_threads * sizeof(t_parman_thread) );
  p->nr_threads = nr_threads;

  for( i=0; i<nr_threads; ++i ) {
    p_thread_state = & p->thread[i].state;
    p_thread_state->p_data = p_data;
    p_thread_state->elements = tot_elements / nr_threads;
    p_thread_state->start_idx = i * p_thread_state->elements;
    if( i == nr_threads-1 )
      p_thread_state->elements = tot_elements - i * p_thread_state->elements - 1;

    retcode = pthread_create( & p->thread[i].renderer, NULL, render_mandel, p_thread_state );
    if( ! retcode ) {
      retcode = pthread_detach( p->thread[i].renderer );
    }

    if( retcode ) {
      log_error("%s,%d: could not create thread no %d error %d!\n",
              __func__, __LINE__, i, retcode );
      for( j=i-1; j>=0; ++j ) {
        p->thread[j].state.stop = 1;
      }
      free( p->thread );
      free( p );
      return NULL;
    }
  }

  return p;
}

/* convenience function which handle both, threads and data */

t_parman_data* get_image_data( t_parman_threads* p_parman_threads )
{
  return p_parman_threads->thread[0].state.p_data;
}

void release_image( t_parman_threads* p_parman_threads )
{
  t_parman_data* p_data = p_parman_threads->thread[0].state.p_data;
  release_rendering( p_parman_threads );
  release_parman_data( p_data );
}

t_parman_threads* render_image( const int res_x,
                                const int res_y,
                                const double min_x,
                                const double min_y,
                                const double width,
                                const double height,
                                const int iterations,
                                const int nr_threads )
{
  t_parman_threads* p_parman_threads;

  t_parman_data* p_data = create_parman_data( res_x, res_y, min_x, min_y, width, height, iterations );
  if( p_data == NULL ) {
    log_error("%s, %d: could not initialize parameter data error!\n", __func__, __LINE__ );
    return NULL;
  }

  p_parman_threads = start_rendering( p_data, nr_threads );
  if( p_parman_threads == NULL ) {
    log_error("%s, %d: could create renderer error!\n", __func__, __LINE__ );
    release_parman_data( p_data );
    return NULL;
  }

  return p_parman_threads;
}

int has_rendering_completed( t_parman_threads* p)
{
  int i, all_done;

  for( i=0, all_done = 1; i < p->nr_threads; ++i )
    all_done = all_done && p->thread[i].state.done;

  return all_done;
}
