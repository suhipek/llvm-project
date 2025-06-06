; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=ve -mattr=+vpu | FileCheck %s

define fastcc <256 x float> @test_vec_fneg_v256f32_v(<256 x float> %v) {
; CHECK-LABEL: test_vec_fneg_v256f32_v:
; CHECK:       # %bb.0:
; CHECK-NEXT:    lea %s0, 256
; CHECK-NEXT:    lvl %s0
; CHECK-NEXT:    vxor %v0, (1)1, %v0
; CHECK-NEXT:    b.l.t (, %s10)
  %neg = fneg <256 x float> %v
  ret <256 x float> %neg
}

define fastcc <256 x double> @test_vec_fneg_v256f64_v(<256 x double> %v) {
; CHECK-LABEL: test_vec_fneg_v256f64_v:
; CHECK:       # %bb.0:
; CHECK-NEXT:    lea %s0, 256
; CHECK-NEXT:    lvl %s0
; CHECK-NEXT:    vxor %v0, (1)1, %v0
; CHECK-NEXT:    b.l.t (, %s10)
  %neg = fneg <256 x double> %v
  ret <256 x double> %neg
}

