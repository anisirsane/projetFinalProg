/*
  Titre      : projet finale programmation embarque et web
  Auteur     : Anis Irsane
  Date       : 09-04-2023
  Description: un prjet ou on recupere les donnees depuis un BME280 en utilisant le protocole I2C
               -on les envoie sur une page html qui est sur un serveur locale
               - on fait les mises a jour des firmware avec OTA
               -l'envoie des donnees est avec le protocole HTTP sou forme d'un objet JSON
               -recuperation des donnees avec ajax et affichage sur notre page html
               -la page html est ecrit avec le html, css, bootstrao et javascript
               - la page est mise a jour chaque 3 secondes .
  Version    : 0.0.1
*/

//libraries necessaires
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <AsyncElegantOTA.h>

//partie de declaration des variables
#define SEALEVELPRESSURE_HPA (1013.25)

// Declaration de notre objet BME
Adafruit_BME280 bme; // I2C

//configuration de wifi
//const char* ssid = "UNIFI_IDO1";
//const char* password = "42Bidules!";
const char* ssid = "Ste-adele";
const char* password = "allo1234";

//declaration des variables
float temperature, pressure,humidity, altitude ; 
unsigned long PreviousTime = 0; 
const int DELAY = 3000;

//lancement de notre serveur
AsyncWebServer server(80);

// Declaration de la constante qui contient la page HTML 
const char fichierhtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        
    <meta http-equiv="X-UA-Compatible" content="IE=edge">  <!-- required to handle IE -->        
    <meta name="viewport" content="width=device-width, initial-scale=1">   
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-rbsA2VBKQhggwzxH7pPCaAqO46MgnOM80zW1RWuH61DGLwZJEdK2Kadq2F9CUG65" crossorigin="anonymous">  
  </head>
  <body>

  <div class="firstimage text-center row">
      <div class="row firstrow pb-4 mb-2 ml-0 mr-0">
        <h3>BME280 WEB SERVER</h3>
      </div>

      <div class="row pb-4 ">
        <div class="rowdata">
          <div class="row pb-2 justify-content-md-center ">

            <div class="col-sm" >

              <h5 class="tempval"><b>TEMPERATURE</b></h5>

            </div>
          </div>
          <div class="col-sm">
            <h2 class="tempval" id="tempval">28.51</h2>
          </div>          
        </div>

    </div>






    <div class="row pb-4 ">
      <div class="rowdata">
        <div class="row pb-2 justify-content-md-center ">
          <div class=" col-sm">

            <h5 class="altval"><b>ALTITUDE</b></h5>

          </div>
        </div>
        <div class="col-sm">
          <h2 class="altval" id="altval">22.85</h2>
        </div>          
      </div>
    </div>
      <div class="row pb-4 ">
        <div class="rowdata">
          <div class="row pb-2 justify-content-md-center " >

            <div class=" col-sm" >

              <h5 class="humidityval"><b>HUMIDITY</b></h5>

            </div>
          </div>
          <div class="col-sm">
            <h2 class="humidityval" id="humidityval">59.44</h2>
          </div>          
        </div>

    </div>

    <div class="row pb-4 ">
      <div class="rowdata">
        <div class="row pb-2 justify-content-md-center " >

          <div class="col-sm">

            <h5 class="preval"><b>PRESSURE</b></h5>

          </div>
        </div>
        <div class="col-sm">
          <h2 class="preval" id="preval">1003.62</h2>
        </div>          
      </div>
    </div>

  </div>

  <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-kenU1KFdBIe4zVF0s0G1M5b4hcpxyD9F7jL+jjXkk+Q2h455rYXK/7HAuoJl+0I4" crossorigin="anonymous"></script>
  <script >
    // fonction pour recuperer les donnees et les afficher sur notre forme
    function updateData() {
      $.ajax({
        url: '/data',
        type: 'GET',
        dataType: 'json',
        success: function(data) { 
          $('#tempval').text(data.temperature+" °C");
          $('#humidityval').text(data.humidity+" %");
          $('#preval').text(data.pressure + " hpa");
          $('#altval').text(data.altitude + " m");  
        }
      });
    };
    
    // update the form every 3 seconds
    setInterval(updateData, 3000);
    
    </script>
    <style>
      .firstimage{
	width: 400px;
	border:1px solid grey;
	border-radius: 20px;
	position:absolute;
	top:20%;
	left:35vw;
}
@media (max-width:575.98px){
	.firstimage{
		top:10%;
		left:10vw;
	}
}
.firstrow{
	background:#481e40;
	color:white;
	border-radius: 20px 20px 0 0;
	margin:auto 0 auto 0;
}
.tempval{
	color:#28716a;
}
.humidityval{
	color:#57b9b2;
}
.preval{
	color:#49c555;
}
.altval{
	color:#c51841;
}
.rowdata{
	max-width: 80%;
	position:relative;
	top:1em;
	left:2.5em;

    box-shadow:0px 0px 5px 5px #dddddd;
}
.wind{
	width:30px;
	height:auto;
}
    </style>


  
</body>
</html>)rawliteral"; 
//fin de notre page html

void setup() {

  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  //configuration de wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {   // Wait for connection
    delay(500);
    Serial.print("-");
  }
  Serial.println(WiFi.localIP());

  //lancement et detection bme280
  bool status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  
  }

  //sources : https://randomnerdtutorials.com/esp32-async-web-server-espasyncwebserver-library
  //https://raphaelpralat.medium.com/example-of-json-rest-api-for-esp32-4a5f64774a05#:~:text=Once%20connected%2C%20display%20the%20ESP32%20IP%20address%2C%20it,%28AsyncWebServerRequest%20%2Arequest%29%20%7B%20request-%3Esend%20%28200%2C%20%22application%2Fjson%22%2C%20%22%20%7B%22message%22%3A%22Welcome%22%7D%22%29%3B
  //affichage de notre page html sur le url "/"
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/html", fichierhtml);  // chargement page html
    });
  //envoie de notre objet json sur le url "/data"
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
      String json = "{\"temperature\":" + String(temperature) + ",\"pressure\":" + String(pressure) + ",\"altitude\":" + String(altitude) + ",\"humidity\":" + String(humidity) + "}";

    request->send(200, "application/json", json);
  });

  // Start serveur ElegantOTA
    AsyncElegantOTA.begin(&server); 
  
  // Lancement de serveur
    server.begin();
    Serial.println("HTTP server started");

}
void loop() {

  //on lit les donnees depuis le bme280 chaque 3 seconde 
  if (millis() - PreviousTime > DELAY)
  {
    temperature = bme.readTemperature();
    pressure = bme.readPressure() / 100.0F;
    altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    humidity = bme.readHumidity();
    PreviousTime = millis();
  }
  
}
