#!/usr/bin/env bash

.PHONY: king
king:
	g++ -g -o king `find src -iname "*.cpp" -print`

.PHONY: clean

clean:
	rm king
