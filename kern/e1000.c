#include <kern/e1000.h>
#include <kern/pci.h>
#include <kern/pmap.h>
#include <inc/stdio.h>
// LAB 6: Your driver code here

#define NTXDESCS	64

struct tx_desc tx_desc_table[NTXDESCS];

volatile uint32_t *e1000;

volatile uint32_t *e1000_tdt;

#define E1000_REG_ADDR(e, off) (((uintptr_t) e) + (off))


int pci_e1000_attach(struct pci_func *pcif) {
	pci_func_enable(pcif);

	e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);

	// initialize tx_desc_table
	struct tx_desc td = {
		.addr = (uint64_t)0,
		.length = 0,
		.cso = 0,
		.cmd = 0,
		.status = E1000_TXD_STAT_DD,
		.css = 0,
		.special = 0
	};
	int i;
	for (i = 0; i < NTXDESCS; i++) {
		tx_desc_table[i] = td;
	}

	uintptr_t tdbal = E1000_REG_ADDR(e1000, E1000_TDBAL);
	*(uint32_t *)tdbal = PADDR(tx_desc_table);
	uintptr_t tdbah = E1000_REG_ADDR(e1000, E1000_TDBAH);
	*(uint32_t *)tdbah = 0;

	uintptr_t tdlen = E1000_REG_ADDR(e1000, E1000_TDLEN);
	*(uint32_t *)tdlen = sizeof(tx_desc_table);

	uintptr_t tdh = E1000_REG_ADDR(e1000, E1000_TDH);
	*(uint32_t *)tdh = 0;
	uintptr_t tdt = E1000_REG_ADDR(e1000, E1000_TDT);
	*(uint32_t *)tdt = 0;
	e1000_tdt = (uint32_t *)tdt;

	uint32_t tflag = 0;
	uintptr_t tctl = E1000_REG_ADDR(e1000, E1000_TCTL);

	tflag |= E1000_TCTL_EN;

	tflag |= E1000_TCTL_PSP;

	tflag |= 0x40 << 12;
	*(uint32_t *)tctl = tflag;

	uint32_t tpg = 0;
	uintptr_t tipg = E1000_REG_ADDR(e1000, E1000_TIPG);
	tpg = 10;
	tpg |= 4 << 10;
	tpg |= 6 << 20;
	tpg &= 0x3FFFFFFF;
	*(uint32_t *)tipg = tpg;

	return 0;
}

int e1000_put_tx_desc(struct tx_desc *td) {
	struct tx_desc *tt = &tx_desc_table[*e1000_tdt];
	if (!(tt->status & E1000_TXD_STAT_DD)) {
		cprintf("transmit descriptor list is full\n");
		return -1;
	}

	*tt = *td;
	tt->cmd |= (E1000_TXD_CMD_RS >> 24);

	*e1000_tdt = (*e1000_tdt + 1) & (NTXDESCS - 1);

	return 0;
}
