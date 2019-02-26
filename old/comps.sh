#!/bin/bash
#!/bin/bash
# Edit it to your environment, dump it in a .sh at the project root and chmod u+x. #
#Run whenever you add/subtract components.

# don't forget to install bear and jq.
# for debian-style releases:
# sudo apt-get install bear
# sudo apt-get install jq

VSPROPERTIES=./.vscode/c_cpp_properties.json

make app-clean
bear make app
echo -e "\t\t{\"inc\" : [" > inc.json
grep -A1 '"-I",' compile_commands.json | grep "\"${HOME}/.*" | sort --unique >> inc.json
echo -e "\t\t\t\"${IDF_PATH}/xtensa-esp32-elf/lib/gcc/xtensa-esp32-elf/5.2.0/include\"," >> inc.json
echo -e "\t\t\t\"${IDF_PATH}/xtensa-esp32-elf/xtensa-esp32-elf/include\"," >> inc.json
echo -e "\t\t\t\"${IDF_PATH}/xtensa-esp32-elf/xtensa-esp32-elf/include/c++/5.2.0\"," >> inc.json
echo -e "\t\t\t\"${IDF_PATH}/xtensa-esp32-elf/xtensa-esp32-elf/include/c++/5.2.0/xtensa-esp32-elf\"" >> inc.json
echo -e "\t\t]}" >> inc.json

cp "$VSPROPERTIES" "$VSPROPERTIES.old"
jq --argfile inc inc.json '.configurations[].includePath = $inc.inc | .configurations[].browse.path = $inc.inc' "$VSPROPERTIES.old" > $VSPROPERTIES

#rm compile_commands.json
#rm inc.json
