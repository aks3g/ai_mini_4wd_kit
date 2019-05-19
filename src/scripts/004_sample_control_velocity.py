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
m.setTireSize(31)

# PID制御のゲインを設定
m.setGainKp(0.1)
m.setGainKi(0.02)
m.setGainKd(0.02)


# PID制御でマシンの速度を10km/hに保ちます
m.setSpeed(10)

# 520回×1/52秒 = 10秒間動かす
cnt = 0
while cnt < 520 :
	# 1/52秒待ち
	m.grab()
	cnt = cnt + 1

# モータードライバの電力を切る
m.setDuty(0)
