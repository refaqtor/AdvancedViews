# Advanced Views for QtQuick 
[![Build Status](https://travis-ci.org/filcuc/AdvancedViews.svg?branch=master)](https://travis-ci.org/filcuc/AdvancedViews)
[![Coverage Status](https://coveralls.io/repos/github/filcuc/AdvancedViews/badge.svg?branch=HEAD)](https://coveralls.io/github/filcuc/AdvancedViews?branch=HEAD)

This project aim to deliver a new set of View for helping
the creation of advanced control (ie. TableViews) for desktop
or other applications that spawn high number of elements.
Another aim of the project is to don't reinvent the wheel
by supporting QAbstractItemModel, QModelIndexes and QSelectionModels.
This should help interfacing with standard Qt models.

# License
This software is licensed under the LGPLv3 license

# Building
For building the project proceed in the usual Unix way
```
mkdir build && cd build
cmake ..
make -j8
```
If everything goes well you should have a QtQuick plugin in 
the `path/to/build/dir/AdvancedViews`

# Examples
For testing give the following commands from the build directory
```
cd /path/to/build/dir
./Examples/Example .
```
