#!/bin/bash
rm ./temp* ./sorted*;
make;
clear;
valgrind --leak-check=full ./build/sr_main2;
