# Projeto 1 - Processamento de Imagens

Projeto desenvolvido para a disciplina de **Computação Visual** da Universidade Presbiteriana Mackenzie.

A aplicação foi desenvolvida em linguagem C utilizando a biblioteca SDL3 e bibliotecas auxiliares para realizar operações de processamento de imagens.

O programa permite carregar uma imagem através da linha de comando, convertê-la para escala de cinza quando necessário, calcular e visualizar seu histograma, analisar luminosidade e contraste, realizar equalização, alternar entre diferentes resoluções e salvar a imagem processada.

---

## Integrantes

- **Luiz Fernando Ferrari Batistela** - RA 10427397
- **Mauricio Custodio Vicentini** - RA 10426074
- **Victor Hong** - RA 10425852

---

## Funcionalidades

O programa implementa as seguintes funcionalidades:

- carregamento de uma imagem informada através da linha de comando;
- tratamento de caminho inexistente ou arquivo inválido;
- identificação automática de imagens coloridas ou já em escala de cinza;
- conversão de imagens coloridas para escala de cinza;
- exibição da imagem processada em uma janela principal;
- criação de uma janela secundária para informações e controles;
- cálculo do histograma da imagem;
- representação gráfica dos 256 níveis de intensidade;
- cálculo da média de intensidade dos pixels;
- classificação da luminosidade da imagem;
- cálculo do desvio padrão das intensidades;
- classificação do contraste da imagem;
- equalização do histograma;
- alternância entre imagem equalizada e imagem original;
- alternância entre resolução 1024x768 e resolução original da imagem;
- tratamento de imagens cuja resolução seja superior à resolução do monitor;
- estados visuais dos botões;
- salvamento da imagem atualmente apresentada;
- sobrescrita automática do arquivo de saída;
- exibição de textos utilizando SDL_ttf;
- encerramento controlado da aplicação e liberação dos recursos utilizados.

---

## Funcionamento

O programa recebe através da linha de comando o caminho da imagem que será processada.

Exemplo:

```powershell
.\programa.exe samples\teste.png
```

Após o carregamento, o programa verifica se a imagem já está em escala de cinza.

Caso a imagem seja colorida, é realizada automaticamente uma conversão para escala de cinza.

A conversão utiliza a seguinte equação de luminância:

```text
Y = 0.2125R + 0.7154G + 0.0721B
```

Onde:

- `R` representa o canal vermelho;
- `G` representa o canal verde;
- `B` representa o canal azul;
- `Y` representa o valor de intensidade em escala de cinza.

Depois da conversão, uma cópia da imagem em escala de cinza é preservada em memória.

Essa cópia é utilizada para permitir que o usuário alterne entre a imagem original e a imagem equalizada sem precisar carregar novamente o arquivo do disco.

Em seguida, o programa calcula o histograma, a média de intensidade e o desvio padrão da imagem.

A aplicação então cria duas janelas:

- uma janela principal para exibição da imagem;
- uma janela secundária para o histograma, informações estatísticas e controles.

---

## Histograma

O programa calcula um histograma contendo **256 níveis de intensidade**, correspondentes aos valores possíveis de uma imagem em escala de cinza:

```text
0 até 255
```

Para cada pixel da imagem, sua intensidade é utilizada como índice no vetor do histograma.

A posição correspondente é incrementada, permitindo contabilizar quantos pixels possuem cada nível de intensidade.

Na interface gráfica, as barras do histograma possuem alturas proporcionais às frequências encontradas.

Dessa forma, é possível visualizar a distribuição das intensidades presentes na imagem.

---

## Média de intensidade

A média é calculada utilizando as intensidades de todos os pixels da imagem.

A expressão utilizada pode ser representada por:

```text
media = soma(intensidade * quantidade) / total_de_pixels
```

O valor obtido também é utilizado para realizar uma classificação aproximada da luminosidade da imagem.

Os intervalos definidos pelo grupo são:

| Média de intensidade | Classificação |
|---|---|
| Menor que 85 | Escura |
| De 85 até abaixo de 171 | Média |
| 171 ou superior | Clara |

Esses intervalos foram definidos pelo grupo para dividir aproximadamente a faixa de intensidades de 0 a 255 em três regiões.

---

## Contraste

O contraste da imagem é analisado através do **desvio padrão das intensidades**.

O desvio padrão representa o quanto os valores dos pixels estão dispersos em relação à média.

De maneira geral, quanto maior a dispersão das intensidades, maior tende a ser o contraste presente na imagem.

Os intervalos utilizados pelo grupo são:

| Desvio padrão | Classificação |
|---|---|
| Menor que 42,5 | Baixo |
| De 42,5 até abaixo de 85 | Médio |
| 85 ou superior | Alto |

Esses limites foram definidos pelo grupo como um critério simples para interpretar o valor calculado.

---

## Equalização do histograma

A aplicação permite realizar a equalização da imagem através do botão:

