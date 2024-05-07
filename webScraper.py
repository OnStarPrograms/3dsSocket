from urllib.request import urlopen
import time
import os
import mouse
from pynput.keyboard import Key, Controller
import serial
SerialObj = serial.Serial('COM6') # COMxx  format on Windows
keyboard = Controller()
if not SerialObj.isOpen(): 
            SerialObj.open()
SerialObj.baudrate = 9600  # set Baud rate to 9600
SerialObj.bytesize = 8   # Number of data bits = 8
SerialObj.parity  ='N'   # No parity
SerialObj.stopbits = 1   # Number of Stop bits = 1
# import threading
# def mover(x,y):
#     mouse.move(x, y, absolute=False)
#url to open 3ds server
url = "http://172.16.42.41/"
# url = "http://"+input("http://")

f = open("conf.cfg", 'r')
fileRead = f.read().split("\n")
f.close()
count = 0
check = 5
for i in fileRead:
    match(i):
        case "enter":
            i = Key.enter
        case "space":
            i = Key.space
        case "caps_lock":
            i = Key.caps_lock
        case "esc":
            i = Key.esc
    if count == 15:
        zldown = i
        count += 1
        
    if count == 14:
        zrdown = i
        count += 1
        
    if count == 13:
        padright = i
        count += 1
        
    if count == 12:
        padleft = i
        count += 1
        
    if count == 11:
        paddown = i
        count += 1
        
    if count == 10:
        padup = i
        count += 1
    
    if count == 9:
        selectdown = i
        count += 1
        
    if count == 8:
        startdown = i
        count += 1
        
    if count == 7:
        ydown = i
        count += 1
        
    if count == 6:
        xdown = i
        count += 1
        
    if count == 5:
        bdown = i
        count += 1
    
    if count == 4:
        adown = i
        count += 1
        
    if count == 3:
        dright = i
        count += 1
        
    if count == 2:
        dleft = i
        count += 1
    
    if count == 1:
        ddown = i
        count += 1
    
    if count == 0:
        dup = i
        count += 1

pastX = [0,0]
pastY = [0,0]
allButtons = []

max = 5


while (True):
    page = urlopen(url)
    html_bytes = page.read()
    html_code = html_bytes.decode("utf-8")
    
    
    dstitle = html_code.split("<tr>")[1]
    dsdata = html_code.split("<tr>")[2]
    dstouchtitle = html_code.split("<tr>")[3]
    dstouchdata = html_code.split("<tr>")[4]
    
    dstitle = dstitle.split("<th>")
    for i in range(len(dstitle)):
        dstitle[i] = dstitle[i].split("</th")[0]
    dstitle.pop(0)
    print(dstitle)
    
    dsdata = dsdata.split("<td>")
    for i in range(len(dsdata)):
        dsdata[i] = dsdata[i].split("</td")[0]
    dsdata.pop(0)
    print(dsdata)

    dstouchtitle = dstouchtitle.split("<th>")
    for i in range(len(dstouchtitle)):
        dstouchtitle[i] = dstouchtitle[i].split("</th")[0]
    dstouchtitle.pop(0)
    print(dstouchtitle)
    
    dstouchdata = dstouchdata.split("<td>")
    for i in range(len(dstouchdata)):
        dstouchdata[i] = dstouchdata[i].split("</td")[0]
    dstouchdata.pop(0)
    dstouchdata[0] = int(dstouchdata[0])-197
    dstouchdata[1] = int(dstouchdata[1])-102
    if (dstouchdata[0] == -197):
        dstouchdata[0]+=197
        dstouchdata[1]+=102
    print(dstouchdata)
    
    for i in allButtons:
        keyboard.release(i)
    # Prints out what buttton is being pushed (add keyboard controls here)
    for i in range(len(dsdata)):
        if dsdata[i] == '1':
            match(dstitle[i]):
                case "a":
                    keyboard.press(adown)
                    allButtons.append(adown)
# >>> keyboard.press('left')     # press the left arrow key
                case "b":
                    print("pushed b")
                    keyboard.press(bdown)
                    allButtons.append(bdown)
                case "x":
                    print("pushed x")
                    keyboard.press(xdown)
                    allButtons.append(xdown)
                case "y":
                    print("pushed y")
                    keyboard.press(ydown)
                    allButtons.append(ydown)
                case "start":
                    print("pushed srt")
                    keyboard.press(startdown)
                    allButtons.append(startdown)
                case "select":
                    print("pushed sel")
                    keyboard.press(selectdown)
                    allButtons.append(selectdown)
                case "l":
                    print("pushed l")
                    mouse.click('left')
                case "r":
                    print("pushed r")
                    mouse.click('right')
                case "dup":
                    print("pushed dp")
                    keyboard.press(dup)
                    allButtons.append(dup)
                case "ddown":
                    print("pushed dd")
                    keyboard.press(ddown)
                    allButtons.append(ddown)
                case "dleft":
                    print("pushed dl")
                    keyboard.press(dleft)
                    allButtons.append(dleft)
                case "dright":
                    print("pushed dr")
                    keyboard.press(dright)
                    allButtons.append(dright)
                case "zrdown":
                    print("pushed zr")
                    keyboard.press(zrdown)
                    allButtons.append(zrdown)
                case "zldown":
                    print("pushed zl")
                    keyboard.press(zldown)
                    allButtons.append(zldown)
                case "padup":
                    print("pushed pu")
                    keyboard.press(padup)
                    allButtons.append(padup)
                case "paddown":
                    print("pushed pd")
                    keyboard.press(paddown)
                    allButtons.append(paddown)
                case "padleft":
                    print("pushed pl")
                    keyboard.press(padleft)
                    allButtons.append(padleft)
                case "padright":
                    print("pushed pr")
                    keyboard.press(padright)
                    allButtons.append(padright)
    
    
    # puts past [set amount] of inputs from the ds into a list (idea average out past inputs to get mouse move) OR move mouse difference of newest move by earliest move with regards to + and -
    if (len(pastX) > max):
        pastX.pop(0)
        pastY.pop(0)
    pastX.append(dstouchdata[0])
    pastY.append(dstouchdata[1])
    try:
        # time.sleep(2)
        # SerialObj.write(b'X')    #SelectPort
        # time.sleep(1)
        # SerialObj.write(b'')
        if (dstouchdata[0] < 0):
            SerialObj.write(b'X')
        elif (dstouchdata[0] > 0):
            SerialObj.write(b'B') #Change the analog output through active port
        SerialObj.write('l')
        if (dstouchdata[1] < 0):
            SerialObj.write(b'A')
        elif (dstouchdata[1] > 0):
            SerialObj.write(b'Y')  
        # SerialObj.close()      # Close the port
    except:
        pass
    print(pastX)
    print(pastY)
    # f = open("C:\\Users\\aiden\\DSData.txt", "w")
    # f.write(dstitle)
    # f.write("\n")
    # f.write(dsdata)
    # f.write("\n")
    # f.write(dstouchtitle)
    # f.write("\n")
    # f.write(dstouchdata)
    # f.close()
    # pyautogui.move(pastX[len(pastX)-1]*5, pastY[len(pastX)-1]*5, .5)
    # t1 = threading.Thread(target=mover(pastX[len(pastX)-1], pastY[len(pastY)-1]), args=(10,))
    
    # starting thread 1
    # t1.start()
    # mover(pastX[len(pastX)-1], pastY[len(pastY)-1])
    time.sleep(.05)
    os.system('cls')
    
    