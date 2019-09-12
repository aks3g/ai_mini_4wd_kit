/*
 * samd51_sdhc.h
 *
 * Created: 2019/03/10
 * Copyright © 2019 Kiyotaka Akasaka. All rights reserved.
 */ 

#ifndef SAMD51_SDHC_H_
#define SAMD51_SDHC_H_

#include <stddef.h>

#pragma pack(1)
typedef union SAMD51_SDHC_CMD_t{
	uint16_t word;
	struct {
		uint16_t response_type_select : 2;
		uint16_t rsvd : 1;
		uint16_t command_crc_checl_enable : 1;
		uint16_t command_index_check_enable : 1;
		uint16_t data_present_select : 1;
		uint16_t command_type : 2;
		uint16_t command_index : 6;
		uint16_t reserved : 2;
	} bm;
} SAMD51_SDHC_CMD;

#pragma pack(1)
typedef struct SdHostControllerRegs_t
{
	uint32_t sdma_system_address_argument2;
//	uint16_t block_size;
	union {
		uint16_t word;
		struct {
			uint16_t transfer_block_size : 12;
			uint16_t host_sdma_buffer_boundary : 3;
			uint16_t reserved : 1;
		} bm;
	} block_size;
	uint16_t block_count;
	uint32_t argument1;
//	uint16_t transfer_mode;
	union {
		uint16_t word;
		struct {
			uint16_t dma_enable : 1;
			uint16_t block_count_enable : 1;
			uint16_t auto_command_enable : 2;
			uint16_t data_transfer_direction_select : 1;
			uint16_t multi_single_block_select : 1;
			uint16_t reserved : 10;
		} bm;
	} transfer_mode;
	SAMD51_SDHC_CMD command;
	uint32_t response[4]; //128bit
	uint32_t buffer_data_port;
//	uint32_t present_state;
	union {
		uint32_t dword;
		struct {
			uint32_t command_inhibit_cmd : 1;
			uint32_t command_inhibit_dat : 1;
			uint32_t dat_line_active : 1;
			uint32_t re_tuning_request : 1;
			uint32_t rsvd : 4;
			uint32_t write_transfer_active : 1;
			uint32_t read_transfer_active : 1;
			uint32_t buffer_write_enable : 1;
			uint32_t buffer_read_enable :1;
			uint32_t rsvd2 : 4;
			uint32_t card_inserted : 1;
			uint32_t card_state_stable : 1;
			uint32_t card_detect_pin_level : 1;
			uint32_t write_protected_switch_pin_l_level : 1;
			uint32_t dat_line_signal_level : 4;
			uint32_t cmd_line_signal_level : 1;
			uint32_t rsvd3 : 7;
		} bm;
	} present_state;
//	uint8_t  host_control1;
	union {
		uint8_t byte;
		struct {
			uint8_t led_control : 1;
			uint8_t data_transfer_width : 1;
			uint8_t high_speed_enable : 1;
			uint8_t dma_select : 2;
			uint8_t extended_data_transfer_width : 1;
			uint8_t card_detect_test_level : 1;
			uint8_t card_detect_signal_seelction : 1;
			
		} bm;
	} host_control1;
//	uint8_t  power_control;
	union {
		uint8_t byte;
		struct {
			uint8_t sd_bus_power : 1;
			uint8_t sd_bus_voltage_select : 3;
			uint8_t rsvd : 4;
		} bm;
	} power_control;
//	uint8_t  block_gap_contorl;
	union {
		uint8_t byte;
		struct {
			uint8_t stop_at_block_gap_request : 1;
			uint8_t continue_request : 1;
			uint8_t read_wait_control : 1;
			uint8_t interrupt_at_block_gap : 1;
			uint8_t rsvd : 4;
		} bm;
	} block_gap_contorl;
//	uint8_t  wakeup_control;
	union {
		uint8_t byte;
		struct {
			uint8_t wakeup_event_enable_on_sd_card_interrupt : 1;
			uint8_t wakeup_event_enable_on_sd_card_insertion : 1;
			uint8_t wakeup_event_enable_on_sd_card_removal : 1;
			uint8_t rsvd : 5;
		} bm;
	} wakeup_control;
//	uint16_t clock_control;
	union {
		uint16_t word;
		struct {
			uint16_t internal_clock_enable : 1;
			uint16_t internal_clock_stable : 1;
			uint16_t sd_clock_enable : 1;
			uint16_t reserved : 2; 
			uint16_t clock_generator_select : 1;
			uint16_t upper_bias_of_sdclk_frequency_select : 2;
			uint16_t sdclk_frequency_select : 8;
		} bm;
	}clock_control;
	uint8_t  timeout_control;
//	uint8_t  software_reset;
	union{
		uint8_t byte;
		struct {
			uint8_t software_reset_for_all : 1;
			uint8_t software_reset_for_cmd_line : 1;
			uint8_t software_reset_for_dat_line : 1;
			uint8_t rsvd : 5;
		} bm;
	} software_reset;
//	uint16_t normal_interrupt_status;
	union{
		uint16_t word;
		struct {
			uint16_t command_complete : 1;
			uint16_t transfer_complete : 1;
			uint16_t block_gap_event : 1;
			uint16_t dma_interrupt : 1;
			uint16_t buffer_write_ready : 1;
			uint16_t buffer_read_ready : 1;
			uint16_t card_insertion : 1;
			uint16_t card_removeal : 1;
			uint16_t card_interrupt : 1;
			uint16_t int_a : 1;
			uint16_t int_b : 1;
			uint16_t int_c : 1;
			uint16_t re_turning_event : 1;
			uint16_t rsvd : 2;
			uint16_t error_interrupt : 1;
		} bm;
	} normal_interrupt_status;
//	uint16_t error_interrupt_status;
	union{
		uint16_t word;
		struct {
			uint16_t command_timeout_error : 1;
			uint16_t command_crc_error : 1;
			uint16_t command_end_bit_error : 1;
			uint16_t command_index_error : 1;
			uint16_t data_timeout_error : 1;
			uint16_t data_crc_error : 1;
			uint16_t data_end_bit_error : 1;
			uint16_t current_limit_error : 1;
			uint16_t auto_cmd12_error : 1;
			uint16_t adma_error : 1;
			uint16_t tuning_error : 1;
			uint16_t rsvd : 1;
			uint16_t vender_specific_error_status : 4;
		} bm;
	} error_interrupt_status;
//	uint16_t normal_interrupt_status_enable;
	union{
		uint16_t word;
		struct {
			uint16_t command_complete : 1;
			uint16_t transfer_complete : 1;
			uint16_t block_gap_event : 1;
			uint16_t dma_interrupt : 1;
			uint16_t buffer_write_ready : 1;
			uint16_t buffer_read_ready : 1;
			uint16_t card_insertion : 1;
			uint16_t card_removeal : 1;
			uint16_t card_interrupt : 1;
			uint16_t int_a : 1;
			uint16_t int_b : 1;
			uint16_t int_c : 1;
			uint16_t re_turning_event : 1;
			uint16_t rsvd : 3;
		} bm;
	} normal_interrupt_status_enable;
//	uint16_t error_interrupt_status_enbale;
	union{
		uint16_t word;
		struct {
			uint16_t command_timeout_error : 1;
			uint16_t command_crc_error : 1;
			uint16_t command_end_bit_error : 1;
			uint16_t command_index_error : 1;
			uint16_t data_timeout_error : 1;
			uint16_t data_crc_error : 1;
			uint16_t data_end_bit_error : 1;
			uint16_t current_limit_error : 1;
			uint16_t auto_cmd12_error : 1;
			uint16_t adma_error : 1;
			uint16_t tuning_error : 1;
			uint16_t rsvd : 1;
			uint16_t vender_specific_error_status : 4;
		} bm;
	} error_interrupt_status_enbale;
//	uint16_t normal_interrupt_signal_enable;
	union{
		uint16_t word;
		struct {
			uint16_t command_complete : 1;
			uint16_t transfer_complete : 1;
			uint16_t block_gap_event : 1;
			uint16_t dma_interrupt : 1;
			uint16_t buffer_write_ready : 1;
			uint16_t buffer_read_ready : 1;
			uint16_t card_insertion : 1;
			uint16_t card_removeal : 1;
			uint16_t card_interrupt : 1;
			uint16_t int_a : 1;
			uint16_t int_b : 1;
			uint16_t int_c : 1;
			uint16_t re_turning_event : 1;
			uint16_t rsvd : 3;
		} bm;
	} normal_interrupt_signal_enable;
//	uint16_t error_interrupt_signal_enable;
	union{
		uint16_t word;
		struct {
			uint16_t command_timeout_error : 1;
			uint16_t command_crc_error : 1;
			uint16_t command_end_bit_error : 1;
			uint16_t command_index_error : 1;
			uint16_t data_timeout_error : 1;
			uint16_t data_crc_error : 1;
			uint16_t data_end_bit_error : 1;
			uint16_t current_limit_error : 1;
			uint16_t auto_cmd12_error : 1;
			uint16_t adma_error : 1;
			uint16_t tuning_error : 1;
			uint16_t rsvd : 1;
			uint16_t vender_specific_error_status : 4;
		} bm;
	} error_interrupt_signal_enable;
//	uint16_t auto_cmd_error_status;
	union{
		uint16_t word;
		struct {
			uint16_t auto_cmd12_not_executed : 1;
			uint16_t auto_cmd_timeout_error : 1;
			uint16_t auto_cmd_crc_error : 1;
			uint16_t auto_cmd_end_bit_error : 1;
			uint16_t auto_cmd_index_error : 1;
			uint16_t rsvd : 2;
			uint16_t command_not_issued_by_auto_cmd12_error : 1;
			uint16_t rsvd2 : 8;
		} bm;
	} auto_cmd_error_status;
//	uint16_t host_control2;
	union{
		uint16_t word;
		struct {
			uint16_t uhs_mode_select : 3;
			uint16_t _1_8v_signaling_enable : 1;
			uint16_t drive_strength_select : 2;
			uint16_t execute_tuning : 1;
			uint16_t sampling_clock_select : 1;
			uint16_t rsvd : 6;
			uint16_t asynchronous_interrupt_enable : 1;
			uint16_t preset_value_enable : 1;
		} bm;
	} host_control2;
//	uint16_t capabilities[4];
	union{
		uint64_t qword;
		struct {
			uint32_t timout_clock_frequency : 6;
			uint32_t rsvd : 1;
			uint32_t timeout_clock_unit : 1;
			uint32_t base_clock_frequency_for_sd_clock : 8;
			uint32_t max_block_length : 2;
			uint32_t _8bit_support_for_embeddded_device : 1;
			uint32_t adma2_support : 1;
			uint32_t rsvd2: 1;
			uint32_t high_speed_support : 1;
			uint32_t sdma_support : 1;
			uint32_t suspend_resume_support : 1;
			uint32_t voltage_support_3v3 : 1;
			uint32_t voltage_support_3v : 1;
			uint32_t voltage_support_1v8 : 1;
			uint32_t rsvd3 : 1;
			uint32_t _64bit_system_bus_support : 1;
			uint32_t asynchronous_interrupt_support : 1;
			uint32_t slot_type : 2;
			uint32_t sdr50_support : 1;
			uint32_t sdr104_support : 1;
			uint32_t ddr50_support : 1;
			uint32_t rsvd4 : 1;
			uint32_t driver_type_a_support : 1;
			uint32_t driver_type_b_support : 1;
			uint32_t driver_type_c_support : 1;
			uint32_t rsvd5 : 1;
			uint32_t timer_count_for_re_tuning : 4;
			uint32_t rsvd6 : 1;
			uint32_t use_tuning_for_sdr50 : 1;
			uint32_t re_tuning_modes : 2;
			uint32_t clock_multiplier : 8;
			uint32_t rsvd7 : 8;
		} bm;
	} capabilities;
//	uint16_t maximum_current_capabilities[4];
	union{
		uint64_t qword;
		struct {
			uint32_t maximun_current_for_3v3 : 8;
			uint32_t maximun_current_for_3v  : 8;
			uint32_t maximun_current_for_1v8 : 8;
			uint32_t rsvd  : 8;
			uint32_t rsvd2 : 32;
		} bm;
	} maximum_current_capabilities;
//	uint16_t force_event_for_auto_cmd_error_status;
	union{
		uint16_t word;
		struct {
			uint16_t force_event_for_auto_cmd12_not_executed : 1;
			uint16_t force_event_for_auto_cmd_timeout_error : 1;
			uint16_t force_event_for_auto_cmd_crc_error : 1;
			uint16_t force_event_for_auto_cmd_end_bit_error : 1;
			uint16_t force_event_for_auto_cmd_index_error : 1;
			uint16_t rsvd : 2;
			uint16_t force_event_for_command_not_issued_by_auto_cmd12_error : 1;
			uint16_t rsvd2 : 8;
		} bm;
	} force_event_for_auto_cmd_error_status;
//	uint16_t force_event_for_error_interrupt_status;
	union{
		uint16_t word;
		struct {
			uint16_t force_event_for_command_timeout_error : 1;
			uint16_t force_event_for_command_crc_error : 1;
			uint16_t force_event_for_command_end_bit_error : 1;
			uint16_t force_event_for_command_index_error : 1;
			uint16_t force_event_for_data_timeout_error : 1;
			uint16_t force_event_for_data_crc_error : 1;
			uint16_t force_event_for_data_end_bit_error : 1;
			uint16_t force_event_for_current_limit_error : 1;
			uint16_t force_event_for_auto_cmd_error : 1;
			uint16_t force_event_for_adma_error : 1;
			uint16_t rsvd : 2;
			uint16_t force_event_for_vendor_specific_error_status : 4;
		}bm;
	} force_event_for_error_interrupt_status;
//	uint8_t  adma_error_status;
	union{
		uint8_t byte;
		struct {
			uint8_t adma_error_state : 2;
			uint8_t adma_length_mismatch_error : 1;
			uint8_t rsvd : 5;
		} bm;
	} adma_error_status;
	uint8_t  reserved[3];
	uint64_t adma_system_address;
//	uint16_t preset_value[8];
	union {
		uint16_t word;
		struct {
			uint16_t sdclk_frequency_select_value : 10;
			uint16_t clock_generator_select_value : 1;
			uint16_t rsvd : 3;
			uint16_t driver_strength_select_value : 2;
		} bm;
	} preset_value[8];
	/* 0x70 - 0xDF */
	uint8_t  reserved2[0xE0 - 0x70];
//	uint32_t shared_bus_control;
	union{
		uint32_t dword;
		struct {
			uint32_t number_of_clock_pins : 3;
			uint32_t rsvd : 1;
			uint32_t number_of_interrupt_input_pins : 2;
			uint32_t rsvd2 : 2;
			uint32_t bus_width_preset : 7;
			uint32_t rsvd3 : 1;
			uint32_t clock_pin_select : 3;
			uint32_t rsvd4 : 1;
			uint32_t interrupt_pin_select : 3;
			uint32_t rsvd5 : 1;
			uint32_t back_end_power_control : 7;
			uint32_t rsvd6 : 1;
		} bm;
	} shared_bus_control;
	/* 0xE4 - 0xFB */
	uint8_t  reserved3[0xFC - 0xE4];
	uint16_t slot_interrupt_status;
//	uint16_t host_controller_version;
	union {
		uint16_t word;
		struct {
			uint8_t specificatoin_version_number;
			uint8_t vender_version_number;
		} bm;
	} host_controller_version;
} SdHostControllerRegs;

