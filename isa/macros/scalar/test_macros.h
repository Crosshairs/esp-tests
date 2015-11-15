// See LICENSE for license details.

#ifndef __TEST_MACROS_SCALAR_H
#define __TEST_MACROS_SCALAR_H


#-----------------------------------------------------------------------
# Helper macros
#-----------------------------------------------------------------------

#define TEST_CASE( testnum, testreg, correctval, code... ) \
test_ ## testnum: \
    code; \
    li  x29, correctval; \
    li  TESTNUM, testnum; \
    bne testreg, x29, fail;

# We use a macro hack to simpify code generation for various numbers
# of bubble cycles.

#define TEST_INSERT_NOPS_0
#define TEST_INSERT_NOPS_1  nop; TEST_INSERT_NOPS_0
#define TEST_INSERT_NOPS_2  nop; TEST_INSERT_NOPS_1
#define TEST_INSERT_NOPS_3  nop; TEST_INSERT_NOPS_2
#define TEST_INSERT_NOPS_4  nop; TEST_INSERT_NOPS_3
#define TEST_INSERT_NOPS_5  nop; TEST_INSERT_NOPS_4
#define TEST_INSERT_NOPS_6  nop; TEST_INSERT_NOPS_5
#define TEST_INSERT_NOPS_7  nop; TEST_INSERT_NOPS_6
#define TEST_INSERT_NOPS_8  nop; TEST_INSERT_NOPS_7
#define TEST_INSERT_NOPS_9  nop; TEST_INSERT_NOPS_8
#define TEST_INSERT_NOPS_10 nop; TEST_INSERT_NOPS_9


#-----------------------------------------------------------------------
# RV64UI MACROS
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Tests for instructions with immediate operand
#-----------------------------------------------------------------------

#define SEXT_IMM(x) ((x) | (-(((x) >> 11) & 1) << 11))

#define TEST_IMM_OP( testnum, inst, result, val1, imm ) \
    TEST_CASE( testnum, x3, result, \
      li  x1, val1; \
      inst x3, x1, SEXT_IMM(imm); \
    )

#define TEST_IMM_SRC1_EQ_DEST( testnum, inst, result, val1, imm ) \
    TEST_CASE( testnum, x1, result, \
      li  x1, val1; \
      inst x1, x1, SEXT_IMM(imm); \
    )

#define TEST_IMM_DEST_BYPASS( testnum, nop_cycles, inst, result, val1, imm ) \
    TEST_CASE( testnum, x6, result, \
      li  x4, 0; \
1:    li  x1, val1; \
      inst x3, x1, SEXT_IMM(imm); \
      TEST_INSERT_NOPS_ ## nop_cycles \
      addi  x6, x3, 0; \
      addi  x4, x4, 1; \
      li  x5, 2; \
      bne x4, x5, 1b \
    )

#define TEST_IMM_SRC1_BYPASS( testnum, nop_cycles, inst, result, val1, imm ) \
    TEST_CASE( testnum, x3, result, \
      li  x4, 0; \
1:    li  x1, val1; \
      TEST_INSERT_NOPS_ ## nop_cycles \
      inst x3, x1, SEXT_IMM(imm); \
      addi  x4, x4, 1; \
      li  x5, 2; \
      bne x4, x5, 1b \
    )

#define TEST_IMM_ZEROSRC1( testnum, inst, result, imm ) \
    TEST_CASE( testnum, x1, result, \
      inst x1, x0, SEXT_IMM(imm); \
    )

#define TEST_IMM_ZERODEST( testnum, inst, val1, imm ) \
    TEST_CASE( testnum, x0, 0, \
      li  x1, val1; \
      inst x0, x1, SEXT_IMM(imm); \
    )

#-----------------------------------------------------------------------
# Tests for vector config instructions
#-----------------------------------------------------------------------

#define VCFG(nvvd, nvvw, nvvh, nvp) \
  (((nvvd) & 0x1ff) | \
  (((nvp) & 0x1f) << 9) | \
  (((nvvw) & 0x1ff) << 14) | \
  (((nvvh) & 0x1ff) << 23))

