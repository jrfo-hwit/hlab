
# H-Lab

**Laboratório educacional de sistemas embarcados HwIT**

Esta plataforma educacional de sistemas embarcados teve como base de sua placa principal (main_hw) o [BitDogLab](https://github.com/Fruett/BitDogLab) [![License: CERN-OHL-S v2.0](https://img.shields.io/badge/License-CERN--OHL--S%20v2.0-blue.svg)](https://cern.ch/cern-ohl), um projeto totalmente aberto, que permite que os seus usuários copiem, fabriquem, montem e melhorem como bem entenderem.

Este repositório contem todas as informações das evoluções executadas pela HwIT no [BitDogLab](https://github.com/Fruett/BitDogLab) (versões 5.3, 6.0, 6.2, 6.3 - versão do programa [Embarcatech](https://embarcatech.softex.br/) e 6.4 - ultima evolução executada pela HwIT), a próxima versão em desenvolvimento será placa HLab 1.0.

## Placa principal

Além da placa principal (main_hw), existe a placa do painel frontal de conexão (front_panel_hw) compondo a solução [BitDogLab](https://github.com/Fruett/BitDogLab).

## Placas periféricas

Esse repositório apresenta os periféricos desenvolvidos para segunda fase do programa [Embarcatech](https://embarcatech.softex.br/), Os periféricos são compostos pelos seguintes componentes:

* Kit Básico de Periféricos (12 itens)
  * Periferico adaptador de sensores/atuadores
      * [Sensor i2c] Acelerômetro
      * [Sensor i2c] Oxímetro e batimentos cardiácos
      * [Sensor i2c] Medição de distância a Laser
      * [Sensor i2c] Temperatura e pressão
      * [Sensor i2c] Luminosidade
      * [Sensor i2c] Umidade e temperatura
      * [Sensor i2c] Cor RGB
      * [Atuador PWM] Servo Motor
  * Periférico HDMI
  * Periférico de extensão de conexões para sensores (i2c)
  * Periférico teclado matricial (GPIO)
  * Periférico de armazenamento de dados SDCARD (SPI)

* Kit Avançado de Periféricos (19 itens) = Kit Básico (12) + itens abaixo (7)
  * Periférico de extensão de conexões (SPI)
  * Periferico conversor analógico digital ADC / digital analógico DAC / GPIO com oito portas (SPI)
  * Periférico câmera de 2M Pixels (interface paralela)
  * Periférico display LCD 320x240 pixels (SPI)
  * Periférico comunicação radio de longa distância LoRA (SPI)
  * Periférico GPS (UART)
  * Periférico Pi Pico debug probe (SWD)

:warning: Acesse esse [link](https://github.com/jrfo-hwit/hlab/tree/main/hardware/peripherals_hw) para obter maiores detalhes sobre cada um dos periféricos

A Placa HLab 1.0 vai integrar a placa principal com o painel frontal, e será evoluida para atender não só as demandas de sistemas embarcados, mas também as demandas de inteligência artifical de borda (EDGE AI) e aprendizado de máquina (Machine Learning), toltamente integradas com os periféricos já desenvolvidos para plataforma atual.

## Micropython

:warning: Esse repositório contém firmwares do interpretador micropython compilado para RP2040 (Pi Pico e Pi Pico W) contida na placa principal, [acesse aqui os últimos releases](https://github.com/jrfo-hwit/hlab/tree/main/firmware/micropython/releases). Esse firmware contém além do micropython todas as bibliotecas necessárias para interagir com a placa principal e seus respectivos periféricos.

**A maneira mais fácil de começar se você é iniciante com a plataforma, é por meio da leitura do nosso [guia do iniciante](https://github.com/jrfo-hwit/hlab/tree/main/firmware/micropython/guia-do-iniciante.md).**

## C/C++

Para usuários avançados que desejam utilizar todo o poder do processador RP2040 pode fazer o mesmo por meio da liguagem C++. Se vocês sabe o que está fazendo e gostaria de construir seu próprio projeto de firmware, você deve utilizar o SDK [Pi Pico](https://github.com/raspberrypi/pico-sdk) e pode utilizar nosso [projeto exemplo](https://github.com/jrfo-hwit/hlab/tree/main/firmware/c_cpp/boilerplate), ou se basear em nossos [exemplos de uso da placa principal e/ou periféricos](https://github.com/jrfo-hwit/hlab/tree/main/firmware/c_cpp/examples).

# Licença

Todas as versões BitDogLab estão sob a licença CERN Open Hardware Licence Version 2 - Strongly Reciprocal [![License: CERN-OHL-S v2.0](https://img.shields.io/badge/License-CERN--OHL--S%20v2.0-blue.svg)](https://cern.ch/cern-ohl).
Para mais detalhes veja o arquivo `LICENSE`.

Os demais hardwares periféricos e a futura placa H-Lab estão sob a licença MIT [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT). Para mais detalhes veja o arquivo `LICENSE`.

# Estrutura do Repositório

Segue abaixo a estrutura de arquivos desse repositório. 

O repositório se divide em firmware (código que controla o processador principal da BitDogLab/HLab) contendo tanto código micropython (usuários básicos) quanto c/c++ (usuários avançados), seguido do hardware eletrônico que contém as informações de todas as placas (principal, painel frontal e periféricos), seguido software contendo informações e código para plataforma low code, mecânica contendo peças e partes mecânicas de suporte ao hardware, e fechando com a pasta educacional contendo informações e material para treinamento de professores, instruturoes e alunos.

```bash
├───education "Arquivos e documentação para treinamentos e aprendizado"
│   ├───teacher "Materiais de treinamento para professores"
│   ├───instructor "Materiais de treinamento para instrutores"
│   └───student "Aulas para estudantes"
├───firmware "Nesta pasta ficam os arquivos do firmware, seja micropython ou c/c++"
│   └───micropython "Interpretador (arquivos fonte e uf2 para carregarmos na Pi Pico) e todos exemplos"
│       ├───micropython "Código fonte e compilado do interpretador micropython"
│       ├───pimoroni-pico "Código fonte pimoroni-pico, contendo bilbiotecas e micropython"
│       ├───releases "firmware compilado micropython"
│       └───examples "Arquivos de exemplo de firmware incluindo codigo fonte e documentação"
│   └───c_cpp "Aqui ficam os arquivos do SDK (submodule) e todos os exemplos"
│       ├───pico_sdk "Link simbolico para SDK"
│       └───examples "Arquivos de exemplo de firmware incluindo codigo fonte e documentação"
├───hardware "Aqui ficam todos os arquivos de hardware para placa principal, painel frontal e perifericos"
│   └───main_hw "Esquemático, Layout and Gerber da placa principal, arquivos de projeto"
│       ├───pcb "Arquivos de fabricação do PCB"
│       ├───3D "Arquivos 3D dos componentes da placa"
│       └───electronics "Datasheets e documentação de projeto"
│   ├───front_panel_hw "Esquemático, Layout and Gerber do painel frontal (quando existente), arquivos de projeto"
│       ├───pcb "Arquivos de fabricação do PCB"
│       ├───3D "Arquivos 3D dos componentes da placa"
│       └───electronics "Datasheets e documentação de projeto"
│   └───peripherals_hw "Esquemático, Layout and Gerber dos periféricos, arquivos de projeto"
│       ├───pcb "Arquivos de fabricação do PCB"
│       ├───3D "Arquivos 3D dos componentes da placa"
│       └───electronics "Datasheets e documentação de projeto"
├───mechanics "Modelos mecânicos para uso com placas da plataforma"
│   ├───main_mech "Partes mecanicas para uso com a placa principal"
│   └───peripherals_mech "Partes mecanicas para uso com os periféricos"
└───software "Softwares de auxilio ao uso da plataforma"
│   └───low_code "Ferramenta de programação em blocos (low code), codigo fonte, biblioteca e documentos"
```

# [Hardware Innovation Technologies](http://www.hwit.com.br/)
