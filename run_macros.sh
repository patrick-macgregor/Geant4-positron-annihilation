#!/bin/bash

RUN_MACRO="run.mac"
APP="bin/positron_annihilation_13B_DH"
MACRO_DIR="macros/"

for macro in ${MACRO_DIR}*.mac
do
    $APP $macro $RUN_MACRO
done
