// RUN: %clang_cc1 -fopenacc -Wno-openacc-self-if-potential-conflict -emit-cir -fclangir %s -o - | FileCheck %s

void acc_parallel(int cond) {
  // CHECK: cir.func @acc_parallel(%[[ARG:.*]]: !s32i{{.*}}) {
  // CHECK-NEXT: %[[COND:.*]] = cir.alloca !s32i, !cir.ptr<!s32i>, ["cond", init]
  // CHECK-NEXT: cir.store %[[ARG]], %[[COND]] : !s32i, !cir.ptr<!s32i>
#pragma acc parallel
  {}
  // CHECK-NEXT: acc.parallel {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT:}

#pragma acc parallel default(none)
  {}
  // CHECK-NEXT: acc.parallel {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } attributes {defaultAttr = #acc<defaultvalue none>}

#pragma acc parallel default(present)
  {}
  // CHECK-NEXT: acc.parallel {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } attributes {defaultAttr = #acc<defaultvalue present>}

#pragma acc parallel
  while(1){}
  // CHECK-NEXT: acc.parallel {
  // CHECK-NEXT: cir.scope {
  // CHECK-NEXT: cir.while {
  // CHECK-NEXT: %[[INT:.*]] = cir.const #cir.int<1>
  // CHECK-NEXT: %[[CAST:.*]] = cir.cast(int_to_bool, %[[INT]] :
  // CHECK-NEXT: cir.condition(%[[CAST]])
  // CHECK-NEXT: } do {
  // CHECK-NEXT: cir.yield
  // cir.while do end:
  // CHECK-NEXT: }
  // cir.scope end:
  // CHECK-NEXT: }
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT:}

#pragma acc parallel self
  {}
  // CHECK-NEXT: acc.parallel {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } attributes {selfAttr}

