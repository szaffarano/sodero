#include <sodero/gdt.h>
#include <video.h>

/**
 * Indice de ubicacion de tareas dentro de la GDT
 */
static word siguiente_indice;

/**
 * Puntero a la GDT
 */
extern gdt *_gdt;

/**
 * Setea la entrada  en la GDT como un descriptor de una tss, pasando
 * como parametro como un descriptor de TSS, obteniendo los datos de la TSS,
 * tambien recibida como parametro.
 *
 * @param tss_tarea Segmento de la tarea en base al cual se seteara
 *                   el descriptor
 * @param desc Descriptor dentro de la GDT en donde se setearan
 *              los valores.
 */
void setear_desc_tss (const tss* tss_tarea, descriptor* desc) {
   static dword limite;
 
   limite = sizeof (tss) - 1;
 
   desc->desc_tss.limite_0_15 = limite & 0x0000FFFF;

   desc->desc_tss.base_0_15 = ((dword)tss_tarea) & 0xffff;
   desc->desc_tss.base_16_23 = (((dword)tss_tarea) >> 16) & 0xff;

   desc->desc_tss.limite_16_19 = (limite &0x000F0000) >> 16;

   desc->desc_tss.tipo_1 = 1; //         bit 8 (Tipo)
   desc->desc_tss.flag_busy = 0; //      bit 9 (Tipo-B)
   desc->desc_tss.tipo_2 = 2; //         bits 10 y 11 (Tipo) 
   desc->desc_tss.cero = 0; //           bit 12
   desc->desc_tss.dpl = 0; //            bits 13 y 14 (DPL)
   desc->desc_tss.presente = 1; //       bit 15
   desc->desc_tss.disponible = 0; //     bit 20 (AVL)
   desc->desc_tss.ceros = 0; //            bits 21 y 22 
   desc->desc_tss.granularidad = 0; //     bit 23 (GRANULARITY) 

   desc->desc_tss.base_24_31 = ((((dword)tss_tarea) >> 16) >> 8 ) & 0xff;
}

/**
 * Funcion que muestra el contenido de un descriptor de una TSS
 *
 * @param desc descriptor a mostrar por la Funcion que muestra el contenido 
 *             de un descriptor de un TSS.
 */
void ver_descriptor (const descriptor* desc) {
   imprimir ( "base tss 1: 0x%xw\n", desc->desc_tss.base_0_15);
   imprimir ( "base tss 2: 0x%xb\n", desc->desc_tss.base_16_23);
   imprimir ( "base tss 3: 0x%xb\n", desc->desc_tss.base_24_31);
   imprimir ( "limite 0_15: 0x%xwb\n", desc->desc_tss.limite_0_15);
}

/**
 * Funcion que obtiene el siguiente indice de entrada en la GDT
 *
 * @return siguiente_indice Siguiente indice que corresponde a la entrada en
 *                          la GDT. 
 */
word siguiente_indice_gdt () {
   return ++siguiente_indice;
}

/**
 * Funcion que setea el siguiente indice de entrada en la GDT
 *
 * @param sig Siguiente indice a setear como siguiente entrada en la GDT.
 */
void setear_siguiente_indice (const word sig) {
   siguiente_indice = sig;
}

/**
 * Setea la entrada en la GDT como un descriptor de segmento, pasado como 
 * parametro como el tipo de segmento a setear, la base del segmento, el 
 * descriptor donde setear los valores y tamanio o limite del segmento. 
 *
 * @param tipo_segmento tipo de segmento a setear (por ej. De Codigo, de 
 *                      de datos, de sistema)
 * @param base Direccion donde comienza el segmento a setear.
 * 
 * @param desc Descriptor dentro de la GDT en donde se setearan
 *              los valores.
 * @param tam Tamanio del segmento a setear, limite del mismo.
 */
void setear_desc_seg (const tipo_segmento tipo, 
                      const dword base, 
                      descriptor* desc, 
                      const dword tam) {
   word limite_0_15;
   unsigned limite_16_19;
   word base_0_15;
   byte base_16_23;
   byte base_24_31;
   unsigned tipo_descriptor;
   unsigned tipo_esc;

   limite_0_15 = tam & 0xFFFF; 
   limite_16_19 = (tam >> 16) & 0xF;
   base_0_15 = base & 0xFFFF;
   base_16_23 = (base >> 16) & 0xFF;
   base_24_31 = (base >> 24) & 0xFF;
   tipo_descriptor = (tipo == SEG_SISTEMA) ? 0 : 1;
   tipo_esc = (tipo == SEG_CODIGO) ? 0xA : 0x2;
   
   desc->desc_seg.limite_0_15 = limite_0_15;
   desc->desc_seg.limite_16_19 = limite_16_19;
   desc->desc_seg.base_0_15 = base_0_15;
   desc->desc_seg.base_16_23 = base_16_23;
   desc->desc_seg.tipo_descriptor = tipo_descriptor;
   desc->desc_seg.base_24_31 = base_24_31;
   desc->desc_seg.presente = 0x1;
   desc->desc_seg.dpl = 0x0;
   desc->desc_seg.disponible = 0x0;   //se setea en 0!
   desc->desc_seg.tipo = tipo_esc;     //escritura-lectura, demas!!!
   desc->desc_seg.cero = 0x0;
   desc->desc_seg.tamanio_operacion = 0x1; //1 si esta en modo_protegido
                                           //32 bits 
   desc->desc_seg.granularidad = 0x1;   
   
}

/**
 * Funcion que muestra el contenido de un descriptor de segmento 
 *
 * @param indice_gdt indice dentro de la gdt donde se encuentra el descriptor
 *                   de segmento a mostrar.
 */
void ver_desc_seg ( const int indice_gdt ) {
   
   imprimir ("Limite_0_15: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.limite_0_15 );
   imprimir ("Limite_16_19: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.limite_16_19 );
   imprimir ("Base_0_15: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.base_0_15 );
   imprimir ("Base_16_23: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.base_16_23 );
   imprimir ("Base_24_31: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.base_24_31 );
   imprimir ("Tipo de descriptor: 0x%x\n", 
            _gdt->descriptores [indice_gdt].desc_seg.tipo_descriptor );
   imprimir ("DPL: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.dpl );
   imprimir ("TIPO: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.tipo );     //ni idea
   imprimir ("PRESENTE: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.presente );
   imprimir ("DISPONIBLE: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.disponible ); //la tss 
                                                                //lo pone en 0!
   imprimir ("CERO: 0x%x\n", _gdt->descriptores [indice_gdt].desc_seg.cero );
   imprimir ("TAMANIO OPERACION: 0x%x\n", 
             _gdt->descriptores [indice_gdt].desc_seg.tamanio_operacion );
   imprimir ("GRANULARIDAD: 0x%x\n", 
              _gdt->descriptores [indice_gdt].desc_seg.granularidad );
}
