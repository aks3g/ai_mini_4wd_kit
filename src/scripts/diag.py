import mini4wd
import math

m = mini4wd.Machine()

m.setTireSize(31)
m.setGainKp(0.1)
m.setGainKi(0.02)
m.setGainKd(0.02)

print ("Start Diagnositic test")

print ("Calibrate Tachometer ... ", end="");

threshold_mv = m.calibrateTachometer();
m.setTachometerThreshold(threshold_mv);
print("OK")
m.led(1)

print (threshold_mv);

print ("Press SW0 ... ", end="")
while False == m.sw(0):
	m.led(1)
print("OK")
m.led(2)

print ("Press SW1 ... ", end="")
while False == m.sw(1):
	m.led(2)
print("OK")
m.led(3)

print ("Wait for internal trigger ... ", end="")
m.waitIntTrig()
print("OK")
m.led(4)

print ("Wait for external trigger ... ", end="")
m.waitExtTrig()
print("OK")
m.led(5)

print ("Rotate around Roll axis ... ", end="")
validate = False
while validate == False:
	m.grab()
	if math.fabs(m.getRoll()) > 100000 :
		validate = True
print ("OK")
m.led(6)

print ("Rotate around Pitch axis ... ", end="")
validate = False
while validate == False:
	m.grab()
	if math.fabs(m.getPitch()) > 100000 :
		validate = True
print ("OK")
m.led(7)

print ("Rotate around Yaw axis ... ", end="")
validate = False
while validate == False:
	m.grab()
	if math.fabs(m.getYaw()) > 100000 :
		validate = True
print ("OK")
m.led(8)

print ("Drive Check ... ");
m.setSpeed(10);

cnt = 0;
while cnt < 208 :
	m.grab()
	cnt = cnt + 1;

print (str(m.getVbat()) + "\t" + str(m.getMotorCurrent()))
m.led(9)

m.setDuty(0);
cnt = 0;
while cnt < 52 :
	m.grab()
	cnt = cnt + 1;
m.led(10)


m.setDuty(-150);
cnt = 0;
while cnt < 52 :
	m.grab()
	cnt = cnt + 1;

print ("OK");
m.led(11)
m.setDuty(0);

