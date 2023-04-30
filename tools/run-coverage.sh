#!/bin/bash
set -e

cmake --preset coverage
cmake --build --preset coverage

