# Sample Name: 012_machine_learning.py
# 概要: 自己位置を推定しながらマシンを走らせます。走行した結果を基に、
#       arr_policyを更新します。
#
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd
import math
import gc

arr_map = [8,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,8,5,8,4,8,5,1,1,8,1,8,8,2,8,2,8,5,5,5,8,8,8,8,1,3,4,4,4,4,4,4,4,4,4,4,4,4,2,8,8,8,8,8,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,1,4,4,4,4,4,4,4,4,4,4,4,4,4,8,6,8,4,8,6,8,8,8,8,8,8,5,5,8,2,4,8,8,8,8,8,8,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,8,8,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,1,1,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,8,5,4,4,8,6,8,8,8,8,8,8,6,8,8,3,8,8,8,8,8,8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,8,8]

arr_policy=[10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0,]



UNIT_mm = 100;
TIRE_SIZE_mm = 26
INTERVAL_sec = 1.0/52.0
COEFF_DELTA  = (1.0 / 60.0) * INTERVAL_sec * TIRE_SIZE_mm * 3.14
COEFF_DEGREE = (1.0 / 1000.0) * INTERVAL_sec

CENTER_TO_OUTER_LEFT  = 640
INNER_TO_CENTER_LEFT  = 510
INNER_TO_CENTER_RIGHT = -510
CENTER_TO_OUTER_RIGHT = -640

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
	if math.fabs(roll) > 200000:
		eva = -1;
	if az >10000:
		eva = -1;

	return eva;


def convertTo_degree(mdps) :
	if math.fabs(mdps) < 10000.0:
		return 0.0
	else:
		return mdps * COEFF_DEGREE


# ミニ四駆制御器を作る
m = mini4wd.Machine();

# マシンのタイヤ直径をマシンに合わせる
m.setTireSize(TIRE_SIZE_mm)

# PID制御のゲインを設定
# PID制御のゲインを設定
m.setGainKp(0.2)
m.setGainKi(0.05)
m.setGainKd(0.05)

#m.setGain(0,    0,          0,  0)
#m.setGain(0.1,  0.0025,     0,  1)
#m.setGain(0.1,  0.008,      0,  2)
#m.setGain(0.1,  0.0135,     0,  3)
#m.setGain(0.1,  0.019,      0,  4)
#m.setGain(0.1,  0.0245,     0,  5)
#m.setGain(0.109,0.03,       0,  6)
#m.setGain(0.114,0.0355,     0,  7)
#m.setGain(0.125,0.041, 0.0005,  8)
#m.setGain(0.144,0.0465,0.0053,  9)
#m.setGain(0.169,0.052, 0.0101, 10)
#m.setGain(0.201,0.0575,0.0149, 11)
#m.setGain(0.240,0.063, 0.0197, 12)
#m.setGain(0.286,0.0685,0.0245, 13)
#m.setGain(0.338,0.074, 0.0293, 14)
#m.setGain(0.398,0.0795,0.0341, 15)
#m.setGain(0.464,0.085, 0.0389, 16)
#m.setGain(0.537,0.0905,0.0437, 17)
#m.setGain(0.616,0.096, 0.0485, 18)
#m.setGain(0.703,0.1015,0.0533, 19)
#m.setGain(0.796,0.107, 0.0581, 20)
#m.setGain(0.896,0.1125,0.0629, 21)
#m.setGain(1.003,0.118, 0.0677, 22)
#m.setGain(1.117,0.1235,0.0725, 23)
#m.setGain(1.237,0.129, 0.0773, 24)
#m.setGain(1.365,0.1345,0.0821, 25)


# 位置推定器を作る
spe = mini4wd.SelfPositionEstimater(2000, 100);

# 特徴量マップを読み込む
for i in range(len(arr_map)):
	spe.push_map_item(arr_map[i]);

spe.set_weight(0.01)
spe.prepare()

# 機械学習器を生成
# 引数
# 1. マップのサイズ
# 2. 学習1回ごとの全体の速度の上昇値
# 3. 時定数（秒）
# 4. マップの1ますごとの距離(mm)
# 5. 初期速度(km/h)
ml = mini4wd.MachineLearning(len(arr_map), 2, 0.4, UNIT_mm, 10.0);
if len(arr_policy) != 0 :
	for i in range(len(arr_policy)) :
		ml.set_policy(i, arr_policy[i]);

cnt = 0;
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

print("arr_policy=[", end="");
while cnt < len(arr_map) :
	# 1/52秒待ち/センサデータの更新
	m.grab()

	print(str(ml.get_policy(cnt)) + ",", end="");

	cnt = cnt + 1;
print("]");

m.led(1)

# 車軸が少し回転するのを待つ
m.waitIntTrig()

m.led(2)

# 立ち上がりは全力で走る
#m.setDuty(128)

m.setSpeed(10)

# 1分走るか、6周、50m走行したら終わる
# while cnt < 3120 and lap < 6 and milage < 50000:
# 1分走るか、150m走行したら終わる
while cnt < 1040 and milage < 50000:
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
	evaluation = evaluation_value(m.getAz(), m.getRoll());

	pos = spe.estimate(delta_mm, feature);

	ml.set_reward(pos, evaluation, m.getSpeed());

	m.setSpeed(ml.get_policy(pos));

	print (m, end='\t');
	print (pos);

	cnt = cnt + 1;
	milage = milage + delta_mm;

print(milage)
print(lap)

m.led(3)
m.setDuty(0)

print (ml)

cnt = 0;
ml.update(0);
 
print("next arr_policy=[", end="");
while cnt < len(arr_map) :
	# 1/52秒待ち/センサデータの更新
	m.grab()

	print(str(ml.get_policy(cnt)) + ",", end="");

	cnt = cnt + 1;
print("]");
