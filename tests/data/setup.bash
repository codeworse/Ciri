#!/bin/bash
set -eEx
ILIAD_REF="https://www.gutenberg.org/cache/epub/2199/pg2199.txt"
ILIAD_FILE=${PWD}/"iliad.data"
wget ${ILIAD_REF} -O ${ILIAD_FILE}
export ILIAD_FILE=${ILIAD_FILE}
