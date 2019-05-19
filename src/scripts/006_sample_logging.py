# Sample Name: 006_sample_logging.py
# 概要: 一定速度で走りながらセンサデータのログを残します
#       マシンのタイヤサイズは使用するマシンによって変更してください
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

m.led(1)

# 車軸が少し回転するのを待つ
m.waitIntTrig()

m.led(2)


# PID制御でマシンの速度を15km/hに保ちます
m.setSpeed(5)

# 1040回×1/52秒 = 20秒間動かす
cnt = 0
while cnt < 1040 :
	# 1/52秒待ち/センサデータの更新
	m.grab()
	cnt = cnt + 1

	print(m)

# モータードライバの電力を切る
m.setDuty(0)
