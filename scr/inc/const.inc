<?
//定義
define("CONST_INC", "1");

//ドキュメントルート
$AryTmp = explode(DIRECTORY_SEPARATOR,__FILE__);
$AryTmp = array_slice($AryTmp,0,count($AryTmp)-2);
define(	"ROOT_DIR",join("/",$AryTmp)."/");

define(	"ROOT_INC",			ROOT_DIR."inc/" );			//インクルードパス
define( "LOG_DIR",			ROOT_DIR."log/" );			//ログ書き出し先
define(	"WEATHER_DIR",	ROOT_DIR."weather/" );	//天気設定ファイルパス
define(	"DATA_DIR",			ROOT_DIR."data/" );			//データ出力先
define(	"XML_DIR",			ROOT_DIR."data_xml/" );	//XMLデータ出力先
define(	"TEMPLATE_DIR",	ROOT_DIR."tpl/" );			//テンプレート格納先

//DB関連情報定義
if(@file_exists(ROOT_INC."db.conf")){
	$lines	= @file(ROOT_INC."db.conf");
	$db			= trim($lines[0]);
}
if($db==""){
	$db = "localhost";
	if($fp = @fopen(ROOT_INC."db.conf","w")){
		fputs($fp,$db);
		fclose ($fp);
		@chmod(ROOT_INC."db.conf",0777);
	}
}

define( "DB_SERVER",	$db );						//サーバ名
define( "DB_PORT",		"5432" );					//ポート番号
define( "DB_NAME",		"iws25_nakano" );			//データベース名
define( "DB_USER",		"postgres" ); 				//ユーザID
define( "DB_PASS",		"neiwebsys" );				//パスワード

//URL
define(	"URL",			"http://192.168.11.100/" );
define(	"URL_SSL",	"http://192.168.11.100/" );

//ログ書き出しレベル
define( "REC_NOTE",		"0" );
define( "REC_SOCK",		"1" );
define( "REC_ERROR",	"5" );
define( "REC_ALERT",	"6" );
define( "REC_DEBUG",	"7" );
//管理ログインユーザ権限定義
define( "USER",					"1" );
define( "USER_OP",			"2" );
define( "USER_ADMIN",		"3" );
define( "USER_SYSADM",	"4" );
define( "USER_ROOT",		"5" );

//リリース年
define( "RELEASE_YEAR", "2019");

$debug = 0;
if(file_exists(ROOT_DIR."debug.dat")){
	$debug = 1;
}
define( "DEBUG",$debug );

//ロガー風向
$dir = array(	"静穏",
	"北北東",
	"北東",
	"東北東",
	"東",
	"東南東",
	"南東",
	"南南東",
	"南",
	"南南西",
	"南西",
	"西南西",
	"西",
	"西北西",
	"北西",
	"北北西",
	"北"
);

//ロガー風向頻度並び
$dir2 = array(	"北",
	"北北東",
	"北東",
	"東北東",
	"東",
	"東南東",
	"南東",
	"南南東",
	"南",
	"南南西",
	"南西",
	"西南西",
	"西",
	"西北西",
	"北西",
	"北北西",
	"静穏"
);

// 天気定義
$weather = array(	"",
	"快晴",
	"晴",
	"曇",
	"煙霧",
	"ちり煙霧",
	"砂塵アラシ",
	"地ふぶき",
	"霧",
	"霧雨",
	"雨",
	"雨強し",
	"にわか雨",
	"みぞれ",
	"雪",
	"にわか雪",
	"あられ",
	"ひょう",
	"雷",
	"天気不明",
	"雪強し",
	"雷強し"
);

//グラフ用色関連情報（ついでにグローバルって認識しやすいように大文字���)
$GRAPH_COLOR = array( array("R" =>		0,"G" =>	150,"B" =>	255,"id" => "b"),
	array("R" =>	255,"G" => 		0,"B" => 		0,"id" => "r"),
	array("R" =>		0,"G" => 	150,"B" => 		0,"id" => "g"),
	array("R" => 	100,"G" => 		0,"B" => 	255,"id" => "p"),
	array("R" =>	255,"G" => 	255,"B" =>		0,"id" => "y"),
	array("R" => 	255,"G" => 		0,"B" => 	255,"id" => "pi"),
	array("R" => 		0,"G" => 	255,"B" => 	255,"id" => "lb"),
	array("R" => 		0,"G" => 	255,"B" => 		0,"id" => "vg"),
	array("R" => 	150,"G" => 		0,"B" => 		0,"id" => "br"),
	array("R" => 	255,"G" => 	150,"B" =>	 50,"id" => "o")
);

//案件別情報読込み
if(!defined("SETTING_INC")) include(ROOT_INC."setting.conf");
?>
