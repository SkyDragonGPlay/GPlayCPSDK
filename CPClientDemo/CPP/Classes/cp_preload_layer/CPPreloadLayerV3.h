#ifndef __GPLAY_PRELOAD_LAYER_H__
#define __GPLAY_PRELOAD_LAYER_H__

#include "cocos2d.h"

#if COCOS2D_VERSION >= 0x00030000
using namespace cocos2d;

namespace cp {
    class GPlayPreloadLayer : public LayerColor{
    public:
        static GPlayPreloadLayer* getInstance();

        static GPlayPreloadLayer* getCurrInstance();

        ~GPlayPreloadLayer();
        
        void onUpdateProgress(double progress, double downloadSpeed);

        void onPreloadSuccessed();

        void onPreloadFailed(int errorCode);
        
        virtual bool init() override;
        
    protected:
        GPlayPreloadLayer();

        ProgressTimer* _progressBar;
        Label* _percentLabel;
        Label* _hintLabel;

        int _currentProgress;

        EventListenerKeyboard* _keyboardListener;
        EventListenerTouchOneByOne* _touchListener;
    };
}
#endif //# COCOS2D_VERSION >= 0x00030000
#endif /** __GPLAY_PRELOAD_LAYER_H__ */
