/**
 * 
 * Cloud Car Monitor
 *
 * Revo Project revoproject@terralink.cl
 * Terralink SpA, Santiago, Chile
 * 
 * CODE FOR KIA OBD Prototype 1
 * 
 * Collaborators:
 * Vicente Matus
 * Andres Astudillo
 *
 * 
 * 
 * OBD-II -- ELM327 -- BT -- BT -- NANO -- SIM800L
 * 
 * Online Utilities:
 * HEX to Binary: https://www.binaryhexconverter.com/hex-to-binary-converter
*/

/***********************
 ****** Libraries ******
 ***********************/

//#include <gprs.h>
#include <SoftwareSerial.h>

/*******************************
 ****** Runtime Variables ******
 ******************************/

unsigned long timestamp           = 0;    //to measure time elapsed getting OBD data
unsigned long time_elapsed        = 0;    //to measure time elapsed getting OBD data
int           time_between_loops  = 5000; //total time elapsed on each iteration in [ms]

#define baud_serial0 9600           //Serial inncluded in arduino
//#define baud_serial1 9600           //SIM
#define baud_serial2 38400          //BT-OBD

//SoftwareSerial SIM_serial(8,7);     //SIM
SoftwareSerial BTOBD_serial(2,3);   //BT-OBD

/**************************
 ****** OBD Variables *****
 **************************/

//boolean ECU_on = false;         //Engine Control Unit's state
String  raw_ELM327_response = ""; //to save OBD's response after sending commands
byte    inData;                   //to parse data received from OBD
char    inChar;                   //to read OBD's response char-wise
String  WorkingString="";         //to cut substrings from raw_ELM327_response
String  cod="";                   //to save DTC
long    A;                        //to save numeric data gotten from control unit's responses
long    B;
//long    C;

float   temp;              
float   rpm;               
float   veloc;     
float   ndtc1;
float   ndtc2;


/*****************************************
 ******** SIM and Cloud Variables ********
 *****************************************/
/*
#define DEFAULT_TIMEOUT     5
char server[] = "api.thingspeak.com";     //Server's address
String WriteAPIKey = "AEYR0MF2O3Y512QQ";  //Thingspeak channel key to write data
//String channel_ID = "369437";             //Thingspeak channel ID
String thingspeak_command = "";           //GET command with fields data (defined after getting OBD data)
//char buffer[512];   
GPRS gprs;                                //SIM808 object
//boolean connectivity = false;             //to attempt connection to the cloud or skip and work oflin
*/

