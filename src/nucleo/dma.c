/*
 * Modulo tomado del sistema operativo EduOS
 * MUCHAS GRACIAS Frank Cornelis!!!!!!!!!!
 * Publicado bajo la licencia GNU
 */
#include <sodero/dma.h>
#include <video.h>

/* Quick-access registers and ports for each DMA channel. */
static byte dma_mask [8] = { 0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4 };
static byte dma_mode [8] = { 0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6 };
static byte dma_clear [8] = { 0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8 };

static byte dma_page [8] = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };
static byte dma_addr [8] = { 0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC };
static byte dma_count [8] = { 0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE };


void setup_dma (dword channel, dword data_ptr, word length, 
		transfer_type trans, mode_type mode,
		BOOL auto_init, BOOL address_inc) {
  dword offset, page;
  MODE_REG mode_reg;

  /* extract offset and page from the given data pointer */
  offset = (data_ptr & 0xFFFF);
  page = (data_ptr >> 16);

  #ifdef DEBUG
  imprimir ( "offset: 0x%x ", offset);
  imprimir ( "page: 0x%x\n", page);
  #endif

  /* prepare mode register fields */
  mode_reg.fields.channel = channel % 4;
  mode_reg.fields.transfer_type = trans;
  mode_reg.fields.auto_init = auto_init;
  mode_reg.fields.address_inc = address_inc;
  mode_reg.fields.mode = mode;

  /* Don't let anyone else mess up what we're doing. */
  cli ();

  /* Set up the DMA channel so we can use it.  This tells the DMA */
  /* that we're going to be using this channel.  (It's masked) */
  pause_dma (channel);

  /* Clear any data transfers that are currently executing. */
  outb (0x00, dma_clear [channel]);

  /* Send the specified mode to the DMA. */
  outb ( mode_reg.value, dma_mode [channel] );

  /* Send the offset address.  The first byte is the low base offset, the */
  /* second byte is the high offset. */
  outb ( offset & 0xFF, dma_addr [channel] );
  outb ( ((offset >> 8) & 0xFF), dma_addr [channel] );

  /* Send the physical page that the data lies on. */
  outb ( page, dma_page [channel] );

  /* Send the length of the data.  Again, low byte first. */
  outb ( ((length-1) & 0xFF), dma_count [channel] );
  outb ( (((length-1) >> 8) & 0xFF), dma_count [channel] );

  /* Ok, we're done.  Enable the DMA channel (clear the mask). */
  unpause_dma (channel);

  /* Re-enable interrupts before we leave. */
  sti ();
}


void pause_dma (byte channel) {
  /* All we have to do is mask the DMA channel's bit on. */
  MASK_REG mask;
  mask.fields.channel = channel % 4;
  mask.fields.mask = 1;
  outb ( mask.value, dma_mask [channel] );
}


void unpause_dma (byte channel) {
  /* Simply clear the mask, and the DMA continues where it left off. */
  MASK_REG mask;
  mask.fields.channel = channel % 4;
  mask.fields.mask = 0;
  outb ( mask.value, dma_mask [channel] );
}


void stop_dma (byte channel) {
  /* We need to set the mask bit for this channel, and then clear the */
  /* selected channel.  Then we can clear the mask. */
  pause_dma (channel);
  /* Send the clear command. */
  outb ( 0x00, dma_clear [channel] );
  /* And clear the mask. */
  unpause_dma (channel);
}

