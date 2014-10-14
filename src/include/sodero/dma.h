#ifndef DMA_H
#define DMA_H

#include <sodero/definiciones.h>
#include <sodero/registros.h>
#include <sodero/puertos.h>

#define BOOL byte

typedef struct {
  unsigned channel:2;
  unsigned mask:1;
  unsigned dont_care:5;
} NOALIGN _MASK_REG;

#define CHANNEL0 0
#define CHANNEL1 1
#define CHANNEL2 2
#define CHANNEL3 3

typedef union {
  _MASK_REG fields;
  byte value;
} MASK_REG;

typedef struct {
  unsigned channel:2;
  unsigned transfer_type:2;
  unsigned auto_init:1;
  unsigned address_inc:1;
  unsigned mode:2;
} NOALIGN _MODE_REG;

#define VERIFY_TRANSFER 0
#define WRITE_TRANSFER 1
#define READ_TRANSFER 2

typedef enum {VERIFY, WRITE, READ} transfer_type;

#define DEMAND_MODE 0
#define SINGLE_MODE 1
#define BLOCK_MODE 2
#define CASCADE_MODE 3

typedef enum {DEMAND, SINGLE, BLOCK, CASCADE} mode_type;

typedef union {
  _MODE_REG fields;
  byte value;
} MODE_REG;

/* public functions */
void setup_dma (dword channel, dword data_ptr, word length, 
		transfer_type trans, mode_type mode,
		BOOL auto_init, BOOL address_inc);
void pause_dma (byte channel);
void unpause_dma (byte channel);
void stop_dma (byte channel);

#endif