/* ちょっと自信ない */
typedef struct Adma_descripter_t
{
	uint32_t address;
	uint16_t length;
	uint8_t  rsv;
	uint8_t  attribute;
} Adma_desceipter;


typedef enum SAMD51_SDHC_t
{
	SAMD51_SDHC0 = 0x45000000UL,
	SAMD51_SDHC1 = 0x46000000UL,	
} SAMD51_SDHC;

typedef enum SAMD51_SDHC_BUS_VOLTAGE_t
{
	SAMD51_SDHC_BUS_3V3 = (0x7),
	SAMD51_SDHC_BUS_3V  = (0x6),
	SAMD51_SDHC_BUS_1V8 = (0x5)
} SAMD51_SDHC_BUS_VOLTAGE;

typedef enum SAMD51_SDHC_BUS_WIDTH_t
{
	SAMD51_SDHC_BUS_1BIT = 0,
	SAMD51_SDHC_BUS_4BIT = 2	
} SAMD51_SDHC_BUS_WIDTH;


typedef enum SAMD51_SDHC_AUTO_CMD_ENABLE_t
{
	SAMD51_SDHC_AUTO_CMD_DISABLED = 0,
	SAMD51_SDHC_AUTO_CMD12 = 1,
	SAMD51_SDHC_AUTO_CMD23 = 2,
	SAMD51_SDHC_AUTO_CMD_RSVD = 3	
} SAMD51_SDHC_AUTO_CMD_ENABLE;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//							    Abbreviation			Resp
// Basic Command (class 0)
#define CMD0			(0)	 // GOTO_IDLE_STATE			-
#define CMD1			(1)	 // SEND_OP_COND			R3
#define CMD2			(2)	 // ALL_SEND_CID			R2
#define CMD3			(3)  // SEND RELATIVE ADDR		R6
#define CMD4			(4)  // SET DSR					-
#define CMD6			(6)  // SWITCH FUNC				R1
#define CMD7			(7)  // SELECT/DESELECT CARD	R1b
#define CMD8			(8)  // SEND IF COND			R7
#define CMD9			(9)  // SEND CSD				R2
#define CMD10			(10) // SEND_CID				R2
#define CMD11			(11) // VOLTAGE SWITCH			R1
#define CMD12			(12) // STOP_TRANSMISSION		R1b
#define CMD13			(13) // SEND_STATUS				R1
#define CMD15			(15) // GO_INACTIVE_STATE		-

