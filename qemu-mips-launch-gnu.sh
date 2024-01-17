#!/bin/bash
qemu-mipsel-static -E LD_DEBUG=ALL -L /usr/mipsel-unknown-linux-gnu/sysroot  $@
