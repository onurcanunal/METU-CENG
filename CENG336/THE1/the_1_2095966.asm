LIST    P=18F8722

#INCLUDE <p18f8722.inc> 
    
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF
   
ra4_pressed        udata 0X20
ra4_pressed
	
re3_pressed        udata 0X21
re3_pressed
	
re4_pressed        udata 0X22
re4_pressed 
	
seci        udata 0X23
seci 
	
secj        udata 0X24
secj 
	
seck        udata 0X25
seck 
    
ORG     0x00
goto    main

sechelper2:
    decfsz seck
    goto sechelper2
    goto sechelper	
	
sechelper:
    movlw 0xFF
    movwf seck
    decfsz secj
    goto sechelper2
    goto sec
	
sec:
    movlw 0xFF
    movwf secj
    decfsz seci
    goto sechelper
    return
    
init
    clrf LATA
    clrf LATB
    clrf LATC
    clrf LATD
    clrf LATE
    movlw b'00010000'
    movwf TRISA
    movlw 0x00
    movwf TRISB
    movwf TRISC
    movwf TRISD
    movlw b'00011000'
    movwf TRISE
    
    movlw 0x0F
    movwf ADCON1
    movwf LATA
    movwf LATB
    movwf LATC
    movwf LATD
    movlw d'104'
    movwf seci
    call sec
    
    movlw 0x00
    movwf LATA
    movwf LATB
    movwf LATC
    movwf LATD
    movlw d'52'
    movwf seci
    call sec
    
    movlw h'00'
    movwf ra4_pressed
    movwf re3_pressed
    movwf re4_pressed
    return

check_ra4:    
    btfss ra4_pressed,0
    return
    btfsc PORTA,4
    return
    clrf ra4_pressed
    goto state2
    
inc_ra4:
    btfss ra4_pressed,0
    incf ra4_pressed
    return

secswhelper2:
    btfsc PORTA,4
    call inc_ra4
    call check_ra4
    decfsz seck
    goto secswhelper2
    goto secswhelper
    
secswhelper:
    movlw 0xFF
    movwf seck
    decfsz secj
    goto secswhelper2
    goto secsw
	
secsw:
    movlw 0xFF
    movwf secj
    decfsz seci
    goto secswhelper
    return
    
opena:
    btfsc LATA,3
    return
    rlcf LATA,f
    incf LATA,f
    movlw d'8'
    movwf seci
    call secsw
    goto opena
    
openb:
    btfsc LATB,3
    return
    rlcf LATB,f
    incf LATB,f
    movlw d'8'
    movwf seci
    call secsw
    goto openb
    
openc:
    btfsc LATC,3
    return
    rlcf LATC,f
    incf LATC,f
    movlw d'8'
    movwf seci
    call secsw
    goto openc
    
opend:
    btfsc LATD,3
    return
    rlcf LATD,f
    incf LATD,f
    movlw d'8'
    movwf seci
    btfss LATD,3
    call secsw
    goto opend
    
closea:
    btfss LATA,0
    return
    decf LATA,f
    rrncf LATA,f
    movlw d'8'
    movwf seci
    btfsc LATA,0
    call secsw
    goto closea
    
closeb:
    btfss LATB,0
    return
    decf LATB,f
    rrncf LATB,f
    movlw d'8'
    movwf seci
    call secsw
    goto closeb
    
closec:
    btfss LATC,0
    return
    decf LATC,f
    rrncf LATC,f
    movlw d'8'
    movwf seci
    call secsw
    goto closec
    
closed:
    btfss LATD,0
    return
    decf LATD,f
    rrncf LATD,f
    movlw d'8'
    movwf seci
    call secsw
    goto closed

waiting:
    btfss PORTA,4
    goto waiting
    incf ra4_pressed
    goto pressed
    
pressed:
    call check_ra4
    goto pressed
    
check_re3:    
    btfss re3_pressed,0
    return
    btfsc PORTE,3
    return
    clrf re3_pressed
    goto state1
    
inc_re3:
    btfss re3_pressed,0
    incf re3_pressed
    return
    
check_re4:    
    btfss re4_pressed,0
    return
    btfsc PORTE,4
    return
    clrf re4_pressed
    goto state3
    
inc_re4:
    btfss re4_pressed,0
    incf re4_pressed
    return
    
state1:
    call opena
    call openb
    call openc
    call opend
    goto waiting
        
state2:
    btfsc PORTE,3
    call inc_re3
    call check_re3
    btfsc PORTE,4
    call inc_re4
    call check_re4
    goto state2
    
state3:
    call closed
    call closec
    call closeb
    call closea
    goto waiting
    
main:
    call init
    goto state1
end