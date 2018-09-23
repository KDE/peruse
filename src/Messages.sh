#!/bin/sh
$XGETTEXT `find . -name \*.qml -or -name \*.cpp` -o $podir/peruse.pot
