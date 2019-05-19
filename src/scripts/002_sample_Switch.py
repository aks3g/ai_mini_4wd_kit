# Sample Name: 002_sample_Switch.py
# 概要: Switchの状態に合わせてLEDを点灯させます
# 
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd

# ミニ四駆オブジェクトの生成
m = mini4wd.Machine()


# 520回×1/52秒 = 10秒間動かす
cnt = 0
while cnt < 520 :
	# 1/52秒待ち
	m.grab()
	cnt = cnt + 1

	# SW0が押されていればLED0を、押されていなければLED1を点灯
	led_pattern = 0
	if True == m.sw(0):
		led_pattern = led_pattern + 1
	else :
		led_pattern = led_pattern + 2

	# SW1が押されていればLED2を、押されていなければLED3を点灯
	if True == m.sw(1):
		led_pattern = led_pattern + 4
	else :
		led_pattern = led_pattern + 8

	m.led(led_pattern)
