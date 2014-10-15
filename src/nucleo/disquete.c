/*
 * Modulo que tiene las funciones de bajo nivel para el acceso al FDC
 * Representa el driver de la disquetera
 */
#include <sodero/disquete.h>

// funciones privadas al modulo
static void estado_interrupcion (byte*, byte*);
static void configurar_fdc ();
static void especificar_fdc ();
static void detectar_disquetera ();
static void esperar_interrupcion ();
static void recalibrar ();

/**
 * Direccion del buffer temporal donde se almacenan los datos leidos
 */
static char* direccion_fd_buffer;

/**
 * El buffer donde se almacentan los datos leidos
 */
static char fd_buffer[1024];

/**
 * Flag que indica si se disparo o no la IRQ6
 */
static int irq6;

/**
 * Flag que indica si el motor esta o no encendido
 */
static byte motor_encendido;

/**
 * Flag que indica si hay un proceso leyendo el FDC ( se usa para
 * apagar el motor del FDC unicamente cuando nadie lo usa )
 */
static int semaforo_fdc;

/**
 * disquetera_handler: Handler de la IRQ6.  Habilita el flag de dicha IRQ
 *
 * @param numero el numero de la IRQ que se disparo
 */
void disquetera_handler (dword numero) {
   #ifdef DEBUG
   imprimir ("se disparo la IRQ6(diskettera)\n");
   #endif
   irq6 = 1;

   outb (0x20, 0x20);
   //outb (0x66, 0x20);
}

/**
 * esperar_interrupcion: espera que se dispare la IRQ
 */
static void esperar_interrupcion () {
   while ( irq6 != 1 );
   #ifdef DEBUG
   imprimir ( "FDC esta listo\n" );
   #endif
}

/**
 * inicializar_fdc: realiza el reset y posterior inicializacion del FDC
 *                  echa en base al pseudocodigo del manual de Intel
 *
 * @return el estado de la inicializacion
 * <pre>
 *    Comentarios sobre los comandos:
 *       Posibles valores del Data Rate: ( se setean unicamente los dos
 *                                         bits menos significativos, el 
 *                                         resto va en cero ).
 *       11: 1 Mbps
 *       00: 500 Kbps
 *       01: 300 Kbps
 *       10: 250 Kbps
 * </pre>
 */
int inicializar_fdc () {
   pcb* aux = NULL;
   int i;
   byte st0, pcn;
   irq6 = 0;
   semaforo_fdc = 1;

   #ifdef DEBUG
   imprimir ( "Se esta inicializando el FDC...\n" );
   #endif

   direccion_fd_buffer = fd_buffer;

   // primer paso, RESET
   outb ( (CMD_APAGAR_MOTOR | CMD_ACTIVA_RESET | DMA), FDC_DOR );
   outb ( (CMD_APAGAR_MOTOR | CMD_NO_RESET | DMA), FDC_DOR );

   // segundo paso, setear el Data Rate en el CCR
   outb ( DATA_RATE_500K, FDC_CCR );

   // tercer paso, esperar interrupcion!
   esperar_interrupcion ();

   // cuarto paso, consultar el estado de la IRQ6 4 veces
   for ( i = 0; i < 4; i++ ) {
      estado_interrupcion ( &st0, &pcn );
   }

   // quinto paso, ejecutar el comando configurar del FDC
   // (solamente si los parametros son diferentes que los de default!)
   configurar_fdc();

   // ultimo paso!, ejecutar el comando especify
   especificar_fdc ();

   detectar_disquetera ();

   // comprobar que el buffer este alineado a 512 bytes
   if ( ((dword)direccion_fd_buffer >> 16) != 
        (((dword)direccion_fd_buffer + 512 - 1) >> 16) ) {
      imprimir ("cambiando la direccion de fd_buffer...\n");
      direccion_fd_buffer += 512;
   }

   /*
    * Crear la tarea que se encarga de apagar el motor periodicamente
    */
   aux = crear_tarea (tarea_apagar_motor);
   aux->dato = (char*) alocar (10);
   aux->dato = "FDC_MOTOR\0";
   aux->tipo = PROCESO_SISTEMA;
   agregar (aux);

   // aun nadie esta utilizando el FDC
   semaforo_fdc = 0;

   // anduvo todo bien! :-)
   return 1;
}

