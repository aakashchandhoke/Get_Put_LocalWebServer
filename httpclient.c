/* Generic */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/* Network */
#include <netdb.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 100


int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
}

// Get host information (used to establishConnection)
struct addrinfo *getHostInfo(char* host, char* port) 
{
  int r;
  struct addrinfo hints, *getaddrinfo_res;
  // Setup hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if ((r = getaddrinfo(host, port, &hints, &getaddrinfo_res))) 
  {
    fprintf(stderr, "[getHostInfo:21:getaddrinfo] %s\n", gai_strerror(r));
    return NULL;
  }
  return getaddrinfo_res;
}

// Establish connection with host
int establishConnection(struct addrinfo *info) 
{
  if (info == NULL) 
    return -1;
  int clientfd;
  for (;info != NULL; info = info->ai_next) 
  {
    if ((clientfd = socket(info->ai_family,
                           info->ai_socktype,
                           info->ai_protocol)) < 0) 
    {
      perror("[establishConnection:35:socket]");
      continue;
    }

    if (connect(clientfd, info->ai_addr, info->ai_addrlen) < 0) 
    {
      close(clientfd);
      perror("[establishConnection:42:connect]");
      continue;
    }

    freeaddrinfo(info);
    return clientfd;
  }

  freeaddrinfo(info);
  return -1;
}

// Send GET request
void GET(int clientfd, char *path) 
{
  char req[1000] = {0};
  sprintf(req, "GET %s HTTP/1.0\r\n\r\n", path);
  send(clientfd, req, strlen(req), 0);
}

int main(int argc, char **argv) 
{
  int clientfd;
  char buf[BUF_SIZE];
  if (argc != 5) 
  {
    fprintf(stderr, "USAGE: ./client <hostname> <port> <GET/PUT> <request path>\n");
    return 1;
  }
  
  char ip[1024];
  //convert hostname of any web url to its corresponding ip
  char url[1024];
  strcpy(url,argv[1]);
  hostname_to_ip(url, ip);

  //Establish connection with <hostname>:<port>
  clientfd = establishConnection(getHostInfo(ip, argv[2]));
  if (clientfd == -1) 
  {
    fprintf(stderr,
            "[main:73] Failed to connect to: %s:%s%s \n",
            argv[1], argv[2], argv[4]);
    return 3;
  }
  if(strcmp(argv[3],"GET")==0)
  {
    // Send GET request > stdout
    if(strcmp(argv[2],"80")!=0)
    {
      GET(clientfd, argv[4]);
      while (recv(clientfd, buf, BUF_SIZE, 0) > 0) 
      {
        fputs(buf, stdout);
        memset(buf, 0, BUF_SIZE);
      }
    }
    else if(strcmp(argv[2],"80")==0)
    {
      char buff[1024] = "GET / HTTP/1.0\r\nHost: ";
      strcat(buff,argv[1]);
      strcat(buff,"\r\n\r\n");
      send(clientfd, buff, strlen(buff), 0);
      while (recv(clientfd, buf, BUF_SIZE, 0) > 0) 
      {
        fputs(buf, stdout);
        memset(buf, 0, BUF_SIZE);
      }
    }
  }
  else if(strcmp(argv[3],"PUT")==0 && strcmp(argv[2],"80")!=0)
  {
    printf("put\n");
    FILE *fptr;
    char sentence[1000];

    fptr = fopen(argv[4], "w");
   
    printf("Enter a sentence:\n");
    //scanf("%s",sentence);
    fgets(sentence,1000,stdin);

    fprintf(fptr,"%s", sentence);
    char req[1000] = {0};
    sprintf(req, "PUT %s HTTP/1.0\r\n\r\n", argv[4]);
    send(clientfd, req, strlen(req), 0);
    while (recv(clientfd, buf, BUF_SIZE, 0) > 0) 
    {
      fputs(buf, stdout);
      memset(buf, 0, BUF_SIZE);
    }
    fclose(fptr);
  }
  else if(strcmp(argv[3],"PUT")==0 && strcmp(argv[2],"80")==0)
  {
    printf("Can only be able to create files in local servers\n");
  }
  close(clientfd);
  return 0;
}
