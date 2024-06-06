/** The MIT License (MIT)

Copyright (c) 2019 magnum129@github

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "TimeDB.h"

TimeDB::TimeDB(String apiKey)
{
  myApiKey = apiKey;
}

void TimeDB::updateConfig(String apiKey, String lat, String lon)
{
  myApiKey = apiKey;
  myLat = lat;
  myLon = lon;
}

time_t TimeDB::getTime()
{
  WiFiClient client;
  String apiGetData = "GET /v2.1/get-time-zone?key=" + myApiKey + "&format=json&by=position&lat=" + myLat + "&lng=" + myLon + " HTTP/1.1";
  Serial.println("Getting Time Data for " + myLat + "," + myLon);
  Serial.println(apiGetData);
  String result = "";
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println(apiGetData);
    client.println("Host: " + String(servername));
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection for time data failed"); //error message if no client connect
    Serial.println();
    return 20;
  }

  while (client.connected() && !client.available()) delay(1); //waits for data

  Serial.println("Waiting for data");

  boolean record = false;
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    if (String(c) == "{") {
      record = true;
    }
    if (record) {
      result = result + c;
    }
    if (String(c) == "}") {
      record = false;
    }
  }
  client.stop(); //stop client
  Serial.println(result);

  int timeStart = result.lastIndexOf('{'); // trim response to start of JSON -- issue 194
  result = result.substring(timeStart);
  char jsonArray [result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  DynamicJsonBuffer json_buf;
  JsonObject& root = json_buf.parseObject(jsonArray);
  localMillisAtUpdate = millis();
  Serial.println();
  if (root["timestamp"] == 0) {
    return 20;
  } else {
    return (unsigned long) root["timestamp"];
  }
}

String TimeDB::getDayName() {
  switch (weekday()) {
    case 1:
      return "Dom";
      break;
    case 2:
      return "Lun";
      break;
    case 3:
      return "Mar";
      break;
    case 4:
      return "Mer";
      break;
    case 5:
      return "Gio";
      break;
    case 6:
      return "Ven";
      break;
    case 7:
      return "Sab";
      break;
    default:
      return "";
  }
}

String TimeDB::getMonthName() {
  String rtnValue = "";
  switch (month()) {
    case 1:
      rtnValue = "Gen";
      break;
    case 2:
      rtnValue = "Feb";
      break;
    case 3:
      rtnValue = "Mar";
      break;
    case 4:
      rtnValue = "Apr";
      break;
    case 5:
      rtnValue = "Mag";
      break;
    case 6:
      rtnValue = "Giu";
      break;
    case 7:
      rtnValue = "Lug";
      break;
    case 8:
      rtnValue = "Ago";
      break;
    case 9:
      rtnValue = "Set";
      break;
    case 10:
      rtnValue = "Ott";
      break;
    case 11:
      rtnValue = "Nov";
      break;
    case 12:
      rtnValue = "Dic";
      break;
    default:
      rtnValue = "";
  }
  return rtnValue;
}


String TimeDB::getAmPm() {
  String ampmValue = "AM";
  if (isPM()) {
    ampmValue = "PM";
  }
  return ampmValue;
}

String TimeDB::zeroPad(int number) {
  if (number < 10) {
    return "0" + String(number);
  } else {
    return String(number);
  }
}
