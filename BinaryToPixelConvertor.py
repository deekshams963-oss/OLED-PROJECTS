import cv2
import numpy as np

vid=cv2.VideoCapture("video.mp4")

vid.set(cv2.CAP_PROP_POS_MSEC, 2000)

success, frame = vid.read()

if success:

    resized = cv2.resize(frame, (128, 64))
    grayscaled = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)
    threshold, binary = cv2.threshold(grayscaled, 0, 255, cv2.THRESH_BINARY+ cv2.THRESH_OTSU)
    bits = (binary>0).astype(np.uint8)
    packed = np.packbits(bits, axis=1, bitorder='big')
    frameData=packed.flatten()
    frameData.tofile("frame.bin")

    print("Original pixels: ", bits.shape)
    print("Packed shape: ", packed.shape)
    print("Total bytes: ", len(frameData))

    unpacked = np.unpackbits(packed, axis=1, bitorder='big')
    reconstructed = unpacked[:, :128] * 255

    cv2.imwrite("Recontructed.jpg", reconstructed)

else:
    print("ERROR! COULD NOT FETCH VIDEO.")

vid.release()
    