#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <time.h>

#include <cstdio>
#include <iostream>
#include <random>
#include <string>

int notMain(int argc, char* argv[]) {
  using namespace std;
  freopen("output.txt", "w", stdout);
  freopen("error.txt", "w", stderr);

  std::cout << "Starting up..." << std::endl;
  std::cerr << "Starting up..." << std::endl;

  SDL_Init(SDL_INIT_EVERYTHING);
  if (SDLNet_Init() < 0) {
  std:
    std::cerr << "Failed to init net:  " << SDLNet_GetError() << std::endl;
  }

  const char* host = "date.jsontest.com";
  IPaddress ip;
  std::cout << "Resolving host:  " << host << std::endl;
  SDLNet_ResolveHost(&ip, host, 80);
  fprintf(stdout, "Resolved host to %d.%d.%d.%d port %hu\n",
          ip.host >> 24, (ip.host >> 16) & 0xff, (ip.host >> 8) & 0xff,
          ip.host & 0xff, ip.port);
  fflush(stdout);

  TCPsocket sock = SDLNet_TCP_Open(&ip);
  // if (!sock) {
    std::cerr << "Failed to connect socket:  " << SDLNet_GetError()
              << std::endl;
  // } else {
    std::cout << "Opened socket:  " << sizeof sock << std::endl;
  // }

  char* http;
  sprintf(http, "GET / HTTP/1.1\nHost:  %s\n\n", host);
  std::cout << "Command:  " << http << std::endl;
  SDLNet_TCP_Send(sock, http, strlen(http) + 1);

  char text[10000];
  while (SDLNet_TCP_Recv(sock, http, 10000)) {
    std::cout << "Response:  [" << text << "]" << std::endl;
  }

  SDLNet_TCP_Close(sock);
  SDLNet_Quit();
  SDL_Quit();

  return 0;
}
