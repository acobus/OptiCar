set(FILTER_NAME aadc_calibrationXml)

# Specify the filter module target
adtf_add_filter(${FILTER_NAME}
                 stdafx.cpp
                 stdafx.h
                 cCalibrationXml.h
                 cCalibrationXml.cpp
                 cCubic.h
)

# set folder (only needed and available for MSVC)
adtf_set_folder(${FILTER_NAME} aadcbase/calibration) 

# Specify where it should be installed to
adtf_install_plugin(${FILTER_NAME} ${AADC_INSTALL_DIR})
