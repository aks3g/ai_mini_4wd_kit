# Sample Name: 001_sample_LED.py
# 概要: LEDをあらかじめ決めたパタン順に点灯させます
#       点灯時間の同期処理には、grab()メソッドを用いています
# 
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd

# ミニ四駆オブジェクトの生成
m = mini4wd.Machine()


# LEDの点灯パタンを生成
led_pattern = [0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x08, 0x0C, 0x04, 0x06, 0x02, 0x03, 0x01]
led_index = 0

# 52回×2×1/52秒 = 2秒間動かす
cnt = 0
while cnt < 52 :
	# 1/52秒待ち
	m.grab()
	# 1/52秒待ち
	m.grab()

	cnt = cnt + 1

	led_index = led_index + 1
	if led_index >= len(led_pattern) :
		led_index = 0

	# 点灯パタンを入力
	m.led(led_pattern[led_index])

