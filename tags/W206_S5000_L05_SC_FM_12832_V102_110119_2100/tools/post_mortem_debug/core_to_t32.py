#! /usr/bin/env python
#                       C O R E    T O    T 3 2
#
# GENERAL DESCRIPTION
#   This script provides a mechnism for extracting post-mortem core dump
#   information from an L4 core dump file, and converting it into a format
#   suitable for loading into Lauterbach T32
#
# Copyright (c) 2005 by QUALCOMM Incorporated.  All rights reserved.
#

import os, sys
from optparse import OptionParser

import read_tlv
import file_binops

# R13 at the time of the abort
_r13 = 0

# Will be filled with textual report information
report_lines = []

def do_nothing(fp, l):
    """
    Function which performs no action. Used to ensure that no action
    is taken for tags which do not require output to T32.
    """
    pass

def core_registers(fp, l):
    """
    Generate T32 script commands to reload contents of all of the ARM
    registers based on the QPMD_REGISTER_DUMP record contents.
    """
    global _r13
    regs = {}
    # User mode registers
    # Skip the first 3 (32bit) words - they are header field of rex-core.
    l = l[12:]
    regs['R0']  = file_binops.listToUint32(l)
    regs['R1']  = file_binops.listToUint32(l[4:])
    regs['R2']  = file_binops.listToUint32(l[8:])
    regs['R3']  = file_binops.listToUint32(l[12:])
    regs['R4']  = file_binops.listToUint32(l[16:])
    regs['R5']  = file_binops.listToUint32(l[20:])
    regs['R6']  = file_binops.listToUint32(l[24:])
    regs['R7']  = file_binops.listToUint32(l[28:])
    regs['R8']  = file_binops.listToUint32(l[32:])
    regs['R9']  = file_binops.listToUint32(l[36:])
    regs['R10'] = file_binops.listToUint32(l[40:])
    regs['R11'] = file_binops.listToUint32(l[44:])
    regs['R12'] = file_binops.listToUint32(l[48:])
    regs['R13'] = file_binops.listToUint32(l[52:])
    # Record r13 for the rebuilding of the stackframe
    _r13        = file_binops.listToUint32(l[52:])
    regs['R14'] = file_binops.listToUint32(l[56:])
    regs['SPSR_SVC'] = file_binops.listToUint32(l[60:])
    regs['PC']       = file_binops.listToUint32(l[64:])
    # SYS mode registers
    # regs['R13_SYS']     = file_binops.listToUint32(l[68:])
    # regs['LR_SYS']   = file_binops.listToUint32(l[72:])
    # IRQ mode registers
    # regs['R13_IRQ']  = file_binops.listToUint32(l[76:])
    # regs['LR_IRQ']   = file_binops.listToUint32(l[80:])
    # regs['SPSR_IRQ'] = file_binops.listToUint32(l[84:])
    # Abort mode registers
    # regs['R13_ABT']  = file_binops.listToUint32(l[88:])
    # regs['LR_ABT']   = file_binops.listToUint32(l[92:])
    # regs['SPSR_ABT'] = file_binops.listToUint32(l[96:])
    # Undefined instruction registers
    # regs['R13_UND']  = file_binops.listToUint32(l[100:])
    # regs['LR_UND']   = file_binops.listToUint32(l[104:])
    # regs['SPSR_UND'] = file_binops.listToUint32(l[108:])
    # FIQ mode
    # regs['R8_FIQ']   = file_binops.listToUint32(l[112:])
    # regs['R9_FIQ']   = file_binops.listToUint32(l[116:])
    # regs['R10_FIQ']  = file_binops.listToUint32(l[120:])
    # regs['R11_FIQ']  = file_binops.listToUint32(l[124:])
    # regs['R12_FIQ']  = file_binops.listToUint32(l[128:])
    # regs['R13_FIQ']  = file_binops.listToUint32(l[132:])
    # regs['LR_FIQ']   = file_binops.listToUint32(l[136:])
    # regs['SPSR_FIQ'] = file_binops.listToUint32(l[140:])
    # Java registers
    # regs['R13_JVA']  = file_binops.listToUint32(l[144:])
    # regs['LR_JVA']   = file_binops.listToUint32(l[148:])
    # regs['SPSR_JVA'] = file_binops.listToUint32(l[152:])
    # CPSR
    regs['CPSR'] = file_binops.listToUint32(l[156:])
    # Write register values to CMM file
    fp.write(";\n; restore register values\n;\n")
    report_lines.append("Register dump")
    for key in regs.keys():
        fp.write("Register.Set %s 0x%x\n" % (key, regs[key]))
        report_lines.append("Register: %s Value 0x%x" % (key, regs[key]))

def core_mem_dump(fp, l):
    """
    Generate T32 script commands to reload contents of an ARM memory
    dump.
    Caution: memory dump is given a virtual address, so we need to
    take care that the MMU is appropriately configured when reloading
    (should not be a problem for single address space components, but
    may be an issue for Linux).
    """
    vaddr = listToUint32(l)
    start_offset = l[4]
    length = listToUint16(l[6:])
    # turn into list of 32 bit, RLE decode and turn back to list of bytes
    bytes = int_list_to_byte_list(rle_decode(byte_list_to_int_list(l[8:])))
    # Now strip out useful data using start_offset and length
    bytes = bytes[start:start+length]
    fp.write(";\n; Dump data starting at 0x%x\n;\n" % vaddr)
    report_lines.append("Write data %s\n at vaddr 0x%x" % (repr(bytes), vaddr))
    for byte in bytes:
        fp.write("Data.Set 0x%x \%Byte 0x%02x\n" % (vaddr, byte))
        vaddr = vaddr + 1

