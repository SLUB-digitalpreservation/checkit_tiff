#!/bin/bash
# converts a PEG grammar in a PEG.js grammar
fgrep -v "set_parse_error" | sed -e "s/^#.*//g" -e "s/<-/=/g" -e "s/[<>]//g" -e "s/[\t ]#//g"
