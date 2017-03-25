#include "CPPreloadLayerV3.h"
#if COCOS2D_VERSION >= 0x00030000

#include <android/log.h>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "gplay.h"

#define  LOG_TAG    "GPlayPreloadLayer"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace cocos2d;
using namespace cocos2d::ui;

struct PreloadTextConf
{
    std::string networkError;
    std::string networkRetryText;
    std::string networkCancelText;
    
    std::string verifyError;
    std::string verifyRetryText;
    std::string verifyCancelText;

    std::string noSpaceError;
    std::string noSpaceContinueText;
    std::string noSpaceCancelText;

    std::string unknownError;
    std::string unknownQuitText;

    std::string quitTip;
    std::string quitCancelText;
    std::string quitConfirmText;

    std::string downloadText;
};

/*
enum class LanguageType
{
    ENGLISH = 0,
    CHINESE,
    FRENCH,
    ITALIAN,
    GERMAN,
    SPANISH,
    DUTCH,
    RUSSIAN,
    KOREAN,
    JAPANESE,
    HUNGARIAN,
    PORTUGUESE,
    ARABIC,
    NORWEGIAN,
    POLISH
};
*/
static std::map<LanguageType, PreloadTextConf> s_preloadTextMap = 
{
    {
        LanguageType::ENGLISH, 
        { 
            "Bah! Download interrupted!\nPick up where you left off? ", "Of course", "No",
            "Oops! Something went wrong.\nDownload failed.", "Retry", "Cancel",
            "No more storage space! Free up some\nspace before pressing continue!", "Continue", "Cancel", 
            "Hm, something isn't right\n but we're trying to fix it.", "Quit",
            "Download incomplete, \nare you sure you want to exit?", "No, must finish!", "YES",
            "Downloading game assets, patience is a virtue :)"
        }
    },
    {
        LanguageType::CHINESE, 
        {
            "下载意外中断\n请确认网络通畅后点击继续!", "继续", "取消",
            "下载失败，请重试!", "重试", "取消",
            "存储空间满了\n请释放出一些空间后继续!", "继续", "取消",
            "遇到了未知的问题\n我们正在尝试解决当中!", "退出",
            "下载未完成，是否退出？", "取消", "确认退出",
            "正在下载游戏必要资源，请稍候…"
        }
    }
};

namespace cp {

    static LanguageType s_choiceLanguage = LanguageType::ENGLISH;

    class PreloadMessageBox;
    typedef std::function<void(PreloadMessageBox*)> MessageBoxCallback;

    class PreloadMessageBox : public Layer
    {
    public:
        static PreloadMessageBox* create(const std::string& message, 
            const std::string& btnText1, const std::string& btnText2, 
            const MessageBoxCallback& callback1, 
            const MessageBoxCallback& callback2)
        {
            auto box = new (std::nothrow) PreloadMessageBox();
            if (box)
            {
                box->initView(message, btnText1, btnText2, callback1, callback2);
                box->autorelease();
                return box;
            }

            delete box;
            return nullptr;
        }

        static PreloadMessageBox* create(const std::string& message, 
            const std::string& btnText1, 
            const MessageBoxCallback& callback1)
        {
            auto box = new (std::nothrow) PreloadMessageBox();
            if (box)
            {
                box->autorelease();
                box->initView(message, btnText1, "", callback1, [](PreloadMessageBox*){

                });
                
                return box;
            }

            delete box;
            return nullptr;
        }

        PreloadMessageBox()
        : _keyboardListener(nullptr)
        , _touchListener(nullptr)
        , buttonBG1(nullptr)
        , buttonBG2(nullptr)
        , _callback1(nullptr)
        , _callback2(nullptr)
        , _bg(nullptr)
        {
        }

        ~PreloadMessageBox()
        {
            if (_keyboardListener)
                _eventDispatcher->removeEventListener(_keyboardListener);
            if(_touchListener)
                _eventDispatcher->removeEventListener(_touchListener);
            if (_bg)
            {
                _bg->removeFromParentAndCleanup(true);
            }
        }
        

