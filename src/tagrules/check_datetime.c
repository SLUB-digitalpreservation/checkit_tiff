/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see file LICENSE)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <assert.h>
#include <string.h>

/** check if date / time values are within correct ranges 
 * @param year year
 * @param month month
 * @param day day
 * @param hour hour
 * @param min min
 * @param sec sec
 * @return 0 if success, otherwise -1
 */
int test_plausibility (int * year, int * month, int * day, int * hour, int * min, int * sec) {
#ifdef DEBUG
  printf ("found: y=%d m=%d d=%d h=%d m=%d s=%d\n", *year, *month, *day, *hour, *min, *sec);
#endif
  if (
      1500 < *year && 
      2100 > *year &&
      0 < *month &&
      13 > *month &&
      0 < *day &&
      32 > *day &&
      0 <= *hour &&
      24 > *hour &&
      0 <= *min &&
      60 > *min &&
      0 <= *sec &&
      60 > *sec
     ) {
    return 0;
  } else {
    return -1;
  }
}


/** loads a tiff, fix it if needed, stores tiff
 * @param filename filename which should be processed, repaired
 */
ret_t check_datetime(ctiff_t * ctif ) {

  ret_t ret = get_empty_ret();

  tifp_check( ctif);
  ret_t rc=check_tag_quiet(ctif, TIFFTAG_DATETIME);
  if (rc.returncode != is_valid) return rc;

  /* find date-tag and fix it */
  TIFFDataType datatype =  TIFFGetRawTagType( ctif, TIFFTAG_DATETIME );
  if (datatype != TIFF_ASCII) {
    char array[VALUESTRLEN];
    snprintf(array, sizeof(array), "type:%s", TIFFTypeName(datatype));
    ret = set_value_found_ret( &ret, array);
    ret.returncode = tagerror_unexpected_type_found;
    return ret;
  }
  int count=0;
  char *datetime=NULL;
  ret = TIFFGetFieldASCII(ctif, TIFFTAG_DATETIME, &datetime, &count);

  // printf("DATETIME='%s'\n", datetime);
    int day=0;
    int month=0;
    int year=0;
    int hour=0;
    int min=0;
    int sec=0;
    int r = 0;
    for (int i=0; i<count; i++) {
        if ((datetime[i] == '\0') && (i != 19)) { /* \0 at 20th byte  allowed */
          r = i+1;
          break;
        }
    }
#ifdef DEBUG
    printf(" count=%u\n\n", count);
#endif
    ret = set_value_found_ret(&ret, datetime);
    if (0 == r) {
      if (6 == sscanf(datetime, "%04d:%02d:%02d%02d:%02d:%02d", &year, &month, &day, &hour, &min, &sec)) {
        if (0 == test_plausibility(&year, &month, &day, &hour, &min, &sec)) {
          ret.returncode=0;
          return ret;
        } else {
          ret.returncode = tagerror_datetime_not_plausible;
          return ret;
        }
      } else {
        ret.returncode = tagerror_datetime_wrong_format;
        return ret;
      }
    } else {
        ret.returncode = tagerror_datetime_wrong_size;
        return ret;
    }
    ret.returncode = should_not_occure;
    return ret;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
