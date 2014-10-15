TOOLS := ../tools

KERNEL_INCLUDE := -I../include

CCOPTS := -fno-stack-protector -m32 -Wall -nostdinc $(KERNEL_INCLUDE)
#NASM_OPTS := -I../include
NASM_OPTS := 

GCC := gcc
#DEBUG := -DDEBUG

PATH_LIB := ../lib
PATH_SYSCALLS := ./syscalls
PATH_APIS := ../apis

OBJETOS_SO := main.o funciones.o interrupciones.o funciones_tss.o \
              registros.o tss.o gdt.o proceso.o init.o \
              dma.o disquete.o admin_procesos.o tareas.o \
              temporizador.o cdrom.o ide.o \
              raton.o fat.o teclado.o

OBJETOS_US := $(PATH_APIS)/video.o $(PATH_APIS)/video_asm.o \
              $(PATH_APIS)/memoria.o $(PATH_APIS)/speaker.o \
              $(PATH_APIS)/timer.o $(PATH_APIS)/usuario.o \
              $(PATH_APIS)/ejecutar.o $(PATH_APIS)/esperar_pid.o \
              $(PATH_APIS)/salir.o $(PATH_APIS)/pausa.o \
              $(PATH_APIS)/matar.o $(PATH_APIS)/ps.o $(PATH_APIS)/reboot.o \
              $(PATH_APIS)/xchg_aprop.o

OBJETOS_SYS := $(PATH_SYSCALLS)/syscalls.o $(PATH_SYSCALLS)/sys_video.o \
               $(PATH_SYSCALLS)/sys_ejecutar.o $(PATH_SYSCALLS)/sys_salir.o \
               $(PATH_SYSCALLS)/sys_esperar_pid.o $(PATH_SYSCALLS)/sys_pausa.o \
               $(PATH_SYSCALLS)/sys_alocar.o $(PATH_SYSCALLS)/sys_dir.o \
               $(PATH_SYSCALLS)/sys_matar.o $(PATH_SYSCALLS)/sys_ps.o \
               $(PATH_SYSCALLS)/sys_reboot.o $(PATH_SYSCALLS)/sys_xchg_aprop.o \
               $(PATH_SYSCALLS)/sys_listar_directorio.o \
               $(PATH_SYSCALLS)/sys_es.o

all: sodero.sys

sodero.sys: sodero.bin main.bin
	cat sodero.bin main.bin > sodero.sys

.PHONY: syscalls
.PHONY: apis

main.bin: $(OBJETOS_SO) $(HEADERS) syscalls apis
	ld -melf_i386 -Map mapa_memoria.txt --cref -o main.ld -Ttext 0x0 \
	-N -e main $(OBJETOS_SO) $(OBJETOS_SYS) $(PATH_LIB)/biblioteca.a
	objcopy -R .comment -R .note -S -O binary main.ld main.bin

syscalls:
	make -C $(PATH_SYSCALLS)

apis:
	make -C $(PATH_APIS)

sodero.bin: sodero.asm main.bin
	./makeInit

main.o: main.c $(HEADERS)
	$(GCC) -c main.c $(CCOPTS)

interrupciones.o: interrupciones.c $(HEADERS)
	$(GCC) -c interrupciones.c $(CCOPTS)

tss.o: tss.c $(HEADERS)
	$(GCC) -c tss.c $(CCOPTS)

fat.o: fat.c $(HEADERS)
	$(GCC) -c fat.c $(CCOPTS)

teclado.o: teclado.c $(HEADERS)
	$(GCC) -c teclado.c $(CCOPTS)

dma.o: dma.c $(HEADERS)
	$(GCC) -c dma.c $(CCOPTS)

ide.o: ide.c $(HEADERS)
	$(GCC) -c ide.c $(CCOPTS)

disquete.o: disquete.c $(HEADERS)
	$(GCC) -c disquete.c $(CCOPTS)

gdt.o: gdt.c $(HEADERS)
	$(GCC) -c gdt.c $(CCOPTS)

proceso.o: proceso.c $(HEADERS)
	$(GCC) -c proceso.c $(CCOPTS)

init.o: init.c $(HEADERS)
	$(GCC) -c init.c $(CCOPTS)

tareas.o: tareas.c $(HEADERS)
	$(GCC) -c tareas.c $(CCOPTS)

admin_procesos.o: admin_procesos.c $(HEADERS)
	$(GCC) -c admin_procesos.c $(CCOPTS)

temporizador.o: temporizador.c $(HEADERS)
	$(GCC) -c temporizador.c $(CCOPTS)

cdrom.o: cdrom.c $(HEADERS)
	$(GCC) -c cdrom.c $(CCOPTS)

raton.o: raton.c $(HEADERS)
	$(GCC) -c raton.c $(CCOPTS)

funciones.o: funciones.asm
	nasm -f elf funciones.asm $(DEBUG) $(NASM_OPTS)

funciones_tss.o: funciones_tss.asm
	nasm -f elf funciones_tss.asm $(DEBUG) $(NASM_OPTS)

registros.o: registros.asm
	nasm -f elf registros.asm $(DEBUG) $(NASM_OPTS)


clean:
	make -C $(PATH_SYSCALLS) clean
	make -C $(PATH_APIS) clean
	$(RM) *.bin *.com mapa_memoria.txt *.o *.ld *.bkp *.dst *.sys .*.swp
