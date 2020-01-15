/*
    Group 70
    
    A.Ilker SIGIRCI - 2171940
    Onur Can UNAL    - 2095966
*/

/*
    Since ISR should be short, we just set flags in ISR and let the main function handle the operations with respect to those flags.
    
    In the RB port change interrupt, we saw that one press-release of RB6-7 buttons actually seen as more than one.
    To solve this issue, we put 1 ms delay in the beginning of RB port change interrupt flag. 
    
    We create 3 seconds busy wait after the button RE1 is pressed by using nested loops, and we checked it via simulator and also according
    to real time.
    
    To update 7 segment digit, we count to 10 timer0 interrupt(10*0.4 = 4ms) and change 7 segment within ISR. We must do this in ISR, because 
    when we try to change 7 segment in the main routine, it flickered.

    In endProgram() function,  we disabled RB and TMR1 interrupt as they won't be necessary when voting time finishes or there is no voters left, continue to check TMR0 interrupt since ADC conversion depends on it.
*/

/*
    Timer0 configuration            -> 8-bit, PSA, 1:16 prescale value
                                       Each Timer0 interrupt = 0.4 ms
                                       Counting 250 Timer0 interrupt is 250 * 0.4= 100ms
                                       Counting 625 Timer0 interrupt is 625 * 0.4= 250ms
                            
    Timer1 configuration            -> 16-bit, Timer1 oscillator, 1:8 prescale value
                                       Each Timer1 interrupt = 0.05s
                                       Counting 20 Timer1 interrupt is 20 * 0.5= 1s
*/

// SUBROUTINES

    void initialization();         // Call initialization functions
    void start();                  // Write  #Electro Vote# to LCD and wait for RE1 button press. Then wait 3 seconds
    void getVoterCount();          // Get total voters
    void beginCountdown();         // Start count from 90 seconds and turn-on timer0 - timer1
    void passOneSecond();          // Each one second, decrease timeLeft
    void blink();                  // blink every 250ms by showing either " " or ">"
    void voteCandidate();          // Vote a candidate with respect to adcValue
    void changeLeader(int n);      // Update ID and vote count of the candidate with respect to the candidate which has maximum vote
    void change7Segment();         // Change display of 7-segment
    void writeSegment(int value);  // Assign the related value to LATJ to display 7-segment representation of the value
    void init();                   // Configure ports
    void initTimer0();             // Initialize Timer0
    void initTimer1();             // Initialize Timer1
    void initRB();                 // Initialize R
    void initADC();                // Initialize ADC
    void startADC();               // Start A/D conversion
    void readADC();                // Read ADC Value
    void endProgram();             // End of the program
    void __interrupt() isr(void);  // ISR routine

#include <p18cxxx.h>
#include "Includes.h"
#include "LCD.h"
#include <stdio.h>
#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#define _XTAL_FREQ   40000000

// beginning of variable initializations
unsigned int adcDone = 0;
unsigned int tmr0Counter = 0;
unsigned int tmr1Counter = 0;
unsigned int votingTime = 90;
unsigned int votingEnd = 0;
unsigned int isRB6Pressed = 0;
unsigned int isRB7Pressed = 0;
unsigned int RB7Counter = 0;
unsigned int adcValue = 0;
unsigned int tensDigit = 0;
unsigned int unitsDigit = 0;
unsigned int voterLeft = 0;
unsigned int tensFlag = 0;
unsigned int unitsFlag = 0;
unsigned int voteFlag = 0;
unsigned int maxVote = 0;
unsigned int maxId = 0;
unsigned char digit;
unsigned char result[16];
unsigned int segmentCounter = 0;
unsigned int whichCandidate = 0;
unsigned int segmentISRCounter = 0;
unsigned int oneSecond = 0;
unsigned int spaceFlag = 0;
unsigned int blinkFlag = 0;
unsigned int i;
unsigned int j;
unsigned int idVote[8][2] = {{18,0},{64,0},{34,0},{23,0},{33,0},{67,0},{63,0},{99,0}};
const char *name[8] = {"Poe    ", "Galib  ", "Selimi ", "Nesimi ", "Hatayi ", "Zweig  ", "Nabi   ", "Austen "};
// end of variable initializations

