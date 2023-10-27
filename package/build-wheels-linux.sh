#!/bin/bash
PYTHON_VERSIONS=("cp38-cp38")

# ------ Build pycolmap wheel ------
WHEEL_DIR="wheels/"
python3.8 -m pip wheel --no-deps -w ${WHEEL_DIR} .
