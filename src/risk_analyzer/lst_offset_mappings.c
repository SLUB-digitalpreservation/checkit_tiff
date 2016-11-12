/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015/2016
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <unistd.h>
#include <assert.h>


void add_mem_entry(mem_map_t * memmap_p, uint32 offset, uint32 count, memtype_t type) {
  mem_map_t memmap = *( memmap_p );
  assert(NULL != memmap.base_p);
  assert(0 < memmap.max_entries);
  assert(count < memmap.max_entries);
  assert(offset + count <= memmap.max_len);
  mem_map_entry_t * memmap_it_p = memmap.base_p;
  memmap_it_p = memmap.base_p + memmap.count;
  memmap_it_p->offset=offset;
  memmap_it_p->count=count;
  memmap_it_p->mem_type=type;
  memmap.count++;
  assert( memmap.max_entries > memmap.count);
  *( memmap_p) = memmap;
}

int compare_memmap(void const * lhs, void const * rhs) {
	mem_map_entry_t left = *((mem_map_entry_t *) lhs);
	mem_map_entry_t right = *((mem_map_entry_t *) rhs);

	return ((left.offset) - (right.offset));
}


mem_map_t * scan_mem_map(ctiff_t * ctif) {
  assert( NULL != ctif);
  mem_map_t memmap;
  memmap.count = 0;
  memmap.base_p = NULL;
  memmap.max_entries = 128;
  memmap.base_p = malloc (sizeof(mem_map_entry_t) * memmap.max_entries);
  if (NULL == memmap.base_p) {
	  perror ("could not allocate mem for memmap, abort");
  }
  /* size of tiff file in bytes */
  memmap.max_len = ctif->streamlen;
  printf("size of file is %lu bytes\n", memmap.max_len);
  /* header */
  add_mem_entry( &memmap, 0, 4, constant);
  /* IFD0 Offset */
  add_mem_entry( &memmap, 4, 4, ifd_offset);
  /* IFDO */
  uint32 ifd = get_ifd0_pos( ctif );
  uint32 count = get_ifd0_count( ctif);

  add_mem_entry( &memmap, 8, 2, ifd); /* count of tags in ifd */
  int ifdbase=8+2;
  /* iterate through IFD0 entries */
  int tagidx;
  

  for (tagidx = 0; tagidx< count; tagidx++) {
	  add_mem_entry( &memmap, ifdbase+(tagidx*12), 8, ifd); /* tagid, field type, count */
	  ifd_entry_t ifd_entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
	  uint32 tag = TIFFGetRawTagListEntry( ctif, tagidx);
	  TIFFDataType datatype = TIFFGetRawTagType( ctif, tag);
	  int datasize;
	  switch (datatype) {
		  case TIFF_ASCII: datasize = 1; break;
		  case TIFF_LONG: datasize =  4; break;
		  case TIFF_SHORT: datasize =  2; break;
		  case TIFF_BYTE: datasize =  1; break;
		  case TIFF_UNDEFINED: datasize =  1; break;
		  case TIFF_RATIONAL: datasize =  8; break;
		  case TIFF_SSHORT: datasize =  2; break;
		  case TIFF_SBYTE: datasize =  1; break;
		  case TIFF_SLONG: datasize =  4; break;
		  case TIFF_SRATIONAL: datasize =  8; break;
		  case TIFF_FLOAT: datasize =  4; break;
		  case TIFF_DOUBLE: datasize =  8; break;
	  }

	  if (ifd_entry.value_or_offset==is_offset) { /* offset */
		  uint32 offset = ifd_entry.data32offset;
		  uint16 count = ifd_entry.count;


		  if (tag < 32768) { /* standard tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,ifd_offset_to_standardized_value ); 
			  add_mem_entry( &memmap, offset, count*datasize, standardized_value );
		  } else if (tag < 65000) { /* registered tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,ifd_offset_to_registered_value ); 
			  add_mem_entry( &memmap, offset, count*datasize, registered_value );
		  } else { /* private tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,ifd_offset_to_private_value ); 
			  add_mem_entry( &memmap, offset, count*datasize, private_value );
		  }
	  } else if (ifd_entry.value_or_offset==is_value) { /* embedded value */
		  if (tag < 32768) { /* standard tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,ifd_embedded_standardized_value ); 
		  } else if (tag < 65000) { /* registered tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,ifd_embedded_registered_value ); 
		  } else { /* private tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,ifd_embedded_private_value ); 
		  }

	  }
  }
  /* check next IFD mark */
  uint32 offset = get_ifd0_pos(ctif );
  uint32 IFDn = get_next_ifd_pos( ctif, offset );
  add_mem_entry( &memmap, offset, 4, ifd_offset);

  /* TODO */
  int j=0;
  for (j=0; j< memmap.count; j++) {
	  mem_map_entry_t * i=memmap.base_p+j;
	  printf ("j=%i, offset=%i count=%i, (end=%i) type=%i\n", j, i->offset, i->count, (i->offset + i->count), i->mem_type);
  }
  printf("%i entries\n", memmap.count);
  /* sort entries by offset */
  qsort(memmap.base_p, memmap.count, sizeof( mem_map_entry_t), compare_memmap);

  printf("---------------------------\n");
  /* TODO */
  for (j=0; j< memmap.count; j++) {
	  mem_map_entry_t * i=memmap.base_p+j;
	  printf ("j=%i,offset=%i count=%i, (end=%i) type=%i\n", j,i->offset, i->count, (i->offset + i->count), i->mem_type);
  }
}
