#!/bin/bash

USER=sudo
# for libreoffice
${USER} apt-get install libreoffice-java-common -y
${USER} apt-get install libreoffice-common -y
${USER} apt-get install libreoffice-writer -y
${USER} apt-get install openjdk-8-jre-headless -y
# for cross compile kernel
${USER} apt-get install libncurses5-dev libssl-dev -y
${USER} apt-get install build-essential openssl -y
${USER} apt-get install zlibc minizip -y
${USER} apt-get install libidn11-dev libidn11 -y
${USER} apt-get install bison -y
${USER} apt-get install flex -y
${USER} apt-get install libelf-dev -y
${USER} apt-get install libelf-devel -y
${USER} apt-get install elfutils-libelf-devel -y
${USER} apt-get install flex -y
${USER} apt-get install bison -y

