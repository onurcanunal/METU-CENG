;Ali Ilker SIGIRCI - 2171940
;Onurcan UNAL - 2095966

list P=18F8722

#include <p18f8722.inc>
config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF
    
tmr0_counter udata 0x21
tmr0_counter
 
tmr1h_temp udata 0x22
tmr1h_temp
 
tmr1l_temp udata 0x23
tmr1l_temp
 
w_temp  udata 0x24
w_temp

status_temp udata 0x25
status_temp

pclath_temp udata 0x26
pclath_temp
 
asteroid_counter udata 0x27
asteroid_counter
 
tmr0_counter2 udata 0x28
tmr0_counter2
 
lasA udata 0x29
lasA

lasB udata 0x30
lasB
 
lasC udata 0x31
lasC
 
lasD udata 0x32
lasD
 
lasE udata 0x33
lasE
 
lasF udata 0x34
lasF
 
astA udata 0x35
astA

astB udata 0x36
astB
 
astC udata 0x37
astC
 
astD udata 0x38
astD
 
astE udata 0x39
astE
 
astF udata 0x40
astF
 
tmr1h_temp2 udata 0x41
tmr1h_temp2
 
tmr1l_temp2 udata 0x42
tmr1l_temp2
 
shift_counter udata 0x43
shift_counter
 
buttonflag udata 0x44
buttonflag
 
temp udata 0x45
temp
 
scorecounter udata 0x46
scorecounter
 
temp2 udata 0x47
temp2
 
goflag udata 0x48
goflag
 
segmentflag udata 0x49
segmentflag
 
score0 udata 0x50
score0
 
score1 udata 0x51
score1
 
score2 udata 0x52
score2
 
score3 udata 0x53
score3
    
org     0x00
goto    init

org     0x08
goto    isr             ;go to interrupt service routine
    
init:
    ;disable interrupts
    clrf INTCON
    
    ;configure output ports
    clrf LATA
    movlw 0xFF
    movwf ADCON1 ;for digital inputs
    clrf TRISA
    
    clrf LATB
    clrf TRISB
    
    clrf LATC
    clrf TRISC
    
    clrf LATD
    clrf TRISD
    
    clrf LATE
    clrf TRISE
    
    clrf LATF
    clrf TRISF
    
    clrf LATH
    clrf TRISH
    
    clrf LATJ
    clrf TRISJ
    
    clrf lasA
    clrf lasB
    clrf lasC
    clrf lasD
    clrf lasE
    clrf lasF
    clrf astA
    clrf astB
    clrf astC
    clrf astD
    clrf astE
    clrf astF
    clrf tmr1h_temp
    clrf tmr1l_temp
    clrf tmr1h_temp2
    clrf tmr1l_temp2
    clrf shift_counter
    clrf buttonflag
    clrf scorecounter
    clrf temp
    clrf temp2
    clrf goflag
    clrf asteroid_counter
    clrf segmentflag
    clrf score0
    clrf score1
    clrf score2
    clrf score3
    
    ;configure input port
    clrf LATG
    movlw b'00001111'
    movwf TRISG
    
    ;initialize timer0
    movlw b'01000100' ;disable timer0 by setting tmr0on to 0 (for now)
		      ;configure timer0 as an 8-bit timer/counter by setting t08bit to 1
		      ;1:32 prescaler assigned
    movwf T0CON
    
    ;initialize timer1
    movlw b'11000000'
    movwf T1CON
    
    ;enable interrupts
    movlw b'11100000' ;enable global, peripheral and  TMR0 by setting GIE, PEIE and TMR0IE bits to 1
    movwf INTCON
    
    bsf T1CON, 0    ;Enable Timer1 by setting TMR1ON to 1
    
    ;initial configuration
    bsf LATA, 3 ;the spaceship should be on RA3 at the start
		        ;score must be 0 
    movlw b'00111111'
    movwf LATJ
    bsf LATH, 3
    bsf lasA, 3
		
play:
    btfss PORTG, 0
    goto play
    bsf T0CON, 7    ;Enable Timer0 by setting TMR0ON to 1
    movlw 0x06	    ;decimal 6 initial value to count 250 (250*32 = 0.8ms)
    movwf TMR0	      
    movff TMR1H, tmr1h_temp
    movff TMR1L, tmr1l_temp
		
