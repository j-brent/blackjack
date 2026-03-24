#!/bin/bash
set -e

# Additional build tools
sudo apt-get update -q
sudo apt-get install -y -q ninja-build clang-format clang-tidy

# Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
~/emsdk/emsdk install latest
~/emsdk/emsdk activate latest
echo 'source ~/emsdk/emsdk_env.sh' >> ~/.bashrc

# Git hooks
git config core.hooksPath .githooks
git config blame.ignoreRevsFile .git-blame-ignore-revs
