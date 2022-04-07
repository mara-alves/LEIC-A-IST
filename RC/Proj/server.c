#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>

#define PASSWORD_SIZE 8
#define MAX_GNAME 24
#define MAX_PATH 30
#define MAX_USERS 99999
#define MAX_GROUPS 99

typedef struct {
	char group_no[MAX_GROUPS][3];
	char group_name[MAX_GROUPS][MAX_GNAME+1];
	char group_mid[MAX_GROUPS][5];
	char group_users[MAX_GROUPS][MAX_USERS][6];
	int no_groups;
	int no_users[MAX_GROUPS];
} GROUPLIST;

struct addrinfo hints_udp, hints_tcp, *res_udp, *res_tcp;
int fd_udp, fd_tcp, errcode, maxfdp1, connfd;
struct sockaddr_in addr;
ssize_t n_udp, n_tcp;
socklen_t addrlen;
fd_set rset;
GROUPLIST *glist;
bool verbose = false; // default


/* Auxiliary functions *********************************************/

/* -------------------------------------------------
    returns number of digits of a number
------------------------------------------------- */
int get_digits(int num) {
    if (num < 10) return 1;
    if (num < 100) return 2;
    if (num < 1000) return 3;
    if (num < 10000) return 4;
    if (num < 100000) return 5;
    if (num < 1000000) return 6;
    if (num < 10000000) return 7;
    if (num < 100000000) return 8;
    if (num < 1000000000) return 9;
	return 10;
}

// returns the max int
int max(int x, int y){
    if (x > y)
        return x;
    else
        return y;
}

// swaps two strings
void swap_strings(char* a, char* b){
    char temp[MAX_GNAME+1];
	strcpy(temp, a);
	strcpy(a, b);
	strcpy(b, temp);
}

// swaps two arrays of strings
void swap_stringarray(char a[MAX_USERS][6], char b[MAX_USERS][6]){
	char temp[MAX_USERS][6];
	memcpy(temp, a, MAX_USERS*6);
	memcpy(a, b, MAX_USERS*6);
	memcpy(b, temp, MAX_USERS*6);
}

