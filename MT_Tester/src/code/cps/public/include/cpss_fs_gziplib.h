/******************************************************************************
* COPYRIGHT @ Reserved
*******************************************************************************
* File         : 
* Function     : 
* Statement    : 
* Creation Info：
******************************************************************************/
/******************************************************************************
* Modification: 
* Time        ：
* Modifier    ：
* Statement   ：
******************************************************************************/
#ifndef WIN32
#include <ioLib.h> /*for vxworks*/
#endif /*WIN32*/
#if defined(NB_VXWORKS)/*pc mode for super nb*/
#define NO_FCNTL_H
#include "osp_usr_ext.h"
#endif

/*#include "osp_usr_ext.h"*/

/* gzip.h -- common declarations for all gzip modules
* Copyright (C) 1992-1993 Jean-loup Gailly.
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License, see the file COPYING.
*/
#ifndef RETSIGTYPE
#  define RETSIGTYPE void
#endif

#if defined(__STDC__) || defined(PROTO)
#  define OF(args)  args
#else
#  define OF(args)  ()
#endif

#ifdef __STDC__
typedef void *voidp;
#else
typedef char *voidp;
#endif

/* I don't like nested includes, but the string and io functions are used
* too often
*/
#include <stdio.h>
#if !defined(NO_STRING_H) || defined(STDC_HEADERS)
#  include <string.h>
#  if !defined(STDC_HEADERS) && !defined(NO_MEMORY_H) && !defined(__GNUC__)
/*#    include <memory.h>*/
#  endif
#  define memzero(s, n)     memset ((voidp)(s), 0, (n))
#else
#  include <strings.h>
#  define strchr            index 
#  define strrchr           rindex
#  define memcpy(d, s, n)   bcopy((s), (d), (n)) 
#  define memcmp(s1, s2, n) bcmp((s1), (s2), (n)) 
#  define memzero(s, n)     bzero((s), (n))
#endif


#define local static

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

/* Return codes from gzip */
#define OK      0
#ifndef GZIP_ERROR
#define GZIP_ERROR   1
#endif
#define WARNING 2

/* Compression methods (see algorithm.doc) */
#define STORED      0
#define COMPRESSED  1
#define PACKED      2
#define LZHED       3
/* methods 4 to 7 reserved */
#define DEFLATED    8
#define MAX_METHODS 9
extern int method;         /* compression method */

						   /* To save memory for 16 bit systems, some arrays are overlaid between
						   * the various modules:
						   * deflate:  prev+head   window      d_buf  l_buf  outbuf
						   * unlzw:    tab_prefix  tab_suffix  stack  inbuf  outbuf
						   * inflate:              window             inbuf
						   * unpack:               window             inbuf  prefix_len
						   * unlzh:    left+right  window      c_table inbuf c_len
						   * For compression, input is done in window[]. For decompression, output
						   * is done in window except for unlzw.
*/
#ifdef WIN32

#else
#define		SMALL_MEM
#endif
#ifndef	INBUFSIZ
#  ifdef SMALL_MEM
#    define INBUFSIZ  0x2000  /* input buffer size */
#  else
#    define INBUFSIZ  0x8000  /* input buffer size */
#  endif
#endif
#define INBUF_EXTRA  64     /* required by unlzw() */

#ifndef	OUTBUFSIZ
#  ifdef SMALL_MEM
#    define OUTBUFSIZ   8192  /* output buffer size */
#  else
#    define OUTBUFSIZ  16384  /* output buffer size */
#  endif
#endif
#define OUTBUF_EXTRA 2048   /* required by unlzw() */

#ifndef DIST_BUFSIZE
#  ifdef SMALL_MEM
#    define DIST_BUFSIZE 0x2000 /* buffer for distances, see trees.c */
#  else
#    define DIST_BUFSIZE 0x8000 /* buffer for distances, see trees.c */
#  endif
#endif

