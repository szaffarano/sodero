boot: sodero_boot.bin

sodero_boot.bin: sodero_boot.asm
	nasm -o sodero_boot.bin sodero_boot.asm

clean:
	$(RM) *.bin *.com mapa_memoria.txt *.o *.ld *.bkp *.dst *.sys