main:
    btfss lasA, 0
    call moveup
    btfsc goflag, 0
    goto init
    btfss lasA, 5
    call movedown
    btfsc goflag, 0
    goto init
    movf lasA, w
    iorwf astA, w
    movwf LATA
    btfsc PORTG, 1 ;fire the laser beams
    call fireinthehole
    movf lasB, w
    iorwf astB, w
    movwf LATB
    goto main
        
moveup:    
    btfss PORTG, 3 ;move spaceship up
    return
    bsf buttonflag, 0
    
isreleasedup:
    btfsc PORTG, 3
    goto isreleasedup
    clrf buttonflag, 0
    rrncf lasA, f
    movf lasA, w
    andwf astA, w
    btfsc STATUS, Z
    return
    movff astA, LATA
    bsf goflag, 0
    return
    
movedown:    
    btfss PORTG, 2 ;move spaceship down
    return
    bsf buttonflag, 0
    
isreleaseddown:
    btfsc PORTG, 2
    goto isreleaseddown
    clrf buttonflag, 0
    rlncf lasA, f
    movf lasA, w
    andwf astA, w
    btfsc STATUS, Z
    return
    movff astA, LATA
    bsf goflag, 0
    return
    
fireinthehole:
    btfss PORTG, 1 ;fire the laser beams
    return
    bsf buttonflag, 0

isreleasedfire:
    btfsc PORTG, 1
    goto isreleasedfire
    clrf buttonflag, 0    
    btfsc lasA, 0
    bsf lasB, 0
    btfsc lasA, 1
    bsf lasB, 1
    btfsc lasA, 2
    bsf lasB, 2
    btfsc lasA, 3
    bsf lasB, 3
    btfsc lasA, 4
    bsf lasB, 4
    btfsc lasA, 5
    bsf lasB, 5
    return
    
isr:
    call save_registers
    incf segmentflag, f
    movlw d'6'
    cpfslt segmentflag
    call display
    movlw d'10'
    cpfsgt asteroid_counter ;The first 10 asteroids should move to the next location in every 500 ms.
    goto interrupt500
    movlw d'30'
    cpfsgt asteroid_counter ;After the first 10 asteroids, newly created 20 asteroids should move in every 400 ms.
    goto interrupt400
    movlw d'50'
    cpfsgt asteroid_counter ;After the first 30 asteroids, newly created 20 asteroids should move in every 300 ms.
    goto interrupt300
    goto interrupt200 ;After the first 50 asteroids, the rest of the newly created asteroids should move in every 200 ms.
    
interrupt500:
    incf tmr0_counter, f
    movf tmr0_counter, w
    sublw d'125' ;625 * 0.8 = 500msec
    btfss STATUS, Z
    goto interrupt_exit
    clrf tmr0_counter
    incf tmr0_counter2, f
    movf tmr0_counter2, w
    sublw d'5'
    btfss STATUS, Z
    goto interrupt_exit
    goto interrupt_move
    
interrupt400:
    incf tmr0_counter, f
    movf tmr0_counter, w
    sublw d'125' ;500 * 0.8 = 400msec
    btfss STATUS, Z
    goto interrupt_exit
    clrf tmr0_counter
    incf tmr0_counter2, f
    movf tmr0_counter2, w
    sublw d'4'
    btfss STATUS, Z
    goto interrupt_exit
    goto interrupt_move
    
interrupt300:
    incf tmr0_counter, f
    movf tmr0_counter, w
    sublw d'125' ;375 * 0.8 = 300msec
    btfss STATUS, Z
    goto interrupt_exit
    clrf tmr0_counter
    incf tmr0_counter2, f
    movf tmr0_counter2, w
    sublw d'3'
    btfss STATUS, Z
    goto interrupt_exit
    goto interrupt_move

interrupt200:
    incf tmr0_counter, f
    movf tmr0_counter, w
    sublw d'125' ;250 * 0.8 = 200msec
    btfss STATUS, Z
    goto interrupt_exit
    clrf tmr0_counter
    incf tmr0_counter2, f
    movf tmr0_counter2, w
    sublw d'2'
    btfss STATUS, Z
    goto interrupt_exit
    goto interrupt_move
    
