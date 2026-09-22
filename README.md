# Projeto 1 - Processamento de Imagens

Projeto desenvolvido para a disciplina de **Computação Visual** da Universidade Presbiteriana Mackenzie.

A aplicação foi desenvolvida em linguagem C utilizando SDL3 para realizar operações de processamento de imagens, incluindo conversão para escala de cinza, análise do histograma, equalização e salvamento da imagem processada.

## Integrantes

- **Luiz Fernando Ferrari Batistela** - RA 10427397
- **Mauricio Custodio Vicentini** - RA 10426074
- **Victor Hong** - RA 10425852

---

## Funcionalidades

O programa permite:

- carregar uma imagem pela linha de comando;
- identificar imagens coloridas ou em escala de cinza;
- converter imagens coloridas para escala de cinza;
- calcular e exibir o histograma de 256 níveis;
- calcular média de intensidade e desvio padrão;
- classificar luminosidade e contraste;
- equalizar o histograma;
- alternar entre imagem original e equalizada;
- alternar entre resolução 1024x768 e resolução original;
- salvar a imagem atual em `output_image.png`;
- tratar arquivos inexistentes ou inválidos;
- utilizar uma interface gráfica com duas janelas.

---

## Funcionamento

O programa recebe o caminho da imagem como argumento:

```powershell
.\programa.exe samples\teste.png
```

Caso a imagem seja colorida, ela é convertida para escala de cinza utilizando:

```text
Y = 0.2125R + 0.7154G + 0.0721B
```

Depois da conversão, o programa preserva uma cópia da imagem original em escala de cinza, permitindo alternar entre a versão original e a equalizada sem carregar novamente o arquivo.

O histograma é calculado utilizando os 256 níveis possíveis de intensidade, de `0` a `255`.

A aplicação também calcula:

- média de intensidade;
- desvio padrão;
- classificação de luminosidade;
- classificação de contraste.

A equalização pode ser realizada pelo botão `Equalizar`. Após a operação, o botão passa a apresentar `Ver original`.

A janela principal inicia em `1024x768`, podendo ser alterada para a resolução original da imagem.

A tecla `S` salva a imagem atualmente apresentada em:

```text
output_image.png
```

---

## Estrutura do projeto

```text
Projeto1-Processamento-Imagens/
|
|-- assets/
|   `-- fonts/
|       `-- DejaVuSans.ttf
|
|-- docs/
|   `-- testes.md
|
|-- samples/
|
|-- src/
|   |-- histogram.c
|   |-- histogram.h
|   |-- image.c
|   |-- image.h
|   |-- main.c
|   |-- window.c
|   `-- window.h
|
|-- .gitignore
|-- Makefile
`-- README.md
```

### Organização do código

- `main.c`: fluxo principal da aplicação e integração entre os módulos.
- `image.c` / `image.h`: carregamento, escala de cinza, equalização, restauração e salvamento.
- `histogram.c` / `histogram.h`: cálculo do histograma, média, desvio padrão e classificações.
- `window.c` / `window.h`: criação das janelas, interface, botões, textos e renderização.
---

## Ambiente e tecnologias

O projeto foi desenvolvido e testado no seguinte ambiente:

| Componente | Versão |
|---|---|
| Sistema operacional | Microsoft Windows 10 Home Single Language - 10.0.19045 |
| Ambiente | MSYS2 MinGW64 |
| GCC | 15.2.0 |
| SDL3 | 3.4.16 |
| SDL3_image | 3.4.6 |
| SDL3_ttf | 3.2.2 |
| Make | mingw32-make |

Tecnologias utilizadas:

- Linguagem C;
- SDL3;
- SDL3_image;
- SDL3_ttf;
- GCC;
- Make;
- pkg-config.

---

## Dependências

No MSYS2 MinGW64 foram utilizados os pacotes:

```text
mingw-w64-x86_64-sdl3
mingw-w64-x86_64-sdl3-image
mingw-w64-x86_64-sdl3-ttf
```

O projeto também utiliza uma cópia da fonte DejaVu Sans localizada em:

```text
assets/fonts/DejaVuSans.ttf
```

---

## Compilação

A partir da raiz do projeto:

```powershell
mingw32-make
```

O executável gerado será:

```text
programa.exe
```

Para remover os arquivos gerados:

```powershell
mingw32-make clean
```

Para realizar uma recompilação completa:

```powershell
mingw32-make rebuild
```

---

## Execução

O programa exige um argumento contendo o caminho da imagem:

```powershell
.\programa.exe caminho_da_imagem
```

Exemplo:

```powershell
.\programa.exe samples\teste.png
```

Durante a execução:

- `Equalizar` aplica a equalização do histograma;
- `Ver original` restaura a imagem anterior;
- `Resolucao original` utiliza as dimensões originais da imagem;
- `1024x768` retorna à resolução inicial;
- a tecla `S` salva a imagem em `output_image.png`.

Caso nenhum arquivo seja informado o programa apresenta uma mensagem de uso.

Arquivos inexistentes ou inválidos também são tratados e informados no terminal