#define TEST_VSETCFGIVL( testnum, nvvd, nvp, vl, result ) \
    TEST_CASE( testnum, x1, result, \
      vsetcfg (nvvd),(nvp); \
      li x1, vl; \
      vsetvl x1,x1; \
    )

#define TEST_VVCFG( testnum, nvvd, nvp, vl, result ) \
    TEST_CASE( testnum, x1, result, \
      li x1, VCFG(nvvd,0,0,nvp); \
      vsetcfg x1; \
      li x1, vl; \
      vsetvl x1,x1; \
    )

#define TEST_VSETVL( testnum, nvvd, nvp, vl, result ) \
    TEST_CASE( testnum, x1, result, \
      li x1, VCFG(nvvd,0,0,nvp); \
      vsetcfg x1; \
      li x1, vl; \
      vsetvl x1, x1; \
    )

#-----------------------------------------------------------------------
# Tests for an instruction with register operands
#-----------------------------------------------------------------------

#define TEST_R_OP( testnum, inst, result, val1 ) \
    TEST_CASE( testnum, x3, result, \
      li  x1, val1; \
      inst x3, x1; \
    )

#define TEST_R_SRC1_EQ_DEST( testnum, inst, result, val1 ) \
    TEST_CASE( testnum, x1, result, \
      li  x1, val1; \
      inst x1, x1; \
    )

#define TEST_R_DEST_BYPASS( testnum, nop_cycles, inst, result, val1 ) \
    TEST_CASE( testnum, x6, result, \
      li  x4, 0; \
1:    li  x1, val1; \
      inst x3, x1; \
      TEST_INSERT_NOPS_ ## nop_cycles \
      addi  x6, x3, 0; \
      addi  x4, x4, 1; \
      li  x5, 2; \
      bne x4, x5, 1b \
    )

#-----------------------------------------------------------------------
# Tests for an instruction with register-register operands
#-----------------------------------------------------------------------

#define TEST_RR_OP( testnum, inst, result, val1, val2 ) \
    TEST_CASE( testnum, x3, result, \
      li  x1, val1; \
      li  x2, val2; \
      inst x3, x1, x2; \
    )

#define TEST_RR_SRC1_EQ_DEST( testnum, inst, result, val1, val2 ) \
    TEST_CASE( testnum, x1, result, \
      li  x1, val1; \
      li  x2, val2; \
      inst x1, x1, x2; \
    )

#define TEST_RR_SRC2_EQ_DEST( testnum, inst, result, val1, val2 ) \
    TEST_CASE( testnum, x2, result, \
      li  x1, val1; \
      li  x2, val2; \
      inst x2, x1, x2; \
    )

#define TEST_RR_SRC12_EQ_DEST( testnum, inst, result, val1 ) \
    TEST_CASE( testnum, x1, result, \
      li  x1, val1; \
      inst x1, x1, x1; \
    )

#define TEST_RR_DEST_BYPASS( testnum, nop_cycles, inst, result, val1, val2 ) \
    TEST_CASE( testnum, x6, result, \
      li  x4, 0; \
1:    li  x1, val1; \
      li  x2, val2; \
      inst x3, x1, x2; \
      TEST_INSERT_NOPS_ ## nop_cycles \
      addi  x6, x3, 0; \
      addi  x4, x4, 1; \
      li  x5, 2; \
      bne x4, x5, 1b \
    )

#define TEST_RR_SRC12_BYPASS( testnum, src1_nops, src2_nops, inst, result, val1, val2 ) \
    TEST_CASE( testnum, x3, result, \
      li  x4, 0; \
1:    li  x1, val1; \
      TEST_INSERT_NOPS_ ## src1_nops \
      li  x2, val2; \
      TEST_INSERT_NOPS_ ## src2_nops \
      inst x3, x1, x2; \
      addi  x4, x4, 1; \
      li  x5, 2; \
      bne x4, x5, 1b \
    )