// Block Oriented Read Command (Class 2)
#define CMD16			(16) // SET_BLOCKLEN			R1
#define CMD17			(17) // READ_SINGLE_BLOCK		R1
#define CMD18			(18) // READ_MULTIPLE_BLOCK		R1
#define CMD19			(19) // SEND_TUNING_BLOCK		R1
#define CMD20			(20) // SPEED CLASS CONTROL		R1b
#define CMD23			(23) // SET_BLOCK_COUNT			R1

// Block Oriented Write Command (class 4)
//#define CMD16			(16) // SET_BLOCKLEN			R1
//#define CMD20			(20) // SPEED CLASS CONTROL		R1b
//#define CMD23			(23) // SET_BLOCK_COUNT			R1
#define CMD24			(24) // WRITE_BLOCK				R1
#define CMD25			(25) // WRITE MULTIPLE BLOCK	R1
#define CMD27			(27) // PROGRAM CSD				R1

// Block Oriented Write Protection Command (class 6)
#define CMD28			(28) // SET_WRITE_PROT			R1b
#define CMD29			(29) // CLR WRITE PROT			R1b
#define CMD30			(30) // SEND WRITE PROT			R1

// Erase Command (class 5)
#define CMD32			(32) // ERASE_WR_BLK_START		R1
#define CMD33			(33) // ERASE_WR_BLK_END		R1
#define CMD38			(38) // ERASE					R1b

