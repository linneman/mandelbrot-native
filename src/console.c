/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rendering.h>
#include <log.h>


int start_head_less( const int nr_threads )
{
  t_parman_data*    p_data;
  t_parman_threads* p_threads;

  const int res_x = 160;
  const int res_y = 50;
  const int iterations = 1000000;

  int retcode, i, j, all_done;


  p_data = create_parman_data( res_x, res_y,
                       -2.0 /* min_x */,
                       -1.25 /* min_y */,
                       2.5 /* width */,
                       2.5 /* height */,
                       iterations /* interations */ );
  if( p_data == NULL )
    return -1;

  p_threads = start_rendering( p_data, nr_threads );
  if( p_threads == NULL )
    return -1;

  while( 1 ) {
    print_mandel( p_data );
    usleep( 500000 );

    for( j=0, all_done = 1; j<nr_threads; ++j ) {
      all_done = all_done && p_threads->thread[j].state.done;
    }

    if( all_done ) {
      print_mandel( p_data );
      break;
    }
  }

  release_rendering( p_threads );
  release_parman_data( p_data );

  return 0;
}
