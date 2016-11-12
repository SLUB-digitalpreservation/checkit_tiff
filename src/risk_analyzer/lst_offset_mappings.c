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
  assert(type < mt_END_marker);
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
  static mem_map_t memmap;
  memmap.count = 0;
  memmap.base_p = NULL;
  memmap.max_entries = 128;
  memmap.base_p = malloc (sizeof(mem_map_entry_t) * memmap.max_entries);
  if (NULL == memmap.base_p) {
	  perror ("could not allocate mem for memmap, abort");
  }
  /* size of tiff file in bytes */
  memmap.max_len = ctif->streamlen;
  /* header */
  add_mem_entry( &memmap, 0, 4, mt_constant);
  /* IFD0 Offset */
  add_mem_entry( &memmap, 4, 4, mt_ifd_offset);
  /* IFDO */
  uint32 ifd = get_ifd0_pos( ctif );
  uint32 count = get_ifd0_count( ctif);

  add_mem_entry( &memmap, 8, 2, mt_ifd); /* count of tags in ifd */
  int ifdbase=8+2;
  /* iterate through IFD0 entries */
  int tagidx;
  

  for (tagidx = 0; tagidx< count; tagidx++) {
	  add_mem_entry( &memmap, ifdbase+(tagidx*12), 8, mt_ifd); /* tagid, field type, count */
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
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_offset_to_standardized_value ); 
			  add_mem_entry( &memmap, offset, count*datasize, mt_standardized_value );
		  } else if (tag < 65000) { /* registered tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_offset_to_registered_value ); 
			  add_mem_entry( &memmap, offset, count*datasize, mt_registered_value );
		  } else { /* private tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_offset_to_private_value ); 
			  add_mem_entry( &memmap, offset, count*datasize, mt_private_value );
		  }
	  } else if (ifd_entry.value_or_offset==is_value) { /* embedded value */
		  if (tag < 32768) { /* standard tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_embedded_standardized_value ); 
		  } else if (tag < 65000) { /* registered tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_embedded_registered_value ); 
		  } else { /* private tag */
			  add_mem_entry( &memmap, ifdbase+(tagidx*12)+8, 4,mt_ifd_embedded_private_value ); 
		  }

	  }
  }
  /* check next IFD mark */
  uint32 offset = get_ifd0_pos(ctif );
  uint32 IFDn = get_next_ifd_pos( ctif, offset );
  add_mem_entry( &memmap, offset, 4, mt_ifd_offset);

  /* sort entries by offset */
  qsort(memmap.base_p, memmap.count, sizeof( mem_map_entry_t), compare_memmap);
  return &memmap;
}



void print_mem_map( mem_map_t * memmap_p ) {
	assert(NULL != memmap_p);
	mem_map_t memmap = *( memmap_p );
	printf("size of file is %u bytes\n", memmap.max_len);
	for (int j=0; j< memmap.count; j++) {
		mem_map_entry_t * i=memmap.base_p+j;
		printf ("j=%03i,offset=%06i count=%06i, (end=%06i) [%02i] type=%s\n", j,i->offset, i->count, (i->offset + i->count), i->mem_type, memtype_string[i->mem_type]);
	}
}

void print_mem_stats( mem_map_t * memmap_p) {
	uint32 size = memmap_p->max_len;
	uint32 stat[ mt_END_marker ];
	uint32 counted=0;
	for (int i=0; i< mt_END_marker; i++) {
		stat[i]=0;
	}

	for (int j=0; j< memmap_p->count; j++) {
		mem_map_entry_t * i=memmap_p->base_p+j;
		stat[i->mem_type]+= i->count;
		counted+=i->count;
	}
	for (int i=0; i<mt_END_marker; i++) {
		float ratio = ((float) (stat[i])) / ((float) size);
		printf("[%02i] type=%32s bytes=%6i ratio=%0.3f\n", i, memtype_string[i], stat[i], ratio);
	}
	printf("counted: %i bytes, size: %i bytes\n", counted, size);
}