void setup() {
  /*****************************************
   *****************************************
   ****************  SETUP  ****************
   *****************************************
   *****************************************/
  
  /*** Begin serial: */
  Serial.begin(baud_serial0);       
  Serial.println("Initializing Cloud Car Monitor System");
  //SIM_serial.begin(baud_serial1);
  //gprs.serialDebug();
  /*
  gprs.preInit();
  while(0 != gprs.init()) {
     delay(1000);
     Serial.println("2G initialization error. Check SIM card.");
  }
  */
  
/*
  gprs.sendCmdAndWaitForResp("AT+CFUN=1\r\n","OK",DEFAULT_TIMEOUT);
  delay(1000);
  gprs.sendCmdAndWaitForResp("AT+CGATT?\r\n","OK",DEFAULT_TIMEOUT);
  gprs.sendCmd("AT+CIPSTATUS\r\n");
  delay(1000);
  gprs.sendCmd("AT+CSTT=\"BAM.ENTELPCS.CL\",\"ENTELPCS\",\"ENTELPCS\"\r\n");
  delay(1000);
  gprs.sendCmdAndWaitForResp("AT+CIICR\r\n","OK",DEFAULT_TIMEOUT);
  gprs.sendCmd("AT+CIFSR\r\n");
  delay(1000);
  //char* IP = gprs.getIPAddress();

  Serial.println("2G Initialized.");
  //Serial.println(IP);
  
 /******************************
 *** Initialize Bluetooth HC 05
 ******************************/  
  BTOBD_serial.begin(baud_serial2);
  Serial.println("Bluetooth communication to ELM327 Initialized");
  
 /*********************
 *** Initialize ELM327
 *********************/  
 
  BTOBD_serial.println("atz");          //Reset
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("at sp 0");      //Automatically choose OBD protocol
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("at dp");        //Describe Protocol
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);

  BTOBD_serial.println("at dpn");       //Describe Protocol Number
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("09 02");           //Request Vehicle Information
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);  //Obtain VIN number substring and hex to cha
  
  BTOBD_serial.println("01 01");           //Request Mode 01 (Number of DTCs)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  //if non zero, request DTCs
  WorkingString = raw_ELM327_response.substring(11,13);              //Cut Byte with DTC number
  ndtc1 = strtol(WorkingString.c_str(),NULL,16);                     //Convert to integer
  ndtc2 = ndtc1-128;                                                 //Number of DTCs
  Serial.print("Number of DTC's: ");Serial.println(ndtc2);
  
  if(0 != ndtc2){
    BTOBD_serial.println("03");                                      //Request Mode 03 (List of DTCs)
    delay(500); read_elm327_response(); Serial.println(raw_ELM327_response); //Increase delay?
    for(int j = 8; j <= (ndtc2*6+6); j = j+6){                     //First hex digit
      WorkingString = raw_ELM327_response.substring((j+1),(j+4));              //Cut Byte with DTC  
      if(j == 0){
          String fdig = "PO";        
        }
        else if(j == 1){
          String fdig = "P1";
        }
        else if(j == 2){
          String fdig = "P2";
        }
        else if(j == 3){
          String fdig = "P3";
        }
        else if(j == 4){
          String fdig = "C0";
        }
        else if(j == 5){
          String fdig = "C1";
        }
        else if(j == 6){
          String fdig = "C2";
        }
        else if(j == 7){
          String fdig = "C3";
        }
        else if(j == 8){
          String fdig = "B0";
        }
        else if(j == 9){
          String fdig = "B1";
        }
        else if(j == A){          //A STRING??
          String fdig = "B2";
        }
        else if(j == B){
          String fdig = "B3";
        }
        else if(j == C){
          String fdig = "UO";
        }
        else if(j == D){
          String fdig = "U1";
        }
        else if(j == E){
          String fdig = "U2";
        }
        else if(j == F){
          String fdig = "U3";
        }
    cod = fdig + WorkingString;
    int i = (j-2)/6;
    Serial.print("DTC n°");Serial.print(i);Serial.print(" :");Serial.println(cod);
    }
  }
  
  BTOBD_serial.println("01 00");        //Receive Mode-01 Available Sensors (PIDs 0 - 20) 
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
    
  BTOBD_serial.println("01 20");        //Receive Mode-01 Available Sensors (PIDs 20 - 40)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response); 
  
  BTOBD_serial.println("01 40");        //Receive Mode-01 Available Sensors (PIDs 40 - 60)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("01 60");        //Receive Mode-01 Available Sensors (PIDs 60 - 80)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);

  BTOBD_serial.println("01 0C");        //Request PID 0C (Engine RPM)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("01 0D");        //Request PID 0D (Vehicle Speed)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("01 63");        //Request PID 63 (Engine reference torque)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("01 33");        //Request PID 33 (Baromtric pressure)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("01 5E");        //Request PID 5E (Fuel rate)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("01 1F");        //Request PID 1F (Run time since engine start)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("01 5C");        //Request PID 5C (Engine oil temperature)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  BTOBD_serial.println("01 05");        //Request PID 05 (Coolant Temperature)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
    
  BTOBD_serial.println("01 46");        //Request PID 46 (Ambient Temperature)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
    
  BTOBD_serial.println("01 5B");        //Request PID 5B (Hybrid battery pack remaining life)
  delay(500); read_elm327_response(); Serial.println(raw_ELM327_response);
  
  
}

