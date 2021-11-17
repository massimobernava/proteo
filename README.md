## Installation


## Server Linux

Install the required libraries:
```bash
sudo apt install build-essential git cmake libluajit-5.1-dev libssl-dev libcurl4-openssl-dev libjson-c-dev libmicrohttpd-dev libsqlite3-dev libzmq3-dev libavcodec-dev libavformat-dev libavutil-dev libswresample-dev libswscale-dev libopencv-dev 
```

Optional install nosql ejdb2 database:
```bash
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:adamansky/ejdb2 
sudo apt-get install ejdb2
```

Install Tensorflow Lite:
```bash
git clone https://github.com/tensorflow/tensorflow.git tensorflow_src
mkdir tflite_build
cd tflite_build
cmake ../tensorflow_src/tensorflow/lite/c -DTFLITE_ENABLE_GPU=ON
cmake --build . -j
```

Clone repository and use cmake:
```bash
git clone https://github.com/massimobernava/proteo.git
cd proteo/server
mkdir build
cd build 
cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl .. 
make
```

Update config.json:
```json
{  	"version":"0.1",
	"basedir":"/usr/local/etc/Proteo/",
	"baseurl":"YOUR_LOCAL_IP",
	"port":8888,"master":1,"ssl":0,
	"server_key":"01234567890123456789012345678901",
	"client_key":"01234567890123456789012345678901",
	"ssl_key":"",
	"ssl_cert":"",
	"admin_enabled":1,"plugins":["proteo","admin","edit","deepcrimson","deepindigo"],
	"servers":[]
}
```
Install it as a service:
```bash
chmod +x ./install.sh
sudo ./install.sh
```

## Client 

Choose, red pill and continue to compile, [blue pill](https://github.com/massimobernava/proteo/releases) and download one of the releases.

### Linux

Install the required libraries:
```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev
```
Use cmake:
```bash
cd ..client
mkdir build
cd build 
cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl .. 
make
```

### OSX

### iOs

### Android

### Windows


## Config

After installation, you need to configure the config.json file on the server and client.

[Detail](https://github.com/massimobernava/proteo/wiki/Config-file)


## API

A few examples. For [details](https://github.com/massimobernava/proteo/wiki/API)

Proteo is distributed under the MIT License.
