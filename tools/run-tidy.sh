#!/bin/bash
set -e

sed 's/\+\+23/\+\+2b/g' -i build/compile_commands.json
find src -name \*.cpp | xargs -P 8 -L 2 -r -- clang-tidy -p build/compile_commands.json 
