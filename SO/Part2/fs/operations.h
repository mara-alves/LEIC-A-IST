#ifndef FS_H
#define FS_H
#include "state.h"

void init_fs();
void destroy_fs();
int is_dir_empty(DirEntry *dirEntries);
int create(char *name, type nodeType);
int delete(char *name);
int lookup(char *name, int *array);
int move(char *name1, char *name2);
int lookup_operation(char* name);
void print_tecnicofs_tree(FILE *fp);
void unlock_locks(int *array);

#endif /* FS_H */
