.syntax unified
.cpu cortex-m3
.thumb

.global g_pfnVectors
.global Reset_Handler
.global _init
.word _estack

.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
g_pfnVectors:
  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler
  /* STM32F103 medium-density IRQ 0..19. */
  .rept 20
  .word Default_Handler
  .endr
  /* IRQ 20: USB low-priority / CAN1 RX0. */
  .word USB_LP_CAN1_RX0_IRQHandler
  .rept 22
  .word Default_Handler
  .endr
.size g_pfnVectors, .-g_pfnVectors

.section .text.Reset_Handler,"ax",%progbits
.type Reset_Handler, %function
Reset_Handler:
  ldr r0, =_sidata
  ldr r1, =_sdata
  ldr r2, =_edata
1:
  cmp r1, r2
  bcc 2f
  b 3f
2:
  ldr r3, [r0], #4
  str r3, [r1], #4
  b 1b
3:
  ldr r1, =_sbss
  ldr r2, =_ebss
  movs r3, #0
4:
  cmp r1, r2
  bcc 5f
  b 6f
5:
  str r3, [r1], #4
  b 4b
6:
  bl SystemInit
  bl __libc_init_array
  bl main
7:
  b 7b
.size Reset_Handler, .-Reset_Handler

.section .text._init,"ax",%progbits
.type _init, %function
_init:
  bx lr
.size _init, .-_init

.section .text.Default_Handler,"ax",%progbits
Default_Handler:
  b Default_Handler

.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler
.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler
.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler
.weak BusFault_Handler
.thumb_set BusFault_Handler, Default_Handler
.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler
.weak SVC_Handler
.thumb_set SVC_Handler, Default_Handler
.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler
.weak PendSV_Handler
.thumb_set PendSV_Handler, Default_Handler
.weak SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler
.weak USB_LP_CAN1_RX0_IRQHandler
.thumb_set USB_LP_CAN1_RX0_IRQHandler, Default_Handler
