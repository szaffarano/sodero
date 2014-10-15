#include <sodero/sys_ejecutar.h>
#include <sodero/syscalls.h>

/**
 * Tarea que esta ejecutando
 **/
extern pcb* tarea_actual;

extern int recuperar_base ();

int sys_ejecutar ( char* arch, char* args[] ) {
   int i, fd, idx_codigo, idx_dato;
   pcb* nueva_tarea;
   dword funcion;
   char* dato = (char*)alocar (11);
   char* archivo = arch + recuperar_base ();
   char** args1 = (char**) (((int)args) + recuperar_base() );

   char* argu;

   if ( args1 != NULL ) {
      i = 0;
      imprimir ( "Mostrando argumentos:\n" );
      while ( args1[i] != NULL ) {
         argu = (char*) ( ((int)args1[i]) + recuperar_base () );
         imprimir ( "args[%d] = <%s>\n", i, argu );
         i++;
      }

      for (i = 0; i < 12 && archivo [i] != '\0'; i++) {
         dato [i] = archivo [i];
      }
   } else {
      imprimir ( "No se recibieron argumentos en ejecutar\n" );
   }

   imprimir_cadena ( "Abriendo: <" );
   imprimir_cadena ( archivo );
   imprimir_cadena ( " - " );
   imprimir_cadena ( dato );
   imprimir_cadena ( ">\n" );

   fd = abrir ( archivo );
   
   funcion = (dword) sys_alocar ( tarea_actual->archivos [fd].ed.tamanio +
                                  TAMANIO_PILA_USUARIO );


   if ( funcion == NULL ) {
     return -1;
   }
   
   if ( fd == ERROR ) {
      return -1;
   } else {
   
      idx_codigo = siguiente_indice_gdt ();
      
      setear_desc_seg ( SEG_CODIGO, funcion, &_gdt->descriptores[idx_codigo],
                        0xFFFFF );
      //                        tarea_actual->archivos [fd].ed.tamanio );
      
      idx_dato = siguiente_indice_gdt ();
      
      setear_desc_seg ( SEG_DATOS, funcion, &_gdt->descriptores[idx_dato],
                        0xFFFFF );
      //                  tarea_actual->archivos [fd].ed.tamanio +
      //                  TAMANIO_PILA_USUARIO );
      
      leer_archivo (fd, (void*)funcion, tarea_actual->archivos [fd].ed.tamanio);
      
      nueva_tarea = crear_tarea ((handler_tarea)funcion);
       
      //nueva_tarea->dato = archivo;
      //nueva_tarea->dato = arch;
      nueva_tarea->dato = dato;

      nueva_tarea->tarea->eip = 0x00;
      
      nueva_tarea->tarea->cs = 8 * idx_codigo;
      
      nueva_tarea->tarea->ds = 8 * idx_dato;
      nueva_tarea->tarea->es = nueva_tarea->tarea->ds;
      nueva_tarea->tarea->fs = nueva_tarea->tarea->ds;
      nueva_tarea->tarea->gs = nueva_tarea->tarea->ds;
      nueva_tarea->selector_ds = nueva_tarea->tarea->ds >> 3;
      
      /*
      nueva_tarea->tarea->esp = funcion + TAMANIO_PILA_USUARIO + 
                                tarea_actual->archivos [fd].ed.tamanio;
      
      nueva_tarea->tarea->ss = nueva_tarea->tarea->ds;
      nueva_tarea->tarea->ss0 = nueva_tarea->tarea->ds;
      nueva_tarea->tarea->ss1 = nueva_tarea->tarea->ds;
      nueva_tarea->tarea->ss2 = nueva_tarea->tarea->ds;
      */

      /*
      imprimir_cadena ("poniendo en ejecucion a ");
      imprimir_cadena (dato);
      imprimir_cadena ("\n");
      */
      agregar (nueva_tarea);

      //cerrar ( fd );
   }
   return nueva_tarea->pid;
}
