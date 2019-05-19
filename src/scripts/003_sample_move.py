# Sample Name: 003_sample_move.py
# 概要: ミニ四駆を前進/後進させます
# 
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd

# ミニ四駆オブジェクトの生成
m = mini4wd.Machine()

# ミニ四駆のモータードライバに前進/50%のパワーを与える
m.setDuty(int(255 * 0.5))

# 156回×1/52秒 = 3秒間動かす
cnt = 0
while cnt < 156 :
	# 1/52秒待ち
	m.grab()
	cnt = cnt + 1


# ミニ四駆のモータードライバに後進/50%のパワーを与える
m.setDuty(int(-255 * 0.5))
cnt = 0
while cnt < 156 :
	# 1/52秒待ち
	m.grab()
	cnt = cnt + 1

# モータードライバの電力を切る
m.setDuty(0)
