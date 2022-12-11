# Sample Name: 011_capture_map.py
# 概要: コースを3周回るか、120m走るまでログをとります
#       ただし、IMUの性能的に3周走ったことの検知に不安定性が残っています。
# 
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd
import math
import gc

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
UNIT_mm = 100;
TIRE_SIZE_mm = 26
INTERVAL_sec = 1.0/52.0
COEFF_DELTA  = (1.0 / 60.0) * INTERVAL_sec * TIRE_SIZE_mm * 3.14
COEFF_DEGREE = (1.0 / 1000.0) * INTERVAL_sec

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
def convertTo_mm(rpm) :
	delta = rpm * COEFF_DELTA
	return delta

def convertTo_degree(mdps) :
	if math.fabs(mdps) < 10000.0:
		return 0.0
	else:
		return mdps * COEFF_DEGREE




#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
m = mini4wd.Machine()

# マシンのタイヤ直径を31㎜に変更
m.setTireSize(TIRE_SIZE_mm)

# PID制御のゲインを設定
m.setGainKp(0.1)
m.setGainKi(0.02)
m.setGainKd(0.02)

delta_mm = 0;
yaw_degree = 0;
x = 0;
x_p = 0;
y = 0;
direction = 0;
lap = 0;
cnt = 0;
milage = 0

m.led(1)

# 車軸が少し回転するのを待つ
m.waitIntTrig()

m.led(2)

# PID制御でマシンの速度を10km/hに保ちます
m.setSpeed(7)

while m.checkExtTrig() == False and lap < 3:
	# 1/52秒待ち/センサデータの更新
	m.grab()

	delta_mm = convertTo_mm(m.getRpm());
	yaw_degree = convertTo_degree(m.getYaw());

	direction = direction + yaw_degree

	x_p = x;
	x = x + delta_mm * math.cos(math.radians(direction))
	y = y + delta_mm * math.sin(math.radians(direction))

	print(m)

	milage = milage + delta_mm;

	if x_p < 0 and x >= 0 :
		lap = lap + 1;

	cnt = cnt + 1;

m.setDuty(0)
m.led(3)