// swaps two ints
void swap_ints(int *a, int *b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

// get number of messages in group GID
int CountMsg(char *GID) {
	char s[MAX_PATH];
	DIR *d;
	struct dirent *dir;
	sprintf(s, "GROUPS/%s/MSG", GID);
	d = opendir(s);
	int count=0;
	if (d){
		while((dir=readdir(d))!=NULL) {
			if(dir->d_name[0]=='.')
				continue;
			if(strlen(dir->d_name)>4)
				continue;
			count++;
		}
		closedir(d);
	}
	return count;
}

// sorts a group list with selection sort
void SortGList(GROUPLIST *list) {
	int i, j, min_idx;

    for (i = 0; i < list->no_groups - 1; i++) {
        // find minimum group number
        min_idx = i;
        for (j = i + 1; j < list->no_groups; j++)
            if (strcmp(list->group_no[j], list->group_no[min_idx])<0)
                min_idx = j;
 
        // swap group info
        swap_strings(list->group_no[min_idx], list->group_no[i]);
        swap_strings(list->group_mid[min_idx], list->group_mid[i]);
        swap_strings(list->group_name[min_idx], list->group_name[i]);
		swap_stringarray(list->group_users[min_idx], list->group_users[i]);
		swap_ints(&list->no_users[min_idx], &list->no_users[i]);
    }
}

// fills a list with the groups and their infos
// returns number of groups
int ListGroupsDir(GROUPLIST *list){
	DIR *d;
	struct dirent *dir;
	int i=0;
	FILE *fp;
	char GIDname[MAX_PATH];

	list->no_groups=0;
	d = opendir("GROUPS");
	if (d){
		while ((dir = readdir(d)) != NULL){
			if(dir->d_name[0]=='.')
				continue;
			if(strlen(dir->d_name)>2)
				continue;
			strcpy(list->group_no[i], dir->d_name);
			sprintf(GIDname,"GROUPS/%.2s/%.2s_name.txt",dir->d_name,dir->d_name);
			fp=fopen(GIDname,"r");
			if(fp){
				fscanf(fp,"%24s",list->group_name[i]);
				fclose(fp);
			}

			// get users subscribed
			char gpath[MAX_PATH];
			DIR *u;
			struct dirent *gdir;
			sprintf(gpath, "GROUPS/%.2s", dir->d_name);
			u = opendir(gpath);
			int j=0;
			char uid[6], file[5];
			if (u){
				while((gdir=readdir(u))!=NULL) {
					if(gdir->d_name[0]=='.')
						continue;
					if(strlen(gdir->d_name)!=9)
						continue;
					if(sscanf(gdir->d_name, "%5[^.]%4s", uid, file)!=2)
						continue;
					if(strcmp(file, ".txt")!=0)
						continue;
					strcpy(list->group_users[i][j], uid);
					j++;
				}
				list->no_users[i] = j;
				closedir(u);
			}

			// get MID
			sprintf(list->group_mid[i], "%.4d", CountMsg(dir->d_name));

			++i;
			if(i==99)
				break;
		}
		list->no_groups=i;
		closedir(d);
	}
	else
		return(-1);

	if(list->no_groups>1)
		SortGList(list);

	return(list->no_groups);
}

// creates directory for user uid
int CreateUserDir(char *UID){
	char user_dirname[20];
	int ret;
	sprintf(user_dirname,"USERS/%s",UID);
	ret=mkdir(user_dirname,0700);
	if(ret==-1)
		return(0);
	return(1);
}

// creates directory for a new group
// returns 1 if successful, else 0
int CreateGroupDir(){
	char group_dirname[20];
	int ret, group_count=ListGroupsDir(glist);
	if(group_count>=MAX_GROUPS) {
		printf("Max number of groups\n");
		return(0);
	}
	group_count++;
	sprintf(group_dirname,"GROUPS/%02d",group_count);
	ret=mkdir(group_dirname,0700);
	if(ret==-1)
		return(0);
	// create MSG directory inside group directory
	sprintf(group_dirname,"GROUPS/%02d/MSG",group_count);
	ret=mkdir(group_dirname,0700);
	if(ret==-1)
		return(0);
	return(1);
}

// creates directory for new message in group GID
// returns path to created directory
char* CreateMsgDir(char *GID){
	int msg_count=CountMsg(GID), ret;
	static char msg_dirname[20];

	if(msg_count>=9999)
		return NULL;
	sprintf(msg_dirname,"GROUPS/%s/MSG/%04d", GID, msg_count+1);
	ret=mkdir(msg_dirname,0700);
	if(ret==-1)
		return NULL;
	return(msg_dirname);
}

// deletes the password text file
int DelPassFile(char *UID){
	char pathname[50];
	sprintf(pathname,"USERS/%s/%s_pass.txt",UID,UID);
	if(unlink(pathname)==0)
		return(1);
	else
		return(0);
}

// deletes the login file of user uid
int DelLoginFile(char *UID){
	char pathname[50];
	sprintf(pathname,"USERS/%s/%s_login.txt",UID,UID);
	if(unlink(pathname)==0)
		return(1);
	else
		return(0);
}

// deletes the user uid
int DelUserDir(char *UID){
	char user_dirname[20];
	sprintf(user_dirname,"USERS/%s",UID);
	if(rmdir(user_dirname)==0)
		return(1);
	else
		return(0);
}

// deletes the file of user uid from the directory of group gid
int DelUserFile(char *GID, char *UID){
	char pathname[50];
	sprintf(pathname,"GROUPS/%s/%s.txt",GID,UID);
	if(unlink(pathname)==0)
		return(1);
	else
		return(0);
}

// does the user uid exist?
bool user_exists(char *uid) {
	char path[MAX_PATH];
	FILE *file;

	sprintf(path, "USERS/%s", uid);
	file = fopen(path, "r");
	if(file == NULL)
		return false;
	fclose(file);
	return true;
}

// is user uid logged in?
bool user_loggedin(char *uid) {
	char path[MAX_PATH];
	FILE *file;

	sprintf(path, "USERS/%s/%s_login.txt", uid, uid);
	file = fopen(path, "r");
	if(file == NULL)
		return false;
	fclose(file);
	return true;
}

// is user uid subscribe to group gid?
bool user_subscribed(char *uid, char *gid) {
	char path[MAX_PATH];
	FILE *file;

	sprintf(path, "GROUPS/%s/%s.txt", gid, uid);
	file = fopen(path, "r");
	if(file == NULL)
		return false;
	fclose(file);
	return true;
}

// is pass the correct password for user uid?
bool correct_pass(char *uid, char *pass) {
	char pass_read[PASSWORD_SIZE+1];
	char path[MAX_PATH];
	FILE *file;

	sprintf(path, "USERS/%s/%s_pass.txt", uid, uid);
	file = fopen(path, "r");
	if(file != NULL) {
		fscanf(file, "%s", pass_read);
		fclose(file);
		if(strcmp(pass, pass_read)==0)
			return true;
	}
	return false;
}

// does the group gid exist?
bool group_exists(char *gid) {
	char path[MAX_PATH];
	FILE *file;
	sprintf(path, "GROUPS/%s", gid);
	file = fopen(path, "r");
	if(file == NULL) 
		return false;
	fclose(file);
	return true;
}

/* -------------------------------------------------
    uid must have 5 digits
------------------------------------------------- */
bool is_valid_uid(char* uid_read) {
    if(strlen(uid_read)!=5) return false;
    for(int i=0; i<5; i++)
        if(!isdigit(uid_read[i]))
            return false;
    return true;
}

/* -------------------------------------------------
    pass must be a string of 8 alphanumerical characters
------------------------------------------------- */
bool is_valid_password(char *pass_read) {
    if(strlen(pass_read)!=PASSWORD_SIZE)
        return false;
    for(int i=0; i<strlen(pass_read); i++)
        if(!isalnum(pass_read[i]))
            return false;
    return true;
}

/* -------------------------------------------------
    gid must be a positive, 2 digits integer
------------------------------------------------- */
bool is_valid_gid(int gid) {
    if(gid<1 || gid>99) return false;
    else return true;
}


/* UDP *************************************************************/

int TimerON(int sd){
	struct timeval tmout;
	memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
	tmout.tv_sec=1; /* Wait for 1 sec for a reply from server. */
	return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,
			(struct timeval *)&tmout,sizeof(struct timeval)));
}

