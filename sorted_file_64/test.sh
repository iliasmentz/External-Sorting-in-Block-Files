#!/bin/bash
rm ./temp* ./sorted*;
make;
clear;
valgrind ./build/sr_main2;
