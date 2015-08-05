#include <kernel/tty.h>
#include <kernel/portio.h>
#include <kernel/kbdus.h>

#include "../arch/i386/log.h"

#define KB_SCANCODE 0x60
#define KB_BRAKECODE 0x80
#define KB_MAXBUFFER 255

volatile char* kb_buffer;
volatile char* kb_buffer_raw; //raw data

volatile int kb_count = 0; //Position in buffer
volatile int kb_count_raw = 0; //Position in buffer

unsigned short kb_ltmp;
volatile int kb_shift_flag=0;
volatile int kb_caps_flag=0;

char* get_buffer;
int get_count;
int get = 0;

/* Handles the keyboard interrupt */
void keyboard_handler(struct interrupt_context* int_ctx)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inport8(KB_SCANCODE);

    //store raw buffer data
    if (kb_count_raw < KB_MAXBUFFER)
    {
      kb_buffer_raw[kb_count_raw++] = scancode;
    }
    else
    {
      kb_count_raw = 0;
      kb_buffer_raw[kb_count_raw] = scancode;
    }

    //Set LED lights
    switch (scancode)
    {
      case 0x3A:
        /* CAPS_LOCK LEDS */
        outport8(0x60,0xED);
        kb_ltmp |= 4;
        outport8(0x60,kb_ltmp);
        
        if(kb_caps_flag)
          kb_caps_flag=0;
        else
           kb_caps_flag=1;
      break;

      case 0x45:
        /* NUM_LOCK LEDS */
        outport8(0x60,0xED);
        kb_ltmp |= 2;
        outport8(0x60,kb_ltmp);
      break;

      case 0x46:
        /* SCROLL_LOCK LEDS */
        outport8(0x60,0xED);
        kb_ltmp |= 1;
        outport8(0x60,kb_ltmp);
      break;
      
      case 60: /* F12 */
        //reboot();
      break;
      
      default:
        break;
    }

    //check for brake codes
    if (scancode & KB_BRAKECODE)
    {
      //TODO add Key release
      //Left and right shifts
      if (scancode - KB_BRAKECODE == 42 || scancode - KB_BRAKECODE == 54)
        kb_shift_flag = 0;
    }
    else
    {
      //TODO add Key press
      //Shift
      if (scancode == 42 || scancode == 54)
        {
          kb_shift_flag = 1;
          return;
        }

      //TODO add uppercase
      //store buffer data
      if (kb_count < KB_MAXBUFFER)
      {
        kb_buffer[kb_count_raw++] = kbdus[scancode];
      }
      else
      {
        kb_count = 0;
        kb_buffer[kb_count] = kbdus[scancode];
      }

      //TODO add echo toggle
      terminal_putchar(kbdus[scancode]);
    }
}

/* Installs the keyboard handler into IRQ1 */
void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
    log_print(NOTICE, "Keyboard");
}