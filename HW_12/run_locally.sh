#!/bin/sh
BINARY_HOME=./bin
INPUT_HOME=./input
INPUT=${INPUT_HOME}/AB_NYC_2019.csv

echo "Calculating mean price:"
cat ${INPUT} | ${BINARY_HOME}/mapper | sort -k1 | ${BINARY_HOME}/reducer > output
cat output

echo "\nCalculating price variance:"
cat ${INPUT} | ${BINARY_HOME}/mapper_variance | sort -k1 | ${BINARY_HOME}/reducer_variance > output_variance
cat output_variance
