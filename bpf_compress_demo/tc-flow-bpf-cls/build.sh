#!/bin/bash
clang -std=gnu89 -g -c -O2 -target bpf -c classifier.c -o classifier.o
