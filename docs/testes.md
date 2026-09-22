# Roteiro de Testes - Projeto 1

## Processamento de Imagens

Este documento registra os testes realizados para verificar o funcionamento do Projeto 1 da disciplina de Computação Visual.

O objetivo foi validar o carregamento de imagens, tratamento de erros, conversão para escala de cinza, análise e exibição do histograma, equalização, interface gráfica, alteração de resolução, salvamento da imagem e integração entre todas as funcionalidades desenvolvidas.

---

## Ambiente utilizado

- Sistema operacional: Windows
- Compilador: GCC 15.2.0 - MSYS2 MinGW64
- Sistema de compilação: Makefile
- Bibliotecas utilizadas:
  - SDL3
  - SDL3_image
  - SDL3_ttf
- Fonte utilizada na interface: DejaVu Sans
- Caminho da fonte: `assets/fonts/DejaVuSans.ttf`

---

# Compilação

Antes da execução dos testes foi realizada uma compilação limpa do projeto.

## Comandos

```powershell
mingw32-make clean
mingw32-make
```

## Resultado esperado

- compilação concluída sem erros;
- criação do executável `programa.exe`;
- criação dos arquivos objeto dentro da pasta `build/`;
- programa pronto para execução.

## Resultado obtido

**APROVADO.**

O projeto foi compilado corretamente e o executável `programa.exe` foi gerado.

Não foram encontrados erros de compilação que impedissem a execução do programa.

---

# Testes realizados

## Teste 01 - Execução sem argumento

### Comando

```powershell
.\programa.exe
```

### Resultado esperado

O programa deve informar que é necessário fornecer o caminho de uma imagem como argumento.

Nenhuma janela deve ser aberta.

### Resultado obtido

**APROVADO.**

O programa apresentou corretamente uma mensagem indicando a forma esperada de utilização:

```text
Uso: programa.exe caminho_da_imagem.ext
```

O programa foi encerrado normalmente sem abrir as janelas.

---

## Teste 02 - Arquivo inexistente

### Comando

```powershell
.\programa.exe samples\arquivo_que_nao_existe.png
```

### Resultado esperado

O programa deve informar que não foi possível carregar o arquivo informado.

Nenhuma janela deve ser aberta e o programa não deve apresentar travamentos.

### Resultado obtido

**APROVADO.**

O programa identificou corretamente que o arquivo não existia e apresentou uma mensagem de erro no terminal.

A aplicação foi encerrada normalmente.

---

## Teste 03 - Arquivo que não é uma imagem

Para o teste foi criado temporariamente um arquivo de texto:

```powershell
"isto nao e uma imagem" | Out-File samples\invalido.txt
```

Foi confirmado que o arquivo existia:

```powershell
Test-Path samples\invalido.txt
```

Em seguida:

```powershell
.\programa.exe samples\invalido.txt
```

### Resultado esperado

O SDL_image deve rejeitar o arquivo por não representar uma imagem válida.

O programa deve apresentar uma mensagem de erro no terminal e encerrar normalmente.

### Resultado obtido

**APROVADO.**

O arquivo inválido foi rejeitado corretamente.

O programa apresentou uma mensagem de erro no terminal e foi encerrado sem travamentos e sem abrir as janelas da aplicação.

Após o teste, o arquivo temporário foi removido:

```powershell
Remove-Item samples\invalido.txt
```

---

## Teste 04 - Carregamento de imagem colorida

### Comando

```powershell
.\programa.exe samples\teste.png
```

### Resultado esperado

O programa deve identificar que a imagem de entrada é colorida.

Em seguida, deve realizar a conversão para escala de cinza antes das demais operações.

### Resultado obtido

**APROVADO.**

O programa identificou corretamente a imagem colorida e realizou sua conversão para escala de cinza.

O terminal apresentou as mensagens correspondentes ao processo.

A imagem exibida na janela principal passou a utilizar tons de cinza.

---

## Teste 05 - Imagem já em escala de cinza

Foi utilizada uma imagem formada por pixels nos quais os valores dos canais vermelho, verde e azul eram iguais.

### Resultado esperado

O programa deve identificar que a imagem já está em escala de cinza.

Nesse caso, uma nova conversão não deve ser necessária.

### Resultado obtido

**APROVADO.**

