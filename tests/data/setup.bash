#!/bin/bash
ILIAD_REF="https://www.gutenberg.org/cache/epub/2199/pg2199.txt"
ILIAD_FILE="iliad.txt"
wget ${ILIAD_REF} -O ${PWD}/${ILIAD_FILE}
