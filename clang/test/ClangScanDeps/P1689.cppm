// RUN: rm -fr %t
// RUN: mkdir -p %t
// RUN: split-file %s %t
//
// RUN: sed "s|DIR|%/t|g" %t/P1689.json.in > %t/P1689.json
// RUN: clang-scan-deps --compilation-database %t/P1689.json -format=p1689 | FileCheck %t/Checks.cpp -DPREFIX=%/t
// RUN: clang-scan-deps --mode=preprocess-dependency-directives --compilation-database %t/P1689.json -format=p1689 | FileCheck %t/Checks.cpp -DPREFIX=%/t
//
// Check the seperated dependency format.
// RUN: clang-scan-deps -format=p1689 --p1689-targeted-file-name=%t/M.cppm --p1689-targeted-output=%t/M.o \
// RUN:   -- -std=c++20 -c \
// RUN:   | FileCheck %t/M.cppm -DPREFIX=%/t
// RUN: clang-scan-deps -format=p1689 --p1689-targeted-file-name=%t/Impl.cpp --p1689-targeted-output=%t/Impl.o \
// RUN:   -- -std=c++20 -c \
// RUN:   | FileCheck %t/Impl.cpp -DPREFIX=%/t
// RUN: clang-scan-deps -format=p1689 --p1689-targeted-file-name=%t/impl_part.cppm --p1689-targeted-output=%t/impl_part.o \
// RUN:   -- -std=c++20 -c \
// RUN:   | FileCheck %t/impl_part.cppm -DPREFIX=%/t
// RUN: clang-scan-deps -format=p1689 --p1689-targeted-file-name=%t/interface_part.cppm --p1689-targeted-output=%t/interface_part.o \
// RUN:   -- -std=c++20 -c \
// RUN:   | FileCheck %t/interface_part.cppm -DPREFIX=%/t
// RUN: clang-scan-deps -format=p1689 --p1689-targeted-file-name=%t/User.cpp --p1689-targeted-output=%t/User.o \
// RUN:   -- -std=c++20 -c \
// RUN:   | FileCheck %t/User.cpp -DPREFIX=%/t
//
// Check we can generate the make-style dependencies as expected.
// RUN: clang-scan-deps -format=p1689 --p1689-targeted-file-name=%t/impl_part.cppm --p1689-targeted-output=%t/impl_part.o \
// RUN:   --p1689-makeformat-output=%t/impl_part.dep -- -std=c++20 -c -MT %t/impl_part.o.ddi -MD
// RUN: cat %t/impl_part.dep | FileCheck %t/impl_part.cppm -DPREFIX=%/t --check-prefix=CHECK-MAKE

//--- P1689.json.in
[
{
  "directory": "DIR",
  "command": "clang++ -std=c++20 DIR/M.cppm -c -o DIR/M.o",
  "file": "DIR/M.cppm",
  "output": "DIR/M.o"
},
{
  "directory": "DIR",
  "command": "clang++ -std=c++20 DIR/Impl.cpp -c -o DIR/Impl.o",
  "file": "DIR/Impl.cpp",
  "output": "DIR/Impl.o"
},
{
  "directory": "DIR",
  "command": "clang++ -std=c++20 DIR/impl_part.cppm -c -o DIR/impl_part.o",
  "file": "DIR/impl_part.cppm",
  "output": "DIR/impl_part.o"
},
{
  "directory": "DIR",
  "command": "clang++ -std=c++20 DIR/interface_part.cppm -c -o DIR/interface_part.o",
  "file": "DIR/interface_part.cppm",
  "output": "DIR/interface_part.o"
},
{
  "directory": "DIR",
  "command": "clang++ -std=c++20 DIR/User.cpp -c -o DIR/User.o",
  "file": "DIR/User.cpp",
  "output": "DIR/User.o"
}
]

//--- M.cppm
export module M;
export import :interface_part;
import :impl_part;
export void Hello();

// CHECK: {
// CHECK-NEXT:   "revision": 0,
// CHECK-NEXT:   "rules": [
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/M.o",
// CHECK-NEXT:       "provides": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "is-interface": true,
// CHECK-NEXT:           "logical-name": "M",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/M.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ],
// CHECK-NEXT:       "requires": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M:interface_part"
// CHECK-NEXT:         },
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M:impl_part"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     }
// CHECK-NEXT:   ],
// CHECK-NEXT:   "version": 1
// CHECK-NEXT: }

