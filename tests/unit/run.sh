#!/bin/bash

PATH=$(git rev-parse --show-toplevel)
/usr/bin/python3.8 "$PATH/tests/unit/check_config.py" 2> /dev/null
/usr/bin/python3.8 -m unittest discover -s "$PATH/tests/unit" -p "test_*.py"