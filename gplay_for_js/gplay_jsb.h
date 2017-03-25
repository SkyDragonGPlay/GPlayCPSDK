#ifndef __JSB_GPLAY_H__
#define __JSB_GPLAY_H__

#include "jsapi.h"
#include "cocos2d.h"

#if COCOS2D_VERSION >= 0x00030500
    void register_gplay_runtime(JSContext* cx, JS::HandleObject global);
#else
    void register_gplay_runtime(JSContext* cx, JSObject* global);
#endif

#endif /* __JSB_GPLAY_H__ */