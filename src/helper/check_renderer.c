/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE
#include <math.h>
#include "check.h"
#include "check_helper.h"
#include "check_renderer.h"
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>

render_type render_engine=render_default;

void set_renderer_to_ansi() {
  if (isatty (STDOUT_FILENO)) {
    render_engine=render_ansi;
  }
}
void set_renderer_to_xml() {
    render_engine=render_xml;
}

const char * renderer ( const retmsg_t * ret ) {
  // call debug renderer
  switch (render_engine) {
    case render_xml: return renderer_xml( ret );
    case render_ansi: return renderer_ansi( ret );
    default: return renderer_default( ret );
  }
  return "";
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