#ifdef DYN_ALLOC
#  define EXTERN(type, array)  extern type * near array
#  define DECLARE(type, array, size)  type * near array
#  define ALLOC(type, array, size) { \
	array = (type*)fcalloc((size_t)(((size)+1L)/2), 2*sizeof(type)); \
	if (array == NULL) error("insufficient memory"); \
}
#  define GZIP_FREE(array) {if (array != NULL) fcfree(array), array=NULL;}
#else
#  define EXTERN(type, array)  extern type array[]
#  define DECLARE(type, array, size)  type array[size]
#  define ALLOC(type, array, size)
#  define GZIP_FREE(array)
#endif

EXTERN(uch, inbuf);          /* input buffer */
EXTERN(uch, outbuf);         /* output buffer */
EXTERN(ush, d_buf);          /* buffer for distances, see trees.c */
EXTERN(uch, window);         /* Sliding window and suffix table (unlzw) */
#define tab_suffix window
#ifndef MAXSEG_64K
#  define tab_prefix prev    /* hash link (see deflate.c) */
#  define head (prev+WSIZE)  /* hash head (see deflate.c) */
EXTERN(ush, tab_prefix);  /* prefix code (see unlzw.c) */
#else
#  define tab_prefix0 prev
#  define head tab_prefix1
EXTERN(ush, tab_prefix0); /* prefix for even codes */
EXTERN(ush, tab_prefix1); /* prefix for odd  codes */
#endif

extern unsigned insize; /* valid bytes in inbuf */
extern unsigned inptr;  /* index of next byte to be processed in inbuf */
extern unsigned outcnt; /* bytes in output buffer */

extern long bytes_in;   /* number of input bytes */
extern long bytes_out;  /* number of output bytes */
extern long header_bytes;/* number of bytes in gzip header */

#define isize bytes_in
/* for compatibility with old zip sources (to be cleaned) */

extern int  ifd;        /* input file descriptor */
extern int  ofd;        /* output file descriptor */
extern char ifname[];   /* input file name or "stdin" */
extern char ofname[];   /* output file name or "stdout" */
extern char *progname;  /* program name */

extern long time_stamp; /* original time stamp (modification time) */
extern long ifile_size; /* input file size, -1 for devices (debug only) */

typedef int file_t;     /* Do not use stdio */
#define NO_FILE  (-1)   /* in memory compression */


#define	PACK_MAGIC     "\037\036" /* Magic header for packed files */
#define	GZIP_MAGIC     "\037\213" /* Magic header for gzip files, 1F 8B */
#define	OLD_GZIP_MAGIC "\037\236" /* Magic header for gzip 0.5 = freeze 1.x */
#define	LZH_MAGIC      "\037\240" /* Magic header for SCO LZH Compress files*/
#define PKZIP_MAGIC    "\120\113\003\004" /* Magic header for pkzip files */

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define CONTINUATION 0x02 /* bit 1 set: continuation of multi-part gzip file */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define ENCRYPTED    0x20 /* bit 5 set: file is encrypted */
#define RESERVED     0xC0 /* bit 6,7:   reserved */

/* internal file attribute */
#define UNKNOWN 0xffff
#define BINARY  0
#define ASCII   1

#ifndef WSIZE
#  define WSIZE 0x8000     /* window size--must be a power of two, and */
#endif                     /*  at least 32K for zip's deflate method */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
* See deflate.c for comments about the MIN_MATCH+1.
*/

#define MAX_DIST  (WSIZE-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
* distances are limited to MAX_DIST instead of WSIZE.
*/

extern int decrypt;        /* flag to turn on decryption */
extern int exit_code;      /* program exit code */
extern int verbose;        /* be verbose (-v) */
extern int quiet;          /* be quiet (-q) */
extern int level;          /* compression level */
extern int test;           /* check .z file integrity */
extern int to_stdout;      /* output to stdout (-c) */
extern int save_orig_name; /* set if original name must be saved */

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf(0))
#define try_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf(1))

						   /* put_byte is used for the compressed output, put_ubyte for the
						   * uncompressed output. However unlzw() uses window for its
						   * suffix table instead of its output buffer, so it does not use put_ubyte
						   * (to be cleaned up).
*/
#define put_byte(c) {outbuf[outcnt++]=(uch)(c); if (outcnt==OUTBUFSIZ)\
flush_outbuf();}
#define put_ubyte(c) {window[outcnt++]=(uch)(c); if (outcnt==WSIZE)\
flush_window();}

