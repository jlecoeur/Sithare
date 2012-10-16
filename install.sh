# installation des paquets necessaires
sudo apt-get install libmpg123-dev libmpg123-0 libcurl3 libcurl3-dev libid3-3.8.3c2a libid3-dev libcv2.1 libcv-dev libcvaux2.1 libcvaux-dev libhighgui2.1 libhighgui-dev libtool automake autoconf

# installation des librairies
# mpg123
cd lib/mpg123/
./configure
make
sudo make install
make clean
cd ../../
# openSMILE
cd lib/opensmile/
sh autogen.sh
sh autogen.sh
./configure
make
make
sudo make install
make clean
cd ../../

# allow usage of shared libraries
sudo ldconfig

# installation du programme sithare
cd src/
make
make clean
cd ../



