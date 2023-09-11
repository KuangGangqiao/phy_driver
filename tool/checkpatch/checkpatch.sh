ROOT=$(git rev-parse --show-toplevel)
CODE_PATH=source/phy_driver/jlsemi
WARNING_TYPE=LINUX_VERSION_CODE,MACRO_WITH_FLOW_CONTROL,PREFER_PR_LEVEL
SYS_HEAD_PATH=$(uname -r)
CHECKPATCH=./tool/checkpatch/checkpatch_5_4.pl
${CHECKPATCH} -f ${ROOT}/${CODE_PATH}/jlsemi* --no-tree --ignore ${WARNING_TYPE} --show-types
