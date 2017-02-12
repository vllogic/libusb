#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <time.h>

#include "libusb.h"


#define VL1602ED_VID	0xB58A
#define VL1602ED_PID	0xC200

#define VLLOGIC_REQUESET_REG_RW				0x00


struct ctrl_regs_t
{
	// read only area
	uint32_t version;

	uint32_t board_in_channels_mask;
	uint32_t board_out_channels_mask;

	uint32_t status_mask;

	// read write area
	uint32_t command;
#define VLLOGIC_CMD_NOP						0
#define VLLOGIC_CMD_START					0x10000000
#define VLLOGIC_CMD_STOP					0x10000001

	uint32_t mode;
#define VLLOGIC_MODE_NOP					0
#define VLLOGIC_MODE_IN						(0x1 << 0)
#define VLLOGIC_MODE_OUT					(0x1 << 1)

	// channel parameter
	uint32_t sum_l32;
	uint32_t sum_h32;
	uint32_t rate;

	/*
	bit			function
	[0, 15] 	digital channels enable mask
	[16, 17]	analog channels enable mask
	*/
	uint32_t channels_in_enable_mask;

	/*
	bit			function
	[0, 15] 	low level trigger enable mask
	[16, 32]	high level trigger enable mask
	*/
	uint32_t digital_channels_level_trigger_mask;

	/*
	bit			function
	[0, 15] 	falling edge trigger enable mask
	[16, 32]	rising edge trigger enable mask
	*/
	uint32_t digital_channels_edge_trigger_mask;

	/*
	bit			function
	[0, 15] 	digital channels enable mask
	[16, 17]	analog channels enable mask
	*/
	uint32_t channels_out_enable_mask;
};

uint8_t buf_20k[20 * 1024 * 1024];

