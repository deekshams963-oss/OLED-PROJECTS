import cv2
import numpy as np
import serial
import time
import os

PORT= 'COM7'
BAUD= 115200
TARGETFPS=10
DURATION=30

print("Current Folder: ", os.getcwd())

vid = cv2.VideoCapture("vikedit.mp4")

if not vid.isOpened():
    print("Could not open the video")
    exit()

sourcefps = vid.get(cv2.CAP_PROP_FPS)

if sourcefps <=0:
    print("INVALID VIDEO FPS")
    vid.release()
    exit()

frameStep = max(1, round(sourcefps/TARGETFPS))

print("Source FPS: ", sourcefps)
print("Target FPS: ", TARGETFPS)

print("CONNECTING TO ARDUINO...")

ser= serial.Serial(PORT, BAUD, timeout=2)

print("CONNECTED!")
time.sleep(3)

ready=False
deadline= time.time() + 5

while time.time() < deadline:
    response = ser.readline().decode(errors='ignore').strip()

    if response:
        print("Arduino: ", response)

    if response == "READY":
       ready= True
       break

if not ready:
    print("Arduino did not send READY")
    ser.close()
    vid.release()
    exit()

print("Arduino ready!")
print("Starting video...")

totalframes = int(vid.get(cv2.CAP_PROP_FRAME_COUNT) / frameStep)
displayed = 0

try:
    for _ in range(totalframes):

        starttime = time.monotonic()

        success, frame = vid.read()

        if not success:
            print("Video ended")
            break

        for _ in range(frameStep - 1):
                    if not vid.grab():
                        break

        resized = cv2.resize(frame, (128, 64))
        
        grayscale = cv2.cvtColor(resized,cv2.COLOR_BGR2GRAY)
        _, binary = cv2.threshold(grayscale, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)

        bits = (binary > 0).astype(np.uint8)
       
        packed = np.packbits(bits.reshape(8, 8, 128).transpose(0, 2, 1), axis=2, bitorder='little')
       
        frameData = packed.flatten()

        ser.write(frameData.tobytes())
        ser.flush()

        confirmation = ser.readline().decode(errors='ignore').strip()

        if confirmation != "FRAMEOK":
          print("Frame transfer failed: ", confirmation)
          break

        displayed += 1
    
        print("Frame: ", displayed)

        elapsed = time.monotonic() - starttime
        remaining = (1 / TARGETFPS) - elapsed
    
        if remaining > 0:
         time.sleep(remaining)

finally:
    vid.release()
    ser.close()

print("Playback finished.")
print("Frames displayed:", displayed)