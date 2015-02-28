#!/bin/sh
#
# This script generates a RTSP video stream from a file.
# The file is played in loop by keeping the output opened so that stream can be
# used to fake a CCTV camera in ZoneMinder.

# SETTING SECTION
# The options commented out show the default values

# Network interface where the stream will be reachable
#IP_ADDR=0.0.0.0

# RTSP port where the stream will be reachable
#PORT=8554

# Path to the SDP file
#SDP_PATH=/media.sdp

# END OF SETTING SECTION
# PLEASE DO NOT MODIFIY LINES BELOW

if [ "$(id -u)" = "0" ]; then
    echo "This script is not supposed to be run as root, exit" 1>&2
    exit 1
fi

if [ -z "$2" ]; then
    echo "Usage: $0 {start|stop} file"
    exit 1
elif [ ! -f "$2" ]; then
    echo "File is not reachable, exit" 1>&2
    exit 1
fi

VLC_BIN=$(which vlc)
if [ -z "$VLC_BIN" ]; then
    echo "VLC is missing, exit" 1>&2
    exit 1
fi

[ -z "$IP_ADDR" ] && IP_ADDR=0.0.0.0
[ -z "$PORT" ] && PORT=8554
[ -z "$SDP_PATH" ] && SDP_PATH=/media.sdp
PID_FILE="$HOME/vlc_$(basename $2).pid"

case "$1" in
start)
    if [ -f "$PID_FILE" ]; then
        echo "VLC is already started, exit" 1>&2
        exit 1
    fi
    MRL="rtsp://$IP_ADDR:$PORT$SDP_PATH"
    $VLC_BIN --quiet --daemon --pidfile "$PID_FILE" --vout=dummy --aout=dummy \
             --intf=dummy $2 --loop --sout-keep \
             --sout='#gather:rtp{ttl=10,sdp='$MRL'}' 2> /dev/null
    echo "Streaming started at '$MRL'"
    ;;
stop)
    if [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
        ps -x | grep $PID | grep $VLC_BIN > /dev/null && kill $PID
    fi
    ;;
*)
    echo "Usage: $0 {start|stop} file"
    exit 1
esac

exit $?
