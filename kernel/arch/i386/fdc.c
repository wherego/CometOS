#include <stdint.h>

#include <kernel/portio.h>
#include "../arch/i386/fdc.h"
#include "../arch/i386/log.h"
#include "../arch/i386/pit.h"

const int FLOPPY_IRQ = 6;
const int FDC_DMA_CHANNEL = 2;
const int FLOPPY_SECTORS_PER_TRACK = 18;
int DMA_BUFFER = NULL;

static uint8_t	CurrentDrive = 0;
static volatile uint8_t FloppyDiskIRQ = 0;

void floppy_set_dma(int addr)
{
	DMA_BUFFER = addr;
}

int floppy_dma_initialize(uint8_t* buffer, unsigned length)
{
	union
	{
      uint8_t byte[4];//Lo[0], Mid[1], Hi[2]
      unsigned long l;
   }a, c;

   a.l = (unsigned)buffer;
   c.l = (unsigned)length - 1;

   //Check for buffer issues
   if ((a.l >> 24) || (c.l >> 16) || (((a.l & 0xffff)+c.l) >> 16))
   {
		return 0;
   }

   dma_reset (1);
   dma_mask_channel( FDC_DMA_CHANNEL );//Mask channel 2
   dma_reset_flipflop ( 1 );//Flipflop reset on DMA 1

   dma_set_address( FDC_DMA_CHANNEL, a.byte[0],a.byte[1]);//Buffer address
   dma_reset_flipflop( 1 );//Flipflop reset on DMA 1

   dma_set_count( FDC_DMA_CHANNEL, c.byte[0],c.byte[1]);//Set count
   dma_set_read ( FDC_DMA_CHANNEL );

   dma_unmask_all( 1 );//Unmask channel 2

   return 1;
}

uint8_t floppy_read_status(void)
{
	return inport8(FLOPPY_MSR);
}

void floppy_write_dor(uint8_t val)
{
	outport8(FLOPPY_DOR, val);
}

void floppy_send_command(uint8_t cmd)
{
	for (int i = 0; i < 500; i++)
	{
		if (floppy_read_status() & FLOPPY_MSR_MASK_DATAREG)
		{
			return outport8(FLOPPY_FIFO, cmd);
		}
	}
}

uint8_t floppy_read_data(void)
{
	for (int i = 0; i < 500; i++)
	{
		if ( floppy_read_status() & FLOPPY_MSR_MASK_DATAREG)
		{
			return inport8(FLOPPY_FIFO);
		}
	}

	return 0;
}

void floppy_write_ccr(uint8_t val)
{
	outport8(FLOPPY_CTRL, val);
}

inline void floppy_wait_irq(void)
{
	while (FloppyDiskIRQ == 0);
	FloppyDiskIRQ = 0;
}

void floppy_irq(void)
{
	FloppyDiskIRQ = 1;
}

void floppy_check_int (uint32_t* st0, uint32_t* cyl)
{
	floppy_send_command(FDC_CMD_CHECK_INT);

	*st0 = floppy_read_data ();
	*cyl = floppy_read_data ();
}

void floppy_control_motor (int b)
{
	if (CurrentDrive > 3)
	{
		return;
	}

	uint32_t motor = 0;

	switch (CurrentDrive)
	{
		case 0:
			motor = FLOPPY_DOR_MASK_DRIVE0_MOTOR;
			break;
		case 1:
			motor = FLOPPY_DOR_MASK_DRIVE1_MOTOR;
			break;
		case 2:
			motor = FLOPPY_DOR_MASK_DRIVE2_MOTOR;
			break;
		case 3:
			motor = FLOPPY_DOR_MASK_DRIVE3_MOTOR;
			break;
	}

	if (b)
	{
		floppy_write_dor(CurrentDrive | motor | FLOPPY_DOR_MASK_RESET | FLOPPY_DOR_MASK_DMA);
	}
	else
	{
		floppy_write_dor(FLOPPY_DOR_MASK_RESET);
	}

	sleep(20);
}

void floppy_drive_data(uint32_t stepr, uint32_t loadt, uint32_t unloadt, int dma)
{

	uint32_t data = 0;

	floppy_send_command(FDC_CMD_SPECIFY);
	data = ( (stepr & 0xf) << 4) | (unloadt & 0xf);

	floppy_send_command(data);
	data = (loadt) << 1 | (dma==0) ? 0 : 1;

	floppy_send_command(data);
}

