# Sample Name: 010_impulse_response.py
# 概要: Dutyを0から最大に設定し、その後0に戻した時のセンサの値を取得します
# 
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd

m = mini4wd.Machine()

cnt = 0;
while cnt < 52 * 2:
	# 1/52秒待ち/センサデータの更新
	m.grab()

	print(m);

	cnt = cnt + 1

#全力でモーターをまわす
m.setDuty(255)

cnt = 0
while cnt < 52 * 5:
	# 1/52秒待ち/センサデータの更新
	m.grab()

	print(m);

	cnt = cnt + 1;

#モーターを止めてその後の動きを確認する
m.setDuty(0)

cnt = 0;
while cnt < 52 * 5:
	# 1/52秒待ち/センサデータの更新
	m.grab()

	print(m);

	cnt = cnt + 1