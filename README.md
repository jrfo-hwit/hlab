
[![License: CERN-OHL-S v2.0](https://img.shields.io/badge/License-CERN--OHL--S%20v2.0-blue.svg)](https://cern.ch/cern-ohl)

# HLab 

Is an educational platform that was initially based on BitDogLab (https://github.com/Fruett/BitDogLab), a project entirely open, allowing it to be freely copied, manufactured, assembled, and improved by users.

This repository contain all hardware informations improved by HwIT from bitdoglab 5.3, 6.0, 6.2, 6.3 (Embarcatech program version) up to 6.4 (last BitDogLab evolution by HwIT), that will be replaced soon by HLab 1.0.

## License
All BitDogLab versions are licensed under the CERN Open Hardware Licence Version 2 - Strongly Reciprocal (CERN-OHL-S).
For more details, see the `LICENSE` file or visit [https://cern.ch/cern-ohl](https://cern.ch/cern-ohl).

## Github structure
```bash
├───firmware "The following files are firmware that should work on BitDogLab"
│   ├───micropython "Interpreter firmware and micropython firmware examples"
│       ├───interpreter "..."
│       └───examples "firmware examples each including src & docs per example"
│   └───c_cpp "supported on the Raspberry pi pico W (wireless version)"
│       ├───sdk "symbolic link to processor SDK"
│       └───examples "firmware examples each including src & docs per example"
├───hardware "The following files are Hardwares informations"
│   ├───main_hw "Schematic, layout and gerber files of DIY version"
│       ├───schematics "..."
│       ├───pcb "..."
│       ├───bom "..."
│       └───electronics "components &| design docs"
│   ├───connection_hw "Schematic, layout and gerber files of SMD version"
│   └───peripherals_hw "Schematic, layout and gerber files of SMD version"
└───software "thirdy party libs for softwares"
│   └───no_code "no code tool based on blockly, including src, libs & docs"
└───mechanics "thirdy party libs for softwares"
│   └───peripherals_mech "Mechanical parts to assist hardware"
└───education "thirdy party libs for softwares"
│   ├───teacher "Training material for teacher"
│   ├───instructor "Training material for instructors"
│   └───student "Training material for student"
```
PAREI AQUI [JRFO]

Getting started doc (micropython e c/c++)
    building
    running
    learning

#### Sponsor: [Hardware Innovation Technologies (Paulinia/SP/Brazil)](http://www.hwit.com.br/)
