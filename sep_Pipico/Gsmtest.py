
def convert_to_string(buf):
    tt = ''.join(chr(byte) for byte in buf)
    return tt.strip()

def send_command(cmdstr, lines=1, msgtext=None):# code from https://diyprojectslab.com/raspberry-pi-pico-home-automation-gsm-sim800/
    global gsm_buffer
    cmdstr = cmdstr+'\r'
    
    while gsm.any():
        gsm.read()
        print(gsm.read())
    
    gsm.write(cmdstr)
    
    if msgtext:
        print(msgtext)
        gsm.write(msgtext)
    utime.sleep(1) 
    buf=gsm.readline()
    print(buf)
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
def send_sms(msgtext):
    global gsm_buffer
    result = send_command('AT+CMGS="{}"\n'.format(destination_phone),99,msgtext+'\x1A')
    if result and result=='>' and gsm_buffer:
        params = gsm_buffer.split(':')
        if params[0]=='+CUSD' or params[0] == '+CMGS':
            print('OK')
            return 'OK'
    print('ERROR')
    return 'ERROR'

def callgsm():
    message = "Hey, your vehicle is in danger."
    res=send_command('AT')
    print(res)
    utime.sleep(1)
    print(send_command("AT+CREG?"))
    utime.sleep(1)
    print(send_command('AT+CMGF=1'))
    utime.sleep(1)
    res=send_sms(message)
    print(res)
    return res
    

   
callgsm()