        void onEnter()
        {
            Layer::onEnter();
            if (_bg == nullptr && _parent)
            {
                _bg = LayerColor::create(Color4B(250,0,0,160));
                _parent->addChild(_bg);
            }
        }

        void initView(const std::string& message, 
            const std::string& btnText1, const std::string& btnText2,
            const MessageBoxCallback& callback1,
            const MessageBoxCallback& callback2)
        {
            auto director = Director::getInstance();
            auto visibleSize = director->getVisibleSize();
            float visibleWidth = visibleSize.width;
            float visibleHeight = visibleSize.height;
            float designWidth,designHeight;
            if (visibleWidth > visibleHeight)
            {
                designWidth = 640;
                designHeight = 360;
            }
            else
            {
                designWidth = 360;
                designHeight = 640;
            }

            setContentSize(Size(visibleWidth * 316 / designWidth, 
                visibleHeight * 186 / designHeight));
            auto bg = Sprite::create("cpres/gplay_custom_resource/new_wendang_bg.png");
            bg->setPosition(Vec2(_contentSize.width/2,_contentSize.height/2));
            float sacleCoefficient = _contentSize.height / bg->getContentSize().height;
            bg->setScale(sacleCoefficient);
            addChild(bg);

            _keyboardListener = EventListenerKeyboard::create();
            _keyboardListener->onKeyPressed = [](EventKeyboard::KeyCode, Event*) {};
            _keyboardListener->onKeyReleased = [](EventKeyboard::KeyCode, Event*) {};
            _eventDispatcher->addEventListenerWithFixedPriority(_keyboardListener, INT_MIN);

            ignoreAnchorPointForPosition(false);

            _messageText = message;
            _btnText1 = btnText1;
            _btnText2 = btnText2;

            auto fontSize = 50;

            _messageLabel = Label::createWithSystemFont(message,"serif", fontSize, 
                Size::ZERO, TextHAlignment::CENTER);
            _messageLabel->setTextColor(Color4B(74,74,74,255));
            addChild(_messageLabel);
            auto messageLabelSize = _messageLabel->getContentSize();
            if (message.find('\n') != std::string::npos)
            {
                auto tipScale = visibleHeight * 40/designHeight / messageLabelSize.height;
                _messageLabel->setScale(tipScale);
            }
            else
            {
                auto tipScale = visibleHeight * 19/designHeight / messageLabelSize.height;
                _messageLabel->setScale(tipScale);
            }
            _messageLabel->setPosition(Vec2(_contentSize.width/2, visibleHeight * 125/designHeight));

            if (btnText2.empty())
            {
                buttonBG1 = Sprite::create("cpres/gplay_custom_resource/new_progress_button3.png");
                auto buttonSize = buttonBG1->getContentSize();
                buttonBG1->setScale(sacleCoefficient);
                buttonBG1->setPosition(Vec2(_contentSize.width/2, visibleHeight * 48/designHeight));
                addChild(buttonBG1);

                _button1 = Label::createWithSystemFont(btnText1,"serif", fontSize);
                _button1->setScale(visibleHeight * 18/designHeight / _button1->getContentSize().height);
                _button1->setPosition(Vec2(_contentSize.width/2, visibleHeight * 48/designHeight));
                addChild(_button1);
            }
            else
            {
                buttonBG1 = Sprite::create("cpres/gplay_custom_resource/new_progress_button1.png");
                addChild(buttonBG1);
                buttonBG2 = Sprite::create("cpres/gplay_custom_resource/new_progress_button2.png");
                addChild(buttonBG2);
                _button1 = Label::createWithSystemFont(btnText1,"serif",fontSize);
                addChild(_button1);
                _button2 = Label::createWithSystemFont(btnText2,"serif",fontSize);
                addChild(_button2);
    
                auto buttonSize = buttonBG1->getContentSize();
                buttonBG1->setScale(sacleCoefficient);
                buttonBG1->setPosition(Vec2(_contentSize.width*0.283f, visibleHeight * 48/designHeight));
                buttonBG2->setScale(sacleCoefficient);
                buttonBG2->setPosition(Vec2(_contentSize.width*0.717f, visibleHeight * 48/designHeight));
    
                _button1->setScale(visibleHeight * 18/designHeight / _button1->getContentSize().height);
                _button1->setPosition(Vec2(_contentSize.width*0.283f, visibleHeight * 48/designHeight));
    
                _button2->setScale(visibleHeight * 18/designHeight / _button2->getContentSize().height);
                _button2->setPosition(Vec2(_contentSize.width*0.717f, visibleHeight * 48/designHeight));
                _button2->setColor(Color3B(244,191,86));
            }

            _callback1 = callback1;
            _callback2 = callback2;

            _touchListener = EventListenerTouchOneByOne::create();
            _touchListener->onTouchBegan = [](Touch*, Event*) {
                return true;
            };
            _touchListener->onTouchEnded = [&](Touch* touch, Event*) {

                Vec2 touchLocation = touch->getLocation();
                Vec2 local = buttonBG1->convertToNodeSpace(touchLocation);
                auto buttonSize = buttonBG1->getContentSize();
                if (local.x > 0 && local.y >0 && local.x < buttonSize.width && local.y < buttonSize.height)
                {
                    _callback1(this);
                }
                else if(buttonBG2)
                {
                    buttonSize = buttonBG2->getContentSize();
                    local = buttonBG2->convertToNodeSpace(touchLocation);
                    if (local.x > 0 && local.y >0 && local.x < buttonSize.width && local.y < buttonSize.height)
                        _callback2(this);
                }
            };
            _touchListener->setSwallowTouches(true);
            _eventDispatcher->addEventListenerWithFixedPriority(_touchListener, INT_MIN);

            auto foregroudListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [&](EventCustom* event){
                _messageLabel->setString("");
                _button1->setString("");
                _button2->setString("");

                _messageLabel->setString(_messageText);
                _button1->setString(_btnText1);
                _button2->setString(_btnText2);
            });
            _eventDispatcher->addEventListenerWithSceneGraphPriority(foregroudListener, this);
        }

