// Tests that the .pcm file will be generated in the same direcotry with the specified 
// output and the name of the .pcm file should be the same with the output file.
// RUN: %clang -std=c++20 %s -fmodule-output -c -o %t/output/Hello.o \
// RUN:   -### 2>&1 | FileCheck %s
//
// Tests that the .pcm file will be generated in the same path with the specified one in the comamnd line.
// RUN: %clang -std=c++20 %s -fmodule-output=%t/pcm/Hello.pcm -o %t/Hello.o \
// RUN:   -c -### 2>&1 | FileCheck %s --check-prefix=CHECK-SPECIFIED

export module Hello;

// CHECK: "-emit-module-interface" {{.*}}"-main-file-name" "module-output.cppm" {{.*}}"-o" "{{.*}}/Hello.pcm" "-x" "c++" "{{.*}}/module-output.cppm"
// CHECK: "-emit-obj" {{.*}}"-main-file-name" "module-output.cppm" {{.*}}"-o" "{{.*}}/Hello.o" "-x" "pcm" "{{.*}}/Hello.pcm"
// CHECK-SPECIFIED: "-emit-module-interface" {{.*}}"-main-file-name" "module-output.cppm" {{.*}}"-o" "{{.*}}/Hello.pcm" "-x" "c++" "{{.*}}/module-output.cppm"
// CHECK-SPECIFIED: "-emit-obj" {{.*}}"-main-file-name" "module-output.cppm" {{.*}}"-o" "{{.*}}/Hello.o" "-x" "pcm" "{{.*}}/Hello.pcm"