int TimerOFF(int sd){
	struct timeval tmout;
	memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
	return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,
			(struct timeval *)&tmout,sizeof(struct timeval)));
}

/* -------------------------------------------------
    sends string s via socket udp
------------------------------------------------- */
void send_udp(char* s){
	TimerON(fd_udp);
	n_udp = sendto(fd_udp, s, strlen(s), 0, (struct sockaddr*)&addr, addrlen);
	if(n_udp == -1) exit(1);
	TimerOFF(fd_udp);
}

/* -------------------------------------------------
    -> REG UID pass
	<- RRG status
------------------------------------------------- */
void reg(char *s){
	char path[27], uid[6], password[PASSWORD_SIZE+1];
	FILE *file;

	sscanf(s, "REG %s %s", uid, password);

	// verifications
	if(!is_valid_uid(uid)) {
		send_udp("RRG NOK\n");
		if(verbose) printf("UID=%s: register error: invalid password\n", uid);
		return;
	}

	if(!is_valid_password(password)) {
		send_udp("RRG NOK\n");
		if(verbose) printf("UID=%s: register error: invalid password\n", uid);
		return;
	}

	// check if duplicated
	if(user_exists(uid)){
		send_udp("RRG DUP\n");
		if(verbose) printf("UID=%s: register error: duplicated user\n", uid);
		return;
	}
	// create user folder
	if(CreateUserDir(uid) == 0){
		send_udp("RRG NOK\n");
		if(verbose) printf("UID=%s: register error\n", uid);
		return;
	}
	// create password file
	sprintf(path, "USERS/%s/%s_pass.txt", uid, uid);
	file = fopen(path, "w");
	if (file == NULL) {
        send_udp("RRG NOK\n");
		if(verbose) printf("UID=%s: register error\n", uid);
		return;
    }
	fputs(password, file);
	fclose(file);

	send_udp("RRG OK\n");
	if(verbose) printf("UID=%s: register ok\n", uid);
}

/* -------------------------------------------------
    -> UNR UID pass
	<- RUN status
------------------------------------------------- */
void unregister(char *s) {
	int n = ListGroupsDir(glist);
	char password[PASSWORD_SIZE+1], uid[6];

	sscanf(s, "UNR %s %s", uid, password);

	if(!user_exists(uid)){
		send_udp("RUN NOK\n");
		if(verbose) printf("UID=%s: unregister error: user doesn't exist\n", uid);
	}
	else if(!correct_pass(uid, password)) {
        send_udp("RUN NOK\n");
		if(verbose) printf("UID=%s: unregister error: wrong password\n", uid);
	}
	else {
		if(DelPassFile(uid) == 0) {
			send_udp("RUN NOK\n");
			if(verbose) printf("UID=%s: unregister error: deleting password file\n", uid);
			return;
		}
		DelLoginFile(uid);
		if(DelUserDir(uid) == 0) {
			send_udp("RUN NOK\n");
			if(verbose) printf("UID=%s: unregister error: deleting user folder\n", uid);
		}
		else {
			// go through every group to check if user file exists
			// if so, delete it
			for(int i=0; i<n; i++) {
				for(int j=0; j<glist->no_users[i]; j++) {
					if(strcmp(glist->group_users[i][j], uid)==0) {
						DelUserFile(glist->group_no[i], uid);
						break;
					}
				}
			}
			send_udp("RUN OK\n");
			if(verbose) printf("UID=%s: unregister ok\n", uid);
		}
	}
}

/* -------------------------------------------------
    -> LOG UID pass
	<- RLO status
------------------------------------------------- */
void login(char *s) {
	char password[PASSWORD_SIZE+1];
	char uid[6], loginpath[MAX_PATH];
	FILE *loginfile;

	sscanf(s, "LOG %s %s", uid, password);

	if(!user_exists(uid)){
		send_udp("RLO NOK\n");
		if(verbose) printf("UID=%s: login error: user doesn't exist\n", uid);
	}
	else if(!correct_pass(uid, password)) {
        send_udp("RLO NOK\n");
		if(verbose) printf("UID=%s: login error: wrong password\n", uid);
	}
	else {
		sprintf(loginpath, "USERS/%s/%s_login.txt", uid, uid);
		loginfile = fopen(loginpath, "w");
		if(loginfile == NULL) {
			send_udp("RLO NOK\n");
			if(verbose) printf("UID=%s: login error: creating login file\n", uid);
		}
		else {
			fclose(loginfile);
			send_udp("RLO OK\n");
			if(verbose) printf("UID=%s: login ok\n", uid);
		}
	}
}

/* -------------------------------------------------
    -> OUT UID pass
	<- ROU status
------------------------------------------------- */
void logout(char *s) {
	char password[PASSWORD_SIZE+1];
	char uid[6];

	sscanf(s, "OUT %s %s", uid, password);

	if(!user_exists(uid)){
		send_udp("ROU NOK\n");
		if(verbose) printf("UID=%s: logout error: user doesn't exist\n", uid);
	}
	else if(!user_loggedin(uid)){
		send_udp("ROU NOK\n");
		if(verbose) printf("UID=%s: logout error: login file doesn't exist\n", uid);
	}
	else if(!correct_pass(uid, password)) {
        send_udp("ROU NOK\n");
		if(verbose) printf("UID=%s: logout error: wrong password\n", uid);
	}
	else{
		if(DelLoginFile(uid)==0) {
			send_udp("ROU NOK\n");
			if(verbose) printf("UID=%s: logout error: deleting login file\n", uid);
		}
		else {
			send_udp("ROU OK\n");
			if(verbose) printf("UID=%s: logout ok\n", uid);
		}
	}
}

