#include <kern/e1000.h>
#include <kern/pci.h>
#include <kern/pmap.h>
#include <inc/stdio.h>
// LAB 6: Your driver code here

#define NTXDESCS	64
#define NRXDESCS	128

struct tx_desc tx_desc_table[NTXDESCS];
struct rx_desc rx_desc_table[NRXDESCS];

volatile uint32_t *e1000;

volatile uint32_t *e1000_tdt;
volatile uint32_t *e1000_rdt;

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

	uintptr_t ral0 = E1000_REG_ADDR(e1000, E1000_RA);
	uintptr_t rah0 = ral0 + sizeof(uint32_t);
	*(uint32_t *)ral0 = 0x12005452;
	*(uint32_t *)rah0 = 0x5634 | E1000_RAH_AV;

	physaddr_t	rx_table = PADDR(&rx_desc_table);
	uintptr_t rdbal = E1000_REG_ADDR(e1000, E1000_RDBAL);
	*(uint32_t *) rdbal = rx_table;
	uintptr_t rdbah = E1000_REG_ADDR(e1000, E1000_RDBAH);
	*(uint32_t *) rdbah = 0;

	for (i = 0; i<NRXDESCS; i++) {
		rx_desc_table[i].addr = page2pa(page_alloc(0)) + 4;
	}

	uintptr_t rdlen = E1000_REG_ADDR(e1000, E1000_RDLEN);
	*(uint32_t *)rdlen = sizeof(rx_desc_table);

	uintptr_t rdt = E1000_REG_ADDR(e1000, E1000_RDT);
	*(uint32_t *)rdt = NRXDESCS - 1;
	uintptr_t rdh = E1000_REG_ADDR(e1000, E1000_RDH);
	*(uint32_t *)rdh = 0;
	e1000_rdt = (uint32_t *)rdt;

	uint32_t rflag = 0;
	uintptr_t rctl = E1000_REG_ADDR(e1000, E1000_RCTL);

	rflag |= E1000_RCTL_EN;
	rflag &= (~E1000_RCTL_DTYP_MASK);
	rflag |= E1000_RCTL_BAM;
	rflag |= E1000_RCTL_SZ_2048;
	rflag |= E1000_RCTL_SECRC;

	*(uint32_t *) rctl = rflag;
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

int e1000_get_rx_desc(struct rx_desc *rd) {

	int i = (*e1000_rdt + 1) & (NRXDESCS - 1);
	if (!(rx_desc_table[i].status & E1000_RXD_STAT_DD ) || !(rx_desc_table[i].status & E1000_RXD_STAT_EOP)) {
		return -1;
	}

	struct rx_desc *rr;
	rr = &rx_desc_table[i];

	uint64_t pa = rd->addr;
	*rd = *rr;
	rr->addr = pa;
	rr->status = 0;

	*e1000_rdt = i;

	return 0;
}
