/*
 * registry.h
 *
 * Created: 2019/09/17 23:00:35
 *  Author: kiyot
 */ 


#ifndef REGISTRY_H_
#define REGISTRY_H_

#pragma pack(1)

typedef struct UpdateFileHeader_t
{
	char signature[4];
	uint32_t version;
	uint8_t hash[4];
	uint32_t size;
	uint8_t file_hash[16];
} UpdateFileHeader;

typedef struct AiMini4wdRegistry_t
{
	//J for SDK
	union {
		uint8_t bytes[256];
		struct {
			uint32_t sdk_version;
			UpdateFileHeader fw;
			uint16_t tachometer_threshold1;
			uint16_t tachometer_threshold2;
		} field;
	} sdk_data;
	
	//J for Application
	uint8_t user_data[256];	
} AiMini4wdRegistry;
#pragma pack()

int aiMini4wdRegistryLoad(void);
AiMini4wdRegistry *aiMini4wdRegistryGet(void);

int aiMini4wdRegistryUpdate(void);


#endif /* REGISTRY_H_ */