/* -------------------------------------------------
    -> GLS
	<- RGL N[ GID GName MID]*
------------------------------------------------- */
void groups() {
	int n = ListGroupsDir(glist);
	char s[3273];

	sprintf(s, "RGL %d", n);
	if(verbose) printf("groups: %d\n", n);

	for(int i=0; i<n; i++) {
		strcat(s, " ");
		strcat(s, glist->group_no[i]);
		strcat(s, " ");
		strcat(s, glist->group_name[i]);
		strcat(s, " ");
		strcat(s, glist->group_mid[i]);
		if(verbose) printf("   group: %s - \"%s\" | messages: %s\n", glist->group_no[i], glist->group_name[i], glist->group_mid[i]);
	}
	strcat(s, "\n");
	send_udp(s);
}

/* -------------------------------------------------
    -> GSR UID GID GName
	<- RGS status
------------------------------------------------- */
void subscribe(char *s) {
	int igid;
	char gname[MAX_GNAME+1], uid[6], gid[3];
	char namepath[50], userpath[50], name_read[MAX_GNAME+1];
	FILE *namefile, *userfile;

	sscanf(s, "GSR %s %s %s", uid, gid, gname);

	// verifications
	if(!user_exists(uid)){
		send_udp("RGS E_USR\n");
		if(verbose) printf("UID=%s: subscribe error: user doesn't exist\n", uid);
		return;
	}
	if(!user_loggedin(uid)){
		send_udp("RGS NOK\n");
		if(verbose) printf("UID=%s: subscribe error: user isn't logged in\n", uid);
		return;
	}
	sscanf(gid, "%d", &igid);
	if(!is_valid_gid(igid) && strcmp(gid,"00")!=0){
		send_udp("RGS NOK\n");
		if(verbose) printf("UID=%s: subscribe error: invalid group id %d\n", uid, igid);
		return;
	}

	int group_count = ListGroupsDir(glist);

	if(strcmp(gid,"00")==0 && group_count<MAX_GROUPS) {
		// create new group directory
		if(CreateGroupDir()==0) {
			if(verbose) printf("UID=%s: subscribe error: can't create group\n", uid);
			return;
		}
		group_count++;
		sprintf(gid, "%02d", group_count);
		// create file with group name
		sprintf(namepath, "GROUPS/%s/%s_name.txt", gid, gid);
		namefile = fopen(namepath, "w");
		if(namefile == NULL) {
			send_udp("RGS NOK\n");
			if(verbose) printf("UID=%s: subscribe error: creating name file\n", uid);
			return;
		}
		// write group name to file
		fputs(gname, namefile);
		fclose(namefile);
		// create subscribed user file
		sprintf(userpath, "GROUPS/%s/%s.txt", gid, uid);
		userfile = fopen(userpath, "w");
		if(userfile == NULL) {
			send_udp("RGS NOK\n");
			if(verbose) printf("UID=%s: subscribe error: creating user file\n", uid);
			return;
		}
		fclose(userfile);
		char res[20];
		sprintf(res, "RGS NEW %s\n", gid);
		send_udp(res);
		if(verbose) printf("UID=%s: new group: %s - \"%s\"\n", uid, gid, gname);
	}
	else if(strcmp(gid,"00")==0 && group_count>=MAX_GROUPS) {
		send_udp("RGS E_FULL\n");
		if(verbose) printf("UID=%s: subscribe error: too many groups already exist\n", uid);
		return;
	}
	else if(!group_exists(gid)){
		send_udp("RGS E_GRP\n");
		if(verbose) printf("UID=%s: subscribe error: group doesn't exist\n", uid);
		return;
	}
	else {
		// open file with name and check
		sprintf(namepath, "GROUPS/%s/%s_name.txt", gid, gid);
		namefile = fopen(namepath, "r");
		if(namefile == NULL) {
			send_udp("RGS NOK\n");
			if(verbose) printf("UID=%s: subscribe error: opening name file\n", uid);
			return;
		}
		fscanf(namefile, "%s", name_read);
		fclose(namefile);
		if(strcmp(name_read, gname)!=0) {
			send_udp("RGS E_GNAME\n");
			if(verbose) printf("UID=%s: subscribe error: wrong group name\n", uid);
			return;
		}
		// create subscribed user file
		sprintf(userpath, "GROUPS/%s/%s.txt", gid, uid);
		userfile = fopen(userpath, "w");
		if(userfile == NULL) {
			send_udp("RGS NOK\n");
			if(verbose) printf("UID=%s: subscribe error: creating user file\n", uid);
			return;
		}
		send_udp("RGS OK\n");
		if(verbose) printf("UID=%s: subscribe ok: group %s - \"%s\"\n", uid, gid, gname);
		fclose(userfile);
	}
}

