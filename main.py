import serial
import time
import cv2
import numpy as np
import io
import os

arduino = serial.Serial(port='COM7',  baudrate=115200, timeout=.1)

def convert_to_array():
    with open(r"C:\Users\myrza\OneDrive\Рабочий стол\Python\qwe.jpg", "rb") as file:
        file_bytes = file.read()

    # Convert the bytes to a NumPy array
    file_array = np.frombuffer(file_bytes, dtype=np.uint8)

    # Decode the image array using OpenCV
    image = cv2.imdecode(file_array, cv2.IMREAD_COLOR)

    a4_width = 70
    a4_height = 50
    resized_image = cv2.resize(image, (a4_width, a4_height))

    # Convert the resized image to grayscale
    gray_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)

    # Invert the grayscale image
    inverted = 255 - gray_image

    # Apply Gaussian blur to the inverted image
    blurred = cv2.GaussianBlur(inverted, (21, 21), 0)

    # Invert the blurred image
    inverted_blur = 255 - blurred

    # Divide the grayscale image by the inverted blurred image
    lined_image = cv2.divide(gray_image, inverted_blur, scale=1)

    # Convert lined_image to an array
    lined_array = np.array(lined_image)
    # print(len(lined_array))
    # print(len(lined_array[0]))

    # Save the array to a text file
    folder_path = r"C:\Users\myrza\OneDrive\Рабочий стол\Python"
    file_path = os.path.join(folder_path, "lined_image_array.txt")
    np.savetxt(file_path, lined_array, fmt="%d")

    print("Array saved as lined_image_array.txt")
    
    return lined_array

def write_read(x):
    arduino.write(bytes(x,  'utf-8'))
    time.sleep(0.01)
    data = arduino.readline() 
    return  data
arr = convert_to_array()

for i in range(70):
    for j in range(50):
        data_to_send = str(arr[i][j])  # Convert matrix element to string
        response = write_read(data_to_send)  # Send data to Arduino
        print("Sent:", data_to_send)
        print("Received:", response)