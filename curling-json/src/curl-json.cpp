#include <stdio.h>
#include <winsock.h>

#include <cstdio>
#include <iostream>
#include <string>

#define PORT 80
#define USER_AGENT "winsock"
#define HOSTNAME "date.jsontest.com"

char *build_command_query(char *, char *);

int main(int argc, char *argv[]) {
  using namespace std;
  freopen("output.txt", "w", stdout);
  freopen("error.txt", "w", stderr);

  int client_socket;
  char *host;
  struct hostent *hent;
  int iplen = 15;  // XXX.XXX.XXX.XXX
  struct sockaddr_in *remote;
  int tmpres;
  char *get;
  char buf[BUFSIZ + 1];

  fprintf(stdout, "Socket client example\n");
  fflush(stdout);

  // create tcp socket
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);

  client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) {
    fprintf(stdout, "client_socket = %d\n", client_socket);
    fflush(stdout);
    fprintf(stderr, "Can't create TCP socket\n");
    fflush(stderr);
    exit(1);
  }

  // get host ip
  host = HOSTNAME;
  fprintf(stdout, "Host:  %s\n", host);
  fflush(stdout);
  char *ip = (char *)malloc(iplen + 1);
  memset(ip, 0, iplen + 1);
  if ((hent = gethostbyname(host)) == NULL) {
    fprintf(stderr, "Can't get IP");
    fflush(stderr);
    exit(1);
  }
  ip = inet_ntoa(*(struct in_addr *)*hent->h_addr_list);
  fprintf(stdout, "The IP: %s\n", ip);
  fflush(stdout);

  // setup remote socket
  remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
  remote->sin_family = AF_INET;
  fprintf(stdout, "s_addr:%d\n", remote->sin_addr.s_addr);
  fflush(stdout);
  remote->sin_addr.s_addr = inet_addr(ip);
  remote->sin_port = htons(PORT);
  fprintf(stdout, "s_addr:%d\n", remote->sin_addr.s_addr);
  fflush(stdout);

  // have to read
  // http://msdn.microsoft.com/en-us/library/windows/desktop/ms737625(v=vs.85).aspx

  // connect socket
  if (connect(client_socket, (struct sockaddr *)remote,
              sizeof(struct sockaddr)) == SOCKET_ERROR) {
    closesocket(client_socket);
    fprintf(stderr, "Could not connect");
    WSACleanup();
    exit(1);
  }

  // prepare query
  get = build_command_query(host, "/");
  fprintf(stdout, "query: \n%s\n", get);
  fflush(stdout);

  // Send the query to the server
  int sent = 0;
  while (sent < strlen(get)) {
    tmpres = send(client_socket, get + sent, strlen(get) - sent, 0);
    if (tmpres == -1) {
      fprintf(stderr, "Can't send query");
      fflush(stderr);
      exit(1);
    }
    sent += tmpres;
  }

  // now it is time to receive the page
  memset(buf, 0, sizeof(buf));
  int htmlstart = 0;
  char *htmlcontent;
  while ((tmpres = recv(client_socket, buf, BUFSIZ, 0)) > 0) {
    if (htmlstart == 0) {
      /* Under certain conditions this will not work.
       * If the \r\n\r\n part is splitted into two messages
       * it will fail to detect the beginning of HTML content
       */
      htmlcontent = strstr(buf, "\r\n\r\n");
      if (htmlcontent != NULL) {
        htmlstart = 1;
        htmlcontent += 4;
      }
    } else {
      htmlcontent = buf;
    }
    if (htmlstart) {
      fprintf(stdout, htmlcontent);
      fflush(stdout);
    }

    memset(buf, 0, tmpres);
  }
  if (tmpres < 0) {
    fprintf(stderr, "Error receiving data");
    fflush(stderr);
  }

  free(get);
  free(remote);
  closesocket(client_socket);
  WSACleanup();

  fprintf(stdout, "Program end");
  fflush(stdout);

  return 0;
}

char *build_command_query(char *host, char *page) {
  char *query;
  char *getpage = page;
  char *tpl = "GET /%s HTTP/1.0\nHost:  %s\nUser-Agent:  %s\n\n";
  if (getpage[0] == '/') {
    getpage = getpage + 1;
    fprintf(stderr, "Removing leading \"/\", converting %s to %s\n", page,
            getpage);
    fflush(stderr);
  }
  query = (char *)malloc(strlen(host) + strlen(getpage) + strlen(USER_AGENT) +
                         strlen(tpl) - 5);
  sprintf(query, tpl, getpage, host, USER_AGENT);
  return query;
}
