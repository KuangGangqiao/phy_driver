ROOT=$(git rev-parse --show-toplevel)
CODE_PATH=source/phy_driver/jlsemi
SYS_HEAD_PATH=$(uname -r)
CHECKPATCH=./tool/checkpatch/checkpatch_5_4.pl
${CHECKPATCH} -f ${ROOT}/${CODE_PATH}/jlsemi* --no-tree
