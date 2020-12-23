//
// Created by figurantpp on 23/12/2020.
//

#include "cmap.h"

#include <iostream>
#include <unordered_map>

struct CMapContainer
{
    std::unordered_map<const void*, const void*> *table;
    
    CMapContainer()
    {
        this->table = new std::unordered_map<const void*, const void*>;
    }

    ~CMapContainer()
    {
        delete table;
    }
};

CMap *cmap_new()
{
    try
    {
        return new CMapContainer();
    }
    catch (std::bad_alloc &error)
    {
        return nullptr;
    }
}

int cmap_set(CMap* table, const void *key, const void *value)
{
    auto container = (CMapContainer*) table;

    try
    {
        container->table->operator[](key) = value;
        return 0;
    }
    catch (std::bad_alloc &error)
    {
        return -1;
    }
}

const void *cmap_get(CMap* table, const void *key)
{
    auto container = (CMapContainer*) table;

    return container->table->operator[](key);
}


void cmap_delete(CMap *table)
{
    auto container = (CMapContainer*) table;

    delete container;
}

