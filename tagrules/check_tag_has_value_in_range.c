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
  tifp_check( tif)
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
                          tif_fails("tag %u (%s) should have value in range %u - %u, but have count/value=%u\n", tag,TIFFTagName(tif, tag), a, b, val);
                        }
                      } else {
                        tif_fails("tag %u (%s) should exist, because defined\n", tag,TIFFTagName(tif, tag));
                      }

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
                          tif_fails("tag %u (%s) should have value in range %u - %u, but have count/value=%u\n", tag,TIFFTagName(tif, tag),  a, b, val);
                        }
                      } else {
                        tif_fails("tag %u (%s) should exist, because defined\n", tag, TIFFTagName(tif, tag));
                      }

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
                          tif_fails("tag %u (%s) should have value in range %u - %u, but have count/value=%f\n", tag,TIFFTagName(tif, tag),  a, b, val);
                        }
                      } else {
                        tif_fails("tag %u (%s) should exist, because defined\n", tag,TIFFTagName(tif, tag));
                      }

                      break;
                        }
    default: /*  none */
                        tif_fails("tag %u (%s) should have values of type long, short or float, but was:%i\n", tag,TIFFTagName(tif, tag), datatype);
  }

}
