/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE
#include "check_renderer.h"
#include <assert.h>



const char * renderer_default ( const retmsg_t * ret ) {
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  } 
  secstrcat( res, "\t", RENDERSIZE  );
  const retmsg_t * startp = ret;
  while (NULL != startp && NULL != startp->next) {
        switch (startp->rm_type) {
          case rm_is_valid:   /* valid results not shown */ break;
          case rm_file:       secstrcat( res, "file: ", RENDERSIZE  );
			      secstrcat(res, startp->rm_msg, RENDERSIZE);
			      break;
	  default: 	      secstrcat(res, startp->rm_msg, RENDERSIZE);
			      break;
	}
	if (startp->rm_type == rm_endrule || startp->rm_type == rm_endtiff || startp->rm_type==rm_file) {
		  secstrcat(res, "\n", RENDERSIZE);
	}
	startp=startp->next;
  }
  secstrcat(res, "\n", RENDERSIZE);
  return res ;
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
