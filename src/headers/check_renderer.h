/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#ifndef _FIXIT_TIFF_CHECK_RENDERER
#define _FIXIT_TIFF_CHECK_RENDERER

#include "check_helper.h"

/* 65536 Tags a 256 chars message + 100 Messages */
#define RENDERSIZE (65536*256 + 100*256)
typedef enum{ render_default, render_ansi, render_xml} render_type;
typedef enum{ within_valid, within_error, within_file, within_summaryerror, within_harderror } render_context_t;

const char * renderer ( const retmsg_t * ret );
void set_renderer_to_ansi();
void set_renderer_to_xml(const char *);
void set_renderer_to_quiet();
const char * renderer_ansi ( const retmsg_t * ret, short int );
const char * renderer_default ( const retmsg_t * ret, short int );
const char * renderer_xml ( const retmsg_t * ret, const char * xml );

#endif

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