#define TEST_RR_SRC21_BYPASS( testnum, src1_nops, src2_nops, inst, result, val1, val2 ) \
    TEST_CASE( testnum, x3, result, \
      li  x4, 0; \
1:    li  x2, val2; \
      TEST_INSERT_NOPS_ ## src1_nops \
      li  x1, val1; \
      TEST_INSERT_NOPS_ ## src2_nops \
      inst x3, x1, x2; \
      addi  x4, x4, 1; \
      li  x5, 2; \
      bne x4, x5, 1b \
    )

#define TEST_RR_ZEROSRC1( testnum, inst, result, val ) \
    TEST_CASE( testnum, x2, result, \
      li x1, val; \
      inst x2, x0, x1; \
    )

#define TEST_RR_ZEROSRC2( testnum, inst, result, val ) \
    TEST_CASE( testnum, x2, result, \
      li x1, val; \
      inst x2, x1, x0; \
    )

#define TEST_RR_ZEROSRC12( testnum, inst, result ) \
    TEST_CASE( testnum, x1, result, \
      inst x1, x0, x0; \
    )

#define TEST_RR_ZERODEST( testnum, inst, val1, val2 ) \
    TEST_CASE( testnum, x0, 0, \
      li x1, val1; \
      li x2, val2; \
      inst x0, x1, x2; \
    )

#-----------------------------------------------------------------------
# Test memory instructions
#-----------------------------------------------------------------------

#define TEST_LD_OP( testnum, inst, result, offset, base ) \
    TEST_CASE( testnum, x3, result, \
      la  x1, base; \
      inst x3, offset(x1); \
    )

#define TEST_ST_OP( testnum, load_inst, store_inst, result, offset, base ) \
    TEST_CASE( testnum, x3, result, \
      la  x1, base; \
      li  x2, result; \
      store_inst x2, offset(x1); \
      load_inst x3, offset(x1); \
    )

#define TEST_LD_DEST_BYPASS( testnum, nop_cycles, inst, result, offset, base ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    inst x3, offset(x1); \
    TEST_INSERT_NOPS_ ## nop_cycles \
    addi  x6, x3, 0; \
    li  x29, result; \
    bne x6, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b; \

