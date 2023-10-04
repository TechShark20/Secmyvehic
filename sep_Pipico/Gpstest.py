from machine import UART, Pin
import utime
import math 
buzzpin =Pin(13,Pin.OUT,value=0)

Trigger=Pin(14,Pin.IN)


destination_phone=+917678422306
#gsm = UART(1, baudrate=9600, tx=Pin(8), rx=Pin(9), timeout=2000)
gps = UART(0, baudrate=9600, tx=Pin(0), rx=Pin(1),timeout =2000)
def convert_to_string(buf):
    tt = ''.join(chr(byte) for byte in buf)
    return tt.strip()

def send_command(cmdstr, lines=1, msgtext=None):# code from https://diyprojectslab.com/raspberry-pi-pico-home-automation-gsm-sim800/
    global gsm_buffer
    print(cmdstr)
    cmdstr = cmdstr+'\r\n'
    
    while gsm.any():
        gsm.read()
    
    gsm.write(cmdstr)
    
    if msgtext:
        print(msgtext)
        gsm.write(msgtext)
 
    buf=gsm.readline() 
    if not buf:
        return None
    result = convert_to_string(buf)
    
    if lines>1:
        gsm_buffer = ''
        for i in range(lines-1):
            buf=gsm.readline()
            if not buf:
                return result
            buf = convert_to_string(buf)
            if not buf == '' and not buf == 'OK':
                gsm_buffer += buf+'\n'
    
    return result

#print(send_command('AT'))
#utime.sleep(1)
#print(send_command("AT+CCID"))
#utime.sleep(1)
#print(send_command("AT+CREG?"))
#utime.sleep(1)

def convertg(latitude,dirlat, longitude,dirlong):
    if latitude=='' or longitude =='':
        return '',''
    # convert latitudes and longitude to degrees to make them use in google maps
    latd=int(float(latitude)/100)
    latm=float((float(latitude)-float(latd*100))/60)
    glat=float(latd+latm)
    longdeg=int(float(longitude)/100)
    longmin=float((float(longitude)-float(longdeg*100))/60)
    glon=float(longdeg+longmin)
    
    
    
    
    if dirlat == 'S':
        glat = -glat
    if dirlong == 'W':
        glon = -glon
    
    return glat, glon

def actionon(): # function to on a buzzer
    buzzpin.toggle()
def actionoff():# function to turn off a buzzer with a 3 second delay
    time.sleep(3)
    buzzpin.toggle()
def convertgps(latitude, longitude):
    message = "Hey, your vehicle is in danger. https://www.google.com/maps/search/?api=1&query={},{}".format(latitude, longitude)
    print(message)
    #print(send_command('AT+CMGF=1'))
    #print(send_command('AT+CNMI=1'))
    #send_command('AT+CMGS="{}"\n'.format(destination_phone),99,message+'\x1A')

def callgps():
    buff = str(gps.readline())
    print(buff)
    parts=[]
    
    while not buff.startswith("b'$GPRMC"):
        buff = str(gps.readline())
        print(buff)
    parts = buff.split(',')
    print(len(parts))
    return [parts[3],parts[4],parts[5],parts[6]]



def callroutine():
      while True:
           buzzpin.toggle()
           sentence = callgps()
           lat,long,dirlat,dirlong =sentence[0],sentence[2],sentence[1],sentence[3]
           lat,long =convertg(lat,dirlat,long,dirlong)
           convertgps(lat ,long )
           utime.sleep(3)
           buzzpin.toggle()
           utime.sleep(120)
           


utime.sleep(4)

while True:
     print(Trigger.value())
     if Trigger.value()==True:
         callroutine()
            
     utime.sleep(1)


    
 


       
        
    
    
    





