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
#include <nodepp/debug.h>
#include <emscripten/websocket.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace _ws_ {

    int WS_EVENT_MESSAGE( int type, const EmscriptenWebSocketMessageEvent* ev, void* userData );

    int WS_EVENT_CLOSE( int type, const EmscriptenWebSocketCloseEvent* ev, void* userData );

    int WS_EVENT_ERROR( int type, const EmscriptenWebSocketErrorEvent* ev, void* userData );

    int WS_EVENT_OPEN( int type, const EmscriptenWebSocketOpenEvent* ev, void* userData );

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class ws_t {
protected:

    struct NODE {
        ulong timeout=0;
        ushort wait = 0;
        int      fd =-1;
        int    feof = 0;
        bool  state = 0;
    };  ptr_t<NODE> obj;

public: ws_t() noexcept : obj( new NODE() ){}

    event_t<ws_t>      onConnect;
    event_t<>          onDrain;
    event_t<except_t>  onError;
    event_t<>          onClose;
    event_t<string_t>  onData;
    
    /*─······································································─*/
    
    virtual ~ws_t() noexcept { if( obj.count() > 1 ){ return; } free(); }
    
    /*─······································································─*/

    ws_t( const string_t& url ) noexcept : obj( new NODE() ) {

        if( !emscripten_websocket_is_supported() ){ 
            _EERROR(onError,"WS not Supported"); return;
        }   

        EmscriptenWebSocketCreateAttributes attr;
        memset( &attr, 0, sizeof( attr ) );
        attr.url                = url.c_str();
        attr.protocols          = nullptr;
        attr.createOnMainThread = true;

        obj->fd = emscripten_websocket_new( &attr ); if( obj->fd <= 0 ){ 
            _EERROR(onError,"Couldn't connect to the server"); return;
        }
        
        obj->timeout = process::millis() + 1000;
        obj->state   = 1;
        auto self    = type::bind( this );
        
        process::add([=](){
        coStart

          //emscripten_websocket_set_onopen_callback   ( self->obj->fd, &self, _ws_::WS_EVENT_OPEN    );
            emscripten_websocket_set_onclose_callback  ( self->obj->fd, &self, _ws_::WS_EVENT_CLOSE   );
            emscripten_websocket_set_onerror_callback  ( self->obj->fd, &self, _ws_::WS_EVENT_ERROR   );
            emscripten_websocket_set_onmessage_callback( self->obj->fd, &self, _ws_::WS_EVENT_MESSAGE );

            while( self->_connect()==-2 ){ coNext; } if ( self->is_closed() ){
                _EERROR( self->onError, "Couldn't connect to the server" ); coEnd;
            }   self->onConnect.emit( *self ); while( !self->is_closed() ){ coNext; }

        coStop
        });

    }

    /*─······································································─*/
    
    int _connect() const noexcept {
        if( process::millis() > obj->timeout ){ obj->wait=3; return -1; }
            emscripten_websocket_get_ready_state( obj->fd, &obj->wait );
        if( obj->wait == 0 ){ return -2; } return obj->wait>1? -1 : 0;
    }
    
    int connect() const noexcept { int c = 0;
        while( (c=_connect())==-2 ){ process::next(); }
        return c;
    }

    /*─······································································─*/
    
    void close() const noexcept { if( is_available() ){ 
        emscripten_websocket_delete( obj->fd );
        onDrain.emit(); free();
    }}
    
    /*─······································································─*/
    
    virtual void free() const noexcept { 
        if( obj->state == 0 ){ return; } 
            obj->state  = 0; onClose.emit();
    }

    /*─······································································─*/

    bool is_available() const noexcept { return !is_closed(); }

    int get_fd()        const noexcept { return obj->fd; }

    bool is_closed()    const noexcept { 
        return obj->fd    <= 0 || obj->feof != 0 || 
               obj->state == 0 || obj->wait >  1 ; 
    }
    
    /*─······································································─*/

    string_t read( ulong /*unused*/ ) const noexcept { return nullptr; }

    int write( string_t msg ) const noexcept {
        if( is_closed() || msg.empty() ){ return -1; }
            obj->feof  = emscripten_websocket_send_binary( obj->fd, msg.get(), msg.size() );
        if( obj->feof != 0 ){ close(); return -1; } return msg.size();
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace _ws_ {

    EMSCRIPTEN_RESULT WS_EVENT_CLOSE( int /*unused*/, const EmscriptenWebSocketCloseEvent* /*unused*/, void* userData ) {
        if( userData == nullptr ){ return EM_FALSE; } auto data = type::cast<ws_t>( userData ); 
        if( data->is_closed()   ){ return EM_FALSE; } 
        data->onData.clear(); data->close(); 
        return EM_FALSE;
    }

    EMSCRIPTEN_RESULT WS_EVENT_ERROR( int /*unused*/, const EmscriptenWebSocketErrorEvent* /*unused*/, void* userData ) {
        if( userData == nullptr ){ return EM_FALSE; } auto data = type::cast<ws_t>( userData ); 
        if( data->is_closed()   ){ return EM_FALSE; }
        data->onError.emit( except_t( "something went wrong" ) );
        data->onData.clear(); data->close(); return EM_FALSE;
    }

    EMSCRIPTEN_RESULT WS_EVENT_MESSAGE( int /*unused*/, const EmscriptenWebSocketMessageEvent* ev, void* userData ) {
        if( userData == nullptr ){ return EM_FALSE; } auto data = type::cast<ws_t>( userData );
        if( data->is_closed()   ){ return EM_FALSE; }
        data->onData.emit( string_t( (char*)ev->data, ev->numBytes ) ); return EM_TRUE;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace ws {
    ws_t connect( const string_t& url ) { return ws_t(url); }
}}

/*────────────────────────────────────────────────────────────────────────────*/
