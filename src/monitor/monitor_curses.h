#ifndef CL_MONITOR_CURSES_H
#define CL_MONITOR_CURSES_H

#if defined(__sun)
/* OmniOS/illumos: ncurses header is available as ncurses/ncurses.h -> curses.h */
  #include <ncurses/ncurses.h>
#else
  /* Linux + BSD */
  #include <ncurses.h>
#endif

#endif

