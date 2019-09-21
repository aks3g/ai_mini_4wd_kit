# Sample Name: 012_machine_learning.py
# �T�v: ���Ȉʒu�𐄒肵�Ȃ���}�V���𑖂点�܂��B���s�������ʂ���ɁA
#       arr_policy���X�V���܂��B
#
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd
import math
import gc


arr_map = [8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,5,5,5,6,5,2,4,8,7,6,8,4,4,8,5,4,4,4,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,4,8,7,5,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8,8,8,8,4,4,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8,8,8,8,8,5,5,5,5,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,6,8,4,2,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,8,8,8,8,8,8,3,8,7,5,2,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,1,1,8,5,5,8,8,8,8,8,5,5,5,5,8,8,8,8,8,8,5,6,5,5,5,5,5,5,5,1,1,1,4,5,7,8,3,8,5,4,4,8,7,5,5,5,5,5,5,5,5,5,5,8,8,8,8,8,8,4,4,4,4,4,4,8,8,8,8,8,8,5,8,4,4,8,8,8,8,8,8,8,8,5,8,8,4,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,1,1,8,4,2,2,2,2,2,2,3,4,2,4,4,4,4,3,3,2,1,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,6,6,6,6,8,4,8,6,7,8,8,4,8,6,8,4,3,3,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,4,8,7,5,3,3,3,3,3,3,3,3,3,2,8,8,8,8,8,8,8,8,8,8,8,2,3,3,3,3,3,3,3,3,3,3,3,2,8,8,8,8,8,8,8,8,8,8,8,7,6,6,5,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,5,8,4,8,7,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,8,8,8,8,8,8,2,8,7,7,8,3,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,1,1,8,7,5,5,5,8,5,5,6,6,5,8,8,8,8,8,6,7,6,6,6,6,6,5,1,1,2,8,7,7,8,2,7,8,4,2,6,6,6,6,6,6,6,6,6,5,8,8,8,8,8,8,4,3,3,3,3,3,3,2,8,8,8,8,8,8,6,8,4,8,7,8,8,8,8,8,8,6,5,8,4,8,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,8,1,1,1,2,2,8,8,8,3,8,8,2,3,2,2,2,8,2,3,2,2,2,2,2,1,1,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,7,7,7,8,4,4,8,7,6,8,4,3,8,5,2,3,2,2,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,8,8,4,7,7,8,4,2,2,2,2,2,2,2,2,2,2,8,8,8,8,8,8,8,8,8,8,8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,8,8,8,8,8,8,8,8,8,8,8,7,7,7,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,6,8,4,2,5,7,7,7,7,7,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,4,8,7,5,4,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,1,1,1,7,7,6,5,5,6,7,7,8,8,8,8,8,7,7,7,7,7,7,1,1,1,4,8,7,5,3,8,7,4,4,8,7,7,7,7,7,7,7,5,8,8,8,8,8,8,4,2,2,2,2,2,2,2,2,8,8,8,8,8,8,8,5,4,4,8,6,1,8,8,8,8,2,8,2,8,8,6,5,8,8,8,1,1,1,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,1,1,2,4,4,3,2,2,4,4,4,4,4,4,4,4,1,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,7,5,5,5,5,6,8,4,8,7,7,8,2,4,8,5,2,4,4,8,8,8,8,8,1,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,8,3,8,7,5,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8,8,8,8,4,4,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8,8,8,8,5,5,5,5,5,1,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,5,8,4,8,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,8,8,8,8,8,8,4,8,7,8,4,8,8,8,8,8,1,8,8,8,8,8,8,8,8,8,8,1,1,1,8,6,8,8,8]

arr_policy=[]


UNIT_mm = 100;
TIRE_SIZE_mm = 31
INTERVAL_sec = 1.0/52.0
COEFF_DELTA  = (1.0 / 60.0) * INTERVAL_sec * TIRE_SIZE_mm * 3.14
COEFF_DEGREE = (1.0 / 1000.0) * INTERVAL_sec

CENTER_TO_OUTER_LEFT  = 620
INNER_TO_CENTER_LEFT  = 510
INNER_TO_CENTER_RIGHT = -510
CENTER_TO_OUTER_RIGHT = -620

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
#		eva = eva - 5.0; log018�܂�
		eva = eva - 3.0;

	return eva;


def convertTo_degree(mdps) :
	if math.fabs(mdps) < 10000.0:
		return 0.0
	else:
		return mdps * COEFF_DEGREE


# �~�j�l�쐧�������
m = mini4wd.Machine();

# �}�V���̃^�C�����a��31�o�ɕύX
m.setTireSize(TIRE_SIZE_mm)

# PID����̃Q�C����ݒ�
m.setGainKp(0.1)
m.setGainKi(0.02)
m.setGainKd(0.02)


# �ʒu���������
spe = mini4wd.SelfPositionEstimater(2000, 100);

# �����ʃ}�b�v��ǂݍ���
for i in range(len(arr_map)):
	spe.push_map_item(arr_map[i]);

spe.set_weight(0.001)
spe.prepare()

# �@�B�w�K��𐶐�
# ����
# 1. �}�b�v�̃T�C�Y
# 2. ��V���`��������͈�
# 3. �w�K1�񂲂Ƃ̑S�̂̑��x�̏㏸�l
# 4. ���萔�i�b�j
# 5. �}�b�v��1�܂����Ƃ̋���(mm)
# 6. �������x(km/h)
ml = mini4wd.MachineLearning(len(arr_map), int(1000/UNIT_mm), 1.0, 0.5, UNIT_mm, 10.0);
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

m.led(1)

# �Ԏ���������]����̂�҂�
m.waitIntTrig()

m.led(2)

# �����オ��͑S�͂ő���
m.setDuty(255)

# 1�����邩�A3��������I���
while milage < 105000 and lap < 3 :
	# 1/52�b�҂�/�Z���T�f�[�^�̍X�V
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

	print (m);
	cnt = cnt + 1;
	milage = milage + delta_mm;

m.led(3)
m.setDuty(0)

cnt = 0;
ml.update(0);

print("arr_policy=[", end="");
while cnt < len(arr_map) :
	# 1/52�b�҂�/�Z���T�f�[�^�̍X�V
	m.grab()

	print(str(ml.get_policy(cnt)) + ",", end="");

	cnt = cnt + 1;
print("]");