```text
Equalizar
```

A equalização utiliza a distribuição acumulada das intensidades presentes no histograma para calcular novos valores de intensidade.

Após a operação:

- a imagem exibida é atualizada;
- o histograma é recalculado;
- a média é recalculada;
- o desvio padrão é recalculado;
- as classificações de luminosidade e contraste são atualizadas;
- o botão passa a apresentar `Ver original`.

Quando o usuário seleciona:

```text
Ver original
```

a aplicação restaura a cópia da imagem original em escala de cinza mantida em memória.

Dessa forma, não é necessário realizar novamente a leitura do arquivo original.

---

## Resolução da imagem

A janela principal inicia com resolução de:

```text
1024x768
```

Através do botão:

```text
Resolucao original
```

é possível alterar o tamanho da janela para utilizar exatamente as dimensões originais da imagem carregada.

Quando a resolução original está ativa, o botão passa a apresentar:

```text
1024x768
```

permitindo retornar ao tamanho inicial.

Quando as dimensões da imagem ultrapassam a área disponível do monitor, a janela é posicionada a partir do canto superior esquerdo da tela.

---

## Salvamento da imagem

A imagem atualmente apresentada pode ser salva utilizando a tecla:

```text
S
```

O arquivo é gerado com o nome:

```text
output_image.png
```

Caso o arquivo ainda não exista, ele é criado.

Caso já exista, ele é sobrescrito.

O salvamento considera:

- o estado atual da imagem;
- a resolução atualmente selecionada.

Dessa maneira, podem ser salvas diferentes combinações, como:

```text
Imagem original + 1024x768
Imagem equalizada + 1024x768
Imagem original + resolução original
Imagem equalizada + resolução original
```

---

## Interface gráfica

A aplicação possui duas janelas.

### Janela principal

A janela principal é responsável pela apresentação da imagem.

Sua resolução inicial é:

```text
1024x768
```

A imagem carregada é transformada em uma textura SDL e renderizada nessa janela.

A janela também pode ser redimensionada para utilizar as dimensões originais da imagem.

### Janela secundária

A janela secundária apresenta as informações relacionadas ao processamento da imagem.

Ela contém:

- histograma;
- média de intensidade;
- classificação da luminosidade;
- desvio padrão;
- classificação do contraste;
- botão de equalização;
- botão de alteração da resolução.

Os botões possuem três estados visuais:

- normal;
- hover;
- pressionado.

Esses estados fornecem retorno visual ao usuário durante a interação com a aplicação.

---

## Fonte da interface

Os textos da interface são renderizados utilizando a biblioteca:

```text
SDL3_ttf
```

A fonte utilizada pelo projeto é:

```text
DejaVu Sans
```

O arquivo está armazenado em:

```text
assets/fonts/DejaVuSans.ttf
```

Manter a fonte dentro do próprio projeto evita depender de uma fonte específica instalada no sistema operacional do computador utilizado para executar a aplicação.

---

## Estrutura do projeto

A organização atual do projeto é:

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
|   `-- imagens utilizadas para testes
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

---

## Organização do código

O código-fonte foi dividido em módulos para separar as principais responsabilidades da aplicação.

### `main.c`

Responsável pelo fluxo principal do programa.

Entre suas principais responsabilidades estão:

- validação dos argumentos recebidos pela linha de comando;
- inicialização da SDL;
- carregamento da imagem;
- conversão para escala de cinza;
- preservação da imagem original;
- cálculo inicial do histograma;
- criação das janelas;
- gerenciamento do loop principal;
- tratamento dos eventos;
- integração entre os diferentes módulos;
- salvamento da imagem;
- encerramento da aplicação.

### `image.c` e `image.h`

Responsáveis pelas operações relacionadas ao processamento das imagens.

Principais funcionalidades:

- carregamento da imagem;
- conversão para o formato utilizado pelo programa;
- identificação de imagens em escala de cinza;
- conversão para escala de cinza;
- preservação da imagem original;
- equalização;
- restauração da imagem original;
- salvamento em PNG;
- gerenciamento da memória utilizada pelas superfícies.

### `histogram.c` e `histogram.h`

Responsáveis pelas operações de análise das intensidades.

Principais funcionalidades:

- cálculo das 256 posições do histograma;
- determinação da maior frequência do histograma;
- contagem do número total de pixels;
- cálculo da média;
- cálculo do desvio padrão;
- classificação da luminosidade;
- classificação do contraste.

### `window.c` e `window.h`

Responsáveis pela interface gráfica da aplicação.

Principais funcionalidades:

- criação da janela principal;
- posicionamento da janela principal;
- criação da janela secundária;
- criação das texturas;
- renderização da imagem;
- renderização do histograma;
- renderização dos textos;
- gerenciamento dos botões;
- tratamento dos estados dos botões;
- gerenciamento dos eventos do mouse;
- alteração da resolução da janela principal.
