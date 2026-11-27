import cv2
import numpy as np
import serial
import time
import os

PORT= 'COM7'
BAUD= 115200
TARGETFPS=20

WIDTH= 128
HEIGHT= 64

print("Current Folder: ", os.getcwd())

inpvid=input("Enter path of the video you want to display: ").strip().strip('"').strip("'")

vid = cv2.VideoCapture(inpvid, cv2.CAP_FFMPEG)

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


def floyd_steinberg(image):

    image= image.astype(np.float32).copy()

    edges = cv2.Canny(image, 30, 80
                      )

    height, width= image.shape

    for y in range(height):
        for x in range(width):

            old_pixel= image[y, x]

            new_pixel = 255 if old_pixel >= 128 else 0

            image[y, x]= new_pixel

            error = old_pixel - new_pixel

            if x + 1 < width:
                image[y, x+1] += error * 7/16

            if y +1 < height:

                if x > 0:
                    image[y+1, x-1] += error * 3/16

                image[y+1, x] += error * 5/16

                if x +1 < width:
                    image[y+1, x+1] += error * 1/16

    return np.clip(image, 0, 255).astype(np.uint8)


displayed = 0

clahe= cv2.createCLAHE(clipLimit=2.0, 
                       tileGridSize=(4,4))

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

        height, width = frame.shape[:2]
        
        scale = min(WIDTH / width, HEIGHT / height)
        
        newWidth = int(width * scale)
        newHeight = int(height * scale)
        
        resized = cv2.resize(
                    frame,
                    (newWidth, newHeight),
                    interpolation=cv2.INTER_AREA
                )
        
        xOffset = (WIDTH - newWidth) // 2
        yOffset = (HEIGHT - newHeight) // 2
        
        canvas = np.zeros(
                    (HEIGHT, WIDTH, 3),
                    dtype=np.uint8
                )
        
        canvas[     yOffset:yOffset + newHeight,
                    xOffset:xOffset + newWidth
              ] = resized

        graysacle= cv2.cvtColor(canvas, cv2.COLOR_BGR2GRAY)
        
        enchanced = clahe.apply(graysacle)

        blurred= cv2.GaussianBlur(enchanced, (3,3), 0)

        binary = cv2.Canny(blurred, 30, 80)
        
        bits = (binary > 0).astype(np.uint8)

        packed = np.packbits(bits.reshape(8, 8 , 128).transpose(0, 2, 1), axis=2, bitorder='little')

        frameData= packed.flatten()

        if frameData.size != 1024:
            print("INVALID FRAME SIZE: ", frameData.size)
            break

        ser.write(frameData.tobytes())
        ser.flush()

        confirmation= ser.readline().decode(
        errors= 'ignore').strip()

        if confirmation != "FRAMEOK":
            print("FRAME TRANSFER FAILED!!!", confirmation)
            break

        displayed+=1

        print("FRAME: ",displayed)


        elapsed = time.monotonic() - starttime

        remaining = (1 / TARGETFPS) - elapsed

        if remaining > 0:
            time.sleep(remaining)

finally:

    vid.release()
    ser.close()

    print("\nPlayback finished.")
    print("Frames displayed:", displayed)