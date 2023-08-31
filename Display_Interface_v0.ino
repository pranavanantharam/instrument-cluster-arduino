#include <MCUFRIEND_kbv.h>
#include <mcp_can.h>
#include <math.h>
#include<SPI.h>
 
#define BLACK   0x0000
#define BLUE	0x001F
#define RED 	0xF800
#define GREEN   0x07E0
#define CYAN	0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY	0x8410
#define CAN0_INT 48                                  // Interrupt Pin
 
MCP_CAN CAN0(46);                                    // Chip Enable Pin
 
MCUFRIEND_kbv tft;
 
uint16_t version = MCUFRIEND_KBV_H_;
uint16_t ID;
 
long unsigned int rxId;
unsigned char len=0;
unsigned char rxBuf[8];
 
float prev_rpm = 0;
int rpm_green = 5000;
int rpm_yellow = 10000;
 
int prev_gear= 0;
int prev_water_temperature = 0;
int prev_air_temperature = 0;
int prev_vehicle_speed = 0;
int prev_fuel = 0;
float prev_battery_voltage = 0;
int prev_vehicle_status = 0;
 
void start_display()
{
   ID = tft.readID();
   tft.begin(ID);
   tft.setRotation(1);
   tft.fillScreen(WHITE);
   tft.setTextColor(BLACK);
  
   tft.setTextSize(6);
   tft.setCursor(85,50);
   tft.print("TEAM");
   tft.setCursor(85,100);
   tft.print("HAYA");
   tft.setCursor(60,150);
   tft.print("RACING");
   tft.setCursor(200,200);
   tft.setTextSize(3);
   tft.print("- 2020");
   delay(1000);
}
 
float process_data(int low_byte, int high_byte)
{
  float data=high_byte*256 + low_byte;
  if(data>32767)
 	data = (data - 65536);
  return data;  
}
 
 
void update_rpm(float rpm)
{
   if (abs(rpm - prev_rpm) > 200)
   {
  	if (rpm < rpm_green)
     	for(int i = 0; i <= (rpm/250); i ++)
        	tft.fillRect(4 + (i * 6), 6, 5, 38, GREEN);
        	
  	if (rpm < rpm_yellow)
     	for(int i = 0; i <= (rpm/250); i ++)
        	if (i <= (rpm_green/250))
           	tft.fillRect(4 + (i * 6), 6, 5, 38, GREEN);  
        	else
           	tft.fillRect(4 + (i * 6), 6, 5, 38, YELLOW);
  	else
     	for(int i = 0; i <= (rpm/250); i ++)
        	if (i <= rpm_green/250)
           	tft.fillRect(4 + (i * 6), 6, 5, 38, GREEN);
             	
        	else if ((i >= rpm_green/250) && (i <= rpm_yellow/250))
           	tft.fillRect(4 + (i * 6), 6, 5, 38, YELLOW);
 
        	else if (i <= 51)
           	tft.fillRect(4 + (i * 6), 6, 5, 38, RED);
 
  	if (rpm == 0)
     	tft.fillRect(4, 6, 5, 38, WHITE);
  	for (int i = (rpm/250) + 1; i <= 51; i++)
     	tft.fillRect(4 + (i * 6), 6, 5, 38, WHITE);
 
  	prev_rpm = rpm;
   }
}
 
void update_air_temperature(int air_temperature)
{
   if (abs(air_temperature - prev_air_temperature) >= 1)
   {
  	tft.fillRect(5,90,55,30,WHITE);
  	if (air_temperature > 99)
     	tft.setCursor(5,90);
  	else
     	tft.setCursor(25,90);
      tft.setTextSize(3);
  	tft.print(air_temperature);
  	prev_air_temperature = air_temperature;
   }
}
 
void update_gear(int gear)
{
   if (abs(gear - prev_gear) >= 1)
   {
  	tft.fillRect(155,90,35,30,WHITE);
  	tft.setCursor(155,90);
 	 tft.setTextSize(3);
  	tft.print(gear);
  	prev_gear = gear;
   }
}
 
void update_water_temperature(int water_temperature)
{
   if (abs(water_temperature - prev_water_temperature) >= 1)
   {
  	tft.fillRect(230,90,55,30,WHITE);
  	if (water_temperature > 99)
     	tft.setCursor(230,90);
  	else
     	tft.setCursor(250,90);
  	tft.setTextSize(3);
  	tft.print(water_temperature);
  	prev_water_temperature = water_temperature;
   }
}
 
