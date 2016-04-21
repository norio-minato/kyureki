# Kyureki (PHP extention) php7 correspondence.

This php extension , to convert the Gregorian calendar in the Japanese calendar.

## INSTALL

phpize
./configure --enable-kyureki7 --with-php-config=/usr/local/bin/php-config CC=g++ CXX=g++
make
make install

## USAGE

### Convert to Kyureki (Japanese old calendar).

$japanese_old_date = get_kyureki(1919, 11, 17);
print_r($japanese_old_date); // return as array

$japanese_old_date[0] : year.
$japanese_old_date[1] : month.
$japanese_old_date[2] : mday.
$japanese_old_date[0] : day of week.

### How to get Rokuyou.

$rokuyou = get_rokuyou(2016, 1, 6);
echo $rokuyou;

0: Taian (大安)
1: Shakkō (赤口)
2: Sensho (先勝)
3: Tomobiki (友引)
4: Senbu (先負)
5: Butsumetsu (仏滅)

## What is Rokuyo

If you were ever wondering why there's lots of weddings in your shrine next door one day and none the other, here's why - there are six labels that indicate how auspicious a given day in the calendar is and people will make decisions based on that.

### 先勝 Sensho

Good luck in the morning, bad luck from 2 to 6pm.

### 友引 Tomobiki

Good luck in the morning, bad luck at noon and very lucky evenings. As Tomobiki translates as "pulls-friends", no funerals are held this day and crematoriums and funeral parlors are closed this day. It is believed that a funeral on a Tomobiki-day might drag friends towards the death, i.e. make them die. On the other hand, some interpret "tomobiki" as "sharing something with friends" and have their wedding on this day - in order to share their happiness with others.

### 先負 Senbu

Bad luck before noon, good luck after noon.

### 仏滅 Butsumetsu

Translates as "Buddha's death" and is considered the most unlucky day of the six Rokuyo-days. Few people get married on a butsumetsu and some wedding parlors even offer discounts for couples getting married on these days.

### 大安 Taian

The most lucky day of all. A good day for weddings, moving house and being discharged from hospitals - shrines and wedding parlors are usually packed and you have a good chance to take pictures of a wedding.

###赤口 Shakkō

Only the time between 11am and 1pm is considered lucky.

