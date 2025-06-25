//api.ino
// deals with geting the  real-time data and extract the departure dta for the train services

//XML tags used to separate out the data to display
const String servicesTag = "<lt8:trainServices>";//length 19 - Start of data 
const String serviceTag = "<lt8:service>";//length 13 - start of each train service data
const String stdTag = "<lt4:std>"; //length 9 - start of the sheduled departure time for a service
const String endStdTag = "</lt4:std>";//length 10 - end of 
const String etdTag = "<lt4:etd>"; //length 9
const String endEtdTag = "</lt4:etd>";//length 10
const String destTag = "<lt5:destination><lt4:location><lt4:locationName>"; // length  49
const String endDestTag = "</lt4:locationName><lt4:crs>";// length 28
const String platTag = "<lt4:platform>";//length 14
const String endPlatTag = "</lt4:platform>";//length 15 
const String endOfServiceTag = "</lt8:service>";
const String endOfTrainServicesTag = "</lt8:trainServices>";//length 20
const String endSoapEnvelopeTag = "</soap:Envelope>";
const String crsTag = "<lt4:crs>"; //length 9
const String endOfCRSTag  = "</lt4:crs>"; //length 10

///////Sunroutines to extract departure data from the Real-time feed
// Routine to determine the number of services to display. The number of services can be less than [NATIONAL_RAIL_ROWS]
// This can happen for stations with infrequent services or during the night when there are fewer departures
int getDepartureRows(){
  int indexEndOfData = XMLresponse.length();
  int indexServiceTag;
  int numberOfServices = 0;
  int searchPosition = 0;
  int previousSearchPosition = 0;
  while (searchPosition < indexEndOfData){
    indexServiceTag =  XMLresponse.indexOf(serviceTag, searchPosition);
    searchPosition = indexServiceTag + 13;
    if(searchPosition > previousSearchPosition ){
     numberOfServices++;
     previousSearchPosition = searchPosition;
    }
    else break;
  }
    if (numberOfServices < NATIONAL_RAIL_ROWS){
        return numberOfServices;
    }
    else return NATIONAL_RAIL_ROWS;
   //Serial.print("number of Services: "); Serial.println(numberOfServices);
} 

void getCRS(){ // get the station CRS code for the data returned
  long int crsPosition = fullXMLresponse.indexOf(crsTag) + 9;
  long int endCRSposition =  fullXMLresponse.indexOf(endOfCRSTag);
  xmlCRS = fullXMLresponse.substring(crsPosition,endCRSposition);
}
void getSTD(){ // gets the sheduled departure time for each service
   long int stdPosition = XMLresponse.indexOf(servicesTag);
   int i;
  for( i = 0 ; i < departureRows; i++){
    long int indexStd = XMLresponse.indexOf(stdTag,stdPosition) + 9;
    long int indexEndStd = XMLresponse.indexOf(endStdTag,stdPosition);
    depTime[i] = XMLresponse.substring(indexStd,indexEndStd);
    stdPosition = indexEndStd + 11;
    //Serial.print("row:"); Serial.print(i); Serial.print(" STD: ");  Serial.println(depSTD[i]);
  }
  for (i = departureRows; i < NATIONAL_RAIL_ROWS; i++){
      depTime[i] = " "; // blank the data for any unused rows
    }
}

void getETD(){// Expected departure - one of - "On time", "Delayed", "Cancelled", "hh:mm"
  long int etdPosition = XMLresponse.indexOf(servicesTag);
  int i;
  for( i = 0 ; i < departureRows; i++){
    
    long int indexEtd = XMLresponse.indexOf(etdTag,etdPosition) + 9;
    long int indexEndEtd = XMLresponse.indexOf(endEtdTag,etdPosition);
    depExp[i] = XMLresponse.substring(indexEtd,indexEndEtd);
    etdPosition = indexEndEtd + 11;
  }
   for (i = departureRows; i < NATIONAL_RAIL_ROWS; i++){
      depExp[i] = " ";
    }
}

// gets the platform number or name - Deals with the instance when a service is cancelled and no platorm XMl tags
// that deliniate the plaform string exist.
  void getPlat(){
  long int searchPosition = XMLresponse.indexOf(servicesTag);
  long int serviceStartPos = 0;
  long int serviceEndPos = 0;
  int i;
  for( i = 0 ; i < departureRows; i++){
    long int indexStartOfService = XMLresponse.indexOf(serviceTag, searchPosition) + 13; 
    long int indexEndOfService = XMLresponse.indexOf(endOfServiceTag, indexStartOfService);
    long int indexPlat = XMLresponse.indexOf(platTag, indexStartOfService) + 14;

    if (indexPlat > indexStartOfService && indexPlat < indexEndOfService){// platform for this service
    long int indexEndPlat = XMLresponse.indexOf(endPlatTag,indexPlat);
    depPlat[i] = XMLresponse.substring(indexPlat,indexEndPlat);
    searchPosition = indexEndOfService + 13 ;
    }
    else{
    depPlat[i] = " ";
    searchPosition = indexEndOfService + 13;
    };
    //Serial.print("row:"); Serial.print(i); Serial.print(" Plat: ");  Serial.println(depPlat[i]);
  }
  for (i = departureRows; i < NATIONAL_RAIL_ROWS; i++){
    depPlat[i] = " ";
    }
}