def core_err_fatal_log(fp, l):
    """
    Generate T32 script commands (where appropriate) and text output
    for the ERR_FATAL data.
    """
    global _r13
    curr_tcb_ptr   = file_binops.listToUint32(l)
    tasklock_state = file_binops.listToUint32(l[4:])
    # Drop the data we've already read and read the stack frame
    l = l[8:]
    stack_frame = {}
    for i in range(64):
        stack_frame[i] =  file_binops.listToUint32(l)
        l = l[4:]
    # Read err_line
    err_line = file_binops.listToUint32(l)
    l = l[4:]
    # Read err_params
    report_lines.append(file_binops.listToUint32(l))
    report_lines.append(file_binops.listToUint32(l[4:]))
    report_lines.append(file_binops.listToUint32(l[8:]))
    report_lines.append(file_binops.listToUint32(l[12:]))
    l = l[16:]
    # Read err_file
    err_file = ''
    while l[0] != 0:
        err_file = err_file + str(chr(l[0]))
        l = l[1:]
    # Skip over '\0'
    l = l[1:]
    # Read err_msg
    err_msg = ''
    while l[0] != 0:
        err_msg = err_msg + str(chr(l[0]))
        l = l[1:]
    # Check the number of % characters in the format string
    n_fmt_str = min(err_msg.count('%'), 4)

    # Just put some info in the cmm file
    fp.write("\n")
    fp.write(";-------------------------------------------------------\n")
    fp.write("; ERR_FATAL\n")
    fp.write("; Error in file: %s at line %d with message\n;  " % (err_file, err_line))
    fp.write(err_msg % tuple(report_lines[:n_fmt_str]))
    fp.write("\n; TASKLOCK state: %d\n" % tasklock_state)
    fp.write(";-------------------------------------------------------\n")

    # Restore the stackframe
    fp.write("\n")
    fp.write(";-------------------------------------------------------\n")
    fp.write("; Stackframe restoration\n")
    fp.write(";-------------------------------------------------------\n")

    for i in range(64):
        fp.write("data.set 0x%x %%LONG 0x%x\n" % (_r13, stack_frame[i]))
        report_lines.append("Stack address: 0x%x Value 0x%x" % (_r13, stack_frame[i]))
        _r13 = _r13 + 4
    
    # Now we do the update of the TCB etc. in T32

#
# Canned scripts - these do now change regardless of the core dump
# functionality, although changes may be required if boot loader and/or
# start-up ordering changes in the build.
#
stateful_script = """
;                      C O R E    R E S T O R E
;
; NB: This script was generated automatically using core_to_t32
;
; Copyright (C) 2005 by QUALCOMM Incorporated. All rights reserved
;
; Script to restore AMSS software running under L4 to the state in which
; an exception occurred, just prior to a core dump. The script is only
; useful if there has been no power cycle (or other operation which would
; disturb memory contents
;
; Script to access memory after reset.  Saves registers,
; runs boot_loader until after the memory controller has
; been configured, configure the MMU so virtual memory
; will be visible, and restore registers.

; NOTE: If the boot_loader changes, the hard-coded address
; to break on will need to be changed.

; Put system in to a known state
system.reset
sys.d
system.cpu ARM926EJ 
system.jtagclock rtck
map.reset
break.reset

; Turn on DACR option so T32 can access another PD's memory.
sys.option dacr on

; Break after the bootloader sets up the memory
sys.u
b.s tmc_dog_report /onchip
g
wait !run()
b.d tmc_dog_report

;
; Now restore memory and registers
;
"""

limited_script = """
;                      C O R E    R E S T O R E
;
; NB: This script was generated automatically using core_to_t32 -l
;
; Copyright (C) 2005 by QUALCOMM Incorporated. All rights reserved
;
; Script to restore AMSS software running under L4 to the state in which
; an exception occurred, just prior to a core dump. The script is designed
; to be used on machines which do not retain the complete memory state
; when the crash occurred. Please be aware that only limited information
; will be valid (crash address, registers, last 64 entries in the stack
; frame and any memory areas which have been specifically restored.
;
; This script should be run after jload_linux <build>
;

;
; Now restore memory and registers
;
"""

def print_script(fp, limited=False):
    """
    This function generates the 'canned' part of the output script, i.e. the
    part of the script which never changes. Different scripts are used depending
    on whether the limited restore functionality is required, for loading into
    a machine after power has been cycled, or whether full memory state can be
    relied upon.
    """
    if limited:
        fp.write(limited_script)
    else:
        fp.write(stateful_script)
        
def read_core_file(filename):
    return read_tlv.tlv_parser(filename)

TAG_DECODERS = { 1: core_registers,
                 2: do_nothing,
                 3: core_mem_dump,
                 4: core_err_fatal_log }

if __name__ == '__main__':
    limited = False
    if len(sys.argv) < 2:
        print "Usage: core_to_t32 [-l] <filename>"
        sys.exit(2)  # Usual Unix convention for command line error
    try:
        if len(sys.argv) == 2:
            tags = read_core_file(sys.argv[1])
        else:
            if sys.argv[1] != '-l':
                print "Usage core_to_t32 [-l] <filename>"
                sys.exit(2)
            tags = read_core_file(sys.argv[2])
            limited = True
    except IOError:
        print "Unable to open %s" % sys.argv[1]
        sys.exit(1)
    try:
        outfile = open("%s.cmm" % sys.argv[1], 'wb')
        print_script(outfile, limited)
        for tag in tags.get_tags():
            TAG_DECODERS[tag](outfile, tags.get_tag(tag))
    finally:
        outfile.close()
        print "Generated cmm file: %s.cmm" % sys.argv[1]
