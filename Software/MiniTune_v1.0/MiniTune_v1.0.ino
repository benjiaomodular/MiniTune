#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LENGTH 512

// LMNC ----
//clipping indicator variables
boolean clipping = 0;

//data storage variables
byte newData = 0;
byte prevData = 0;

//freq variables
unsigned int timer = 0;//counts period of wave
unsigned int period;
int frequency;
char *notes[] = {
  "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
int note_idx = 0;
int flatsharp; //-1 if flat, 0 if on point, 1 if sharp

bool low = false;
bool high = false;

//---

void setup() {
  Serial.begin(9600);

  cli();//diable interrupts
  
  //set up continuous sampling of analog pin 0
  
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei();//enable interrupts

//   Setup display 
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  delay(2000);

}

ISR(ADC_vect) {//when new ADC value ready

  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >=127){//if increasing and crossing midpoint
    period = timer;//get period
    timer = 0;//reset timer
  }
  
  timer++;//increment timer at rate of 38.5kHz
}


void loop() {

 frequency = 384620/period;//timer rate with an extra zero/period
 computeNote();
 delay(70);
 Serial.println(frequency);

 drawNote();
 }

void computeNote(){

  low = 0 < frequency && 158 > frequency;
  high = 10180 < frequency && 100000 > frequency;
  
  //C
  if ((158<frequency && 161>frequency)    ||
      (318<frequency && 323>frequency)    ||
      (636<frequency && 650>frequency)    ||
      (1272<frequency && 1302>frequency)  ||
      (2550<frequency && 25590>frequency) ||
      (5155<frequency && 5170>frequency)  ||
      (10180<frequency && 10320>frequency)) {
      flatsharp = -1;
      note_idx = 0;
  }
  
  
  if ((161<frequency && 165>frequency) ||
      (323<frequency && 330>frequency) ||
      (650<frequency && 660>frequency) ||
      (1302<frequency && 1315>frequency) ||
      (2590<frequency && 2640>frequency) ||
      (5170<frequency && 5300>frequency) ||
      (10320<frequency && 10730>frequency)) {
      flatsharp = 0;
      note_idx = 0;
  }
  
  
  if ((165<frequency && 168>frequency) ||
      (330<frequency && 336>frequency) ||
      (660<frequency && 674>frequency) ||
      (1315<frequency && 1350>frequency) ||
      (2640<frequency && 2690>frequency) ||
      (5300<frequency && 5400>frequency) ||
      (10730<frequency && 11000>frequency)) {
      flatsharp = 1;
      note_idx = 0;
  }
  
  
  //C#
  if ((168<frequency && 171>frequency) ||
      (336<frequency && 345>frequency)  ||
      (674<frequency && 689>frequency)   ||
      (1350<frequency && 1378>frequency)   ||
      (2690<frequency && 2750>frequency)   ||
      (5420<frequency && 5500>frequency)) {
      flatsharp = -1;
      note_idx = 1;
  }
  
  if ((171<frequency && 175>frequency) ||
      (345<frequency && 351>frequency)  ||
      (689<frequency && 698>frequency)   ||
      (1378<frequency && 1392>frequency)   ||
      (2750<frequency && 2800>frequency)   ||
      (5490<frequency && 5591>frequency)) {
      flatsharp = 0;
      note_idx = 1;
  }
  
  if ((175<frequency && 178>frequency) ||
      (351<frequency && 356>frequency)  ||
      (698<frequency && 714>frequency)   ||
      (1392<frequency && 1429>frequency)   ||
      (2800<frequency && 2855>frequency)   ||
      (5581<frequency && 5700>frequency)) {
      flatsharp = 1;
      note_idx = 1;
  }
                   
  
  //D
  if ((178<frequency && 181>frequency) ||
      (356<frequency && 364>frequency) ||
      (714<frequency && 731>frequency) ||
      (1429<frequency && 1461>frequency) ||
      (2855<frequency && 2923>frequency) ||
      (5700<frequency && 5830>frequency)) {
      flatsharp = -1;
      note_idx = 2;
  }
  
  if ((181<frequency && 185>frequency) ||
      (364<frequency && 370>frequency)  ||
      (731<frequency && 741>frequency)    ||
      (1461<frequency && 1475>frequency)   ||
      (2923<frequency && 2955>frequency)   ||
      (5820<frequency && 5930>frequency)) {
      flatsharp = 0;
      note_idx = 2;
  }
  
  if ((185<frequency && 188>frequency) ||
      (370<frequency && 377>frequency)  ||
      (741<frequency && 756>frequency)   ||
      (1475<frequency && 1512>frequency)   ||
      (2955<frequency && 3020>frequency)   ||
      (5920<frequency && 6080>frequency)) {   
      flatsharp = 1;
      note_idx = 2;
  }
  
  
  //D#
  if ((188<frequency && 192>frequency)  || (377<frequency && 386>frequency)  || (756<frequency && 776>frequency)    || (1512<frequency && 1548>frequency)   || (3020<frequency && 3090>frequency)   || (6080<frequency && 6190>frequency)      ){    
      flatsharp = -1;
      note_idx = 3;}
  
  if ((192<frequency && 196>frequency)  || (386<frequency && 392>frequency)  || (776<frequency && 785>frequency)    || (1548<frequency && 1563>frequency)   || (3090<frequency && 3125>frequency)   || (6180<frequency && 300>frequency)      ){     
      flatsharp = 0;
      note_idx = 3;}
  
  if ((196<frequency && 201>frequency)  || (392<frequency && 400>frequency)  || (785<frequency && 799>frequency)    || (1563<frequency && 1602>frequency)   || (3125<frequency && 3200>frequency)   || (6290<frequency && 6440>frequency)      ){     
      flatsharp = 1;
      note_idx = 3;}
  
  
  
  //E
                       
  
  if ((201<frequency && 203>frequency)  || (400<frequency && 408>frequency)  || (799<frequency && 816>frequency)    || (1602<frequency && 1640>frequency)   || (3200<frequency && 3270>frequency)   || (6440<frequency && 6550>frequency)){
      flatsharp = -1;
      note_idx = 4;
  }
  
  
  if ((203<frequency && 208>frequency)   || (408<frequency && 415>frequency)  || (816<frequency && 829>frequency)    || (1640<frequency && 1656>frequency)   || (3270<frequency && 3330>frequency)   || (6540<frequency && 6660>frequency)        ) {
      flatsharp = 0;
      note_idx = 4;
  }
  
  if ((208<frequency && 212>frequency)  || (415<frequency && 423>frequency)  || (829<frequency && 844>frequency)    || (1656<frequency && 1700>frequency)   || (3330<frequency && 3400>frequency)   || (6650<frequency && 6850>frequency)        ) {
      flatsharp = 1;
      note_idx = 4;
  }
  
  
  
  
  
  //F0
                       
  
  
  if ((212<frequency && 215>frequency)  || (423<frequency && 432>frequency)  || (844<frequency && 866>frequency)    || (1700<frequency && 1738>frequency)   || (3400<frequency && 3475>frequency)   || (6850<frequency && 6930>frequency)        ){
      flatsharp = -1;
      note_idx = 5;
  }
  
  if ((215<frequency && 220>frequency)  || (432<frequency && 442>frequency)  || (866<frequency && 880>frequency)    || (1738<frequency && 1753>frequency)   || (3475<frequency && 3490>frequency)   || (6920<frequency && 7040>frequency)        )   {
      flatsharp = 0;
      note_idx = 5;
  }
  
  if ((220<frequency && 224>frequency)  || (442<frequency && 448>frequency)  || (880<frequency && 900>frequency)    || (1753<frequency && 1800>frequency)   || (3490<frequency && 3600>frequency)   || (7030<frequency && 7180>frequency)        ){
      flatsharp = 1;
      note_idx = 5;
  }
  
  
  //F#0
                       
  
  
  if ((224<frequency && 229>frequency)  || (448<frequency && 460>frequency)  || (900<frequency && 918>frequency)    || (1800<frequency && 1842>frequency)   || (3600<frequency && 3680>frequency)   || (7180<frequency && 7340>frequency)    ) {
      flatsharp = -1;
      note_idx = 6;
  }
  
  if ((229<frequency && 234>frequency)  || (460<frequency && 468>frequency)  || (918<frequency && 932>frequency)    || (1742<frequency && 1858>frequency)   || (3680<frequency && 3730>frequency)   || (7330<frequency && 7460>frequency)    )    {
      flatsharp = 0;
      note_idx = 6;
  }
  
  if ((234<frequency && 239>frequency)  || (468<frequency && 476>frequency)  || (932<frequency && 955>frequency)    || (1858<frequency && 1915>frequency)   || (3730<frequency && 3820>frequency)   || (7450<frequency && 7630>frequency)    ){
      flatsharp = 1;
      note_idx = 6;
  }
  
  
  //G0
  if ((239<frequency && 242>frequency)  || (476<frequency && 487>frequency)  || (955<frequency && 973>frequency)    || (1915<frequency && 1953>frequency)   || (3820<frequency && 3900>frequency)   || (7763<frequency && 7800>frequency)  ){
      flatsharp = -1;
      note_idx = 7;
  }
  
  if ((242<frequency && 247>frequency)  || (487<frequency && 494>frequency)  || (973<frequency && 988>frequency)    || (1953<frequency && 1968>frequency)   || (3900<frequency && 3950>frequency)   || (7790<frequency && 7910>frequency)  ){
      flatsharp = 0;
      note_idx= 7;
  }
  
  if ((247<frequency && 252>frequency)  || (494<frequency && 505>frequency)  || (988<frequency && 1005>frequency)    || (1968<frequency && 2020>frequency)   || (3950<frequency && 4030>frequency)   || (7900<frequency && 8100>frequency)  ){
      flatsharp = 1;
      note_idx = 7;}
  
  //G#0
  if ((252<frequency && 257>frequency)  || (505<frequency && 516>frequency)  || (1005<frequency && 1031>frequency)   || (2020<frequency && 2068>frequency)   || (4030<frequency && 4110>frequency)   || (8100<frequency && 8270>frequency)      ) {
      flatsharp = -1;
      note_idx = 8;
  }
  
  if ((257<frequency && 261>frequency)  || (516<frequency && 525>frequency)  || (1031<frequency && 1045>frequency)   || (2068<frequency && 2085>frequency)   || (4110<frequency && 4180>frequency)    || (8260<frequency && 8390>frequency)     )  {
      flatsharp = 0;
      note_idx = 8;
  }
  
  if ((261<frequency && 266>frequency)  || (525<frequency && 538>frequency)  || (1045<frequency && 1075>frequency)   || (2085<frequency && 2155>frequency)   || (4180<frequency && 4265>frequency)   || (8380<frequency && 8570>frequency)      ){
      flatsharp = 1;
      note_idx = 8;
  }
  
  
  //A0
  if ((266<frequency && 273>frequency)  || (538<frequency && 546>frequency)  || (1075<frequency && 1093>frequency)   || (2155<frequency && 2190>frequency)   || (4265<frequency && 4385>frequency)   || (8570<frequency && 8730>frequency)       ) {
      flatsharp = -1;
      note_idx = 9;
  }
  
  if ((273<frequency && 277>frequency)  || (546<frequency && 555>frequency)  || (1093<frequency && 1117>frequency)   || (2190<frequency && 2210>frequency)   || (4385<frequency && 4480>frequency)   || (8720<frequency && 8880>frequency)       ){
      flatsharp = 0;
      note_idx = 9;
  }
  
  if ((277<frequency && 283>frequency)  || (555<frequency && 567>frequency)  || (1117<frequency && 1137>frequency)   || (2210<frequency && 2285>frequency)   || (4480<frequency && 4560>frequency)   || (8870<frequency && 9050>frequency)       ){
      flatsharp = 1;
      note_idx = 9;
  }
  
  //A#0
                       
  
  
  if ((283<frequency && 288>frequency)  || (567<frequency && 578>frequency)  || (1137<frequency && 1158>frequency)   || (2285<frequency && 2320>frequency)   || (4560<frequency && 4630>frequency)   || (9050<frequency && 9250>frequency)      ) {
      flatsharp = -1;
      note_idx = 10;
  }
  
  if ((288<frequency && 293>frequency)  || (578<frequency && 587>frequency)  || (1158<frequency && 1172>frequency)   || (2320<frequency && 2342>frequency)   || (4630<frequency && 4705>frequency)   || (9240<frequency && 9410>frequency)      ){
      flatsharp = 0;
      note_idx = 10;
  }
  if ((293<frequency && 300>frequency)  || (585<frequency && 600>frequency)  || (1172<frequency && 1190>frequency)   || (2342<frequency && 2410>frequency)   || (4705<frequency && 4800>frequency)   || (9400<frequency && 9580>frequency)      ){
      flatsharp = 1;
      note_idx = 10;
  }
  
  
  
  //B0
  
  
  if ((300<frequency && 306>frequency)   || (600<frequency && 612>frequency)  || (1190<frequency && 1222>frequency)   || (2410<frequency && 2460>frequency)   || (4800<frequency && 4895>frequency)   || (9580<frequency && 9800>frequency)    ){
      flatsharp = -1;
      note_idx = 11;
  }
  
  if ((306<frequency && 312>frequency)   || (612<frequency && 623>frequency)  || (1222<frequency && 1237>frequency)   || (2460<frequency && 2482>frequency)   || (4895<frequency && 4950>frequency)    || (9790<frequency && 9990>frequency)   ){
      flatsharp = 0;
      note_idx = 11;
  }
  
  if ((312<frequency && 318>frequency)   || (623<frequency && 636>frequency)  || (1237<frequency && 1272>frequency)   || (2482<frequency && 2550>frequency)   || (4950<frequency && 5055>frequency)   || (9980<frequency && 10180>frequency)    ){
      flatsharp = 1;
      note_idx = 11;
  }

}

void drawNote(){
  display.clearDisplay();
  display.setTextColor(WHITE); // Draw white text

  display.setTextSize(1);
  display.setCursor(0, 0); // Start at top-left corner
  display.print(frequency/10.0f);
  display.println("Hz");

  
  display.setCursor(80, 0); // Start at top-left corner

  if (low) {
     display.print("TOO LOW");     
  }

  if (high) {
    display.print("TOO HIGH");  
  }
  

  display.setTextSize(3);
  display.setCursor(0, 10); // Start at top-left corner


  if (flatsharp == -1){
    display.print(" > ");
  } else {
    display.print("   ");
  }

  display.print(notes[note_idx]);

  if (flatsharp == 1){
    display.print(" < ");
  } else {
    display.print("   ");
  }


  
  display.display();
}
