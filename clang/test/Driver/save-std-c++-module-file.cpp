// RUN: rm -rf %t
// RUN: mkdir %t
// RUN: split-file %s %t
//
// RUN: %clang -std=c++20 %t/Hello.cppm -fsave-std-c++-module-file -### 2>&1 | \
// RUN:   FileCheck %t/Hello.cppm -DPREFIX=%t
//
// RUN: %clang -std=c++20 %t/Hello.cppm -fsave-std-c++-module-file -o %t/output/Hello.o \
// RUN:   -### 2>&1 | FileCheck %t/Hello.cppm -DPREFIX=%t --check-prefix=CHECK-WITH-OUTPUT
//
// RUN: %clang -std=c++20 %t/Hello.cppm -fsave-std-c++-module-file=%t/tmp/H.pcm \
// RUN:   -### 2>&1 | FileCheck %t/Hello.cppm -DPREFIX=%t --check-prefix=CHECK-SPECIFIED
//
// RUN: %clang -std=c++20 %t/Hello.cppm -fsave-std-c++-module-file=%t/tmp/H.pcm -o %t/Hello.o \
// RUN:   -### 2>&1 | FileCheck %t/Hello.cppm -DPREFIX=%t --check-prefix=CHECK-SPECIFIED

//--- Hello.cppm
export module Hello;

// CHECK: "-o" "[[PREFIX]]/Hello.pcm"
// CHECK-WITH-OUTPUT: "-o" "[[PREFIX]]/output/Hello.pcm"
// CHECK-SPECIFIED: "-o" "[[PREFIX]]/tmp/H.pcm"
