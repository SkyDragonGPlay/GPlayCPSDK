<?php
/**
 * Author: Jason.wu
 * Date: 2016/3/4
 * Time: 15:03
 */
include_once 'HttpHelper.php';

$params = $_REQUEST;

//检查必要参数是否存在
if(!isset($params['channel_code']) || !isset($params['client_id']) || !isset($params['client_secret'])){
    echo 'params not complete';
    exit;
}
$loginCheckUrl = 'http://api.skydragon-inc.cn/user/LoginOauth';

$http = new HttpHelper();
$result = $http->post($loginCheckUrl, $params); //使用POST方式提交数据

//TODO 在这里处理游戏逻辑，在服务器注册用户信息等

$result = json_decode($result,true);
//result格式如下
/*
{
    "result": {
    "status": "ok",
        "error": "",
        "error_no": ""
    },
    "data": {
        "info": { //渠道返回原始验证数据
            "user_id": 34,
            "phone": ""
        },
        "common": {
            "channel_code": "666666",//渠道代码
            "uid": 34//用户在渠道上的唯一标识
        }
        "ext":""
    }

}*/
$result['data']['ext'] = 'xxxxxxxxxxxxxxxxxx';//需要透传到游戏客户端的数据

echo $result;
