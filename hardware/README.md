
# HLab Hardware

Essa pasta apresenta detalhes sobre todos os relativos a plataforma HLAB e trabalhos anteriores no suporte da bitdoglab.

Basicamente com relação ao bitdoglab, suportamos a evolução da placa a partir da versão 5.3 até a versão 6.3 (embarcatech) e 6.4. Os arquivos esquemáticos da placa principal se encontram na pasta main_hw, nos seguintes links:

* bitdoglab 5.3 [esquemático](./main_hw/bitdoglab_v5_3_smd.pdf) e [código fonte (Kicad)](./main_hw/bitdoglab_v5_3_smd/)
* bitdoglab 6.0 [esquemático](./main_hw/bitdoglab_v6_0_smd.pdf)
* bitdoglab 6.2 [esquemático](./main_hw/bitdoglab_v6_2_smd.pdf)
* bitdoglab 6.3 - versão embarcatech [esquemático](./main_hw/bitdoglab_v6_3_smd.pdf)
* bitdoglab 6.4 [esquemático](./main_hw/bitdoglab_v6_4_main.pdf)

Os arquivos esquemáticos da placa do painel frontal se encontra na pasta front_panel_hw e no seguinte [link](./front_panel_hw/bitdoglab_v6_4_painel.pdf).

Os periféricos são compostos pelos seguintes componentes e seus respectivos links para os esquemáticos:

* Kit Básico de Periféricos (12 itens)
  * [Esquemático](./peripherals_hw/Adaptadora_R1.1.pdf) Periferico adaptador de sensores/atuadores
      * [Sensor i2c] Acelerômetro
      * [Sensor i2c] Oxímetro e batimentos cardiácos
      * [Sensor i2c] Medição de distância a Laser
      * [Sensor i2c] Temperatura e pressão
      * [Sensor i2c] Luminosidade
      * [Sensor i2c] Umidade e temperatura
      * [Sensor i2c] Cor RGB
      * [Atuador PWM] Servo Motor
  * [Esquemático](./peripherals_hw/bitdoglab-dvi-hdmi_v1r1.pdf) Periférico HDMI-DVI
  * [Esquemático](./peripherals_hw/I2C_Extender_R1.1.pdf) Periférico de extensão de conexões para sensores (i2c)
  * [Esquemático](./peripherals_hw/PRJ_Teclado.pdf) Periférico teclado matricial (GPIO)
  * [Esquemático](./peripherals_hw/LS_R1.1.pdf) Periférico de armazenamento de dados SDCARD (SPI)
* Kit Avançado de Periféricos (19 itens) = Kit Básico (12) + itens abaixo (7)
  * [Esquemático](./peripherals_hw/ADC_DAC_R1.1.pdf) Periferico conversor analógico digital ADC / digital analógico DAC / GPIO com oito portas (SPI)
  * [Esquemático](./peripherals_hw/CAMERA.pdf) Periférico câmera de 2M Pixels (interface paralela)
  * [Esquemático](./peripherals_hw/LCD_Rev1.2.pdf) Periférico display LCD 320x240 pixels (SPI)
  * [Esquemático](./peripherals_hw/LS_R1.1.pdf) Periférico comunicação radio de longa distância LoRA (SPI)
  * [Esquemático](./peripherals_hw/Adaptadora_R1.1.pdf) Periférico GPS (UART)
  * [Esquemático](./peripherals_hw/DEBUGGER_R1.1.pdf) Periférico Pi Pico debug probe (SWD)
  * [Esquemático](./peripherals_hw/IDC_Extender_R1.1.pdf) Periférico de extensão de conexões (SPI)
* Gerenciador de bateria bq25622 (i2c, embarcado na placa principal)

# [Hardware Innovation Technologies](http://www.hwit.com.br/)
