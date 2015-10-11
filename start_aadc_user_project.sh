#!/bin/sh

export ADTF_DIR=/opt/adtf/2.13.1
export ADTF_PROJECT_PATH=./config/UserConfig/UserConfig.prj

if [ -d $ADTF_DIR ]; then
    echo "ADTF dir found."
else
    echo "ADTF dir not found in $ADTF_DIR. Check the path to ADTF."
    exit 1
fi


$ADTF_DIR/bin/adtf_devenv -project=$ADTF_PROJECT_PATH

#the qt filters and the video displays have to be deactivated before
#$ADTF_DIR/bin/adtf_runtime -project=$ADTF_PROJECT_PATH -minimized -run