// Lock Card (class 7)
//#define CMD16			(16) // SET_BLOCKLEN			R1
#define CMD41			(41) // LOCK UNLOCK				R1

// Application Specific Command (class 8)
#define CMD55			(55) // APP CMD					R1
#define CMD56			(56) // GEN CMD					R1

// Application Specific Commands used/reserved by SD Memory Card
#define ACMD6			(6)  // SET BUS WIDTH			R1
#define ACMD13			(13) // SD STATUS				R1
#define ACMD22			(22) // SEND NUM WR BLOCKS		R1
#define ACMD23			(23) // SET WR BLK ERASE COUNT	R1
#define ACMD41			(41) // SD SEND OP COND			R3
#define ACMD42			(42) // SET CLR CARD DETECT		R1
#define ACMD51			(51) // SEND SCR				R1

#define CMD_INDEX(x)			((x) << 8)
#define CMD_TYPE(t)				((t) << 6)
#define CMD_DATA_PRESENT(b)		((b) << 5)
#define CMD_IDX_CHECK(e)		((e) << 4)
#define CMD_CRC_CHECK(e)		((e) << 3)
#define CMD_RESP_TYPE(r)		((r) << 0)

#define NO_R					(CMD_IDX_CHECK(0) | CMD_CRC_CHECK(0) | CMD_RESP_TYPE(0))
#define R1						(CMD_IDX_CHECK(1) | CMD_CRC_CHECK(1) | CMD_RESP_TYPE(2))
#define R1b						(CMD_IDX_CHECK(1) | CMD_CRC_CHECK(1) | CMD_RESP_TYPE(3))
#define R2						(CMD_IDX_CHECK(0) | CMD_CRC_CHECK(1) | CMD_RESP_TYPE(1))
#define R3						(CMD_IDX_CHECK(0) | CMD_CRC_CHECK(0) | CMD_RESP_TYPE(2))
#define R4						(CMD_IDX_CHECK(0) | CMD_CRC_CHECK(0) | CMD_RESP_TYPE(2))
#define R5						(CMD_IDX_CHECK(1) | CMD_CRC_CHECK(1) | CMD_RESP_TYPE(2))
#define R5b						(CMD_IDX_CHECK(1) | CMD_CRC_CHECK(1) | CMD_RESP_TYPE(3))
#define R6						(CMD_IDX_CHECK(1) | CMD_CRC_CHECK(1) | CMD_RESP_TYPE(2))
#define R7						(CMD_IDX_CHECK(1) | CMD_CRC_CHECK(1) | CMD_RESP_TYPE(2))

