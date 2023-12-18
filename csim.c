#include <bits/types/FILE.h>
#include <getopt.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cachelab.h"

typedef struct Cache_info {
    size_t s;
    size_t S;
    size_t E;
    size_t b;
} Cache_info;

typedef struct Cache_line {
    bool   valid;
    size_t label;
    size_t LRU_label;
} Cache_line;

typedef struct Cache_group {
    Cache_line *cache_lines;
} Cache_group;

typedef struct Cache {
    Cache_group *cache_group;
} Cache;

typedef struct Result {
    size_t hit;
    size_t miss;
    size_t evict;
} Result;

// 根据访问的地址，判断是哪个组（s)，再遍历组中的行(E)，通过标记位(t)与有效位判断是否命中

#define MAX_LINE_LENGTH 100
#define FILE_PATH_BUFFER 100
#define GET_GROUP(address, b, s) ((address >> b) & ((1 << s) - 1))
#define GET_LABEL(address, b, s) ((address >> (b + s)))

void init_cache(Cache *cache, Cache_info cache_info) {
    cache->cache_group =
        (Cache_group *) malloc(sizeof(Cache_group) * cache_info.S);
    for (int i = 0; i < cache_info.S; i++) {
        cache->cache_group[i].cache_lines =
            (Cache_line *) malloc(sizeof(Cache_line) * cache_info.E);
        for (int j = 0; j < cache_info.E; j++) {
            cache->cache_group[i].cache_lines[j].valid     = false;
            cache->cache_group[i].cache_lines[j].label     = 0;
            cache->cache_group[i].cache_lines[j].LRU_label = 0;
        }
    }
}

void update_LRU(Cache_group *cache_group, size_t line_num) {
    for (int i = 0; i < line_num; i++) {
        if (cache_group->cache_lines[i].valid) {
            cache_group->cache_lines[i].LRU_label++;
        }
    }
}

void write_line(Cache_line *cache_line, size_t label) {
    cache_line->valid     = true;
    cache_line->label     = label;
    cache_line->LRU_label = 0;
#ifndef NDEBUG
    printf("        Line be writen: label %ld, LRU %ld\n", cache_line->label,
           cache_line->LRU_label);
#endif
}

void print_cache_status(Cache cache, Cache_info cache_info) {
    for (int i = 0; i < cache_info.E; i++) {
        printf("Group %d:\n", i);
        Cache_group cg = cache.cache_group[i];
        for (int j = 0; j < cache_info.E; j++) {
            Cache_line cl = cg.cache_lines[j];
            printf("    Line %d, valid: %d, label: %ld, LRU: %ld\n", j,
                   cl.valid, cl.label, cl.LRU_label);
        }
    }
}

// return value: 1. hit 0. miss 2. miss & evict
int find_line(Cache *cache, Cache_info *cache_info, size_t address) {
    size_t group = GET_GROUP(address, cache_info->b, cache_info->s);
    size_t label = GET_LABEL(address, cache_info->b, cache_info->s);
#ifndef NDEBUG
    printf("Address: %lx, group: %ld, label: %ld\n", address, group, label);
#endif
    // is hit?
    Cache_group *selected_group = &cache->cache_group[group];
    for (int i = 0; i < cache_info->E; i++) {
        Cache_line *line = &selected_group->cache_lines[i];
        if (line->valid && (line->label == label)) {
            update_LRU(selected_group, cache_info->E);
            line->LRU_label = 0;
            return 1;
        }
    }

    update_LRU(selected_group, cache_info->E);

    // find a empty line or LRU line in group and evict
    size_t select_evict_line = -1;
    int    max_value         = -1;

    for (int i = 0; i < cache_info->E; i++) {
        Cache_line *line = &selected_group->cache_lines[i];
        if (!line->valid) {
            write_line(line, label);
            return 0;
        }
        if (line->valid && ((int) line->LRU_label) > max_value) {
            select_evict_line = i;
            max_value         = line->LRU_label;
        }
    }
    write_line(&(selected_group->cache_lines[select_evict_line]), label);
    return 2;
}

void process_operator(char operator, size_t address, size_t length,
                      Result *result, Cache *cache, Cache_info *cache_info) {
    int ret = find_line(cache, cache_info, address);
#ifndef NDEBUG
    printf("Ret: %d\n", ret);
    print_cache_status(*cache, *cache_info);
#endif
    switch (operator) {
    case 'L':
    case 'S':
        if (ret == 1) {
            result->hit++;
        } else if (ret == 0) {
            result->miss++;
        } else {
            result->miss++;
            result->evict++;
        }
        break;
    case 'M':
        if (ret == 1) {
            result->hit += 2;
        } else if (ret == 0) {
            result->miss++;
            result->hit++;
        } else {
            result->miss++;
            result->evict++;
            result->hit++;
        }
        break;
    }
}

int main(int argc, char *argv[]) {
    const char *optstr = "hvs:E:b:t:";
    int         opt;
    Cache_info  cache_info;
    Cache       cache;
    FILE       *file;
    char        line[MAX_LINE_LENGTH];
    // file path buffer
    char   file_path[FILE_PATH_BUFFER];
    Result result = {0, 0, 0};

    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
        case 's':
            cache_info.s = atoi(optarg);
            cache_info.S = (size_t) pow(2, cache_info.s);
            break;

        case 'b':
            cache_info.b = atoi(optarg);
            break;

        case 'E':
            cache_info.E = atoi(optarg);
            break;
        case 't':
            strcpy(file_path, optarg);
            break;
            // TODO: other case
        }
    }

    // printf("%x\n", GET_GROUP(0xff2d32, 8, 8));
    init_cache(&cache, cache_info);

    // read data
    file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Fail to open the file\n");
        exit(-1);
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char   type;
        size_t address, length;

        // If I, just skip it.
        if (line[0] == 'I') {
            continue;
        }

        if (sscanf(line, " %c %lx,%ld", &type, &address, &length) == 3) {
            // printf("Type: %c, Num1: %lx, Num2: %ld\n", type, num1, num2);
            process_operator(type, address, length, &result, &cache,
                             &cache_info);
        } else {
            fprintf(stderr, "Error parsing line: %s", line);
        }
    }
    fclose(file);

    printSummary(result.hit, result.miss, result.evict);
    return 0;
}