O programa identificou corretamente a imagem já em escala de cinza e informou que uma nova conversão não era necessária.

---

## Teste 06 - Janela principal

### Resultado esperado

Ao iniciar o programa:

- a janela principal deve possuir inicialmente resolução de 1024x768 pixels;
- a imagem deve ser exibida na janela;
- a janela deve iniciar centralizada no monitor principal.

### Resultado obtido

**APROVADO.**

A janela principal foi criada corretamente com resolução inicial de 1024x768.

A imagem foi apresentada corretamente e a janela iniciou centralizada.

---

## Teste 07 - Janela secundária

### Resultado esperado

A janela secundária deve:

- ser criada juntamente com a janela principal;
- funcionar como janela secundária da aplicação;
- possuir tamanho fixo;
- apresentar o histograma;
- apresentar as informações estatísticas;
- apresentar os botões de interação.

### Resultado obtido

**APROVADO.**

A janela secundária foi criada e apresentou corretamente os elementos esperados.

O histograma, as informações estatísticas e os botões ficaram disponíveis para interação.

---

## Teste 08 - Histograma

### Resultado esperado

O programa deve calcular e exibir um histograma correspondente aos 256 níveis possíveis de intensidade da imagem em escala de cinza.

As barras devem possuir alturas proporcionais à frequência dos pixels de cada intensidade.

Também devem ser apresentadas informações relacionadas a:

- média de intensidade;
- classificação de luminosidade;
- desvio padrão;
- classificação de contraste.

### Resultado obtido

**APROVADO.**

O histograma foi calculado e renderizado corretamente.

As barras apresentaram distribuição correspondente às intensidades presentes na imagem.

As informações de média, luminosidade, desvio padrão e contraste também foram exibidas corretamente.

---

## Teste 09 - Equalização do histograma

Foi pressionado o botão:

```text
Equalizar
```

### Resultado esperado

Após o clique:

- a imagem deve ser equalizada;
- a imagem apresentada na janela principal deve ser atualizada;
- o histograma deve ser recalculado;
- as informações estatísticas devem ser recalculadas;
- o botão deve passar a mostrar `Ver original`.

### Resultado obtido

**APROVADO.**

A equalização foi executada corretamente.

A imagem apresentada foi atualizada e o histograma mudou de acordo com a nova distribuição de intensidades.

As informações estatísticas também foram atualizadas.

O texto do botão passou para:

```text
Ver original
```

---

## Teste 10 - Retorno à imagem original

Após realizar a equalização, foi pressionado:

```text
Ver original
```

### Resultado esperado

A aplicação deve:

- restaurar a imagem original em escala de cinza;
- restaurar o histograma correspondente;
- atualizar novamente as informações estatísticas;
- alterar o botão para `Equalizar`;
- restaurar a imagem utilizando a cópia mantida em memória.

### Resultado obtido

**APROVADO.**

A imagem original foi restaurada corretamente.

O histograma e as informações estatísticas voltaram a corresponder à imagem original.

O botão voltou a apresentar:

```text
Equalizar
```

---

## Teste 11 - Vários ciclos de equalização

Foi executada repetidamente a sequência:

```text
Equalizar
Ver original
Equalizar
Ver original
Equalizar
Ver original
```

### Resultado esperado

A aplicação deve continuar funcionando corretamente após vários ciclos de alteração.

Não devem ocorrer:

- travamentos;
- corrupção da imagem;
- perda da imagem original;
- inconsistências no histograma.

### Resultado obtido

**APROVADO.**

Todos os ciclos foram executados corretamente.

Não foram observados travamentos, corrupção da imagem ou perda da imagem original.

---

## Teste 12 - Estados visuais dos botões

Os dois botões foram testados nos seguintes estados:

- estado normal;
- mouse posicionado sobre o botão;
- botão pressionado.

### Resultado esperado

Cada estado deve apresentar uma aparência visual diferente para fornecer retorno ao usuário.

### Resultado obtido

**APROVADO.**

Os botões responderam corretamente aos eventos do mouse.

Os estados normal, hover e pressionado apresentaram diferenças visuais.

---

## Teste 13 - Resolução original

Foi pressionado o botão:

```text
Resolucao original
```

### Resultado esperado

A janela principal deve passar a utilizar exatamente as dimensões originais da imagem carregada.

