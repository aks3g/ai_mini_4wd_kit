import mini4wd

m = mini4wd.Machine()

m.setTireSize(31)
m.setGainKp(0.1)
m.setGainKi(0.02)
m.setGainKd(0.02)

m.led(1)

m.waitIntTrig()
m.led(2)

m.setSpeed(10)
cnt = 0;
while cnt < 208 :
	m.grab()
	cnt = cnt + 1
	print (m)

m.setDuty(0)