/* -------------------------------------------------
    -> GUR UID GID
	<- RGU status
------------------------------------------------- */
void unsubscribe(char *s) {
	char uid[6], gid[3];

	sscanf(s, "GUR %s %s", uid, gid);

	// verifications
	if(!user_exists(uid)){
		send_udp("RGU E_USR\n");
		if(verbose) printf("UID=%s: unsubscribe error: user doesn't exist\n", uid);
	}
	else if(!user_loggedin(uid)){
		send_udp("RGU E_USR\n");
		if(verbose) printf("UID=%s: unsubscribe error: user isn't logged in\n", uid);
	}
	else if(!group_exists(gid)){
		send_udp("RGU E_GRP\n");
		if(verbose) printf("UID=%s: unsubscribe error: group doesn't exist\n", uid);
	}

	else if(DelUserFile(gid, uid)==0) {
		send_udp("RGU NOK\n");
		if(verbose) printf("UID=%s: unsubscribe error: deleting user file\n", uid);
	}
	else {
		send_udp("RGU OK\n");
		if(verbose) printf("UID=%s: unsubscribe ok: group %s\n", uid, gid);
	}
}

/* -------------------------------------------------
    -> GLM UID
	<- RGM N[ GID GName MID]*
------------------------------------------------- */
void mgl(char *s) {
	char uid[6], rgm[3273], buffer[3273];
	int count=0, n=ListGroupsDir(glist);
	char *res = buffer;

	sscanf(s, "GLM %s", uid);
	// verifications
	if(!user_exists(uid)){
		send_udp("RGM E_USR\n");
		if(verbose) printf("UID=%s: my_groups error: user doesn't exist\n", uid);
		return;
	}
	if(!user_loggedin(uid)){
		send_udp("RGM E_USR\n");
		if(verbose) printf("UID=%s: my_groups error: user isn't logged in\n", uid);
		return;
	}
	
	if(verbose) printf("UID=%s: my_groups:\n", uid);
	// go through every group to check if user file exists
	for(int i=0; i<n; i++) {
		for(int j=0; j<glist->no_users[i]; j++) {
			if(strcmp(glist->group_users[i][j], uid)==0) {
				count++;
				sprintf(res, " %s %s %s", glist->group_no[i], glist->group_name[i], glist->group_mid[i]);
				if(verbose) printf("   group: %s - \"%s\" | messages: %s\n", glist->group_no[i], glist->group_name[i], glist->group_mid[i]);
				res += (1+2+1+strlen(glist->group_name[i])+1+4);
				break;
			}
		}
	}
	if(verbose) printf("   - %d group(s)\n", count);
	sprintf(rgm, "RGM %d", count);
	if(count>0) strcat(rgm, buffer);
	strcat(rgm, "\n");
	send_udp(rgm);
}

/* -------------------------------------------------
    see which command was received
	and act accordingly
------------------------------------------------- */
void udp_commands(char* s){
	char command[3];
	sscanf(s, "%s", command);

	if(strcmp(command, "REG") == 0)
		reg(s);
	else if(strcmp(command, "UNR") == 0)
		unregister(s);
	else if(strcmp(command, "LOG") == 0)
		login(s);
	else if(strcmp(command, "OUT") == 0)
		logout(s);
	else if(strcmp(command, "GLS") == 0)
		groups();
	else if(strcmp(command, "GSR") == 0)
		subscribe(s);
	else if(strcmp(command, "GUR") == 0)
		unsubscribe(s);
	else if(strcmp(command, "GLM") == 0)
		mgl(s);
	else
		send_udp("ERR\n");
}


/* TCP commands ****************************************************/
/* -------------------------------------------------
    receives a tcp message byte by byte 
	until a space is found
	returns it's length
------------------------------------------------- */
int receive_tcp_space(char *buffer){
    ssize_t nread, total=0;
    char *ptr;

    bool found = false;

    ptr=buffer;
    while(!found){
        nread=read(connfd,ptr,1);
        if(nread==-1) exit(1); // error
        else if(nread==0) break; // closed by peer
        // see if it's end of message
        if(ptr[0] == ' '){
        	found=true;
        	break;
        }
        ptr+=nread;
		total+=nread;
    }
    return total;
}
/* -------------------------------------------------
    receives a tcp message of known length
    saves it in buffer
------------------------------------------------- */
int receive_tcp(char *buffer, int length){
	ssize_t nleft, nread;
	char *ptr;

	nleft = length;
	ptr = buffer;

	while(nleft>0) {
		nread=read(connfd, ptr, nleft);
		if(errno==EAGAIN || errno==EWOULDBLOCK) break;
		if(nread==-1) exit(1);
		else if(nread==0) break;
		nleft-=nread;
		ptr+=nread;
	}
	return nleft;
}

/* -------------------------------------------------
    sends to socket the string s
------------------------------------------------- */
void send_tcp(char *s, int length){
    ssize_t nbytes, nleft, nwritten;
    char *ptr;

    ptr=s;
    nbytes=length;
    nleft=nbytes;

    while(nleft>0){
        nwritten=write(connfd,ptr,nleft);
        if(nwritten<=0) exit(1);
        nleft-=nwritten;
        ptr+=nwritten;
    }
}

