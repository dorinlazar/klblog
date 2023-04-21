#!/bin/bash
pushd build
clang-tidy `find ../src -name \*.\[hc\]pp`