/* Output a 16 bit value, lsb first */
#define put_short(w) \
{ if (outcnt < OUTBUFSIZ-2) { \
    outbuf[outcnt++] = (uch) ((w) & 0xff); \
    outbuf[outcnt++] = (uch) ((ush)(w) >> 8); \
} else { \
    put_byte((uch)((w) & 0xff)); \
    put_byte((uch)((ush)(w) >> 8)); \
} \
}

/* Output a 32 bit value to the bit stream, lsb first */
#define put_long(n) { \
    put_short((n) & 0xffff); \
    put_short(((ulg)(n)) >> 16); \
}

#define seekable()    0  /* force sequential output */
#define translate_eol 0  /* no option -a yet */

#define tolow(c)  (isupper(c) ? (c)-'A'+'a' : (c))    /* force to lower case */

/* Macros for getting two-byte and four-byte header values */
#define GZIP_SH(p) ((ush)(uch)((p)[0]) | ((ush)(uch)((p)[1]) << 8))
#define LG(p) ((ulg)(GZIP_SH(p)) | ((ulg)(GZIP_SH((p)+2)) << 16))

/* Diagnostic functions */
#ifdef DEBUG
#  define Assert(cond,msg) {if(!(cond)) error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

#define WARN(msg) {if (!quiet) fprintf msg ; \
if (exit_code == OK) exit_code = WARNING;}

/* in zip.c: */
extern int zip        OF((int in, int out));
extern int file_read  OF((char *buf,  unsigned size));

/* in unzip.c */
extern int unzip      OF((int in, int out));
extern int check_zipfile OF((int in));

/* in unpack.c */
extern int unpack     OF((int in, int out));

/* in unlzh.c */
extern int unlzh      OF((int in, int out));

/* in gzip.c */
RETSIGTYPE abort_gzip OF((void));

/* in deflate.c */
void lm_init OF((int pack_level, ush *flags));
ulg  deflate OF((void));

/* in trees.c */
void ct_init     OF((ush *attr, int *method));
int  ct_tally    OF((int dist, int lc));
ulg  flush_block OF((char *buf, ulg stored_len, int eof));

/* in bits.c */
local void     bi_init    OF((file_t zipfile));
local void     send_bits  OF((int value, int length));
local unsigned bi_reverse OF((unsigned value, int length));
local void     bi_windup  OF((void));
local void     copy_block OF((char *buf, unsigned len, int header));
extern   int (*read_buf) OF((char *buf, unsigned size));

/* in util.c: */
extern int gzip_copy           OF((int in, int out));
extern ulg  updcrc        OF((uch *s, unsigned n));
extern void clear_bufs    OF((void));
extern int  fill_inbuf    OF((int eof_ok));
extern void flush_outbuf  OF((void));
extern void flush_window  OF((void));
extern void write_buf     OF((int fd, voidp buf, unsigned cnt));
extern char *gzipstrlwr   OF((char *s));
extern char *basename     OF((char *fname));
extern void make_simple_name OF((char *name));
extern char *add_envopt   OF((int *argcp, char ***argvp, char *env));
extern void error         OF((char *m));
extern void warn          OF((char *a, char *b));
extern void read_error    OF((void));
extern void write_error   OF((void));
extern void display_ratio OF((long num, long den, FILE *file));
extern voidp cpss_xmalloc      OF((unsigned int size));

/* in inflate.c */
extern int inflate OF((void));




/* lzw.h -- define the lzw functions.
* Copyright (C) 1992-1993 Jean-loup Gailly.
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License, see the file COPYING.
*/
#ifndef BITS
#  define BITS 16
#endif
#define INIT_BITS 9              /* Initial number of bits per code */

#define	LZW_MAGIC  "\037\235"   /* Magic header for lzw files, 1F 9D */

