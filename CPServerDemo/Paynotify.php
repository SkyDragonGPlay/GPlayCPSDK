<?php
/**
 * cp支付通知回调地址Demo
 * Author: Jason.wu
 * Date: 2016/3/4
 * Time: 15:22
 */
error_reporting(0);//关闭错误输出

$params = $_POST;

//验证签名
$sign = $params['sign'];
unset($params['sign']);
$privateKey = 'xxxxx';//Gplay SDK分配给CP的privateKey
ksort($params);//升序排列
$string = '';
foreach($params as $value){
    $string.=$value;
}//按顺序不加任何连接符将参数值拼接在一起

//第一次md5并转换成小写
$theFirstMd5String = strtolower(md5($string));
//追加privatekey
$addPrivateKeyString = $theFirstMd5String . $privateKey;
//第二次md5并转换成小写 生成最终签名
$theLastMd5String = strtolower(md5($addPrivateKeyString));

//验证签名
if($sign == $theLastMd5String){
    //TODO cp服务器逻辑，处理订单，发放道具等
    echo 'ok';//输出响应成功标识
}else{
    echo 'fail';//输出失败标识
}