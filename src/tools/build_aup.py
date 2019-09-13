import sys
import os
import struct
import binascii
import hashlib

def get_sdk_version() :
	sdk_version_h = open(os.path.dirname(__file__) + "/../library/include/sdk_version.h", 'r');

	for line in sdk_version_h :
		if '#define AI_MINI_4WD_VERSION' in line :
			line = line.rstrip(')\r\n');
			line = line.rstrip(')\n');
			line = line.rstrip(')\r');
			line = line.lstrip('#define AI_MINI_4WD_VERSION');
			line = line.lstrip('\t');
			line = line.lstrip('(');

			return int(line, 16)

	return 0x00000000;

def get_firmware_size(fw_name) :
	return os.path.getsize(fw_name);

def get_git_hash() :
	orig_head = open(os.path.dirname(__file__) + "/../../.git/ORIG_HEAD", 'r');

	return orig_head.readline()[0:4];

aup_file = open("MINI4WD.AUP", 'w+');
firmware_file = open(sys.argv[1], 'r');

firmware = firmware_file.read();

header = '';
header += '4WD\0';
header += struct.pack('I', get_sdk_version());
header += get_git_hash();
header += struct.pack('I', get_firmware_size(sys.argv[1]));
header += binascii.unhexlify(hashlib.md5(firmware).hexdigest());

aup_file.write(header);
aup_file.write(firmware);