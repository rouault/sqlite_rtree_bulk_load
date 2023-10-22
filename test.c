#define SQLITE_RTREE_BL_SYMBOL(x) test_##x

#include "sqlite_rtree_bulk_load.c"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
#define STATIC_CAST(type, value) static_cast<type>(value)
#else
#define STATIC_CAST(type, value) (type)(value)
#endif

int main(int argc, char** argv) {
    if( argc != 4 ) {
        fprintf(stderr, "Usage: test source.db out.db rtree_name\n");
        return 1;
    }
    const char* sourceDb = argv[1];
    const char* targetDb = argv[2];
    const char* rtreeName = argv[3];
    sqlite3* hDBRef = NULL;
    if (sqlite3_open(sourceDb, &hDBRef) != SQLITE_OK || !hDBRef)
    {
        fprintf(stderr, "sqlite3_open(sourceDb) failed\n");
        return 1;
    }
    sqlite3_stmt *hStmt = NULL;
    char* sql = sqlite3_mprintf(
        "select id, minx, miny, maxx, maxy from \"%w\"",
        rtreeName);
    sqlite3_prepare_v2(hDBRef, sql, -1, &hStmt, NULL);
    sqlite3_free(sql);
    if (!hStmt)
    {
        fprintf(stderr, "sqlite3_prepare_v2() failed\n");
        return 1;
    }

    sqlite3* hDB = NULL;
    if (sqlite3_open(targetDb, &hDB) != SQLITE_OK || !hDBRef)
    {
        fprintf(stderr, "sqlite3_open(targetDb) failed\n");
        return 1;
    }

    char** papszResult = NULL;
    sqlite3_get_table(hDB, "PRAGMA page_size", &papszResult, NULL, NULL, NULL);
    const int page_size = atoi(papszResult[1]);
    sqlite3_free_table(papszResult);

    struct sqlite_rtree_bl* t = SQLITE_RTREE_BL_SYMBOL(sqlite_rtree_bl_new)(page_size);
    while (sqlite3_step(hStmt) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(hStmt, 0);
        const double minx = sqlite3_column_double(hStmt, 1);
        const double miny = sqlite3_column_double(hStmt, 2);
        const double maxx = sqlite3_column_double(hStmt, 3);
        const double maxy = sqlite3_column_double(hStmt, 4);
        if (!SQLITE_RTREE_BL_SYMBOL(sqlite_rtree_bl_insert)(t, id, minx, miny, maxx, maxy)) {
            fprintf(stderr, "sqlite_rtree_bl_insert failed\n");
            return 1;
        }
    }

    sqlite3_finalize(hStmt);
    sqlite3_close(hDBRef);

    printf("RTree RAM usage = %d MB\n",
           STATIC_CAST(int, SQLITE_RTREE_BL_SYMBOL(sqlite_rtree_bl_ram_usage)(t) / (1024 * 1024)));

    sqlite3_exec(hDB, "BEGIN", NULL, NULL, NULL);

    char* error_msg = NULL;
    if (!SQLITE_RTREE_BL_SYMBOL(sqlite_rtree_bl_serialize)(t, hDB,
                                rtreeName,
                                "id", "minx", "miny", "maxx", "maxy",
                                &error_msg)) {
        fprintf(stderr, "error: %s\n", error_msg);
        return 1;
    }
    sqlite3_free(error_msg);
    SQLITE_RTREE_BL_SYMBOL(sqlite_rtree_bl_free)(t);

    sqlite3_exec(hDB, "COMMIT", NULL, NULL, NULL);

    sqlite3_close(hDB);

    return 0;
}
