import sys
import math
import json

FEATURE_UNKNOWN=0;
FEATURE_UPDOWN = 1;
FEATURE_OUT_LEFT = 2; 
FEATURE_CENTER_LEFT = 3;
FEATURE_IN_LEFT = 4;
FEATURE_OUT_RIGHT = 5;
FEATURE_CENTER_RIGHT = 6;
FEATURE_IN_RIGHT = 7;
FEATURE_STRAIGHT = 8

class SampledPoint:
	def __init__(self, x, y, l, f):
		self.x = x
		self.y = y
		self.lane = l
		self.feature = f

def parts_from_name(parts,name):
	for p in parts:
		if name == p["name"]:
			return p["info"]

	return  None

def ddirection_from_type():
	return 0

def delta_direction(t, delta):
	if t == FEATURE_OUT_LEFT:
		return 360*delta/(2*math.pi*661.5)
	elif t == FEATURE_CENTER_LEFT:
		return 360*delta/(2*math.pi*544.5)
	elif t == FEATURE_IN_LEFT:
		return 360*delta/(2*math.pi*427.5)
	elif t == FEATURE_OUT_RIGHT:
		return -360*delta/(2*math.pi*661.5)
	elif t == FEATURE_CENTER_RIGHT:
		return -360*delta/(2*math.pi*544.5)
	elif t == FEATURE_IN_RIGHT:
		return -360*delta/(2*math.pi*427.5)
	else:
		return 0

def main():
	with open('jcjc_parts.json','r') as f:
		parts=json.load(f) 

	with open('gat2024_layout.json','r') as f:
		layout=json.load(f) 

	map_arr=[]
	lane=0
	fragment = {"type": 8,"length": 0.0,"ratio":1.0}

	x=0
	y=0
	direction=0

	dx=0
	dy=0
	ddirection = 0

	# 100mmでSliceします
	for i in range(3):
		for name in layout:
			info = parts_from_name(parts, name)
			for section in info[lane]:
				total += section['length']

				length = section['length'] + fragment['length']

				while length > 100:
					if fragment['length'] > 0:
						if fragment['type'] == FEATURE_UPDOWN:
							t=FEATURE_UPDOWN
						elif fragment['length']>50:
							t=fragment['type']
						else:
							t=section['type']

						ddirection += delta_direction(t, fragment['length'])

						fragment['length'] = 0;
					else:
						t = section['type']
						ddirection += delta_direction(t, 100)

					length -= 100

					# print (idx, name, lane, t, length, direction + ddirection)

					dx += 100 * math.cos(math.radians(direction + ddirection))
					dy += 100 * math.sin(math.radians(direction + ddirection))

					map_arr.append(SampledPoint(x+dx, y+dy, i , t))

				fragment = {"type": section['type'],"length": length,"ratio": section['ratio']}

				lane =  section.get('next',lane)
				x += section.get('dx',0) * math.cos(math.radians(direction)) - section.get('dy',0) * math.sin(math.radians(direction))
				y += section.get('dx',0) * math.sin(math.radians(direction)) + section.get('dy',0) * math.cos(math.radians(direction))
				direction += section.get('rotate',0)

			dx=0
			dy=0
			ddirection=0

	# Tool形式でファイル出力する
	str_map = 'arr_map=['
	str_x = 'arr_x=['
	str_y = 'arr_y=['
	str_l = 'arr_l=['
	str_policy = 'arr_policy=['

	for s in map_arr:
		str_map += str(s.feature) + ','
		str_x += str(s.x) + ','
		str_y += str(s.y) + ','
		str_l += str(s.lane) + ','
		str_policy += str(25) + ','


	str_map += ']\n'
	str_x += ']\n'
	str_y += ']\n'
	str_l += ']\n'
	str_policy += ']\n'

	with open('map.txt', 'w') as f:
		f.write('# SSV\n')
		f.write(str_map)
		f.write(str_x)
		f.write(str_y)
		f.write(str_l)
		f.write(str_policy)


	print(total)

if __name__ == "__main__":
    main()