/* -------------------------------------------------
    -> ULS GID
	<- RUL status [GName [UID ]*]
------------------------------------------------- */
void ulist(){
	int gid;
	char s[3], sgid[3], buffer[3273];
	char *res = buffer;

	receive_tcp(s, 4);
	sscanf(s, " %d", &gid);
	sprintf(sgid, "%02d", gid);

	ListGroupsDir(glist);

	if(group_exists(sgid)) {
		sprintf(res, "RUL OK %s", glist->group_name[gid-1]);
		res += (7+strlen(glist->group_name[gid-1]));
		if(verbose) printf("GID=%s: ulist: %d\n", sgid, glist->no_users[gid-1]);
		for(int i=0; i<glist->no_users[gid-1]; i++) {
			sprintf(res, " %s", glist->group_users[gid-1][i]);
			if(verbose) printf("   user: %s\n", glist->group_users[gid-1][i]);
			res += 6;
		}
		*res = '\n';
		send_tcp(buffer, strlen(buffer));
	}
	else {
		if(verbose) printf("GID=%s: ulist error: group doesn't exist\n", sgid);
		send_tcp("RUL NOK\n", 8);
	}

}

/* -------------------------------------------------
    -> PST UID GID Tsize text [Fname Fsize data]
	<- RPT status
------------------------------------------------- */
void post(){
	char uid[6], gid[3], mid[5], text[241], fname[25], filepath[50], res[10];
	int tsize, fsize, nread;
	FILE *file;
	char *s = malloc(sizeof(char)*350);

	receive_tcp(s, 13); // receive until Tsize
	sscanf(s, " %s %s %d", uid, gid, &tsize);

	if(!user_subscribed(uid, gid)){
		send_tcp("RPT NOK\n", 8);
		if(verbose) printf("UID=%s: post error: isn't subscribed to group %s\n", uid, gid);
		free(s);
		return;
	}
	if(tsize>240) {
		send_tcp("RPT NOK\n", 8);
		if(verbose) printf("UID=%s: post error: invalid text size\n", uid);
		free(s);
		return;
	}

	int scanned = 10+get_digits(tsize);
	int left = 1+tsize-(13-scanned);
	char *ptr = s+13;

	// read text
	receive_tcp(ptr, left);
	ptr=s+scanned+1;
	for(int i=0; i<tsize; i++) {
		text[i] = *ptr;
		ptr++;
	}
	text[tsize]='\0'; // end string

	// create message directory
	sprintf(mid, "%04d", CountMsg(gid)+1);
	char *msgpath = CreateMsgDir(gid);
	if(msgpath==NULL) {
		send_tcp("RPT NOK\n", 8);
		if(verbose) printf("GID=%s: post error: failed to create msg directory\n", gid);
		free(s);
		return;
	}

	// create author file
	sprintf(filepath, "%s/A U T H O R.txt", msgpath);
	file = fopen(filepath, "w");
	if(file == NULL) {
		send_tcp("RPT NOK\n", 8);
		if(verbose) printf("UID=%s: post error: failed to create AUTHOR file\n", uid);
		free(s);
		return;
	}
	fputs(uid, file);
	fclose(file);

	// create text file
	sprintf(filepath, "%s/T E X T.txt", msgpath);
	file = fopen(filepath, "w");
	if(file == NULL) {
		send_tcp("RPT NOK\n", 8);
		if(verbose) printf("GID=%s: post error: failed to create TEXT file\n", gid);
		free(s);
		return;
	}
	fputs(text, file);
	fclose(file);
	
	receive_tcp(ptr, 1);
	if(ptr[0]=='\n'){ // no file
		sprintf(res, "RPT %s\n", mid);
		send_tcp(res, strlen(res));
		if(verbose) printf("UID=%s: post group %s:\n           \"%s\"\n", uid, gid, text);
	}
	else { // there is a file
		ptr++;
		// receive until space to get fname
		nread = receive_tcp_space(ptr);
		sscanf(ptr, "%s ", fname);
		ptr+=nread;
		// receive until space to get fsize
		nread = receive_tcp_space(ptr);
		sscanf(ptr, "%d ", &fsize);

		s = realloc(s, sizeof(char)*(350+fsize));
		ptr = s+scanned+1+tsize+1;
		scanned = strlen(fname)+1+get_digits(fsize)+1;
		ptr += scanned;
		// receive the file length
		receive_tcp(ptr, fsize);

		// create received file
		sprintf(filepath, "%s/%s", msgpath, fname);
		file = fopen(filepath, "wb");
		if(file == NULL || fwrite(ptr, sizeof(char), fsize, file)!=fsize) {
			send_tcp("RPT NOK\n", 8);
			if(verbose) printf("GID=%s: post error: failed to save file\n", gid);
			free(s);
			return;
		}
		fclose(file);
		sprintf(res, "RPT %s\n", mid);
		send_tcp(res, strlen(res));
		if(verbose) printf("UID=%s: post group %s:\n           \"%s\" %s\n", uid, gid, text, fname);
	}
	free(s);
}

