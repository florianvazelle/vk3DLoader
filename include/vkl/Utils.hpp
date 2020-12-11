#pragma once

#if defined(_WIN32)
#  include <winuser.h>
#else
#  include <iostream>
#endif

static void exitOnError(const char *msg) {
#if defined(_WIN32)
  MessageBox(NULL, msg, ENGINE_NAME, MB_ICONERROR);
#else
  std::cout << msg << std::endl;
#endif
  exit(EXIT_FAILURE);
}