int floppy_calibrate(uint32_t drive)
{

	uint32_t st0, cyl;

	if (drive >= 4)
	{
		return -2;
	}

	floppy_control_motor(1);

	for (int i = 0; i < 10; i++)
	{
		floppy_send_command(FDC_CMD_CALIBRATE);
		floppy_send_command(drive);
		floppy_wait_irq();
		floppy_check_int(&st0, &cyl);

		if (!cyl)
		{
			floppy_control_motor (0);
			return 0;
		}
	}

	floppy_control_motor (0);
	return -1;
}

void floppy_disable_controller(void)
{
	floppy_write_dor (0);
}

void floppy_enable_controller(void)
{
	floppy_write_dor(FLOPPY_DOR_MASK_RESET | FLOPPY_DOR_MASK_DMA);
}

void floppy_reset(void)
{
	uint32_t st0, cyl;

	floppy_disable_controller();
	floppy_enable_controller();
	floppy_wait_irq();

	for (int i=0; i<4; i++)
	{
		floppy_check_int (&st0,&cyl);
	}

	floppy_write_ccr (0);
	floppy_drive_data (3,16,240, 1);
	floppy_calibrate (CurrentDrive);
}

void floppy_read_sector_imp(uint8_t head, uint8_t track, uint8_t sector)
{
	uint32_t st0, cyl;

	//! initialize DMA
	floppy_dma_initialize((uint8_t*) DMA_BUFFER, 512);
	dma_set_read(FDC_DMA_CHANNEL);

	floppy_send_command(FDC_CMD_READ_SECT | FDC_CMD_EXT_MULTITRACK | FDC_CMD_EXT_SKIP | FDC_CMD_EXT_DENSITY);
	floppy_send_command(head << 2 | CurrentDrive);
	floppy_send_command(track);
	floppy_send_command(head);
	floppy_send_command(sector);
	floppy_send_command(FLOPPY_SECTOR_DTL_512 );
	floppy_send_command(((sector + 1) >= FLOPPY_SECTORS_PER_TRACK ) ? FLOPPY_SECTORS_PER_TRACK : sector + 1);
	floppy_send_command(FLOPPY_GAP3_LENGTH_3_5);
	floppy_send_command(0xff);

	floppy_wait_irq();

	for (int j=0; j<7; j++)
	{
		floppy_read_data();
	}

	floppy_check_int(&st0,&cyl);
}

int floppy_seek(uint32_t cyl, uint32_t head)
{
	uint32_t st0, cyl0;

	if (CurrentDrive >= 4)
	{
		return -1;
	}

	for (int i = 0; i < 10; i++)
	{
		floppy_send_command(FDC_CMD_SEEK);
		floppy_send_command((head) << 2 | CurrentDrive);
		floppy_send_command(cyl);

		floppy_wait_irq();
		floppy_check_int(&st0,&cyl0);

		if (cyl0 == cyl)
		{
			return 0;
		}
	}

	return -1;
}

void floppy_convert(int lba,int *head,int *track,int *sector)
{
   *head = (lba % (FLOPPY_SECTORS_PER_TRACK * 2)) / (FLOPPY_SECTORS_PER_TRACK);
   *track = lba / (FLOPPY_SECTORS_PER_TRACK * 2);
   *sector = lba % FLOPPY_SECTORS_PER_TRACK + 1;
}

void floppy_initialize(int irq)
{
	irq_install_handler(irq, floppy_irq);
	floppy_set_dma(malloc(512));
	floppy_reset();
	floppy_drive_data(13, 1, 0xf, 1);
	log_print(NOTICE, "FDC");
}

void floppy_drive_set(uint8_t drive)
{
	if (drive < 4)
	{
		CurrentDrive = drive;
	}
}

uint8_t floppy_drive_get(void)
{

	return CurrentDrive;
}

uint8_t* floppy_read(int sectorLBA)
{

	if(CurrentDrive >= 4)
	{
		return 0;
	}

	int head=0, track=0, sector=1;
	floppy_convert(sectorLBA, &head, &track, &sector);

	floppy_control_motor(1);
	if (floppy_seek(track, head) != 0)
	{
		return 0;
	}

	floppy_read_sector_imp(head, track, sector);
	floppy_control_motor(0);

	return (uint8_t*)DMA_BUFFER;
}