O texto do botão deve passar para:

```text
1024x768
```

### Resultado obtido

**APROVADO.**

A janela principal foi redimensionada corretamente para a largura e altura originais da imagem.

O texto do botão foi atualizado corretamente.

---

## Teste 14 - Retorno para 1024x768

Após utilizar a resolução original, foi pressionado:

```text
1024x768
```

### Resultado esperado

A janela principal deve voltar para:

```text
1024x768
```

A janela também deve voltar a ser centralizada no monitor.

O botão deve novamente apresentar:

```text
Resolucao original
```

### Resultado obtido

**APROVADO.**

A janela retornou corretamente para 1024x768 e foi novamente centralizada.

O botão também retornou ao texto esperado.

---

## Teste 15 - Imagem maior que o monitor

Foi utilizada uma imagem cuja resolução original era superior à resolução disponível no monitor.

Após o carregamento foi selecionado:

```text
Resolucao original
```

### Resultado esperado

A janela principal deve assumir a resolução original da imagem.

Como a janela é maior que a área disponível do monitor, seu canto superior esquerdo deve ser posicionado em:

```text
0,0
```

### Resultado obtido

**APROVADO.**

A aplicação utilizou corretamente as dimensões originais da imagem.

A janela foi posicionada conforme esperado quando suas dimensões ultrapassaram a resolução disponível do monitor.

Ao retornar para 1024x768, a janela voltou a ser centralizada.

---

## Teste 16 - Criação de output_image.png

Antes do teste, uma possível saída anterior foi removida:

```powershell
Remove-Item .\output_image.png -ErrorAction SilentlyContinue
```

Com a aplicação aberta, foi pressionada a tecla:

```text
S
```

### Resultado esperado

O programa deve criar o arquivo:

```text
output_image.png
```

Também deve informar no terminal que o arquivo foi criado.

### Resultado obtido

**APROVADO.**

O arquivo `output_image.png` foi criado corretamente.

A aplicação apresentou uma mensagem correspondente no terminal.

---

## Teste 17 - Sobrescrita de output_image.png

Com `output_image.png` já existente, a tecla `S` foi pressionada novamente.

### Resultado esperado

O arquivo existente deve ser substituído pela imagem atualmente apresentada.

O programa deve informar que o arquivo foi sobrescrito.

### Resultado obtido

**APROVADO.**

O arquivo existente foi sobrescrito corretamente e o programa informou a operação no terminal.

---

## Teste 18 - Salvamento da imagem equalizada

Foi executada a sequência:

```text
Equalizar
S
```

### Resultado esperado

O arquivo `output_image.png` deve corresponder à versão equalizada atualmente apresentada na aplicação.

### Resultado obtido

**APROVADO.**

A imagem equalizada foi salva corretamente.

O conteúdo do arquivo correspondeu ao estado atual da imagem apresentada.

---

## Teste 19 - Salvamento em 1024x768

A aplicação foi mantida no modo:

```text
1024x768
```

Em seguida, foi pressionada a tecla:

```text
S
```

### Resultado esperado

O arquivo salvo deve possuir resolução de:

```text
1024x768
```

### Resultado obtido

**APROVADO.**

A imagem foi salva corretamente com resolução de 1024x768 pixels.

---

## Teste 20 - Salvamento na resolução original

Foi selecionada a opção:

```text
Resolucao original
```

Em seguida foi pressionada a tecla:

```text
S
```

### Resultado esperado

O arquivo salvo deve possuir as mesmas dimensões da imagem original.

### Resultado obtido

**APROVADO.**

A imagem foi salva corretamente utilizando sua resolução original.

---

## Teste 21 - Fonte da interface

### Resultado esperado

A fonte DejaVu Sans deve ser carregada pelo programa a partir do arquivo:

```text
assets/fonts/DejaVuSans.ttf
```

A aplicação não deve depender de uma fonte específica instalada no sistema operacional.

Os seguintes textos devem ser exibidos corretamente:

- Histograma;
- média;
- classificação da luminosidade;
- desvio padrão;
- classificação do contraste;
- Equalizar;
- Ver original;
- Resolucao original;
- 1024x768.

### Resultado obtido

**APROVADO.**

A fonte foi carregada corretamente pelo SDL_ttf.