#define BIT_MASK    0x1f /* Mask for 'number of compression bits' */
/* Mask 0x20 is reserved to mean a fourth header byte, and 0x40 is free.
* It's a pity that old uncompress does not check bit 0x20. That makes
* extension of the format actually undesirable because old compress
* would just crash on the new format instead of giving a meaningful
* error message. It does check the number of bits, but it's more
* helpful to say "unsupported format, get a new version" than
* "can only handle 16 bits".
*/

#define BLOCK_MODE  0x80
/* Block compression: if table is full and compression rate is dropping,
* clear the dictionary.
*/

#define LZW_RESERVED 0x60 /* reserved bits */

#define	CLEAR  256       /* flush the dictionary */
#define FIRST  (CLEAR+1) /* first free entry */

extern int maxbits;      /* max bits per code for LZW */
extern int block_mode;   /* block compress mode -C compatible with 2.0 */

extern int lzw    OF((int in, int out));
extern int unlzw  OF((int in, int out));
/* tailor.h -- target dependent definitions
* Copyright (C) 1992-1993 Jean-loup Gailly.
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License, see the file COPYING.
*/

/* The target dependent definitions should be defined here only.
* The target dependent functions should be defined in tailor.c.
*/

/* $Id: tailor.h,v 0.18 1993/06/14 19:32:20 jloup Exp $ */

#if defined(__MSDOS__) && !defined(MSDOS)
#  define MSDOS
#endif

#if defined(__OS2__) && !defined(OS2)
#  define OS2
#endif

#if defined(OS2) && defined(MSDOS) /* MS C under OS/2 */
#  undef MSDOS
#endif

#ifdef MSDOS
#  ifdef __GNUC__
/* DJGPP version 1.09+ on MS-DOS.
* The DJGPP 1.09 stat() function must be upgraded before gzip will
* fully work.
* No need for DIRENT, since <unistd.h> defines POSIX_SOURCE which
* implies DIRENT.
*/
#    define near
#  else
#    define MAXSEG_64K
#    ifdef __TURBOC__
#      define NO_OFF_T
#      ifdef __BORLANDC__
#        define DIRENT
#      else
#        define NO_UTIME
#      endif
#    else /* MSC */
#      define HAVE_SYS_UTIME_H
#      define NO_UTIME_H
#    endif
#  endif
#  define PATH_SEP2 '\\'
#  define PATH_SEP3 ':'
#  define MAX_PATH_LEN  128
#  define NO_MULTIPLE_DOTS
#  define MAX_EXT_CHARS 3
#  define Z_SUFFIX "z"
#  define NO_CHOWN
#  define PROTO
#  define STDC_HEADERS
#  define NO_SIZE_CHECK
#  define casemap(c) tolow(c) /* Force file names to lower case */
#  include <io.h>
#  define OS_CODE  0x00
#  define SET_BINARY_MODE(fd) setmode(fd, O_BINARY)
#  if !defined(NO_ASM) && !defined(ASMV)
#    define ASMV
#  endif
#else
#  define near
#endif

#ifdef OS2
#  define PATH_SEP2 '\\'
#  define PATH_SEP3 ':'
#  define MAX_PATH_LEN  260
#  ifdef OS2FAT
#    define NO_MULTIPLE_DOTS
#    define MAX_EXT_CHARS 3
#    define Z_SUFFIX "z"
#    define casemap(c) tolow(c)
#  endif
#  define NO_CHOWN
#  define PROTO
#  define STDC_HEADERS
#  include <io.h>
#  define OS_CODE  0x06
#  define SET_BINARY_MODE(fd) setmode(fd, O_BINARY)
#  ifdef _MSC_VER
#    define HAVE_SYS_UTIME_H
#    define NO_UTIME_H
#    define MAXSEG_64K
#    undef near
#    define near _near
#  endif
#  ifdef __EMX__
#    define HAVE_SYS_UTIME_H
#    define NO_UTIME_H
#    define DIRENT
#    define EXPAND(argc,argv) \
{_response(&argc, &argv); _wildcard(&argc, &argv);}
#  endif
#  ifdef __BORLANDC__
#    define DIRENT
#  endif
#  ifdef __ZTC__
#    define NO_DIR
#    define NO_UTIME_H
#    include <dos.h>
#    define EXPAND(argc,argv) \
       {response_expand(&argc, &argv);}
