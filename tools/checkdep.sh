#!/bin/bash

sdl-config --version >/dev/null || echo "Package 'SDL' not found"

pkg-config zziplib --exists || echo "Package 'zziplib' not found"

pkg-config zziplib --exists || echo "Package 'zziplib' not found"

pkg-config libconfuse --exists || echo "Package 'libconfuse' not found"

pkg-config gl --exists || echo "Package 'gl' not found"

pkg-config glu --exists || echo "Package 'glu' not found"

flex --version >/dev/null || echo "Tool 'flex' not found"

pkg-config lua5.1 --exists || echo "Package 'lua5.1' not found"
