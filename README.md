libzm-plugin-openalpr
=====================

## Overview

libzm-plugin-openalpr is a free, open source Licence Plate Recognition plugin for the ZoneMinder CCTV sofware (https://github.com/ZoneMinder/ZoneMinder).
It is based on openalpr library (https://github.com/openalpr/openalpr).
The recognized license plates are added to Zoneminder's event notes.

## Requirements

libzm-plugin-openalpr requires:
- A ZoneMinder installation with the plugin framework
- The OpenALPR library

### Installation

#### Debian

Currently the plugin framework is not supported in upstream ZoneMinder.
ZoneMinder with plugin support can be built from the repository https://github.com/manupap1/ZoneMinder

Installation of the building environment
```bash
sudo apt-get install build-essential git devscripts
```
Compilation of ZoneMinder
```bash
git clone https://github.com/manupap1/ZoneMinder.git
cd ZoneMinder/
git fetch origin plugin_support:plugin_support
git checkout plugin_support
cp -R distros/debian8 ./debian
fakeroot debian/rules get-orig-source
sudo apt-get install debhelper po-debconf dh-systemd autoconf automake quilt libphp-serialization-perl libgnutls28-dev libmysqlclient-dev libdbd-mysql-perl libdate-manip-perl libwww-perl libjpeg62-turbo-dev libpcre3-dev libavcodec-dev libavformat-dev libswscale-dev libavutil-dev libv4l-dev libbz2-dev libtool libsys-mmap-perl libnetpbm10-dev libavdevice-dev libdevice-serialport-perl libarchive-zip-perl libmime-lite-perl dh-autoreconf libvlccore-dev libvlc-dev libcurl4-openssl-dev libgcrypt20-dev libpolkit-gobject-1-dev libdbi-perl libnet-sftp-foreign-perl libexpect-perl libmime-tools-perl pkg-config
debuild
```
Installation of ZoneMinder plugin development library
```bash
cd ..
sudo dpkg -i libzoneminder-plugin-dev_1.28.1-0.1_amd64.deb
```
Compilation of OpenALPR
```bash
git clone https://github.com/openalpr/openalpr.git
cd openalpr
cp -R distros/debian ./
fakeroot debian/rules get-orig-source
sudo apt-get install debhelper git cmake quilt libopencv-dev libtesseract-dev libtesseract-dev libleptonica-dev liblog4cplus-dev libcurl3-dev uuid-dev
debuild
```
Installation of OpenALPR development library
```bash
cd ..
sudo dpkg -i libopenalpr-dev_2.0.0-0.2_amd64.deb libopenalpr2_2.0.0-0.2_amd64.deb libopenalpr-data_2.0.0-0.2_all.deb
```
Compilation of libzm-plugin-openalpr
```bash
git clone https://github.com/manupap1/libzm-plugin-openalpr.git
cd libzm-plugin-openalpr
cp -R distros/debian ./
fakeroot debian/rules get-orig-source
sudo apt-get install debhelper quilt dh-autoreconf pkg-config libboost-program-options1.55-dev libopencv-dev libopencv-core-dev libopencv-imgproc-dev
debuild
```
Installation of ZoneMinder, OpenALPR and libzm-plugin-openalpr
```bash
cd ..
sudo apt-get install libavcodec56 libavdevice55 libavformat56 libavutil54 libbz2-1.0 libc6 libcurl3 libgcc1 libgcrypt20 libgnutls-deb0-28 libgnutls-openssl27 libjpeg62-turbo libmysqlclient18 libpcre3 libstdc++6 libswscale3 libvlc5 zlib1g debconf init-system-helpers perl libdevice-serialport-perl libimage-info-perl libjson-any-perl libsys-mmap-perl liburi-encode-perl libwww-perl libarchive-tar-perl libarchive-zip-perl libdate-manip-perl libdbi-perl libmodule-load-conditional-perl libmime-lite-perl libmime-tools-perl libnet-sftp-foreign-perl libphp-serialization-perl libav-tools rsyslog netpbm zip policykit-1 apache2 libapache2-mod-php5 php5 php5-mysql mysql-server
sudo dpkg -i libzoneminder-perl_1.28.1-0.1_all.deb zoneminder-database_1.28.1-0.1_all.deb zoneminder-core_1.28.1-0.1_amd64.deb zoneminder-ui-base_1.28.1-0.1_amd64.deb zoneminder-ui-classic_1.28.1-0.1_all.deb libopenalpr2_2.0.0-0.2_amd64.deb libopenalpr-data_2.0.0-0.2_all.deb libzoneminder-plugin-openalpr_1.0.0-1_amd64.deb
```