        protected:
            EventListenerKeyboard* _keyboardListener;
            EventListenerTouchOneByOne* _touchListener;
            Sprite* buttonBG1;
            Sprite* buttonBG2;
            MessageBoxCallback _callback1;
            MessageBoxCallback _callback2;

            Label* _messageLabel;
            Label* _button1;
            Label* _button2;

            std::string _messageText;
            std::string _btnText1;
            std::string _btnText2;

            LayerColor* _bg;
    };

    static GPlayPreloadLayer* s_preloadProgressLayer = nullptr;

    GPlayPreloadLayer* GPlayPreloadLayer::getCurrInstance()
    {
        return s_preloadProgressLayer;
    }

    GPlayPreloadLayer* GPlayPreloadLayer::getInstance()
    {
        if(s_preloadProgressLayer)
            return s_preloadProgressLayer;

        auto ret = new (std::nothrow) GPlayPreloadLayer();
        if (ret && ret->init())
        {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    bool GPlayPreloadLayer::init()
    {
        if(!LayerColor::initWithColor(Color4B(250,0,0,150))) {
            return false;
        }

        auto currentLanguage = Application::getInstance()->getCurrentLanguage();
        if (currentLanguage != s_choiceLanguage && 
            s_preloadTextMap.find(currentLanguage) != s_preloadTextMap.end())
        {
            s_choiceLanguage = currentLanguage;
        }
        auto& choiceTextConf = s_preloadTextMap[s_choiceLanguage];

        //拦截touch事件
        _touchListener = EventListenerTouchOneByOne::create();
        _touchListener->onTouchBegan = [](Touch*, Event*) {
            return true;
        };
        _touchListener->setSwallowTouches(true);
        _eventDispatcher->addEventListenerWithFixedPriority(_touchListener, INT_MIN + 1);

        static bool s_showQuitConfirm = true;
        auto preloadLayer = this;
        //拦截按键事件
        _keyboardListener = EventListenerKeyboard::create();
        _keyboardListener->onKeyPressed = [](EventKeyboard::KeyCode, Event*) {};
        _keyboardListener->onKeyReleased = [preloadLayer](EventKeyboard::KeyCode, Event*) {
            if(!s_showQuitConfirm)
                return;

            s_showQuitConfirm = false;
            auto& choiceTextConf = s_preloadTextMap[s_choiceLanguage];

            auto box = PreloadMessageBox::create(choiceTextConf.quitTip, choiceTextConf.quitCancelText, choiceTextConf.quitConfirmText,
                [](PreloadMessageBox* sender1){
                    sender1->runAction(Sequence::create(DelayTime::create(0.01f), RemoveSelf::create(), nullptr));
                    s_showQuitConfirm = true;
                },
                [](PreloadMessageBox* sender2){
                    gplay::common::quitGame();
                });
            box->setPosition(Vec2(preloadLayer->getContentSize().width/2, preloadLayer->getContentSize().height/2));
            preloadLayer->addChild(box, INT_MAX);
        };
        _eventDispatcher->addEventListenerWithFixedPriority(_keyboardListener, INT_MIN + 1);

        auto director = Director::getInstance();
        auto visibleSize = director->getVisibleSize();
        auto origin = director->getVisibleOrigin();
        float visibleWidth = visibleSize.width;
        float visibleHeight = visibleSize.height;

        auto loadBg = Sprite::create("cpres/gplay_custom_resource/new_progress_bg.png");
        auto loadBgSize = loadBg->getContentSize();

        float designWidth,designHeight;
        if (visibleWidth > visibleHeight)
        {
            designWidth = 640;
            designHeight = 360;
        }
        else
        {
            designWidth = 360;
            designHeight = 640;
        }
        float sacle = visibleHeight * 86 / designHeight / loadBgSize.height;
        loadBg->setScale(sacle);
        loadBg->setPosition(Vec2(_contentSize.width/2, _contentSize.height * 75 / designHeight));
        this->addChild(loadBg);

        auto spriteProgressBar = Sprite::create("cpres/gplay_custom_resource/new_progress_bar.png");
        auto progressBarSize = spriteProgressBar->getContentSize();
        _progressBar = ProgressTimer::create(spriteProgressBar);
        _progressBar->setScale(sacle);
        _progressBar->setBarChangeRate(Vec2(1,0));
        _progressBar->setType(ProgressTimer::Type::BAR);
        _progressBar->setMidpoint(Vec2(0,1));
        _progressBar->setPercentage(0);
        this->addChild(_progressBar);

        auto fontSize = 50;

        _hintLabel = Label::createWithSystemFont(choiceTextConf.downloadText,"serif",fontSize);
        _hintLabel->setTextColor(Color4B(74,74,74,255));
        this->addChild(_hintLabel);

        _percentLabel = Label::createWithSystemFont("0%","serif",fontSize);
        _percentLabel->setTextColor(Color4B(74,74,74,255));
        this->addChild(_percentLabel);
        
        _progressBar->setPosition(Vec2(_contentSize.width/2, _contentSize.height * 75 / designHeight));
        _percentLabel->setPosition(Vec2(_contentSize.width/2, _contentSize.height * 75 / designHeight));
        _hintLabel->setPosition(Vec2(_contentSize.width/2, _contentSize.height * 92 / designHeight));

        _percentLabel->setScale(visibleHeight * 14/designHeight / _percentLabel->getContentSize().height);
        _hintLabel->setScale(visibleHeight * 14/designHeight / _hintLabel->getContentSize().height);

        auto foregroudListener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [&](EventCustom* event){
            _percentLabel->setString("");
            _hintLabel->setString("");

            _hintLabel->setString(s_preloadTextMap[s_choiceLanguage].downloadText);

            char buf_str[16];  
            sprintf(buf_str,"%d%%", _currentProgress);
            _percentLabel->setString(buf_str);
        });
        _eventDispatcher->addEventListenerWithSceneGraphPriority(foregroudListener, this);

        return true;
    }

    void GPlayPreloadLayer::onUpdateProgress(double progress, double downloadSpeed) {
        if ((int)progress != _currentProgress)
        {
            _currentProgress = (int)progress;

            char buf_str[16];  
            sprintf(buf_str,"%d%%", _currentProgress);
            _percentLabel->setString(buf_str);
            _progressBar->setPercentage(_currentProgress);
        }
    }

    void GPlayPreloadLayer::onPreloadSuccessed()
    {
        this->removeFromParentAndCleanup(true);
    }

    void GPlayPreloadLayer::onPreloadFailed(int errorCode)
    {
        auto& choiceTextConf = s_preloadTextMap[s_choiceLanguage];

        std::string errorMessage;
        std::string retryText;
        std::string quitText;

        if (errorCode == gplay::common::PRELOAD_ERROR_VERIFY_FAILED) {
            errorMessage = choiceTextConf.verifyError;
            retryText = choiceTextConf.verifyRetryText;
            quitText = choiceTextConf.verifyCancelText;
        }
        else if (errorCode == gplay::common::PRELOAD_ERROR_NETWORK) {
            errorMessage = choiceTextConf.networkError;
            retryText = choiceTextConf.networkRetryText;
            quitText = choiceTextConf.networkCancelText;
        }
        else if (errorCode == gplay::common::PRELOAD_ERROR_NO_SPACE) {
            errorMessage = choiceTextConf.noSpaceError;
            retryText = choiceTextConf.noSpaceContinueText;
            quitText = choiceTextConf.noSpaceCancelText;
        }
        else {
            errorMessage = choiceTextConf.unknownError;
            quitText = choiceTextConf.unknownQuitText;
        }

        PreloadMessageBox* preloadMessageBox;
        if (retryText.empty())
        {
            preloadMessageBox = PreloadMessageBox::create(errorMessage, quitText, [](PreloadMessageBox* sender){
                gplay::common::quitGame();
            });
        }
        else
        {
            preloadMessageBox = PreloadMessageBox::create(errorMessage, retryText, quitText,
                [](PreloadMessageBox* sender){
                    sender->runAction(Sequence::create(DelayTime::create(0.01f), RemoveSelf::create(), nullptr));

                    auto preloadLayer = GPlayPreloadLayer::getInstance();
                    auto scene = Director::getInstance()->getRunningScene();
                    scene->addChild(preloadLayer, INT_MAX);
    
                    gplay::common::retryPreload();
                },
                [](PreloadMessageBox* sender){
                    gplay::common::quitGame();
                });
        }
        
        preloadMessageBox->setPosition(Vec2(_contentSize.width/2, _contentSize.height/2));
        getParent()->addChild(preloadMessageBox, INT_MAX);

        this->removeFromParentAndCleanup(true);
    }

    GPlayPreloadLayer::GPlayPreloadLayer()
    : _progressBar(nullptr)
    , _percentLabel(nullptr)
    , _currentProgress(0)
    , _keyboardListener(nullptr)
    , _touchListener(nullptr)
    {
        s_preloadProgressLayer = this;
    }

    GPlayPreloadLayer::~GPlayPreloadLayer() {
        s_preloadProgressLayer = nullptr;
        if(_keyboardListener)
            _eventDispatcher->removeEventListener(_keyboardListener);
        if(_touchListener)
            _eventDispatcher->removeEventListener(_touchListener);
    }
}

#endif
