
/* 

FILE: DepartureBoard_PICO_V2.0
By: Oldmaker49
June 2025
MIT License
Copyright (c) 2025 Oldmaker49
*/

// Changes from V1.0
// Utilises the SD card reader on the Waveshare Pico-touch display board as follows:
// Read WiFi credentials from SD card within the "Wifi.txt" file
// Read Station list names and CRS codes from SD card winthin "myStations.txt" file
// the code to do this is in readStationList.ino

#include <string>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <TFT_eSPI.h> 
#include <SPI.h>
#include <SD.h>
#include <User_Setup.h> // Setup for the Pico-ResTouch-LCD-3.5 display: Use Resources/TFT_eSPI_User_Setup.h
#include "Free_Fonts.h"
#include <time.h>
#include <Button2.h>

// SD pin connections///////////////
// Use SPI1
const int SD_MISO = 12;
const int SD_MOSI = 11;
const int SD_SCK = 10;
const int SD_CS = 22;
/////////////////////////
File myFile;// for reading files on SD card

////////////////////////
// WIFI CREDENTIALS
String SSID = ""; // holds WiFi network name read from SD card
String Password = ""; // holds  WiFi pasword read from SD card
///////////////////////

// NATIONAL RAIL  DETAILS //////////////////
#define NATIONAL_RAIL_TOKEN "your-token-number" // National Rail Darwin Token
//#define NATIONAL_RAIL_CRS "BHM" // Station code: https://www.nationalrail.co.uk/stations_destinations/48541.aspx
#define NATIONAL_RAIL_ROWS 15 // DO NOT CHANGE THIS!! maximum number of departures to be displayed 
#define UPDATE_INTERVAL 30000 // Data update interval 30s

//defines for station select functions
#define STATIONS_LIST 24  // 24 is the maximum. This can be changed but ensure that the const String stationCRS[STATIONS_LIST] array has the correct number of entries
#define BUTTON_PIN 1 // GPIO pin of the button used to select and scroll the list of stations

unsigned long api_previous_check = -50000; // Force update immediately
int apiRetries = 0;
//int services_total = 0; // Total number of services returned
//int service_rounds = 0; // Keep count of how many loops a row has been shown
//int service_display = 1; // service_display, which service to display in the bottom row
int departureRows = NATIONAL_RAIL_ROWS;

//LCD Display constants
//WaveShre PICO touch 320 x 480 pixels - potrait orientation
// Plat Destination  Plat  Exp
int PlatHeader_Xpos = 220;
int Time_Xpos = 0;  // X position of start of Time columb
int Dest_Xpos = 50; // X position of Start of Destination column
int Plat_Xpos = 240; // X position of start of Platform column
int Exp_Xpos  = 260; // X position of start of Expected Column
int Disp_Width = 320;



// Strings for the departure data returned
String  depTime[NATIONAL_RAIL_ROWS]; // scheduled departure time
String  depDest[NATIONAL_RAIL_ROWS]; // departure destination
String  depPlat[NATIONAL_RAIL_ROWS]; // platform name or number - can be blank
String  depExp[NATIONAL_RAIL_ROWS];  // expected departure - "on-time", "delayed", "cancelled", or hh:mm

String XMLresponse;// contians only information related to train services derived fromm fullXMLresponse
String fullXMLresponse;// response from National Rail Web Server
String xmlCRS; // the station code for the data returned 
bool forceUpdate;// 1 if data update required immediately
//Time variables
struct tm timeinfo; // structure to hold the time iformation obtained from NTP server
char timeNow[6];
const char* TZ_UK = "GMT0BST,M3.5.0/1,M10.5.0";// rule for UK timezone GMT/BST adjustment
time_t t; // current UNIX time


WiFiClientSecure client;

TFT_eSPI tft = TFT_eSPI(); 

Button2 button;// invoke the button functions from button2.h

//variables for station select functions
bool firstTime;

int stationIndex = 0;//pointer to current position in the stations list

