#!/bin/sh
valgrind --tool=memcheck --leak-check=full --show-reachable=yes --num-callers=20 --track-origins=yes --track-fds=yes ./oaisim -b1 -u1 -F -n100 2>leakage
