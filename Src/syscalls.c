/**
 ******************************************************************************
 * @file      syscalls.c
 * @author    Auto-generated by STM32CubeIDE
 * @brief     STM32CubeIDE Minimal System calls file
 *
 *            For more information about which c-functions
 *            need which of these lowlevel functions
 *            please consult the Newlib libc-manual
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2020-2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes */
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <string.h>


/* Variables */
extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));


char *__env[1] = { 0 };
char **environ = __env;


/* Functions */
void initialise_monitor_handles()
{
}

int _getpid(void)
{
  return 1;
}

int _kill(int pid, int sig)
{
  (void)pid;
  (void)sig;
  errno = EINVAL;
  return -1;
}

void _exit (int status)
{
  _kill(status, -1);
  while (1) {}    /* Make sure we hang here */
}

__attribute__((weak)) int _read(int file, char *ptr, int len)
{
  (void)file;
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    *ptr++ = (char)__io_getchar();
  }

  return len;
}

void force_print(char* ptr, unsigned len, uint32_t r)
{
  unsigned DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  if (r)
  {
    __io_putchar('\r');
  }
}

__attribute__((weak)) int _write(int file, char *ptr, int len)
{
  // if (exc_num)
  // {
  //   if (IS_MAIN_DEBUGGING || IS_INTR_DEBUGGING)
  //   {
  //     sprintf(msg, "exc %lu interrupted %lu\n", exc_num, get_debugging_intr());
  //     force_print(msg, strlen(msg), 1);
  //     abort();
  //   }
  //   // sprintf(msg, "exc %lu is debugging\n", exc_num);
  //   // force_print(msg, strlen(msg), 1);
  //   //set_debugging_intr(exc_num);
  // }
  // else
  // {
  //   set_main_debugging();
  // }
  (void)file;
  int DataIdx;
  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  __io_putchar('\r');
  // if (exc_num) {
  //   clear_debugging_intr();
  // } else {
  //   clear_main_debugging();
  // }
  return len;
}

int _close(int file)
{
  (void)file;
  return -1;
}


int _fstat(int file, struct stat *st)
{
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  (void)file;
  return 1;
}

int _lseek(int file, int ptr, int dir)
{
  (void)file;
  (void)ptr;
  (void)dir;
  return 0;
}

void clear(void)
{
  __io_putchar(27);
  __io_putchar(91);
  __io_putchar(50);
  __io_putchar(74);
}

void reset(void)
{
  __io_putchar(27);
  __io_putchar(91);
  __io_putchar(72);
}

void reset_terminal(void)
{
  clear();
  reset();
}

int _open(char *path, int flags, ...)
{
  (void)path;
  (void)flags;
  /* Pretend like we always fail */
  //__io_putchar('\r');
 clear();
 reset();
  return -1;
}

int _wait(int *status)
{
  (void)status;
  errno = ECHILD;
  return -1;
}

int _unlink(char *name)
{
  (void)name;
  errno = ENOENT;
  return -1;
}

int _times(struct tms *buf)
{
  (void)buf;
  return -1;
}

int _stat(char *file, struct stat *st)
{
  (void)file;
  st->st_mode = S_IFCHR;
  return 0;
}

int _link(char *old, char *new)
{
  (void)old;
  (void)new;
  errno = EMLINK;
  return -1;
}

int _fork(void)
{
  errno = EAGAIN;
  return -1;
}

int _execve(char *name, char **argv, char **env)
{
  (void)name;
  (void)argv;
  (void)env;
  errno = ENOMEM;
  return -1;
}
