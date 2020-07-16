/*
 * scsi.h
 *
 * Created: 2019/06/16
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */


#ifndef SCSI_H_
#define SCSI_H_


#define SCSI_OK								(0)
#define SCSI_ERROR_NOT_SUPPORTED			(-1)
#define SCSI_ERROR_NULL						(-2)
#define SCSI_ERROR_CONTINUE					(-3)
#define SCSI_ERROR_NOMEM					(-4)
#define SCSI_NOT_IMPLEMENTED				(-5)
#define SCSI_ERROR_NOT_READY				(-6)

/*--------------------------------------------------------------------------*/
//J SCSI
/*--------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct ScsiCommandDescriptorBlock_6bytes_t
{
	uint8_t operation_code;
	uint8_t misc_cdb_info;
	uint16_t logical_block_address;
	uint8_t length;
	uint8_t control;
} ScsiCdb6;
#pragma pack()

#pragma pack(1)
typedef struct ScsiCommandDescriptorBlock_10bytes_t
{
	uint8_t operation_code;
	uint8_t misc_cdb_info1;
	uint32_t logical_block_address;
	uint8_t misc_cdb_info2;
	uint16_t length;
	uint8_t control;
} ScsiCdb10;
#pragma pack()

#pragma pack(1)
typedef struct ScsiCommandDescriptorBlock_12bytes_t
{
	uint8_t operation_code;
	uint8_t misc_cdb_info1;
	uint32_t logical_block_address;
	uint32_t length;
	uint8_t misc_cdb_info2;
	uint8_t control;
} ScsiCdb12;
#pragma pack()

#pragma pack(1)
typedef struct ScsiCommandDescriptorBlock_16bytes_t
{
	uint8_t operation_code;
	uint8_t misc_cdb_info1;
	uint64_t logical_block_address;
	uint32_t length;
	uint8_t misc_cdb_info2;
	uint8_t control;
} ScsiCdb16;
#pragma pack()

#pragma pack(1)
typedef struct ScsiCommandDescriptorBlock_variable_len_t
{
	uint8_t operation_code;
	uint8_t control;
	uint8_t misc_cdb_info1;
	uint8_t misc_cdb_info2;
	uint8_t misc_cdb_info3;
	uint8_t misc_cdb_info4;
	uint8_t misc_cdb_info5;
	uint8_t additional_cdb_len;
	uint16_t service_action;
	uint8_t service_action_specific_fileds[0];
} ScsiCdbValiableLen;
#pragma pack()




/* SCSI Group Code */
#define SCSI_OPCODE_GROUP_MASK					(7 << 5)
#define SCSI_OPCODE_GROUP_6B					(0 << 5)
#define SCSI_OPCODE_GROUP_10B					(1 << 5)
#define SCSI_OPCODE_GROUP_10B_					(2 << 5)
#define SCSI_OPCODE_GROUP_RESERVED				(3 << 5)
#define SCSI_OPCODE_GROUP_16B					(4 << 5)
#define SCSI_OPCODE_GROUP_12B					(5 << 5)
#define SCSI_OPCODE_GROUP_VENDOR1				(6 << 5)
#define SCSI_OPCODE_GROUP_VENDOR2				(7 << 5)


