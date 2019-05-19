# Sample Name: 005_sample_internal_trigger.py
# 概要: 内部トリガー（車軸の回転を検知する）が入るとLEDが点灯します
#
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd

# ミニ四駆オブジェクトの生成
m = mini4wd.Machine()

m.led(0x1)

# 車軸が少し回転するのを待つ
m.waitIntTrig()

m.led(0x3)

# 車軸が少し回転するのを待つ
m.waitIntTrig()

m.led(0x7)

# 車軸が少し回転するのを待つ
m.waitIntTrig()

m.led(0xf)


# 104回×1/52秒 = 2秒間動かす
cnt = 0
while cnt < 104 :
	# 1/52秒待ち
	m.grab()
	cnt = cnt + 1
