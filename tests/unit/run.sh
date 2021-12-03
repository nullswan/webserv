#!/bin/bash

PATH=$(git rev-parse --show-toplevel)
/usr/bin/python3.8 "$PATH/tests/unit/check_config.py"