#define SCSI_OPCODE_GHANGE_DEFINITION			(0x40)
#define SCSI_OPCODE_COMPARE						(0x39)
#define SCSI_OPCODE_COPY						(0x18)
#define SCSI_OPCODE_COPY_AND_VERIFY				(0x3A)
#define SCSI_OPCODE_FORMAT_UNIT					(0x04)
#define SCSI_OPCODE_INQUIRY						(0x12)
#define SCSI_OPCODE_LOCK_UNLOCK_CACHE_10		(0x36)
#define SCSI_OPCODE_LOCK_UNLOCK_CASHE_16		(0x92)
#define SCSI_OPCODE_LOG_SELECT					(0x4C)
#define SCSI_OPCODE_LOG_SENSE					(0x4D)
#define SCSI_OPCODE_MODE_SELECT_6				(0x15)
#define SCSI_OPCODE_MODE_SELECT_10				(0x55)
#define SCSI_OPCODE_MODE_SENSE_6				(0x1A)
#define SCSI_OPCODE_MODE_SENSE_10				(0x5A)
#define SCSI_OPCODE_PERSISTENT_RESERVE_IN		(0x5E)
#define SCSI_OPCODE_PERSISTENT_PESERVE_OUT		(0x5F)
#define SCSI_OPCODE_READ_6						(0x08)
#define SCSI_OPCODE_READ_10						(0x28)
#define SCSI_OPCODE_READ_12						(0xA8)
#define SCSI_OPCODE_READ_16						(0x88)
#define SCSI_OPCODE_READ_32						(0x7F)	//0x0009?
#define SCSI_OPCODE_READ_BUFFER					(0x3C)
#define SCSI_OPCODE_READ_CAPACITY_10			(0x25)
#define SCSI_OPCODE_READ_CAPACITY_16			(0x9E)	//0x10?
#define SCSI_OPCODE_READ_DEFECT_DATA_10			(0x37)
#define SCSI_OPCODE_READ_DEFECT_DATA_12			(0xB7)
#define SCSI_OPCODE_READ_LONG_10				(0x3E)
#define SCSI_OPCODE_READ_LONG_16				(0x9E)
#define SCSI_OPCODE_REASSIGN_BLOCKS				(0x07)
#define SCSI_OPCODE_RECEIVE_DIAGNOSTIC_RESULT	(0x1C)
#define SCSI_OPCODE_RELEASE_6					(0x17)
#define SCSI_OPCODE_RELEASE_10					(0x57)
#define SCSI_OPCODE_REPORT_DEVICE_IDENTIFIER	(0xA3)	//0x05?
#define SCSI_OPCODE_REPORT_LUNS					(0xA0)
#define SCSI_OPCODE_REQUEST_SENSE				(0x03)
#define SCSI_OPCODE_RESERVE_6					(0x16)
#define SCSI_OPCODE_RESERVE_10					(0x56)
#define SCSI_OPCODE_REZERO_UNIT					(0x01)
#define SCSI_OPCODE_SEEK_6						(0x0B)
#define SCSI_OPCODE_SEEK_EXTENDED_10			(0x2B)
#define SCSI_OPCODE_SEND_DIAGNOSTIC				(0x1D)
#define SCSI_OPCODE_SET_DEVICE_IDENTIFIER		(0xA4)	//0x06?
#define SCSI_OPCODE_START_STOP_UNIT				(0x1B)
#define SCSI_OPCODE_SYNCHRONIZE_CACHE_10		(0x35)
#define SCSI_OPCODE_SYNCHRONIZE_CACHE_16		(0x91)
#define SCSI_OPCODE_TEST_UNIT_READY				(0x00)
#define SCSI_OPCODE_UNMAP						(0x42)
#define SCSI_OPCODE_VERIFY_10					(0x2F)
#define SCSI_OPCODE_VERIFY_12					(0xAF)
#define SCSI_OPCODE_VERIFY_16					(0x8F)
#define SCSI_OPCODE_VERIFY_32					(0x7F)	//0x000A?
#define SCSI_OPCODE_WRITE_6						(0x0A)
#define SCSI_OPCODE_WRITE_10					(0x2A)
#define SCSI_OPCODE_WRITE_16					(0xAA)
#define SCSI_OPCODE_WRITE_32					(0x7F)	//0x000B?
#define SCSI_OPCODE_WRITE_AND_VERIFY_10			(0x2E)
#define SCSI_OPCODE_WRITE_AND_VERIFY_12			(0xAE)
#define SCSI_OPCODE_WRITE_AND_VERIFY_16			(0x8E)
#define SCSI_OPCODE_WRITE_AND_VERIFY_32			(0x7F)	//0x000C?
#define SCSI_OPCODE_WRITE_BUFFER				(0x3B)
#define SCSI_OPCODE_WRITE_LONG					(0x3F)
#define SCSI_OPCODE_WRITE_LONG_16				(0x9F)	//0x11
#define SCSI_OPCODE_WRITE_SAME_10				(0x41)
#define SCSI_OPCODE_WRITE_SAME_16				(0x93)
#define SCSI_OPCODE_WRITE_SAME_32				(0x7F)	//0x000D

#define SCSI_OPCODE_READ_FORMAT_CAPACITIES		(0x23)	//UFI
#define SCSI_OPCODE_PREVENT_ALLOW_MEDIUM_REMOVAL	(0x1E)	//UFI



