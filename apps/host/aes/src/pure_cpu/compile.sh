#!/bin/bash
#g++ grep.cpp -O3 -o grep -std=c++11 -mcmodel=medium
g++ main.cpp AESfunctions.h AEStables.h AESfunctions.cpp -O3 -o main_aes -std=c++11 -mcmodel=medium