//--- Impl.cpp
module;
#include "header.mock"
module M;
void Hello() {
    std::cout << "Hello ";
}

// CHECK: {
// CHECK-NEXT:   "revision": 0,
// CHECK-NEXT:   "rules": [
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/Impl.o",
// CHECK-NEXT:       "requires": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     }
// CHECK-NEXT:   ],
// CHECK-NEXT:   "version": 1
// CHECK-NEXT: }

//--- impl_part.cppm
module;
#include "header.mock"
module M:impl_part;
import :interface_part;

std::string W = "World.";
void World() {
    std::cout << W << std::endl;
}

// CHECK: {
// CHECK-NEXT:   "revision": 0,
// CHECK-NEXT:   "rules": [
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/impl_part.o",
// CHECK-NEXT:       "provides": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "is-interface": false,
// CHECK-NEXT:           "logical-name": "M:impl_part",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/impl_part.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ],
// CHECK-NEXT:       "requires": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M:interface_part"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     }
// CHECK-NEXT:   ],
// CHECK-NEXT:   "version": 1
// CHECK-NEXT: }

// CHECK-MAKE: [[PREFIX]]/impl_part.o.ddi:
// CHECK-MAKE:   [[PREFIX]]/impl_part.cppm
// CHECK-MAKE:   [[PREFIX]]/header.mock

//--- interface_part.cppm
export module M:interface_part;
export void World();

// CHECK: {
// CHECK-NEXT:   "revision": 0,
// CHECK-NEXT:   "rules": [
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/interface_part.o",
// CHECK-NEXT:       "provides": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "is-interface": true,
// CHECK-NEXT:           "logical-name": "M:interface_part",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/interface_part.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     }
// CHECK-NEXT:   ],
// CHECK-NEXT:   "version": 1
// CHECK-NEXT: }

//--- User.cpp
import M;
import third_party_module;
int main() {
    Hello();
    World();
    return 0;
}

// CHECK: {
// CHECK-NEXT:   "revision": 0,
// CHECK-NEXT:   "rules": [
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/User.o",
// CHECK-NEXT:       "requires": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M"
// CHECK-NEXT:         },
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "third_party_module"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     }
// CHECK-NEXT:   ],
// CHECK-NEXT:   "version": 1
// CHECK-NEXT: }

//--- Checks.cpp
// CHECK: {
// CHECK-NEXT:   "revision": 0,
// CHECK-NEXT:   "rules": [
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/Impl.o",
// CHECK-NEXT:       "requires": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/M.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     },
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/M.o",
// CHECK-NEXT:       "provides": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "is-interface": true,
// CHECK-NEXT:           "logical-name": "M",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/M.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ],
// CHECK-NEXT:       "requires": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M:interface_part",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/interface_part.cppm"
// CHECK-NEXT:         },
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M:impl_part",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/impl_part.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     },
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/User.o",
// CHECK-NEXT:       "requires": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/M.cppm"
// CHECK-NEXT:         },
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "third_party_module"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     },
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/impl_part.o",
// CHECK-NEXT:       "provides": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "is-interface": false,
// CHECK-NEXT:           "logical-name": "M:impl_part",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/impl_part.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ],
// CHECK-NEXT:       "requires": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "logical-name": "M:interface_part",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/interface_part.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     },
// CHECK-NEXT:     {
// CHECK-NEXT:       "primary-output": "[[PREFIX]]/interface_part.o",
// CHECK-NEXT:       "provides": [
// CHECK-NEXT:         {
// CHECK-NEXT:           "is-interface": true,
// CHECK-NEXT:           "logical-name": "M:interface_part",
// CHECK-NEXT:           "source-path": "[[PREFIX]]/interface_part.cppm"
// CHECK-NEXT:         }
// CHECK-NEXT:       ]
// CHECK-NEXT:     }
// CHECK-NEXT:   ],
// CHECK-NEXT:   "version": 1
// CHECK-NEXT: }

//--- header.mock