void initialization(){ // Call initialization functions
    GIE = 0;  // Global interrupts are disabled
    init();
    InitLCD();
    ClearLCDScreen();
    initTimer0();
    initTimer1();
    initRB();
    initADC();    
    GIE = 1;  // Global interrupts are enabled
    TRISE= 0x02; // b'00000010' -> RE1 input 
}

void start(){ // Write  #Electro Vote# to LCD and wait for RE1 button press.
              // Then wait 3 seconds
    
    WriteCommandToLCD(0x80);
    WriteStringToLCD(" #Electro Vote# ");
    WriteCommandToLCD(0xC0);
    WriteStringToLCD(" ############## ");
    while(1){ if(PORTEbits.RE1) break; } // RE1 pressed
    while(PORTEbits.RE1 == 1); // RE1 released
    
    for(i=0;i<60125;i++){ // 3 second busy wait delay
        for(j=0; j<54; j++);
    }
}

void getVoterCount(){ // Get total voters
    WriteCommandToLCD(0x80);
    WriteStringToLCD(" #Enter Voters# ");
    WriteCommandToLCD(0xC0);
    WriteStringToLCD(" 00             ");   
    
    while(RB7Counter < 2){ // User is still entering voter number
        if(tensFlag){      // Change tens digit
            WriteCommandToLCD(0xC1);
            digit = (char)(((int)'0') + tensDigit);
            WriteDataToLCD(digit);
            tensFlag = 0;
        }
        if(unitsFlag){     // Change units digit
            WriteCommandToLCD(0xC2);
            digit = (char)(((int)'0') + unitsDigit);
            WriteDataToLCD(digit);
            unitsFlag = 0;
        }
    }
    voterLeft = tensDigit*10 + unitsDigit; // Store total voterCount on voterLeft variable
}

void beginCountdown(){ // Start count from 90 seconds and turn-on timer0 - timer1
    WriteCommandToLCD(0x80);
    WriteStringToLCD("  Time left :90 ");
    T0CONbits.TMR0ON = 1;
    T1CONbits.TMR1ON = 1;
}

void passOneSecond(){ // Each one second, decrease timeLeft
    WriteCommandToLCD(0x80);
    sprintf(result, "  Time left :%d%d ", votingTime/10, votingTime%10);
    WriteStringToLCD(result);
}

void blink(){ // blink every 250ms by showing either " " or ">"
    if(spaceFlag){
        WriteCommandToLCD(0xC1);
        WriteStringToLCD(" ");
        spaceFlag = 0;
    }
    if(blinkFlag){
        WriteCommandToLCD(0xC1);
        WriteStringToLCD(">");
        blinkFlag = 0;
    }
}

void voteCandidate(){ // Vote a candidate with respect to adcValue
    if(adcValue <= 127)                         whichCandidate = 0; // Vote Poe
    else if(128 <= adcValue && adcValue <= 255) whichCandidate = 1; // Vote Galib
    else if(256 <= adcValue && adcValue <= 383) whichCandidate = 2; // Vote Selimi
    else if(384 <= adcValue && adcValue <= 511) whichCandidate = 3; // Vote Nesimi
    else if(512 <= adcValue && adcValue <= 639) whichCandidate = 4; // Vote Hatayi
    else if(640 <= adcValue && adcValue <= 767) whichCandidate = 5; // Vote Zweig
    else if(768 <= adcValue && adcValue <= 895) whichCandidate = 6; // Vote Nabi
    else if(896 <= adcValue && adcValue <= 1023)whichCandidate = 7; // Vote Austen
    
    if(voteFlag){
        idVote[whichCandidate][1]++; // Increment the vote count of the candidate which is choosed
        voteFlag = 0;
        if(--voterLeft==0) votingEnd = 1; // Decrement the remaining voter count, end voting period if voter left is 0
        if(idVote[whichCandidate][1] > maxVote) changeLeader(whichCandidate); // Change leader from the 7-segment display
                                                                              // if he/she becomes a leader with this vote
    }
    WriteCommandToLCD(0xC2);
    sprintf(result, "%d%d %s:%d%d ", idVote[whichCandidate][0]/10, idVote[whichCandidate][0]%10, // Write ID of the candidate to LCD
                                     name[whichCandidate], //Write name of the candidate to LCD
                                     idVote[whichCandidate][1]/10, idVote[whichCandidate][1]%10); // Write vote count of the candidate to LCD
    WriteStringToLCD(result);
}

