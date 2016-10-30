#ifndef JOS_INC_NETE1000_H
#define JOS_INC_NETE1000_H

#include <inc/types.h>

//
//  63            48 47   40 39   32 31   24 23   16 15             0
//  +---------------------------------------------------------------+
//  |                         Buffer address                        |
//  +---------------+-------+-------+-------+-------+---------------+
//  |    Special    |  CSS  | Status|  Cmd  |  CSO  |    Length     |
//  +---------------+-------+-------+-------+-------+---------------+
//
struct tx_desc
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
};

// See 8254x_GBe_SDM.pdf Section 3.2.3 Receive Descriptor Format
//
//  63            48 47   40 39   32 31          16 15             0
//  +---------------------------------------------------------------+
//  |                         Buffer address                        |
//  +---------------+-------+-------+-------+-------+---------------+
//  |    Special    | Errors| Status| Pkt. Checksum |    Length     |
//  +---------------+-------+-------+-------+-------+---------------+
//
struct rx_desc
{
	uint64_t addr;		// addr is vritual address in user space
						//         physical address in kernel space
	uint16_t length;
	uint16_t checksum;
	uint8_t status;
	uint8_t errors;
	uint16_t special;
};

#endif