#!/bin/bash
g++ AES.cpp AESfunctions.h AEStables.h AESfunctions.cpp -O3 -o aes_main -std=c++11 -mcmodel=medium