// STATION LIST ////////////
// define station names and associated CRS code
// the number of entries defined must match the value of STATIONS_LIST (default 24)
//const String stationCRS[STATIONS_LIST] =  {"BTH",     "BRI",                  "BHM",                  "CDF",            "CRE",  "EDB",      "EXD",             "GLC",            "LIV",              "EUS",          "PAD",              "STP",              "WAT",            "MAN",                  "NCL",      "NRW",     "NOT",       "OXF",   "RDG",    "RUG",  "SAL",      "SHF",      "SHR",       "YRK"};
//const String stationName[STATIONS_LIST] = {"Bath Spa","Bristol Temple Meads", "Birmingham New Street","Cardiff Central","Crewe","Edinburgh","Exeter St Davids","Glasgow Central","Liverpool Lime St","London Euston","London Paddington","London St Pancras","London Waterloo","Manchester Piccadilly","Newcastle","Norwich","Nottingham","Oxford","Reading","Rugby","Salisbury","Sheffield","Shrewsbury","York"};
String stationCRS[STATIONS_LIST];
String stationName[STATIONS_LIST];

int listPosition = 0;// the current position in the stations list
String stringStationCRS;// used within the message to obtain the real-time data for that station from the Nationl Rail web server
String stringStationName;// string containg the name of the station selected
bool triggerDisplayStations;// allows the station list to be displayed once
bool triggerDisplayDepartureHeadings;// allows the fixed departure board headings to be displayed once
int mode = 0; // 0  = display station list, 1 = save station and display departures 
bool triggerGetData;
//
// SUROUTINES ////////////////////



void displayTime(){// real-time cloch displayed at the bottonm of the display
  time(&t);
  localtime_r(&t, &timeinfo);
  Serial.print("Current time: ");
  Serial.println(asctime(&timeinfo));
  strftime(timeNow, 6, "%H:%M", &timeinfo);
  //timeNow = 
  Serial.println(timeNow);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(FONT7);
  tft.setTextPadding(tft.textWidth("     ", FONT7));
  tft.drawCentreString(timeNow, 160, 400, FONT7 );
  
}

void displayData(){// displays the rows for each departure 
          Serial.println("Updating Display");
          tft.setTextColor(TFT_ORANGE, TFT_BLACK);
          tft.setTextFont(FONT2);
          // Time     Destination   Plat  Expected 
          for(int i = 0; i < NATIONAL_RAIL_ROWS; i++ ){
          int row_Ypos = (i + 1) * 20;
          tft.setTextPadding(Dest_Xpos - Time_Xpos);
          tft.drawString(depTime[i], Time_Xpos, row_Ypos, FONT2);
          //Serial.print(depTime[i]);Serial.print("  ");
          tft.setTextPadding(PlatHeader_Xpos - Dest_Xpos);
          tft.drawString(depDest[i], Dest_Xpos, row_Ypos, FONT2);
          //Serial.print(depDest[i]);Serial.print("  ");
          tft.setTextPadding(Exp_Xpos - PlatHeader_Xpos );
          tft.drawRightString(depPlat[i], Plat_Xpos, row_Ypos, FONT2);
          //Serial.print(depPlat[i]);Serial.print("  ");
          tft.setTextPadding(Disp_Width - Exp_Xpos);
          tft.drawString(depExp[i], Exp_Xpos, row_Ypos, FONT2);
          //Serial.println(depExp[i]);
          }
}

void printData(){
  for (int i = 0; i < departureRows; i++){
  Serial.print(depTime[i]); Serial.print("  "); Serial.print(depDest[i]); Serial.print("  "); Serial.print(depPlat[i]); Serial.print("  ");Serial.println(depExp[i]);
 }
}