interrupt_move:
    clrf tmr0_counter2
    ;YAPILACAKLAR
    ;Begin Control
    ;begin game over control
    movf lasA, w
    andwf astA, w
    btfss STATUS, Z
    goto gameover
    ;end game over control

    ;begin B-B
    movf  lasB, w
    andwf astB, w
    movwf temp
    comf temp, w
    andwf lasB, f
    andwf astB, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end B-B

    ;begin B-C
    movf  lasB, w
    andwf astC, w
    movwf temp
    comf temp, w
    andwf lasB, f
    andwf astC, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end B
    ;begin C-C
    movf  lasC, w
    andwf astC, w
    movwf temp
    comf temp, w
    andwf lasC, f
    andwf astC, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end C-C

    ;begin C-D
    movf  lasC, w
    andwf astD, w
    movwf temp
    comf temp, w
    andwf lasC, f
    andwf astD, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end C-D

    ;begin D-D
    movf  lasD, w
    andwf astD, w
    movwf temp
    comf temp, w
    andwf lasD, f
    andwf astD, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end D-D

    ;begin D-E
    movf  lasD, w
    andwf astE, w
    movwf temp
    comf temp, w
    andwf lasD, f
    andwf astE, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end D-E
    
    ;begin E-E
    movf  lasE, w
    andwf astE, w
    movwf temp
    comf temp, w
    andwf lasE, f
    andwf astE, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end E-E
    
    ;begin E-F
    movf  lasE, w
    andwf astF, w
    movwf temp
    comf temp, w
    andwf lasE, f
    andwf astF, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end E-F

    ;begin F-F
    movf  lasF, w
    andwf astF, w
    movwf temp
    comf temp, w
    andwf lasF, f
    andwf astF, f
    btfsc temp, 0
    call incrementscore
    btfsc temp, 1
    call incrementscore
    btfsc temp, 2
    call incrementscore
    btfsc temp, 3
    call incrementscore
    btfsc temp, 4
    call incrementscore
    btfsc temp, 5
    call incrementscore
    ;end F-F

    ;End Control

    ;Begin shift

    movff lasE, lasF
    movff lasD, lasE
    movff lasC, lasD
    movff lasB, lasC

    movff astB, astA
    movff astC, astB
    movff astD, astC
    movff astE, astD
    movff astF, astE
    
    clrf astF
    clrf lasB

    ;End shift
    
    ;;;
    movlw b'00000111' ;clear 3 4 5 6 7
    andwf tmr1l_temp , w
    xorlw b'00000000' ;case 0
    btfsc STATUS, Z
    bsf astF, 0
    xorlw b'00000001' ;case 1
    btfsc STATUS, Z
    bsf astF, 1
    xorlw b'00000011' ;case 2
    btfsc STATUS, Z
    bsf astF, 2
    xorlw b'00000001' ;case 3
    btfsc STATUS, Z
    bsf astF, 3
    xorlw b'00000111' ;case 4
    btfsc STATUS, Z
    bsf astF, 4
    xorlw b'00000001' ;case 5
    btfsc STATUS, Z
    bsf astF, 5
    xorlw b'00000011' ;case 6
    btfsc STATUS, Z
    bsf astF, 0
    xorlw b'00000001' ;case 7
    btfsc STATUS, Z
    bsf astF, 1
    movff tmr1h_temp, tmr1h_temp2
    movff tmr1l_temp, tmr1l_temp2
    rrncf tmr1h_temp
    rrncf tmr1l_temp
    btfsc tmr1h_temp2, 0
    bsf tmr1l_temp, 7
    btfss tmr1h_temp2, 0
    bcf tmr1l_temp, 7
    btfsc tmr1l_temp2, 0
    bsf tmr1h_temp, 7
    btfss tmr1l_temp2, 0
    bcf tmr1h_temp, 7
    incf shift_counter, f
    movf shift_counter, w
    sublw d'10'
    btfss STATUS, Z
    goto create_asteroid
    comf tmr1h_temp, f
    comf tmr1l_temp, f
    clrf shift_counter
    