void update_vehicle_speed (int vehicle_speed)
{
   if (abs(vehicle_speed - prev_vehicle_speed) >= 1)
   {
  	tft.fillRect(80,130,125,50,WHITE);
  	if (vehicle_speed < 10)
     	tft.setCursor(165,130);
  	else if(vehicle_speed < 100)
      	tft.setCursor(125,130);
  	else
      	tft.setCursor(80,130);
  	tft.setTextSize(7);
  	tft.print(vehicle_speed);
  	prev_vehicle_speed = vehicle_speed;
   }
}
 
void update_fuel_level(int fuel)
{
   if (abs(fuel - prev_fuel) >= 1)
   {
  	if(fuel < 1)
  	{
     	tft.fillRect(5,210,8,24,WHITE);
     	tft.fillRect(5,210,2,24,RED);
     	tft.fillRect(17,210,8,24,WHITE);
     	tft.fillRect(29,210,8,24,WHITE);
     	tft.fillRect(41,210,8,24,WHITE);
     	tft.fillRect(53,210,8,24,WHITE);
     	tft.fillRect(65,210,8,24,WHITE);
  	}
  	else if(fuel == 1)
  	{
     	tft.fillRect(5,210,8,24,RED);
     	tft.fillRect(17,210,8,24,WHITE);
     	tft.fillRect(29,210,8,24,WHITE);
     	tft.fillRect(41,210,8,24,WHITE);
     	tft.fillRect(53,210,8,24,WHITE);
     	tft.fillRect(65,210,8,24,WHITE);
  	}
  	else if(fuel==2)
  	{
     	tft.fillRect(5,210,8,24,YELLOW);
     	tft.fillRect(17,210,8,24,YELLOW);
     	tft.fillRect(29,210,8,24,WHITE);
     	tft.fillRect(41,210,8,24,WHITE);
     	tft.fillRect(53,210,8,24,WHITE);
     	tft.fillRect(65,210,8,24,WHITE);
      }
  	else if(fuel == 3)
  	{
     	tft.fillRect(5,210,8,24,BLACK);
     	tft.fillRect(17,210,8,24,BLACK);
     	tft.fillRect(29,210,8,24,BLACK);
     	tft.fillRect(41,210,8,24,WHITE);
     	tft.fillRect(53,210,8,24,WHITE);
     	tft.fillRect(65,210,8,24,WHITE);
  	}
  	else if(fuel == 4)
  	{
     	tft.fillRect(5,210,8,24,BLACK);
     	tft.fillRect(17,210,8,24,BLACK);
     	tft.fillRect(29,210,8,24,BLACK);
     	tft.fillRect(41,210,8,24,BLACK);
     	tft.fillRect(53,210,8,24,WHITE);
     	tft.fillRect(65,210,8,24,WHITE);
  	}
  	else if(fuel == 5)
  	{
     	tft.fillRect(5,210,8,24,BLACK);
     	tft.fillRect(17,210,8,24,BLACK);
     	tft.fillRect(29,210,8,24,BLACK);
     	tft.fillRect(41,210,8,24,BLACK);
     	tft.fillRect(53,210,8,24,BLACK);
     	tft.fillRect(65,210,8,24,WHITE);
  	}
  	else if(fuel >= 6)
  	{
     	tft.fillRect(5,210,8,24,BLACK);
  	   tft.fillRect(17,210,8,24,BLACK);
     	tft.fillRect(29,210,8,24,BLACK);
     	tft.fillRect(41,210,8,24,BLACK);
     	tft.fillRect(53,210,8,24,BLACK);
     	tft.fillRect(65,210,8,24,BLACK);
  	}
  	prev_fuel = fuel;
   }
}
 
void update_battery_voltage(float battery_voltage)
{
   if (abs(battery_voltage - prev_battery_voltage) >= 0.1)
   {
  	tft.fillRect(102,215,70,30,WHITE);
  	if (battery_voltage < 9.9)
     	tft.setCursor(120,215);
  	else
     	tft.setCursor(102,215);
  	tft.setTextSize(3);
  	tft.print(battery_voltage,1); 
  	prev_battery_voltage = battery_voltage;
   }
}
 
