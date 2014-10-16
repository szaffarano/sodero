## Proyecto SODERO

Trabajo de fin de carrera realizado allá por el año 2002 por

- Micaela Antelo
- Matías Blasi
- Hernán Mazzoni
- Sebastián Zaffarano

Se trata de un SO multitarea de 32 bits desarrollado en ANSI C y assembler (NASM).

## Modo de uso

Se adaptaron los makefiles para poder ejecutar el SO dentro de un ambiente [qemu](http://wiki.qemu.org/Main_Page),
por tal motivo, hay que tener previamente instalado dicho paquete.

```sh
# Compila SO y programas de usuario
$ make all
# Instala el SO y los programas de usuario en una imagen raw
$ make install
# Lanza qemu, booteando el SO
$ make run
```