/**
 * estado_interrupcion: implementa el comando del FDC que consulta el estado
 *                      (condiciones de fin, etc) de la ultima interrupcion
 *                       que se disparo.
 * @param st0 Primer Registro de Estado (Status Register 0).  Se setea a 
 *            la vuelta
 * @param pcn Indica la pista en la cual esta posicionada la cabeza lectora
 *
 * <pre>
 *    Estructura del primer registro de estado (ST0):
 *      bits7-6: Interrupt Code, si = 00 terminacion normal, comando finalizado
 *               exitosamente. Si 01: terminacion anormal.
 *
 *      bit5: (SE): Seek End.  Se pone a uno cuando acaba una operacion de Seek.
 *      bit4: (EC) Equipment Check (comprobacion de equipo), se pone a uno si la
 *            unidad informa de un error o si tras un recalibrado no aparece aun
 *            la senial que indica que se alcanzo el cilindro cero.
 *      bit3: Not Ready, se pone a uno cuando la FDC informa sobre esta 
 *            condicion
 *      bit2: Head Address: indica el cabezal activo en el momento de la 
 *            interrupcion
 *      bits1-0: (US) Unt Select(Unidad Activa): indica la unidad activa 
 *               durante la  interrupcion: 0 -> A, 1 -> B, etc.
 *
 *    Obtengo los primeros dos bits de la palabra de estado, que me va
 *    a dar la unidad sobre la cual se efectuo el posicionamiento
 *    de cabeza.  Si es distinto de cero significa que la unidad no
 *    es A.
 * </pre>
 */
static void estado_interrupcion (byte* st0, byte* pcn) {
   if ( escribir_byte (CMD_ESTADO_IRQ) != ESCRITURA_OK ) {
      imprimir ( "no se pudo obtener el estado de la IRQ\n" );
   } else {
      *st0 = leer_byte (); 
      *pcn = leer_byte (); 
      #ifdef DEBUG
      imprimir ( "<estado_irq>ST0: 0x%x\n", *st0 );
      imprimir ( "<estado_irq>PCN: 0x%x\n", *pcn );
      #endif
   }
}

/**
 * recalibrar: ejecuta el comando recalibrar del FDC, el cual posiciona la
 *             cabeza lectora en la pista cero.
 */
static void recalibrar () {
   byte st0, pcn;
   #ifdef DEBUG
   imprimir ( "recalibrando...\n" );
   #endif
   if ( escribir_byte (CMD_RECALIBRAR) != ESCRITURA_OK ) {
      imprimir ( "no se pudo enviar el byte0 del comando recalibrar\n" );
   } else if ( escribir_byte (DISPOSITIVO_0) != ESCRITURA_OK ) {
      imprimir ( "no se pudo enviar el byte1 del comando recalibrar\n" );
   }
   irq6 = 0;
   esperar_interrupcion ();
   estado_interrupcion ( &st0, &pcn );
}

/**
 * configurar_fdc: comando configure del FDC, debido a que los parametros
 *                 por default estan bien, no hace falta ejecutarlo.
 */
static void configurar_fdc () {
  // por el momento no hacemos nada
}

/**
 *
 * especificar_fdc: Implementacion del comando specify del FDC
 * 
 * <pre>
 * Valores de los argumentos:
 * byte0: 0x03 (duro)
 * byte1: 
 * 	+-----+-----+-----+-----+-----+-----+-----+-----+
 * 	| SR3 | SR2 | SR1 | SR0 | HU3 | HU2 | HU1 | HU0 |
 * 	+-----+-----+-----+-----+-----+-----+-----+-----+
 * byte 2:
 * 	+-----+-----+-----+-----+-----+-----+-----+-----+
 * 	| HL6 | HL5 | HL4 | HL3 | HL2 | HL1 | HL0 | DMA |
 * 	+-----+-----+-----+-----+-----+-----+-----+-----+
 * SR: (Step Rate Time) tiempo comprendido entre dos impulsos consecutivos 
 *     de la senial que mueve el motor paso a paso.  Para unidades de alta 
 *     densidad se calcula como (16-SR), en milisegundos.
 * HL: (Head Load Time) Se mide en milisegundos, para unidades de alta densidad
 *     se calcula como (HL+1)*2
 * HU: (Head Unload Time) Se mide en milisegundos, en unidades de alta densidad
 *     se calcula como (HU*16)
 * DMA: si 0 modo DMA, si 1 modo NO DMA
 * <pre>
 */
