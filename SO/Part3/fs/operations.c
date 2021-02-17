#include "operations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Given a path, fills pointers with strings for the parent path and child
 * file name
 * Input:
 *  - path: the path to split. ATENTION: the function may alter this parameter
 *  - parent: reference to a char*, to store parent path
 *  - child: reference to a char*, to store child file name
 */
void split_parent_child_from_path(char * path, char ** parent, char ** child) {

	int n_slashes = 0, last_slash_location = 0;
	int len = strlen(path);

	// deal with trailing slash ( a/x vs a/x/ )
	if (path[len-1] == '/') {
		path[len-1] = '\0';
	}

	for (int i=0; i < len; ++i) {
		if (path[i] == '/' && path[i+1] != '\0') {
			last_slash_location = i;
			n_slashes++;
		}
	}

	if (n_slashes == 0) { // root directory
		*parent = "";
		*child = path;
		return;
	}

	path[last_slash_location] = '\0';
	*parent = path;
	*child = path + last_slash_location + 1;

}


/*
 * Initializes tecnicofs and creates root node.
 */
void init_fs() {
	inode_table_init();
	
	/* create root inode */
	int root = inode_create(T_DIRECTORY);
	
	if (root != FS_ROOT) {
		printf("failed to create node for tecnicofs root\n");
		exit(EXIT_FAILURE);
	}
}


/*
 * Destroy tecnicofs and inode table.
 */
void destroy_fs() {
	inode_table_destroy();
}


/*
 * Checks if content of directory is not empty.
 * Input:
 *  - entries: entries of directory
 * Returns: SUCCESS or FAIL
 */

int is_dir_empty(DirEntry *dirEntries) {
	if (dirEntries == NULL) {
		return FAIL;
	}
	for (int i = 0; i < MAX_DIR_ENTRIES; i++) {
		if (dirEntries[i].inumber != FREE_INODE) {
			return FAIL;
		}
	}
	return SUCCESS;
}


/*
 * Looks for node in directory entry from name.
 * Input:
 *  - name: path of node
 *  - entries: entries of directory
 * Returns:
 *  - inumber: found node's inumber
 *  - FAIL: if not found
 */
int lookup_sub_node(char *name, DirEntry *entries) {
	if (entries == NULL) {
		return FAIL;
	}
	for (int i = 0; i < MAX_DIR_ENTRIES; i++) {
        if (entries[i].inumber != FREE_INODE && strcmp(entries[i].name, name) == 0) {
            return entries[i].inumber;
        }
    }
	return FAIL;
}


/*
 * Creates a new node given a path.
 * Input:
 *  - name: path of node
 *  - nodeType: type of node
 * Returns: SUCCESS or FAIL
 */
int create(char *name, type nodeType){

	int parent_inumber, child_inumber;
	char *parent_name, *child_name, name_copy[MAX_FILE_NAME];
	/* use for copy */
	type pType;
	union Data pdata;
	int array[INODE_TABLE_SIZE] = {0}; /* array with locked nodes' numbers */
	int canContinue = 0;
	int tries = 0;

	strcpy(name_copy, name);
	split_parent_child_from_path(name_copy, &parent_name, &child_name);

	while(!canContinue) {
		if ((parent_inumber = lookup(parent_name, array)) != FAIL) {
			tries++;
			/* unlock read lock to write instead */
			if (pthread_rwlock_unlock(inode_getLock(parent_inumber)) != 0) {
				printf("error unlocking node %d\n", parent_inumber);
				exit(EXIT_FAILURE);
			}
			if (pthread_rwlock_trywrlock(inode_getLock(parent_inumber)) == 0) {
				canContinue = 1;
			}
			else {
				array[parent_inumber] = NOTLOCKED;
				unlock_locks(array);
				usleep(rand() % (MAX*tries+1)); // random microseconds between 0 and MAX*tries
			}
		}
		else {
			printf("failed to create %s, invalid parent dir %s\n", name, parent_name);
			unlock_locks(array);
			return FAIL;
		}
	}

	inode_get(parent_inumber, &pType, &pdata);

	if(pType != T_DIRECTORY) {
		printf("failed to create %s, parent %s is not a dir\n",
		        name, parent_name);
		unlock_locks(array);
		return FAIL;
	}

	if (lookup_sub_node(child_name, pdata.dirEntries) != FAIL) {
		printf("failed to create %s, already exists in dir %s\n",
		       child_name, parent_name);
		unlock_locks(array);
		return FAIL;
	}

	/* create node and add entry to folder that contains new node */
	child_inumber = inode_create(nodeType);

	if (child_inumber == FAIL) {
		printf("failed to create %s in  %s, couldn't allocate inode\n",
		        child_name, parent_name);
		unlock_locks(array);
		return FAIL;
	}

	if (pthread_rwlock_wrlock(inode_getLock(child_inumber)) != 0) {
		printf("error locking node %d\n", child_inumber);
		unlock_locks(array);
        exit(EXIT_FAILURE);
	}

	if (dir_add_entry(parent_inumber, child_inumber, child_name) == FAIL) {
		printf("could not add entry %s in dir %s\n",
		       child_name, parent_name);
		unlock_locks(array);
		pthread_rwlock_unlock(inode_getLock(child_inumber));
		return FAIL;
	}

	unlock_locks(array);
	if (pthread_rwlock_unlock(inode_getLock(child_inumber)) != 0) {
		printf("error unlocking node %d\n", child_inumber);
        exit(EXIT_FAILURE);
	}

	return SUCCESS;
}


