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
