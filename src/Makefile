BOOT := boot
NUCLEO := nucleo
USUARIO := usuario
SODERO_IMG := sodero.img
MNT_DIR := mnt

.PHONY: usuario

PATH_LIB := ./lib

all:
	@echo compilando el bootstrap...
	@make -C $(BOOT)
	@echo compilando el nucleo...
	@make -C $(NUCLEO)
	@echo compilando programas de usuario...
	@make -C $(NUCLEO)

install: all usuario
	qemu-img create -o size=1.44M $(SODERO_IMG)
	mkfs.msdos -n SODERO $(SODERO_IMG)
	sudo losetup /dev/loop0 $(SODERO_IMG)
	dd if=$(BOOT)/sodero_boot.bin of=/dev/loop0
	sudo losetup -d /dev/loop0
	ls $(MNT_DIR) 2> /dev/null 1>&2 || mkdir $(MNT_DIR)
	sudo mount $(SODERO_IMG) $(MNT_DIR)
	sudo cp $(NUCLEO)/sodero.sys $(MNT_DIR)
	sudo cp $(USUARIO)/aprop $(USUARIO)/ayuda $(USUARIO)/cat $(USUARIO)/dir \
		$(USUARIO)/interfaz $(USUARIO)/matar $(USUARIO)/molestar \
		$(USUARIO)/ps $(USUARIO)/reboot $(USUARIO)/salida \
		$(USUARIO)/ayuda.txt $(MNT_DIR)/
	sudo sync
	sudo umount $(MNT_DIR)
	rmdir $(MNT_DIR)

run: install
	qemu-system-i386 -fda $(SODERO_IMG)

clean:
	make -C $(BOOT) clean
	make -C $(NUCLEO) clean
	make -C $(USUARIO) clean
	$(RM) $(PATH_LIB)/* $(SODERO_IMG)

usuario:
	@echo compilando un proceso en un archivo de usuario...
	@make -C $(USUARIO)

usuarioinstall:
	@make install -C $(USUARIO)

usuarioclean:
	make -C $(USUARIO) clean
