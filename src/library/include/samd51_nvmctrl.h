/*
 * samd51_clock.h
 *
 * Created: 2019/05/22
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */

#ifndef SAMD51_NVMCTRL_H_
#define SAMD51_NVMCTRL_H_

int samd51_nvmctrl_read (const uint32_t phisical_adr, void *buf, const size_t size);
int samd51_nvmctrl_write_page(const uint32_t phisical_adr, const void *buf, const size_t npages);
int samd51_nvmctrl_erase_block(const uint32_t phisical_adr, size_t blocks);

size_t samd51_nvmctrl_get_page_size(void);
size_t samd51_nvmctrl_get_block_size(void);
size_t samd51_nvmctrl_get_pages(void);

#endif /* SAMD51_NVMCTRL_H_ */