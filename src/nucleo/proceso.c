#include <sodero/proceso.h>
#include <sodero/admin_procesos.h>
#include <timer.h>
#include <sodero/definiciones.h>
#include <sodero/syscalls.h>

extern gdt* _gdt;
extern pcb* tarea_actual;

/**
 * Algoritmo de administracion de procesos
 */
algoritmo_proceso_id algoritmo_proceso = FIFO; 

/**
 * Puntero al inicio de la lista de procesos
 */
nodo* lista_ready_inicio;

/**
 * Puntero al final de la lista de procesos
 */
nodo* lista_ready_fin;

/**
 * Puntero a la tarea que esta ejecutando actualmente.
 */
extern pcb* tarea_actual;

/**
 * Funcion que agrega proceso en la cola de listos.
 *
 * @param pcb_tarea Recibe el proceso a agregar en la cola e listos para su
 *                 posterior ejecucion.
 */
void agregar (const pcb* pcb_tarea) {
   nodo* aux;
   
   if ((aux = nuevonodo ()) != NULL) {
      aux->tarea = (pcb*)pcb_tarea;
      //aux->tarea->estado = READY;
      aux->siguiente = NULL;
      aux->anterior = NULL;
      if (lista_ready_inicio == NULL) {
         lista_ready_fin = aux;
         lista_ready_inicio = aux;
      } else {
         switch (algoritmo_proceso) {
           case FIFO:
                aux->siguiente = (struct nodo*)lista_ready_fin;
                lista_ready_fin->anterior = (struct nodo*)aux;
                lista_ready_fin = aux; 
              break;
           case LIFO:
              break;
           case PRIO:
              break;
           case STF:
              break;
           case SODERO:
              break;
           case RND:
              break;
           case GPF:
              break;
           case LPF:
              break;
           default:
              break; 
         }
      } 
   }
}

/**
 * Funcion que saca un proceso de la cola de listos.
 *
 * @return pcb Proceso eliminado de la cola, por ejemplo para su ejecucion.
 */
pcb* sacar () {
   pcb* aux = NULL;

   if ( lista_ready_inicio != NULL ) {
      aux = lista_ready_inicio->tarea;
      //aux->estado = EXEC;
      //deberia liberar la memoria del nodo actual
      if (lista_ready_inicio == lista_ready_fin) {
         lista_ready_fin = NULL;
      }
      lista_ready_inicio = (nodo*)lista_ready_inicio->anterior;
      lista_ready_inicio->siguiente = NULL;
   } else {
      imprimir ("\nlista vacia\n");
      bucle_infinito ();
   }
   return (aux);
}

/**
 * Funcion que crea una nueva tarea.
 *
 * @param tarea direccion de la funcion principal de la tarea a crear
 *              para setear el EIP de la TSS en ese lugar, a priori.
 * 
 * @return pcb Proceso de la nueva tarea creada con su pcb correspondiente.
 */
pcb* crear_tarea (const handler_tarea tarea) {
   pcb* aux;

   //Se aloca la pcb del nuevo proceso.
   if ((aux = (pcb*)sys_alocar (sizeof(pcb))) != NULL) {
      aux->selector = siguiente_indice_gdt ();
      aux->pid = aux->selector;
      aux->tarea = (tss*)sys_alocar (sizeof(tss));
      aux->ultima_tarea = NULL;
      aux->tipo = PROCESO_USUARIO;
      setear_desc_tss (aux->tarea, 
                       &_gdt->descriptores[aux->selector]);
      setear_tarea (aux->tarea, tarea);
      aux->selector_ds = aux->tarea->ds >> 3;
      aux->ts_ejec = 0; //tiempo de ejecucion se inicializa en cero.
   } else {
      imprimir ("No hay memoria...\n");
      bucle_infinito ();
   } 
   #ifdef DEBUG
   imprimir ("agregando: %x - %xw\n", aux, aux->selector);
   #endif
   return (aux);
}

/**
 * Funcion que crea un nuevo nodo en el uso de colas.
 *
 * @return nodo nuevo nodo con su espacio de memoria reservado.
 */
nodo* nuevonodo () {
    return ((nodo*)sys_alocar (sizeof(nodo)));
}

/*
 * Funcion dump_cola_ready
 * Esta funcion muestra el contenido de la Cola de Ready.
 * Muestra del primero que sale al ultimo que ingreso en la Cola.
 */
void dump_cola_ready () {
    nodo* nodo_aux;
    int flag_cola;

    flag_cola = 0;

    //setear_coordenadas(0,16);
    nodo_aux = lista_ready_inicio;

    imprimir("Dump de la Cola Ready\n");
    imprimir("~~~~ ~~ ~~ ~~~~ ~~~~~\n");
    while(nodo_aux != NULL) {
       imprimir("%s  ", nodo_aux->tarea->dato);
       imprimir("%d\n", nodo_aux->tarea->pid);
       flag_cola = 1;
       nodo_aux = (nodo*)(nodo_aux->anterior);
    }
    if (flag_cola == 0) {
       imprimir("COLA VACIA\n");
    }
    delay ( MICRO(2000000L) );
}

/*
 * Funcion que recupera la base de un segmento apuntado por un descriptor de
 * segmento de la GDT.
 *
 * @return base base del segmento en tamanio de 32bits.  
 */
int recuperar_base () {
   //int indice = (tarea_actual != NULL) ? (tarea_actual->tarea->gs >> 3)
   //             : 0x10 >> 3;

   int indice = (tarea_actual != NULL) ? (tarea_actual->selector_ds)
                : 0x10 >> 3;

   return ( (dword) (_gdt->descriptores [indice].desc_seg.base_0_15) + \
           (dword)(_gdt->descriptores [indice].desc_seg.base_16_23 << 16) + \
           (dword)(_gdt->descriptores [indice].desc_seg.base_24_31 << 24) );
}

int recuperar_base_segmento ( int indice ) {
   return ( (dword) (_gdt->descriptores [indice].desc_seg.base_0_15) + \
           (dword)(_gdt->descriptores [indice].desc_seg.base_16_23 << 16) + \
           (dword)(_gdt->descriptores [indice].desc_seg.base_24_31 << 24) );
}
