#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstdio>

const char *inet_ntop(int, const void *, char *, socklen_t);

int main(int argc, char *argv[]) {
  using namespace std;
  freopen("output.txt", "w", stdout);
  freopen("error.txt", "w", stderr);

  WSADATA wsaData;
  int wsaerr;

  // Using MAKEWORD macro, Winsock version request 2.2
  wsaerr = WSAStartup(MAKEWORD(2, 2), &wsaData);

  /* Confirm that the WinSock DLL supports 2.2.*/
  /* Note that if the DLL supports versions greater    */
  /* than 2.2 in addition to 2.2, it will still return */
  /* 2.2 in wVersion since that is the version we      */
  /* requested.                                        */

  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.*/

    fprintf(stdout, "The dll do not support the Winsock version %u.%u!\n",
            LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
    WSACleanup();
    return 0;
  } else {
    fprintf(stdout, "The dll supports the Winsock version %u.%u!\n",
            LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
    fprintf(stdout, "The highest version this dll can support: %u.%u\n",
            LOBYTE(wsaData.wHighVersion), HIBYTE(wsaData.wHighVersion));
  }

  /* The next task... */
  SOCKET m_socket;
  m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_socket == INVALID_SOCKET) {
    fprintf(stdout, "Error at socket(): %ld\n", WSAGetLastError());
    WSACleanup();
    return 0;
  } else {
    fprintf(stdout, "socket() is OK!\n");
  }

  struct addrinfo hints, *res, *p;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;  // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;

  int status;
  char ipstr[INET6_ADDRSTRLEN];
  if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    WSACleanup();
    return 1;
  }

  fprintf(stdout, "IP addresses for %s:\n\n", argv[1]);

  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    char *ipver;

    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET) {  // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    } else {  // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    inet_ntop(p->ai_family, addr, (PSTR)ipstr, sizeof ipstr);
    fprintf(stdout, " %s: %s\n", ipver, ipstr);
  }
  freeaddrinfo(res);  // free the linked list

  WSACleanup();
  return 0;
}

const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt) {
  if (af == AF_INET) {
    struct sockaddr_in in;
    memset(&in, 0, sizeof(in));
    in.sin_family = AF_INET;
    memcpy(&in.sin_addr, src, sizeof(struct in_addr));
    getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in), dst, cnt,
                NULL, 0, NI_NUMERICHOST);
    return dst;
  } else if (af == AF_INET6) {
    struct sockaddr_in6 in;
    memset(&in, 0, sizeof(in));
    in.sin6_family = AF_INET6;
    memcpy(&in.sin6_addr, src, sizeof(struct in_addr6));
    getnameinfo((struct sockaddr *)&in, sizeof(struct sockaddr_in6), dst, cnt,
                NULL, 0, NI_NUMERICHOST);
    return dst;
  }
  return NULL;
}
