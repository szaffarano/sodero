#include <sodero/cdrom.h>

//#define DEBUG 1

void ejecutar_cd ( int disp ) {
   word comando[12] = { CDROM_PLAY, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0 };
   comando[8] = 1;

   cdrom_cmd ( disp, 0, NULL, comando );
}

int cdrom_cmd ( int idx, unsigned long num, word* buffer, word* pkt ) {

   byte fase;
   word puerto = descs_ide[idx].puerto_base;
   byte id = descs_ide[idx].id_dispositivo;

   imprimir ( "Se va a ejecutar %xw %xb\n", puerto, id );

   atapi ( puerto, id );

   /*

   si reemplazo esto por atapi() funciona OK (?!?!?!)

   if ( !seleccionar_dispositivo ( puerto, id ) ) {
      imprimir ( "Error al seleccionar el dispositivo\n" );
   }

   outb ( 0, puerto + REG_FEATURE );
   outb ( ((word)(32768 & 0xFF)), puerto + REG_LSB_CYL );
   outb ( 32768 >> 8, puerto + REG_LSB_CYL );
   outb ( 0x40 | id, puerto + REG_SEL_DRIVER );
   delay ( MICRO(40000L) );

   outb ( COMANDO_PAQUETE,  puerto + COMANDO_PAQUETE );
   delay ( MICRO(40000L) );

   if ( leer_estado ( puerto, 0x88, 0x08, 50 ) ) {
      imprimir ( "Error al leer estado!\n");
      return -1;
   }

   fase = inb ( puerto + REG_ESTADO ) & 0x08;
   fase |= (inb (puerto + REG_MOTIVO)) & 0x03;

   if ( fase != FASE_SIN_COMANDO ) {
      imprimir ("Fase erronea: %xb\n", fase);
      return -1;
   }
   */

   escribir_puerto ( puerto + REG_DATOS, (word*) pkt, 12 );

   while (1) {
      if ( esperar_irq ( 0xC000, 1000 ) == 0 ) {
         imprimir ( "No se disparo la IRQ!!\n" );
      }

      delay ( MICRO(10000L) );

      fase = inb ( puerto + REG_ESTADO ) & 0x08;
      fase |= (inb (puerto + REG_MOTIVO)) & 0x03;

      if ( fase == FASE_FINALIZO ) {
         imprimir ( "Escritura exitosa!\n" );
         return 0;
      }
   }
   return -1;
}

void atapi ( dword base, byte id ) {
   int i;

   outb ( id, base+6 );
   delay ( MICRO(100000L) );

   if ( (inb (base+7) & 0x80) != 0 ) {
      //.res
      outb ( 0x8, base+7 );
      outb ( 0xE, base+0x206 );
      delay ( MICRO(10000L) );
      outb ( 0x8, base+0x206 );
      delay ( MICRO(30000L) );

      //.cdl5
      for ( i = 0; i < 10; i++ ) {
         if ( (inb (base+7) & 0x88) != 0 ) {
            delay ( MICRO(10000L) );
         }
      }
   }
   //.cdl6
   outb ( 0x00, base+4 );
   outb ( 0x00, base+5 );
   outb ( 0xEC, base+7 );
   delay ( MICRO(10000L) );
   outb ( 0x00, base+1 );
   outb ( 0x00, base+2 );
   outb ( 0x00, base+3 );
   outb ( 0x00, base+4 );
   outb ( 0x80, base+5 );
   outb ( 0xA0, base+7 );
   //.cdl1
   for ( i = 0; i < 100; i++ ) {
      if ( ((inb (base+7) & 0x88) & 0x0F) != 0x8 ) {
         delay ( MICRO(2000L) );
      }
   }
   //.cdl2
}


int play_cd ( dword base, byte id, byte min_xx, word fr_sec ) {
   int i = 0;
   byte ret;
   word param;

   for ( i = 0; i < 1; i++ ) {
      atapi ( base, id );

      cli();

      outw ( 0x0047, base );

      param = ( min_xx << 8 ) + 0x01;
      outw ( param, base );

      outw ( fr_sec, base );

      outw ( 256 + 99, base );
      outw ( 0x0001, base );
      outw ( 0x0000, base );
      delay ( MICRO(200000L) );

      sti();
   
      ret = inb ( base+7 );
      if ( ret == 0xC0 || ret == 0x50 || ret == 0xD0 ) {
         #ifdef DEBUG
         imprimir ( "anduvo todo bien: 0x%xb!!\n", ret);
         #endif
         return 0;
      } else {
         #ifdef DEBUG
         imprimir ("Fallo: 0x%xb!!!, reintentando...\n", ret);
         #endif
      }
   }
   return -1;
}

int listar_cd ( dword base, byte id ) {
   int i, j, k;
   byte lectura;

   for ( i = 0; i < 1; i++ ) {
      atapi ( base, id );

      outw ( 0x43 + 0x2 * 0x100, base );
      outw ( 0x00, base );
      outw ( 0x00, base );
      outw ( 0x00, base );
      outw ( 0xC8, base );
      outw ( 0x00, base );
      inb ( base );

      for ( j = 0; j < 500; j++ ) {
         //.cdtrnwewait
         delay ( MICRO(10000L) );
         lectura = inb ( base + 7 );
         //imprimir ( "lectura: 0x%xb\n", lectura );

         /**
          * Se chequea que el bit 7 no haya venido en uno
          */
         if ( (lectura & 0x80) == 0x00 ) { 
            //.cdtrl1
            k = 0;
            //.cdtrread
            while ( k++ < 100) {
               lectura = inb ( base + 7 );
               //imprimir ( "lectura1: 0x%xb\n", lectura );
               /**
                * 0x50 = 0101 0000
                * 0x08 = 0000 1000
                */
               if ( 1 || (lectura & 0x08) == 0x08 ) {
                  imprimir ( "lei: 0x%xw ", inw ( base ) );
               } else {
                  imprimir ( "inb\n" );
                  break;
                  //return 0;
               }
            }
            break;
            //return 0;
         }
      }
   }
   imprimir ("saliendo...\n");
   return -1;
}

/**
 * 0x6200 = 0110 0010 0000 0000
 * 0x10B = 0001 0000 1011
 * 0000 0000 0000 0000 0010 0000 0000 0010 0001 0000 0000 0000
 * 0xC6 = 1100 0110
 */
