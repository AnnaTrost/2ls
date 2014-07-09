/* Generated by CIL v. 1.3.7 */
/* print_CIL_Input is true */

#line 1 "stateful_check.c"
void __blast_assert(void) 
{ 

  {
  ERROR: 
  goto ERROR;
}
}
#line 7 "stateful_check.c"
int ldv_mutex  =    1;
#line 9 "stateful_check.c"
int open_called  =    0;
#line 10
int __VERIFIER_nondet_int(void) { int x; return x; }
#line 12 "stateful_check.c"
void mutex_lock(void) 
{ 

  {
#line 14
  if (ldv_mutex == 1) {

  } else {
    {
#line 14
    __blast_assert();
    }
  }
#line 15
  ldv_mutex = 2;
#line 16
  return;
}
}
#line 18 "stateful_check.c"
void mutex_unlock(void) 
{ 

  {
#line 20
  if (ldv_mutex == 2) {

  } else {
    {
#line 20
    __blast_assert();
    }
  }
#line 21
  ldv_mutex = 1;
#line 22
  return;
}
}
#line 24 "stateful_check.c"
void check_final_state(void) 
{ 

  {
#line 26
  if (ldv_mutex == 1) {

  } else {
    {
#line 26
    __blast_assert();
    }
  }
#line 27
  return;
}
}
#line 29 "stateful_check.c"
static int misc_release(void) 
{ 

  {
#line 32
  if (open_called) {
    {
#line 34
    mutex_lock();
#line 35
    mutex_unlock();
#line 36
    open_called = 0;
    }
  } else {
    {
#line 39
    mutex_lock();
#line 40
    mutex_lock();
    }
  }
#line 42
  return (0);
}
}
#line 45 "stateful_check.c"
static int misc_llseek(void) 
{ 

  {
#line 46
  return (0);
}
}
#line 49 "stateful_check.c"
static int misc_read(void) 
{ 

  {
#line 50
  return (0);
}
}
#line 53 "stateful_check.c"
static int misc_open(void) 
{ int tmp ;

  {
  {
#line 55
  tmp = __VERIFIER_nondet_int();
  }
#line 55
  if (tmp) {
#line 57
    return (1);
  } else {
#line 59
    open_called = 1;
#line 60
    return (0);
  }
}
}
#line 64 "stateful_check.c"
static int my_init(void) 
{ 

  {
#line 67
  open_called = 0;
#line 68
  return (0);
}
}
#line 76
extern int ( /* missing proto */  __VERIFIER_nondet_int)() ;
#line 71 "stateful_check.c"
void main(void) 
{ int ldv_s_misc_fops_file_operations ;
  int tmp ;
  int tmp___0 ;

  {
  {
#line 72
  ldv_s_misc_fops_file_operations = 0;
#line 73
  my_init();
  }
  {
#line 74
  while (1) {
    while_0_continue: /* CIL Label */ ;
    {
#line 74
    tmp___0 = __VERIFIER_nondet_int();
    }
#line 74
    if (tmp___0) {

    } else {
      goto while_0_break;
    }
    {
#line 76
    tmp = __VERIFIER_nondet_int();
    }
#line 78
    if (tmp == 0) {
      goto switch_1_0;
    } else {
#line 86
      if (tmp == 1) {
        goto switch_1_1;
      } else {
#line 94
        if (tmp == 2) {
          goto switch_1_2;
        } else {
#line 102
          if (tmp == 3) {
            goto switch_1_3;
          } else {
            {
            goto switch_1_default;
#line 76
            if (0) {
              switch_1_0: /* CIL Label */ 
#line 79
              if (ldv_s_misc_fops_file_operations == 0) {
                {
#line 80
                misc_open();
#line 81
                ldv_s_misc_fops_file_operations = ldv_s_misc_fops_file_operations + 1;
                }
              } else {

              }
              goto switch_1_break;
              switch_1_1: /* CIL Label */ 
#line 87
              if (ldv_s_misc_fops_file_operations == 1) {
                {
#line 88
                misc_read();
#line 89
                ldv_s_misc_fops_file_operations = ldv_s_misc_fops_file_operations + 1;
                }
              } else {

              }
              goto switch_1_break;
              switch_1_2: /* CIL Label */ 
#line 95
              if (ldv_s_misc_fops_file_operations == 2) {
                {
#line 96
                misc_llseek();
#line 97
                ldv_s_misc_fops_file_operations = ldv_s_misc_fops_file_operations + 1;
                }
              } else {

              }
              goto switch_1_break;
              switch_1_3: /* CIL Label */ 
#line 103
              if (ldv_s_misc_fops_file_operations == 3) {
                {
#line 104
                misc_release();
#line 105
                ldv_s_misc_fops_file_operations = 0;
                }
              } else {

              }
              goto switch_1_break;
              switch_1_default: /* CIL Label */ ;
              goto switch_1_break;
            } else {
              switch_1_break: /* CIL Label */ ;
            }
            }
          }
        }
      }
    }
  }
  while_0_break: /* CIL Label */ ;
  }
  {
#line 113
  check_final_state();
  }
#line 114
  return;
}
}