#  endif
#endif
#ifndef _lint
#ifdef WIN32 /* Windows NT */
#  define HAVE_SYS_UTIME_H
#  define NO_UTIME_H
#  define PATH_SEP2 '\\'
#  define PATH_SEP3 ':'
#  define NO_CHOWN
#  define PROTO
#  define STDC_HEADERS
#  define SET_BINARY_MODE(fd) setmode(fd, O_BINARY)
#  include <io.h>
#  include <malloc.h>
#  ifdef NTFAT
#    define NO_MULTIPLE_DOTS
#    define MAX_EXT_CHARS 3
#    define Z_SUFFIX "z"
#    define casemap(c) tolow(c) /* Force file names to lower case */
#  endif
#  define OS_CODE  0x0b
#endif
#endif /*_lint*/
#ifdef MSDOS
#  ifdef __TURBOC__
#    include <alloc.h>
#    define DYN_ALLOC
/* Turbo C 2.0 does not accept static allocations of large arrays */
void * fcalloc (unsigned items, unsigned size);
void fcfree (void *ptr);
#  else /* MSC */
#    include <malloc.h>
#    define fcalloc(nitems,itemsize) halloc((long)(nitems),(itemsize))
#    define fcfree(ptr) hfree(ptr)
#  endif
#else
#  ifdef MAXSEG_64K
#    define fcalloc(items,size) calloc((items),(size))
#  else
#    define fcalloc(items,size) malloc((size_t)(items)*(size_t)(size))
#  endif
#  define fcfree(ptr) free(ptr)
#endif

#if defined(VAXC) || defined(VMS)
#  define PATH_SEP ']'
#  define PATH_SEP2 ':'
#  define SUFFIX_SEP ';'
#  define NO_MULTIPLE_DOTS
#  define Z_SUFFIX "-gz"
#  define RECORD_IO 1
#  define casemap(c) tolow(c)
#  define OS_CODE  0x02
#  define OPTIONS_VAR "GZIP_OPT"
#  define STDC_HEADERS
#  define NO_UTIME
#  define EXPAND(argc,argv) vms_expand_args(&argc,&argv);
#  include <file.h>
#  define unlink delete
#  ifdef VAXC
#    define NO_FCNTL_H
#    include <unixio.h>
#  endif
#endif

#ifdef AMIGA
#  define PATH_SEP2 ':'
#  define STDC_HEADERS
#  define OS_CODE  0x01
#  define ASMV
#  ifdef __GNUC__
#    define DIRENT
#    define HAVE_UNISTD_H
#  else /* SASC */
#    define NO_STDIN_FSTAT
#    define SYSDIR
#    define NO_SYMLINK
#    define NO_CHOWN
#    define NO_FCNTL_H
#    include <fcntl.h> /* for read() and write() */
#    define direct dirent
extern void _expand_args(int *argc, char ***argv);
#    define EXPAND(argc,argv) _expand_args(&argc,&argv);
#    undef  O_BINARY /* disable useless --ascii option */
#  endif
#endif

#if defined(ATARI) || defined(atarist)
#  ifndef STDC_HEADERS
#    define STDC_HEADERS
#    define HAVE_UNISTD_H
#    define DIRENT
#  endif
#  define ASMV
#  define OS_CODE  0x05
#  ifdef TOSFS
#    define PATH_SEP2 '\\'
#    define PATH_SEP3 ':'
#    define MAX_PATH_LEN  128
#    define NO_MULTIPLE_DOTS
#    define MAX_EXT_CHARS 3
#    define Z_SUFFIX "z"
#    define NO_CHOWN
#    define casemap(c) tolow(c) /* Force file names to lower case */
#    define NO_SYMLINK
#  endif
#endif

#ifdef MACOS
#  define PATH_SEP ':'
#  define DYN_ALLOC
#  define PROTO
#  define NO_STDIN_FSTAT
#  define NO_CHOWN
#  define NO_UTIME
#  define chmod(file, mode) (0)
#  define OPEN(name, flags, mode) open(name, flags)
#  define OS_CODE  0x07
#  ifdef MPW
#    define isatty(fd) ((fd) <= 2)
#  endif
#endif

