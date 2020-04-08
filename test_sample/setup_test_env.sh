#!/bin/bash

BIN_DIR=bin
REPORT_DIR=report
LOG_DIR=report/log

export LD_LIBRARY_PATH=${BIN_DIR}:$LD_LIBRARY_PATH

if [ -d ${REPORT_DIR} ]; then
    rm -rf ${REPORT_DIR}
fi
mkdir -p ${LOG_DIR}
