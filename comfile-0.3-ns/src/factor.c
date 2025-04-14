/* factor.c: Compute the prime factors of given numbers.
 * Copyright 2019 by Brian Raiter <breadbox@muppetlabs.com>.
 * Copying and distribution of this file, with or without
 * modification, are permitted in any medium without royalty provided
 * the copyright notice and this notice are preserved. This file is
 * offered as-is, without any warranty.
 */

#include "libcom.h"

/* Return true if a character represents ASCII whitespace.
 */
static int iswhite(char ch)
{
    return ch == ' ' || (ch >= '\t' && ch <= '\r');
}

/* Translate an ASCII decimal representation of a positive integer.
 * The return value is zero if the string contains any non-digit
 * characters, or if the represented number is too large.
 */
static unsigned long atol(char const *str)
{
    unsigned long m, n;

    n = 0;
    while (*str) {
	if (*str < '0' || *str > '9')
	    return 0;
	m = n;
	n *= 10;
	if (n < m)
	    return 0;
	n += *str - '0';
	++str;
    }
    return n;
}

/* Print the ASCII decimal representation of a number.
 */
static void printn(unsigned long n)
{
    char buf[32];
    int i;

    i = 32;
    buf[--i] = '\0';
    for (;;) {
	buf[--i] = '0' + (n % 10);
	n /= 10;
	if (n == 0)
	    break;
    }
    print(buf + i);
}

/* Find the prime factors of a given number. The return value is the
 * number of factors stored in the provided array (which can be zero
 * if the given number is one or zero).
 */
static int factor(unsigned long number, unsigned long *factors)
{
    unsigned long factor, q, r;
    int step, n;

    n = 0;
    if (number <= 1)
	return 0;

    while (number % 2 == 0) {
	number /= 2;
	factors[n++] = 2;
    }
    while (number % 3 == 0) {
	number /= 3;
	factors[n++] = 3;
    }
    factor = 5;
    step = 2;
    for (;;) {
	q = number / factor;
	r = number % factor;
	if (r == 0) {
	    factors[n++] = factor;
	    number = q;
	} else {
	    if (q < factor)
		break;
	    factor += step;
	    step ^= 6;
	}
    }
    if (number > 1)
	factors[n++] = number;
    return n;
}

/* Process a single argument, outputting either the number's factors
 * or an error message as appropriate.
 */
static int processnumber(char const *input)
{
    unsigned long number, factors[64];
    int i, n;

    number = atol(input);
    if (!number)
	return 0;

    n = factor(number, factors);

    printn(number);
    print(":");
    for (i = 0 ; i < n ; ++i) {
	print(" ");
	printn(factors[i]);
    }
    print("\n");
    return 1;
}

/* Output an error message on stderr for an invalid argument.
 */
static void errmsg(char const *str)
{
    write(2, "factor: invalid positive integer: \"", 35);
    write(2, str, strlen(str));
    write(2, "\"\n", 2);
}

/* Parse and factor numbers from standard input.
 */
static int processinput(void)
{
    char buf[32];
    int len, n, r = 0;

    len = 0;
    for (;;) {
        n = read(0, buf + len, 1);
        if (n <= 0 && len == 0)
            break;
        if (len == 0) {
            if (n > 0 && !iswhite(*buf))
                len = 1;
        } else {
            if (n <= 0 || iswhite(buf[len])) {
                buf[len] = '\0';
                if (!processnumber(buf)) {
                    errmsg(buf);
                    r = 1;
                }
                len = 0;
            } else {
                ++len;
                if (len > (int)(sizeof buf)) {
                    buf[sizeof buf - 1] = '\0';
                    errmsg(buf);
                    r = 1;
                    len = 0;
                }
            }
        }
        if (n <= 0)
            break;
    }
    return r;
}

/* Parse and factor numbers from command line arguments.
 */
static int processargs(int argc, char *argv[])
{
    int i, r = 0;

    for (i = 1 ; i < argc ; ++i) {
        if (!processnumber(argv[i])) {
            errmsg(argv[i]);
            r = 1;
        }
    }
    return r;
}

MAIN(int argc, char *argv[])
{
    int exitcode;

    if (argc <= 1) {
        exitcode = processinput();
    } else if (argv[1][0] != '-') {
        exitcode = processargs(argc, argv);
    } else {
	print("Usage: factor.com [NUMBER ...]\n"
              "Print the prime factors of each given number.\n");
        exitcode = 0;
    }
    exit(exitcode);
}
