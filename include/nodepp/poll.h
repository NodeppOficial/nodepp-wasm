/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_POLL
#define NODEPP_POLL

#if _KERNEL == NODEPP_KERNEL_WASM
    #include "event.h"
    #include "wasm/poll.h"
#else
    #error "This poll Does not support dns.h"
#endif

#endif

/*────────────────────────────────────────────────────────────────────────────*/