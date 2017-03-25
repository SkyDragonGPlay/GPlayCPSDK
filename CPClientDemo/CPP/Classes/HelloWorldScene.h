#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include <string>
#include "cocos2d.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    CREATE_FUNC(HelloWorld);

    virtual bool init() override;
    
    void onClickPayButton();

    void showToast(const std::string& text);
};

#endif // __HELLOWORLD_SCENE_H__
