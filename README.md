# SO TP2

Repositorio para el **TP2** de la materia **Sistemas Operativos** en el **ITBA**

## Tabla de contenidos

- [SO TP2](#so-tp2)
  - [Tabla de contenidos](#tabla-de-contenidos)
  - [Autores](#autores)
  - [Requisitos](#requisitos)
  - [Instalacion](#instalacion)
    - [Docker](#docker)
    - [QEMU](#qemu)
      - [Descargar imagen](#descargar-imagen)
      - [Ejecutar el contenedor](#ejecutar-el-contenedor)
      - [Correr el contenedor](#correr-el-contenedor)
    - [Compilación y Ejecución](#compilación-y-ejecución)

## Autores

- Grupo 12:
  - Ben Deyheralde (Legajo: 63559)
  - Lucas Gonzalez Rouco (Legajo: 63366)
  - Matias Mutz (Legajo: 63590)

## Requisitos

Para la realizacion del trabajo se requieren los siguientes elementos:

- **docker:** Nos va a permitir compilar el TPE dentro de un contenedor
- **QEMU:** Nos va a permitir ejecutar el TPE en un emulador

## Instalacion

### Docker

La opcion mas facil va a ser ir a la pagina oficial de [docker](https://www.docker.com/products/docker-desktop/) donde podran elegir la version a instalar.

### QEMU

Al igual que con docker, lo mas recomendable es ir a la pagina oficial de [QEMU](https://www.qemu.org/download/) y seleccionar la version a instalar

#### Descargar imagen

```bash
docker pull agodio/itba-so:2.0
```

Y pueden revisar que la imagen aparezca utilizando:

```bash
docker images
```

#### Ejecutar el contenedor

```bash
docker run -v ${PWD}:/root --privileged -ti --name SO agodio/itba-so:2.0
```

Esto debería mostrar un prompt como el siguiente:

```bash
root@c3285f863835:/#
```

#### Correr el contenedor

```bash
docker start SO
docker exec -ti SO bash
```

### Compilación y Ejecución

Para compilar el proyecto se debe correr el siguiente comando (desde afuera del contenedor), esto ejecutara tambien el proyecto:

```bash
./docker.sh
```

> Puede que sea necesario cambiar la configuracion de audio en run.sh
