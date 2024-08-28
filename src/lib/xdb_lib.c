/******************************************************************************
* Copyright (c) 2024-present JC Wang. All rights reserved
*
*   https://crossdb.org
*   https://github.com/crossdb-org/crossdb
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
******************************************************************************/

#include "xdb_lib.h"
#include "xdb_str.c"
#include "xdb_file.c"
#include "xdb_mmap.c"
#include "xdb_objm.c"
#include "xdb_bmp.c"
#include "xdb_time.c"
#include "xdb_dynbuf.c"
#include "xdb_thread.c"
#if (XDB_ENABLE_SERVER == 1)
#include "xdb_sock.c"
#endif

#if 0
XDB_STATIC void 
xdb_hexdump (const void *addr, int len)
{
	int i, j;
	const uint8_t *ptr = addr;
	xdb_print ("hexdump addr %p len %d\n", addr, len);
	for (i = 0; i < len; i+=16, ptr+=16) {
		xdb_print ("%08x  ", i);
		for (j = 0; j < 16; ++j) {
			if (i + j < len) {
				xdb_print ("%02x ", ptr[j]);
			} else {
				xdb_print ("   ");
			}
			if (j == 7) xdb_print (" ");
		}
		xdb_print (" |");
		for (j = 0; j < 16; ++j) {
			xdb_print ("%c", (i+j>=len)?' ':(isprint(ptr[j])?ptr[j]:'.'));
			if (j == 7) xdb_print (" ");
		}
		xdb_print ("|\n");
	}
}
#endif

/*
	sigaction(SIGPIPE, &(struct sigaction){{SIG_IGN}}, NULL)
	signal(SIGPIPE,SIG_IGN);
*/
#if (XDB_ENABLE_SERVER == 1)
XDB_STATIC int 
xdb_signal_block (int signum)
{
#ifndef _WIN32
	sigset_t signal_mask; 
	sigemptyset(&signal_mask); 
	sigaddset(&signal_mask, signum); 
	if (pthread_sigmask(SIG_BLOCK, &signal_mask, NULL) == -1) {
		perror ("SIGPIPE");
		return -1;
	}
#endif
	return 0;
}
#endif

XDB_STATIC uint64_t 
xdb_strcasehash(const char *key, int len)
{
	XDB_BUF_DEF(str, 2048);
	if (len > 2048) {
		XDB_BUF_ALLOC (str, len);
		if (NULL == str) {
			return -1;
		}
	}
	for (int i = 0; i< len; ++i) {
		str[i] = tolower(key[i]);
	}
	
	uint64_t hash = xdb_wyhash (str, len);

	XDB_BUF_FREE(str);

	return hash;
}