/*
 * Deletes a node given a path.
 * Input:
 *  - name: path of node
 * Returns: SUCCESS or FAIL
 */
int delete(char *name){

	int parent_inumber, child_inumber;
	char *parent_name, *child_name, name_copy[MAX_FILE_NAME];
	/* use for copy */
	type pType, cType;
	union Data pdata, cdata;
	int array[INODE_TABLE_SIZE] = {0}; /* array with locked nodes' numbers */
	int canContinue = 0;
	int tries = 0;

	strcpy(name_copy, name);
	split_parent_child_from_path(name_copy, &parent_name, &child_name);

	parent_inumber = lookup(parent_name, array);

	if (parent_inumber == FAIL) {
		printf("failed to delete %s, invalid parent dir %s\n",
		        child_name, parent_name);
		unlock_locks(array);
		return FAIL;
	}

	while(!canContinue) {
		if ((parent_inumber = lookup(parent_name, array)) != FAIL) {
			tries++;
			/* unlock read lock to write instead */
			if (pthread_rwlock_unlock(inode_getLock(parent_inumber)) != 0) {
				printf("error unlocking node %d\n", parent_inumber);
				exit(EXIT_FAILURE);
			}
			if (pthread_rwlock_trywrlock(inode_getLock(parent_inumber)) == 0) {
				canContinue = 1;
			}
			else {
				array[parent_inumber] = NOTLOCKED;
				unlock_locks(array);
				usleep(rand() % (MAX*tries+1)); // random microseconds between 0 and MAX*tries
			}
		}
		else {
			printf("failed to delete %s, invalid parent dir %s\n", name, parent_name);
			unlock_locks(array);
			return FAIL;
		}
	}

	inode_get(parent_inumber, &pType, &pdata);

	if(pType != T_DIRECTORY) {
		printf("failed to delete %s, parent %s is not a dir\n",
		        child_name, parent_name);
		unlock_locks(array);
		return FAIL;
	}

	child_inumber = lookup_sub_node(child_name, pdata.dirEntries);

	if (child_inumber == FAIL) {
		printf("could not delete %s, does not exist in dir %s\n",
		       name, parent_name);
		unlock_locks(array);
		return FAIL;
	}

	if (pthread_rwlock_wrlock(inode_getLock(child_inumber)) != 0) {
		printf("error locking node %d\n", child_inumber);
        exit(EXIT_FAILURE);
	}

	inode_get(child_inumber, &cType, &cdata);

	if (cType == T_DIRECTORY && is_dir_empty(cdata.dirEntries) == FAIL) {
		printf("could not delete %s: is a directory and not empty\n",
		       name);
		pthread_rwlock_unlock(inode_getLock(child_inumber));
		unlock_locks(array);
		return FAIL;
	}

	/* remove entry from folder that contained deleted node */
	if (dir_reset_entry(parent_inumber, child_inumber) == FAIL) {
		printf("failed to delete %s from dir %s\n",
		       child_name, parent_name);
		pthread_rwlock_unlock(inode_getLock(child_inumber));
		unlock_locks(array);
		return FAIL;
	}

	if (inode_delete(child_inumber) == FAIL) {
		printf("could not delete inode number %d from dir %s\n",
		       child_inumber, parent_name);
		pthread_rwlock_unlock(inode_getLock(child_inumber));
		unlock_locks(array);
		return FAIL;
	}

	if (pthread_rwlock_unlock(inode_getLock(child_inumber)) != 0) {
		printf("error unlocking node %d\n", child_inumber);
        exit(EXIT_FAILURE);
	}
	unlock_locks(array);

	return SUCCESS;
}


