//this file contains the functions used to display and select the station form the stations list



void displayStationsList(){
  if(triggerDisplayStations){
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.setTextFont(FONT2);
      for (int i = 0; i < STATIONS_LIST; i++){
        tft.drawString(stationName[i], 10, 20*i, FONT2);
      }
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString(stationName[stationIndex], 10, 20*stationIndex);
    // set text colour for departure rows
    }
    triggerDisplayStations = 0;// doit once

  }
  
void released(Button2& btn){
  if (mode == 0){
    listPosition++;
    Serial.print("ListPosition  "); Serial.println(listPosition);
  if (listPosition == STATIONS_LIST){
    listPosition = 0;
  }
  
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString(stationName[listPosition], 10, 20*listPosition);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (listPosition != 0){
    tft.drawString(stationName[listPosition - 1], 10, 20*(listPosition -1 ));
  }
  else {
    tft.drawString(stationName[STATIONS_LIST - 1], 10, 20*(STATIONS_LIST -1 )); 
  }
stationIndex = listPosition;
  }
}

void longClick(Button2& btn){
  mode = !mode;
  if (mode == 0) {//display stations list and scroll list - button released()
      triggerDisplayStations = 1;
  }
  if (mode == 1){//{button longClick() save station and return to display departures
        stringStationCRS = stationCRS[stationIndex];
        stringStationName = stationName[stationIndex];
        //Serial.print("station index : "); Serial.println(stationIndex);
        triggerDisplayDepartureHeadings = 1;
        displayDepartureHeadings();
        API_update_data();
  }
}

void displayDepartureHeadings(){
   //Display screen headings
  // Time    Destination   Plat Exp
      if(triggerDisplayDepartureHeadings){
      tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
      tft.drawString("Time", 0, 0, FONT2);
      tft.drawString("Destination", 50, 0, FONT2);
      tft.drawString("Plat", PlatHeader_Xpos, 0, FONT2);
      tft.drawString("Expected", 260, 0, FONT2);
      tft.drawCentreString(stringStationName, 160, 340, FONT4);
      // set text colour for departure rows
      tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    
  }
  
  triggerDisplayDepartureHeadings = 0;// doit once
}




