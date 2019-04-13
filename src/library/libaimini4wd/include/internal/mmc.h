/*
 * mmc.h
 *
 * Created: 2019/03/10
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */ 


#ifndef MMC_H_
#define MMC_H_

typedef enum MmcCardType_t{
	CARD_TYPE_MMC,
	CARD_TYPE_SDC,
	CARD_TYPE_SDCv2,
	CARD_TYPE_SDHC
} MmcCardType;


typedef struct MmcDrive_t
{
	MmcCardType type;
} MmcDrive;




#endif /* MMC_H_ */