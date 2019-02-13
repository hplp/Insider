#!/bin/bash
g++ data_generator.cpp  -std=c++11 -O3 -fopenmp -o data_gen

if [ -e aes_input.txt ]
then
    rm aes_input.txt
else
    echo "no aes_input.txt"
fi