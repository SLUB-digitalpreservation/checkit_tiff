/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015/16
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE
#include <math.h>
#include "check.h"
#include "check_helper.h"
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
/*
#define DEBUG
*/

#define ANSI_RED "\033[22;31m"
#define ANSI_RED_BOLD     "\033[01;31m"
#define ANSI_GREEN "\033[22;32m"
#define ANSI_GREEN_BOLD "\033[01;32m"
#define ANSI_RESET   "\033[22;30m"
#define ANSI_BLACK "\033[22;30m"
#define ANSI_BLACK_BOLD "\033[01;30m"
#define ANSI_BROWN "\033[22;33m"
#define ANSI_BLUE "\033[22;34m"
#define ANSI_BLUE_BOLD "\033[01;34m"
#define ANSI_PURPLE "\033[22;35m"
#define ANSI_CYAN "\033[22;36m"
#define ANSI_GREY "\033[22;37m"
#define ANSI_DARK_GREY "\033[01;30m"
#define ANSI_YELLOW "\033[01;33m"
#define ANSI_WHITE "\033[01;37m"
#define ANSI_INVERSE "\033[7m"


render_type render_engine=render_default;

void set_renderer_to_ansi() {
  if (isatty (STDOUT_FILENO)) {
    render_engine=render_ansi;
  }
}




const char * renderer_debug ( ret_t ret ) {
  char res[1024] = "";
  if (0 == ret.returncode) {
    secstrcat( res, "\t", 1024  );
    secstrcat (res, ANSI_GREEN, 1024);
  } else {
    secstrcat( res, "\t", 1024  );
    secstrcat (res, ANSI_RED_BOLD, 1024);
  }
  retmsg_t * startp = ret.returnmsg;
  int c = 0;
  while (NULL != startp && c < ret.count) {
        switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, ANSI_RESET    , 1024); break;
          case rm_tag:        secstrcat( res, ANSI_RED_BOLD , 1024); break;
          case rm_value:      secstrcat( res, ANSI_BLUE     , 1024); break;
          case rm_expected:   secstrcat( res, ANSI_RED      , 1024); break;
          case rm_hard_error: secstrcat( res, ANSI_RED_BOLD , 1024); break;
          case rm_error:      secstrcat( res, ANSI_RED      , 1024); break;
          case rm_warning:    secstrcat( res, ANSI_YELLOW   , 1024); break;
          case rm_file:       secstrcat( res, ANSI_BLUE_BOLD, 1024); break;
          default:            secstrcat( res, ANSI_INVERSE    , 1024);
        }
        secstrcat(res, startp->rm_msg, 1024);
        startp++;
        c++;
  }
  secstrcat(res, ANSI_RESET, 1024);
  secstrcat(res, "\n", 1024);
  return strdup( res );
}

const char * renderer_default ( ret_t ret ) {
  char res[1024] = "";
  if (0 == ret.returncode) {
  } else {
    secstrcat( res, "\t", 1024  );
  }
  retmsg_t * startp = ret.returnmsg;
  int c = 0;
  while (NULL != startp && c < ret.count) {
        secstrcat(res, startp->rm_msg, 1024);
        startp++;
        c++;
  }
  secstrcat(res, "\n", 1024);
  return strdup( res );
}


const char * renderer_ansi ( ret_t ret ) {
  char res[1024] = "";
  if (0 == ret.returncode) {
    secstrcat( res, "", 1024  );
    secstrcat (res, ANSI_GREEN, 1024);
  } else {
    secstrcat (res, ANSI_RED_BOLD, 1024);
    secstrcat( res, "==> ", 1024  );
  }
  retmsg_t * startp = ret.returnmsg;
  int c = 0;
  while (NULL != startp && c < ret.count) {
        switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, ANSI_RESET    , 1024); break;
          case rm_tag:        secstrcat( res, ANSI_BLACK_BOLD, 1024); break;
          case rm_value:      secstrcat( res, ANSI_BLUE     , 1024); break;
          case rm_expected:   secstrcat( res, ANSI_RED      , 1024); break;
          case rm_hard_error: secstrcat( res, ANSI_RED_BOLD , 1024); break;
          case rm_error:      secstrcat( res, ANSI_RED      , 1024); break;
          case rm_warning:    secstrcat( res, ANSI_YELLOW   , 1024); break;
          case rm_file:       secstrcat( res, ANSI_BLUE_BOLD, 1024); break;
          default:            secstrcat( res, ANSI_RESET  , 1024);
        }
        secstrcat(res, startp->rm_msg, 1024);
        startp++;
        c++;
  }
  secstrcat(res, ANSI_RESET, 1024);
  secstrcat(res, "\n", 1024);
  return strdup( res );
}

const char * renderer ( ret_t ret ) {
  // call debug renderer
  switch (render_engine) {
        case render_ansi: return renderer_ansi( ret );
        case render_debug: return renderer_debug( ret );
        default: return renderer_default( ret );
  }
  return "";
}
