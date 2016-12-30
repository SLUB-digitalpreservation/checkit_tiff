/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE

#include <string.h>

#include "check.h"
#include "check_helper.h"
#include <assert.h>
/* 
#define DEBUG
*/

ret_t check_tag_has_value_in_range(ctiff_t * ctif, tag_t tag, unsigned int a, unsigned int b) {
  //printf("check if tag %u (%s) has value in range %u - %u\n", tag, TIFFTagName(tif, tag), a, b);
  tifp_check( ctif);
   char msg[EXPECTSTRLEN];
  snprintf(msg, sizeof(msg), "has value in range %u - %u", a, b);
  tif_rules_tag(tag, strdup(msg));
  ret_t res = check_tag_has_valid_type( ctif, tag);
    if (res.returncode == 0) {

  if (a > b) { unsigned int c=a; a=b; b=c; }
  TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
  switch (datatype) {
    case TIFF_LONG: {
                      uint32 * valp = NULL;
                      uint32 val;
                      int found=TIFFGetFieldLONG(ctif, tag, &valp);
                      if (1 == found) {
                        val = *valp;
                        if ((val >= a && val <= b )) { 
                          ret_t tmp_res;
                          tmp_res.returnmsg=NULL;
                          tmp_res.returncode=0;
			  free( valp);
                          return tmp_res;

                        } else {
		          free(valp);		
                          return tif_fails_tag( tag, range2str(a, b), int2str(val) );
                        }
                      } else {
			if (NULL != valp) {
				free(valp);
				valp=NULL;
			}
                        return tif_fails_tag( tag, "", "was not found, but requested because defined");                   
		      }

                      break;
                    }
    case TIFF_SHORT: {
                      uint16 * valp = NULL;
                      uint16 val;
                      int found=TIFFGetFieldSHORT(ctif, tag, &valp);
                      if (1 == found) {
                        val = *valp;
                        if ((val >= a && val <= b )) { 
                          ret_t tmp_res;
                          tmp_res.returnmsg=NULL;
                          tmp_res.returncode=0;
			  free( valp);
                          return tmp_res;

                        } else {
			  free( valp);
                          return tif_fails_tag( tag, range2str(a, b), int2str(val) );
                        }
		      } else {
			      if (NULL != valp) {
				      free(valp);
				      valp=NULL;
			      }

                       return tif_fails_tag( tag, "", "was not found, but requested because defined");                      
		      }

                      break;
                     }
    case TIFF_RATIONAL: {
                      float * valp = NULL;
                      float val;
                      int found=TIFFGetFieldRATIONAL(ctif, tag, &valp);
                      if (1 == found) {
                        val = * valp;
                        if ((val >= a && val <= b )) { 
                          ret_t tmp_res;
                          tmp_res.returnmsg=NULL;
                          tmp_res.returncode=0;
			  free( valp);
                          return tmp_res;

                        } else {
                          // tif_fails("tag %u (%s) should have value in range %u - %u, but have count/value=%f\n", tag,TIFFTagName(tif, tag),  a, b, val);
			  free( valp);
                          return tif_fails_tag( tag, range2str(a, b), float2str(val) );
			}
		      } else {
			      if (NULL != valp) {
				      free(valp);
				      valp=NULL;
			      }

			      return tif_fails_tag( tag, "", "was not found, but requested because defined");                      
		      }

                      break;
                        }
    default: /*  none */
                        {
                          // tif_fails("tag %u (%s) should have values of type long, short or float, but was:%i\n", tag, TIFFTagName(tif, tag), datatype);
                          char array[VALUESTRLEN];
                          snprintf(array, sizeof(array), "but was:%i", datatype);
                          return tif_fails_tag( tag, "of type long, short or float", array);
                        }
  }
  }
  return tif_fails_by_returns( res );
}
