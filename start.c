#include "type.h"
#include "protect.h"

u8 gdt_ptr[6];
Descriptor gdt[GDT_SIZE];

u8 idt_ptr[6];
Gate idt[IDT_SIZE];

u8 *cur_vb = (u8*)0xb8000;

// char fg attribute 
#define HRED 0xc
#define HGREEN 0xa
#define HBLUE 0x9
#define HWHITE 0xf

#define RED 4
#define GREEN 2
#define BLUE 1
#define WHITE 7


void clear()
{
  u8* vb = (u8*)0xb8000;
  const u8 row=25;
  
  for (int x = 0; x < 80*row ; ++x)
  {
    *vb++ = 0x20;
    *vb++ = WHITE;
  }
}

// prefix s32 means simple, 32bit code (in x86 protected mode)
void s32_put_char(u8 ch, u8 *vb)
{
  *vb = ch;
}

void s32_print(const u8 *s, u8 *vb)
{
  while(*s)
  {
    s32_put_char(*s, vb);
    ++s;
    vb+=2;
  }
}

char* s32_itoa(int n, char* str, int radix)
{
  char digit[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char* p=str;
  char* head=str;
  //int radix = 10;

//  if(!p || radix < 2 || radix > 36)
//    return p;
  if (n==0)
  {
    *p++='0';
    *p=0;
    return str;
  }
  if (radix == 10 && n < 0)
  {
    *p++='-';
    n= -n;
  }
  while(n)
  {
    *p++=digit[n%radix];
    //s32_put_char(*(p-1), (u8*)(0xb8000+80*2));
    n/=radix;
  }
  *p=0;
  for (--p; head < p ; ++head, --p)
  {
    char temp=*head;
    if (*(p-1) != '-')
    {
      *head=*p;
      *p=temp;
    }
  }
  return str;
}

void p_asm_memcpy(void *dest, void *src, u16 n);

void c_test()
{
#if 0
{
  u8 stack_str[10]="y";
  u8 *sp = stack_str;
  u16 *p_gdt_limit = (u16*)(&gdt_ptr[0]);
  u32 *p_gdt_base = (u32*)(&gdt_ptr[2]);

  sp = s32_itoa(*p_gdt_limit, stack_str, 10);
  s32_print(sp, (u8*)(0xb8000+160*0));
  sp = s32_itoa(*p_gdt_base, stack_str, 10);
  s32_print(sp, (u8*)(0xb8000+160*1));
}
#endif
}

void init_protected_mode_by_c()
{
  u8 stack_str[10]="y";
  u8 *sp = stack_str;
  //int i=65535;
#if 0
{
  u16 *p_gdt_limit = (u16*)(&gdt_ptr[0]);
  u32 *p_gdt_base = (u32*)(&gdt_ptr[2]);

  sp = s32_itoa(*p_gdt_limit, stack_str, 10);
  s32_print(sp, (u8*)(0xb8000+160*2));
  sp = s32_itoa(*p_gdt_base, stack_str, 10);
  s32_print(sp, (u8*)(0xb8000+160*3));
}
#endif


  p_asm_memcpy(&gdt, (void*)(*((u32*)(&gdt_ptr[2]))), *((u16*)(&gdt_ptr[0])) + 1);

  u16 *p_gdt_limit = (u16*)(&gdt_ptr[0]);
  u32 *p_gdt_base = (u32*)(&gdt_ptr[2]);
  *p_gdt_limit = GDT_SIZE * sizeof(Descriptor) - 1;
  *p_gdt_base = (u32)&gdt;

#if 0
  sp = s32_itoa(*p_gdt_limit, stack_str, 10);
  s32_print(sp, (u8*)(0xb8000+160*4));

  sp = s32_itoa(*p_gdt_base, stack_str, 10);
  s32_print(sp, (u8*)(0xb8000+160*5));
#endif

}

// vector no
#define DIVIDE_NO               0x0
#define DEBUG_NO                0x1
#define NMI_NO                  0x2
#define BREAKPOINT_NO           0x3
#define OVERFLOW_NO             0x4
#define BOUNDS_NO               0x5
#define INVAL_OP_NO             0x6
#define COPROC_NOT_NO           0x7
#define DOUBLE_FAULT_NO         0x8
#define COPROC_SEG_NO           0x9
#define INVAL_TSS_NO            0xA
#define SEG_NOT_NO              0xB
#define STACK_FAULT_NO          0xC
#define PROTECTION_NO           0xD
#define PAGE_FAULT_NO           0xE
#define COPROC_ERR_NO           0x10

void init_idt_by_c()
{
  void init_8259a();

  //s32_print("init_8259a", (u8*)(0xb8000+160*5));
  //init_8259a();

  void init_idt_desc_by_c(u8 vector_no, u8 desc_type, IntHandler handler, u8 privilege);

  u16 *idt_limit = (u16 *)(&idt_ptr[0]);
  u32 *idt_base = (u32 *)(&idt_ptr[2]);
  *idt_limit = IDT_SIZE * sizeof(Gate) - 1;
  *idt_base = (u32)&idt;
//  u8 a=0;

#if 0
  __asm__ volatile ("lidt %0\t\n"
                     :
		     :"m"(idt_ptr)
                   );
#endif
  void divide_error(void);
  void single_step_exception(void);
  void nmi(void);
  void breakpoint_exception(void);
  void overflow(void);
  void bounds_check(void);
  void inval_opcode(void);

#if 1
  init_idt_desc_by_c(DIVIDE_NO, DA_386IGate, divide_error, PRIVILEGE_KRNL);
  init_idt_desc_by_c(DEBUG_NO, DA_386IGate, single_step_exception, PRIVILEGE_KRNL);
  init_idt_desc_by_c(NMI_NO, DA_386IGate, nmi, PRIVILEGE_KRNL);
  init_idt_desc_by_c(BREAKPOINT_NO, DA_386IGate, breakpoint_exception, PRIVILEGE_KRNL);
  init_idt_desc_by_c(OVERFLOW_NO, DA_386IGate, overflow, PRIVILEGE_KRNL);
  init_idt_desc_by_c(BOUNDS_NO, DA_386IGate, bounds_check, PRIVILEGE_KRNL);
  init_idt_desc_by_c(INVAL_OP_NO, DA_386IGate, inval_opcode, PRIVILEGE_KRNL);
  #endif

#if 0
  void spurious_handler(void);

  for (int i=0 ; i <= 6; ++i)
    init_idt_desc_by_c(i, DA_386IGate, spurious_handler, PRIVILEGE_KRNL);
#endif

}

void init_idt_desc_by_c(u8 vector_no, u8 desc_type, IntHandler handler, u8 privilege)
{
  Gate *cur_gate = &idt[vector_no];

  u32 base = (u32)handler;
  cur_gate->offset_low      = base & 0xFFFF;
  cur_gate->selector        = SELECTOR_KERNEL_CS;
  cur_gate->dcount          = 0;
  cur_gate->attr            = desc_type | (privilege << 5);
  cur_gate->offset_high     = (base >> 16) & 0xFFFF;
}

void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags)
{
  clear();
  s32_print("exception_handler", (u8*)(0xb8000+160*24));
}


