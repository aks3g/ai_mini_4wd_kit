# Sample Name: 014_self_calibration.py
# 概要: タコメータの閾値を自己計測してアップデートします。
#
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd
import math
import gc


m = mini4wd.Machine()

m.led(1)

threshold_mv = m.calibrateTachometer();
m.setTachometerThreshold(threshold_mv);

print (threshold_mv);

m.led(2)
