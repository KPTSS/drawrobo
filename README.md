# drawrobo
Drawing robot that is based on arduino

We take the AI generated image or image from any source and with the help of python and libraries, we created countour-lined version of the image. Then the created lined image is saved as the array and then sent to the main arduino using serial connection (by using serial library).

In terms of the transmitter, this is a transmitter which uses the values of the joystick and sends data with serial connection to the main arduino by using NRF24L01 module and using RF24 libraries in both transmitter and receiver (file omniwheelsrobot). Joystick sends the values of button and xy values. For button clicks, there are 3 main states where:
State 0: indicates that the robot should be off.
State 1: Joystick mode. With this state we can control the robot using the xy values of the joytick. The xy values are also send by the serial connection.
State 2: Drawing mode. Here, robot switches to drawing mode and stars drawing.

And in the main code (omniwheelsrobot), we accept the data from python using serial connection in 115200 baudrate (we opened it as Serial1) and save it to our main matrix. Then in the main loop, we atart receving states as well as the xy valeus of the joystick if the state is on 1st one. The drawing algorithms is simple and just goes through the rows of matrix one by one.
