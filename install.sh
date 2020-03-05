#!/bin/bash

if [[ -z "${NIBBLER_INSTALL_FOLDER}"  ]]; then
	export NIBBLER_INSTALL_FOLDER=${HOME}
fi

git submodule init && git submodule update
rm -rf ${NIBBLER_INSTALL_FOLDER}/.nibbler
rm -rf ./build && mkdir -v ./build && cd ./build
cmake -DCMAKE_INSTALL_PREFIX=${NIBBLER_INSTALL_FOLDER} \
-DCMAKE_BUILD_TYPE=Release ..
make install -j8
