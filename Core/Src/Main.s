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

// base address
	.equ     GPIOI_BASE, 	0x58022000 	// I
	.equ     GPIOJ_BASE, 	0x58022400 	// J

// offset registers
	.equ     GPIOx_MODER, 	0x00 		// mode vzhoda/izhoda
	.equ     GPIOx_BSSR,  	0x18 		// atomicno branje in pisanje

////// BSSR /////////////////////////////////////////////////////////////////////
	.equ     I13_S,		0x00002000 // LED red
	.equ     I13_R,		0x20000000 // LED red
	.equ     J2_S,		0x00000004 // LED green
	.equ     J2_R,		0x00040000 // LED green
// ############################################################################

////// SysTick Timer //////////////////////////////////////////////////////////
	.equ     SCS_BASE,		0xe000e000
	.equ     SCS_SYST_CSR,	0x10	// Control/Status register
	.equ     SCS_SYST_RVR,	0x14	// Value to countdown from
	.equ     SCS_SYST_CVR,	0x18	// Current value

	.equ	 SYSTICK_RELOAD_1MS,	63999  //1 msec at 64MHz ...
// ############################################################################

////// LCD ////////////////////////////////////////////////////////////////////
	// LCD Controller Base Address (Hypothetical)
	.equ LCD_CONTROLLER_BASE, 0x60000000

	// LCD Controller Register Offsets
	.equ LCD_CTRL_REG, 0x00   // Control Register
	.equ LCD_DATA_REG, 0x04   // Data Register
	.equ LCD_WIDTH, 800       // Width of the LCD in pixels
	.equ LCD_HEIGHT, 480      // Height of the LCD in pixels

	.equ LCD_ON, 0x00002000
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
	    bl 	INIT_GPIO
	    bl 	INIT_GPIO_I
	    bl 	INIT_GPIO_J

	    bl 	INIT_TC     // Priprava SysTick časovnika
		// bl  INIT_LCD    // Priprava LCD

loop:


		bl LED_RED_ON
		bl LED_GREEN_ON

		mov r0,#500
		bl DELAYTC

		bl LED_RED_OFF
		bl LED_GREEN_OFF

		mov r0,#500
		bl DELAYTC

		b loop          // skok na vrstico loop:


__end: 	b 	__end


// ############################################################################
// ############################################################################
// ############################################################################
// Functions
///////////////////////////////////////////////////////////////////////////////
// INIT methods
///////////////////////////////////////////////////////////////////////////////

INIT_GPIO:
  	push {r0, r1, lr}

	ldr r1, = RCC_AHB4ENR
	ldr r0, [r1]
	orr r0, #0x00000300		// Enable bit 8(I), 9(J)
	str r0, [r1]
	
	pop {r0, r1, pc}


INIT_GPIO_I:
	push {r0, r1, lr}	
	// za vsaka vrata je treba nastavit mode	
	// O: P13(27:26)
	// I: /
	ldr r1, =GPIOI_BASE
	ldr r0, [r1,#GPIOx_MODER] // beremo vrednost mode registran (base + odmik za MODER)
	and r0, #0xF3FFFFFF       // Clear
	orr r0, #0x04000000       // Write
	str r0, [r1]

  	pop {r0, r1, pc}

INIT_GPIO_J:

  	push {r0, r1, lr}
	// za vsaka vrata je treba nastavit mode
	// O: P2(5:4)
	// I: /
	ldr r1, =GPIOJ_BASE
	ldr r0, [r1,#GPIOx_MODER] // beremo vrednost mode registran (base + odmik za MODER)
	and r0, #0xFFFFFFCF       // Clear
	orr r0, #0x00000010       // Write
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

INIT_LCD:
	push {r0, r1, r2, r3, r4, lr}
    // Initialize the LCD controller here according to its specifications
    // For example:
    // 1. Set the resolution
    // 2. Configure display settings
    // 3. Set up the pixel format
    // 4. Any other necessary initialization

    // Once initialized, you can proceed to turn on specific pixels
    // For demonstration purposes, let's turn on a pixel at (x=100, y=200)

    // Calculate the memory address of the pixel (assuming 32-bit color depth)
    // Assuming a simple linear frame buffer:
    // Address = BaseAddress + (y * Width + x) * BytesPerPixel
    // Here, BytesPerPixel is 4 (32-bit color depth)
    ldr r0, =LCD_CONTROLLER_BASE
    ldr r1, =100    // x coordinate
    ldr r2, =200    // y coordinate
    ldr r3, =0xFF00FF00  // Example pixel color (green)

    // Calculate the memory address of the pixel
    mov r4, r2, LSL #10    // y * Width
    add r4, r4, r1, LSL #2 // (y * Width + x) * BytesPerPixel

    add r0, r0, r4    // Add offset to the base address

    // Write the pixel color to the calculated memory address
    str r3, [r0]
	
	pop {r0, r1, r2, r3, r4, lr}

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

LCD_PIXEL:
    push {r1, r2, lr}

	// Set GPIOx Pins to 0 (through BSSR register)
	ldr    r2, =GPIOI_BASE       // Load GPIOI BASE address to r1

	mov    r1, #LCD_ON

	str    r1, [r2,#GPIOx_BSSR] // Write to BSRR register


  	pop {r1, r2, pc}