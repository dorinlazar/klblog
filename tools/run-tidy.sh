#!/bin/bash
pushd build
sed 's/c\+\+23/c\+\+2b/g' -i compile_commands.json
popd
clang-tidy -p build/compile_commands.json `find src -name \*.\[hc\]pp`

