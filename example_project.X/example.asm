	#include <p16f84a.INC>
	__config _XT_OSC & _WDT_OFF & _PWRTE_ON & _CP_OFF

	cblock 0xC
		main_counter

		wait
		wait_more

		bit_pattern_output

		toggling_bit
		set_bit_input
		bit_register
		bit_number
		register_pattern
		bit_counter
	endc

	ORG 0x0
	GOTO start

	; Interrupt handler (unused)
	ORG 0x4
	RETFIE

start
	CLRF PORTA
	CLRF PORTB
	BSF STATUS, RP0

	MOVLW b'00000000'
	MOVWF TRISB
	MOVLW b'00000'
	MOVWF TRISA

	BCF STATUS, RP0

	MOVLW PORTB
	MOVWF bit_register

main_loop

	MOVLW 0x8
	MOVWF main_counter
inner_loop
	MOVF main_counter, W
	MOVWF bit_number
	DECF bit_number, F

	CALL toggle_bit

	DECFSZ main_counter, F
		GOTO inner_loop

	GOTO main_loop

; Calculates the bit pattern required to toggle a bit with xor
; Example: W = 4
;       => W = b'00010000'
;
; Input: W
; Output: W
bit_pattern
	CLRF bit_pattern_output

	ADDLW 0x1
	BSF STATUS, C ; For RLF
bit_pattern_loop
	RLF bit_pattern_output, F
	ADDLW 0xFF ; Same as W = W - 1
	BCF STATUS, C ; Clear carry
	BTFSS STATUS, Z
		GOTO bit_pattern_loop

	MOVF bit_pattern_output, W

	RETURN

; Toggles a bit in a F register
; Example: bit_register = 32, bit_number = 3, [32] = b'11001100'
;       => [32] = b'11000100'
;
; Input: bit_register, bit_number
; Output: [bit_register]
toggle_bit
	BSF toggling_bit, 0
	GOTO set_bit_main

; Sets or clears a bit in a F register, depending on W
; Example: bit_register = 32, bit_number = 3, W = 1, [32] = b'01010101'
;       => [32] = b'01011101'
;
; Input: bit_register, bit_number, W
; Output: [bit_register]
set_bit
	BCF toggling_bit, 0
set_bit_main
	MOVWF set_bit_input

	MOVF bit_register, W
	MOVWF FSR

	MOVF bit_number, W
	CALL bit_pattern
	MOVWF register_pattern

	MOVF INDF, W

	BTFSC toggling_bit, 0
		GOTO bit_toggle

	BTFSS set_bit_input, 0
		GOTO bit_off

bit_on
	IORWF register_pattern, W
	MOVWF INDF

	RETURN

bit_off
	COMF register_pattern, F
	ANDWF register_pattern, W
	MOVWF INDF

	RETURN

bit_toggle
	XORWF register_pattern, W
	MOVWF INDF

	RETURN

; Waits a number of clock cycles which is approximately proportional to W
; cycles = 2 + (4 + 0x20) * W = 2 + 128 * W
;
; Input: W
; Output: -
delay
	MOVWF wait_more
delay_loop2
	MOVLW 0x20
	MOVWF wait
delay_loop1
	DECFSZ wait, F
		GOTO delay_loop1

	DECFSZ wait_more, F
		GOTO delay_loop2

	RETURN

	END
