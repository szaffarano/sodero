TOOLS := ../tools

KERNEL_INCLUDE := -I../include

CCOPTS := -m32 -Wall -nostdinc $(KERNEL_INCLUDE)
#NASM_OPTS := -I../include
NASM_OPTS := 

GCC := gcc
#DEBUG := -DDEBUG

PATH_LIB := ../lib
PATH_SYSCALLS := ./syscalls

OBJETOS_US := video.o video_asm.o puertos.o memoria.o speaker.o \
              timer.o usuario.o ejecutar.o esperar_pid.o salir.o pausa.o \
              dir.o matar.o ps.o reboot.o xchg_aprop.o es.o

all: $(OBJETOS_US) biblioteca.a

biblioteca.a: $(OBJETOS_US)
	ar rv $(PATH_LIB)/biblioteca.a $(OBJETOS_US)

video.o: video.c $(HEADERS)
	$(GCC) -c video.c $(CCOPTS)

video_asm.o: video_asm.asm
	nasm -f elf video_asm.asm $(DEBUG) $(NASM_OPTS)

puertos.o: puertos.asm
	nasm -f elf puertos.asm $(DEBUG) $(NASM_OPTS)

memoria.o: memoria.c $(HEADERS)
	$(GCC) -c memoria.c $(CCOPTS)

speaker.o: speaker.c $(HEADERS)
	$(GCC) -c speaker.c $(CCOPTS)

timer.o: timer.asm
	nasm -f elf timer.asm $(DEBUG) $(NASM_OPTS)

usuario.o: usuario.asm
	nasm -f elf usuario.asm $(DEBUG) $(NASM_OPTS)

ejecutar.o: ejecutar.c $(HEADERS)
	$(GCC) -c ejecutar.c $(CCOPTS)

esperar_pid.o: esperar_pid.c $(HEADERS)
	$(GCC) -c esperar_pid.c $(CCOPTS)

salir.o: salir.c $(HEADERS)
	$(GCC) -c salir.c $(CCOPTS)

pausa.o: pausa.c $(HEADERS)
	$(GCC) -c pausa.c $(CCOPTS)

dir.o: dir.c $(HEADERS)
	$(GCC) -c dir.c $(CCOPTS)

ps.o: ps.c $(HEADERS)
	$(GCC) -c ps.c $(CCOPTS)

matar.o: matar.c $(HEADERS)
	$(GCC) -c matar.c $(CCOPTS)

reboot.o: reboot.c $(HEADERS)
	$(GCC) -c reboot.c $(CCOPTS)

xchg_aprop.o: xchg_aprop.c $(HEADERS)
	$(GCC) -c xchg_aprop.c $(CCOPTS)

es.o: es.c $(HEADERS)
	$(GCC) -c es.c $(CCOPTS)

clean:
	$(RM) *.bin $(PATH_LIB)/*.a *.o *.ld *.bkp *.dst *.sys .*.swp