#define SDHC_CMD0				(CMD_INDEX(CMD0)  | CMD_DATA_PRESENT(0) | NO_R)
#define SDHC_CMD1				(CMD_INDEX(CMD1)  | CMD_DATA_PRESENT(0) | R3)
#define SDHC_CMD2				(CMD_INDEX(CMD2)  | CMD_DATA_PRESENT(0) | R2)
#define SDHC_CMD3				(CMD_INDEX(CMD3)  | CMD_DATA_PRESENT(0) | R6)
#define SDHC_CMD4				(CMD_INDEX(CMD4)  | CMD_DATA_PRESENT(0) | NO_R)
#define SDHC_CMD6				(CMD_INDEX(CMD6)  | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD7				(CMD_INDEX(CMD7)  | CMD_DATA_PRESENT(0) | R1b)
#define SDHC_CMD8				(CMD_INDEX(CMD8)  | CMD_DATA_PRESENT(0) | R7)
#define SDHC_CMD9				(CMD_INDEX(CMD9)  | CMD_DATA_PRESENT(0) | R2)
#define SDHC_CMD10				(CMD_INDEX(CMD10) | CMD_DATA_PRESENT(0) | R2)
#define SDHC_CMD11				(CMD_INDEX(CMD11) | CMD_DATA_PRESENT(0) | R1)
#define SDHC_CMD12				(CMD_INDEX(CMD12) | CMD_DATA_PRESENT(0) | R1b)
#define SDHC_CMD13				(CMD_INDEX(CMD13) | CMD_DATA_PRESENT(0) | R1)
#define SDHC_CMD15				(CMD_INDEX(CMD15) | CMD_DATA_PRESENT(0) | NO_R)
#define SDHC_CMD16				(CMD_INDEX(CMD16) | CMD_DATA_PRESENT(0) | R1)
#define SDHC_CMD17				(CMD_INDEX(CMD17) | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD18				(CMD_INDEX(CMD18) | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD19				(CMD_INDEX(CMD19) | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD20				(CMD_INDEX(CMD20) | CMD_DATA_PRESENT(0) | R1b)
#define SDHC_CMD23				(CMD_INDEX(CMD23) | CMD_DATA_PRESENT(0) | R1)
#define SDHC_CMD24				(CMD_INDEX(CMD24) | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD25				(CMD_INDEX(CMD25) | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD27				(CMD_INDEX(CMD27) | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD28				(CMD_INDEX(CMD28) | CMD_DATA_PRESENT(0) | R1b)
#define SDHC_CMD29				(CMD_INDEX(CMD29) | CMD_DATA_PRESENT(0) | R1b)
#define SDHC_CMD30				(CMD_INDEX(CMD30) | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD32				(CMD_INDEX(CMD32) | CMD_DATA_PRESENT(0) | R1)
#define SDHC_CMD33				(CMD_INDEX(CMD33) | CMD_DATA_PRESENT(0) | R1)
#define SDHC_CMD38				(CMD_INDEX(CMD38) | CMD_DATA_PRESENT(0) | R1b)
#define SDHC_CMD42				(CMD_INDEX(CMD42) | CMD_DATA_PRESENT(1) | R1)
#define SDHC_CMD55				(CMD_INDEX(CMD55) | CMD_DATA_PRESENT(0) | R1)
#define SDHC_CMD56				(CMD_INDEX(CMD56) | CMD_DATA_PRESENT(1) | R1)

#define SDHC_ACMD6				(CMD_INDEX(ACMD6) | CMD_DATA_PRESENT(0) | R1)
#define SDHC_ACMD13				(CMD_INDEX(ACMD13)| CMD_DATA_PRESENT(1) | R1)
#define SDHC_ACMD22				(CMD_INDEX(ACMD22)| CMD_DATA_PRESENT(1) | R1)
#define SDHC_ACMD23				(CMD_INDEX(ACMD23)| CMD_DATA_PRESENT(0) | R1)
#define SDHC_ACMD41				(CMD_INDEX(ACMD41)| CMD_DATA_PRESENT(0) | R3)
#define SDHC_ACMD42				(CMD_INDEX(ACMD42)| CMD_DATA_PRESENT(0) | R1)
#define SDHC_ACMD51				(CMD_INDEX(ACMD51)| CMD_DATA_PRESENT(1) | R1)


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#pragma pack(1)
typedef union ArgCmd4_t
{
	uint32_t dword;
	struct {
		uint16_t rsvd;
		uint16_t dsr;
	} bm;
} ArgCmd4;

