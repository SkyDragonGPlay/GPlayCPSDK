#include "HelloWorldScene.h"
#include "gplay_cpp.h"
#include "cp_preload_layer/CPPreloadLayerV3.h"

#include "platform/android/jni/JniHelper.h"

USING_NS_CC;

using namespace std;

#define  LOG_TAG "HelloWorld"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static int testExtendJAR()
{
    int ret = -1;

    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "com/skydragon/gplay/test/TestJAR", "testExtendJAR", "()I")) {
        ret = t.env->CallStaticIntMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    } 

    return ret;
}

bool HelloWorld::init()
{
    if ( !Layer::init() )
        return false;
    
    const char* appKey = "qorfl0bjUOhKw2EK";
    const char* appSecret = "F1rBOjWdL34fjwGu";
    const char* privateKey = "FNJANZSGTVC0ScZh";
    gplay::initSDK(appKey, appSecret, privateKey);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto bg = Sprite::create("game_bg.jpg");
    bg->setPosition(Vec2(origin.x + visibleSize.width / 2 ,origin.y + visibleSize.height/2));
    addChild(bg);

    auto helloLayer = this;
    MenuItemFont::setFontSize(26);

    auto btnPreload = MenuItemFont::create("测试默认加载界面", [helloLayer](Ref* sender){
        helloLayer->showToast("加载场景1...");

        gplay::setPreloadResponseCallback(nullptr);

        std::vector<std::string> scenes;
        scenes.push_back("scene1");
        gplay::preloadGroups(scenes, [helloLayer](int result, const char* jsonResult){
            switch (result) 
            {
                case 0:
                    helloLayer->showToast("加载场景1成功");
                    break;
                default:
                    helloLayer->showToast("加载场景1失败");
                    break;
            }
        });
    });

    auto btnPreload2 = MenuItemFont::create("测试加载多个场景", [helloLayer](Ref* sender){
        helloLayer->showToast("加载多场景...");

        gplay::setPreloadResponseCallback(nullptr);

        std::vector<std::string> scenes;
        scenes.push_back("scene3");
        scenes.push_back("scene4");
        scenes.push_back("scene5");
        gplay::preloadGroups(scenes, [helloLayer](int result, const char* jsonResult){
            switch (result) 
            {
                case 0:
                    helloLayer->showToast("加载场景3,4,5成功");
                    break;
                default:
                    helloLayer->showToast("加载场景3,4,5失败");
                    break;
            }
        });
    });

    auto btnPreload3 = MenuItemFont::create("测试CP定制加载界面", [helloLayer](Ref* sender){
        helloLayer->showToast("加载场景2...");

        gplay::setPreloadResponseCallback([helloLayer](int resultCode, int errorCode, 
            const std::string& groupName, float percent, float speed){
            auto  preloadLayer = cp::GPlayPreloadLayer::getCurrInstance();
            if (resultCode != gplay::PRELOAD_RESULT_SUCCESS)
            {
                if (preloadLayer == NULL)
                {
                    preloadLayer = cp::GPlayPreloadLayer::getInstance();
                    auto scene = Director::getInstance()->getRunningScene();
                    scene->addChild(preloadLayer, 0xffffff);
                }
    
                if (resultCode == gplay::PRELOAD_RESULT_FAILED)
                    preloadLayer->onPreloadFailed(errorCode);
                else
                    preloadLayer->onUpdateProgress(percent, speed);
            }
            else
            {
                if (preloadLayer)
                    preloadLayer->onPreloadSuccessed();
            }
        });

        std::vector<std::string> scenes;
        scenes.push_back("scene2");
        gplay::preloadGroups(scenes, [helloLayer](int result, const char* jsonResult){
            switch (result) 
            {
                case 0:
                    helloLayer->showToast("加载场景2成功");
                    break;
                default:
                    helloLayer->showToast("加载场景2失败");
                    break;
            }
        });
    });

    auto btnLogin = MenuItemFont::create("用户登录",[helloLayer](Ref* sender){
        helloLayer->showToast("用户登录...");
        gplay::callAsyncFunc("login", "", [helloLayer](int result, const char* jsonResult) {
        //gplay::login([helloLayer](int result, const char* jsonResult){
            switch (result) 
            {
                case gplay::USER_LOGIN_RESULT_SUCCESS:
                    helloLayer->showToast("登录成功");
                    break;
                case gplay::USER_LOGIN_RESULT_CANCEL:
                    helloLayer->showToast("取消登录");
                    break;
                default:
                    helloLayer->showToast("登录失败");
                    break;
            }
        });
    });

    auto btnLogout = MenuItemFont::create("用户登出",[helloLayer](Ref* sender){
        helloLayer->showToast("用户登出...");
        gplay::logout([helloLayer](int result, const char* jsonResult){
            LOGE("logout result:%d jsonResult:%s",result, jsonResult);
            switch (result) 
            {
                case gplay::USER_LOGOUT_RESULT_SUCCESS:
                    helloLayer->showToast("登出成功");
                    break;
                default:
                    helloLayer->showToast("登出失败");
                    break;
            }
        });
    });

    auto btnPay = MenuItemFont::create("支付",[helloLayer](Ref* sender){
        helloLayer->onClickPayButton();
    });

    auto btnQuitGame = MenuItemFont::create("退出",[helloLayer](Ref* sender){
        helloLayer->showToast("退出游戏...");
        gplay::quitGame();
    });

    auto btnCreateShortcut = MenuItemFont::create("发送快捷方式",[helloLayer](Ref* sender){
        helloLayer->showToast("发送快捷方式到桌面...");
        gplay::createShortcut([helloLayer](int result, const char* jsonResult){
            LOGE("createShortcut result:%d jsonResult:%s",result, jsonResult);
            switch (result) 
            {
                case gplay::SHORTCUT_RESULT_SUCCESS:
                    helloLayer->showToast("发送快捷方式成功");
                    break;
                default:
                    helloLayer->showToast("发送快捷方式失败");
                    break;
            }
        });
    });

    auto btnCaptureScreen = MenuItemFont::create("截图1",[helloLayer](Ref* sender){
        helloLayer->showToast("截图并保存到/sdcard/test_cs1.jpeg...");
        gplay::callAsyncFunc("CAPTURE_SCREEN", "/mnt/sdcard/test_cs1.jpeg", [helloLayer](int result, const char* jsonResult){
            switch (result) 
            {
                case gplay::CAPTURE_SCREEN_SUCCESS:
                    helloLayer->showToast("截图成功");
                    break;
                default:
                    helloLayer->showToast("截图失败");
                    break;
            }
        });
    });

    auto btnCaptureScreen2 = MenuItemFont::create("截图2",[helloLayer](Ref* sender){
        helloLayer->showToast("截图并以50%的压缩品质保存到/sdcard/test_cs2.jpeg...");
        gplay::callAsyncFunc("CAPTURE_SCREEN", 
            "{\"FILE_NAME\":\"/mnt/sdcard/test_cs2.jpeg\",\"QUALITY\":50}", 
            [helloLayer](int result, const char* jsonResult)
            {
            switch (result) 
            {
                case gplay::CAPTURE_SCREEN_SUCCESS:
                    helloLayer->showToast("截图成功");
                    break;
                default:
                    helloLayer->showToast("截图失败");
                    break;
            }
        });
    });

    auto btnExtendJar = MenuItemFont::create("测试CP扩展JAR包",[helloLayer](Ref* sender){
        if (testExtendJAR() != -1)
            helloLayer->showToast("调用CP JAR函数成功");
        else
            helloLayer->showToast("调用CP JAR函数失败");
    });

    auto btnExtendSync = MenuItemFont::create("测试同步扩展接口",[helloLayer](Ref* sender){
        LOGE("测试同步扩展接口...");
        std::string tmp("SyncMethod返回:");
        tmp += gplay::callSyncFunc("testJson", "");
        LOGE("%s", tmp.c_str());
        helloLayer->showToast(tmp);
    });

    auto menu = Menu::create(btnLogin, btnLogout, btnPay, 
        btnPreload, btnPreload3, 
        btnPreload2, btnExtendJar,
        btnCaptureScreen, btnCaptureScreen2, btnQuitGame,
        btnCreateShortcut, btnExtendSync, nullptr);
    menu->setColor(Color3B::GREEN);
    menu->alignItemsInColumns(3, 2, 2, 3, 2, nullptr); 
    menu->setPosition(Vec2(origin.x + visibleSize.width / 2 ,origin.y + visibleSize.height/2));
    this->addChild(menu);

    return true;
}

