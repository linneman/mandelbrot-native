/*
 * Parallel Rendering of the Mandelbrot Set
 *
 * Created by Otto Linnemann
 * Copyright 2019 GNU General Public Licence. All rights reserved
 */

#ifndef LOG_H_
#define LOG_H_


#ifdef __cplusplus
extern "C" {
#endif

/*! \file log.h
    \brief logging

    \addtogroup utils utils
    @{
 */

#ifndef LOG_TAG
#define LOG_TAG "MANDEL"          /*!< used application log tag in syslog */
#endif


/*!
 * for message log output, currently to stdout
 */
void log_message( const char* fmt, ... );


/*!
 * for error log output, currently to stderr
 */
void log_error( const char* fmt, ... );


/*! @} */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef LOG_H_ */
