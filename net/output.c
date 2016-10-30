#include "ns.h"

#include <inc/lib.h>

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	struct tx_desc td;
	int r;

	while(1) {
		r = sys_ipc_recv(&nsipcbuf);
		if (r != 0) {
			continue;
		}

		if ((thisenv->env_ipc_from != ns_envid) || (thisenv->env_ipc_value != NSREQ_OUTPUT)) {
			continue;
		}

		memset(&td, 0, sizeof(td));

		if (thisenv->env_ipc_value == NSREQ_OUTPUT) {
			td.addr = (uint32_t)nsipcbuf.pkt.jp_data;
			td.length = nsipcbuf.pkt.jp_len;
			td.cmd = 9;
			r = sys_tx_pkt(&td);
		}
	}

}