#pragma pack(1)
typedef union ArgCmd7_t
{
	uint32_t dword;
	struct {
		uint16_t rsvd;
		uint16_t rca;
	} bm;
} ArgCmd7;

#pragma pack(1)
typedef union ArgCmd8_t
{
	uint32_t dword;
	struct {
		uint32_t pattern : 8;
		uint32_t supply_voltage : 4;
		uint32_t rsvd:20;
	} bm;
} ArgCmd8;

#pragma pack(1)
typedef union ArgCmd9_t
{
	uint32_t dword;
	struct {
		uint16_t rsvd;
		uint16_t rca;
	} bm;
} ArgCmd9;

#pragma pack(1)
typedef union ArgCmd10_t
{
	uint32_t dword;
	struct {
		uint16_t rsvd;
		uint16_t rca;
	} bm;
} ArgCmd10;

#pragma pack(1)
typedef union ArgCmd13_t
{
	uint32_t dword;
	struct {
		uint16_t rsvd;
		uint16_t rca;
	} bm;
} ArgCmd13;

#pragma pack(1)
typedef union ArgCmd15_t
{
	uint32_t dword;
	struct {
		uint16_t rsvd;
		uint16_t rca;
	} bm;
} ArgCmd15;

#pragma pack(1)
typedef union ArgCmd16_t
{
	uint32_t dword;
	uint32_t block_length;
} ArgCmd16;

#pragma pack(1)
typedef union ArgCmd17_t
{
	uint32_t dword;
	uint32_t data_address;
} ArgCmd17;

#pragma pack(1)
typedef union ArgCmd18_t
{
	uint32_t dword;
	uint32_t data_address;
} ArgCmd18;

#pragma pack(1)
typedef union ArgCmd20_t
{
	uint32_t dword;
	struct {
		uint32_t rsvd : 28;
		uint32_t speed_class_control : 4;
	} bm;
} ArgCmd20;

#pragma pack(1)
typedef union ArgCmd23_t
{
	uint32_t dword;
	uint32_t block_count;
} ArgCmd23;

#pragma pack(1)
typedef union ArgCmd24_t
{
	uint32_t dword;
	uint32_t data_address;
} ArgCmd24;

#pragma pack(1)
typedef union ArgCmd25_t
{
	uint32_t dword;
	uint32_t data_address;
} ArgCmd25;

#pragma pack(1)
typedef union ArgCmd28_t
{
	uint32_t dword;
	uint32_t data_address;
} ArgCmd28;

#pragma pack(1)
typedef union ArgCmd29_t
{
	uint32_t dword;
	uint32_t data_address;
} ArgCmd29;

#pragma pack(1)
typedef union ArgCmd30_t
{
	uint32_t dword;
	uint32_t write_protect_data_address;
} ArgCmd30;

#pragma pack(1)
typedef union ArgCmd32_t
{
	uint32_t dword;
	uint32_t data_address;
} ArgCmd32;

#pragma pack(1)
typedef union ArgCmd33_t
{
	uint32_t dword;
	uint32_t data_address;
} ArgCmd33;

#pragma pack(1)
typedef union ArgCmd55_t
{
	uint32_t dword;
	struct {
		uint16_t rsvd;
		uint16_t rca;
	} bm;
} ArgCmd55;

#pragma pack(1)
typedef union ArgCmd56_t
{
	uint32_t dword;
	struct {
		uint32_t rd_wr : 1;
		uint32_t rsvd : 31;
	} bm;
} ArgCmd56;

#pragma pack(1)
typedef union ArgAcmd6_t
{
	uint32_t dword;
	struct {
		uint32_t bus_width : 2;
		uint32_t rsvd : 30;
	} bm;
} ArgAcmd6;

#pragma pack(1)
typedef union ArgAcmd23_t
{
	uint32_t dword;
	struct {
		uint32_t number_of_blocks : 23;
		uint32_t rsvd : 9;
	} bm;
} ArgAcmd23;

#pragma pack(1)
typedef union ArgAcmd41_t
{
	uint32_t dword;
	struct {
		uint32_t rsvd0 : 8;
		uint32_t ocr : 16;
		uint32_t s18r : 1;
		uint32_t rsvd : 3;
		uint32_t xpc : 1;
		uint32_t rsvd2 : 1;
		uint32_t hcs : 1;
		uint32_t rsvd3 : 1;
	} bm;
} ArgAcmd41;

#pragma pack(1)
typedef union ArgCmd42_t
{
	uint32_t dword;
	struct {
		uint32_t set_cd : 1;
		uint32_t rsvd : 31;
	} bm;
} ArgCmd42;




//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#pragma pack(1)
typedef union SdcOcrReg_t
{
	uint32_t dword;
	struct {
		uint32_t rsvd : 15;
		uint32_t voltage_window : 9;
		uint32_t s18a : 1;
		uint32_t rsvd2 : 5;
		uint32_t card_capacity_status : 1;
		uint32_t busy : 1;
	} bm;
} SdcOcrReg;

