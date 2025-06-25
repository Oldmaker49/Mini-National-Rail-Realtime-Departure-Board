/* ReadStationList.ino 
Deals with reading and parsing two text files on SD card
Wifi.txt holds the Wifi SSID and password
myStations.txt holds the List of station names and associated CRS code
*/

// variables for parsing the station list data from SD card
String stationList = "";// holds station list from SD card in file "myStations.txt"
String WiFiDetails = "";// holds Wifi SSID and password from Sd card file "WiFi.txt"

//text file delimiter tags//////
//  in myStations.txt //
const String listCRStag = "<crs>"; // length 5
const String endListCRStag = "</crs>"; // length 6
const String stationNameTag = "<name>"; // length 6
const String endStationNameTag = "</name>"; // length 7
// in Wifi.txt //
const String ssidTag = "<ssid>";// start tag of network name length 6
const String endSSIDtag = "</ssid>";//end tag of network name 
const String pwTag = "<pw>"; // start tag of wifi password length 4
const String endPWtag = "</pw>"; // end tag of wifi password

  
  // open the "Wifi.txt" details  file for reading:
 void getWiFidetailsFromSD(){
  myFile = SD.open("WiFi.txt");
  if (myFile) {
    Serial.println("WiFi.txt:");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      //Serial.write(myFile.read());
      int inChar = myFile.read();
      if (inChar > 31){//ignore control characters
      WiFiDetails = WiFiDetails + (char)inChar;
      }
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening station WiFi.txt");
  }
  Serial.println(WiFiDetails);
  getWiFiCredentials();// extract the SSID and password from WiFidetails String
 }

// extract the SSID and Pawword from WiFi deatils String derived from within the "myStations.txt" file
void getWiFiCredentials(){
  //extract SSID from WifiDetails String
  long int SSIDposition = WiFiDetails.indexOf(ssidTag,0) + 6;
  long int endSSIDposition = WiFiDetails.indexOf(endSSIDtag,0);
  SSID = WiFiDetails.substring(SSIDposition,endSSIDposition);
  //extract Password from WifiDetails String
  long int passwordPosition = WiFiDetails.indexOf(pwTag,0) + 4;
  long int endPasswordPosition = WiFiDetails.indexOf(endPWtag,0);
  Password = WiFiDetails.substring(passwordPosition,endPasswordPosition);
  Serial.print(SSID); Serial.print(" : ");Serial.println(Password);
}

//Read the "myStations.txt" file from the SD card and hold in stationList String
  void getStationListFromSD(){
  myFile = SD.open("myStations.txt");
  if (myFile) {
    Serial.println("myStations.txt:");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      //Serial.write(myFile.read());
      int inChar = myFile.read();
      if (inChar > 31){//ignore control characters
      stationList = stationList + (char)inChar;
      }
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening myStations.txt");
  }
  Serial.println(stationList);
  
  getListCRS();
  getListStationNames();
  //Serial.print("Number of Stations:  "); Serial.println(getNumberOfStations());
  printStationList();
}


void getListCRS(){ // get the station CRS code for the data returned
  long int crsPosition = 0;
  long int endCRSposition = 0;
  long int searchPosition = 0;
  for(int i = 0; i < STATIONS_LIST; i++){
  crsPosition = stationList.indexOf(listCRStag, searchPosition) +5;
  endCRSposition =  stationList.indexOf(endListCRStag, searchPosition);
  stationCRS[i] = stationList.substring(crsPosition,endCRSposition);
  //Serial.print("crsPosition :"); Serial.print(crsPosition); Serial.print("  endCRSposition : ");Serial.println(endCRSposition);
  searchPosition = endCRSposition + 7 ;
  //Serial.print("searchPosition : ");Serial.println(searchPosition);
  }
}
void getListStationNames(){ // get the station CRS code for the data returned
  long int namePosition = 0;
  long int endNamePosition = 0;
  long int searchPosition = 0;
  for(int i = 0; i < STATIONS_LIST; i++){
  namePosition = stationList.indexOf(stationNameTag, searchPosition) + 6;
  endNamePosition =  stationList.indexOf(endStationNameTag, searchPosition);
  stationName[i] = stationList.substring(namePosition,endNamePosition);
  searchPosition = endNamePosition + 8 ;
  }
}
int getNumberOfStations(){
  int indexEndOfData = stationList.length();
  int indexCRStag;
  int numberOfStations = 0;
  int searchPosition = 0;
  int previousSearchPosition = 0;
  while (searchPosition < indexEndOfData){
    indexCRStag =  stationList.indexOf(listCRStag, searchPosition);
    searchPosition = indexCRStag + 6;
    if(searchPosition > previousSearchPosition ){
     numberOfStations++;
     previousSearchPosition = searchPosition;
    }
    else break;
  }
    if (numberOfStations < STATIONS_LIST){
        return numberOfStations;
    }
    else return STATIONS_LIST;
   //Serial.print("number of Services: "); Serial.println(numberOfServices);
} 

void printStationList(){
  for(int i = 0; i < STATIONS_LIST ; i++){
    Serial.print(stationName[i]); Serial.print(" : ");Serial.println(stationCRS[i]);
  }
  Serial.println("END OF PRINT STATION LIST");
  //Serial.print("SSID : ");Serial.print(SSID);Serial.print("Password : "); Serial.println(Password);
    
}


