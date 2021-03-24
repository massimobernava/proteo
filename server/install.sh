systemctl stop proteo
cp ./build/proteo_server /usr/local/bin/
rm -r /usr/local/etc/proteo
mkdir /usr/local/etc/proteo
cp -r ./lib /usr/local/etc/proteo/
cp -r ./plugin /usr/local/etc/proteo/
cp -r ./script /usr/local/etc/proteo/
cp -r ./web /usr/local/etc/proteo/
cp favicon.ico /usr/local/etc/proteo/
cp login.lua /usr/local/etc/proteo/
cp route.lua /usr/local/etc/proteo/
cp ./proteo.service /etc/systemd/system/
systemctl start proteo
