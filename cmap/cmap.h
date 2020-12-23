//
// Created by figurantpp on 23/12/2020.
//

#ifndef BF_BTABLE_H
#define BF_BTABLE_H

#endif //BF_BTABLE_H

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

typedef void CMap;

EXTERN_C CMap *cmap_new();

EXTERN_C int cmap_set(CMap* table, const void *key, const void *value);
EXTERN_C const void* cmap_get(CMap* table, const void *key);

EXTERN_C void cmap_delete();

void cmap_delete(CMap *table);
