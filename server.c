#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>

char webpage []=
"HTTP/1.1 200 OK\r\n"
"Contect-Type: text/html; charset=UTF -8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>WTFun</title>\r\n"
"<style>body {background-color: #FFFF00  } </style></head>\r\n"
"<body><center><h1>Hello world !</h1><br>\r\n"
"<form action=\"/upload?path=Debug/\" method=\"post\" enctype=\"multipart/form-data\">"
"<input type=\"file\" name=\"file\" />"
"<input type=\"submit\" value=\"Upload\" /></form>"
"<img src =\"computer.jpg\"></center></body></html>\r\n";

char upload[]=
"HTTP/1.1 200 OK\r\n"
"Contect-Type: text/html; charset=UTF -8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>WTFun</title>\r\n"
"<style>body {background-color: #FFFF00  } </style></head>\r\n"
"<body><center><h1>upload successfull !</h1><br>\r\n";
  char buf[1000000];

/*void LOAD(char buffer[strlen(buf)+1],int fd,int ret)   {
    char *tmp = strstr (buffer,"filename");
    if (tmp == 0 ) return;
    char filename[strlen(buf)+1],data[strlen(buf)+1],location[strlen(buf)+1];
    memset (filename,'\0',strlen(buf));
    memset (data,'\0',strlen(buf));
    memset (location,'\0',strlen(buf));


    char *a,*b;
    a = strstr(tmp,"\"");
    b = strstr(a+1,"\"");
    strncpy (filename,a+1,b-a-1);
    strcat (location,"upload/");
    strcat (location,filename);

    a = strstr(tmp,"\n");
    b = strstr(a+1,"\n");
    a = strstr(b+1,"\n");
    b = strstr(a+1,"---------------------------");

    int download = open(location,O_CREAT|O_WRONLY|O_TRUNC|O_SYNC,S_IRWXO|S_IRWXU|S_IRWXG);

    char t[strlen(buf)+1];
    int last_write,last_ret;
    if (b != 0)
    write(download,a+1,b-a-3);
    else {
    int start = (int )(a - &buffer[0])+1;
    last_write = write(download,a+1,ret -start -61);
    last_ret = ret;
    memcpy (t,a+1+last_write,61);

    while ((ret=read(fd, buffer,strlen(buf)))>0) {
        write(download,t,61);
        last_write = write(download,buffer,ret - 61);
        memcpy (t,buffer+last_write,61);
        last_ret = ret;
        if (ret!=8096)
            break;
    }
    }

    close(download);
    printf ("UPLOAD FILE NAME :%s\n",filename);
    return ;
}*/

int main (int argc ,char argv[])
{
    struct sockaddr_in server_addr,client_addr;
    socklen_t sin_len=sizeof(client_addr );
    int fd_server ,fd_client ;
  
    char file_name[256];
    int fdimg ;
    int on=1;

    fd_server =socket(AF_INET,SOCK_STREAM,0);
    if (fd_server<0)
    {
        perror ("socket");
        exit(1);
    }
setsockopt(fd_server,SOL_SOCKET,SO_REUSEADDR,&on, sizeof(int ));

server_addr.sin_family=AF_INET;
server_addr.sin_addr.s_addr=INADDR_ANY;
server_addr.sin_port=htons(8080);

if(bind(fd_server,(struct sockaddr *) &server_addr,sizeof(server_addr))==-1)
{
perror("bind ");
close (fd_server);
exit(1);
}
if(listen(fd_server,10)==-1)
{
 perror("listen");
 close(fd_server);
exit(1);
}
while(1)
{

fd_client=accept(fd_server, (struct sockaddr *) &client_addr,&sin_len);

if(fd_client ==-1)
{
    perror("Connection failed.....\n");
    continue;
}
printf("Got client connection.....\n");

if(!fork())
{
 close(fd_server);
 memset(buf,0,1000000);
read(fd_client,buf,999999);

printf("%s\n",buf);

if(!strncmp(buf,"GET /computer.jpg",17))
{
 fdimg =open("./computer.jpg",O_RDONLY);
 sendfile(fd_client,fdimg,NULL,60000);
 close(fdimg);
}
else if (!strncmp(buf,"POST /upload",12)){
  write(fd_client,upload ,sizeof(upload) -1);
        char *tmp ,  fname[2048];
		int i = 0;
        char *tmp1;
        char body[1000000];
		char  path[30] = "./upload/";
	    tmp1 = strstr(buf,"filename=");
	    strcpy(fname, tmp1+10);
	    for(int i = 0; i < strlen(fname); i++)
	    {
		    if(fname[i]=='"')
		    {
			    fname[i] = 0;
			    break;
		    }
	   }
		strcat(path,fname);
        tmp = strstr(tmp1, "\n\r\n")+3;
		FILE *fp = fopen(path,"a+");
        while (*tmp!='\n'||*(tmp+1)!='-'||*(tmp+2)!='-')
        {
            fprintf(fp,"%c",*tmp);
            tmp++; 
        }
        fclose(fp);
        }

else 
     write(fd_client,webpage ,sizeof(webpage) -1);
     close(fd_client);
     printf("closing...\n");
     exit(0);
}
close(fd_client);
}
    return 0;
}