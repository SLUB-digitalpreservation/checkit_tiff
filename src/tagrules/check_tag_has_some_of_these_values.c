/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <string.h>
/*
#define DEBUG
*/

ret_t check_tag_has_some_of_these_values(ctiff_t * ctif, tag_t tag, int count, unsigned int * values) {
  //printf("check if tag %u (%s) has some of these %i-values", tag, TIFFTagName(tif, tag), count);
  tifp_check( ctif);
  char msg[EXPECTSTRLEN];
  char expected[EXPECTSTRLEN]="";
  snprintf(msg, sizeof(msg), "has some of these %i-values: ", count);
  int i;
  unsigned int * p = values;
  for (i=0; i< count; i++) {
    if( i>= 1) secstrcat (expected, ", ", EXPECTSTRLEN);
    secstrcat (expected, int2str(*p), EXPECTSTRLEN);
    p++;
  }
  secstrcat (msg, expected, EXPECTSTRLEN);
  tif_rules_tag(tag, strdup(msg));
  ret_t res = check_tag_has_valid_type( ctif, tag);
  if (res.returncode == 0) {

  TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
  switch (datatype) {
    case TIFF_LONG: { 
                      p = values;
                      ret_t res;
                      for (i=0; i< count; i++) {
#ifdef DEBUG
                        printf("### value = %u", *p);
#endif
                        res = check_tag_has_u32value(ctif, tag, *p);
                        if (res.returncode == 0) return res;
                        p++;
                      }
                      uint32 val;
                      TIFFGetField(ctif->tif, tag, &val);
                      return tif_fails_tag( tag, strdup(expected), int2str(val));
                      break;
                    }
    case TIFF_SHORT: {
                       p = values;
                       ret_t res;
                       for (i=0; i< count; i++) {
#ifdef DEBUG
                         printf("### value = %u", *p);
#endif
                         res = check_tag_has_u16value(ctif, tag, *p);
                         if (res.returncode == 0) return res;
                         p++;
                       }
                       uint16 val;
                       TIFFGetField(ctif->tif, tag, &val);
                       return tif_fails_tag( tag, strdup(expected), int2str(val));
                       break;
                     }
    case TIFF_RATIONAL: {
                          p = values;
                          ret_t res;
                          for (i=0; i< count; i++) {
#ifdef DEBUG
                            printf("### value = %u", *p);
#endif
                            res = check_tag_has_fvalue(ctif, tag, *p);
                            if (res.returncode == 0) return res;
                            p++;
                          }
                          float val;
                          TIFFGetField(ctif->tif, tag, &val);
                          return tif_fails_tag( tag, strdup(expected), float2str(val));
                          //tif_fails("tag %u (%s) does not have some of expected values (but have:%f)\n", tag, TIFFTagName(tif, tag), val);
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
  return res; 
}

