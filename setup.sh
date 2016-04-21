
make clean
/home/lab/usr/php/bin/phpize
#./configure --enable-kyureki --enable-debug --with-php-config=/usr/local/bin/php-config CC=g++ CXX=g++
./configure --enable-kyureki --with-php-config=/home/lab/usr/php/bin/php-config CC=g++ CXX=g++
make
sudo make install