static void especificar_fdc () {
   if ( escribir_byte (CMD_ESPECIFY) != ESCRITURA_OK ) {
      imprimir ( "Error en el comando ESPECIFY\n" );
   // 1101 1111
   } else if ( escribir_byte (0xDF) != ESCRITURA_OK ) {
      imprimir ( "Error escribiendo el byte1 de ESPECIFY\n" );
   // 0000 0011
   } else if ( escribir_byte (0x02) != ESCRITURA_OK ) {
      imprimir ( "Error escribiendo el byte2 de ESPECIFY\n" );
   }
}

/**
 * escribir_byte: le escribe una palabra al FDC unicamente cuando
 *                el bit RQM del MSR esta en uno y el bit DIO de
 *                dicho registro esta en cero.  Echa en base al
 *                pseudocodigo del manual del 82077AA.
 * @param dato palabra a escribir
 *
 * @return estado de la escritura
 */
int escribir_byte (byte dato) {
   int reintentos = REINTENTOS;
   byte estado;

   while ( reintentos-- ) {
      estado = inb (FDC_MSR);
      if ( (estado & 0xC0) == 0x80 ) {
         outb ( dato, FDC_FIFO );
	 return ESCRITURA_OK;
      } else {
      #ifdef DEBUG
         imprimir ( "estado: 0x%xb ", estado );
      #endif
      }
      delay ( MICRO(DELAY_FDC) );
   }
   #ifdef DEBUG
   #endif
   imprimir ( "Error al escribir palabra!\n" );

   return ESCRITURA_ERRONEA;
}

/**
 * leer_byte: leee una palabra desde el registro FIFO del FDC unicamente 
 *            cuando los bits RQM y DIO del MSR estan en uno.  Echa en 
 *            base al pseudocodigo del manual del 82077AA.
 * @return palabra leida o 0xFF ( -1 ) en caso de error
 */
byte leer_byte () {
   int reintentos = REINTENTOS;
   byte estado;

   while ( reintentos-- ) {
      estado = inb (FDC_MSR);
      if ( (estado & 0xD0) == 0xD0 ) {
         return inb ( FDC_FIFO );
      } else {
	  #ifdef DEBUG
         imprimir ( "estado: 0x%xb ", estado );
	  #endif
      }
      delay ( MICRO(DELAY_FDC) );
   }
   #ifdef DEBUG
   #endif
   imprimir ( "Error al leer palabra!\n" );

   return LECTURA_ERRONEA;
}

/**
 * detectar_disquetera: detecta el tipo de disquetera que tiene el equipo
 *                      Simplemente lo informa por pantalla.
 */
static void detectar_disquetera() {
   byte lectura;

   #ifdef DEBUG
   imprimir ( "detectando disquetera...\n" );
   #endif
   if ( escribir_byte ( CMD_VERSION ) != ESCRITURA_OK ) {
      imprimir ( "error al escribir version\n" );
   } else {
      switch ( (lectura = leer_byte()) ) {
         case FDC_NEC:
            imprimir ( "Se detecto una disquetera NEC765!\n");
            break;
         case FDC_82077:
            imprimir ( "Se detecto una disquetera 82077AA!\n");
            break;
         default:
            imprimir ( "Disquetera desconocida(0x%xb)!\n", lectura );
      }
   }
}

/*
 * encender_motor: implementa el comando reset con el bit de encender motor
 *                 habilitado.
 * <pre>
 * Estructura del byte del argumento:
 *    bit 7: encender motor de la unidad 3
 *    bit 6: encender motor de la unidad 2
 *    bit 5: encender motor de la unidad 1
 *    bit 4: encender motor de la unidad 0
 *    bit 3: 1 si DMA activa
 *    bit 2: 0 si reiniciar FDC
 *    bits 0-1:
 *    	   00: seleccionar A
 *    	   01: seleccionar B
 *    	   10: seleccionar C
 * 	   11: seleccionar D
 * </pre>
 */
void encender_motor() {
   #ifdef DEBUG
   imprimir ("encendiendo motor...\n");
   #endif
   outb ( CMD_ENCENDER_MOTOR | CMD_NO_RESET | DMA, FDC_DOR );
   // segun el manual de intel, hace falta esperar 300 ms con el motor
   // encendido para poder comenzar a operar.
   delay ( MICRO(300000L) );
   recalibrar ();
   motor_encendido = 1;
}