#pragma pack(1)
typedef union SdcCidReg_t
{
	uint8_t array[16];
	struct {
		//		uint8_t unused : 1;
		//		uint8_t crc7 : 7;
		uint16_t manufacturing_date : 12;
		uint16_t rsvd : 4;
		uint32_t product_serial_number : 32;
		uint8_t product_revision : 8;
		uint8_t product_name[5];
		uint16_t oem_application_id;
		uint8_t manufacturer_id;
	} bm;
} SdcCidReg;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#pragma pack(1)
typedef union SdcCsdReg_t
{
	uint8_t array[16];
	struct {
		//		uint8_t unused : 1;
		//		uint8_t crc7 : 7;
		uint8_t rsvd : 2;
		uint8_t file_format : 2;
		uint8_t temporary_write_protection : 1;
		uint8_t permanent_write_protection : 1;
		uint8_t copy_flag : 1;
		uint8_t file_format_group : 1;
		uint8_t rsvd2 : 5;
		uint8_t oartial_blocks_for_write_allowed : 1;
		uint8_t max_write_data_block_length : 4;
		uint8_t write_speed_factor : 3;
		uint8_t rsvd3 : 2;
		uint8_t write_protect_group_enable : 1;
		uint8_t write_protect_group_size : 7;
		uint8_t erase_sector_size : 7;
		uint8_t erase_single_block_enable : 1;
		uint8_t device_size_multiplier : 3;
		uint8_t max_write_current_at_vdd_max : 3;
		uint8_t max_write_current_at_vdd_min : 3;
		uint8_t max_read_current_at_vdd_max : 3;
		uint8_t max_read_current_at_vdd_min : 3;
		uint16_t device_size : 12;
		uint8_t rsvd4 : 2;
		uint8_t dsr_implemented : 1;
		uint8_t read_block_misalignment : 1;
		uint8_t write_block_misalignment : 1;
		uint8_t partial_blocks_for_read_allowed : 1;
		uint8_t max_read_data_block_length : 4;
		uint16_t card_command_classs : 12;
		uint8_t max_data_transfer_rate : 8;
		uint8_t nsac : 8;
		uint8_t taac : 8;
		uint8_t rsvd5 : 6;
		uint8_t csd_structure : 2;
	} v1;
	struct {
		//		uint8_t unused : 1;
		//		uint8_t crc7 : 7;
		uint8_t rsvd : 2;
		uint8_t file_format : 2;
		uint8_t temporary_write_protection : 1;
		uint8_t permanent_write_protection : 1;
		uint8_t copy_flag : 1;
		uint8_t file_format_group : 1;
		uint8_t rsvd2 : 5;
		uint8_t oartial_blocks_for_write_allowed : 1;
		uint8_t max_write_data_block_length : 4;
		uint8_t write_speed_factor : 3;
		uint8_t rsvd3 : 2;
		uint8_t write_protect_group_enable : 1;
		uint8_t write_protect_group_size : 7;
		uint8_t erase_sector_size : 7;
		uint8_t erase_single_block_enable : 1;
		uint8_t rsvd4 : 1;
		uint32_t device_size : 22;
		uint8_t rsvd5 : 6;
		uint8_t dsr_implemented : 1;
		uint8_t read_block_misalignment : 1;
		uint8_t write_block_misalignment : 1;
		uint8_t partial_blocks_for_read_allowed : 1;
		uint8_t max_read_data_block_length : 4;
		uint16_t card_command_classs : 12;
		uint8_t max_data_transfer_rate : 8;
		uint8_t nsac : 8;
		uint8_t taac : 8;
		uint8_t rsvd6 : 6;
		uint8_t csd_structure : 2;
	} v2;
	struct {
		uint8_t  reserved4 : 1;
		uint8_t  _7bit_crc : 7;
		uint8_t  ecc_code : 2;
		uint8_t  file_format : 2;
		uint8_t  temporary_write_protection : 1;
		uint8_t  permanent_write_protection : 1;
		uint8_t  copy_flag_otp : 1;
		uint8_t  file_format_group : 1;
		uint8_t  reserved3 : 5;
		uint8_t  pararel_blocks_for_write_allowed : 1;
		uint8_t  max_write_data_block_length : 4;
		uint8_t  write_speed_factor : 3;
		uint8_t  manufacturer_default_ecc : 2;
		uint8_t  write_protect_group_enable : 1;
		uint8_t  write_protect_group_size : 5;
		uint8_t  erase_group_size_multplier : 5;
		uint8_t  erase_group_size : 5;
		uint8_t  devie_size_multiplier : 3;
		uint8_t  max_write_current_at_vdd_min : 3;
		uint8_t  max_write_current_at_vdd_max : 3;
		uint8_t  max_read_current_at_vdd_min : 3;
		uint8_t  max_read_current_at_vdd_max : 3;
		uint16_t device_size : 12;
		uint8_t  reserved2 : 2;
		uint8_t  dsr_implemented : 1;
		uint8_t  read_block_misalignment : 1;
		uint8_t  write_block_misalignement : 1;
		uint8_t  partial_blocks_for_read_allowed : 1;
		uint16_t max_read_data_block_length : 4;
		uint16_t card_command_class : 12;
		uint8_t  max_data_transfer_rate : 8;
		uint8_t  data_read_access_time2_in_clk : 8;
		uint8_t  data_read_access_time1 : 8;
		uint8_t  rserved : 2;
		uint8_t  spec_version : 4;
		uint8_t  signature : 2;
	} mmc;
} SdcCsdReg;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#pragma pack(1)
typedef union SdcR1_t
{
	uint32_t card_status;
	struct {
		uint32_t rsvd : 3;
		uint32_t ake_seq_error : 1;
		uint32_t rsvd2: 1;
		uint32_t app_cmd : 1;
		uint32_t rsvd3 : 1;
		uint32_t ready_for_data : 1;
		uint32_t current_state : 4;
		uint32_t erase_reset : 1;
		uint32_t card_ecc_disabled : 1;
		uint32_t wp_erase_skip : 1;
		uint32_t csd_overwrite : 1;
		uint32_t rsvd4 : 2;
		uint32_t error : 1;
		uint32_t cc_error : 1;
		uint32_t card_ecc_failed : 1;
		uint32_t illigal_command : 1;
		uint32_t com_crc_error : 1;
		uint32_t lock_unlock_failed : 1;
		uint32_t card_is_locked : 1;
		uint32_t wp_violation : 1;
		uint32_t erase_param : 1;
		uint32_t erase_seq_error : 1;
		uint32_t block_len_error : 1;
		uint32_t address_error : 1;
		uint32_t out_of_range : 1;
	};
} SdcR1;