/* -------------------------------------------------
    -> RTV UID GID MID
	<- RRT status [N[ MID UID Tsize text[ / Fname Fsize data]]*]
------------------------------------------------- */
void retrieve(){
	int mid, count=0, res_len=1;
	char s[20], uid[6], gid[3], path[30], msgpath[30], fpath[55];
	char author[6], fname[25], firstmid[5], smid[5];
	char *string;
	long int tsize, fsize, string_len, filestring_len;
	DIR *d, *m;
	struct dirent *dir, *msgdir;
	FILE *file;

	receive_tcp(s, 15);
	if(sscanf(s, " %s %s %d\n", uid, gid, &mid)!=3) {
		send_tcp("RRT NOK\n", 8);
		if(verbose) printf("retrieve: error: wrong format\n");
		return;
	}
	if(!user_subscribed(uid, gid)){
		send_tcp("RRT NOK\n", 8);
		if(verbose) printf("UID=%s: retrieve: error: isn't subscribed to group %s\n", uid, gid);
		return;
	}
	sprintf(firstmid, "%04d", mid);
	sprintf(path, "GROUPS/%s/MSG", gid);
	d = opendir(path);
	if(d){
		char *res = calloc(res_len, sizeof(char));
		if(verbose) printf("UID=%s: retrieve group %s, message(s):\n", uid, gid);
		// folders inside GROUPS/GID/MSG
		while((dir=readdir(d))!=NULL && count<20) {
			// skip non relevant files
			if(strcmp(dir->d_name, firstmid)<0)
				continue;
			if(dir->d_name[0]=='.')
				continue;
			if(strlen(dir->d_name)>4)
				continue;
			if (dir->d_type == DT_REG) // if entry is a regular file
         		continue;
    			
			sprintf(smid, "%04d", mid);
			sprintf(msgpath, "GROUPS/%s/MSG/%s", gid, smid);

			// inside GROUPS/GID/MSG/MID ------------------------------
			// author file
			sprintf(fpath, "%s/A U T H O R.txt", msgpath);
			file = fopen(fpath, "r");
			if(file == NULL) {
				send_tcp("RRT NOK\n", 8);
				if(verbose) printf("retrieve: error: can't open author file\n");
				free(res);
				closedir(d);
				return;	
			}
			fscanf(file, "%s", author);
			fclose(file);

			// text file
			sprintf(fpath, "%s/T E X T.txt", msgpath);
		    file = fopen(fpath, "r");
			if(file == NULL) {
				send_tcp("RRT NOK\n", 8);
				if(verbose) printf("retrieve: error: can't open text file\n");
				free(res);
				closedir(d);
				return;	
			}
			// get text size
			fseek(file, 0, SEEK_END);
			tsize=ftell(file);
			fseek(file, 0L, SEEK_SET);
			// prepare string
			string_len = 1+4+1+5+1+get_digits(tsize)+1+tsize;
			string = calloc(string_len+1, sizeof(char)); // +1 for \0 at end of text
			sprintf(string, " %s %s %ld ", smid, author, tsize);
			// copy contents of text file to string
			fread(string+string_len-tsize, sizeof(char), tsize, file);
			fclose(file);
			string[string_len] = '\0';
			if(verbose) printf("   %s - %s \"%s\"", smid, author, &string[string_len-tsize]);

			// other files
			m = opendir(msgpath);
			if(m) {
				while((msgdir=readdir(m))!=NULL) {
					if(msgdir->d_name[0]=='.') continue;
					if(strcmp(msgdir->d_name, "A U T H O R.txt")==0) continue;
					if(strcmp(msgdir->d_name, "T E X T.txt")==0) continue;

					// open file
					strcpy(fname, msgdir->d_name);
					sprintf(fpath, "%s/%s", msgpath, fname);
					file = fopen(fpath, "rb");
					if(file==NULL) {
						send_tcp("RRT NOK\n", 8);
						if(verbose) printf("retrieve: error: can't open file\n");
						free(res);
						free(string);
						closedir(d);
						return;	
					}
					if(verbose) printf(" %s", fname);
					// get file size
					fseek(file, 0L, SEEK_END);
					fsize = ftell(file);
					// put file info into string
					filestring_len = 3+strlen(fname)+1+get_digits(fsize)+1+fsize;
					string = realloc(string, sizeof(char)*(string_len+filestring_len));
					sprintf(&string[string_len], " / %s %ld ", fname, fsize);
					string_len+=filestring_len;
					fseek(file, 0L, SEEK_SET);
					fread(&string[string_len-fsize], sizeof(char), fsize, file);
					fclose(file);
					break;
				}
				if(verbose) printf("\n");
				// add string of this message to the final string
				res = realloc(res, (res_len+string_len)*sizeof(char));
				memcpy(&res[res_len-1], string, string_len);
				res_len+=string_len;
				free(string);
				closedir(m);
				mid++;
				count++;
			}
			else{
				send_tcp("RRT NOK\n", 8);
				if(verbose) printf("GID=%s: retrieve error: can't open MSG sub folder\n", gid);
				free(res);
				closedir(d);
				return;
			}
		}
		// no messages found
		if(count==0){
			send_tcp("RRT EOF\n", 8);
			if(verbose) printf("   no messages\n");
			free(res);
			closedir(d);
			return;
		}
		// if there are messages
		// create final string with total number of messages
		int final_len = 7+get_digits(count)+res_len;
		char *final = calloc(final_len, sizeof(char));
		sprintf(final, "RRT OK %d", count);
		memcpy(&final[final_len-res_len], res, res_len-1);
		final[final_len-1] = '\n';
		send_tcp(final, final_len);
		free(res);
		free(final);
		closedir(d);
	}
	else{
		send_tcp("RRT NOK\n", 8);
		if(verbose) printf("GID=%s: retrieve error: can't open MSG directory\n", gid);
	}
}

