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

#include <xbee.h>

/* this node will issue a message once every second */

/* this holds onto the ID for the 64-bit data connection */
unsigned char conType;

/* the callback function */
void myCB(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt, void **userData) {
	/* what did the message say? */
	printf("They said this: %*s\n", (*pkt)->datalen, (*pkt)->data);
}

int main(int argc, char *argv[]) {
	int ret;
	void *p;
	
	/* this is our xbee instance... from 'user' space you don't have access to the struct */
	struct xbee *xbee;
	
	/* this is the connection we will make... again, you don't have access to the struct */
	struct xbee_con *con;
	struct xbee_conAddress addr;
	
	/* set the log level REALLY high, so we can see all the messages. default is 0 */
	xbee_logSetLevel(100);
	/* make a lixbee instance, and connect it to /dev/ttyUSB1 @ 57600 baud
	   you don't have to keep hold of the returned xbee, in which case you can pass NULL and the most recently started instance will be used! */
	if ((ret = xbee_setup("/dev/ttyUSB1", 57600, &xbee)) != 0) {
		fprintf(stderr, "xbee_setup(): failed... (%d)\n", ret);
		exit(1);
	}
	/* setup libxbee to use the series 1 packets - you have to do this before you do anything else! */
	xbee_modeSet(xbee, "series2");
	
	/* get the connection type ID, you pass in a string, it returns an ID */
	if ((ret = xbee_conTypeIdFromName(xbee, "Data (explicit)", &conType)) != 0) {
		fprintf(stderr, "xbee_conTypeIdFromName(): failed... (%d)\n", ret);
		exit(1);
	}
	
	/* clear the address field */
	memset(&addr, 0, sizeof(addr));
	/* build a connection to the following address */
	addr.addr64_enabled = 1;
	/* this is the address for my coordinator */
	addr.addr64[0] = 0x00;
	addr.addr64[1] = 0x13;
	addr.addr64[2] = 0xA2;
	addr.addr64[3] = 0x00;
	addr.addr64[4] = 0x40;
	addr.addr64[5] = 0x2D;
	addr.addr64[6] = 0x60;
	addr.addr64[7] = 0x7B;
	
	if ((ret = xbee_conNew(xbee, &con, conType, &addr, NULL)) != 0) {
		fprintf(stderr, "xbee_newcon(): failed... (%d)\n", ret);
		exit(1);
	}
	
	xbee_conAttachCallback(xbee, con, myCB, NULL);
	
	/* the main loop */
	for (;;) {
	
		printf("Sending...\n");
		if (xbee_conTx(xbee, con, "Hello End Node!")) {
			xbee_log(xbee,0,"Error while sending message... :(");
		}
		
		sleep(1);
	}
	
	/* shutdown the connection, 'p' here gets set to the user data (see inside the callback) */
	xbee_conEnd(xbee, con, NULL);
	
	/* shutdown the libxbee instance */
	xbee_shutdown(xbee);
	
	return 0;
}
