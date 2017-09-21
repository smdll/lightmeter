#include <Wire.h>
#include <Adafruit_ssd1306syp.h>
#include<math.h>
#define ADDRESS 0x23
#define SDA_PIN 8
#define SCL_PIN 7
Adafruit_ssd1306syp display(SDA_PIN, SCL_PIN);
byte index[2] = {2, 4};
double shut;
float tab[2][9] = {
  {25, 50, 100, 200, 400, 800, 1600, 3200, 6400},//ISO
  {1.4, 2.0, 2.8, 5.6, 8.0, 11, 16, 22, 32},//Aperture
};

int UV_Read()
{
  return analogRead(A0);
}

unsigned int Lx_Read() //
{
  int i = 0;
  unsigned int val = 0;
  byte buff[2];
  Wire.beginTransmission(ADDRESS);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
  delay(200);
  Wire.beginTransmission(ADDRESS);
  Wire.requestFrom(ADDRESS, 2);
  while (Wire.available()) //
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();
  if (i == 2)
    val = ((buff[0] << 8) | buff[1]) / 1.2;
  return val;
}

void Display(unsigned int lx, double ev, int uv)
{
  display.clear();
  display.setCursor(0, 0);
  display.print("ISO:");
  display.println(tab[0][index[0]]);
  display.setCursor(0, 10);
  display.print("Aperture:f");
  display.println(tab[1][index[1]]);
  display.setCursor(0, 20);
  if(shut > 1)
  {
    display.print("Shutter:1/");
    display.println(shut);
  }
  else
  {
    display.print("Shutter:");
    display.println(1/shut);
  }
  display.setCursor(0, 30);
  display.print("EV:");
  display.println(ev);
  display.setCursor(0, 40);
  display.print("UV:");
  display.println(uv);
  display.setCursor(0, 50);
  display.print("lx:");
  display.println(lx);
  display.update();
}

void IncISO()
{
  index[0]++;
  if(index[0] >= 9)
    index[0] = 0;
}

void IncAper()
{
  index[1]++;
  if(index[1] >= 9)
    index[1] = 0;
}

double Lx2ev(unsigned int lx)
{
  if(lx == 0)
    return 0;
  double ev = log(lx) / log(2) - 1.3;
  return ev;
}

void cal(double ev)
{
  double currEv = pow(2, ev + index[0] - 3);
  double fv = pow(tab[1][index[1]], 2);
  shut = currEv / fv;
}

void setup()
{
  Wire.begin();
  display.initialize();
  attachInterrupt(0, IncISO, RISING);
  attachInterrupt(1, IncAper, RISING);
}

void loop()
{
  unsigned lx = Lx_Read();
  double ev = Lx2ev(lx);
  cal(ev);
  int uv = UV_Read();
  Display(lx, ev, uv);
}

