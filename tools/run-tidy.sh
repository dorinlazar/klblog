#!/bin/bash
sed 's/\+\+23/\+\+2b/g' -i build/compile_commands.json
clang-tidy -p build/compile_commands.json `find src -name \*.\[hc\]pp`

