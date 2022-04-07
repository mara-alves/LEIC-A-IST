#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>

#define MAX_INPUT_SIZE 500
#define MAX_TSIZE 240
#define MAX_FNAME 24
#define MAX_GNAME 24
#define MAX_TEXT 240
#define PASSWORD_SIZE 8

int uid=-1; // currently logged in
char pass[PASSWORD_SIZE];
int gid=-1; // selected group

// Socket variables
int fd_udp, fd_tcp;
ssize_t n_udp, n_tcp;
struct addrinfo hints_udp, hints_tcp, *res_udp, *res_tcp;


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

/* -------------------------------------------------
    GName must be a string of 24 alphanumerical characters
    or less, and may have '-' and '_'
------------------------------------------------- */
bool is_valid_gname(char *gname) {
    if(strlen(gname)>MAX_GNAME)
        return false;
    for(int i=0; i<strlen(gname); i++)
        if(!isalnum(gname[i]) && gname[i]!='-' && gname[i]!='_')
            return false;
    return true;
}

/* -------------------------------------------------
    text must be a string of 24 characters
    or less
------------------------------------------------- */
bool is_valid_text(char *text) {
    if(strlen(text)>MAX_TEXT)
        return false;
    else return true;
}

/* -------------------------------------------------
    Fname must be a string of 24 alphanumerical characters
    or less, and may have '-' and '_', and end with .xxx
------------------------------------------------- */
bool is_valid_fname(char *gname) {
    if(strlen(gname)>MAX_FNAME)
        return false;
    if(gname[strlen(gname)-4]!='.')
        return false;
    for(int i=0; i<strlen(gname); i++)
        if(!isalnum(gname[i]) && gname[i]!='-' && gname[i]!='_' && gname[i]!='.')
            return false;
    return true;
}


/* UDP *************************************************************/