/*---------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct ScsiInquiryData_t
{
	uint8_t peripheralDeviceType;
	union {
		uint8_t byte;
		struct {
			uint8_t reserved : 7;
			uint8_t rmb: 1;
		} bm;
	} removable;
	uint8_t version;
	union {
		uint8_t byte;
		struct {
			uint8_t response_data_format : 4;
			uint8_t hi_sup               : 1;
			uint8_t norm_aca             : 1;
		} bm;
	} aca_flags;
	uint8_t additional_length;
	union {
		uint8_t byte;
		struct {
			uint8_t protect  : 1;
			uint8_t reserved : 2;
			uint8_t _3pc     : 1;
			uint8_t tpgs     : 2;
			uint8_t acc      : 1;
			uint8_t sccs     : 1;
		} bm;
	} sccs_flags;
	union {
		uint8_t byte;
		struct {
			uint8_t reserved : 4;
			uint8_t multi_ip : 1;
			uint8_t reserved2: 1;
			uint8_t enc_serv : 1;
			uint8_t bque     : 1;
		} bm;
	} bque_flags;
	union {
		uint8_t byte;
		struct {
			uint8_t reserved : 1;
			uint8_t cmd_que : 1;
		} bm;
	} rel_adr;
	uint8_t vender_identification[8];
	uint8_t product_identification[16];
	uint8_t product_revision_level[4];
} ScsiInquiryData;
#pragma pack()

//J SPEC的には複数のフォーマットリストを送ることが出来るが、1個あればええやろ
#pragma pack(1)
typedef struct ScsiCapacityList_t
{
	struct {
		uint8_t reserved[3];
		uint8_t capacity_list_length;
	} capacityListHeader;
	struct {
		uint32_t number_of_blocks;
		uint8_t descriptor_code;
		uint8_t block_length[3];
	} currentCapacityHeader;
} ScsiCapacityList;
#pragma pack()


#define SENSE_KEY_NO_SENSE				(0x00)
#define SENSE_KEY_RECOVERED_ERROR		(0x01)
#define SENSE_KEY_NOT_READY				(0x02)
#define SENSE_KEY_MIDIUM_ERROR			(0x03)
#define SENSE_KEY_HARDWARE_ERROR		(0x04)
#define SENSE_KEY_ILLEGAL_REQUEST		(0x05)
#define SENSE_KEY_UNIT_ATTENTION		(0x06)
#define SENSE_KEY_DATA_PROTECT			(0x07)
#define SENSE_KEY_BLANK_CHECK			(0x08)
#define SENSE_KEY_VENDER_SPEC			(0x09)
#define SENSE_KEY_RESERVED_A			(0x0A)
#define ABORTED_COMMAND					(0x0B)
#define SENSE_KEY_RESERVED_C			(0x0C)
#define SENSE_KEY_VOLUME_OVERFLOW		(0x0D)
#define SENSE_KEY_MISCOMPARE			(0x0E)
#define SENSE_KEY_RESERVED_F			(0x0F)


#pragma pack(1)
typedef struct ScsiRequestSenseStandardData_t
{
	union {
		struct {
			uint8_t error_code : 7;
			uint8_t valid      : 1;
		} bm;
		uint8_t byte;
	}error_codes;
	uint8_t reserved;
	uint8_t sense_key;
	uint32_t information;
	uint8_t additional_sense_length;
	uint32_t reserved_2;
	uint8_t additional_sense_code;
	uint8_t additional_sense_code_qualifier;
	uint32_t reserved_3;
} ScsiRequestSenseStandardData;
#pragma pack()

#define SCSI_CAPACITY_DESCRIPTOR_TYPE_UNFORMATTED_MEDIA			(0x1)
#define SCSI_CAPACITY_DESCRIPTOR_TYPE_FORMATTED_MEDIA			(0x2)
#define SCSI_CAPACITY_DESCRIPTOR_TYPE_NO_CARTRIDGE_IN_DRIVE		(0x3)

//J Read Capacity 10 へのレスポンス
#pragma pack(1)
typedef struct ScsiCapacityData10_t
{
	uint32_t last_logical_block_address;
	uint32_t block_size;	
} ScsiCapacityData10;
#pragma pack()

/*---------------------------------------------------------------------------*/
int8_t scsiInitialize(int force);
int8_t scsiParseCommands(uint8_t *cdb, size_t cdb_len, uint8_t *buf, size_t buf_len, size_t *tranfer_len);
uint8_t scsiCheckMoreTransfer(uint8_t *cdb, size_t cdb_len);

void scsiDiskEnable(int enable);
int  scsiIsDiskEnabled(void);
int  scsiAnyAttentionIsAvailable(void);

#endif /* SCSI_H_ */