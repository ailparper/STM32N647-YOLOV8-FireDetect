#!/bin/bash

ProjectDir="$(dirname "$(readlink -f "$0")")"

for j in $(find "$ProjectDir" -name "*_Appli.bin"); do
    ProjectOut="$j"
done

arm-none-eabi-objcopy -I binary "$ProjectOut" --change-addresses 0x70100400 -O ihex "$ProjectDir"/../../Binary/appli.hex