git pull
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl .. 
make
cp ./proteo_server ../
cd ..