#pragma pack(1)
typedef union SdcR2_t
{
	uint8_t bytes[16];
	SdcCsdReg csd;
	SdcCidReg cid;
} SdcR2;

#pragma pack(1)
typedef SdcOcrReg SdcR3;

#pragma pack(1)
typedef struct SdcR6_t
{
	uint16_t card_status;
	uint16_t new_published_rca;
} SdcR6;

#pragma pack(1)
typedef struct SdcR7_t
{
	uint8_t echo_back_of_check_platform : 8;
	uint8_t voltage_accepted : 4;
	uint32_t rsvd : 20;
} SdcR7;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#pragma pack(1)
typedef struct SdhcAdmaDescriptor_t
{
	uint16_t attr_valid : 1;
	uint16_t attr_end : 1;
	uint16_t attr_int : 1;
	uint16_t attr_zero : 1;
	uint16_t attr_act : 2;
	uint16_t reserved : 10;
	uint16_t length;
	uint32_t address;
} SdhcAdmaDescriptor;

typedef enum SAMD51_SDHC_CARD_TYPE_t
{
	SAMD51_SDHC_UNKNOWN = 0,
	SAMD51_SDHC_MMC = 1,
	SAMD51_SDHC_SDv1 = 2,
	SAMD51_SDHC_SDv2 = 3
} SAMD51_SDHC_CARD_TYPE;

#define SAMD51_SDHC_CARD_STATUS_NODISK		(0x00000001)
#define SAMD51_SDHC_CARD_STATUS_NOINIT		(0x00000002)

typedef struct Samd51_sdhcCardContext_t
{
	uint32_t status;
	uint32_t rca;
	SAMD51_SDHC_CARD_TYPE type;
	int isBlockDevice;
	
	SdcCidReg cid;
	SdcCsdReg csd;
} Samd51_sdhcCardContext;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
typedef void (*Samd51SdhcTransactionDoneCallback)(int status, const void *resp, const size_t resp_len);
typedef void (*Samd51SdhcErrorCallback)(int error_code);


//----------------------------------------------------------------------------
int samd51_sdhc_initialize(SAMD51_SDHC sdhc);
void samd51_sdhc_finalize(SAMD51_SDHC sdhc);
int samd51_sdhc_get_host_controller_info(SAMD51_SDHC sdhc, char *info, size_t info_len);
int samd51_sdhc_get_host_controller_version(SAMD51_SDHC sdhc);

int samd51_sdhc_is_card_inserted(SAMD51_SDHC sdhc);

int samd51_sdhc_enable_sd_clock(SAMD51_SDHC sdhc, uint32_t BaseClock, uint32_t sdclkHz, int enable);
int samd51_sdhc_sd_bus_power_control(SAMD51_SDHC sdhc, SAMD51_SDHC_BUS_VOLTAGE bus_voltage);

int samd51_sdhc_send_cmd_blocing(SAMD51_SDHC sdhc, uint16_t cmd, uint32_t arg, void *resp, size_t resp_len);
int samd51_sdhc_send_cmd(SAMD51_SDHC sdhc, uint16_t cmd, uint32_t arg, Samd51SdhcTransactionDoneCallback cb);


int samd51_sdhc_card_initialization_and_identification(SAMD51_SDHC sdhc, Samd51_sdhcCardContext *ctx);
int samd51_transfer(SAMD51_SDHC sdhc,  Samd51_sdhcCardContext *ctx, int dir_read, uint32_t sector, void *buf, uint32_t length_byte, int blocking);

int samd51_sdhc_changing_bus_width(SAMD51_SDHC sdhc, uint16_t rca, SAMD51_SDHC_BUS_WIDTH bus_width);
int samd51_sdhc_timeout_setting_on_dat_line(SAMD51_SDHC sdhc, uint32_t timeout_us);

int samd51_sdhc_signal_voltage_switch(SAMD51_SDHC sdhc, SAMD51_SDHC_BUS_VOLTAGE voltage);

#endif/*SAMD51_SDHC_H_*/