void update_vehicle_status(int vehicle_status)
{
   String message; // Less than 7 characters
   if(abs(vehicle_status - prev_vehicle_status))
   {
  	if (vehicle_status % 2 == 0)
  	{
     	message = "ALL OK";
     	tft.setTextColor(GREEN);
  	}
  	else if (vehicle_status % 2 == 1)
      {
     	message = " ERROR";
     	tft.setTextColor(RED);
  	}  
  	tft.fillRect(245,218,80,30,WHITE);
  	tft.setCursor(245,218);
  	tft.setTextSize(2);
  	tft.print(message);
  	tft.setTextColor(BLACK);
  	prev_vehicle_status = vehicle_status;
   }
}
 
void can_not_found()
{
   tft.fillScreen(WHITE);
   tft.setTextColor(RED);
   tft.setTextSize(2);
   tft.setCursor(25,120);
   tft.print("CAN Module not detected");
}
 
void initialize_display()
{
   tft.fillScreen(WHITE);
   tft.setTextColor(BLACK);
 
   // Initialize RPM
   tft.drawRect(3, 5, 313, 40, BLACK); // reverting it back. Made it 40
   tft.setTextSize(1);
   tft.setCursor(260,59);
   tft.print("x1000 rpm");
   for(int i=0; i<13; i++)
   {
  	tft.setCursor(4+(i)*24, 48);
  	tft.print(i);
   }
 
   // Initialize Air Temperature
   tft.setCursor(5,75);
   tft.setTextSize(1);
   tft.print("Water Temp");
   tft.setCursor(35,87);
   tft.setTextSize(1);
   tft.print("o");
   tft.setCursor(45,90);
   tft.setTextSize(3);
   tft.print("C");
 
   // Initialize Gear
   tft.setTextSize(1);
   tft.setCursor(150,75);
   tft.print("Gear");
 
   // Initialize Water Temperature
   tft.setCursor(250,75);
   tft.setTextSize(1);
   tft.print("Water Temp");
   tft.setCursor(285,87);
   tft.setTextSize(1);
   tft.print("o");
   tft.setCursor(295,90);
   tft.setTextSize(3);
   tft.print("C");
 
   // Initialize Speed
   tft.setTextSize(2);
   //tft.setCursor(20,147); // To display "Speed"
   //tft.print("Speed");
   tft.setCursor(215,147);
   tft.print("km/h");
 
   // Initialize Fuel Level
   tft.setCursor(5,200);
   tft.setTextSize(1);
   tft.print("Fuel");
 
   // Initialize Battery Voltage
   tft.setCursor(105,200);
   tft.setTextSize(1);
   tft.print("Battery Voltage");
   tft.setCursor(178,215);
   tft.setTextSize(3);
   tft.print("V");
 
   // Initialize Status
   tft.setCursor(270,200);
   tft.setTextSize(1);
   tft.print("Status");
}
 
void setup()
{
   start_display();
   Serial.begin(115200);
   while(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) != CAN_OK)
   {
  	Serial.println("Initializing");
  	can_not_found();
   }
   CAN0.setMode(MCP_NORMAL);
   pinMode(CAN0_INT, INPUT);
  
   initialize_display();
}
 
void loop()
{
   if(!digitalRead(CAN0_INT))
   {
  	CAN0.readMsgBuf(&rxId, &len, rxBuf);
  	Serial.println(rxId);    	
     	if (rxId == 0x8CFFF048)    	//PE 1 - 2365583432
 	    {
        	Serial.print("RPM -> ");
        	float rpm = process_data(rxBuf[0], rxBuf[1]);
        	Serial.println(rpm);
        	update_rpm(rpm);
     	}
     	
      	if (rxId == 0x8CFFF548)    	// PE 6
      	{
       	 Serial.print("Battery Voltage -> ");
        	float bv = process_data(rxBuf[0], rxBuf[1])/100;
        	float at = process_data(rxBuf[2], rxBuf[3])/10;
        	float wt = process_data(rxBuf[4], rxBuf[5])/10;
        	Serial.println(bv);
 	       
        	Serial.print("Water Temperature -> ");
        	Serial.println(wt);
        	update_battery_voltage(bv);
        	update_water_temperature(wt);
        	update_air_temperature(at);
      	}
    	}
   }
