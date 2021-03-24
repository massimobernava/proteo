git pull
cd build
rm CMakeCache.txt
cmake -G"CodeLite - Unix Makefiles" -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl .. 
