#!/bin/sh

# at root of repo: cd .git/hooks && ln -s ../../ci/pre-commit.sh pre-commit
./ci/clang-format.sh HEAD develop
