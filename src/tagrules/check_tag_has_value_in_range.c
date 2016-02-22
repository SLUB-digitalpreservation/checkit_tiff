/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "../check.h"
#include "../check_helper.h"
#include <assert.h>
/* 
#define DEBUG
*/

ret_t check_tag_has_value_in_range(TIFF* tif, tag_t tag, unsigned int a, unsigned int b) {
  printf("check if tag %u (%s) has value in range %u - %u\n", tag, TIFFTagName(tif, tag), a, b);
  tifp_check( tif);
  ret_t res = check_tag_has_valid_type( tif, tag);
    if (res.returncode == 0) {

  if (a > b) { unsigned int c=a; a=b; b=c; }
  TIFFDataType datatype =  TIFFGetRawTagType( tif, tag );
  switch (datatype) {
    case TIFF_LONG: {
                      uint32 val;
                      int found=TIFFGetField(tif, tag, &val);
                      if (1 == found) {
                        if ((val >= a && val <= b )) { 
                          ret_t res;
                          res.returnmsg=NULL;
                          res.returncode=0;
                          return res;

                        } else {
                          return tif_fails_tag( tag2str(tif, tag), range2str(a, b), int2str(val) );
                        }
                      } else {
                        return tif_fails_tag( tag2str(tif, tag), "", "was not found, but requested because defined");                      }

                      break;
                    }
    case TIFF_SHORT: {
                       uint16 val;
                      int found=TIFFGetField(tif, tag, &val);
                      if (1 == found) {
                        if ((val >= a && val <= b )) { 
                          ret_t res;
                          res.returnmsg=NULL;
                          res.returncode=0;
                          return res;

                        } else {
                          return tif_fails_tag( tag2str(tif, tag), range2str(a, b), int2str(val) );
                        }
                      } else {
                       return tif_fails_tag( tag2str(tif, tag), "", "was not found, but requested because defined");                      }

                      break;
                     }
    case TIFF_RATIONAL: {
                       float val;
                      int found=TIFFGetField(tif, tag, &val);
                      if (1 == found) {
                        if ((val >= a && val <= b )) { 
                          ret_t res;
                          res.returnmsg=NULL;
                          res.returncode=0;
                          return res;

                        } else {
                          // tif_fails("tag %u (%s) should have value in range %u - %u, but have count/value=%f\n", tag,TIFFTagName(tif, tag),  a, b, val);
                          return tif_fails_tag( tag2str(tif, tag), range2str(a, b), float2str(val) );
                        }
                      } else {
                        return tif_fails_tag( tag2str(tif, tag), "", "was not found, but requested because defined");                      }

                      break;
                        }
    default: /*  none */
                        {
                          // tif_fails("tag %u (%s) should have values of type long, short or float, but was:%i\n", tag, TIFFTagName(tif, tag), datatype);
                          char array[10];
                          snprintf(array, sizeof(array), "but was:%i", datatype);
                          return tif_fails_tag( tag2str(tif, tag), "of type long, short or float", array);
                        }
  }
  }
  return tif_fails_by_returns( res );
}