#ifdef __50SERIES /* Prime/PRIMOS */
#  define PATH_SEP '>'
#  define STDC_HEADERS
#  define NO_MEMORY_H
#  define NO_UTIME_H
#  define NO_UTIME
#  define NO_CHOWN 
#  define NO_STDIN_FSTAT 
#  define NO_SIZE_CHECK 
#  define NO_SYMLINK
#  define RECORD_IO  1
#  define casemap(c)  tolow(c) /* Force file names to lower case */
#  define put_char(c) put_byte((c) & 0x7F)
#  define get_char(c) ascii2pascii(get_byte())
#  define OS_CODE  0x0F    /* temporary, subject to change */
#  ifdef SIGTERM
#    undef SIGTERM         /* We don't want a signal handler for SIGTERM */
#  endif
#endif

#if defined(pyr) && !defined(NOMEMCPY) /* Pyramid */
#  define NOMEMCPY /* problem with overlapping copies */
#endif

#ifdef TOPS20
#  define OS_CODE  0x0a
#endif

#ifndef unix
#  define NO_ST_INO /* don't rely on inode numbers */
#endif


/* Common defaults */

#ifndef OS_CODE
#  define OS_CODE  0x03  /* assume Unix */
#endif

#ifndef PATH_SEP
#  define PATH_SEP '/'
#endif

#ifndef casemap
#  define casemap(c) (c)
#endif

#ifndef OPTIONS_VAR
#  define OPTIONS_VAR "GZIP"
#endif

#ifndef Z_SUFFIX
#  define Z_SUFFIX ".gz"
#endif

#ifdef MAX_EXT_CHARS
#  define MAX_SUFFIX  MAX_EXT_CHARS
#else
#  define MAX_SUFFIX  30
#endif

#ifndef MAKE_LEGAL_NAME
#  ifdef NO_MULTIPLE_DOTS
#    define MAKE_LEGAL_NAME(name)   make_simple_name(name)
#  else
#    define MAKE_LEGAL_NAME(name)
#  endif
#endif

#ifndef MIN_PART
#  define MIN_PART 3
/* keep at least MIN_PART chars between dots in a file name. */
#endif

#ifndef EXPAND
#  define EXPAND(argc,argv)
#endif

#ifndef RECORD_IO
#  define RECORD_IO 0
#endif

#ifndef SET_BINARY_MODE
#  define SET_BINARY_MODE(fd)
#endif

#ifndef OPEN
#  define OPEN(name, flags, mode) open(name, flags, mode)
#endif

#ifndef get_char
#  define get_char() get_byte()
#endif

#ifndef put_char
#  define put_char(c) put_byte(c)
#endif
#ifndef WIN32 
/*
* Macros for number representation conversion.
*/
#if _BYTE_ORDER==_BIG_ENDIAN
#define ntohl(x) (x)
#define ntohs(x) (x)
#define htonl(x) (x)
#define htons(x) (x)

#define NTOHL(x) (x) = ntohl((u_long)(x))
#define NTOHS(x) (x) = ntohs((u_short)(x))
#define HTONL(x) (x) = htonl((u_long)(x))
#define HTONS(x) (x) = htons((u_short)(x))


#endif /* _BYTE_ORDER==_BIG_ENDIAN */

#if _BYTE_ORDER==_LITTLE_ENDIAN

#if FALSE
UINT16 ntohs ();
UINT16 htons ();
UINT32 ntohl ();
UINT32 htonl ();
#endif /* FALSE */
#define ntohl(x) ((((x) & 0x000000ff) << 24) | \
	(((x) & 0x0000ff00) <<  8) | \
	(((x) & 0x00ff0000) >>  8) | \
(((x) & 0xff000000) >> 24))

#define htonl(x) ((((x) & 0x000000ff) << 24) | \
	(((x) & 0x0000ff00) <<  8) | \
	(((x) & 0x00ff0000) >>  8) | \
(((x) & 0xff000000) >> 24))

