# Roteiro de Testes - Projeto 1

## Processamento de Imagens

Este documento registra os testes utilizados para verificar o funcionamento
do Projeto 1 da disciplina de Computação Visual.

O objetivo dos testes é verificar o carregamento de imagens, tratamento de
erros, conversão para escala de cinza, análise do histograma, equalização,
interface gráfica, alteração de resolução e salvamento da imagem.

---

## Ambiente utilizado

- Sistema operacional: Windows
- Compilador: GCC 15.2.0 - MSYS2 MinGW64
- Build: Makefile
- Bibliotecas:
  - SDL3
  - SDL3_image
  - SDL3_ttf

---

## Compilação

Antes dos testes, realizar uma compilação limpa:

```powershell
mingw32-make clean
mingw32-make
