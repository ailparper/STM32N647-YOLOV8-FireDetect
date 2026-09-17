#!/bin/bash

ProjectDir="$(dirname "$(readlink -f "$0")")"
STM32SigningTool="$1/STM32_SigningTool_CLI"

for j in $(find "$ProjectDir" -name "*_FSBL.bin"); do
    ProjectOut="$j"
done
ProjectConv="${ProjectOut%.*}-trusted.${ProjectOut##*.}"

rm -f "$ProjectConv"
"$STM32SigningTool" -bin "$ProjectOut" -nk -of 0x80000000 -t fsbl -o "$ProjectConv" -hv 2.3 -dump "$ProjectConv"
arm-none-eabi-objcopy -I binary "$ProjectConv" --change-addresses 0x70000000 -O ihex "$ProjectDir"/../../Binary/fsbl.hex