/*
 * Lookup for a given path.
 * Input:
 *  - name: path of node
 *  - array: an array with all the nodes that were locked
 * Returns:
 *  inumber: identifier of the i-node, if found
 *     FAIL: otherwise
 */
int lookup(char *name, int *array) {
	char full_path[MAX_FILE_NAME];
	char delim[] = "/";
	char *saveptr;

	strcpy(full_path, name);

	/* start at root node */
	int current_inumber = FS_ROOT;

	if(array[0] == NOTLOCKED) {
		if (pthread_rwlock_rdlock(inode_getLock(0)) != 0) {
			printf("error locking root\n");
        	exit(EXIT_FAILURE);
		}
		array[0] = LOCKED;
	}

	/* use for copy */
	type nType;
	union Data data;

	/* get root inode data */
	inode_get(current_inumber, &nType, &data);

	char *path = strtok_r(full_path, delim, &saveptr); 

	/* search for all sub nodes */
	while (path != NULL && (current_inumber = lookup_sub_node(path, data.dirEntries)) != FAIL) {
		if(array[current_inumber] == NOTLOCKED) {
			if (pthread_rwlock_rdlock(inode_getLock(current_inumber)) != 0) {
				printf("error locking node %d\n", current_inumber);
				exit(EXIT_FAILURE);
			}
			array[current_inumber] = LOCKED;
		}
		inode_get(current_inumber, &nType, &data);
		path = strtok_r(NULL, delim, &saveptr);
	}
	return current_inumber;
}

/*
 * Lookup operation for the main program
 * Lookup for a file or directory,
 * without creating or deleting.
 */
int lookup_operation(char* name) {
    int array[INODE_TABLE_SIZE] = {0}; /* array with locked nodes' numbers */
    int res = lookup(name, array);
    unlock_locks(array);
    return res;
}

/*
 * Prints tecnicofs tree.
 * Input:
 *  - fp: pointer to output file
 */
int print_tecnicofs_tree(FILE *fp){
	/* lock root to write */
	if (pthread_rwlock_wrlock(inode_getLock(0)) != 0) {
		printf("error locking root\n");
		exit(EXIT_FAILURE);
	}

	inode_print_tree(fp, FS_ROOT, "");

	/* unlock root */
	if (pthread_rwlock_unlock(inode_getLock(0)) != 0) {
		printf("error unlocking root\n");
		exit(EXIT_FAILURE);
	}
	return SUCCESS;
}


/*
 * Unlocks every locked node in array,
 * starting by the root
 */ 
void unlock_locks(int *array) {
	int i;
	
	for(i=INODE_TABLE_SIZE-1; i>=0; i--) {
		if (array[i] == LOCKED) {
			if (pthread_rwlock_unlock(inode_getLock(i)) != 0) {
				printf("error unlocking node %d\n", i);
				exit(EXIT_FAILURE);
			}
			array[i] = NOTLOCKED;
		}
	}
}


/*
 * Moves file or directory.
 * Input:
 *  - name: path of node
 *  - nodeType: type of node
 * Returns: SUCCESS or FAIL
 */
