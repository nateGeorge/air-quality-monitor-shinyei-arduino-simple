import serial, datetime, pickle
import numpy as np
import requests
import glob


PrKEY = '<private key>'
PuKEY = '<public key>'
postADDR = 'http://data.sparkfun.com/input/%s?private_key=%s&1um_particle_concentration=' % (PuKEY, PrKEY)

saveFile = 'dustdata - 93 ridgeview '


fileCount = 0
for file in glob.iglob(saveFile + '*'):
	print "already existing files:"
	print file
	fileCount += 1
	
saveFile = saveFile + str(fileCount)

print ""
print "saving file as:"
print saveFile

ser=serial.Serial()
ser.baudrate=9600
ser.port=2
ser.open()
firsttime=True
counter=0
while True:
	tempdata=ser.readline().rstrip('\r\n')
	
	if len(tempdata)>0:
		try:
			r = requests.post(postADDR + tempdata)
			print r.status_code, r.reason
			print 'air quality: ', tempdata
		except Exception as e: # need to implement better code for storing data and sending if can't connect
			print 'coulnd\'t connect, skipping data'
			print e
			continue
		timenow=datetime.datetime.now()
		if firsttime:
			dustdata=np.array([timenow,float(tempdata)])
			firsttime=False
		else:
			try:
				float(tempdata)
				dustdata=np.append(dustdata,[timenow,tempdata])
			except ValueError:
				print 'air quality: ', tempdata
		#print dustdata
		counter+=1
		if counter==1:
			try:
				pickle.dump(dustdata,open(saveFile,'wb'))
			except Exception as msg:
				print 'whoops'
				print msg
			
			#close('dustdata')
			counter=0
		