void changeLeader(int n){ // Update ID and vote count of the candidate with respect to
    maxId = idVote[n][0]; // the candidate which has maximum vote
    maxVote = idVote[n][1];
}

void change7Segment(){ // Change display of 7-segment
    LATH = 0x00;
    if(maxId == 0){ // maxId is 0 if voting has not begun
        LATJ = 0x40; // b'01000000' respresentation to write - to a digit
        if     (segmentCounter == 0){ LATHbits.LATH0 = 1; segmentCounter++;} // Choose zeroth digit
        else if(segmentCounter == 1){ LATHbits.LATH1 = 1; segmentCounter++;} // Choose first digit
        else if(segmentCounter == 2){ LATHbits.LATH2 = 1; segmentCounter++;} // Choose second digit
        else if(segmentCounter == 3){ LATHbits.LATH3 = 1; segmentCounter=0;} // Choose third digit
    }
    else{ // Display current leader and voting
        if     (segmentCounter == 0){ writeSegment(maxId/10)  ; LATHbits.LATH0 = 1; segmentCounter++;} // Choose zeroth digit
        else if(segmentCounter == 1){ writeSegment(maxId%10)  ; LATHbits.LATH1 = 1; segmentCounter++;} // Choose first digit
        else if(segmentCounter == 2){ writeSegment(maxVote/10); LATHbits.LATH2 = 1; segmentCounter++;} // Choose second digit
        else if(segmentCounter == 3){ writeSegment(maxVote%10); LATHbits.LATH3 = 1; segmentCounter=0;} // Choose third digit
    }
}

void writeSegment(int value){ // Assign the related value to LATJ to display 7-segment representation of the value
    
    switch(value){
        case 0:
            LATJ = 0x3F; // 7-segment representation of 0
            break;
        case 1:    
            LATJ = 0x06; // 7-segment representation of 1
            break;
        case 2:
            LATJ = 0x5B; // 7-segment representation of 2
            break;
        case 3:
            LATJ = 0x4F; // 7-segment representation of 3
            break;
        case 4:
            LATJ = 0x66; // 7-segment representation of 4
            break;
        case 5:    
            LATJ = 0x6D; // 7-segment representation of 5
            break;
        case 6:
            LATJ = 0x7D; // 7-segment representation of 6
            break;
        case 7:
            LATJ = 0x07; // 7-segment representation of 7
            break;
        case 8:
            LATJ = 0x7F; // 7-segment representation of 8
            break;
        case 9:
            LATJ = 0x67; // 7-segment representation of 9
            break;
        default:
            break;
    }
}

void init(){ // Configure ports
    TRISB= 0xC0; // b'11000000' -> RB6,7 input
    TRISE= 0x02; // b'00000010' -> RE1 input    
    TRISH= 0x00; // TRISH0, TRISH1, TRISH2, TRISH3 should be output for 7 segment display
    TRISJ= 0x00; // TRISJ bits should be output for 7 segment display  
    INTCON = 0x00;
    
}

void initTimer0(){ // Initialize Timer0
    TMR0 = 6; // 256-6 = 250; to count 250 each time
    T0CON = 0x43; // b'01000011'; 8-bit, PSA, 1:16 prescale value
    INTCONbits.TMR0IE = 1; // Enable Timer0 interrupt
    INTCONbits.TMR0IF = 0; // Clear Timer0 flag
    
    // Turn-on Timer0 later
    // Each Timer0 interrupt = 0.4 ms
    // Counting 250 Timer0 interrupt is 250 * 0.4= 100ms
    // Counting 625 Timer0 interrupt is 625 * 0.4= 250ms
}

void initTimer1(){ // Initialize Timer1
    TMR1 = 3036; // 65536-3036 = 62500; to count 62500 each time
    T1CON = 0xF0;  // b'11110000'; 16-bit, Timer1 oscillator, 1:8 prescale value
    PIE1bits.TMR1IE = 1; // Enable Timer1 interrupt
    PIR1bits.TMR1IF = 0; // Clear Timer1 flag
    
    // Turn-on Timer1 later
    // Each Timer1 interrupt = 0.05s
    // Counting 20 Timer1 interrupt is 20 * 0.5= 1s
}

void initRB(){ // Initialize RB
    INTCONbits.RBIE = 1; // Enable RB port change interrupt
    INTCONbits.RBIF = 0; // Clear  RB port change interrupt flag
}

