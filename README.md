# M5AtomS3GPS
Code Pilot Request:

write code for the M5Stack Atom S3 and M3080-KT GPS that records NMEA data in GPX format (lat, long, elevation and time) in files saved by daily and named by the nearest Capital/State city in format YYYYMMDD-StateCity.GPX. 

Only record position data if unit moves more than 2m.

Write the code for upload in Arduino IDE 2.3.2

Enable the following functions:
1. Enable logging with power on
2. Enable toggle on/off with screen click
3. Enable message on screen when logging data showing Lat, Long, Elevation, Speed, battery %, and number of GPX files recorded this week, with a green border around the screen
4. When it has no satellite fix enable red border around the screen and a number depicting number of satellites connected, if none display and X
5. Side button toggles the display on/off
6. Allow a Wi-Fi connection to get NTP data and read the SD remotely
7. Allow a Bluetooth connection for iOS Bluetooth Terminal functions
