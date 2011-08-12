#!/bin/bash


find orig -mindepth 2 -name "*.obj" -or -name "*.png" -not -name "*uv*" -not -name "*render*" | xargs -I {} cp {} data/cr/animmodels/
