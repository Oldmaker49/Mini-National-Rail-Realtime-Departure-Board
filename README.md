## Mini-National-Rail-Realtime-Departure-Board
Looks like a typical UK rail departure board. Uses 3.5inch TFT display and Raspberry Pi Pico 2W to display real-time departures for any station. 
National Rail Departure Board showing real-time departure information from any rail station within Great Britain on a 3.5 inch 320 x 480 pixel LCD display.

![image](https://github.com/user-attachments/assets/7fcf3950-94f7-4306-8c55-6519d9cb9ecf)
![image](https://github.com/user-attachments/assets/7ed9f180-e11c-4af3-85b8-e0fb625a512c)

     Figure 1                                               Figure 2

## Overview
•	Designed to emulate the look of a typical station departure board (Figure 1)
•	Pulls data from National Rail's LDB Webservice data feed
•	Departures for up to 15 services can be displayed
•	The station can be selected from a user defined list of 24 stations (Figure 2)
•	The display also shows the current time derived from NTP server
•	The user’s Wifi SSID and Password are held on the SD card in a text file “WiFi.txt"
•	The user’s list of stations is held in on the SD card in a text file “myStations.txt”. 

## Hardware Requirements
Raspberry Pi Pico 2W microprocessor
Waveshare 3.5inch Pico-Res Touch 480 x 320 display https://www.waveshare.com/pico-restouch-lcd-3.5.htm
Push-button (n.o. spst) for scrolling and selecting the Station from the Station List. The button is soldered to the GPIO 01 pad and the GND pad on the reverse of the display board beneath the Pico plug-in connector as shown in Figure 3 below

![image](https://github.com/user-attachments/assets/1f895425-a748-40cc-a189-f85fac3d39f5)

      Figure 3

## Software Requirements 
Arduino IDE

## Obtaining access to UK National Rail data feed
A user token is required to access the National Rail data feed.
To get the token: Visit: https://www.nationalrail.co.uk/100296.aspx
Scroll down to the Darwin Data Feeds table. Find the row labelled "LDB Webservice (PV)" and click register here. Follow the registration steps. Wait for a bit and you should get an email confirming your account is activated. The email also contains your token.

## Code Setup
Compiled with Arduino IDE 2.3.6 and Raspberry Pi Pico https://github.com/earlephilhower/arduino-pico v4.5.1
Required Libraries: TFT_eSPI graphics library v2.5.4 by Bodmer : https://github/bodmer.TFT_eSPI
 The User_Setup file (TFT_eSPI_User_Setup.h) in the resources folder should be saved as User_Setup.h in the TFT_eSPI directory.
Button2 by Lennart Hennigs v2.3.3: https://github.com/LennartHennigs/Button2
Plus: WiFi.h; WiFiClient.h; WiFiClientSecure.h; SD.h

Modify the following options within the departureboad_PICO.ino file to suit. NATIONAL_RAIL_TOKEN .- your individual token number.

The User’s Wifi SSID and Password are held on an SD card in a file “WiFi.txt”
The individual station codes (CRS) and Station Names need to be defined in a text file “myStations.txt” on the SD card for each of the station options to be available to be selected for display. 24  stations must be  listed. Example files can be found in the SDfiles folder

## Operating Instructions
Before use the text files ”myStations.txt” and “Wifi.txt” must be created and stored on an SD card (max 32GB) – see User Manual - then plugged into the SD card slot on the display board. Connect the power supply. While connecting to WiFi the display shows “Connecting.. “ . Once connected the list of Stations previously entered into the code will be displayed. The current selection is highlighted. <short-click> the button to scroll down the list and <long-click> to select the station required. The departures for that station will then be displayed. <long-click> to return to the Station List and make another selection.
For more details see the User Manual V2-0 in the Resources folder.
Note: the number of departures displayed will be determined by the number of departures in a 2.5 hour time period from the current time and can be less than 15 or sometimes none when departures are infrequent.

