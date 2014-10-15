TOOLS := ../tools
CCOPTS := -m32 -I../include -Wall -nostdinc
GCC := gcc
#DEBUG := -DDEBUG

PATH_LIB := ../lib

LIB := $(PATH_LIB)/biblioteca.a

all: dir interfaz ps matar reboot molestar aprop cat ayuda

dir: dir.o $(HEADERS) $(LIB)
	ld -melf_i386 -Map mapa_memoria.txt --cref -o dir.ld -Ttext 0x0 \
	-N -e main dir.o $(LIB)
	objcopy -R .comment -R .note -S -O binary dir.ld dir

interfaz: interfaz.o $(HEADERS) $(LIB)
	ld -Map mapa_memoria.txt --cref -o interfaz.ld -Ttext 0x0 \
	-N -e main interfaz.o $(LIB)
	objcopy -R .comment -R .note -S -O binary interfaz.ld interfaz

ps: ps.o $(HEADERS) $(LIB)
	ld -Map mapa_memoria.txt --cref -o ps.ld -Ttext 0x0 \
	-N -e main ps.o $(LIB)
	objcopy -R .comment -R .note -S -O binary ps.ld ps

matar: matar.o $(HEADERS) $(LIB)
	ld -Map mapa_memoria.txt --cref -o matar.ld -Ttext 0x0 \
	-N -e main matar.o $(LIB)
	objcopy -R .comment -R .note -S -O binary matar.ld matar

reboot: reboot.o $(HEADERS) $(LIB)
	ld -Map mapa_memoria.txt --cref -o reboot.ld -Ttext 0x0 \
	-N -e main reboot.o $(LIB)
	objcopy -R .comment -R .note -S -O binary reboot.ld reboot

molestar: molestar.o $(HEADERS) $(LIB)
	ld -Map mapa_memoria.txt --cref -o molestar.ld -Ttext 0x0 \
	-N -e main molestar.o $(LIB)
	objcopy -R .comment -R .note -S -O binary molestar.ld molestar

aprop: xchg_aprop.o $(HEADERS) $(LIB)
	ld -Map mapa_memoria.txt --cref -o aprop.ld -Ttext 0x0 \
	-N -e main xchg_aprop.o $(LIB)
	objcopy -R .comment -R .note -S -O binary aprop.ld aprop

cat: cat.o $(HEADERS) $(LIB)
	ld -Map mapa_memoria.txt --cref -o cat.ld -Ttext 0x0 \
	-N -e main cat.o $(LIB)
	objcopy -R .comment -R .note -S -O binary cat.ld cat

ayuda: ayuda.o $(HEADERS) $(LIB)
	ld -Map mapa_memoria.txt --cref -o ayuda.ld -Ttext 0x0 \
	-N -e main ayuda.o $(LIB)
	objcopy -R .comment -R .note -S -O binary ayuda.ld ayuda

dir.o: dir.c $(HEADERS)
	$(GCC) -c dir.c $(CCOPTS)

interfaz.o: interfaz.c $(HEADERS)
	$(GCC) -c interfaz.c $(CCOPTS)

ps.o: ps.c $(HEADERS)
	$(GCC) -c ps.c $(CCOPTS)

matar.o: matar.c $(HEADERS)
	$(GCC) -c matar.c $(CCOPTS)

reboot.o: reboot.c $(HEADERS)
	$(GCC) -c reboot.c $(CCOPTS)

molestar.o: molestar.c $(HEADERS)
	$(GCC) -c molestar.c $(CCOPTS)

xchg_aprop.o: xchg_aprop.c $(HEADERS)
	$(GCC) -c xchg_aprop.c $(CCOPTS)

cat.o: cat.c $(HEADERS)
	$(GCC) -c cat.c $(CCOPTS)

ayuda.o: ayuda.c $(HEADERS)
	$(GCC) -c ayuda.c $(CCOPTS)

install: all
	mcopy -o dir a:
	mcopy -o interfaz a:
	mcopy -o ps a:
	mcopy -o matar a:
	mcopy -o reboot a:
	mcopy -o molestar a:
	mcopy -o aprop a:
	mcopy -o cat a:
	mcopy -o ayuda a:

clean:
	$(RM) dir interfaz ps matar reboot molestar aprop cat ayuda \
	*.bin mapa_memoria.txt *.o *.ld *.bkp *.dst *.sys .*.swp
