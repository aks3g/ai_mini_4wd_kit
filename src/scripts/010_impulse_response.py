# Sample Name: 010_impulse_response.py
# �T�v: Duty��0����ő�ɐݒ肵�A���̌�0�ɖ߂������̃Z���T�̒l���擾���܂�
# 
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd

m = mini4wd.Machine()

cnt = 0;
while cnt < 52 * 2:
	# 1/52�b�҂�/�Z���T�f�[�^�̍X�V
	m.grab()

	print(m);

	cnt = cnt + 1

#�S�͂Ń��[�^�[���܂킷
m.setDuty(255)

cnt = 0
while cnt < 52 * 5:
	# 1/52�b�҂�/�Z���T�f�[�^�̍X�V
	m.grab()

	print(m);

	cnt = cnt + 1;

#���[�^�[���~�߂Ă��̌�̓������m�F����
m.setDuty(0)

cnt = 0;
while cnt < 52 * 5:
	# 1/52�b�҂�/�Z���T�f�[�^�̍X�V
	m.grab()

	print(m);

	cnt = cnt + 1