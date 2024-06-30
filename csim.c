#include <stdio.h>
#include "cachelab.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
typedef struct per_block
{
    bool isVaild;
    int tag; 
    int timestamp;
} per_block;

typedef per_block* set;

typedef struct cache
{
    int lines;
    int nsets;
    int blocks; 
    int bsets;
    set* head;
} cache;

typedef struct operation
{
    char op; 
    int size; 
    unsigned long address;
} operation;

static inline unsigned getSet(unsigned long address, int sets, int blocks)
{
    return (address >> blocks) % (1 << sets);
}

static inline unsigned getTag(unsigned long address, int sets, int blocks)
{
    return address >> (sets + blocks);
}

cache allocateset(int sets, int b, int e)
{
    int nsets = 1 << sets;
    set* head = calloc(nsets, sizeof(set));
    for (int i = 0; i < nsets; ++i)
        head[i] = calloc(e, sizeof(per_block));

    cache c = { e, nsets, b, sets, head };
    return c;
}

void freeCache(cache* c)
{
    for (int i = 0; i < c->nsets; ++i)
    {
        free(c->head[i]);
        c->head[i] = NULL;
    }
    free(c->head);
    c->head = NULL;
}

operation fetchoperation(FILE* fp)
{
    char temp[128];
    operation oper = { 0, 0, 0 };
    if(!fgets(temp, 128 , fp))
        return oper;
    sscanf(temp, "\n%c %lx,%d", &oper.op, &oper.address, &oper.size);
    return oper;
}

per_block* find_match_block(set s, int e, int tag)
{
    for (int i = 0; i < e; ++i)
        if (s[i].tag == tag && s[i].isVaild)
            return s + i;
    return NULL;
}

per_block* find_empty_block(set s, int e)
{
    for (int i = 0; i < e; ++i)
        if (!s[i].isVaild)
            return s + i;
    return NULL;
}

per_block* find_recent_block(set s, int e)
{
    per_block* least_recent = s;
    for (int i = 1; i < e; ++i)
        if (s[i].timestamp < least_recent->timestamp)
            least_recent = s + i;
    return least_recent;
}
int hit_count = 0;
int miss_count = 0;
int eviction_count = 0;
long long timestamp = 0;
void loadorstore(cache* c,operation* op,bool isdis,bool bymain)
{
    int s = getSet(op->address, c->bsets, c->blocks);
    int t = getTag(op->address, c->bsets, c->blocks);

    if (isdis&&bymain)
        printf("%c %lx,%d ", op->op, op->address, op->size);

    per_block* appropriate_tag = find_match_block(c->head[s], c->lines, t);
    if (appropriate_tag)
    {
        ++hit_count;

        if (isdis&&bymain)
            printf("hit ");
    }
    else
    {
        ++miss_count;
        if (isdis&&bymain)
            printf("miss ");

        appropriate_tag = find_empty_block(c->head[s], c->lines);
        if (appropriate_tag)
            appropriate_tag->isVaild = true;
        else
        {
            ++eviction_count;
            appropriate_tag = find_recent_block(c->head[s], c->lines);
            if (isdis&&bymain)
                printf("eviction ");
        }

        appropriate_tag->tag = t;
    }

    appropriate_tag->timestamp = timestamp;
    timestamp++;

    if (isdis&&bymain)
        printf("\n");
}

void modify(cache* c, operation* op, bool isdis)
{
    if (isdis)
        printf("%c %lx,%d ", op->op, op->address, op->size);
    op->op = 'L';
    loadorstore(c, op, isdis, false);
    op->op = 'S';
    loadorstore(c, op, isdis, false);
    if (isdis)
        printf("\n");
}

void get_arguments(int argc, char** argvs, int* s, int* e, int* b, char** path, bool* isDisplay)
{
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argvs[i], "-s") == 0)
            *s = atoi(argvs[i + 1]);
        else if (strcmp(argvs[i], "-E") == 0)
            *e = atoi(argvs[i + 1]);
        else if (strcmp(argvs[i], "-b") == 0)
            *b = atoi(argvs[i + 1]);
        else if (strcmp(argvs[i], "-t") == 0)
            *path = argvs[i + 1];
        else if (strcmp(argvs[i], "-v") == 0)
            *isDisplay = true;
    }
}
int main(int argc, char** argvs)
{
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argvs[i], "-h") == 0)
        {
            return 0;
        }
    }

    char* path = NULL;
    bool isDisplay = false;

    int s = 0, e = 0, b = 0;
    get_arguments(argc, argvs, &s, &e, &b, &path, &isDisplay);

    cache sh = allocateset(s, b, e);
    FILE* pfile = fopen(path, "r");
    setvbuf(pfile, NULL, _IOFBF, 1024);
    operation o;
    while ((o = fetchoperation(pfile)).op)
    {
        if (o.op == 'M')
            modify(&sh, &o, isDisplay);
        else if (o.op == 'L' || o.op == 'S')
            loadorstore(&sh, &o, isDisplay, true);
        else if (o.op == 'I')
            continue;
    }

    fclose(pfile);
    freeCache(&sh);
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
