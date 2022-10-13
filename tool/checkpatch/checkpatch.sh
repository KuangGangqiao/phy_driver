ROOT=$(git rev-parse --show-toplevel)
CODE_PATH=source/phy_driver/jlsemi
SYS_HEAD_PATH=$(uname -r)
CHECKPATCH=/usr/src/linux-headers-${SYS_HEAD_PATH}/scripts/checkpatch.pl
${CHECKPATCH} -f ${ROOT}/${CODE_PATH}/jlsemi* --no-tree
