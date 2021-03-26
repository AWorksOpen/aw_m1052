/* This file is generated automatically by configure */
/* It is valid only for the system type x86_64-pc-linux-gnu */

#ifndef __BYTEORDER_H
#define __BYTEORDER_H
#include "libmtp.h"

#define HTOBE16(x) (x) = htobe16(x)
#define HTOBE32(x) (x) = htobe32(x)
#define BE32TOH(x) (x) = be32toh(x)
#define BE16TOH(x) (x) = be16toh(x)

#ifndef htobe16
# define htobe16(x) htons(x)
#endif
#ifndef htobe32
# define htobe32(x) htonl(x)
#endif
#ifndef be16toh
# define be16toh(x) ntohs(x)
#endif
#ifndef be32toh
# define be32toh(x) ntohl(x)
#endif

/* On little endian machines, these macros are null */
#ifndef htole16
# define htole16(x)      (x)
#endif
#ifndef htole32
# define htole32(x)      (x)
#endif
#ifndef htole64
# define htole64(x)      (x)
#endif
#ifndef le16toh
# define le16toh(x)      (x)
#endif
#ifndef le32toh
# define le32toh(x)      (x)
#endif
#ifndef le64toh
# define le64toh(x)      (x)
#endif

#define be16atoh(x)     ((u16)(((x)[0]<<8)|(x)[1]))
#define be32atoh(x)     ((u32)(((x)[0]<<24)|((x)[1]<<16)|((x)[2]<<8)|(x)[3]))
#define be64atoh_x(x,off,shift)     (((u64)((x)[off]))<<shift)
#define be64atoh(x)     ((u64)(be64atoh_x(x,0,56)|be64atoh_x(x,1,48)|be64atoh_x(x,2,40)| \
        be64atoh_x(x,3,32)|be64atoh_x(x,4,24)|be64atoh_x(x,5,16)|be64atoh_x(x,6,8)|((x)[7])))
#define le16atoh(x)     ((u16)(((x)[1]<<8)|(x)[0]))
#define le32atoh(x)     ((u32)(((x)[3]<<24)|((x)[2]<<16)|((x)[1]<<8)|(x)[0]))
#define le64atoh_x(x,off,shift) (((u64)(x)[off])<<shift)
#define le64atoh(x)     ((u64)(le64atoh_x(x,7,56)|le64atoh_x(x,6,48)|le64atoh_x(x,5,40)| \
        le64atoh_x(x,4,32)|le64atoh_x(x,3,24)|le64atoh_x(x,2,16)|le64atoh_x(x,1,8)|((x)[0])))

#define htobe16a(a,x)   (a)[0]=(u8)((x)>>8), (a)[1]=(u8)(x)
#define htobe32a(a,x)   (a)[0]=(u8)((x)>>24), (a)[1]=(u8)((x)>>16), \
        (a)[2]=(u8)((x)>>8), (a)[3]=(u8)(x)
#define htobe64a(a,x)   (a)[0]=(u8)((x)>>56), (a)[1]=(u8)((x)>>48), \
        (a)[2]=(u8)((x)>>40), (a)[3]=(u8)((x)>>32), \
        (a)[4]=(u8)((x)>>24), (a)[5]=(u8)((x)>>16), \
        (a)[6]=(u8)((x)>>8), (a)[7]=(u8)(x)
#define htole16a(a,x)   (a)[1]=(u8)((x)>>8), (a)[0]=(u8)(x)
#define htole32a(a,x)   (a)[3]=(u8)((x)>>24), (a)[2]=(u8)((x)>>16), \
        (a)[1]=(u8)((x)>>8), (a)[0]=(u8)(x)
#define htole64a(a,x)   (a)[7]=(u8)((x)>>56), (a)[6]=(u8)((x)>>48), \
        (a)[5]=(u8)((x)>>40), (a)[4]=(u8)((x)>>32), \
        (a)[3]=(u8)((x)>>24), (a)[2]=(u8)((x)>>16), \
        (a)[1]=(u8)((x)>>8), (a)[0]=(u8)(x)
#endif
