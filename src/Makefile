BOOT := boot
NUCLEO := nucleo
USUARIO := usuario

PATH_LIB := ./lib

all:
	@echo compilando el bootstrap...
	@make -C $(BOOT)
	@echo compilando el nucleo...
	@make -C $(NUCLEO)

install: all
	dd if=$(BOOT)/sodero_boot.bin of=/dev/fd0
	-mdel a:\sodero.sys
	mcopy $(NUCLEO)/sodero.sys a:

clean:
	make -C $(BOOT) clean
	make -C $(NUCLEO) clean
	make -C $(USUARIO) clean
	$(RM) $(PATH_LIB)/*

usuario:
	@echo compilando un proceso en un archivo de usuario...
	@make -C $(USUARIO)

usuarioinstall:
	@make install -C $(USUARIO)

usuarioclean:
	make -C $(USUARIO) clean