#pragma acc parallel self(cond)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[BOOL_CAST:.*]] = cir.cast(int_to_bool, %[[COND_LOAD]] : !s32i), !cir.bool
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[BOOL_CAST]] : !cir.bool to i1
  // CHECK-NEXT: acc.parallel self(%[[CONV_CAST]]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel self(0)
  {}
  // CHECK-NEXT: %[[ZERO_LITERAL:.*]] = cir.const #cir.int<0> : !s32i
  // CHECK-NEXT: %[[BOOL_CAST:.*]] = cir.cast(int_to_bool, %[[ZERO_LITERAL]] : !s32i), !cir.bool
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[BOOL_CAST]] : !cir.bool to i1
  // CHECK-NEXT: acc.parallel self(%[[CONV_CAST]]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel if(cond)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[BOOL_CAST:.*]] = cir.cast(int_to_bool, %[[COND_LOAD]] : !s32i), !cir.bool
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[BOOL_CAST]] : !cir.bool to i1
  // CHECK-NEXT: acc.parallel if(%[[CONV_CAST]]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel if(1)
  {}
  // CHECK-NEXT: %[[ONE_LITERAL:.*]] = cir.const #cir.int<1> : !s32i
  // CHECK-NEXT: %[[BOOL_CAST:.*]] = cir.cast(int_to_bool, %[[ONE_LITERAL]] : !s32i), !cir.bool
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[BOOL_CAST]] : !cir.bool to i1
  // CHECK-NEXT: acc.parallel if(%[[CONV_CAST]]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel if(cond == 1)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[ONE_LITERAL:.*]] = cir.const #cir.int<1> : !s32i
  // CHECK-NEXT: %[[EQ_RES:.*]] = cir.cmp(eq, %[[COND_LOAD]], %[[ONE_LITERAL]]) : !s32i, !cir.bool
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[EQ_RES]] : !cir.bool to i1
  // CHECK-NEXT: acc.parallel if(%[[CONV_CAST]]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel if(cond == 1) self(cond == 2)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[ONE_LITERAL:.*]] = cir.const #cir.int<1> : !s32i
  // CHECK-NEXT: %[[EQ_RES_IF:.*]] = cir.cmp(eq, %[[COND_LOAD]], %[[ONE_LITERAL]]) : !s32i, !cir.bool
  // CHECK-NEXT: %[[CONV_CAST_IF:.*]] = builtin.unrealized_conversion_cast %[[EQ_RES_IF]] : !cir.bool to i1
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !s32i
  // CHECK-NEXT: %[[EQ_RES_SELF:.*]] = cir.cmp(eq, %[[COND_LOAD]], %[[TWO_LITERAL]]) : !s32i, !cir.bool
  // CHECK-NEXT: %[[CONV_CAST_SELF:.*]] = builtin.unrealized_conversion_cast %[[EQ_RES_SELF]] : !cir.bool to i1
  // CHECK-NEXT: acc.parallel self(%[[CONV_CAST_SELF]]) if(%[[CONV_CAST_IF]]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel num_workers(cond)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[COND_LOAD]] : !s32i to si32
  // CHECK-NEXT: acc.parallel num_workers(%[[CONV_CAST]] : si32) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel num_workers(cond) device_type(nvidia) num_workers(2u)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[COND_LOAD]] : !s32i to si32
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !u32i
  // CHECK-NEXT: %[[TWO_CAST:.*]] = builtin.unrealized_conversion_cast %[[TWO_LITERAL]] : !u32i to ui32
  // CHECK-NEXT: acc.parallel num_workers(%[[CONV_CAST]] : si32, %[[TWO_CAST]] : ui32 [#acc.device_type<nvidia>]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel num_workers(cond) device_type(nvidia, host) num_workers(2) device_type(radeon) num_workers(3)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[COND_LOAD]] : !s32i to si32
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !s32i
  // CHECK-NEXT: %[[TWO_CAST:.*]] = builtin.unrealized_conversion_cast %[[TWO_LITERAL]] : !s32i to si32
  // CHECK-NEXT: %[[THREE_LITERAL:.*]] = cir.const #cir.int<3> : !s32i
  // CHECK-NEXT: %[[THREE_CAST:.*]] = builtin.unrealized_conversion_cast %[[THREE_LITERAL]] : !s32i to si32
  // CHECK-NEXT: acc.parallel num_workers(%[[CONV_CAST]] : si32, %[[TWO_CAST]] : si32 [#acc.device_type<nvidia>], %[[TWO_CAST]] : si32 [#acc.device_type<host>], %[[THREE_CAST]] : si32 [#acc.device_type<radeon>]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel num_workers(cond) device_type(nvidia) num_workers(2) device_type(radeon, multicore) num_workers(4)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[COND_LOAD]] : !s32i to si32
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !s32i
  // CHECK-NEXT: %[[TWO_CAST:.*]] = builtin.unrealized_conversion_cast %[[TWO_LITERAL]] : !s32i to si32
  // CHECK-NEXT: %[[FOUR_LITERAL:.*]] = cir.const #cir.int<4> : !s32i
  // CHECK-NEXT: %[[FOUR_CAST:.*]] = builtin.unrealized_conversion_cast %[[FOUR_LITERAL]] : !s32i to si32
  // CHECK-NEXT: acc.parallel num_workers(%[[CONV_CAST]] : si32, %[[TWO_CAST]] : si32 [#acc.device_type<nvidia>], %[[FOUR_CAST]] : si32 [#acc.device_type<radeon>], %[[FOUR_CAST]] : si32 [#acc.device_type<multicore>]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel device_type(nvidia) num_workers(2) device_type(radeon) num_workers(3)
  {}
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !s32i
  // CHECK-NEXT: %[[TWO_CAST:.*]] = builtin.unrealized_conversion_cast %[[TWO_LITERAL]] : !s32i to si32
  // CHECK-NEXT: %[[THREE_LITERAL:.*]] = cir.const #cir.int<3> : !s32i
  // CHECK-NEXT: %[[THREE_CAST:.*]] = builtin.unrealized_conversion_cast %[[THREE_LITERAL]] : !s32i to si32
  // CHECK-NEXT: acc.parallel num_workers(%[[TWO_CAST]] : si32 [#acc.device_type<nvidia>], %[[THREE_CAST]] : si32 [#acc.device_type<radeon>]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel vector_length(cond)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[COND_LOAD]] : !s32i to si32
  // CHECK-NEXT: acc.parallel vector_length(%[[CONV_CAST]] : si32) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel vector_length(cond) device_type(nvidia) vector_length(2u)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[COND_LOAD]] : !s32i to si32
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !u32i
  // CHECK-NEXT: %[[TWO_CAST:.*]] = builtin.unrealized_conversion_cast %[[TWO_LITERAL]] : !u32i to ui32
  // CHECK-NEXT: acc.parallel vector_length(%[[CONV_CAST]] : si32, %[[TWO_CAST]] : ui32 [#acc.device_type<nvidia>]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel vector_length(cond) device_type(nvidia, host) vector_length(2) device_type(radeon) vector_length(3)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[COND_LOAD]] : !s32i to si32
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !s32i
  // CHECK-NEXT: %[[TWO_CAST:.*]] = builtin.unrealized_conversion_cast %[[TWO_LITERAL]] : !s32i to si32
  // CHECK-NEXT: %[[THREE_LITERAL:.*]] = cir.const #cir.int<3> : !s32i
  // CHECK-NEXT: %[[THREE_CAST:.*]] = builtin.unrealized_conversion_cast %[[THREE_LITERAL]] : !s32i to si32
  // CHECK-NEXT: acc.parallel vector_length(%[[CONV_CAST]] : si32, %[[TWO_CAST]] : si32 [#acc.device_type<nvidia>], %[[TWO_CAST]] : si32 [#acc.device_type<host>], %[[THREE_CAST]] : si32 [#acc.device_type<radeon>]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel vector_length(cond) device_type(nvidia) vector_length(2) device_type(radeon, multicore) vector_length(4)
  {}
  // CHECK-NEXT: %[[COND_LOAD:.*]] = cir.load %[[COND]] : !cir.ptr<!s32i>, !s32i
  // CHECK-NEXT: %[[CONV_CAST:.*]] = builtin.unrealized_conversion_cast %[[COND_LOAD]] : !s32i to si32
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !s32i
  // CHECK-NEXT: %[[TWO_CAST:.*]] = builtin.unrealized_conversion_cast %[[TWO_LITERAL]] : !s32i to si32
  // CHECK-NEXT: %[[FOUR_LITERAL:.*]] = cir.const #cir.int<4> : !s32i
  // CHECK-NEXT: %[[FOUR_CAST:.*]] = builtin.unrealized_conversion_cast %[[FOUR_LITERAL]] : !s32i to si32
  // CHECK-NEXT: acc.parallel vector_length(%[[CONV_CAST]] : si32, %[[TWO_CAST]] : si32 [#acc.device_type<nvidia>], %[[FOUR_CAST]] : si32 [#acc.device_type<radeon>], %[[FOUR_CAST]] : si32 [#acc.device_type<multicore>]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

#pragma acc parallel device_type(nvidia) vector_length(2) device_type(radeon) vector_length(3)
  {}
  // CHECK-NEXT: %[[TWO_LITERAL:.*]] = cir.const #cir.int<2> : !s32i
  // CHECK-NEXT: %[[TWO_CAST:.*]] = builtin.unrealized_conversion_cast %[[TWO_LITERAL]] : !s32i to si32
  // CHECK-NEXT: %[[THREE_LITERAL:.*]] = cir.const #cir.int<3> : !s32i
  // CHECK-NEXT: %[[THREE_CAST:.*]] = builtin.unrealized_conversion_cast %[[THREE_LITERAL]] : !s32i to si32
  // CHECK-NEXT: acc.parallel vector_length(%[[TWO_CAST]] : si32 [#acc.device_type<nvidia>], %[[THREE_CAST]] : si32 [#acc.device_type<radeon>]) {
  // CHECK-NEXT: acc.yield
  // CHECK-NEXT: } loc

  // CHECK-NEXT: cir.return
}