void getDest(){// gets the Departure Destination string
  long int destPosition = XMLresponse.indexOf(servicesTag);
  int i;
  for( i = 0 ; i < departureRows; i++){
    long int indexDest = XMLresponse.indexOf(destTag,destPosition) + 49;
    //Serial.print("indexDest : "); Serial.println(indexDest);
    destPosition = indexDest;
    long int indexEndDest = XMLresponse.indexOf(endDestTag,destPosition);
    depDest[i] = XMLresponse.substring(indexDest,indexEndDest);
    destPosition = indexEndDest + 28;
    //Serial.print("row:"); Serial.print(i); Serial.print(" Dest: ");  Serial.println(depDest[i]);
  }
  for(i = departureRows; i < NATIONAL_RAIL_ROWS; i++){
      depDest[i] = " ";
    }
}

bool ifServices(){
  long int XMLlength = XMLresponse.length();
  long int indexServices = XMLresponse.indexOf(servicesTag);
  if (indexServices < XMLlength){
    return true;
  }
  else{return false;}
}

//  Routine to get the data from the server and extract the data
void API_update_data() {
  HTTPClient https;
  
  Serial.println("Getting data update");
  https.begin(client, "https://lite.realtime.nationalrail.co.uk/OpenLDBWS/ldb12.asmx");
  https.addHeader("Content-Type", "text/xml");
  
   int httpResponseCode = https.POST("<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:typ=\"http://thalesgroup.com/RTTI/2013-11-28/Token/types\" xmlns:ldb=\"http://thalesgroup.com/RTTI/2021-11-01/ldb/\"><soapenv:Header><typ:AccessToken><typ:TokenValue>" + String(NATIONAL_RAIL_TOKEN) + "</typ:TokenValue></typ:AccessToken></soapenv:Header><soapenv:Body><ldb:GetDepartureBoardRequest><ldb:numRows>" + String(NATIONAL_RAIL_ROWS) + "</ldb:numRows><ldb:crs>" + stringStationCRS + "</ldb:crs></ldb:GetDepartureBoardRequest></soapenv:Body></soapenv:Envelope>");
  

  //Serial.print(httpResponseCode);Serial.println(httpResponseCode);
  Serial.println(httpResponseCode);
  // If successful...
  if (httpResponseCode == 200) {
    
    // Get response and end connection
      fullXMLresponse = https.getString();// Full response from server
    //Serial.println(response.length());
    //Serial.println(response);
      https.end();
      client.stop();
    // extract sub String of data required for trainservice departures
      long int XMLendIndex = fullXMLresponse.length();
      long int indexStartTrainServices = fullXMLresponse.indexOf(servicesTag); //start of trainServices data in the repsponse 
      long int indexEndTrainServices = fullXMLresponse.indexOf(endOfTrainServicesTag) + 20;// end of the trainSErvice data in the resposnse
      XMLresponse = fullXMLresponse.substring(indexStartTrainServices, indexEndTrainServices);// extract the trainServices data
    //
      getCRS();// get the CRS ( station code) of the data returned
      Serial.print("xmlCRS  : ");Serial.print(xmlCRS); Serial.print("  ");Serial.print("stringStationCRS  : ");Serial.println(stringStationCRS);
      fullXMLresponse = "";//delete the full XML string 

    // check that data string is complete
        if(XMLresponse.endsWith(endOfTrainServicesTag) && xmlCRS.equals(stringStationCRS)){// check that the traiservices data string are valid
    //Serial.println(XMLresponse);
        Serial.print("XMLresponse length: "); Serial.println(XMLresponse.length());
        Serial.println("Data OK - Updating");
    //int n = getNumberOfServices();
    //Serial.print("number of services: "); Serial.println(getNumberOfServices());
          departureRows = getDepartureRows(); // ontain the number of departures in the data - can be less that the number requested (default 15)
          Serial.print("number of Services: "); Serial.println(departureRows);
          getSTD();
          getDest();
          getPlat();
          getETD();
          XMLresponse = "";// null the response String
          displayData();
          printData();
          displayTime();
          forceUpdate = 0;
          apiRetries = 0;
          
    }
    else apiRetry();// if string is imcomplete then try again
    }
    
    XMLresponse = "";// delete string
 
  
}//end of API_updateData

void apiRetry(){
  if(apiRetries < 4){// limit of 3 retries
    forceUpdate =1;
    apiRetries++;
    }
    else forceUpdate = 0;
}
