import cv2
import numpy as np
import tkinter as tk


def getFrameRGB():
    # gets video and counts total frames
    video = cv2.VideoCapture("video.mp4") # ADD CORRECT VIDEO PATH HERE
    frameCount = video.get(cv2.CAP_PROP_FRAME_COUNT)

    # for storing the rgb values
    frameRGBs = []


    # goes through each frame and gets one rgb value per frame
    for i in range(int(frameCount)):
        video.set(cv2.CAP_PROP_POS_FRAMES, i)
        flag, frame = video.read()

        if flag:
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            meanRgb = frame.mean(axis=(0,1))
            meanRgbInt = meanRgb.round().astype(int)
            frameRGBs.append(meanRgbInt)
        else:
            print("Could not decode video frame.")


    # converts rgb values to hex
    hexArray = []
    for i in frameRGBs:
        hex = f"#{i[0]:02x}{i[1]:02x}{i[2]:02x}"
        hexArray.append(hex)

    return hexArray


def main():
    np.set_printoptions(threshold=np.inf)

    hexArray = getFrameRGB()

    # sets up the window
    root = tk.Tk()
    root.title("Colours of Motion")
    root.geometry(f"{len(hexArray)*5}x500")
    root.resizable(False, False)

    # makes the canvas for the colours
    canvas = tk.Canvas(root, height=500, width=f"{len(hexArray)*5}")
    canvas.pack(side=tk.LEFT)
    
    # creates a rectangle for each colour
    # (however many frames are present = amount of rectangles)
    for i, hex in enumerate(hexArray):
        canvas.create_rectangle(i * 5, 0, (i * 5) + 5, 500, fill=hex, outline="")

    # shows window
    root.mainloop()


if __name__ == "__main__":
    main()