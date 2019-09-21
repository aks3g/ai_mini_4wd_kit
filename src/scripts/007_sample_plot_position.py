# Sample Name: 006_sample_logging.py
# 概要: 一定速度で走りながらセンサデータのログを残します
#       マシンのタイヤサイズは使用するマシンによって変更してください
#
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd
import math

TIRE_SIZE_mm = 31.0
INTERVAL_sec = 1.0/52.0

def convertTo_mm(rpm) :
	delta = (rpm / 60.0) * INTERVAL_sec * TIRE_SIZE_mm * 3.14
	return delta

def convertTo_degree(mdps) :
	if math.fabs(mdps) < 10000.0:
		return 0.0
	else:
		return (mdps / 1000.0) * INTERVAL_sec


print (" ")
# ミニ四駆オブジェクトの生成
m = mini4wd.Machine()

# マシンのタイヤ直径を31㎜に変更
m.setTireSize(TIRE_SIZE_mm)

# PID制御のゲインを設定
m.setGainKp(0.1)
m.setGainKi(0.02)
m.setGainKd(0.02)

m.led(1)

# 車軸が少し回転するのを待つ
m.waitIntTrig()

m.led(2)


# PID制御でマシンの速度を10km/hに保ちます
m.setSpeed(10)

cnt = 0
x = 0.0
y = 0.0
yaw_degree = 0.0
delta_mm = 0.0
direction = 0.0
while cnt < 520 :
	# 1/52秒待ち/センサデータの更新
	m.grab()
	cnt = cnt + 1

	yaw_degree = convertTo_degree(m.getYaw())
	delta_mm = convertTo_mm(m.getRpm())

	direction = direction + yaw_degree

	x = x + delta_mm * math.cos(math.radians(direction))
	y = y + delta_mm * math.sin(math.radians(direction))

	print (str(x) + "\t" + str(y))

# モータードライバの電力を切る
m.setDuty(0)
m.led(15)

