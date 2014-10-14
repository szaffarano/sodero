TOOLS := ../tools

SYSCALLS_INCLUDE := -I../../include

CCOPTS := -Wall -nostdinc $(SYSCALLS_INCLUDE)

GCC := gcc
#DEBUG := -DDEBUG

PATH_LIB := ../lib
PATH_SYSCALLS := ./syscalls

OBJETOS_SYS := syscalls.o sys_video.o sys_ejecutar.o sys_esperar_pid.o  \
               sys_salir.o sys_pausa.o sys_alocar.o sys_dir.o sys_matar.o \
               sys_ps.o sys_reboot.o sys_xchg_aprop.o sys_listar_directorio.o \
               sys_es.o

all: $(OBJETOS_SYS)

sys_ejecutar.o: sys_ejecutar.c $(HEADERS)
	$(GCC) -c sys_ejecutar.c $(CCOPTS)

sys_pausa.o: sys_pausa.c $(HEADERS)
	$(GCC) -c sys_pausa.c $(CCOPTS)

sys_salir.o: sys_salir.c $(HEADERS)
	$(GCC) -c sys_salir.c $(CCOPTS)

sys_esperar_pid.o: sys_esperar_pid.c $(HEADERS)
	$(GCC) -c sys_esperar_pid.c $(CCOPTS)

syscalls.o: syscalls.c $(HEADERS)
	$(GCC) -c syscalls.c $(CCOPTS)

sys_video.o: sys_video.c $(HEADERS)
	$(GCC) -c sys_video.c $(CCOPTS)

sys_alocar.o: sys_alocar.c $(HEADERS)
	$(GCC) -c sys_alocar.c $(CCOPTS)

sys_dir.o: sys_dir.c $(HEADERS)
	$(GCC) -c sys_dir.c $(CCOPTS)

sys_ps.o: sys_ps.c $(HEADERS)
	$(GCC) -c sys_ps.c $(CCOPTS)

sys_matar.o: sys_matar.c $(HEADERS)
	$(GCC) -c sys_matar.c $(CCOPTS)

sys_reboot.o: sys_reboot.c $(HEADERS)
	$(GCC) -c sys_reboot.c $(CCOPTS)

sys_xchg_aprop.o: sys_xchg_aprop.c $(HEADERS)
	$(GCC) -c sys_xchg_aprop.c $(CCOPTS)

sys_listar_directorio.o: sys_listar_directorio.c $(HEADERS)
	$(GCC) -c sys_listar_directorio.c $(CCOPTS)

sys_es.o: sys_es.c $(HEADERS)
	$(GCC) -c sys_es.c $(CCOPTS)

clean:
	$(RM) *.bin *.com mapa_memoria.txt *.o *.ld *.bkp *.dst *.sys .*.swp
