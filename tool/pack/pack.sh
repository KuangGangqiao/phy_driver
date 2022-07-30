ROOT=$(git rev-parse --show-toplevel)
BUILDS_DIR=${ROOT}/build
REMOTE_NAME=origin

version=1.0.3
src_dir=${ROOT}
dest_dir=${BUILDS_DIR}
exclude_file=${ROOT}/tool/pack/pack_repo.exclude_files

git fetch ${REMOTE_NAME}

githash=$(git log -1 --format=%h)
repo_name=$(basename `git config --get remote.${REMOTE_NAME}.url` | sed  's/.git//')

mkdir -p ${dest_dir}

path_name=$(basename ${src_dir})
rm ${dest_dir}/${path_name}/* -rf

file_name=${repo_name}_${version}_${githash}
rsync -r --exclude-from="${exclude_file}" ${src_dir} ${dest_dir}
cp ${dest_dir}/${path_name} ${dest_dir}/${file_name} -R

# tar
cd ${dest_dir}
rm ./${file_name}/Makefile
rm ./${file_name}/README.md
mv ./${file_name}/source/phy_driver/jlsemi/* ./${file_name}/source/
rm ./${file_name}/source/phy_driver/ -rf
tar -czvf ${dest_dir}/${repo_name}_${version}_${githash}.tar.gz ${file_name}
rm ${file_name} ${repo_name} -rf

