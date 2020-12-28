#pragma once

#if defined(_WIN32)
#  include <windows.h>
#else
#  include <iostream>
#endif

// Display a message and exit the program
static void exitOnError(const char *msg) {
#if defined(_WIN32)
  MessageBox(NULL, msg, ENGINE_NAME, MB_ICONERROR);  // Pop-up for window user
#else
  std::cout << msg << std::endl;
#endif
  exit(EXIT_FAILURE);
}