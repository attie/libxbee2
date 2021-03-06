/*
  libxbee - a C library to aid the use of Digi's Series 1 XBee modules
            running in API mode (AP=2).

  Copyright (C) 2009  Attie Grande (attie@attie.co.uk)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "plugin.h"

int myInit(struct xbee *xbee, void *arg, void **pluginData) {
	int ret;
	printf("Hello!! from myInit() xbee=%p\n", xbee);
	ret = xbee_modeSet(xbee,"myplugin");
	printf("xbee_modeSet() returned %d\n", ret);
	*pluginData = (void*)15;
	return 0;
}

void myThread(struct xbee *xbee, void *arg, void **pluginData) {
	int i = 5;
	for (; i; i--) {
		printf("Hello!! from myThread() xbee=%p i=%d pluginData=%d\n", xbee, i, (int)*pluginData);
		sleep(1);
	}
}

int myRemove(struct xbee *xbee, void *arg, void **pluginData) {
	printf("Bye bye from myRemove()!\n");
}

/* ######################################################################### *\
 * ######################################################################### *
\* ######################################################################### */

int xbee_sG_atRx(struct xbee *xbee, struct xbee_pktHandler *handler, char isRx, struct bufData **buf, struct xbee_con *con, struct xbee_pkt **pkt) {
	int ret = XBEE_ENONE;
	
	if (!xbee)         return XBEE_ENOXBEE;
	if (!handler)      return XBEE_EMISSINGPARAM;
	if (!isRx)         return XBEE_EINVAL;
	if (!buf || !*buf) return XBEE_EMISSINGPARAM;
	if (!con)          return XBEE_EMISSINGPARAM;
	if (!pkt || !*pkt) return XBEE_EMISSINGPARAM;
	
	if ((*buf)->len < 1) {
		ret = XBEE_ELENGTH;
		goto die1;
	}
	
	if ((*buf)->len < 5) {
		ret = XBEE_ELENGTH;
		goto die1;
	}
	
	con->frameID_enabled = 1;
	con->frameID = (*buf)->buf[1];
	
	(*pkt)->atCommand[0] = (*buf)->buf[2];
	(*pkt)->atCommand[1] = (*buf)->buf[3];
	
	(*pkt)->status = (*buf)->buf[4];
	
	(*pkt)->datalen = (*buf)->len - 5;
	if ((*pkt)->datalen > 1) {
		void *p;
		if ((p = realloc((*pkt), sizeof(struct xbee_pkt) + (sizeof(unsigned char) * ((*pkt)->datalen)))) == NULL) {
			ret = XBEE_ENOMEM;
			goto die1;
		}
		(*pkt) = p;
	}
	if ((*pkt)->datalen) {
		memcpy((*pkt)->data, &((*buf)->buf[5]), (*pkt)->datalen);
		(*pkt)->data[(*pkt)->datalen] = '\0';
	}
	
	goto done;
die1:
done:
	return ret;
}

int xbee_sG_atTx(struct xbee *xbee, struct xbee_pktHandler *handler, char isRx, struct bufData **buf, struct xbee_con *con, struct xbee_pkt **pkt) {
	int ret = XBEE_ENONE;
	struct bufData *nBuf;
	int offset;
	void *p;
	
	if (!xbee)         return XBEE_ENOXBEE;
	if (!handler)      return XBEE_EMISSINGPARAM;
	if (isRx)          return XBEE_EINVAL;
	if (!buf || !*buf) return XBEE_EMISSINGPARAM;
	if (!con)          return XBEE_EMISSINGPARAM;
	if (!handler->conType || !handler->conType->txEnabled) return XBEE_EINVAL;
	if ((*buf)->len < 2) return XBEE_ELENGTH; /* need at least the 2 AT characters! */
	
	if ((nBuf = calloc(1, sizeof(struct bufData) + (sizeof(unsigned char) * (XBEE_MAX_PACKETLEN - 1)))) == NULL) {
		ret = XBEE_ENOMEM;
		goto die1;
	}
	
	nBuf->buf[0] = handler->conType->txID;
	if (con->frameID_enabled) {
		nBuf->buf[1] = con->frameID;
	}
	
	nBuf->len = 2 + (*buf)->len;
	if (nBuf->len > XBEE_MAX_PACKETLEN) {
		ret = XBEE_ELENGTH;
		goto die2;
	}
	memcpy(&(nBuf->buf[2]), (*buf)->buf, (*buf)->len);
	
	/* try (and ignore failure) to realloc buf to the correct length */
	if ((p = realloc(nBuf, sizeof(struct bufData) + (sizeof(unsigned char) * (nBuf->len - 1)))) != NULL) nBuf = p;
	
	*buf = nBuf;
	
	goto done;
die2:
	free(nBuf);
die1:
done:
	return ret;
}

/* ######################################################################### *\
 * ######################################################################### *
\* ######################################################################### */

/* this links pktHandlers together to form a Tx/Rx capable connection */
static struct xbee_conType conTypes[] = {
	ADD_TYPE_RXTX(0x88, 0x08, 0, "Local AT"),
	
	ADD_TYPE_TERMINATOR()
};

/* this links a packet ID with a function */
static struct xbee_pktHandler pktHandlers[] = {
	ADD_HANDLER(0x88, xbee_sG_atRx),      /* local AT */
	ADD_HANDLER(0x08, xbee_sG_atTx),      /* local AT */
	
	ADD_HANDLER_TERMINATOR()
};

/* this links the handlers and the connections into a 'mode' */
struct xbee_mode my_mode = {
	pktHandlers: pktHandlers,
	conTypes: conTypes,
	name: "myplugin"
};

/* this is a list of modes that the plugin provides */
struct xbee_mode *my_libxbee_mode[] = {
	&my_mode,
	NULL
};

/* this is the functionality that this plugin provides */
struct plugin_features libxbee_features = {
	.init = myInit,
	.thread = myThread,
	.remove = myRemove,
	.threadMode = PLUGIN_THREAD_RESPAWN,
	.xbee_modes = my_libxbee_mode
};
