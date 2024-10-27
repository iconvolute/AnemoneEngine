#!/bin/sh
find ./Editor ./Engine ./Samples ./Tests ./Tools -iname *.cxx -o -iname *.hxx -o -iname *.inl | xargs clang-format -i
