/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_FILE
#define NODEPP_FILE

/*────────────────────────────────────────────────────────────────────────────*/

#if   _KERNEL == NODEPP_KERNEL_WASM
    #include "os.h"
    #include "event.h"
    #include "generator.h"
    #include "wasm/file.cpp"
#else
    #error "This OS Does not support file.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif