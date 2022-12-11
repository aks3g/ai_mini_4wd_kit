# Sample Name: 013_running.py
# 概要: 自己位置を推定しながらマシンを走らせます。自己位置に応じた速度設定
#       を、arr_policyに基づいて行います。
#
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd
import math
import gc

arr_map = [8,8,8,8,8,8,8,8,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,8,8,1,1,8,3,8,8,2,2,8,2,2,2,8,8,8,2,2,2,2,8,2,2,8,8,8,2,2,2,8,6,7,8,2,8,1,8,8,8,8,8,8,8,8,8,8,6,8,8,8,8,8,8,1,1,2,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,8,8,8,8,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,1,1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,8,5,7,5,8,3,8,8,8,8,8,8,8,5,5,8,8,4,8,8,8,8,8,8,8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,8,8,8,8,8,8,8,8,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,8,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,8,7,7,8,3,8,8,8,8,8,8,8,8,7,8,8,3,8,8,8,8,8,8,8,8,8,2,8,8,8,8,8,2,8,8,8,2,8,8,2,8,8,2,8,8,2,2,8,8,8]


arr_policy=[13.1512,12.041,11.4705,11.3134,11.3299,11.0622,10.1072,8.75991,8.42603,7.92785,7.01454,6.51737,10.0,7.27033,8.88456,6.81572,6.81198,7.00606,9.14608,11.9301,11.8662,11.782,10.7696,10.7733,10.8195,10.8212,10.9716,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,16.3116,16.291,16.4687,16.5789,16.6774,16.6106,16.6561,15.433,14.7457,14.299,14.0769,13.2787,12.0783,11.8208,11.6109,11.1398,11.0057,11.2618,10.4263,10.2364,9.22015,11.0107,11.9365,12.9492,13.6807,14.2834,14.777,15.4074,15.6126,15.5648,16.159,16.1404,15.6077,16.2219,16.1576,16.2994,16.372,16.2002,16.1545,16.313,16.5298,16.5848,16.4338,16.4561,16.4764,16.6589,16.6838,16.6823,16.6876,16.7035,15.9818,16.0679,16.1303,16.1241,16.3121,16.203,15.9388,15.7585,14.7914,14.9026,14.9379,14.9563,16.097,16.1267,16.1162,16.1085,16.2975,16.357,16.5798,16.8272,16.8451,16.8577,16.8466,16.9828,16.9725,16.9761,16.7654,16.8915,16.8464,16.7183,16.7701,16.7474,16.7524,16.6007,16.5997,16.2576,16.2226,16.2518,15.9416,15.4127,15.2844,15.278,15.1655,14.6379,13.3369,12.9531,11.7106,11.7535,11.0917,11.1372,11.1914,11.2906,10.7957,10.9353,10.9895,11.3772,12.8955,14.0216,14.2051,14.1782,14.1432,11.7631,10.7724,15.1268,15.5961,15.6599,15.6517,15.678,15.6637,15.6525,15.6194,15.6066,13.5595,12.4771,12.4391,12.9424,12.9624,12.9849,12.8668,11.8531,12.6136,11.6342,13.5783,13.6063,14.5966,15.3313,15.3605,15.4161,14.5064,14.5349,14.6654,14.6916,14.908,14.436,12.9299,12.1025,11.7186,11.146,9.92018,8.81507,8.14083,8.01849,8.81121,8.36522,8.68694,7.9773,8.69058,9.29508,9.0512,9.39079,10.7446,13.3823,14.2367,14.5912,14.3433,13.7937,12.7139,15.5128,11.9934,11.9998,11.9958,11.998,10.9998,10.9997,10.997,10.999,10.9978,10.9984,10.9985,11.0,10.9987,10.9986,10.9987,10.0,10.0,10.0,10.9994,10.0,10.0,10.0,10.0,10.8982,10.8695,10.8788,10.8798,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,]

UNIT_mm = 100;
TIRE_SIZE_mm = 26
INTERVAL_sec = 1.0/52.0
COEFF_DELTA  = (1.0 / 60.0) * INTERVAL_sec * TIRE_SIZE_mm * 3.14
COEFF_DEGREE = (1.0 / 1000.0) * INTERVAL_sec

CENTER_TO_OUTER_LEFT  = 600
INNER_TO_CENTER_LEFT  = 490
INNER_TO_CENTER_RIGHT = -480
CENTER_TO_OUTER_RIGHT = -590


FEATURE_UPDOWN = 1;
FEATURE_OUT_LEFT = 2; 
FEATURE_CENTER_LEFT = 3;
FEATURE_IN_LEFT = 4;
FEATURE_OUT_RIGHT = 5;
FEATURE_CENTER_RIGHT = 6;
FEATURE_IN_RIGHT = 7;
FEATURE_STRAIGHT = 8

def feature_value(delta_mm, yaw_degree, pitch_degree):

	if yaw_degree != 0:
		radius = (delta_mm / (yaw_degree/360)) / (2 * 3.1415);
	else:
		radius = 0;

	if pitch_degree > 3.0 :
		feature = FEATURE_UPDOWN;
	elif CENTER_TO_OUTER_LEFT <= radius and radius < 800.0:
		feature = FEATURE_OUT_LEFT
	elif INNER_TO_CENTER_LEFT <= radius and radius < CENTER_TO_OUTER_LEFT:
		feature = FEATURE_CENTER_LEFT;
	elif 300.0 <= radius and radius < INNER_TO_CENTER_LEFT:
		feature = FEATURE_IN_LEFT;
	elif INNER_TO_CENTER_RIGHT <= radius and radius < -300.0:
		feature = FEATURE_IN_RIGHT;
	elif CENTER_TO_OUTER_RIGHT <= radius and radius < INNER_TO_CENTER_RIGHT:
		feature = FEATURE_CENTER_RIGHT;
	elif -800 <= radius and radius < CENTER_TO_OUTER_RIGHT:
		feature = FEATURE_OUT_RIGHT;
	else :
		feature = FEATURE_STRAIGHT;

	return feature;

def evaluation_value(az, roll):
	eva = 0;

#	if az > 3000 :
#	if az < 0 :
#		eva = eva - 2.5;

	if math.fabs(roll) > 30000:
#		eva = eva - 5.0; log018まで
		eva = eva - 3.0;

	return eva;


def convertTo_degree(mdps) :
	if math.fabs(mdps) < 10000.0:
		return 0.0
	else:
		return mdps * COEFF_DEGREE


# ミニ四駆制御器を作る
m = mini4wd.Machine();

# マシンのタイヤ直径を31㎜に変更
m.setTireSize(TIRE_SIZE_mm)

# PID制御のゲインを設定
m.setGainKp(0.2)
m.setGainKi(0.04)
m.setGainKd(0.02)


# 位置推定器を作る
spe = mini4wd.SelfPositionEstimater(2000, 100);

# 特徴量マップを読み込む
for i in range(len(arr_map)):
	spe.push_map_item(arr_map[i]);

spe.set_weight(0.001)
spe.prepare()

# 機械学習器を生成
# 引数
# 1. マップのサイズ
# 2. 報酬が伝搬数する範囲
# 3. 学習1回ごとの全体の速度の上昇値
# 4. 時定数（秒）
# 5. マップの1ますごとの距離(mm)
# 6. 初期速度(km/h)
ml = mini4wd.MachineLearning(len(arr_map), int(1000/UNIT_mm), 0, 0.2, UNIT_mm, 10.0);
if len(arr_policy) != 0 :
	for i in range(len(arr_policy)) :
		ml.set_policy(i, arr_policy[i]);

f = 0;
pos = 0;
delta_mm = 0;
pitch_degree = 0;
yaw_degree = 0;
radius = 0;
feature = 0;
x = 0;
x_p = 0;
direction = 0;
lap = 0
milage = 0;

m.led(1)

# 車軸が少し回転するのを待つ
m.waitIntTrig()

m.led(2)

# 立ち上がりは全力で走る
m.setDuty(255)

# 1分走るか、3周したら終わる
while milage < 90000 and lap < 12 :
	# 1/52秒待ち/センサデータの更新
	m.grab()

	delta_mm     = m.getRpm() * COEFF_DELTA;
	yaw_degree   = convertTo_degree(m.getYaw());
	pitch_degree = m.getPitch() * COEFF_DEGREE;

	direction = direction + yaw_degree;

	x_p = x;
	x = x + delta_mm * math.cos(math.radians(direction));
	if x_p < 0 and x >= 0:
		lap = lap + 1;

	feature = feature_value(delta_mm, yaw_degree, pitch_degree);

	pos = spe.estimate(delta_mm, feature);
	m.setSpeed(ml.get_policy(pos));

	print (m);
	milage = milage + delta_mm;

m.led(3)
m.setDuty(0)
