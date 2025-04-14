/* libcom.h: Header file for the .com standard library.
 * Copyright 2019 by Brian Raiter
 * Copying and distribution of this file, with or without
 * modification, are permitted in any medium without royalty provided
 * the copyright notice and this notice are preserved. This file is
 * offered as-is, without any warranty.
 */

#ifndef _libcom_h_
#define _libcom_h_

extern int errno;

extern int open(char const *filename, int mode, int perm);
extern long read(int fd, void *buf, long size);
extern long write(int fd, void const *buf, long size);
extern int close(int fd);
extern void *brk(void const *p);
extern void __attribute__((noreturn)) exit(int code);
extern long strlen(char const *str);
extern int fprint(int fd, char const *str);

#define print(str) (fprint(1, (str)))

#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR   0x0002
#define O_CREAT  0x0040
#define O_EXCL   0x0080
#define O_TRUNC  0x0100
#define O_APPEND 0x0200

#define MAIN void __attribute__((section(".main"))) _main

#endif
