from machine import Pin
import time 

Trigger=Pin(14,Pin.IN)
buzzer =Pin(13,Pin.OUT,value=0)



while True:
    print(Trigger.value())
    if Trigger.value()==True:
        buzzer.toggle()
        time.sleep(0.4)
        buzzer.toggle()
        
        
    time.sleep(0.4)
    
    