/* -------------------------------------------------
    see which command was received
	and act accordingly
------------------------------------------------- */
void tcp_commands(char* s){
	if(strncmp(s, "ULS", 3) == 0)
		ulist();
	else if(strncmp(s, "PST", 3) == 0)
		post();
	else if(strncmp(s, "RTV", 3) == 0)
		retrieve();
	else
		send_tcp("ERR\n", 4);
}


/********
 * MAIN *
 ********/

int main(int argc, char**argv){
	char buffer[128];
    char dsport[128]="58032"; // default
	pid_t childpid;
	int nready;

	glist = malloc(sizeof(GROUPLIST));

	/* Initial arguments 
     * [-p DSport] [-v] */
    if(argc==1);
	else if(argc==2) {
        if(strcmp("-v",argv[1])==0)
            verbose = true;
        else {
            printf("Error: wrong arguments\n");
            exit(1);
        }
	}
    else if(argc==3) {
        if(strcmp("-p",argv[1])==0)
            strcpy(dsport, argv[2]);
        else {
            printf("Error: wrong arguments\n");
            exit(1);
        }
    }
	else if(argc==4) {
        if(strcmp("-p",argv[1])==0) {
            strcpy(dsport, argv[2]);
			if(strcmp("-v",argv[3])==0)
				verbose = true;
			else {
			    printf("Error: wrong arguments\n");
            	exit(1);	
			}
		}
		else if(strcmp("-v",argv[1])==0) {
            verbose = true;
			if(strcmp("-p",argv[2])==0)
            	strcpy(dsport, argv[3]);
			else {
				printf("Error: wrong arguments\n");
				exit(1);
			}
		}
	}
	else {
        printf("Error: wrong number of arguments\n");
        exit(1);
    }

    // Socket UDP ---------------------------------------------------
	if((fd_udp=socket(AF_INET,SOCK_DGRAM,0))==-1) { /*error*/
		printf("error creating socket udp\n");
		exit(1);
	}
	memset(&hints_udp,0,sizeof hints_udp);
	
	hints_udp.ai_family=AF_INET;//IPv4
	hints_udp.ai_socktype=SOCK_DGRAM;//UDP socket
	hints_udp.ai_flags=AI_PASSIVE;
	
	if((errcode=getaddrinfo(NULL,dsport,&hints_udp,&res_udp))!=0){ /*error*/
		printf("error getaddrinfo udp\n");
		exit(1);
	}
	if(bind(fd_udp,res_udp->ai_addr,res_udp->ai_addrlen)==-1){ /*error*/
		printf("error bind udp\n");
		exit(1);
	}
	
	// Socket TCP ---------------------------------------------------
	if((fd_tcp=socket(AF_INET,SOCK_STREAM,0))==-1){ /*error*/
		printf("error creating socket tcp\n");
		exit(1);
	}
	memset(&hints_tcp,0,sizeof hints_tcp);
	
	hints_tcp.ai_family=AF_INET;//IPv4
	hints_tcp.ai_socktype=SOCK_STREAM;//TCP socket
	hints_tcp.ai_flags=AI_PASSIVE;

	if((errcode=getaddrinfo(NULL,dsport,&hints_tcp,&res_tcp))!=0){ /*error*/
		printf("error getaddrinfo tcp\n");
		exit(1);
	}
	if(bind(fd_tcp,res_tcp->ai_addr,res_tcp->ai_addrlen)==-1){ /*error*/
		printf("error bind tcp\n");
		exit(1);
	}
	if(listen(fd_tcp,5)==-1){ /*error*/
		printf("error listen\n");
		exit(1);
	}

	// --------------------------------------------------------------

	FD_ZERO(&rset);
	maxfdp1 = max(fd_udp, fd_tcp) + 1;
	
	for(;;){
		FD_SET(fd_udp, &rset);
		FD_SET(fd_tcp, &rset);

		nready=select(maxfdp1, &rset, NULL, NULL, NULL);
		if(nready<=0)/*error*/exit(1);

		// receive UDP message if UDP socket is readable
		if(FD_ISSET(fd_udp, &rset)){
			addrlen = sizeof(addr);
			bzero(buffer, sizeof(buffer));
			TimerON(fd_udp);
			n_udp=recvfrom(fd_udp, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
			if(n_udp == -1) exit(1);
			TimerOFF(fd_udp);
			udp_commands(buffer);
		}

		// accept TCP connection if TCP socket is readable
		if(FD_ISSET(fd_tcp, &rset)){
			addrlen = sizeof(addr);
			connfd = accept(fd_tcp, (struct sockaddr*)&addr, &addrlen);
			if((childpid = fork()) == 0){
				close(fd_tcp);
				bzero(buffer, sizeof(buffer));

				// receive start of message
				int nread=0;
				char command[3];
				while(nread<3) {
					n_tcp=read(connfd, command, sizeof(command));
					if(n_tcp==0) break;
					else if(n_tcp==-1) exit(1);
					nread+=n_tcp;
				}
				tcp_commands(command);
				close(connfd);
				exit(0);
			}
			close(connfd);
		}
	}
	
	freeaddrinfo(res_udp);
	close(fd_udp);
	freeaddrinfo(res_tcp);
	close(fd_tcp);
	free(glist);
	exit(0);
}