#!/bin/sh

#valgrind --tool=memcheck --leak-check=yes -v ./rfc2047 -e "�й�����"
valgrind --tool=memcheck --leak-check=yes -v ./rfc2047
