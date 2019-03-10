/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#include <unistd.h>
#include <sdlif.h>
#include <console.h>
#include <log.h>
#include <getopt.h>

#define MAX_THREADS 1000

static int start_gui( const int nr_threads )
{
  t_gui* p_gui;
  int retcode, i, j, all_done;

  p_gui = create_gui( nr_threads );
  if( p_gui == NULL ) {
    return -1;
  }

  printf("Parallel Rendering of the Mandelbrot Set\n");
  printf("Created by Otto Linnemann\n\n");
  printf("Copyright 2019 GNU General Public Licence. All rights reserved\n\n");
  printf("Drag with left mouse key the area to enlarge.\n");
  printf("Use right mouse key or two finger tap on the Mac to zoom out.\n\n");

  while( ! p_gui->done ) {
    // printf("looping ...\n");
    sleep( 1 );
  }

  release_gui( p_gui );

  return 0;
}


static void help( char* name )
{
  printf("Parallel Rendering of the Mandelbrot Set\n");
  printf("Created by Otto Linnemann\n\n");
  printf("Copyright 2019 GNU General Public Licence. All rights reserved\n\n");
  printf("Invocation: %s [ options ]\n\n", name );
  printf("Options:\n");
  printf("--threads\n-t\n");
  printf("\tNumber of processing threads\n\n");
  printf("--nogui\n-n\n");
  printf("\tInvoke with text console\n\n");
  printf("--help\n-h\n");
  printf("\tThis help screen.\n\n");
}


int main(int argc, char* argv[])
{
  int optindex, optchar;
  const struct option long_options[] =
  {
    { "help", no_argument, NULL, 'h' },
    { "threads", required_argument, NULL, 't' },
    { "nogui", no_argument, NULL, 'n' },
  };
  int nr_threads = 16;
  int headless = 0;

  while( ( optchar = getopt_long( argc, argv, "hnt:", long_options, &optindex ) ) != -1 )
  {
    switch( optchar )
    {
    case 'h':
      help( argv[0] );
      return -1;
      break;

    case 't':
      nr_threads = atoi( optarg );
      if( nr_threads < 1 || nr_threads >= MAX_THREADS ) {
        log_error("number of threads must be in range [%d:%d]\n", 1, MAX_THREADS );
        return -1;
      }
      break;

    case 'n':
      headless = 1;
      break;

    default:
      fprintf( stderr, "input argument error!\n");
      return -1;
    }
  }

  if( headless )
    return start_head_less( nr_threads );
  else
    return start_gui( nr_threads );
}
