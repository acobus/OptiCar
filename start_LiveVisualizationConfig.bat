@echo off

echo setting environment varibles...
set ADTF_DIR=C:/Program Files/ADTF/2.13.0
set CONFIG_PATH=%CD%

echo starting ADTF ...
CALL "%ADTF_DIR%/bin/adtf_devenv.exe" -project="%CONFIG_PATH%/config/LiveVisualizationConfig/LiveVisualizationConfig.prj"

echo closing ...