void setClock() {// initialise the NTP clock
  NTP.begin("pool.ntp.org", "time.nist.gov");
  setenv("TZ",TZ_UK, 1);
  tzset();
  Serial.print("Waiting for NTP time sync: ");
  time(&t);
  while (t < 1748736000) {// 1 June 2025
    delay(500);
    Serial.print(".");
    time(&t);
  }
  Serial.println("");
  localtime_r(&t, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

 void stringReserve(){// the Strings for the XMl response returned can be large - 
  fullXMLresponse.reserve(18000);
  XMLresponse.reserve(18000);
  
  for (int i = 0; i < NATIONAL_RAIL_ROWS; i++){
    depDest[i].reserve(30);
    depTime[i].reserve(8);
    depExp[i].reserve(12);
    depPlat[i].reserve(8);
  }
 }

 
// Does the device have a working wifi connection?
bool Network_wifi_check() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  } else {
    return true;
  }
}

void displayNetworkStatus(){
  if (mode == 1){
    if(Network_wifi_check()){
      tft.setTextColor(TFT_GREEN); tft.setTextFont(FONT4);
      tft.drawString("!",10,460,FONT4);
      }
      else{
        tft.setTextColor(TFT_ORANGE); tft.setTextFont(FONT4);
        tft.drawString("!",10,460,FONT4);
      }
  }
}
void setup() {
  Serial.begin(115200);
  delay(500);
  ////setup SD Card/////////////////////
  bool sdInitialized = false;
      SPI1.setRX(SD_MISO);
      SPI1.setTX(SD_MOSI);
      SPI1.setSCK(SD_SCK);
  Serial.print("Initializing SD card...");
  sdInitialized = SD.begin(SD_CS, SPI1);
  if (!sdInitialized) {
    Serial.println("initialization failed!");
    return;
  }
  delay(1000);
  getWiFidetailsFromSD();
  getStationListFromSD();
  SD.end();
  ////////////////////////
  /////Set up TFT display////
  tft.begin();
  tft.setRotation(2);// Portrait
  //Turn on LCD baklight
  pinMode(TFT_BL, INPUT);
  digitalWrite(TFT_BL, LOW);
  tft.fillScreen(TFT_BLACK);
  Serial.print("Connecting to ");
  tft.setTextFont(FONT4);
  tft.setTextColor(TFT_ORANGE,TFT_BLACK);
  tft.drawCentreString("Connecting.....", 160, 240, FONT4);
  
  //////////////////////////////////////////////////////
  //// Set up WiFi  ////////////////////////////////
    delay(500);
     //Serial.print("SSID : ");Serial.print(SSID);Serial.print("Password : "); Serial.println(Password);
    WiFi.begin(SSID.c_str(), Password.c_str());
    
    Serial.println(SSID);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    //Serial.println("IP address: ");
  // SSL setup
  client.setInsecure();
  ///////////////////////////////////
  ///////////Connect to NTP server////////////
  setClock();
  ////////////////////////////////////////////

  tft.fillScreen(TFT_BLACK);
///// define the button functions////
  button.begin(BUTTON_PIN);
  button.setLongClickDetectedHandler(longClick);// long press to enter/leave station select mode
  button.setLongClickTime(500);
  button.setClickHandler(released);// short click to scroll list of stations
//////////////////////////////////

  //display list of stations on startup and enter station select mode (mode = 0) /////
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextFont(FONT2);
  for (int i = 0; i < STATIONS_LIST; i++){
    tft.drawString(stationName[i], 10, 20*i, FONT2);
  }
  // highlight first station in list in Orange text
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  listPosition  = 0;// current position in list
  tft.drawString(stationName[listPosition], 10, 20*listPosition);

  firstTime = 1;
  mode = 0;// display station list on startup
  
}

void loop() {
     button.loop();// check pushbutton state
     //displayNetworkStatus();// use this if you want to display an indication of the status of the network connection
                              // at the bottom of the departure display screen

     if(!firstTime && mode == 1){
        
        displayDepartureHeadings();

      unsigned long currentMillis = millis();
        if (((currentMillis - api_previous_check) >= UPDATE_INTERVAL) || forceUpdate){
          
          api_previous_check = currentMillis;
          API_update_data();// 
          
        }
     }

     if(firstTime){
      triggerDisplayStations =1;
      firstTime = 0;
      }
      if(firstTime || mode == 0){
      displayStationsList();
      }

     }
