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

### Configuration

After installation, please make some adjustments in file `/etc/zm/plugins.d/openalpr.conf`.
Most of default values can be kept, but if you live in Europe, you can set the `country_code` setting to `eu` to improve reading of plates with EU format

All the next configuration steps are done through the web interface.

Firstly, the plugin loading has to be enabled in ZM options (please check the `LOAD_PLUGIN` setting in `config` tab).

Then, you can configure the plugin settings from each `Zone` configuration page.

![Plugin Conf Image](https://github.com/manupap1/libzoneminder-plugin-openalpr/blob/master/misc/plugin.png "Plugin Conf Image")

Available plugins are listed with a color code under the `Plugins` row:
- `Default color` - Plugin is not enabled for the zone
- `Green` - Plugin is enabled for the zone
- `Grey` - Plugin loading is disabled (please check `LOAD_PLUGIN` setting in `config` tab)
- `Orange` - Plugin is enabled for the zone but not active (configuration setting mismatch)
- `Red` - ZoneMinder failed to load the plugin object (software error)

Once a plugin object is loaded, the `Plugin` configuration page is accessed by clicking on the plugin name.

The first options are available for all plugins:
- `Enabled` - A yes/no select box to enable or disable the plugin
- `Require Native Detection` - A yes/no select box to specify if native detection is required before to process plugin analysis. This option allow to limit CPU usage by using the plugin for post processing after native detection. This option is recommended for libzm-plugin-openalpr as the plugin may use a lot of CPU ressources
- `Include Native Detection` - A yes/no select box to specify if native detection shall be included in alarm score and image overlay
- `Reinit. Native Detection` - A yes/no select box to specify if native detection shall be reinitialized after detection. ZoneMinder's native detection is performed by comparing the current image to a reference image. By design, the reference image is assigned when analysis is activated, and this image is not periodically refreshed. This operating method is not necessarily optimal because some plugins may require native detection only when motion is truly detected (current image different from the previous image). This option is recommended for libzm-plugin-openalpr. For example, without this option enabled, if a vehicle appears and parks in the camera field of view, the native detection will be be triggered as long as the vehicle is parked, and therefore the plugin analysis would be performed for an unnecessary period of time. With this option enabled, the plugin analysis stops when the vehicle stops.
- `Alarme Score` - A text box to enter the score provided by the plugin in case of license plate detection

The next options are specifics to this plugin and can be used to adjust the detection accuracy:
- `Minimum Confidence` - A text box to enter the minimum confidence level. All plates with a lower confidence level will be excluded.
- `Adaptive Confidence` - A yes/no select box to enable or disable the adaptive adjustment of the minimum confidence level. This option should no be used (experimental setting).
- `Minimum Number of Characters` - A text box to enter the minimum number of characters in a license plate. All plates with a lower number of detected characters will be excluded.
- `Maximum Number of Characters` - A text box to enter the maximum number of characters in a license plate. All plates with a greater number of detected characters will be excluded.
- `Maximum Exclusion Period` - A text box to enter the period of time (in seconds) in which already detected license plates will be excluded.

The configuration is saved to the database and applied when clicking on the `Save` button.

### Using
