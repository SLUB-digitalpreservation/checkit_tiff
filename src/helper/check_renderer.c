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
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
/*
#define DEBUG
*/


void renderer_debug ( ret_t ret ) {
  //printf("\t");
  if (0 == ret.returncode) {
    printf("OK|");
  } else {
    printf("FALSE|");
  }
  retmsg_t * startp = ret.returnmsg;
  int c = 0;
  while (NULL != startp && c < ret.count) {
        switch (startp->rm_type) {
          case rm_rule: printf ("RULE|"); break;
          case rm_tag: printf ("TAG|"); break;
          case rm_value:printf ("VALUE|"); break;
          case rm_expected:printf ("EXPECTED|"); break;
          case rm_hard_error:printf ("HERROR|"); break;
          case rm_error:printf ("ERR|"); break;
          case rm_warning:printf ("WARN|"); break;
          default: ;
        }
        printf("%s", startp->rm_msg);
        startp++;
        c++;
  }
  printf ("\n");
}

char * secstrcat (char * dest, char * src, int maxsize) {
        int destsize = strlen(dest);
        int srclen = strlen(src);
        //fprintf(stderr, "1dest='%s' , src='%s', destsize=%i, srclen=%i\n", dest, src, destsize, srclen);
        if (destsize+srclen < maxsize) {
          strcat( dest, src);
        }
        //destsize = strlen(dest);
        //fprintf(stderr, "2dest='%s' , src='%s', destsize=%i, srclen=%i\n", dest, src, destsize, srclen);
        return dest;
}

const char * renderer_ansi ( ret_t ret ) {
  char res[1024] = "";
#define ANSI_RED "\033[22;31m"
#define ANSI_RED_BOLD     "\033[01;31m"
#define ANSI_GREEN "\033[22;32m"
#define ANSI_GREEN_BOLD "\033[01;32m"
#define ANSI_RESET   "\033[22;30m"
#define ANSI_BOLD "\033[0;31m"
#define ANSI_ITALIC "\033[0;34m"
#define ANSI_UNDERLINE "\033[0;32m"
#define ANSI_DEFAULT "\033[0;37m"
#define ANSI_BLACK "\033[22;30m"
#define ANSI_BROWN "\033[22;33m"
#define ANSI_BLUE "\033[22;34m"
#define ANSI_PURPLE "\033[22;35m"
#define ANSI_CYAN "\033[22;36m"
#define ANSI_GREY "\033[22;37m"
#define ANSI_DARK_GREY "\033[01;30m"
#define ANSI_YELLOW "\033[01;33m"
#define ANSI_WHITE "\033[01;37m"
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
          case rm_rule:       secstrcat( res, ANSI_DARK_GREY, 1024); break;
          case rm_tag:        secstrcat( res, ANSI_RED_BOLD , 1024); break;
          case rm_value:      secstrcat( res, ANSI_BLUE     , 1024); break;
          case rm_expected:   secstrcat( res, ANSI_RED      , 1024); break;
          case rm_hard_error: secstrcat( res, ANSI_RED_BOLD , 1024); break;
          case rm_error:      secstrcat( res, ANSI_RED      , 1024); break;
          case rm_warning:    secstrcat( res, ANSI_YELLOW   , 1024); break;
          default:            secstrcat( res, ANSI_RESET    , 1024);
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
  return renderer_ansi( ret );
}
