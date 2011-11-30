#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include <xbee.h>

/* this holds onto the ID for the 64-bit data connection */
unsigned char conType;

/* the callback function */
void myCB(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt, void **userData) {
	int ret;
	
	if ((*pkt)->status) {
		printf("status = %d\n",(*pkt)->status);
		return;
	}
	
	if ((*pkt)->datalen == 0) {
		printf("Scan complete!\n");
		if (userData && *userData) sem_post(*userData);
		return;
	}

	if ((*pkt)->datalen < 12) {
		printf("Short value...\n");
		return;
	}

	/* what did the message say? */
	printf("They said this: %*s\n", (*pkt)->datalen - 12, &((*pkt)->data[11]));
}


int main(int argc, char *argv[]) {
	int ret;
	sem_t sem;
	void *p;
	
	/* this is our xbee instance... from 'user' space you don't have access to the struct */
	struct xbee *xbee;
	
	/* this is the connection we will make... again, you don't have access to the struct */
	struct xbee_con *con;
	struct xbee_conAddress addr;
	
	/* the packet that is recieved... you have access to this! (see xbee.h) */
	struct xbee_pkt *pkt;
	
	/* make a lixbee instance, and connect it to /dev/ttyUSB1 @ 57600 baud
	   you don't have to keep hold of the returned xbee, in which case you can pass NULL and the most recently started instance will be used! */
	if ((ret = xbee_setup("/dev/ttyUSB0", 57600, &xbee)) != 0) {
		xbee_log(-1,"xbee_setup(): failed... (%d)", ret);
		exit(1);
	}
	/* setup libxbee to use the series 1 packets - you have to do this before you do anything else! */
	xbee_modeSet(xbee, "series1");
	
	/* get the connection type ID, you pass in a string, it returns an ID */
	if ((ret = xbee_conTypeIdFromName(xbee, "Local AT", &conType)) != 0) {
		xbee_log(-1,"xbee_conTypeIdFromName(): failed... (%d)", ret);
		exit(1);
	}
	
	if ((ret = sem_init(&sem, 0, 0)) != 0) {
		xbee_log(-1,"sem_init(): failed... (%d)", ret);
		exit(1);
	}
	
	/* build a connection to the following address */
	addr.addr16_enabled = 0;
	addr.addr64_enabled = 0;
	if ((ret = xbee_conNew(xbee, &con, conType, &addr, &sem)) != 0) {
		xbee_log(-1,"xbee_newcon(): failed... (%d)", ret);
		exit(1);
	}
	{
    struct xbee_conOptions opts;
    /* enable waitForAck... this allows us to see if the packet was sent successfully! */
    xbee_conOptions(xbee, con, &opts, NULL);
    opts.waitForAck = 1;
    xbee_conOptions(xbee, con, NULL, &opts);
	}
	/* attach the callback */
	xbee_conAttachCallback(xbee, con, myCB, NULL);

	/* send the request */
	if ((ret = xbee_conTx(xbee, con, "ND")) != 0) {
		xbee_log(-1,"Something went wrong... (%d)", ret);
	} else {
		sem_wait(&sem);
	}

	sem_destroy(&sem);

	/* shutdown the connection */
	xbee_conEnd(xbee, con, NULL);
	
	/* shutdown the libxbee instance */
	xbee_shutdown(xbee);
	
	return 0;
}