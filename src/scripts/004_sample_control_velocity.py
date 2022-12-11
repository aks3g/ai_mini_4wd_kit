# Sample Name: 004_sample_control_velocity.py
# 概要: ミニ四駆の速度を一定に保ちます
#       タイヤの径はマシンに合わせて変更してください
#
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd

# ミニ四駆オブジェクトの生成
m = mini4wd.Machine()

# マシンのタイヤ直径を31㎜に変更
m.setTireSize(26)

# PID制御のゲインを設定
m.setGain(0,    0,          0,  0)
m.setGain(0.1,  0.0025,     0,  1)
m.setGain(0.1,  0.008,      0,  2)
m.setGain(0.1,  0.0135,     0,  3)
m.setGain(0.1,  0.019,      0,  4)
m.setGain(0.1,  0.0245,     0,  5)
m.setGain(0.109,0.03,       0,  6)
m.setGain(0.114,0.0355,     0,  7)
m.setGain(0.125,0.041, 0.0005,  8)
m.setGain(0.144,0.0465,0.0053,  9)
m.setGain(0.169,0.052, 0.0101, 10)
m.setGain(0.201,0.0575,0.0149, 11)
m.setGain(0.240,0.063, 0.0197, 12)
m.setGain(0.286,0.0685,0.0245, 13)
m.setGain(0.338,0.074, 0.0293, 14)
m.setGain(0.398,0.0795,0.0341, 15)
m.setGain(0.464,0.085, 0.0389, 16)
m.setGain(0.537,0.0905,0.0437, 17)
m.setGain(0.616,0.096, 0.0485, 18)
m.setGain(0.703,0.1015,0.0533, 19)
m.setGain(0.796,0.107, 0.0581, 20)
m.setGain(0.896,0.1125,0.0629, 21)
m.setGain(1.003,0.118, 0.0677, 22)
m.setGain(1.117,0.1235,0.0725, 23)
m.setGain(1.237,0.129, 0.0773, 24)
m.setGain(1.365,0.1345,0.0821, 25)

# PID制御でマシンの速度を10km/hに保ちます
m.setSpeed(12)

# 520回×1/52秒 = 10秒間動かす
cnt = 0
while cnt < 520 :
	# 1/52秒待ち
	m.grab()
	cnt = cnt + 1

	print (m)

# モータードライバの電力を切る
m.setDuty(0)