#define ntohs(x) ((((x) & 0x00ff) << 8) | \
(((x) & 0xff00) >> 8))

#define htons(x) ((((x) & 0x00ff) << 8) | \
(((x) & 0xff00) >> 8))
#define NTOHL(x) (x) = ntohl((u_long)(x))
#define NTOHS(x) (x) = ntohs((u_short)(x))
#define HTONL(x) (x) = htonl((u_long)(x))
#define HTONS(x) (x) = htons((u_short)(x))

#endif /* _BYTE_ORDER==_LITTLE_ENDIAN */
#endif/*ndef win32*/
#if defined(NB_WINNT) || defined(NB_VXWORKS)/*pc mode for super nb*/
#define NO_FCNTL_H
#include "osp_usr_ext.h"
#else
#include <sys/stat.h>
#endif
#ifdef WIN32									/* 操作系统标准头文件	      */
	#include <windows.h>                                            /* windows头文件                                    */
	#include <stddef.h>                                             /* 标准类型定义，引入errno宏                        */
	#include <Wininet.h>                                            /* wininet头文件                                    */
	/*#include "winsock2.h"*/
#else
/*	#include <netinet/in.h>*/
	#include <sys/socket.h>
/*	#include "nb_system_def.h" */
	#include <stddef.h>
#endif
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>

#ifdef WIN32
	#undef  errno  
#endif /*WIN32*/
#include <errno.h>
#ifdef NO_TIME_H
#  include <sys/time.h>
#else
#  include <time.h>
#endif

#ifndef NO_FCNTL_H
#  include <fcntl.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#if defined(STDC_HEADERS) || !defined(NO_STDLIB_H)
#  include <stdlib.h>
#else
extern s32 errno;
#endif

#if defined(DIRENT)
#  include <dirent.h>
typedef struct dirent dir_type;
#  define NLENGTH(dirent) ((s32)strlen((dirent)->d_name))
#  define DIR_OPT "DIRENT"
#else
#  define NLENGTH(dirent) ((dirent)->d_namlen)
#  ifdef SYSDIR
#    include <sys/dir.h>
typedef struct direct dir_type;
#    define DIR_OPT "SYSDIR"
#  else
#    ifdef SYSNDIR
#      include <sys/ndir.h>
typedef struct direct dir_type;
#      define DIR_OPT "SYSNDIR"
#    else
#      ifdef NDIR
#        include <ndir.h>
typedef struct direct dir_type;
#        define DIR_OPT "NDIR"
#      else
#        define NO_DIR
#        define DIR_OPT "NO_DIR"
#      endif
#    endif
#  endif
#endif

#ifndef NO_UTIME
#  ifndef NO_UTIME_H
#	ifndef WIN32
#    include <utime.h>
#	endif
#    define TIME_OPT "UTIME"
#  else
#    ifdef HAVE_SYS_UTIME_H
#      include <sys/utime.h>
#      define TIME_OPT "SYS_UTIME"
#    else
struct utimbuf {
	time_t actime;
	time_t modtime;
};
#      define TIME_OPT ""
#    endif
#  endif
#else
#  define TIME_OPT "NO_UTIME"
#endif

#if !defined(S_ISDIR) && defined(S_IFDIR)
#  define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG) && defined(S_IFREG)
#  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

#ifndef	O_BINARY
#  define  O_BINARY  0  /* creation mode for open() */
#endif

#ifndef O_CREAT
/* Pure BSD system? */
#  include <sys/file.h>
#  ifndef O_CREAT
#    define O_CREAT FCREAT
#  endif
#  ifndef O_EXCL
#    define O_EXCL FEXCL
#  endif
#endif

#ifndef S_IRUSR
#  define S_IRUSR 0400
#endif
#ifndef S_IWUSR
#  define S_IWUSR 0200
#endif
#define RW_USER (S_IRUSR | S_IWUSR)  /* creation mode for open() */

#ifndef MAX_PATH_LEN
#  define MAX_PATH_LEN   1024 /* max pathname length */
#endif

#ifndef SEEK_END
#  define SEEK_END 2
#endif


extern int gziplib_compressfile(int OperType,char * inputname);