void ptr_test(u8 *ch)
{
  ch[0] = 'X';
  //*ch = 'X';
}

int AA;
char data_str[]="data_string";

void startc()
{
#if 1
  clear();
  u8 *ro_str="ro_string";
  u8 *vb=(u8*)0xb8000;
  int i=65536;
  int c=AA+1;
  u8 stack_str[10]="y";
  u8 *sp = stack_str;

  *vb = 'W';
  #if 0
  while(1);
  #else
  __asm__ ("nop\n\t");
  __asm__ ("nop\n\t");
  #endif
  #if 0
  u8 ch='Q';
  *(vb+15*2) = ch;
  ptr_test(&ch);
  *(vb+17*2) = ch;
  #endif
  //s32_print(ro_str, vb+80*2);
  sp = s32_itoa(i, stack_str, 10);
  s32_print(sp, (u8*)(0xb8000+160));
  //s32_put_char(sp[0], vb);
  sp = s32_itoa(c, stack_str, 10);
  s32_print(sp, (u8*)(0xb8000+160*2));
  s32_print(data_str, (u8*)(0xb8000+160*3));
  static char *s_str="static_point"; 
  s32_print(s_str, (u8*)(0xb8000+160*4));
  static char s_str_a[]="static_array"; 
  s32_print(s_str_a, (u8*)(0xb8000+160*5));
#endif
}

u8 io_in8(u16 port)
{

// ref: http://wiki.osdev.org/Inline_Assembly/Examples
  u8 ret;
  __asm__ volatile( "inb %1, %0"
               : "=a"(ret) : "Nd"(port) );
  return ret;
}


static inline
void io_out8(u16 port, u8 data)
{
//    outb    %al, $0x92
// N: "N" : Constant in range 0 to 255 (for out instruction).
// ref: http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html#s6
  __asm__ volatile("outb %0, %1"
               : : "a"(data), "Nd"(port) );
  __asm__ volatile("nop\t\n");
  __asm__ volatile("nop\t\n");
}

static inline
void io_out16(u16 port, u16 data)
{
  __asm__ volatile("outw %0, %1"
               : : "a"(data), "Nd"(port) );
  __asm__ volatile("nop\t\n");
  __asm__ volatile("nop\t\n");
}

#define INT_M_PORT 0x20
#define INT_S_PORT 0xa0
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28
#define INT_M_CTLMASK 0x21
#define INT_S_CTLMASK 0xa1

void init_8259a()
{
  // master 8259 icw1
  io_out8(INT_M_PORT, 0x11);

  // slave 8259 icw1
  io_out8(INT_S_PORT, 0x11);

  // master 8259 icw2
  io_out8(INT_M_CTLMASK, INT_VECTOR_IRQ0);

  /* Slave  8259, ICW2. 設置 '從8259' 的中斷入口地址為 0x28 */
  io_out8(INT_S_CTLMASK, INT_VECTOR_IRQ8);

  /* Master 8259, ICW3. IR2 對應 '從8259'. */
  io_out8(INT_M_CTLMASK, 0x4);

  /* Slave  8259, ICW3. 對應 '主8259' 的 IR2. */
  io_out8(INT_S_CTLMASK, 0x2);

  /* Master 8259, ICW4. */
  io_out8(INT_M_CTLMASK, 0x1);

  /* Slave  8259, ICW4. */
  io_out8(INT_S_CTLMASK, 0x1);

  /* Master 8259, OCW1.  */
  io_out8(INT_M_CTLMASK, 0xFF);

  /* Slave  8259, OCW1.  */
  io_out8(INT_S_CTLMASK, 0xFF);

}