/**
 * apagar_motor: implementa el comando reset con el bit de encender motor
 *                 deshabilitado.
 * <pre>
 * Estructura del byte del argumento:
 *    bit 7: encender motor de la unidad 3
 *    bit 6: encender motor de la unidad 2
 *    bit 5: encender motor de la unidad 1
 *    bit 4: encender motor de la unidad 0
 *    bit 3: 1 si DMA activa
 *    bit 2: 0 si reiniciar FDC
 *    bits 0-1:
 *    	   00: seleccionar A
 *    	   01: seleccionar B
 *    	   10: seleccionar C
 * 	   11: seleccionar D
 * </pre>
 */
void apagar_motor() {
   #ifdef DEBUG
   imprimir ("apagando motor...\n");
   #endif
   outb ( CMD_APAGAR_MOTOR | CMD_NO_RESET | DMA, FDC_DOR );
   motor_encendido = 0;
}


/**
 * leer_sector: Lee un sector de disco, recibiendo como argumentos cabeza, 
 *              pista y sector. Analogo a la int 13 del BIOS
 *              Implementada en base al pseudo codigo del manual de intel
 * @param cabeza la cabeza a leer
 * @param pista la pista a leer
 * @param sector el sector a leer
 * @param buff espacio en memoria donde se almacena el sector leido
 *
 * @return el estado de la lectura
 *
 * <pre>
 *    Comando leer datos
 *    +---+---+---+---+---+---+---+---+
 *    | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *    +---+---+---+---+---+---+---+---+
 *      |   |   |           |   |
 *      |   |   |           +---+--> si 11 entonces voy a leer
 *      |   |   |
 *      |   |   +--> si = 1 -> Skip Control
 *      |   +--> si = 1 -> doble densidad
 *      +--> si = 1 -> multi track 
 *
 *    palabra de control = 11100110 = 0xE6 (con multitrack)
 *    palabra de control = 01100110 = 0x66 (sin multitrack)
 * </pre>
 */
int leer_sector (dword cabeza, dword pista, dword sector, void* buff) {
   // palabras de estado a leer luego de ejecutar el comando
   byte estado0;
   byte estado1;
   byte estado2;
   dword pista_leida;
   dword cabeza_leida;
   dword sector_leido;
   dword bps;

   int i; // contador de reintentos

   semaforo_fdc = 1;

   #ifdef DEBUG
   imprimir ("\nleyendo (cabeza: %d, pista: %d, sector: %d)...\n", cabeza,
                                                                   pista,
                                                                   sector);
   #endif

   // primer paso: encender el motor
   while ( motor_encendido != 1 ) {
      encender_motor ();
      delay ( MICRO(DELAY_FDC) );
   }

   // segundo paso: setear el Data Rate (ver inicializar_fdc)
   outb ( DATA_RATE_500K, FDC_CCR );

   // tercer paso: recalibrar 
   //recalibrar();

   for ( i = 0; i < 3; i++ ) {
      if ( seek (pista, cabeza) == ERROR ) {
         imprimir ("fallo al posicionar pista, reintentando...\n");
         continue;
      }
      setup_dma(CHANNEL2, (dword)direccion_fd_buffer, 512, WRITE, SINGLE, 0, 0);

     
      escribir_byte (FLAG_MT | FLAG_MFM | FLAG_SC | CMD_LECTURA); // byte0

      /*
       * Cero porque es drive A, 1 porque es drive B, etc
       */
      escribir_byte ( ((cabeza << 2) | FDC_UNIDAD) ); // byte1
      escribir_byte (pista);   //cilindro
      escribir_byte (cabeza);  //cabeza
      escribir_byte (sector);  //sector inicial
      escribir_byte (FLAG_BPS_512); //tsector (longitud)
      escribir_byte (0x12);  //sector final (SECTORES POR PISTA!)
      //escribir_byte (sector);  //sector final (SECTORES POR PISTA!)
      escribir_byte (FDC_GAP);     // 0x1B porque el disco es de HD  (gap)
      irq6 = 0;
      escribir_byte (0xFF); // longitud de datos (128)

      esperar_interrupcion ();

      estado0 = leer_byte ();
      estado1 = leer_byte ();
      estado2 = leer_byte ();
      pista_leida = leer_byte ();
      cabeza_leida = leer_byte ();
      sector_leido = leer_byte ();
      bps = leer_byte ();

      #ifdef DEBUG
      imprimir ("<estado0=0x%xb>", estado0);
      imprimir ("<estado1=0x%xb>", estado1);
      imprimir ("<estado2=0x%xb>", estado2);
      imprimir ("<leyendo bps=0x%xb>\n", bps);
      imprimir ("<pista=0x%xb>", pista_leida);
      imprimir ("<cabeza_leida=0x%xb>", cabeza_leida);
      imprimir ("<sector=0x%xb>\n", sector_leido);
      #endif

      // vemos si los bits 6-7 del ST0 estan en 01 -> error o 00 -> todo OK
      if ( (estado0 >> 6) == 0x01 ) { // deberia ser distinto de cero
         imprimir ( "Error en la lectura de un sector, reintentando...\n" );
         continue;
      }

      for ( i = 0; i < 512; i++ ) {
         //buff[i] = *(direccion_fd_buffer+i);
         *((char*)buff+i) = *(direccion_fd_buffer+i);
      }
      semaforo_fdc = 0;
      return  LECTURA_OK;
   }
   semaforo_fdc = 0;
   return LECTURA_ERRONEA;
}

