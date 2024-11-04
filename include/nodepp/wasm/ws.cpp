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
        int     fd =-1;
        int   feof = 0;
        bool state = 0;
    };  ptr_t<NODE> obj;

public: ws_t() noexcept : obj( new NODE() ){}

    event_t<ws_t>      onConnect;
    event_t<>          onDrain;
    event_t<except_t>  onError;
    event_t<>          onClose;
    event_t<string_t>  onData;
    
    /*─······································································─*/
    
    virtual ~ws_t() noexcept {
        if( obj.count() > 1 ){ return; } 
        if( obj->state == 0 ){ return; } free();
    }
    
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
        }   auto self = type::bind( this );

        process::add([=](){ 
        coStart
            while( self->is_closed() ){ coDelay(1000); if( self->is_closed() ){ 
                _EERROR(self->onError,"Couldn't connect to the server"); coEnd; 
            }}  while(!self->is_closed() ){ coNext; }
        coStop });

        emscripten_websocket_set_onopen_callback( obj->fd, &self, _ws_::WS_EVENT_OPEN );
        emscripten_websocket_set_onclose_callback( obj->fd, &self, _ws_::WS_EVENT_CLOSE );
        emscripten_websocket_set_onerror_callback( obj->fd, &self, _ws_::WS_EVENT_ERROR );
        emscripten_websocket_set_onmessage_callback( obj->fd, &self, _ws_::WS_EVENT_MESSAGE );
    }

    /*─······································································─*/

    void resume() const noexcept { obj->state = 1; }
    
    void  close() const noexcept { 
        if( obj->state == 0 ){ return; } 
            obj->state =  0; onDrain.emit(); free();
    }
    
    /*─······································································─*/
    
    virtual void free() const noexcept {
        if( obj.count() > 1 || is_closed() ){ return; }
        emscripten_websocket_close( obj->fd, 1000, "Closing connection" );
        emscripten_websocket_delete( obj->fd ); obj->state = 0; onClose.emit();
    }
    
    /*─······································································─*/

    bool is_available() const noexcept { return !is_closed(); }

    bool is_closed() const noexcept { return obj->fd <= 0 || obj->feof != 0 || obj->state == 0; }
    
    /*─······································································─*/

    string_t read( ulong /*unused*/ ) const noexcept { return nullptr; }

    int write( string_t msg ) const noexcept {
        if( is_closed() || msg.empty() ){ return -1; }
        obj->feof = emscripten_websocket_send_binary( obj->fd, msg.get(), msg.size() );
        if( obj->feof != 0 ){ close(); return -1; } return msg.size();
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace _ws_ {

    int WS_EVENT_MESSAGE( int /*unused*/, const EmscriptenWebSocketMessageEvent* ev, void* userData ) {
        string_t message ( (char*) ev->data, ev->numBytes );
        auto data = type::cast<ws_t>( userData );
        data->onData.emit( message );
        return ev->socket;
    }

    int WS_EVENT_CLOSE( int /*unused*/, const EmscriptenWebSocketCloseEvent* ev, void* userData ) {
        auto data = type::cast<ws_t>( userData ); 
        data->onData.clear(); data->close(); 
        return ev->socket;
    }

    int WS_EVENT_ERROR( int /*unused*/, const EmscriptenWebSocketErrorEvent* ev, void* userData ) {
        string_t error = "something went wrong";
        auto data = type::cast<ws_t>( userData );
        data->onError.emit( except_t( error ) );
        data->close(); return ev->socket;
    }

    int WS_EVENT_OPEN( int /*unused*/, const EmscriptenWebSocketOpenEvent* ev, void* userData ) {
        auto data = type::cast<ws_t>( userData ); data->resume();
        data->onConnect.emit( *data );
        return ev->socket;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace ws {
    ws_t connect( const string_t& url ) { return ws_t(url); }
}}

/*────────────────────────────────────────────────────────────────────────────*/