void initADC(){ // Initialize ADC
    TRISH4 = 1; // Use RH4 as analog input
    ADCON1 = 0x02; // AVdd, AVss, AN12 analog
    ADFM = 1; // Right justified
    ACQT2 = 1; // We need at least 11 Tad, so select 12 Tad
    ACQT1 = 0;
    ACQT0 = 1;
    ADCON0 = 0x31; // Choose channel 12 (AN12), enable A/D converter module
}

void startADC(){ // Start A/D conversion
    ADIF = 0; // Clear A/D converter interrupt flag bit
    ADCON0bits.GO_DONE = 1;  // A/D conversion in progress
    ADIE = 1; // Enable A/D converter interrupt
}

void readADC(){ // Read ADC value
    adcValue = (ADRESH << 8) + ADRESL;
}
void endProgram(){ // End of the program
    PIE1bits.TMR1IE = 0;
    INTCONbits.RBIE = 0;
    WriteCommandToLCD(0xC1);
    WriteStringToLCD(">");
    while(1){
        if(adcDone){
            voteCandidate();            
            adcDone = 0;
        }
    }
}

void __interrupt() isr(void){ //Interrupt service routine
    if(TMR0IF){ // Timer0 interrupt to update 7Segment in every 4 ms
                // To set blinkFlag in every 500 ms
                // To start ADC in every 100ms
                
        segmentISRCounter++;
        if(segmentISRCounter == 10){ // 0.4*10 = 4 ms
            change7Segment(); // Update 7 segment display
            segmentISRCounter = 0;
        }
        if(tmr0Counter % 625 == 0 && !votingEnd){ // Blinking flags sets -> 0.4*625 = 250 ms
            if(tmr0Counter % 1250 == 0){ // Set flag to write '>' to LCD  -> 0.4*1250 = 500 ms
                blinkFlag = 1;
                tmr0Counter = 0;
            }
            else spaceFlag = 1; // Set flag to write ' ' to LCD
        }
        if(tmr0Counter % 250 == 0) startADC(); // ADC conversion -> 0.4*250 = 100 ms
        tmr0Counter++;
        TMR0IF = 0;
        TMR0 = 6;
    }
    if(TMR1IF){ // Timer1 interrupt to decide whether one second is passed or not
        tmr1Counter++;
        if(tmr1Counter % 20 == 0){ // 1 second passed -> 0.05*20 = 1 second
            votingTime--;
            if(votingTime == 0) // Set flag to end program
                votingEnd = 1;
            oneSecond = 1;
        }
        TMR1IF = 0;
        TMR1 = 3036;
    }
    if(RBIF && !votingEnd){ // RB port change interrupt
        __delay_ms(1); // Wait 1 ms to handle bouncing in the RB input pins
        if(isRB6Pressed == 0){ // RB6 pressed
            if(PORTBbits.RB6)
                isRB6Pressed = 1;
        }
        else if(isRB6Pressed == 1){ // RB6 released
            if(!PORTBbits.RB6){
                if(RB7Counter == 0){ // Increment tens digit
                    tensDigit = (++tensDigit)%10;
                    tensFlag = 1;
                }
                else if (RB7Counter == 1){ // Increment units digit
                    unitsDigit = (++unitsDigit)%10;
                    unitsFlag = 1;
                }
                isRB6Pressed = 0;
            }      
        }
        if(isRB7Pressed == 0){ // RB7 pressed
            if(PORTBbits.RB7)
                isRB7Pressed = 1;
        }
        
        else if(isRB7Pressed == 1){ // RB7 released
            if(!PORTBbits.RB7){
                //
                RB7Counter++;
                if(RB7Counter >= 3){ // Set flag to vote
                    voteFlag = 1;
                }
                isRB7Pressed = 0;
            }      
        }
        RBIF = 0;
    }
    if(ADIF){ // ADC interrupt to get value from ADC
        readADC();
        adcDone = 1;
        ADIF = 0;
        ADIE = 0;
    }
}

void main(void) {
    
    initialization();
    start();    
    getVoterCount();
    beginCountdown();
    while (1) {
        if(oneSecond){
            passOneSecond();
            oneSecond = 0;
        }
        if(votingEnd) break;
        blink();
        if(adcDone){
            voteCandidate();            
            adcDone = 0;
        }
    }
    endProgram();
}
