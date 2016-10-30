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

#endif