/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#ifndef _FIXIT_TIFF_MSG_TIFFPARSE
#define _FIXIT_TIFF_MSG_TIFFPARSE
#include "check.h"
const char * TIFFTypeName( uint16 tagtype ); 
const char * TIFFTagName( tag_t tag );
#endif