create_asteroid:
    movff astF, temp2
    comf astF, w
    andwf lasF, w
    movwf temp
    comf lasF, w
    andwf astF, f
    movff temp, lasF
    movf astF, w
    cpfseq temp2
    call incrementscore
    movf astF, w
    iorwf lasF, w
    movwf LATF
    movlw d'100'
    cpfseq asteroid_counter
    incf asteroid_counter
    
openLeds:
    movf astB, w
    iorwf lasB, w
    movwf LATB
    movf astC, w
    iorwf lasC, w
    movwf LATC
    movf astD, w
    iorwf lasD, w
    movwf LATD
    movf astE, w
    iorwf lasE, w
    movwf LATE
    
interrupt_exit:
    bcf INTCON, 2     ;clear TMROIF
    movlw 0x06	      ;decimal 6 initial value to count 250 (250*16 = 0.4ms)
    movwf TMR0
    call restore_registers
    retfie
    
save_registers:
    movwf 	w_temp          ;Copy W to TEMP register
    swapf 	STATUS, w       ;Swap status to be saved into W
    clrf 	STATUS          ;bank 0, regardless of current bank, Clears IRP,RP1,RP0
    movwf 	status_temp     ;Save status to bank zero STATUS_TEMP register
    movf 	PCLATH, w       ;Only required if using pages 1, 2 and/or 3
    movwf 	pclath_temp     ;Save PCLATH into W
    clrf 	PCLATH          ;Page zero, regardless of current page
    return
    
restore_registers:
    movf 	pclath_temp, w  ;Restore PCLATH
    movwf 	PCLATH          ;Move W into PCLATH
    swapf 	status_temp, w  ;Swap STATUS_TEMP register into W
    movwf 	STATUS          ;Move W into STATUS register
    swapf 	w_temp, f       ;Swap W_TEMP
    swapf 	w_temp, w       ;Swap W_TEMP into W
    return
    
gameover:
    bcf INTCON, 2     ;clear TMROIF
    bsf goflag, 0
    call restore_registers
    retfie
    
display:
    clrf segmentflag
    clrf LATH
    movlw d'0'
    cpfseq scorecounter
    goto led1

led0:
    movf score0, w
    call segment
    movwf LATJ
    bsf LATH, 3
    incf scorecounter
    return
    
led1:
    movlw d'1'
    cpfseq scorecounter
    goto led2
    movf score1, w
    call segment
    movwf LATJ
    bsf LATH, 2
    incf scorecounter
    return
    
led2:
    movlw d'2'
    cpfseq scorecounter
    goto led3
    movf score2, w
    call segment
    movwf LATJ
    bsf LATH, 1
    incf scorecounter
    return
    
led3:
    movf score3, w
    call segment
    movwf LATJ
    bsf LATH, 0
    clrf scorecounter
    return
    
segment:
    xorlw b'00000000' ;case 0
    btfsc STATUS, Z
    retlw b'00111111' ;
    xorlw b'00000001' ;case 1
    btfsc STATUS, Z
    retlw b'00000110' ;
    xorlw b'00000011' ;case 2
    btfsc STATUS, Z
    retlw b'01011011' ;
    xorlw b'00000001' ;case 3
    btfsc STATUS, Z
    retlw b'01001111' ;
    xorlw b'00000111' ;case 4
    btfsc STATUS, Z
    retlw b'01100110' ;
    xorlw b'00000001' ;case 5
    btfsc STATUS, Z
    retlw b'01101101' ;
    xorlw b'00000011' ;case 6
    btfsc STATUS, Z
    retlw b'01111101' ;
    xorlw b'00000001' ;case 7
    btfsc STATUS, Z
    retlw b'00000111' ;
    xorlw b'00001111' ;case 8
    btfsc STATUS, Z
    retlw b'01111111' ;
    xorlw b'00000001' ;case 9
    btfsc STATUS, Z
    retlw b'01100111' ;
    
incrementscore:
    incf score0, f
    movlw d'9'
    cpfsgt score0
    return 
    incf score1, f
    clrf score0
    movlw d'9'
    cpfsgt score1
    return
    incf score2, f
    clrf score1
    movlw d'9'
    cpfsgt score2
    return 
    incf score3, f
    clrf score2
end