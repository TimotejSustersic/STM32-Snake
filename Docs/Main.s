.syntax unified
.cpu cortex-m7
.thumb


// Register Addresses
// You can find the base addresses for all peripherals from Memory Map section 2.3.2
// RM0433 on page 131. Then the offsets can be found on their relevant sections.


///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

// Constants///////////////////////////////////////////////////////////////////
	.equ     LEDDELAY,      64000 // 64MHz
// ############################################################################

////// GPIO /////////////////////////////////////////////////////////////////////

// RCC   base address is 0x58024400
//   AHB4ENR register offset is 0xE0
	.equ     RCC_AHB4ENR,	0x580244E0 	// RCC AHB4 peripheral clock reg
	//   APB3ENR register offset is 0x0E4
	.equ     RCC_APB3ENR,	0x580244E4 	// RCC APB3 peripheral clock reg
	.equ     RCC_C1_APB3ENR,	0x58024544 	// RCC APB3 peripheral clock reg

// base address
	.equ     GPIOI_BASE, 	0x58022000 	// I
	.equ     GPIOJ_BASE, 	0x58022400 	// J
	.equ     GPIOK_BASE, 	0x58022800 	// K

// offset registers
	.equ     GPIOx_MODER, 	0x00 		// mode vzhoda/izhoda
	.equ     GPIOx_BSSR,  	0x18 		// atomicno branje in pisanje

////// BSSR /////////////////////////////////////////////////////////////////////
// S -- OFF; R -- ON
	.equ     I9_R,		0x02000000 // LCD VSYNC ON
	.equ     I12_R,		0x10000000 // LCD HSYNC ON
	.equ     I13_S,		0x00002000 // LED red
	.equ     I13_R,		0x20000000
	.equ     I14_R,		0x40000000 // LCD CLC ON
//
	.equ     J2_S,		0x00000004 // LED green
	.equ     J2_R,		0x00040000
//
	.equ     K7_R,		0x00800000 // LCD DE 

	// ENABLI LCD_DE pol pa probi
	
// ############################################################################

////// SysTick Timer //////////////////////////////////////////////////////////
	.equ     SCS_BASE,		0xe000e000
	.equ     SCS_SYST_CSR,	0x10	// Control/Status register
	.equ     SCS_SYST_RVR,	0x14	// Value to countdown from
	.equ     SCS_SYST_CVR,	0x18	// Current value

	.equ	 SYSTICK_RELOAD_1MS,	63999  //1 msec at 64MHz ...
// ############################################################################

////// LCD ////////////////////////////////////////////////////////////////////
	.equ LTDC_BASE, 0x50001000

	.equ LTDC_SSCR, 0x008 // 0x00090001 // synchronization value (HSW-VSH)
	.equ LTDC_BPCR, 0x00C // 0x001D0003 // back porch value (HBP-VBP)
	.equ LTDC_AWCR, 0x010 // 0x015D00F3 // Accumulated active value (AAW-AAH)
	.equ LTDC_TWCR, 0x014 // 0x016700F7 // total value (TOTALW-TOTALH)

	.equ LTDC_GCR,  0x018 // has some properties one allows us to enable LTDC
	.equ LTDC_SRCR, 0x024 // apply shadow registers (vrednosti k smo jih nastavlaj je treba aktiverat)
	.equ LTDC_BCCR, 0x02C // background color register


	.equ LCD_CTRL_REG, 0x00   // Control Register
	.equ LCD_DATA_REG, 0x04   // Data Register
	.equ LTDC_BackGroundColor, 0x0000FFFF  
	.equ INIT_LTDC_BSSR, 0x52000000  
// ############################################################################


// ############################################################################
// ############################################################################
// ############################################################################
// Main
///////////////////////////////////////////////////////////////////////////////

// DOCS
// REGISTERS USAGE
// r0 used as delay param (if delay alyays the same optimise storage)

// Start of text section
        .text

        .type  main, %function
        .global main

   	   	.align
main:
		// I/O
	    bl 	INIT_GPIO
	    bl 	INIT_GPIO_I
	    bl 	INIT_GPIO_J
	    bl 	INIT_GPIO_K

	    bl 	INIT_TC     // Priprava SysTick časovnika
		bl  INIT_LTDC   // Priprava LTDC