/**
 * tarea_apagar_motor: tarea encargada de apagar el motor periodicamente 
 *                     (siempre y cuando este encendido y nadie lo este
 *                     utilizando)
 */
void tarea_apagar_motor () {
   while ( 1 ) {
      delay ( MICRO (DELAY_APAGA_MOTOR) );
      if ( semaforo_fdc == 0 && motor_encendido == 1 ) {
         apagar_motor ();
      }
   }
}

/**
 * seek: posiciona la cabeza lectora en una pista dada.
 *
 * @param pista pista a posicionar
 * @param lado lado a posicionar
 *
 * @return estado de retorno del comando
 *
 * <pre>
 *    Comando seek:
 * 
 *    byte0: 0x0F (duro)
 *    byte1: 
 *    	   bits7-3: x
 * 	   bit 2: cabezal
 * 	   bits1-0: unidad
 *    byte2: nro de cilindro
 * </pre>
 */
int seek (byte pista, byte lado) {
   byte st0, pcn;

   #ifdef DEBUG
   imprimir ("posicionando cabeza en pista %d...\n", pista);
   #endif

   // escribo comando (byte0)
   if ( escribir_byte ( CMD_SEEK ) == ESCRITURA_ERRONEA ) {
      #ifdef DEBUG
      imprimir ("0x0F ERROR\n");
      #endif
      return ERROR;
   }

   // escribo byte1 (lado y dispositivo, que por ahora va duro cero!)
   if ( escribir_byte ( (lado << 0x2) | 0 ) == ESCRITURA_ERRONEA ) {
      #ifdef DEBUG
      imprimir ("(0x0 << 0x2) | 0 ERROR\n");
      #endif
      return ERROR;
   }

   // byte2: pista
   if ( escribir_byte ( pista ) == ESCRITURA_ERRONEA ) {
      #ifdef DEBUG
      imprimir ("pista ERROR");
      #endif
      return ERROR;
   }

   irq6 = 0;
   esperar_interrupcion ();
   estado_interrupcion ( &st0, &pcn );

   #ifdef DEBUG
   imprimir ( "estado0: %xb\n", estado0);
   imprimir ( "pista_resultado: %xb\n", pista_resultado);
   #endif

   // los dos ultimos bits me dan la unidad
   if ( (st0 & 0x03) != 0 ) {
      imprimir ("Error unidad A\n");
      return ERROR;
   } 

   // obtengo el bit 5 para saber si finalizo el Seek
   if ( ((st0 >> 5) & 1) != 1 ) {
      imprimir ("Seek no termino!!!\n");
      return ERROR;
   } 

   // obtengo el codigo de interrupcion, a ver si finalizo OK
   if ( (st0 >> 6) != 0 ) {
      imprimir ("seek erroneo!\n");
      return ERROR;
   }

   if ( pista != pcn ) {
      imprimir ("se posiciono en cualquier lado\n");
      return ERROR;
   }
   return OK;
}

/** 
 * Copia una posicion de memoria en otra
 * @param origen desde donde se copia
 * @param destino hacia donde se copia
 * @param tamanio la cantidad a copiar
 * 
 * @return la direccion de memoria donde se copio
 */
void* copiar_memoria (void* origen, void* destino, unsigned int tamanio) {
   int i = 0;

   for ( i = 0; i < tamanio; i++ ) {
      *((char *)destino + i) = *((char *)origen + i);
   }

   return destino;
}