#define TEST_LD_SRC1_BYPASS( testnum, nop_cycles, inst, result, offset, base ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, base; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst x3, offset(x1); \
    li  x29, result; \
    bne x3, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_ST_SRC12_BYPASS( testnum, src1_nops, src2_nops, load_inst, store_inst, result, offset, base ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x1, result; \
    TEST_INSERT_NOPS_ ## src1_nops \
    la  x2, base; \
    TEST_INSERT_NOPS_ ## src2_nops \
    store_inst x1, offset(x2); \
    load_inst x3, offset(x2); \
    li  x29, result; \
    bne x3, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_ST_SRC21_BYPASS( testnum, src1_nops, src2_nops, load_inst, store_inst, result, offset, base ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x2, base; \
    TEST_INSERT_NOPS_ ## src1_nops \
    la  x1, result; \
    TEST_INSERT_NOPS_ ## src2_nops \
    store_inst x1, offset(x2); \
    load_inst x3, offset(x2); \
    li  x29, result; \
    bne x3, x29, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#-----------------------------------------------------------------------
# Test branch instructions
#-----------------------------------------------------------------------

#define TEST_BR1_OP_TAKEN( testnum, inst, val1 ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x1, val1; \
    inst x1, 2f; \
    bne x0, TESTNUM, fail; \
1:  bne x0, TESTNUM, 3f; \
2:  inst x1, 1b; \
    bne x0, TESTNUM, fail; \
3:

#define TEST_BR1_OP_NOTTAKEN( testnum, inst, val1 ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x1, val1; \
    inst x1, 1f; \
    bne x0, TESTNUM, 2f; \
1:  bne x0, TESTNUM, fail; \
2:  inst x1, 1b; \
3:

#define TEST_BR1_SRC1_BYPASS( testnum, nop_cycles, inst, val1 ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  li  x1, val1; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst x1, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_BR2_OP_TAKEN( testnum, inst, val1, val2 ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x1, val1; \
    li  x2, val2; \
    inst x1, x2, 2f; \
    bne x0, TESTNUM, fail; \
1:  bne x0, TESTNUM, 3f; \
2:  inst x1, x2, 1b; \
    bne x0, TESTNUM, fail; \
3:

#define TEST_BR2_OP_NOTTAKEN( testnum, inst, val1, val2 ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x1, val1; \
    li  x2, val2; \
    inst x1, x2, 1f; \
    bne x0, TESTNUM, 2f; \
1:  bne x0, TESTNUM, fail; \
2:  inst x1, x2, 1b; \
3:

#define TEST_BR2_SRC12_BYPASS( testnum, src1_nops, src2_nops, inst, val1, val2 ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  li  x1, val1; \
    TEST_INSERT_NOPS_ ## src1_nops \
    li  x2, val2; \
    TEST_INSERT_NOPS_ ## src2_nops \
    inst x1, x2, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_BR2_SRC21_BYPASS( testnum, src1_nops, src2_nops, inst, val1, val2 ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  li  x2, val2; \
    TEST_INSERT_NOPS_ ## src1_nops \
    li  x1, val1; \
    TEST_INSERT_NOPS_ ## src2_nops \
    inst x1, x2, fail; \
    addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#-----------------------------------------------------------------------
# Test jump instructions
#-----------------------------------------------------------------------

#define TEST_JR_SRC1_BYPASS( testnum, nop_cycles, inst ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x6, 2f; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst x6; \
    bne x0, TESTNUM, fail; \
2:  addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \

#define TEST_JALR_SRC1_BYPASS( testnum, nop_cycles, inst ) \
test_ ## testnum: \
    li  TESTNUM, testnum; \
    li  x4, 0; \
1:  la  x6, 2f; \
    TEST_INSERT_NOPS_ ## nop_cycles \
    inst x19, x6, 0; \
    bne x0, TESTNUM, fail; \
2:  addi  x4, x4, 1; \
    li  x5, 2; \
    bne x4, x5, 1b \


#-----------------------------------------------------------------------
# RV64UF MACROS
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Tests floating-point instructions
#-----------------------------------------------------------------------

#define TEST_FP_OP_INTERNAL( testnum, flags, result, val1, val2, val3, fload, load, code... ) \
test_ ## testnum: \
  li  TESTNUM, testnum; \
  la  a0, 3f; fload f0, 0(a0); \
  la  a0, 4f; fload f1, 0(a0); \
  la  a0, 5f; fload f2, 0(a0); \
  la  a0, 6f; load a3, 0(a0); \
  code; \
  fsflags a1, x0; \
  li a2, flags; \
  bne a0, a3, fail; \
  bne a1, a2, fail; \
  j 2f; \
  .data; \
  test_ ## testnum ## _data: \
  .align 3; \
3:.val1; \
4:.val2; \
5:.val3; \
  .align 3; \
6:.result; \
  .text; \
2:

#define TEST_FCVT_S_D( testnum, result, val1 ) \
  TEST_FP_OP_INTERNAL( testnum, 0, double result, double val1, double 0.0, double 0.0, fld, ld, \
                       fcvt.s.d f3, f0; fcvt.d.s f3, f3; fmv.x.d a0, f3)

#define TEST_FCVT_D_S( testnum, result, val1 ) \
  TEST_FP_OP_INTERNAL( testnum, 0, float result, float val1, float 0.0, float 0.0, flw, lw, \
                       fcvt.d.s f3, f0; fcvt.s.d f3, f3; fmv.x.s a0, f3)

#define TEST_FP_OP1_S( testnum, inst, flags, result, val1 ) \
  TEST_FP_OP_INTERNAL( testnum, flags, float result, float val1, float 0.0, float 0.0, flw, lw, \
                       inst f3, f0; fmv.x.s a0, f3)

#define TEST_FP_OP1_D( testnum, inst, flags, result, val1 ) \
  TEST_FP_OP_INTERNAL( testnum, flags, double result, double val1, double 0.0, double 0.0, fld, ld, \
                       inst f3, f0; fmv.x.d a0, f3)

#define TEST_FP_OP2_S( testnum, inst, flags, result, val1, val2 ) \
  TEST_FP_OP_INTERNAL( testnum, flags, float result, float val1, float val2, float 0.0, flw, lw, \
                       inst f3, f0, f1; fmv.x.s a0, f3)

#define TEST_FP_OP2_D( testnum, inst, flags, result, val1, val2 ) \
  TEST_FP_OP_INTERNAL( testnum, flags, double result, double val1, double val2, double 0.0, fld, ld, \
                       inst f3, f0, f1; fmv.x.d a0, f3)

#define TEST_FP_OP3_S( testnum, inst, flags, result, val1, val2, val3 ) \
  TEST_FP_OP_INTERNAL( testnum, flags, float result, float val1, float val2, float val3, flw, lw, \
                       inst f3, f0, f1, f2; fmv.x.s a0, f3)

#define TEST_FP_OP3_D( testnum, inst, flags, result, val1, val2, val3 ) \
  TEST_FP_OP_INTERNAL( testnum, flags, double result, double val1, double val2, double val3, fld, ld, \
                       inst f3, f0, f1, f2; fmv.x.d a0, f3)

#define TEST_FP_INT_OP_S( testnum, inst, flags, result, val1, rm ) \
  TEST_FP_OP_INTERNAL( testnum, flags, word result, float val1, float 0.0, float 0.0, flw, lw, \
                       inst a0, f0, rm)

#define TEST_FP_INT_OP_D( testnum, inst, flags, result, val1, rm ) \
  TEST_FP_OP_INTERNAL( testnum, flags, dword result, double val1, double 0.0, double 0.0, fld, ld, \
                       inst a0, f0, rm)

#define TEST_FP_INT_OP_S_HEX( testnum, inst, flags, result, val1, rm ) \
  TEST_FP_OP_INTERNAL( testnum, flags, dword result, word val1, word 0, word 0, flw, ld, \
                       inst a0, f0, rm)

#define TEST_FP_INT_OP_D_HEX( testnum, inst, flags, result, val1, rm ) \
  TEST_FP_OP_INTERNAL( testnum, flags, dword result, dword val1, dword 0, dword 0, fld, ld, \
                       inst a0, f0, rm)

#define TEST_FP_CMP_OP_S( testnum, inst, result, val1, val2 ) \
  TEST_FP_OP_INTERNAL( testnum, 0, word result, float val1, float val2, float 0.0, flw, lw, \
                       inst a0, f0, f1)

#define TEST_FP_CMP_OP_D( testnum, inst, result, val1, val2 ) \
  TEST_FP_OP_INTERNAL( testnum, 0, dword result, double val1, double val2, double 0.0, fld, ld, \
                       inst a0, f0, f1)

#define TEST_FP_CLASS_S( testnum, result, val1 ) \
  TEST_FP_OP_INTERNAL( testnum, 0, dword result, word val1, word 0, word 0, flw, ld, \
                       fclass.s a0, f0)

#define TEST_FP_CLASS_D( testnum, result, val1 ) \
  TEST_FP_OP_INTERNAL( testnum, 0, dword result, dword val1, dword 0, dword 0, fld, ld, \
                       fclass.d a0, f0)

#define TEST_INT_FP_OP_S( testnum, inst, flags, result, val1 ) \
  TEST_FP_OP_INTERNAL( testnum, flags, float result, word val1, word 0, word 0, flw, lw, \
                       fmv.x.s a0, f0; inst f0, a0; fmv.x.s a0, f0)

#define TEST_INT_FP_OP_D( testnum, inst, flags, result, val1 ) \
  TEST_FP_OP_INTERNAL( testnum, flags, double result, dword val1, dword 0, dword 0, fld, ld, \
                       fmv.x.d a0, f0; inst f0, a0; fmv.x.d a0, f0)

#-----------------------------------------------------------------------
# RV64UV MACROS
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Test memory instructions
#-----------------------------------------------------------------------
#define TEST_CASE_NREG_SCALAR( testnum, nxreg, npreg, input, output, code... ) \
test_ ## testnum: \
  vsetcfg nxreg,npreg; \
  li a3,2048; \
  vsetvl a3,a3; \
  li a1,input; \
  vmss vs3, a1; \
  la a4,src1; \
  vmsa va2, a4; \
  vmss vs2, a4; \
  la a4,dst; \
  vmsa va1, a4; \
  vmss vs1, a4; \
  lui a0,%hi(vtcode ## testnum ); \
  vf %lo(vtcode ## testnum )(a0); \
  fence; \
  li a1, output; \
  li TESTNUM, testnum; \
  ld a0,0(a4); \
  beq a0,a1,skip ## testnum; \
  j fail; \
skip ## testnum : \
  j next ## testnum; \
.align 3; \
vtcode ## testnum : \
  code; \
  vstop; \
next ## testnum :

#define TEST_VST_OP( testnum, store_inst, areg, result ) \
    TEST_CASE_NREG_SCALAR( testnum, 1, 1, result, result, \
      store_inst areg, vs3; \
    )

#define TEST_VLD_OP( testnum, load_inst, store_inst, sreg, dreg, input, output ) \
    TEST_CASE_NREG_SCALAR( testnum, 1, 1, input, output, \
      store_inst sreg, vs3; \
      load_inst vs4, sreg; \
      store_inst dreg, vs4; \
    )

#define TEST_VLDU_OP( testnum, load_inst, store_inst, sreg, dreg, result ) \
    TEST_VLD_OP( testnum, load_inst, store_inst, sreg, dreg, result, result )

#-----------------------------------------------------------------------
# RV64SV MACROS
#-----------------------------------------------------------------------

#define TEST_ILLEGAL_VT_REGID( testnum, nxreg, nfreg, inst, reg1, reg2, reg3) \
  la a0, handler ## testnum; \
  csrw stvec, a0; \
  vsetcfg nxreg, nfreg; \
  li a0, 4; \
  vsetvl a0, a0; \
  la a0, src1; \
  vmsa va0, a0; \
  la a1, src2; \
  vmsa va1, a1; \
  la a3, dest; \
  vmsa va2, a3; \
  lui a0,%hi(vtcode1 ## testnum); \
  vf %lo(vtcode1 ## testnum)(a0); \
  fence; \
.align 3; \
vtcode1 ## testnum: \
  vld vv2,va0; \
  vld vv3,va1; \
  vadd.vv vv2, vv2, vv3; \
  vsd vv2, va2; \
illegal ## testnum: \
  v ## inst.vv v ## reg1, v ## reg2, v ## reg3; \
  vstop; \
.align 3; \
vtcode2 ## testnum: \
  vld vv2,va0; \
  vld vv3,va1; \
  vadd.vv vv2, vv2, vv3; \
  vsd vv2, va2; \
  vstop; \
handler ## testnum: \
  vxcptkill; \
  li TESTNUM,2; \
  csrr a0, scause; \
  li a1,HWACHA_CAUSE_VF_ILLEGAL_REGID; \
  bne a0,a1,fail; \
  csrr a0, sbadaddr; \
  la a1,illegal ## testnum; \
  bne a0,a1,fail; \
  vsetcfg 32,1; \
  li a0,4; \
  vsetvl a0,a0; \
  la a0, src1; \
  vmsa va0, a0; \
  la a1, src2; \
  vmsa va1, a1; \
  la a3, dest; \
  vmsa va2, a3; \
  lui a0,%hi(vtcode2 ## testnum); \
  vf %lo(vtcode2 ## testnum)(a0); \
  fence; \
  ld a1,0(a3); \
  li a2,5; \
  li TESTNUM,2; \
  bne a1,a2,fail; \
  ld a1,8(a3); \
  li TESTNUM,3; \
  bne a1,a2,fail; \
  ld a1,16(a3); \
  li TESTNUM,4; \
  bne a1,a2,fail; \
  ld a1,24(a3); \
  li TESTNUM,5; \
  bne a1,a2,fail; \

#-----------------------------------------------------------------------
# Pass and fail code (assumes test num is in TESTNUM)
#-----------------------------------------------------------------------

#define TEST_PASSFAIL \
        bne x0, TESTNUM, pass; \
fail: \
        RVTEST_FAIL; \
pass: \
        RVTEST_PASS \


#-----------------------------------------------------------------------
# Test data section
#-----------------------------------------------------------------------

#define TEST_DATA

#endif