int move(char *name1, char *name2) {

	int parent_inumber1, child_inumber1, parent_inumber2, child_inumber2;
	char *parent_name1, *child_name1, *parent_name2, *child_name2, name_copy[MAX_FILE_NAME];
	/* use for copy */
	type pType;
	union Data pdata;
	int array[INODE_TABLE_SIZE] = {0}; /* array with locked nodes */
	int canContinue = 0;
	int tries = 0;

	srand(time(NULL));

	/* verifications of file to be moved ------------------------------------------------*/

	strcpy(name_copy, name1);
	split_parent_child_from_path(name_copy, &parent_name1, &child_name1);

	while(!canContinue) {
		if ((parent_inumber1 = lookup(parent_name1, array)) != FAIL) {
			tries++;
			/* unlock read lock to write instead */
			if (pthread_rwlock_unlock(inode_getLock(parent_inumber1)) != 0) {
				printf("error unlocking node %d\n", parent_inumber1);
				unlock_locks(array);
				exit(EXIT_FAILURE);
			}
			if (pthread_rwlock_trywrlock(inode_getLock(parent_inumber1)) == 0) {
				canContinue = 1;
			}
			else {
				array[parent_inumber1] = NOTLOCKED;
				unlock_locks(array);
				usleep(rand() % (MAX*tries+1)); // random microseconds between 0 and MAX*tries
			}
		}
		else {
			printf("failed to move %s, invalid parent dir %s\n", name1, parent_name1);
			unlock_locks(array);
			return FAIL;
		}
	}
	
	inode_get(parent_inumber1, &pType, &pdata);

	if(pType != T_DIRECTORY) {
		printf("failed to move %s, parent %s is not a dir\n",
		        name1, parent_name1);
		unlock_locks(array);
		return FAIL;
	}

	child_inumber1 = lookup_sub_node(child_name1, pdata.dirEntries);

	if (child_inumber1 == FAIL) {
		printf("could not move %s, does not exist in dir %s\n",
		       child_name1, parent_name1);
		unlock_locks(array);
		return FAIL;
	}

	if (pthread_rwlock_wrlock(inode_getLock(child_inumber1)) != 0) {
		printf("error locking node %d\n", child_inumber1);
		unlock_locks(array);
        return FAIL;
	}

	/* verifications of path where the file is going to be moved ------------------------*/

	strcpy(name_copy, name2);
	split_parent_child_from_path(name_copy, &parent_name2, &child_name2);

	canContinue = 0;
	tries = 0;

	while(!canContinue) {
		if ((parent_inumber2 = lookup(parent_name2, array)) != FAIL) {
			tries++;
			/* unlock read lock to write instead */
			if (pthread_rwlock_unlock(inode_getLock(parent_inumber2)) != 0) {
				printf("error unlocking node %d\n", parent_inumber2);
				pthread_rwlock_unlock(inode_getLock(child_inumber1));
				unlock_locks(array);
				exit(EXIT_FAILURE);
			}
			if (pthread_rwlock_trywrlock(inode_getLock(parent_inumber2)) == 0) {
				canContinue = 1;
			}
			else {
				array[parent_inumber2] = NOTLOCKED;
				array[parent_inumber1] = NOTLOCKED; // prevent losing this write lock
				unlock_locks(array);
				array[parent_inumber1] = LOCKED;
				lookup(parent_name1, array); // recover read locks from first path
				usleep(rand() % (MAX*tries+1)); // random microseconds between 0 and MAX*tries
			}
		}
		else {
			printf("failed to move %s, invalid parent dir %s\n", name1, parent_name2);
			pthread_rwlock_unlock(inode_getLock(child_inumber1));
			unlock_locks(array);
			return FAIL;
		}
	}

	inode_get(parent_inumber2, &pType, &pdata);

	if(pType != T_DIRECTORY) {
		printf("failed to move to %s, parent %s is not a dir\n",
		        name2, parent_name2);
		pthread_rwlock_unlock(inode_getLock(child_inumber1));
		unlock_locks(array);
		return FAIL;
	}

	child_inumber2 = lookup_sub_node(child_name2, pdata.dirEntries);

	/* check if there's no entry with the new pathname */
	if (child_inumber2 != FAIL) {
		printf("failed to move to %s, %s already exists in dir %s\n",
		       name2, child_name2, parent_name2);
		pthread_rwlock_unlock(inode_getLock(child_inumber1));
		unlock_locks(array);
		return FAIL;
	}

	/* end of verifications ---------------------------------------------------------*/
	
	/* add entry on new path */
	if (dir_add_entry(parent_inumber2, child_inumber1, child_name2) == FAIL) {
		printf("could not add entry %s in dir %s\n",
		       child_name2, parent_name2);
		pthread_rwlock_unlock(inode_getLock(child_inumber1));
		unlock_locks(array);
		return FAIL;
	}

	/* remove entry from its previous location */
	if (dir_reset_entry(parent_inumber1, child_inumber1) == FAIL) {
		printf("failed to delete %s from dir %s\n",
		       child_name1, parent_name1);
		pthread_rwlock_unlock(inode_getLock(child_inumber1));
		unlock_locks(array);
		return FAIL;
	}
	
	/* unlock all locks */
	if(pthread_rwlock_unlock(inode_getLock(child_inumber1)) != 0) {
		printf("failed to unlock node %d", child_inumber1);
		return FAIL;
	}
	unlock_locks(array);

	return SUCCESS;
}