void loop(){
/****************************************
 ****************************************
 ****************  LOOP  ****************
 ****************************************
 ****************************************/
  
  //(do not use println in the loop)
  
  //ELM327_enter_terminal_mode();       //Un-comment to access terminal mode for ELM327
  //gprs.serialDebug();                 //Un-comment to access terminal mode for SIM800L

  timestamp = millis();   //Register initial timestamp

  Serial.print("\n");     //Start new line (do not use println in any print of the loop)
  Serial.print(timestamp); 
  Serial.print("\t[ms]\t");
  
/***************************************
 ******** Get data from sensors ********
 ***************************************
 Structure for each sensor:
  - Send sensor PID
  - Wait for the ELM327 to acquire
  - Read ELM327's response
  - Cut A,B,C,... Bytes values
  - Convert to integers
  - Apply formula
  - Display value and unit (tabulated)
 ***************************************/
  
  /*
   * Get Temperature
   */
  
  BTOBD_serial.println("0105");                         //Send temperature sensor PID
  delay(310);                                           //Wait for the ELM327 to acquire
  read_elm327_response();                               //Read ELM327's response
  WorkingString = raw_ELM327_response.substring(11,13); //Cut A Byte value
  A = strtol(WorkingString.c_str(),NULL,16)-40;         //Convert to integer
  temp = A-40;                                          //Apply formula
  Serial.print(A); Serial.print("\tºC\t");              //Display value and unit (tabulated)
  
  /*
   * Get RPM
   */
  
  BTOBD_serial.println("010C");                         //Send rpm sensor PID
  delay(310);                                           //Wait for the ELM327 to acquire
  read_elm327_response();                               //Read ELM327's response
  WorkingString = raw_ELM327_response.substring(11,13); //Cut A Byte value
  A = strtol(WorkingString.c_str(),NULL,16);            //Convert to integer
  WorkingString = raw_ELM327_response.substring(14,16); //Cut B Byte value  
  B = strtol(WorkingString.c_str(),NULL,16);            //Convert to integer    
  rpm = (256*A+B)/4;                                    //Apply formula
  Serial.print(rpm); Serial.print("\tRPM\t");           //Display value and unit (tabulated)
  
  /**
   * Get Velocity
   */

  BTOBD_serial.println("010D");                         //Send rpm sensor PID       
  delay(310);                                           //Wait for the ELM327 to acquire
  read_elm327_response();                               //Read ELM327's response
  WorkingString = raw_ELM327_response.substring(11,13); //Cut A Byte value
  A = strtol(WorkingString.c_str(),NULL,16);            //Convert to integer
  veloc=A;                                              //Apply formula
  Serial.print(veloc); Serial.print("\t[km/h]\t");      //Display value and unit (tabulated)

  //BTOBD_serial.end();
  
  
  /************************************
   ****** Send data to the cloud ******
   ************************************/
  
  /*
  GPRS gprs;
  delay(1000);
  if(0 == gprs.connectTCP(server, 80)){
      //Serial.print("Connect successfuly to ");
      //Serial.println(server);
  }
  else{
      //Serial.println("connect error");
  }
  thingspeak_command = ("GET /update?api_key="+WriteAPIKey+"&field1="+rpm+"&field2="+veloc+"    HTTP/1.0\r\n\r\n");
  //Serial.println("command="+thingspeak_command);
  char* http_cmd = const_cast<char*>(thingspeak_command.c_str()); //Parse command to char array

  if(0 == gprs.sendTCPData(http_cmd)){
    //gprs.serialDebug();
    char fin_get[] = "CLOSED\n";
    gprs.waitForResp(fin_get,5);
    Serial.println("Sent");
  }
  else{
    Serial.println("Not sent");
  }

  gprs.serialSIM800.end();
  delay(1000);
  BTOBD_serial.begin(baud_serial2);
  */
  
  
  time_elapsed=millis()-timestamp;
  if(time_between_loops>time_elapsed){
    delay(time_between_loops-time_elapsed);
  }

}

void read_elm327_response(){
  raw_ELM327_response = "";
  while (BTOBD_serial.available()>0){
    inData=0;
    inChar=0;
    inData = BTOBD_serial.read();
    inChar=char(inData);
    raw_ELM327_response = raw_ELM327_response + inChar;
  } 
  //read_elm327_response()
  //Serial.println(raw_ELM327_response);
}



void ELM327_enter_terminal_mode(){
  /**Debug HC05 Bluetooth and ELM327 OBD
   * *  Captures serial2 data and writes input to it.
  */
  while(true){
    if (BTOBD_serial.available()){
      Serial.write(BTOBD_serial.read());
    }
    if (Serial.available()){
      BTOBD_serial.write(Serial.read());
    } //if
  }//while
}//ELM327_enter_terminal_mode()