void HelloWorld::onClickPayButton()
{
    showToast("支付...");
    gplay::GplayPayParams params;
    params.productId = "gplayid001";
    params.productName = "monthly card";
    params.productPrice = 100;
    params.productCount = 100;
    params.productDescription = "monthly";
    params.gameUserId = "userid001";
    params.gameUserName = "xxxx";
    params.serverId = "001";
    params.serverName = "server01";
    params.extraData = "{}";
    gplay::pay(params, [this](int result, const char* jsonResult){
        switch (result) 
        {
            case gplay::PAY_RESULT_SUCCESS:
                this->showToast("pay success");
                break;
            case gplay::PAY_RESULT_CANCEL:
                this->showToast("pay cancel");
                break;
            case gplay::PAY_RESULT_INVALID:
                this->showToast("pay invalid");
                break;
            case gplay::PAY_RESULT_NETWORK_ERROR:
                this->showToast("pay network error");
                break;
            case gplay::PAY_RESULT_NOW_PAYING:
                this->showToast("now paying...");
                break;
            default:
                this->showToast("pay failed");
                break;
        }
    });
}

void HelloWorld::showToast(const std::string& text)
{
    static Label* previousToast = nullptr;

    auto toastLabel = Label::createWithSystemFont(text,"Arial",20);
    if (previousToast)
    {
        auto tmp = previousToast->getPositionY() - previousToast->getContentSize().height;
        toastLabel->setPosition(Vec2(_contentSize.width / 2, MIN(_contentSize.height / 2, tmp)));
    }
    else 
        toastLabel->setPosition(Vec2(_contentSize.width / 2, _contentSize.height / 2));
    addChild(toastLabel);
    previousToast = toastLabel;

    toastLabel->runAction(Sequence::create(MoveBy::create(2.0f, Vec2(0,_contentSize.height/2)),
                                           CallFunc::create([toastLabel](){ 
                                            if(toastLabel == previousToast) previousToast = nullptr;
                                           }),
                                           RemoveSelf::create(), nullptr));
}

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}
