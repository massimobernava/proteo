systemctl stop proteo
cp ./build/proteo_server /usr/local/bin/

cp  /usr/local/etc/Proteo/config.json ./config.bkp || :

rm -rf /usr/local/etc/Proteo
mkdir /usr/local/etc/Proteo
cp -r ./lib /usr/local/etc/Proteo/
cp -r ./plugin /usr/local/etc/Proteo/
cp -r ./script /usr/local/etc/Proteo/
cp -r ./web /usr/local/etc/Proteo/
cp -r ./dl /usr/local/etc/Proteo/
cp favicon.ico /usr/local/etc/Proteo/
cp login.lua /usr/local/etc/Proteo/
cp route.lua /usr/local/etc/Proteo/
cp ticket.lua /usr/local/etc/Proteo/
cp debugger.lua /usr/local/etc/Proteo/
cp config.json /usr/local/etc/Proteo/

cp ./proteo.service /etc/systemd/system/
systemctl start proteo
