#!/bin/bash
rm ./temp*;
make;
clear;
valgrind ./build/sr_main2;
