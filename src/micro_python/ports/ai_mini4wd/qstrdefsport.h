/*
 * qstrdefsport.h
 *
 * Created: 2019/03/10
 * Copyright 2019 Kiyotaka Akasaka. All rights reserved.
 */
#ifndef QSTRDEFSPORT_H_
#define QSTRDEFSPORT_H_


//J Mini4wd
Q(mini4wd)
Q(Machine)

Q(led)
Q(sw)

Q(setDuty)
Q(setRpm)
Q(setSpeed)
Q(setTireSize)
Q(setGainKp)
Q(setGainKi)
Q(setGainKd)
Q(grab)
Q(getAx)
Q(getAy)
Q(getAz)
Q(getRoll)
Q(getPitch)
Q(getYaw)
Q(getSpeed)
Q(getRpm)
Q(getVbat)
Q(getMotorCurrent)
Q(waitIntTrig)
Q(waitExtTrig)
Q(checkExtTrig)

//J FS
Q(ai_mini4wd_disk)
Q(readblocks)
Q(writeblocks)
Q(ioctl)

//J PYB
Q(pyb)
Q(millis)
Q(elapsed_millis)
Q(delay)

Q(real)
Q(imag)
Q(complex)
Q(float)
Q(math)
Q(cmath)
Q(e)
Q(erf)
Q(degrees)
Q(radians)
Q(trunc)
Q(gamma)

// ext
Q(urandom)





#endif /* QSTRDEFSPORT_H_ */