static int vl1602ed_vllogic_test(libusb_device_handle *handle)
{
	int r;
	struct ctrl_regs_t regs;

	// control test
	r = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
		VLLOGIC_REQUESET_REG_RW, 0, 0, (uint8_t *)&regs, sizeof(struct ctrl_regs_t), 100);
	if (r != sizeof(struct ctrl_regs_t))
	{
		printf("Get Regs Fail\n");
		return;
	}
		
	regs.rate = 240000000;

	libusb_control_transfer(handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
		VLLOGIC_REQUESET_REG_RW, 0, 0, (uint8_t *)&regs, sizeof(struct ctrl_regs_t), 100);
	memset(&regs, 0, sizeof(struct ctrl_regs_t));

	r = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
		VLLOGIC_REQUESET_REG_RW, 0, 0, (uint8_t *)&regs, sizeof(struct ctrl_regs_t), 100);
	if (r != sizeof(struct ctrl_regs_t))
	{
		printf("Get Regs Fail\n");
		return;
	}


	//while (1)
	{
		int transfer_len = 0, sum = 0;
		clock_t start, end;

		start = clock();
		printf("\n20k Block Test \n");
		printf("Start Clock %d\n", start);
		while (sum < 100 * 1024 * 1024)
		{
			r = libusb_bulk_transfer(handle, 0x80 | 0x1, buf_20k, 20 * 1024, &transfer_len, 1000);
			if (r == LIBUSB_ERROR_PIPE) {
				libusb_clear_halt(handle, 1);
				printf("Get 20k buffer Fail\n");
				break;
			}
			else if (transfer_len == 20 * 1024)
			{
				sum += transfer_len;
			}
			else
			{
				printf("Get buffer %d\n", transfer_len);
				break;
			}
		}
		end = clock();
		printf("End Clock %d\n", end);
		float speed = 100000.0 / (end - start);
		printf("Speed %f MB/S\n", speed);
	}
	{
		int transfer_len = 0, sum = 0;
		clock_t start, end;

		start = clock();
		printf("\n1MB Block Test \n");
		printf("Start Clock %d\n", start);
		while (sum < 100 * 1024 * 1024)
		{
			r = libusb_bulk_transfer(handle, 0x80 | 0x1, buf_20k, 1024 * 1024, &transfer_len, 1000);
			if (r == LIBUSB_ERROR_PIPE) {
				libusb_clear_halt(handle, 1);
				printf("Get 20k buffer Fail\n");
				break;
			}
			else if (transfer_len == 1024 * 1024)
			{
				sum += transfer_len;
			}
			else
			{
				printf("Get buffer %d\n", transfer_len);
				break;
			}
		}
		end = clock();
		printf("End Clock %d\n", end);
		float speed = 100000.0 / (end - start);
		printf("Speed %f MB/S\n", speed);
	}
	{
		int transfer_len = 0, sum = 0;
		clock_t start, end;

		start = clock();
		printf("\n10MB Block Test \n");
		printf("Start Clock %d\n", start);
		while (sum < 100 * 1024 * 1024)
		{
			r = libusb_bulk_transfer(handle, 0x80 | 0x1, buf_20k, 10 * 1024 * 1024, &transfer_len, 1000);
			if (r == LIBUSB_ERROR_PIPE) {
				libusb_clear_halt(handle, 1);
				printf("Get 20k buffer Fail\n");
				break;
			}
			else if (transfer_len == 10 * 1024 * 1024)
			{
				sum += transfer_len;
			}
			else
			{
				printf("Get buffer %d\n", transfer_len);
				break;
			}
		}
		end = clock();
		printf("End Clock %d\n", end);
		float speed = 100000.0 / (end - start);
		printf("Speed %f MB/S\n", speed);
	}
	{
		int transfer_len = 0, sum = 0;
		clock_t start, end;

		start = clock();
		printf("\n20MB Block Test \n");
		printf("Start Clock %d\n", start);
		while (sum < 100 * 1024 * 1024)
		{
			r = libusb_bulk_transfer(handle, 0x80 | 0x1, buf_20k, 20 * 1024 * 1024, &transfer_len, 1000);
			if (r == LIBUSB_ERROR_PIPE) {
				libusb_clear_halt(handle, 1);
				printf("Get 20k buffer Fail\n");
				break;
			}
			else if (transfer_len == 20 * 1024 * 1024)
			{
				sum += transfer_len;
			}
			else
			{
				printf("Get buffer %d\n", transfer_len);
				break;
			}
		}
		end = clock();
		printf("End Clock %d\n", end);
		float speed = 100000.0 / (end - start);
		printf("Speed %f MB/S\n", speed);
	}
}

static int vl1602ed_test(void)
{
	int r;
	libusb_device_handle *handle;
	libusb_device *dev;

	printf("Opening device %04X:%04X...\n", VL1602ED_VID, VL1602ED_PID);
	handle = libusb_open_device_with_vid_pid(NULL, VL1602ED_VID, VL1602ED_PID);
	if (handle == NULL)
	{
		printf("  Open Failed.\n");
		return -1;
	}
	else
		printf("  Find Device.\n");

	dev = libusb_get_device(handle);

	libusb_set_auto_detach_kernel_driver(handle, 1);
	printf("Claim Interface 0\n");
	r = libusb_claim_interface(handle, 0);
	if (r != LIBUSB_SUCCESS)
	{
		printf("   Failed.\n");
	}
	else
	{
		r = libusb_claim_interface(handle, 1);

		if (r == LIBUSB_SUCCESS)
		{
			printf("   Success.\n");
			vl1602ed_vllogic_test(handle);
		}
	}
	//printf("Claim Interface 1\n");
	//r = libusb_claim_interface(handle, 0);
	//if (r != LIBUSB_SUCCESS)
	//{
	//	printf("   Failed.\n");
	//}

	printf("Closing device...\n");
	libusb_close(handle);
}

int main(int argc, char** argv)
{
	int j, r;
	const struct libusb_version* version;

	version = libusb_get_version();
	printf("Using libusb v%d.%d.%d.%d\n\n", version->major, version->minor, version->micro, version->nano);
	r = libusb_init(NULL);
	if (r < 0)
		return r;

	vl1602ed_test();

	libusb_exit(NULL);

	return 0;
}