int TimerON(int sd){
	struct timeval tmout;
	memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
	tmout.tv_sec=15; /* Wait for 15 sec for a reply from server. */
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
    sends string s via socket udp and receives response
------------------------------------------------- */
char* send_receive_udp(char* s, int length){
    socklen_t addrlen;
    struct sockaddr_in addr;
    static char buffer[3274+1];
    // max length occurs when response to groups
    // is filled with 99 groups all with max group name
    // hence the 3274 = 3+1+2+(1+2+1+24+1+4)*99+1

    n_udp=sendto(fd_udp,s,length,0,res_udp->ai_addr,res_udp->ai_addrlen);
    if(n_udp==-1) exit(1);

    addrlen=sizeof(addr);

    TimerON(fd_udp);
    n_udp=recvfrom(fd_udp,buffer,3274,0,(struct sockaddr*)&addr,&addrlen);
    if(n_udp==-1) exit(1);
    TimerOFF(fd_udp);

    buffer[n_udp] = '\0'; // end string after \n
    return(buffer);
}

/* -------------------------------------------------
    reg UID pass
    regista um novo utilizador
------------------------------------------------- */
void reg() {
    char s[MAX_INPUT_SIZE], pass_read[PASSWORD_SIZE+1], uid_read[6];
    int count;

    // read line
    fgets(s,MAX_INPUT_SIZE,stdin);
    count = sscanf(s, " %s %s", uid_read, pass_read);

    if(count != 2) {
        printf("  Error: please enter UID and password\n");
        return;
    }
    if(!is_valid_uid(uid_read)) {
        printf("  Error: invalid uid\n");
        return;
    }
    if(!is_valid_password(pass_read)) {
        printf("  Error: invalid password\n");
        return;
    }
    // send message in correct format
    sprintf(s, "REG %s %s\n", uid_read, pass_read);
    char* res = send_receive_udp(s, strlen(s));
    // check result
    if(strcmp(res, "RRG OK\n")==0)
        printf("  User successfully registered\n");
    else if(strcmp(res, "RRG DUP\n")==0)
        printf("  User already exists\n");
    else if(strcmp(res, "RRG NOK\n")==0)
        printf("  Error registering user...\n");
}

/* -------------------------------------------------
    unregister/unr UID pass
    remove utilizador
------------------------------------------------- */
void unregister() {
    char s[MAX_INPUT_SIZE], pass_read[PASSWORD_SIZE+1];
    int uid_read, count;

    // read line
    fgets(s,MAX_INPUT_SIZE,stdin);
    count = sscanf(s, " %d %s", &uid_read, pass_read);
    
    if(count != 2) {
        printf("  Error: please enter UID and password\n");
        return;
    }
    // send message in correct format
    sprintf(s, "UNR %05d %s\n", uid_read, pass_read);
    char* res = send_receive_udp(s, strlen(s));
    // check result
    if(strcmp(res, "RUN OK\n")==0) {
        // if the user unregistered was logged in, logout
        if(uid_read==uid){uid=-1;strcpy(pass, "");}
        printf("  User successfully unregistered\n");
    }
    else if(strcmp(res, "RUN NOK\n")==0)
        printf("  Couldn't unregister user\n");
}

/* -------------------------------------------------
    login UID pass
    inicia sessao
------------------------------------------------- */
void login() {
    char s[MAX_INPUT_SIZE], pass_read[PASSWORD_SIZE+1];
    int uid_read, count;

    // read line
    fgets(s,MAX_INPUT_SIZE,stdin);
    count = sscanf(s, " %d %s", &uid_read, pass_read);
    if(count != 2) {
        printf("  Error: please enter UID and password\n");
        return;
    }
    // verify if user is already logged in
    if(uid!=-1) {
        printf("  Logout first\n");
        return;
    }
    // send message in correct format
    sprintf(s, "LOG %05d %s\n", uid_read, pass_read);
    char* res = send_receive_udp(s, strlen(s));
    // check result
    if(strcmp(res, "RLO OK\n")==0) {
        printf("  You are now logged in\n");
        uid = uid_read;
        strncpy(pass, pass_read, 8);
    }
    else if(strcmp(res, "RLO NOK\n")==0)
        printf("  Wrong password or uid\n");
}

/* -------------------------------------------------
    logout
    termina sessao do utilizador
------------------------------------------------- */
void logout() {
    char s[MAX_INPUT_SIZE];

    if(uid==-1) {
        printf("  You're already logged out\n");
        return;
    }
    // send message in correct format
    sprintf(s, "OUT %05d %.8s\n", uid, pass);
    char* res = send_receive_udp(s, strlen(s));
    // check result
    if(strcmp(res, "ROU OK\n")==0) {
        printf("  You are now logged out\n");
        uid = -1;
        strcpy(pass, "");
    }
    else if(strcmp(res, "ROU NOK\n")==0)
        printf("  Couldn't log out\n");
    else
        printf("  Error\n");
}

/* -------------------------------------------------
    groups/gl
    apresenta lista de todos os grupos existentes
------------------------------------------------- */
void groups() {
    int count, gid_read, mid;
    char gname[MAX_GNAME+1];

    // send message in correct format
    char* res = send_receive_udp("GLS\n", 4);

    // print result in a list
    if(sscanf(res, "RGL %d", &count)==1) {
        printf("  %d group(s)\n", count);
        res += (3+1+get_digits(count));
        for(int i=0; i<count; i++) {
            sscanf(res, " %d %s %d", &gid_read, gname, &mid);
            printf("  Group: %d - %s | Messages: %d\n", gid_read, gname, mid);
            res += (1+2+1+strlen(gname)+1+4);
        }
    }
    else
        printf("  Error\n");
}

/* -------------------------------------------------
    subscribe/s GID GName
    subscreve o utilizador ao grupo
------------------------------------------------- */
void subscribe() {
    char s[MAX_INPUT_SIZE], gname[MAX_GNAME];
    int gid_read, count;
    
    // read line
    fgets(s,MAX_INPUT_SIZE,stdin);
    count = sscanf(s, " %d %s", &gid_read, gname);

    // verifications
    if(uid==-1){
        printf("  You're not logged in!\n");
        return;
    }
    if(count != 2) {
        printf("  Error: please enter GID and GName\n");
        return;
    }
    if(gid_read==0 && !is_valid_gname(gname)) {
        printf("  Invalid name for new group!\n");
        return;
    }
    if(!is_valid_gid(gid_read) && gid_read!=0) {
        printf("  Invalid group id\n");
        return;
    }
    // send message in correct format
    sprintf(s, "GSR %05d %02d %s\n", uid, gid_read, gname);
    char* res = send_receive_udp(s, strlen(s));

    // check result
    if(strcmp(res, "RGS OK\n")==0)
        printf("  Subscribed: %02d - \"%s\"\n", gid_read, gname);
    else if(strstr(res, "RGS NEW")!=NULL) {
        int new_gid;
        sscanf(res, "RGS NEW %d", &new_gid);
        printf("  New group created and subscribed: %02d - \"%s\"\n", new_gid, gname);
    }
    else if(strcmp(res, "RGS E_USR\n")==0)
        printf("  Error: invalid user\n");
    else if(strcmp(res, "RGS E_GRP\n")==0)
        printf("  Error: invalid group id\n");
    else if(strcmp(res, "RGS E_GNAME\n")==0)
        printf("  Error: invalid group name\n");
    else if(strcmp(res, "RGS E_FULL\n")==0)
        printf("  Error: too many groups already exist\n");
    else if(strcmp(res, "RGS NOK\n")==0)
        printf("  Error\n");
}

/* -------------------------------------------------
    unsubscribe/u GID
    remove utilizador do grupo
------------------------------------------------- */
void unsubscribe() {
    char s[MAX_INPUT_SIZE];
    int gid_read, count;

    // read line
    fgets(s,MAX_INPUT_SIZE,stdin);
    count = sscanf(s, " %d", &gid_read);

    // verifications
    if(uid==-1){
        printf("  You're not logged in!\n");
        return;
    }
    if(count != 1) {
        printf("  Error: please enter group id\n");
        return;
    }
    if(!is_valid_gid(gid_read)) {
        printf("  Invalid group id\n");
        return;
    }
    // send message in correct format
    sprintf(s, "GUR %05d %02d\n", uid, gid_read);
    char* res = send_receive_udp(s, strlen(s));

    // check result
    if(strcmp(res, "RGU OK\n")==0)
        printf("  Unsubscribed: %02d\n", gid_read);
    else if(strcmp(res, "RGU E_USR\n")==0)
        printf("  Error: invalid user id\n");
    else if(strcmp(res, "RGU E_GRP\n")==0)
        printf("  Error: invalid group id\n");
    else if(strcmp(res, "RGU NOK\n")==0)
        printf("  Error\n");
}

/* -------------------------------------------------
    my_groups/mgl
    mostra grupos a que utilizador estah subscrito
------------------------------------------------- */
void mgl() {
    char s[MAX_INPUT_SIZE];
    char gname[MAX_GNAME];
    int count, gid_read, mid;
    
    if(uid==-1) {
        printf("  You're not logged in!\n");
        return;
    }

    // send message in correct format
    sprintf(s, "GLM %05d\n", uid);
    char* res = send_receive_udp(s, strlen(s));

    if(strcmp(res, "RGM E_USR\n")==0) {
        printf("  Error: invalid user id\n");
        return;
    }

    if(sscanf(res, "RGM %d", &count)==1) {
        // print results in a list
        res+=(4+get_digits(count)+1);
        for(int i=0;i<count;i++) {
            sscanf(res, "%d %s %d", &gid_read, gname, &mid);
            res += (2+1+strlen(gname)+1+4+1);
            printf("  Group: %02d - %s | Messages: %04d\n", gid_read, gname, mid);
        }
        printf("  Subscribed to %d group(s)\n", count);
    }
    else
        printf("  Error\n");
}

/* -------------------------------------------------
    select/sag GID
    seleciona como grupo ativo o grupo GID
------------------------------------------------- */
void sag() {
    char s[MAX_INPUT_SIZE];
    int gid_read, count;

    // read line
    fgets(s,MAX_INPUT_SIZE,stdin);
    count = sscanf(s, " %d", &gid_read);
    if(count != 1) {
        printf("  Error: please enter group id\n");
        return;
    }
    if(!is_valid_gid(gid_read)) {
        printf("  Error: invalid group id\n");
        return;
    }
    printf("  Group %02d is now the active group\n", gid_read);
    gid = gid_read;
}


/* TCP *************************************************************/

/* -------------------------------------------------
    sends to socket fd the string s
------------------------------------------------- */
void send_tcp(char *s, int length){
    ssize_t nbytes, nleft, nwritten;
    char *ptr;

    ptr=s;
    nbytes=length;
    nleft=nbytes;

    while(nleft>0){
        nwritten=write(fd_tcp,ptr,nleft);
        if(nwritten<=0) exit(1);
        nleft-=nwritten;
        ptr+=nwritten;
    }
}

/* -------------------------------------------------
    receives a tcp message until a \n is found
    and returns it
------------------------------------------------- */
char* receive_tcp(){
    ssize_t nread;
    char *ptr;
    static char buffer[60027]; 
    // max length occurs when response to ulist has all users
    // hence the 60026 = 3+1+2+1+24+(1+5)*9999+1

    bool found = false;

    ptr=buffer;
    while(!found){
        nread=read(fd_tcp,ptr,128);
        if(nread==-1) exit(1); // error
        else if(nread==0) break; // closed by peer
        // look for end of message
        for(int i=0; i<nread; i++){
            char c = ptr[i];
            if(c == '\n'){
                found=true;
                break;
            }
        }
        ptr+=nread;
    }
    *ptr = '\0'; // end string after \n
    return buffer;
}

/* -------------------------------------------------
    receives a tcp message of known length
    saves it in buffer
------------------------------------------------- */
void receive_tcp_len(char *buffer, int length){
	ssize_t nleft, nread;
	char *ptr;

	nleft = length;
	ptr = buffer;

	while(nleft>0) {
		nread=read(fd_tcp, ptr, nleft);
		if(nread==-1) exit(1);
		else if(nread==0) break;
		nleft-=nread;
		ptr+=nread;
	}
    *ptr='\0';
}
/* -------------------------------------------------
    receives a tcp message byte by byte 
	until a space is found
	returns it's length
------------------------------------------------- */
int receive_tcp_space(char *buffer){
    ssize_t nread;
    int total=0;
    char *ptr;

    bool found = false;

    ptr=buffer;
    while(!found){
        nread=read(fd_tcp,ptr,1);
        if(nread==-1) exit(1); // error
        else if(nread==0) break; // closed by peer
        // see if it's end of message
        if(ptr[0] == ' '){
        	found=true;
        	break;
        }
		total+=nread;
        ptr+=nread;
    }
    *ptr = '\0';
    return total;
}

/* -------------------------------------------------
    ulist/ul
    mostra lista dos utilizadores subscritos
    no grupo selecionado
------------------------------------------------- */
void ulist() {
    char s[MAX_INPUT_SIZE], gname[MAX_GNAME+1], status[4], *res;
    int uid_read;

    if(gid==-1) {
        printf("  First select the group\n");
        return;
    }
    // create socket
    fd_tcp=socket(AF_INET, SOCK_STREAM, 0);
    if(fd_tcp==-1) exit(1);
    // connect client socket to server socket
    n_tcp=connect(fd_tcp,res_tcp->ai_addr, res_tcp->ai_addrlen);
    if(n_tcp==-1) exit(1);
    // send message in correct format
    sprintf(s, "ULS %02d\n", gid);
    send_tcp(s,strlen(s));

    // check result
    res = receive_tcp(); 
    sscanf(res, "RUL %s %s", status, gname);

    if(strcmp(status, "NOK")==0) 
        printf("  Group %02d does not exist\n", gid);

    else if(strcmp(status, "OK")==0) {
        printf("  Users subscribed to group %02d - \"%s\":\n", gid, gname);
        // print result in a list
        res += (7+strlen(gname)); // RUL OK GName
        while(sscanf(res, " %d", &uid_read)==1) {
            printf("  - %05d\n", uid_read);
            res += 6;
        }
    }
    else
        printf("  Error\n");

    close(fd_tcp); // close socket
}

/* -------------------------------------------------
    post “text” [Fname] 
    utilizador envia mensagem a grupo selecionado
    poderah ou nao ter ficheiro
------------------------------------------------- */
void post(){
    int count, mid;
    long int filesize, len;
    char s[MAX_INPUT_SIZE], text[MAX_TSIZE+1], filename[MAX_FNAME+1];
    char *res, *ss, *ptr;
    FILE *file;

    // read input
    fgets(s,MAX_INPUT_SIZE,stdin);

    // verifications
    if(uid==-1){
        printf("  You're not logged in to post messages!\n");
        return;
    }
    if(gid==-1) {
        printf("  First select the group\n");
        return;
    }

    // create socket
    fd_tcp=socket(AF_INET, SOCK_STREAM, 0);
    if(fd_tcp==-1) exit(1);
    // connect client socket to server socket
    n_tcp=connect(fd_tcp,res_tcp->ai_addr, res_tcp->ai_addrlen);
    if(n_tcp==-1) exit(1);

    count = sscanf(s, " \"%[^\"]\" %s", text, filename);

    if(!is_valid_text(text)) {
        printf("   Invalid text length (max is 240)\n");
        return;
    }

    if(count==1) {
        sprintf(s, "PST %05d %02d %ld %s\n", uid, gid, strlen(text), text);
        // send message in correct format
        send_tcp(s, strlen(s));
        // check result
        res = receive_tcp();
        if(strcmp(res, "RPT NOK\n")==0)
            printf("  Error\n");
        else if(strstr(res, "RPT")!=NULL){
            sscanf(res, "RPT %d", &mid);
            printf("  Posted message number %d to group %d\n", mid, gid);
        }
    }
    else if(count==2) {
        if(!is_valid_fname(filename)) {
            printf("  That's not a valid file name\n");
            close(fd_tcp); // close socket
            return;
        }
        // open file
        file = fopen(filename, "r");
        if (file == NULL) {
            printf("  Error opening file\n");
            close(fd_tcp); // close socket
            return;
        }
        // get file size
        fseek(file, 0L, SEEK_END);
        filesize = ftell(file);

        len = 13+get_digits(strlen(text))+1+strlen(text)+1+strlen(filename)+1+get_digits(filesize)+1+filesize+1;
        ss = malloc(sizeof(char)*len);
        sprintf(ss, "PST %05d %02d %ld %s %s %ld ", 
                uid, gid, strlen(text), text, filename, filesize);
        
        // get file data
        ptr = ss+len-filesize-1;
        fseek(file, 0L, SEEK_SET);
        fread(ptr, sizeof(char), filesize, file);
        fclose(file);
        ss[len-1] = '\n';

        // send message in correct format
        send_tcp(ss, len);
        // check result
        res = receive_tcp();
        if(strcmp(res, "RPT NOK\n")==0)
            printf("  Error\n");
        else if(strstr(res, "RPT")!=NULL){
            sscanf(res, "RPT %d", &mid);
            printf("  Posted message number %d to group %d\n", mid, gid);
        }

        free(ss);
    }
    else 
        printf("  Wrong number of arguments (and don't forget the quotes around the text)\n");

    close(fd_tcp); // close socket
}

/* -------------------------------------------------
    retrieve/r MID
    obtem ateh MID mensagens nao lidas do grupo
------------------------------------------------- */
void retrieve() {
	int m, count, tsize, fsize, space, offset;
    char s[MAX_INPUT_SIZE], fname[MAX_FNAME+1], mid[5], uid_read[6], stsize[4], sfsize[11];
    char *data;

    // read line
    fgets(s,MAX_INPUT_SIZE,stdin);
    count = sscanf(s, " %d", &m);
    if(count!=1) {
        printf("  Wrong number of arguments\n");
        return;
    }
    // verifications
    if(uid==-1){
        printf("  You're not logged in to view messages!\n");
        return;
    }
    if(gid==-1) {
        printf("  First select the group\n");
        return;
    }

    // create socket
    fd_tcp=socket(AF_INET, SOCK_STREAM, 0);
    if(fd_tcp==-1) exit(1);
    // connect client socket to server socket
    n_tcp=connect(fd_tcp,res_tcp->ai_addr, res_tcp->ai_addrlen);
    if(n_tcp==-1) exit(1);
    // send message in correct format
    sprintf(s, "RTV %05d %02d %04d\n", uid, gid, m);
    send_tcp(s, strlen(s));

    // receive message
    char res[10];
    receive_tcp_len(res, 7); // RRT XXX -> 7 chars


    if(strncmp(res, "RRT EOF", 7)==0) {
        receive_tcp_len(res+7, 1); // receive \n
        if(strncmp(res, "RRT EOF\n", 8)==0)
            printf("  There are no messages to retrieve\n");
        close(fd_tcp);
        return;
    }
    if(strncmp(res,"RRT NOK", 7)==0) {
        receive_tcp_len(res+7, 1); // receive \n
        if(strncmp(res, "RRT NOK\n", 8)==0)
            printf("  Error\n");
        close(fd_tcp);
        return;
    }
    if(strncmp(res,"RRT OK ", 7)!=0) {
        printf("  Error\n");
        close(fd_tcp);
        return;
    }

    // read number of messages
    receive_tcp_space(&res[7]);
    sscanf(res, "RRT OK %d", &m);
    printf("  %d message(s) retrieved:\n", m);
    space = 0;
    offset = 0;

    for(int i=0; i<m; i++) {
        // read space for most iterations, except:
        // first iteration and those after a message with no file
        if(space==1)
            receive_tcp_len(s, space);
        
        receive_tcp_len(&mid[offset], 5-offset); // MID
        printf("  %c%c%c%c", mid[0],mid[1], mid[2],mid[3]);

        receive_tcp_len(uid_read, 6); // UID

        receive_tcp_space(stsize); // Tsize
        sscanf(stsize, "%d ", &tsize);

        // read text plus space plus / or start of next msg
        char text[tsize+3];
        receive_tcp_len(text, tsize+2);
        text[tsize] = '\0';

        printf(" - \"%s\"", text);

        // there's a file
        if(text[tsize+1]=='/') {
            receive_tcp_len(&text[tsize+2], 1); // space

            receive_tcp_space(fname); // Fname
            printf("; file stored: %s\n", fname);

            receive_tcp_space(sfsize); // Fsize
            sscanf(sfsize, "%d ", &fsize);

            // read from socket the file
            data = calloc(fsize, sizeof(char));
            receive_tcp_len(data, fsize);

            // create received file
		    FILE * file = fopen(fname, "wb");
            fwrite(data, sizeof(char), fsize, file);
            fclose(file);
            free(data);
        
            space = 1; // need to read space on next message
            offset = 0;
        }
        // no file
        else{
            printf("\n");
            space = 0; // already read, text[tsize]
            offset = 1;
            mid[0] = text[tsize+1];
        }
    }
    close(fd_tcp);
}

/* -------------------------------------------------
    showuid/su 
    print uid of logged in user
------------------------------------------------- */
void showuid() {
    if(uid==-1)
        printf("  Not logged in\n");
    else
        printf("  Currently user with id = %d is logged in\n", uid);
}
/* -------------------------------------------------
    showgid/sg 
    print gid of selected group
------------------------------------------------- */
void showgid() {
    if(gid==-1)
        printf("  No group is selected\n");
    else
        printf("  Currently selected group with id = %d\n", gid);
}


/********
 * MAIN *
 ********/

int main(int argc, char**argv) {
    char dsip[MAX_INPUT_SIZE]= "127.0.0.1"; // default
    char dsport[MAX_INPUT_SIZE]= "58032"; // default
    char op[MAX_INPUT_SIZE];
    int errcode;

    /* Initial arguments 
     * [-n DSIP] [-p DSport] */
    if(argc==1);
    else if(argc==3) {
        if(strcmp("-n",argv[1])==0)
            strcpy(dsip, argv[2]);
        else if(strcmp("-p", argv[1])==0)
            strcpy(dsport, argv[2]);
        else {
            printf("Error: wrong arguments\n");
            exit(1);
        }
    }
    else if(argc==5) {
        if(strcmp("-n",argv[1])==0) {
            strcpy(dsip, argv[2]);
            if(strcmp("-p", argv[3])==0)
                strcpy(dsport, argv[4]);
            else {
                printf("Error: wrong arguments\n");
                exit(1);
            }
        }
        else if(strcmp("-p", argv[1])==0) {
            strcpy(dsport, argv[2]);
            if(strcmp("-n", argv[3])==0)
                strcpy(dsip, argv[4]);
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
    fd_udp=socket(AF_INET, SOCK_DGRAM, 0); // create socket
    if(fd_udp==-1) exit(1);

    memset(&hints_udp,0,sizeof hints_udp);
    hints_udp.ai_family=AF_INET;
    hints_udp.ai_socktype=SOCK_DGRAM;

    errcode=getaddrinfo(dsip,dsport,&hints_udp,&res_udp);
    if(errcode!=0) exit(1);

    // TCP ----------------------------------------------------------
    memset(&hints_tcp,0,sizeof hints_tcp);
    hints_tcp.ai_family=AF_INET;
    hints_tcp.ai_socktype=SOCK_STREAM;

    errcode=getaddrinfo(dsip,dsport,&hints_tcp,&res_tcp);
    if(errcode!=0) exit(1);
    
    // --------------------------------------------------------------

    /* Read Inputs */
    while(1){
        printf("> ");
        scanf("%s", op);
        if (strcmp(op,"reg")==0)
            reg();
        else if (strcmp(op, "unregister")==0 || strcmp(op, "unr")==0)
            unregister();
        else if (strcmp(op, "login")==0)
            login();
        else if (strcmp(op, "logout")==0)
            logout();
        else if (strcmp(op, "showuid")==0 || strcmp(op, "su")==0)
            showuid();
        else if (strcmp(op, "groups")==0 || strcmp(op, "gl")==0)
            groups();
        else if (strcmp(op, "subscribe")==0 || strcmp(op, "s")==0)
            subscribe();
        else if (strcmp(op, "unsubscribe")==0 || strcmp(op, "u")==0)
            unsubscribe();
        else if (strcmp(op, "my_groups")==0 || strcmp(op, "mgl")==0)
            mgl();
        else if (strcmp(op, "select")==0 || strcmp(op, "sag")==0)
            sag();
        else if (strcmp(op, "showgid")==0 || strcmp(op, "sg")==0)
            showgid();
        else if (strcmp(op, "showgid")==0 || strcmp(op, "sg")==0)
            printf("%02d\n", gid);
        else if (strcmp(op, "ulist")==0 || strcmp(op, "ul")==0)
            ulist();
        else if (strcmp(op, "post")==0)
            post();
        else if (strcmp(op, "retrieve")==0 || strcmp(op, "r")==0)
            retrieve();
        else if (strcmp(op, "exit")==0){
            logout();
            freeaddrinfo(res_tcp);
            freeaddrinfo(res_udp);
            close(fd_udp);
            exit(0);
        }
        else
            printf("  Unknown command\n");
    }

}
