/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#pragma once
#include <emscripten/websocket.h>

namespace nodepp { queue_t<void*> user; }

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class ws_t {
protected:

    struct NODE {
        string_t msg ;
        int     fd =-1;
        int  state = 0;
    };  ptr_t<NODE> obj;

private:

    static EMSCRIPTEN_RESULT WS_EVENT_CLOSE( int /*unused*/, const EmscriptenWebSocketCloseEvent* ev, void* userData ) {
        if( userData == nullptr ){ return EM_FALSE; }  auto user = type::cast<queue_t<void*>>( userData );
        auto x = user->first(); while( x != nullptr ){ auto z = type::cast<ws_t>( x->data ); auto y = x->next;
            if( z->obj->fd==ev->socket ){ z->obj->state |= 8; break; }
        x = y; } return EM_TRUE;
    }

    static EMSCRIPTEN_RESULT WS_EVENT_ERROR( int /*unused*/, const EmscriptenWebSocketErrorEvent* ev, void* userData ) {
        if( userData == nullptr ){ return EM_FALSE; }  auto user = type::cast<queue_t<void*>>( userData );
        auto x = user->first(); while( x != nullptr ){ auto z = type::cast<ws_t>( x->data ); auto y = x->next;
            if( z->obj->fd==ev->socket ){ z->obj->state |= 4; break; }
        x = y; } return EM_TRUE;
    }

    static EMSCRIPTEN_RESULT WS_EVENT_OPEN( int /*unused*/, const EmscriptenWebSocketOpenEvent* ev, void* userData ) {
        if( userData == nullptr ){ return EM_FALSE; }  auto user = type::cast<queue_t<void*>>( userData );
        auto x = user->first(); while( x != nullptr ){ auto z = type::cast<ws_t>( x->data ); auto y = x->next;
            if( z->obj->fd==ev->socket ){ z->obj->state |= 2; break; }
        x = y; } return EM_TRUE;
    }

    static EMSCRIPTEN_RESULT WS_EVENT_MESSAGE( int /*unused*/, const EmscriptenWebSocketMessageEvent* ev, void* userData ) {
        if( userData == nullptr ){ return EM_FALSE; }  auto user = type::cast<queue_t<void*>>( userData );
        auto x = user->first(); while( x != nullptr ){ auto z = type::cast<ws_t>( x->data ); auto y = x->next;
            if( z->obj->fd==ev->socket ){ z->obj->state |= 1; z->obj->msg = string_t( (char*)ev->data, ev->numBytes ); break; }
        x = y; } return EM_TRUE;
    }

public: ws_t() noexcept : obj( new NODE() ){}

    event_t<ws_t>      onConnect;
    event_t<>          onDrain;
    event_t<except_t>  onError;
    event_t<>          onClose;
    event_t<string_t>  onData;
    
    /*─······································································─*/
    
    virtual ~ws_t() noexcept { if( obj.count() > 1 ) { return; } free(); }
    
    /*─······································································─*/

    ws_t( const string_t& url ) noexcept : obj( new NODE() ) {
        auto self = type::bind( this ); obj->state=1;

        if( !emscripten_websocket_is_supported() ){ 
            _EERROR(onError,"WS not Supported"); 
            free(); return;
        }

        ptr_t<EmscriptenWebSocketCreateAttributes> attr ;
        attr = new EmscriptenWebSocketCreateAttributes();
        attr->url                = url.c_str();
        attr->protocols          = nullptr;
        attr->createOnMainThread = EM_TRUE;
        
        obj->fd = emscripten_websocket_new( &attr ); user.push( &self );

        process::poll::add([=](){ 
        coStart

            while( self->is_available() ){ 
               if( self->obj->state & 4 ){ self->onError.emit("Something Went Wrong"); }
               if( self->obj->state & 1 ){ self->onData.emit( self->obj->msg ); }
               if( self->obj->state & 2 ){ self->onConnect.emit( *self ); }
               if( self->obj->state & 8 ){ self->onDrain.emit(); break; }
                   self->obj->state &=0; coNext; 
            }
            
            do {
            auto x = user.first(); while( x != nullptr ){
            auto z = type::cast<ws_t>( x->data );
            auto y = x->next; 

                if( z->get_fd() == self->obj->fd )
                  { user.erase( x ); break; } x = y;

            }} while(0);

            coDelay(100); self->onClose.emit();

        coStop
        });

        emscripten_websocket_set_onopen_callback   ( obj->fd, (void*)&user, WS_EVENT_OPEN    );
        emscripten_websocket_set_onclose_callback  ( obj->fd, (void*)&user, WS_EVENT_CLOSE   );
        emscripten_websocket_set_onerror_callback  ( obj->fd, (void*)&user, WS_EVENT_ERROR   );
        emscripten_websocket_set_onmessage_callback( obj->fd, (void*)&user, WS_EVENT_MESSAGE );

    }

    /*─······································································─*/

    void free()  const noexcept { if( !is_available() ){ return; } obj->state=-1; }
    
    void close() const noexcept { if( !is_available() ){ return; }
        emscripten_websocket_delete( obj->fd ); free();
    }
    
    /*─······································································─*/

    bool is_closed()    const noexcept { return obj->state < 0; }

    bool is_available() const noexcept { return !is_closed(); }

    int get_fd()        const noexcept { return obj->fd; }
    
    /*─······································································─*/

    string_t read( ulong /*unused*/ ) const noexcept { return nullptr; }

    int write( string_t msg ) const noexcept { 
        if( is_closed() || msg.empty() ){ return -1; }
        return emscripten_websocket_send_binary( obj->fd, msg.get(), msg.size() );
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace ws {
    ws_t connect( const string_t& url ) { return ws_t(url); }
}}

/*────────────────────────────────────────────────────────────────────────────*/
