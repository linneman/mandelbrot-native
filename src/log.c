/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#include <log.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/syslog.h>


#define LOG_SETTING  ( LOG_NOWAIT | LOG_PID )

void log_message( const char* fmt, ... )
{
  va_list args;

  va_start( args, fmt );
  // openlog( LOG_TAG, LOG_SETTING, LOG_SYSLOG );
  // vsyslog( LOG_INFO, fmt, args );
  // closelog();
  vfprintf( stderr, fmt, args );
  va_end( args );
}

void log_error( const char* fmt, ... )
{
  va_list args;

  va_start( args, fmt );
  // openlog( LOG_TAG, LOG_SETTING, LOG_SYSLOG );
  // vsyslog( LOG_ERR, fmt, args );
  // closelog();
  vfprintf( stderr, fmt, args );
  va_end( args );
}
