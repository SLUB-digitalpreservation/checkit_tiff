/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#ifndef _FIXIT_TIFF_CHECK_HELPER
#define _FIXIT_TIFF_CHECK_HELPER
#include <stdlib.h>
#include <tiff.h>
#include <tiffio.h>

typedef enum{ render_default, render_debug, render_ansi} render_type;

/* helper */
ret_t check_tag_has_fvalue(ctiff_t * ctif, tag_t tag, float value);
ret_t check_tag_has_u16value(ctiff_t * ctif, tag_t tag, uint16 value);
ret_t check_tag_has_u32value(ctiff_t * ctif, tag_t tag, uint32 value);
const char * TIFFTagName (tag_t tag);
uint32 TIFFGetRawTagTypeListEntry( ctiff_t * ctif, int tagidx );
tag_t TIFFGetRawTagListEntry( ctiff_t * ctif, int tagidx ) ;
int TIFFGetRawTagListCount (ctiff_t * ctif, uint32 ifpos) ;
TIFFDataType TIFFGetRawTagType(ctiff_t * ctif, tag_t tag);
ifd_entry_t TIFFGetRawIFDEntry( ctiff_t * ctif, tag_t tag);
ifd_entry_t TIFFGetRawTagIFDListEntry( ctiff_t * ctif, int tagidx );
offset_t read_offsetdata( ctiff_t * ctif, uint32 address, uint16 count, uint16 datatype);
//uint32 get_first_IFD(ctiff_t * ctif);
//int parse_header_and_endianess(ctiff_t * ctif);
//int TIFFIsByteSwapped(TIFF * tif);
const char * renderer ( const ret_t ret );
void set_renderer_to_ansi();
char * secstrcat (char * dest, const char * src, int maxsize);
ctiff_t * initialize_ctif( const char * filename );
void free_ctif( ctiff_t * ctif);
uint32 get_ifd0_pos( ctiff_t * ctif );
uint32 get_next_ifd_pos( ctiff_t * ctif, uint32 actual_pos );
uint16 get_ifd0_count( ctiff_t * ctif );
char is_byteswapped( ctiff_t * ctif );
#endif