loop:
		//bl LED_RED_ON
		//bl LED_GREEN_ON

		//mov r0,#500
		//bl DELAYTC

		// bl LED_RED_OFF
		// bl LED_GREEN_OFF

		//mov r0,#500
		//bl DELAYTC

		//b loop          // skok na vrstico loop:


__end: 	b 	__end


// ############################################################################
// ############################################################################
// ############################################################################
// INIT methods
///////////////////////////////////////////////////////////////////////////////

INIT_GPIO:
  	push {r0, r1, lr}
	ldr r1, =RCC_AHB4ENR
	ldr r0, [r1]
	orr r0, #0x00000700		// Enable bit 8(I), 9(J), 10(K)
	str r0, [r1]	
	pop {r0, r1, pc}

INIT_GPIO_I:
	push {r0, r1, r2, r3, lr}	
	// za vsaka vrata je treba nastavit mode	
	// O: P14(29:28), P13(27:26), P12(25:24), P9(19:18)
	// I: /
	ldr r1, =GPIOI_BASE
	ldr r0, [r1,#GPIOx_MODER] // beremo vrednost mode registran (base + odmik za MODER)
	// 0xC0F3FFFF
	mov r3, #0xC0000000
	add r3, r3, #0xF30000
	add r3, r3, #0xFF00
	add r3, r3, #0xFF
	and r0, r3       // Clear

	mov r2, #0x10000000
	add r2, r2, #0x05000000
	add r2, r2, #0x40000
	orr r0, r2       // Write

	str r0, [r1]
  	pop {r0, r1, r2, r3, pc}

INIT_GPIO_J:
  	push {r0, r1, lr}
	// za vsaka vrata je treba nastavit mode
	// O: P2(5:4)
	// I: /
	ldr r1, =GPIOJ_BASE
	ldr r0, [r1,#GPIOx_MODER] // beremo vrednost mode registran (base + odmik za MODER)
	and r0, #0x00000000       // Clear
	orr r0, #0x55555555       // Write
	str r0, [r1]
  	pop {r0, r1, pc}

INIT_GPIO_K:
  	push {r0, r1, lr}
	// za vsaka vrata je treba nastavit mode
	// O: P2(5:4)
	// I: /
	ldr r1, =GPIOK_BASE
	ldr r0, [r1,#GPIOx_MODER] // beremo vrednost mode registran (base + odmik za MODER)
	and r0, #0xFFFF3FFF       // Clear 
	orr r0, #0x00004000       // Write
	str r0, [r1]
  	pop {r0, r1, pc}

INIT_TC:
  	push {r0, r1, lr}
	ldr r1, =SCS_BASE

	ldr r0, =SYSTICK_RELOAD_1MS
	str r0, [r1, #SCS_SYST_RVR]

	mov r0, #0
	str r0, [r1, #SCS_SYST_CVR]

	mov r0, #0b101
	str r0, [r1, #SCS_SYST_CSR]

  	pop {r0, r1, pc}

INIT_LTDC:
	push {r0, r1, lr}
	
	// 1. Enable the LTDC clock in the RCC register (breaks device)
	ldr r1, =RCC_APB3ENR
	// ldr r1, =RCC_C1_APB3ENR
	ldr r0, [r1]
	orr r0, #0x00000008		// Enable bit 3(LCD)
	str r0, [r1]




	// 2. Configure the required pixel clock following the panel datasheet.
	// ne vem tocno kaj je panel datasheet ampak sklepam da to pomeni nadalnje korake

	// so ze po defaultu enablani
	@ ldr r1, =GPIOI_BASE
	@ mov r0, #I14_R
	@ str r0, [r1,#GPIOx_BSSR]

	@ ldr r1, =GPIOI_BASE
	@ mov r0, #I9_R
	@ str r0, [r1,#GPIOx_BSSR]	

	@ ldr r1, =GPIOI_BASE
	@ mov r0, #I12_R
	@ str r0, [r1,#GPIOx_BSSR]	
	
	// ve skupi naenkrt aktiviramo
	@ ldr r1, =GPIOI_BASE
	@ mov r0, #INIT_LTDC_BSSR
	@ str r0, [r1,#GPIOx_BSSR]

	// 3. Configure the synchronous timings: VSYNC, HSYNC, vertical and horizontal back 
	// 	  porch, active data area and the front porch timings
	ldr r1, =LTDC_BASE 
	// SSCR
	@ mov r0, #0x00090001
	@ str r0, [r1,#LTDC_SSCR]
	@ // BPCR
	@ mov r0, #0x001D0003
	@ str r0, [r1,#LTDC_BPCR]	
	@ // AWCR
	@ mov r0, #0x015D00F3
	@ str r0, [r1,#LTDC_AWCR]	
	@ // TWCR
	@ mov r0, #0x016700F7
	@ str r0, [r1,#LTDC_TWCR]	

	// 4. Configure the synchronous signals and clock polarity in the LTDC_GCR register.
	// tuki nastavis lahko low ali high ne vem ker pa more bt nastavljen pa kaj pomenijo

	// 5. Spremenimo backround color
	ldr r1, =LTDC_BASE 
	mov r0, #LTDC_BackGroundColor
	str r0, [r1,#LTDC_BCCR]	

	// 6. Configure the needed interrupts in the LTDC_IER and LTDC_LIPCR register
	// Interuptov ne rabim

	// 7. Configure the layer1/2 parameters
	// glede an to da mam CLC sistem ne vem ce je to potrebno ker je vmesna plast

	// 8. Enable layer1/2 and if needed the CLUT in the LTDC_LxCR register
	// Ne vem spet ce jo rabim ampak bom za ziher enablu

	// enabla morm layer 1 in disablat layer 2 pa mu nastavit blending factor na 0x00 torej alfa

	// 9. If needed, enable dithering and color keying respectively in the LTDC_GCR and 
	// LTDC_LxCKCR registers. They can be also enabled on the fly.
	// diether ne rabm

	// 10. Reload the shadow registers to active register through the LTDC_SRCR register.
	ldr r1, =LTDC_BASE 
	// SSCR
	mov r0, #0x00000001     // bit 0 enable
	str r0, [r1,#LTDC_SRCR]

	// 11. Enable the LTDC controller in the LTDC_GCR register.
	ldr r1, =LTDC_BASE
	ldr r0, [r1,#LTDC_GCR]
	orr r0, #0x00000001     // bit 0 enable
	str r0, [r1]

//////////////
    // Calculate the memory address of the pixel
    @ mov r4, r2, LSL #10    // y * Width
    @ add r4, r4, r1, LSL #2 // (y * Width + x) * BytesPerPixel

    @ add r0, r0, r4    // Add offset to the base address

    @ // Write the pixel color to the calculated memory address
    @ str r3, [r0]
	
	pop {r0, r1, lr}

// ############################################################################
// LED
///////////////////////////////////////////////////////////////////////////////

LED_RED_ON:
    push {r0, r1, lr}
	ldr    r1, =GPIOI_BASE
	mov    r0, #I13_R
	str    r0, [r1,#GPIOx_BSSR]
  	pop {r0, r1, pc}

LED_RED_OFF:
    push {r0, r1, lr}
	ldr    r1, =GPIOI_BASE
	mov    r0, #I13_S
	str    r0, [r1,#GPIOx_BSSR]
  	pop {r0, r1, pc}

LED_GREEN_ON:
    push {r0, r1, lr}
	ldr    r1, =GPIOJ_BASE 
	mov    r0, #J2_R
	str    r0, [r1,#GPIOx_BSSR]	
  	pop {r0, r1, pc}

LED_GREEN_OFF:
    push {r0, r1, lr}
	ldr    r1, =GPIOJ_BASE
	mov    r0, #J2_S
	str    r0, [r1,#GPIOx_BSSR] 
  	pop {r0, r1, pc}

// ############################################################################
// DELAYTC
///////////////////////////////////////////////////////////////////////////////

// Delay with internal timer based loop approx. r0 x ms
DELAYTC:
    push {r1, r2, lr}
    ldr r1, =SCS_BASE


DELAYTC_LOOP:ldr r2, [r1, #SCS_SYST_CSR]
			 tst r2, #0x10000   // COUNT_FLAG = 1 ?
			 beq DELAYTC_LOOP

		subs r0, r0, #1
		bne DELAYTC_LOOP
	pop {r1, r2, pc}


// ############################################################################
// LCD
///////////////////////////////////////////////////////////////////////////////