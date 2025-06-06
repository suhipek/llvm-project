; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 4
; RUN: opt < %s -S -passes=instcombine | FileCheck %s

target datalayout = "E-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f128:64:128"

define i32 @test() {
; CHECK-LABEL: define i32 @test() {
; CHECK-NEXT:  entry:
; CHECK-NEXT:    br i1 true, label [[BB_I:%.*]], label [[CALCULATECOLORSPECIFICBLACKLEVEL_EXIT:%.*]]
; CHECK:       bb.i:
; CHECK-NEXT:    br label [[BB51_I_I:%.*]]
; CHECK:       bb27.i.i:
; CHECK-NEXT:    br label [[BB51_I_I]]
; CHECK:       bb51.i.i:
; CHECK-NEXT:    br i1 false, label [[BB27_I_I:%.*]], label [[BB57_I_I:%.*]]
; CHECK:       bb57.i.i:
; CHECK-NEXT:    ret i32 0
; CHECK:       calculateColorSpecificBlackLevel.exit:
; CHECK-NEXT:    ret i32 undef
;
entry:
  %tmp50.i17 = icmp slt i32 0, 4
  br i1 %tmp50.i17, label %bb.i, label %calculateColorSpecificBlackLevel.exit

bb.i:
  br label %bb51.i.i

bb27.i.i:
  %tmp31.i.i = load i16, ptr null, align 2
  %tmp35.i.i = icmp ult i16 %tmp31.i.i, 1
  %tmp41.i.i = icmp ugt i16 %tmp31.i.i, -1
  %bothcond.i.i = or i1 %tmp35.i.i, %tmp41.i.i
  %bothcond1.i.i = zext i1 %bothcond.i.i to i32
  %tmp46.i.i = xor i32 %bothcond1.i.i, 1
  %count.0.i.i = add i32 %count.1.i.i, %tmp46.i.i
  %tmp50.i.i = add i32 %x.0.i.i, 2
  br label %bb51.i.i

bb51.i.i:
  %count.1.i.i = phi i32 [ %count.0.i.i, %bb27.i.i ], [ 0, %bb.i ]
  %x.0.i.i = phi i32 [ %tmp50.i.i, %bb27.i.i ], [ 0, %bb.i ]
  %tmp54.i.i = icmp slt i32 %x.0.i.i, 0
  br i1 %tmp54.i.i, label %bb27.i.i, label %bb57.i.i

bb57.i.i:
  ret i32 0

calculateColorSpecificBlackLevel.exit:
  ret i32 undef
}
