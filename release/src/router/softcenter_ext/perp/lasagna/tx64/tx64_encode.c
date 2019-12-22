/* tx64_encode.c
** base64 encoding utility
** wcm, 2009.07.19 - 2009.07.20
** ===
*/
#include <stddef.h>
#include <stdint.h>

#include "tx64.h"

size_t
tx64_encode(char *dest, const char *src, size_t len, const char *vec, int flags)
{
    uint8_t   bits = 0;
    uint32_t  temp = 0;
    size_t    w = 0;
    size_t    i;

    /* if dest is NULL, report size required to encode src: */
    if(dest == NULL){
        size_t  need;
        if(flags & TX64_NOPAD)
            need = ((len / 3) * 4) + ((((len % 3) * 4) + 2) / 3);
        else
            need = ((len + 2) / 3) * 4;

        return need;
    }

    for(i = 0; i < len; ++i){
        temp <<= 8;
        temp += (unsigned char)src[i];
        bits += 8;
        while(bits > 6){
            dest[w] = vec[ ((temp >> (bits - 6)) & 63) ];
            ++w;
            bits -= 6;
        }
    }

    if(bits){
        temp <<= (6 - bits);
        dest[w] = vec[ (temp & 63) ];
        ++w;
    }

    if(flags & TX64_NOPAD){
        return w;
    }

    /* else: pad incomplete src triplet to 4 chars: */
    while(w % 4){
        dest[w] = vec[64];
        ++w;
    }    
        
    return w;
}

/* eof: tx64_encode.c */