Todos os textos da interface foram apresentados corretamente.

---

## Teste 22 - Integração entre funcionalidades

Foi executada a seguinte sequência de operações:

```text
Equalizar
Resolucao original
S
Ver original
1024x768
S
Equalizar
S
Ver original
```

### Resultado esperado

As diferentes funcionalidades devem continuar funcionando corretamente quando utilizadas em sequência.

Devem permanecer sincronizados:

- imagem;
- equalização;
- histograma;
- estatísticas;
- resolução;
- salvamento.

### Resultado obtido

**APROVADO.**

Todas as funcionalidades permaneceram sincronizadas durante a sequência de operações.

Não foram observados travamentos ou comportamentos inconsistentes.

---

## Teste 23 - Fechamento da aplicação

Foram testados separadamente:

- fechamento da janela principal;
- fechamento da janela secundária.

### Resultado esperado

Ao fechar qualquer uma das janelas, a aplicação deve ser encerrada normalmente.

Os recursos utilizados pelo programa devem ser liberados.

### Resultado obtido

**APROVADO.**

A aplicação foi encerrada corretamente em ambos os casos.

Não foram observados travamentos durante o encerramento.

---

# Problemas identificados durante os testes

Durante o desenvolvimento e a execução dos testes foram identificados problemas que precisaram ser corrigidos antes da versão final.

---

## 1. Arquivo da fonte DejaVu Sans inválido

Inicialmente, o arquivo:

```text
assets/fonts/DejaVuSans.ttf
```

não correspondia a um arquivo TrueType válido.

O arquivo possuía tamanho incorreto e não podia ser carregado pelo SDL_ttf.

### Consequência

Os textos da interface não eram apresentados corretamente porque a biblioteca não conseguia abrir a fonte.

### Correção realizada

O arquivo foi substituído por uma versão válida da fonte DejaVu Sans.

Após a correção, o SDL_ttf passou a carregar a fonte corretamente e os textos foram apresentados normalmente.

---

## 2. Saída do terminal no Windows

Durante os testes foi identificado que o `pkg-config` adicionava automaticamente a opção:

```text
-mwindows
```

durante a etapa de linkedição no Windows.

### Consequência

As mensagens utilizadas pelo programa através de `printf` e `fprintf` não eram apresentadas adequadamente no terminal.

Isso prejudicava principalmente os testes de:

- execução sem argumento;
- arquivo inexistente;
- arquivo inválido;
- mensagens de salvamento.

### Correção realizada

O Makefile foi alterado para remover a opção `-mwindows` das bibliotecas retornadas pelo `pkg-config`.

Foi utilizada a seguinte definição:

```makefile
SDL_LIBS := $(filter-out -mwindows,$(shell $(PKG_CONFIG) --libs sdl3 sdl3-image sdl3-ttf))
```

Após essa alteração, as mensagens passaram a ser exibidas corretamente no terminal.

---

# Resultado geral

Após a execução da bateria de testes, as funcionalidades implementadas apresentaram o comportamento esperado.

Foram validados:

- carregamento de imagens;
- tratamento de caminho inexistente;
- tratamento de arquivos inválidos;
- identificação de imagens coloridas;
- identificação de imagens em escala de cinza;
- conversão para escala de cinza;
- criação da janela principal;
- criação da janela secundária;
- cálculo do histograma;
- renderização do histograma;
- cálculo da média de intensidade;
- classificação da luminosidade;
- cálculo do desvio padrão;
- classificação do contraste;
- equalização do histograma;
- retorno à imagem original;
- manutenção da imagem original em memória;
- estados visuais dos botões;
- alteração para resolução original;
- retorno para 1024x768;
- tratamento de janelas maiores que o monitor;
- salvamento da imagem;
- sobrescrita de `output_image.png`;
- salvamento da imagem equalizada;
- salvamento em 1024x768;
- salvamento na resolução original;
- carregamento da fonte com SDL_ttf;
- integração entre as funcionalidades;
- fechamento correto da aplicação.

---

# Situação final

Todos os testes realizados foram concluídos com sucesso.

Os problemas encontrados durante o processo de testes foram corrigidos e posteriormente testados novamente.

A aplicação apresentou funcionamento estável nos cenários avaliados e não foram identificados erros que impeçam a utilização das funcionalidades implementadas.
