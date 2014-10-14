#!/bin/sh

bad=0
clear
echo "Bienvenido al programa instalador de SODERO V0.01"
echo -n "Presione una tecla para comenzar con la instalacion..."
read
echo "Instalando el bootstrap..."
dd if=./boot/sodero_boot.bin of=/dev/fd0 > /dev/null

if [ $? -ne 0 ];then
	echo "Hubo un error al instalar el bootstrap!"
	exit 1
fi

echo "Instalando el nucleo de SODERO..."
mcopy -o ./nucleo/sodero.sys a:

if [ $? -ne 0 ];then
	echo "Hubo un error al instalar el nucleo de SODERO!"
	exit 1
fi

echo -n "Instalando archivos de usuario."
mcopy -o ./usuario/aprop a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/ayuda a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/ayuda.txt a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/cat a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/dir a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/interfaz a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/matar a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/molestar a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/ps a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

mcopy -o ./usuario/reboot a: > /dev/null
echo -n "."
if [ "$?" -ne 0 ]; then
	bad=1
fi

echo ""

if [ "$bad" -eq 1 ]; then
	echo "Hubo errores en al menos un archivo de usuario"
	exit 1
fi

echo "Felicitaciones! Acaba de instalar exitosamente SODERO en su PC"
echo "Reinicie la PC y sorprendase!"
