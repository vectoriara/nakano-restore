<?
//============================================================//
//	ベースロガークラス																				//
//------------------------------------------------------------//
//	処理概要																									//
//	　ロガー基本処理																					//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/11/11	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class BaseLogger
{
	//変数宣言
	var $log;

	var $setting	= array();						//ロガー情報格納用配列
	var $load_flg = 0;									//ロガー情報読込みフラグ
	var $AryColData;										//データ補填時用通常レコード
	var $AryColSumData;									//データ補填時用集計レコード
	var $fp;														//ソケット通信用ファイルポインタ
	var $LastCloseTime			= 0;				//ソケットクローズ最終日付
	var $SendCmdCnt		 			= array();	//連続問合せ時用コマンドリスト
	var $execute_collection	= 0;				//補填動作フラグ
	var $weather_out				= 0;				//ロガー天気出力可否
	var $is_edit_weather = true;	// 天気編集機能フラグ
	var $is_edit_warning = true;	// 気象庁警報編集機能フラグ

	public $isEditDaily = false;

	//月報レコード作成時参照先レコード（ロガー取得機能がない場合用）
	var	$AryFld300 = array(
		"avg_ws"					=> array("avg_ws",				"40"),
		"avg_wd"					=> array("avg_wd",				"40"),
		"max_ws"					=> array("max_ws",				"50"),
		"max_wd"					=> array("max_wd",				"50"),
		"avg_temp" 				=> array("temp",					"40"),
		"max_temp"				=> array("temp",					"50"),
		"min_temp" 				=> array("temp",					"60"),
		"avg_humid1"			=> array("humid1",				"40"),
		"max_humid1"			=> array("humid1",				"50"),
		"min_humid1"			=> array("humid1",				"60"),
		"humid2"					=> array("humid2",				"40"),
		"press1"					=> array("press1",				"40"),
		"avg_press1"			=> array("press1",				"40"),
		"max_press1"			=> array("press1",				"50"),
		"min_press1"			=> array("press1",				"60"),
		"press2"					=> array("press2",				"40"),
		"avg_press2"			=> array("press2",				"40"),
		"max_press2"			=> array("press2",				"50"),
		"min_press2"			=> array("press2",				"60"),
		"max_rain_10min"	=> array("max_rain_10min","50"),
		"max_rain_hour"		=> array("rain_hour",			"50"),
		"rain_day"				=> array("rain_day",			"70"),
		"rain_fall"				=> array("rain_fall",			"50"),
		"snow_day"				=> array("move_snow_hour","70"),
		"avg_water_day" 	=> array("water_10min",		"40"),
		"max_water_day"		=> array("water_10min",		"50"),
		"min_water_day"		=> array("water_10min",		"60"),
		"snow_level"			=> array("snow_level",		"50"),
		"weather1"				=> array(),
		"weather2"				=> array(),
		"weather3"				=> array(),
		"ins_day"				=> array("ins_day","70"),
		"sun_day"				=> array("sun_day","70")
	);

	//年報レコード作成時参照先レコード（ロガー取得機能がない場合用）
	var	$AryFld400 = array(
		"avg_ws"					=> array("avg_ws",				"40"),
		"avg_wd"					=> array("avg_wd",				"40"),
		"max_ws"					=> array("max_ws",				"50"),
		"max_wd"					=> array("max_wd",				"50"),
		"avg_temp"				=> array("avg_temp",			"40"),
		"max_temp"				=> array("max_temp",			"50"),
		"min_temp"				=> array("min_temp",			"60"),
		"avg_humid1"			=> array("avg_humid1",		"40"),
		"max_humid1"			=> array("max_humid1",		"50"),
		"min_humid1"			=> array("min_humid1",		"60"),
		"humid2"					=> array("humid2",				"40"),
		"avg_humid2"			=> array("humid2",				"40"),
		"max_humid2"			=> array("humid2",				"50"),
		"min_humid2"			=> array("humid2",				"60"),
		"press1"					=> array("press1",				"40"),
		"avg_press1"			=> array("press1",				"40"),
		"max_press1"			=> array("press1",				"50"),
		"min_press1"			=> array("press1",				"60"),
		"press2"					=> array("press2",				"40"),
		"avg_press2"			=> array("press2",				"40"),
		"max_press2"			=> array("press2",				"50"),
		"min_press2"			=> array("press2",				"60"),
		"max_rain_10min"	=> array("max_rain_10min","50"),
		"max_rain_hour"		=> array("max_rain_hour",	"50"),
		"max_rain_day"		=> array("rain_day",			"50"),
		"rain_fall"				=> array("rain_fall",			"50"),
		"rain_month"			=> array("rain_day",			"70"),
		"snow_month"			=> array("snow_day",			"70"),
		"avg_water_month"	=> array("avg_water_day",	"40"),
		"max_water_month"	=> array("max_water_day",	"50"),
		"min_water_month"	=> array("min_water_day",	"60"),
		"snow_level"			=> array("snow_level",		"50"),
		"ins_month"				=> array("ins_day","70"),
		"sun_month"				=> array("sun_day","70")
	);

	//コンストラクタ
	function __construct(&$log = ""){
		if(is_object($log))	$this->log = $log;
	}

	//局のロガー設定を取得	[2006/12/29]
	function LoadLoggerInfo($pnt_id){
		$args 	= array();
		$strSql = "SELECT * FROM t_lgrdat WHERE lgr_id = '".$this->id."' AND lgr_pnt_id = '$pnt_id';";
		$dbs 		= new CONN;
		if(!$row=$dbs->Query($strSql)){
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		foreach($this->map as $key => $val){
			$this->setting[$key] = $row[$val];
		}
		$this->setting["lgr_enable_value"]	= ($this->setting["lgr_enable"]	? "checked" : "");
		$this->setting["lgr_time_value"] 		= ($this->setting["lgr_time"]		? "checked" : "");

		$this->load_flg = 1;
		return $this->setting;
	}

	//局のロガー設定を保存	[2006/12/29]
	function SaveLoggerInfo($args){
		$AryTmp = array();
		foreach($this->map as $key => $val){
			if($val=="lgr_pnt_id" || $val=="lgr_id") continue;
			if($val=="lgr_enable" || $val=="lgr_time"){
				$AryTmp[] = "$val = ".($args[$key]=="" ? 0 : 1)."";
			}else{
				$AryTmp[] = "$val = '".$args[$key]."'";
			}
		}
		$strSql = "UPDATE t_lgrdat SET ".join(",",$AryTmp)." WHERE lgr_id = '".$args["lgr_id"]."' AND lgr_pnt_id = '".$args["pnt_id"]."';";
		$dbs		= new CONN;
		if($dbs->Execute($strSql)){
			if(is_object($this->log))	$this->log->logPrint($strSql,REC_NOTE,__LINE__,__FILE__);
			$this->load_flg = 0;
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	//データ取得処理	[2006/12/29]
	function GetData($pnt_id,$sys_no,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		switch($sys_no){
			case "000":
				$function = "GetPresentData";
				break;
			case "100":
				$function = "GetMinutelyData";
				break;
			case "200":
				$function = "GetDailyData";
				break;
			case "300":
				$function = "GetMonthlyData";
				break;
			case "400":
				$function = "GetYearlyData";
				break;
			default:
				return false;
				break;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." {$function} Start",REC_DEBUG,__LINE__,__FILE__);
		if($ret=$this->$function($pnt_id,$curTime,$args)){
			if(is_object($this->log))	$this->log->logPrint($this->id." {$function} Succeeded",REC_DEBUG,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint($this->id." {$function} Failed",REC_NOTE,__LINE__,__FILE__);
		}
		return $ret;
	}

	//集計データ取得処理		[2006/12/29]
	function GetSumData($pnt_id,$sys_no,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		switch($sys_no){
			case "200":
				$function = "GetDailySumData";
				break;
			case "300":
				$function = "GetMonthlySumData";
				break;
			case "400":
				$function = "GetYearlySumData";
				break;
			default:
				return false;
				break;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." {$function} Start",REC_DEBUG,__LINE__,__FILE__);
		if($ret=$this->$function($pnt_id,$curTime,$args)){
			if(is_object($this->log))	$this->log->logPrint($this->id." {$function} Succeeded",REC_DEBUG,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint($this->id." {$function} Failed",REC_NOTE,__LINE__,__FILE__);
		}
		return $ret;
	}

	//現在データ取得処理(ダミーデータ出力用）	[2006/12/29]
	function GetPresentData($pnt_id,$curTime,&$args){
		$AryDmy = array();
		$AryDmy[] = array("avg_ws"=>"00.0","avg_wd"=>"00","max_ws" =>"00.6","max_wd"=>"00","temp"=>"15","humid1"=>"11.3","humid2"=>"73.0","press1"=>"1010.1","press2"=>"1016.2","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"1.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"01.0","avg_wd"=>"01","max_ws" =>"01.6","max_wd"=>"01","temp"=>"15.5","humid1"=>"21.5","humid2"=>"80.0","press1"=>"1011.1","press2"=>"1017.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.5");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.2","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.4","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryDmy[] = array("avg_ws"=>"02.0","avg_wd"=>"06","max_ws" =>"02.7","max_wd"=>"08","temp"=>"10.5","humid1"=>"30.0","humid2"=>"65.0","press1"=>"1022.1","press2"=>"998.5","rain_1min"=>"0.0");
		$AryTmp = $AryDmy[ rand(0,count($AryDmy)-1) ];

		if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp["avg_ws"])			? number_format($AryTmp["avg_ws"],	1, ".", "")  : "");
		if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp["avg_wd"])			? $AryTmp["avg_wd"] : "");
		if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	(is_numeric($AryTmp["max_ws"])			? number_format($AryTmp["max_ws"],	1, ".", "")  : "");
		if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	(is_numeric($AryTmp["max_wd"])			? $AryTmp["max_wd"] : "");
		if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp["temp"]) 				? number_format($AryTmp["temp"],		1, ".", "")  : "");
		if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp["humid1"]) 			? number_format($AryTmp["humid1"], 	1, ".", "")  : "");
		if(array_key_exists("humid2",$args))			$args["humid2"]			=	(is_numeric($AryTmp["humid2"]) 			? number_format($AryTmp["humid2"],	1, ".", "")  : "");
		if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp["press1"]) 			? number_format($AryTmp["press1"],	1, ".", "")  : "");
		if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp["press2"]) 			? number_format($AryTmp["press2"],	1, ".", "")  : "");
		if(array_key_exists("rain_1min",$args))		$args["rain_1min"]	=	(is_numeric($AryTmp["rain_1min"]) 	? number_format($AryTmp["rain_1min"],1, ".", "")  : "");
		//積算データ取得
		$rain = $this->GetRain($pnt_id,$curTime,$AryTmp["rain_1min"]);
		if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($rain["rain_10min"]) 		? number_format($rain["rain_10min"],1, ".", "")  : "");
		if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($rain["rain_hour"]) 		? number_format($rain["rain_hour"],	1, ".", "")  : "");
		if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	(is_numeric($rain["rain_day"]) 			? number_format($rain["rain_day"],	1, ".", "")  : "");
		//移動積算取得
		$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);
		if(array_key_exists("rain_1min",$args))				$args["rain_1min"]				= $AryMove["rain_1min"];
		if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	= $AryMove["move_rain_10min"];
		if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		= $AryMove["move_rain_hour"];
		if(array_key_exists("move_rain_day",$args))		$args["move_rain_day"]		= $AryMove["move_rain_day"];
		//累計雨量算出＆記録処理
		if(array_key_exists("rain_rui",$args))	$args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
		//天気取得
		if(array_key_exists("weather",$args))		$args["weather"]	= $this->GetWeather($pnt_id);

		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//時報データ取得処理	[2007/01/31]
	function GetMinutelyData($pnt_id,$curTime,&$args){
		$date 		= DateConvert($curTime);
		$trg			= $date["date"];
		$data			= new Data;
		$AryData	= $data->LoadData($pnt_id,"000",$curTime);

		$result = false;
		if(!is_array($AryData[$trg])){
			//データが取得できなかった場合は前10分前までの最新データ取得
			for($i=1;$i<10;$i++){
				$date 		= DateConvert($curTime- $i * 60);
				$trg			= $date["date"];
				$AryData	= $data->LoadData($pnt_id,"000",$curTime- $i * 60);
				if(is_object($this->log))	$this->log->logPrint("====GetMinutelyData==== Retry Get Data => {$trg}",REC_DEBUG,__LINE__,__FILE__);
				if(is_array($AryData[$trg])){
					$result = true;
					break;
				}
			}
		}else{
			$result = true;
		}
		foreach($args as $key => $val){
			$args[$key] =	$AryData[$trg][$key];
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $result;
	}

	//時報データ手動収集（取得データを連想配列に格納する	[2006/12/31]
	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		$this->execute_collection = 1;	//手動収集フラグON

		//格納用連想配列初期化
		$this->AryColData = array();

		if($trg2!=""){
			$time1 = DateConvert3($trg1);
			$time2 = DateConvert3($trg2);
		}else{
			$time1 = DateConvert3($trg1);
			$time2 = $time1;
		}

		$data		= new Data;
		$AryRec	= $data->GetDataField("100");
		while($time1 <= $time2){
			$date = DateConvert($time1);
			$args = $AryRec;
			if($this->GetMinutelyData($pnt_id,$time1,$args)) $this->AryColData[ $date["date"] ] = $args;
			$time1 += 600;
		}
		$this->SendCmd();
		$this->execute_collection = 0;	//手動収集フラグOFF
		return true;
	}

	//日報データ取得(共通タイプ）
	function GetDailyData($pnt_id,$curTime,&$args){
		//指定時刻 (XX-1):01 から XX:00 までの時報データを使用して日報データを作成
		$dbs = new CONN;

		//雨量データ取得
		if(in_array("max_rain_10min",$this->OutField["200"])||in_array("rain_hour",$this->OutField["200"])||in_array("rain_day",$this->OutField["200"])||in_array("rain_fall",$this->OutField["200"])){
			$rain = $this->GetDailyRain($pnt_id,$curTime);
		}

		//最大瞬間風速データ取得
		if(in_array("max_ws",$this->OutField["200"])||in_array("max_wd",$this->OutField["200"])){
			$wind = $this->GetDailyWind($pnt_id,$curTime);
		}

		$now		= DateConvert($curTime);
		$prev		= DateConvert($curTime-3600);
		$tbl		= "t100_dat_".$now["year"];
		$trg		= "t100_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["year"].$now["month"].$now["day"].$now["hour"]."00'";
		$strSql = "SELECT * FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} ORDER BY t100_date DESC LIMIT 1;";

		if(is_object($this->log))	$this->log->logPrint("****GetDailyData**** ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);

		foreach($this->OutField["200"] as $key){
			if(!array_key_exists($key,$args)) continue;

			switch($key){
				case "max_rain_10min":
				case "rain_hour":
				case "rain_day":
				case "rain_fall":
					$args[$key] = $rain[$key];
					break;
				case "max_ws":
				case "max_wd":
					$args[$key] = $wind[$key];
					break;
				case "weather":
					$args[$key]	=	$this->GetWeatherData($pnt_id,$curTime);
					break;
//					case "snow_level":
//						$args[$key]	=	$this->GetSnowData1($pnt_id,$curTime);
//						break;
//					case "snow_hour":
//						$args[$key]	=	$this->GetSnowData2($pnt_id,$curTime);
//						$this->SetSnowData2($pnt_id,$curTime,$args[$key]);
//						break;

				case "move_snow_hour":
				case "snow_level":
				case "sun_day":
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 0, ".", "");
					break;
				case "avg_wd":
					if(is_numeric($row[$key]))	$args[$key] = $row[$key];
					break;
				case "ins_day":
				case "water_10min":
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 2, ".", "");
					break;
				default:
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 1, ".", "");
			}
		}
		if(is_object($this->log))	$this->log->logPrint($this->id."****GetDailyData**** ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//日報集計データ取得	[2006/12/29]
	function GetDailySumData($pnt_id,$curTime,&$args){
		//日付設定
		$date = DateConvert($curTime);
		$trg1 = $date["year"].$date["month"].$date["day"]."0000";
		$trg2 = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];

		$dbs = new CONN;

		$ArySumFld = array(
			"avg_ws	[平均風速]平均値" 		=> array("40","avg_ws","100","avg_ws"),
			"avg_ws	[平均風速]最高値" 		=> array("50","avg_ws","100","avg_ws"),
			"avg_ws	[平均風速]最高値起時"	=> array("55","avg_ws","100","avg_ws"),
			"avg_wd	[平均風向]平均値"		=> array("40","avg_wd","100","avg_wd"),
			"avg_wd	[平均風向]最高値"		=> array("50","avg_wd","100","avg_wd"),
			"max_ws	[最大風速]最高値" 		=> array("50","max_ws","100","max_ws"),
			"max_ws	[最大風速]最高値起時" 	=> array("55","max_ws","100","max_ws"),
			"max_wd	[最大風向]最高値"		=> array("50","max_wd","100","max_wd"),

			"temp	[気温]平均" 		=> array("40","temp","100","temp"),
			"temp  	[気温]最高値" 		=> array("50","temp","100","temp"),
			"temp  	[気温]最高値起時"	=> array("55","temp","100","temp"),
			"temp  	[気温]最低値" 		=> array("60","temp","100","temp"),
			"temp  	[気温]最低値起時"	=> array("65","temp","100","temp"),

			"humid1	[相対湿度]平均" 		=> array("40","humid1","100","humid1"),
			"humid1	[相対湿度]最高値" 		=> array("50","humid1","100","humid1"),
			"humid1	[相対湿度]最高値起時" 	=> array("55","humid1","100","humid1"),
			"humid1	[相対湿度]最低値" 		=> array("60","humid1","100","humid1"),
			"humid1	[相対湿度]最低値起時"	=> array("65","humid1","100","humid1"),
			"humid2	[実効湿度]平均" 		=> array("40","humid2","100","humid2"),

			"press1	[現地気圧]平均" 		=> array("40","press1","100","press1"),
			"press1	[現地気圧]最高値" 		=> array("50","press1","100","press1"),
			"press1	[現地気圧]最高値起時" 	=> array("55","press1","100","press1"),
			"press1	[現地気圧]最低値" 		=> array("60","press1","100","press1"),
			"press1	[現地気圧]最低値起時"	=> array("65","press1","100","press1"),
			"press2	[海面気圧]平均" 		=> array("40","press2","100","press2"),
			"press2	[海面気圧]最高値" 		=> array("50","press2","100","press2"),
			"press2	[海面気圧]最高値起時" 	=> array("55","press2","100","press2"),
			"press2	[海面気圧]最低値" 		=> array("60","press2","100","press2"),
			"press2	[海面気圧]最低値起時"	=> array("65","press2",	"100","press2"),

			"max_rain_10min [10分最大]最高値"		=> array("50","rain_10min","100","max_rain_10min"),
			"max_rain_10min [10分最大]最高値起時"	=> array("55","rain_10min","100","max_rain_10min"),
			"rain_hour   	[時間積算]最高値"		=> array("50","rain_hour","200","rain_hour"),
			"rain_hour   	[時間積算]最高値起時" 	=> array("55","rain_hour","200","rain_hour"),
			"rain_day	   	[日積算]積算値"			=> array("70","rain_day","100","rain_day"),

			// 雨量特殊処理（起時対応版）
			// 既存処理と2重処理になるが、既存処理の後に処理することで特殊処理を適用する
			"(特殊処理)max_rain_10min [10分最大]最高値"	=> array("50","rain_10min_max","100","max_rain_10min"),
			"(特殊処理)move_rain_hour [時間積算]最高値"	=> array("50","move_rain_hour_max","100","rain_hour"),

			"ins_day	[日射]積算値" => array("70","ins_day","000","ins_day"),
			"sun_day	[日照]積算値" => array("70","sun_day","000","sun_day"),

			"rain_fall	[降雨強度]最高値"		=> array("50","rain_fall",	"100","rain_fall"),
			"rain_fall	[降雨強度]最高値起時"	=> array("55","rain_fall",	"100","rain_fall"),

			"snow_hour		[時間積算積雪]積算値"	=> array("70","snow_hour","200","snow_hour"),
			"snow_level		[積雪深]最高値"			=> array("50","snow_level","100","snow_level"),
			"snow_level		[積雪深]最高値起時"		=> array("55","snow_level","100","snow_level"),
			"move_snow_hour	[降雪深]最高値"			=> array("50","move_snow_hour","100","move_snow_hour"),
			"move_snow_hour	[積雪深]最高値起時"		=> array("55","move_snow_hour","100","move_snow_hour"),
			"move_snow_hour	[降雪深]積算値"			=> array("70","move_snow_10min","100","move_snow_hour"),

			"water_10min [水位]平均"		=> array("40","water_10min",		"100","water_10min"),
			"water_10min [水位]最高値"		=> array("50","water_10min",		"100","water_10min"),
			"water_10min [水位]最高値起時"	=> array("55","water_10min",		"100","water_10min"),
			"water_10min [水位]最低値"		=> array("60","water_10min",		"100","water_10min"),
			"water_10min [水位]最低値起時"	=> array("65","water_10min",		"100","water_10min"),

			"風向頻度" => array("90","avg_ws","200","avg_ws")
		);

		foreach($ArySumFld as $name => $val){
			$sum_no = $val[0];	//集計No
			$fld	= $val[1];	//問合せ用フィールド
			$sys_no	= $val[2];	//問合せ用テーブル
			$fld_id	= $val[3];	//処理フィールド

			//問合せSQL生成
			$tbl		= "t{$sys_no}_dat_".$date["year"];
			$fld_date	= "t{$sys_no}_date";
			$strWhere 	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
			$strWhere2	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code = {$sum_no}";

			//ロガー出力項目チェック
			if(!in_array($fld_id,$this->OutField["200"])) continue;
			if(!array_key_exists($fld_id,$args[$sum_no])) continue;

			//集計データ取得処理
			if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== {$name}",REC_DEBUG,__LINE__,__FILE__);
			switch($sum_no){
				case "40":
					//平均
					if(strpos(" ".$fld_id,"_wd")>0){
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' GROUP BY {$fld} ORDER BY COUNT(*) DESC LIMIT 2;";
						if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if($row[0]=="00"){
							$new = $dbs->Next();
							if($new[0]!=""){
								$row = $new;
								if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== Next Record Read [00] ==> [".$row[0]."]",REC_DEBUG,__LINE__,__FILE__);
							}
						}
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = $row[0];
					}else{
						$strSql = "SELECT AVG(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(strpos(" ".$fld_id,"water_")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}else{
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					break;
				case "50":
					//最高値
					if(strpos(" ".$fld_id,"_wd")>0){
						//最大風速時風向
						$tmp_fld_id = str_replace("_wd","_ws",$fld_id);
						if(is_numeric($args[$sum][$tmp_fld_id])){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $tmp_fld_id = '".$args[$sum][$tmp_fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
							if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = (int) $row[0];
						}
					}elseif($fld=="rain_10min_max"||$fld=="move_rain_hour_max"){	//平均風速最大値(CK4700特殊処理)
						$strSql = "SELECT * FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' ORDER BY TO_NUMBER({$fld},'9999.99') DESC,t100_date DESC LIMIT 1;";
						if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						if(is_object($this->log)) $this->log->logPrint("_____".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row[$fld])) {
							$args["50"][$fld_id] = $row[$fld];
							$args["55"][$fld_id] = $row[$fld . "_time"];
							if(strpos(" ".$fld_id,"rain")>0){
								if($args["50"][$fld_id]==0){
									if (is_object($this->log)) $this->log->logPrint("雨量データ０[{$fld_id}]", REC_DEBUG, __LINE__, __FILE__);
									$args["55"][$fld_id] = "";
								}
							}
						}
					}else{
						//最高値算出
						$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row[0])){
							if(strpos(" ".$fld_id,"_wd")>0){
								$args[$sum_no][$fld_id] = (int) $row[0];
							}else{
								if(strpos(" ".$fld_id,"water_")>0){
									$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
								}elseif(strpos(" ".$fld_id,"snow")>0){
									$args[$sum_no][$fld_id] = number_format($row[0], 0, ".", "");
								}else{
									$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
								}
							}
						}
					}
					break;
				case "60":
					//最低値
					$strSql = "SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					if(is_numeric($row[0])){
						if(strpos(" ".$fld_id,"water_")>0){
							$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}elseif(strpos(" ".$fld_id,"snow")>0){
							$args[$sum_no][$fld_id] = number_format($row[0], 0, ".", "");
						}else{
							$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					break;
				case "55":
				case "65":
					//起時
					$sum = $sum_no - 5;
					if(is_numeric($args[$sum][$fld_id])){
						if(strpos(" ".$fld_id,"rain")>0){
							if($args[$sum][$fld_id]==0){
								if(is_object($this->log)) $this->log->logPrint("雨量データ０ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
								break;
							}
						}
						$strSql = "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						$date = DateConvert2($row[$fld_date]);
						$args[$sum_no][$fld_id] = $date["hour"].":".$date["min"];

//							if($args[$sum][$fld_id] < 1){
//								//気温以外の値が０の場合は起時を空白にする
//								if(strpos(" ".$fld_id,"temp")<1 && strpos(" ".$fld_id,"water_")<1)	$args[$sum_no][$fld_id] = "";
//							}
//							//雨量データ０時は起時を空白にする
//							if(strpos(" ".$fld_id,"rain")>0){
//								if(!is_numeric($args[$sum_no][$fld_id]))	$args[$sum_no][$fld_id] = "";
//								if($args[$sum_no][$fld_id]==0)						$args[$sum_no][$fld_id] = "";
//							}
					}
					break;
				case "70":
					//積算値
					if(strpos(" ".$fld_id,"snow_hour")>0){
						$strSql = "SELECT SUM(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					}else{
						$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					}
					if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					$args[$sum_no][$fld_id] = $row[0];
					break;
				case "90":
					$args[$sum_no][$fld_id] = $this->GetDirection($pnt_id,"200",$curTime);
					break;
			}
			if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== [{$sum_no}][{$fld_id}] =>".$args[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
		}
		if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//月報データ取得	[2007/01/23]
	function GetMonthlyData($pnt_id,$curTime,&$args){
		$data		 	= new Data($this->log);
		$date			= DateConvert($curTime);
		$SumData	= $data->LoadSumData($pnt_id,"200",strtotime($date["year"]."/".$date["month"]."/".$date["day"]." 00:01"));

		if(is_object($this->log))	$this->log->logPrint("====GetMonthlyData==== 月報レコード作成 ".$date["year"]."年".$date["month"]."月".$date["day"]."日",REC_DEBUG,__LINE__,__FILE__);

		//天気取得
		$AryTmp	= $this->GetWeatherData2($pnt_id,strtotime($date["year"]."/".$date["month"]."/".$date["day"]." 00:01"));

		foreach($this->OutField["300"] as $key){
			if(!array_key_exists($key,$args)) continue;
			switch($key){
				case "weather1":
				case "weather2":
				case "weather3":
					$args[$key] = $AryTmp[$key];
					if(is_object($this->log))	$this->log->logPrint("====GetMonthlyData==== fld_id => ".sprintf("%-63s",$key)." Data => ".sprintf("%10s",$args[$key]),REC_DEBUG,__LINE__,__FILE__,0,1);
					break;
				default:
					$fld	=	$this->AryFld300[$key][0];
					$code = $this->AryFld300[$key][1];
					$args[$key] = $SumData[$code][$fld];
					if(is_object($this->log))	$this->log->logPrint("====GetMonthlyData==== fld_id => ".sprintf("%-20s",$key)."fld => ".sprintf("%-20s",$fld)." sum_no => ".sprintf("%-5s",$code)." Data => ".sprintf("%10s",$args[$key]),REC_DEBUG,__LINE__,__FILE__,0,1);
			}
		}
		return true;
	}

	//月報集計データ取得	[2006/12/29]
	function GetMonthlySumData($pnt_id,$curTime,&$args){
		//日付設定
		$date = DateConvert($curTime);
		$trg1	= $date["year"].$date["month"]."000000";
		$trg2	= $date["year"].$date["month"].$date["day"]."9999";

		$dbs = new CONN;

		$ArySumFld = array(
			"avg_ws [平均風速]平均値"											=> array("40","avg_ws",					"200","avg_ws"),
			"avg_ws [平均風速]最高値" 										=> array("50","avg_ws",					"200","avg_ws"),
			"avg_ws	[平均風速]最高値起時" 								=> array("55","avg_ws",					"200","avg_ws"),
			"avg_wd [平均風向]平均値"											=> array("40","avg_wd",					"200","avg_wd"),
			"avg_wd	[平均風向]最高値"											=> array("50","avg_wd",					"200","avg_wd"),
			"max_ws	[最大風速]最高値" 										=> array("50","max_ws",					"200","max_ws"),
			"max_ws	[最大風速]最高値起時" 								=> array("55","max_ws",					"200","max_ws"),
			"max_wd [最大風向]最高値"											=> array("50","max_wd",					"200","max_wd"),
			"avg_temp [平均気温]平均" 										=> array("40","temp",						"200","avg_temp"),
			"max_temp [最高気温]最高値"					 					=> array("50","temp",						"200","max_temp"),
			"max_temp	[最高気温]最高値起時"								=> array("55","temp",						"200","max_temp"),
			"max_temp	[最高気温]日数"											=> array("80","max_temp",				"300","max_temp"),
			"min_temp [最低気温]最低値" 									=> array("60","temp",						"200","min_temp"),
			"min_temp	[最低気温]最低値起時"								=> array("65","temp",						"200","min_temp"),
			"min_temp	[最低気温]日数"											=> array("80","min_temp",				"300","min_temp"),
			"avg_humid1 [平均相対湿度]平均" 							=> array("40","humid1",					"200","avg_humid1"),
			"max_humid1 [最高相対湿度]最高値" 						=> array("50","humid1",					"200","max_humid1"),
			"max_humid1 [最高相対湿度]最高値起時" 				=> array("55","humid1",					"200","max_humid1"),
			"min_humid1 [最低相対湿度]最低値" 						=> array("60","humid1",					"200","min_humid1"),
			"min_humid1	[最低相対湿度]最低値起時"					=> array("65","humid1",					"200","min_humid1"),
			"humid2 [実効湿度]平均" 											=> array("40","humid2",					"200","humid2"),
			"humid2 [実効湿度]最高値" 										=> array("50","humid2",					"300","humid2"),
			"humid2 [実効湿度]最高値起時"								 	=> array("55","humid2",					"300","humid2"),
			"humid2	[実効湿度]最低値" 										=> array("60","humid2",					"300","humid2"),
			"humid2 [実効湿度]最低値起時"									=> array("65","humid2",					"300","humid2"),
			"press1 [現地気圧]平均" 											=> array("40","press1",					"200","press1"),
			"press1 [現地気圧]最高値" 										=> array("50","press1",					"200","press1"),
			"press1	[現地気圧]最高値起時" 								=> array("55","press1",					"200","press1"),
			"press1	[現地気圧]最低値" 										=> array("60","press1",					"200","press1"),
			"press1	[現地気圧]最低値起時"									=> array("65","press1",					"200","press1"),
			"press2 [海面気圧]平均" 											=> array("40","press2",					"200","press2"),
			"press2	[海面気圧]最高値" 										=> array("50","press2",					"200","press2"),
			"press2	[海面気圧]最高値起時"						 			=> array("55","press2",					"200","press2"),
			"press2	[海面気圧]最低値" 										=> array("60","press2",					"200","press2"),
			"press2 [海面気圧]最低値起時"									=> array("65","press2",					"200","press2"),
			"max_rain_10min [10分最大]最高値"							=> array("50","max_rain_10min",	"200","max_rain_10min"),
			"max_rain_10min [10分最大]最高値起時"					=> array("55","max_rain_10min",	"200","max_rain_10min"),
			"max_rain_hour [時間最大]最高値"							=> array("50","rain_hour",			"200","max_rain_hour"),
			"max_rain_hour [時間最大]最高値起時"					=> array("55","rain_hour",			"200","max_rain_hour"),
			"rain_day [日積算]最高値"											=> array("50","rain_day",				"300","rain_day"),
			"rain_day [日積算]最高値起時"									=> array("55","rain_day",				"300","rain_day"),
			"rain_day [日積算]積算値"											=> array("70","rain_day",				"300","rain_day"),
			"rain_day [日積算]日数"												=> array("80","rain_day",				"300","rain_day"),

			"ins_day [日積算日射]最高値"		=> array("50","ins_day","300","ins_day"),
			"ins_day [日積算日射]最高値起時"	=> array("55","ins_day","300","ins_day"),
			"ins_day [日積算日射]積算値"		=> array("70","ins_day","300","ins_day"),

			"sun_day [日積算日照]最高値"		=> array("50","sun_day","300","sun_day"),
			"sun_day [日積算日照]最高値起時"	=> array("55","sun_day","300","sun_day"),
			"sun_day [日積算日照]積算値"		=> array("70","sun_day","300","sun_day"),

			"rain_fall   [降雨強度]最高値"								=> array("50","rain_fall",	"200","rain_fall"),
			"rain_fall   [降雨強度]最高値起時" 						=> array("55","rain_fall",	"200","rain_fall"),

			"snow_level [積雪深]最高値"										=> array("50","snow_level",			"200","snow_level"),
			"snow_level [積雪深]最高値起時"								=> array("55","snow_level",			"200","snow_level"),
			"snow_day [日積算積雪]最高値"									=> array("50","move_snow_hour",	"200","snow_day"),
			"snow_day [日積算積雪]最高値起時"							=> array("55","move_snow_hour",	"200","snow_day"),
			"snow_day [日積算積雪]積算値"									=> array("70","snow_day",				"300","snow_day"),
			"avg_water_day [日平均水位]平均" 							=> array("40","water_10min",		"200","avg_water_day"),
			"max_water_day [日最高水位]最高値"						=> array("50","water_10min",		"200","max_water_day"),
			"max_water_day [日最高水位]最高値起時"				=> array("55","water_10min",		"200","max_water_day"),
			"min_water_day [日最低水位]最低値"						=> array("60","water_10min",		"200","min_water_day"),
			"min_water_day [日最低水位]最低値起時"				=> array("65","water_10min",		"200","min_water_day"),
			"風向頻度"									 									=> array("90","avg_ws",					"200","avg_ws")
		);

		//日数算出定義の設定
		$Points	= new Points($this->log);
		$Point 	= $Points->LoadPointInfo($pnt_id);
		$cmp_value = array();
		$cmp_value["max_temp"]["value"]	=	$Point["pnt_summer"];
		$cmp_value["max_temp"]["comp"]	=	">=";
		$cmp_value["min_temp"]["value"]	=	$Point["pnt_winter"];
		$cmp_value["min_temp"]["comp"]	=	"<=";
		$cmp_value["rain_day"]["value"]	=	0;
		$cmp_value["rain_day"]["comp"]	=	">";

		$AryDate = array();
		foreach($ArySumFld as $name => $val){
			$sum_no = $val[0];	//集計No
			$fld		= $val[1];	//問合せ用フィールド
			$sys_no	= $val[2];	//問合せ用テーブル
			$fld_id	= $val[3];	//処理フィールド

			//問合せSQL生成
			$tbl				= "t{$sys_no}_dat_".$date["year"];
			$fld_date		=	"t{$sys_no}_date";
			$strWhere 	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
			$strWhere2	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code = {$sum_no}";

			//ロガー出力項目チェック
			if(!in_array($fld_id,$this->OutField["300"]))	continue;
			if(!array_key_exists($fld_id,$args[$sum_no])) continue;

			//集計データ取得処理
			if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name}",REC_DEBUG,__LINE__,__FILE__);
			switch($sum_no){
				case "40":
					//平均
					if(strpos(" ".$fld_id,"_wd")>0){
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere2} AND $fld <> '' GROUP BY {$fld} ORDER BY COUNT(*) DESC LIMIT 1;";
						$row		= $dbs->Query($strSql);
						if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						if($row[0]=="00"){
							$new = $dbs->Next();
							if($new[0]!=""){
								$row = $new;
								if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== Next Record Read [00] ==> [".$row[0]."]",REC_DEBUG,__LINE__,__FILE__);
							}
						}
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = $row[0];
					}else{
						$strSql = "SELECT AVG(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '';";
						$row		= $dbs->Query($strSql);
						if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);

						if(strpos(" ".$fld_id,"water_")>0){
							if(is_numeric($row[0])) $args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}else{
							if(is_numeric($row[0])) $args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}

					}
					break;
				case "50":
					//最高値
					if(strpos(" ".$fld_id,"_wd")>0){
						//風向最高値
						$tmp_fld_id =	str_replace("_wd","_ws",$fld_id);
						$tmp_value	= $args[$sum_no][$tmp_fld_id];
						if(is_numeric($tmp_value)){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere2} AND $tmp_fld_id = '{$tmp_value}' ORDER BY {$fld_date} DESC LIMIT 1;";
							$row		= $dbs->Query($strSql);
							if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = sprintf("%02d",$row[0]);
						}
					}else{
						if($fld_id=="humid2"||$fld_id=="rain_day"||$fld_id=="ins_day"||$fld_id=="sun_day"){
							$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						}else{
							$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '';";
						}
						$row = $dbs->Query($strSql);
						if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						if(strpos(" ".$fld_id,"water_")>0||strpos(" ".$fld_id,"ins_day")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}elseif(strpos(" ".$fld_id,"snow")>0||strpos(" ".$fld_id,"sun_day")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 0, ".", "");
						}else{
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					//最高値起時用に日付データを取得
					if(is_numeric($args[$sum_no][$fld_id])){
						if($fld_id=="humid2"||$fld_id=="rain_day"||$fld_id=="ins_day"||$fld_id=="sun_day"){
							$fldMax = $dbs->Query("SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> ''");
							$strSql = "SELECT $fld_date FROM {$tbl} WHERE {$strWhere} AND $fld <> '' AND $fld = '".$fldMax[0]."' ORDER BY $fld_date DESC LIMIT 1;";
						}else{
							$strSql = "SELECT $fld_date FROM {$tbl} WHERE {$strWhere2} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') = (SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '') ORDER BY $fld_date DESC LIMIT 1;";
						}
						$row			= $dbs->Query($strSql);
						$tmp_date = DateConvert2($row[0]);
						$AryDate[$sum_no][$fld_id] = $tmp_date["year"].$tmp_date["month"].$tmp_date["day"];
						if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} Date[{$fld_id}][{$sum_no}] =>".$AryDate[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
					}
					break;
				case "60":
					if($fld_id=="humid2"){
						$strSql = "SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					}else{
						$strSql = "SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '';";
					}
					$row = $dbs->Query($strSql);
					if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					if(is_numeric($row[0])){
						if(strpos(" ".$fld_id,"water_")>0){
							$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}elseif(strpos(" ".$fld_id,"snow")>0){
							$args[$sum_no][$fld_id] = number_format($row[0], 0, ".", "");
						}else{
							$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					//最低値起時用に日付データを取得
					if(is_numeric($args[$sum_no][$fld_id])){
						if($fld_id=="humid2"||$fld_id=="rain_day"){
							$strSql = "SELECT $fld_date FROM {$tbl} WHERE {$strWhere} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') = (SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '') ORDER BY $fld_date DESC LIMIT 1;";
						}else{
							$strSql = "SELECT $fld_date FROM {$tbl} WHERE {$strWhere2} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') = (SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '') ORDER BY $fld_date DESC LIMIT 1;";
						}
						$row			= $dbs->Query($strSql);
						$tmp_date = DateConvert2($row[0]);
						$AryDate[$sum_no][$fld_id] = $tmp_date["year"].$tmp_date["month"].$tmp_date["day"];
						if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} Date[{$fld_id}][{$sum_no}] =>".$AryDate[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
					}
					break;
				case "55":
				case "65":
					//起時
					$sum			 	= $sum_no - 5;
					$tmp_value	= $args[$sum][$fld_id];
					$tmp_date		= $AryDate[$sum][$fld_id];
					if(is_numeric($tmp_value)){
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere2} AND {$fld_date} LIKE '{$tmp_date}%';";
						$row 		= $dbs->Query($strSql);
						if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);

						$tmp_date = DateConvert2($tmp_date);
						if(!is_numeric($tmp_date["month"])) break;
						switch($fld_id){
							case "rain_day":
								$args[$sum_no][$fld_id] = $tmp_date["month"]."/".$tmp_date["day"];
								break;
							case "humid2":
								$args[$sum_no][$fld_id] = $tmp_date["month"]."/".$tmp_date["day"]." 24:00";
								break;
							default:
								$args[$sum_no][$fld_id] = $tmp_date["month"]."/".$tmp_date["day"]." ".$row[0];
						}
						//雨量データ０時は起時を空白にする
						if(strpos(" ".$fld_id,"rain")>0){
							if(!is_numeric($args[$sum][$fld_id]))	$args[$sum_no][$fld_id] = "";
							if($args[$sum][$fld_id]==0)						$args[$sum_no][$fld_id] = "";
						}
					}
					break;
				case "70":
					//積算値
					$strSql = "SELECT SUM(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					$row		= $dbs->Query($strSql);
					if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$args[$sum_no][$fld_id] = $row[0];
					break;
				case "80":
					$v = $cmp_value[$fld_id]["value"];
					$c = $cmp_value[$fld_id]["comp"];

					//日数
					//$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') {$c} {$v} LIMIT 1;";
					$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' LIMIT 1;";
					if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					if($row[0]!=""){
						$strSql = "SELECT COUNT({$fld}) FROM {$tbl} WHERE {$strWhere} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') {$c} {$v};";
						if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						$args[$sum_no][$fld_id] = $row[0];
					}
					break;
				case "90":
					$args[$sum_no][$fld_id] = $this->GetDirection($pnt_id,"300",$curTime);
					break;
			}
			if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== [{$fld_id}][{$sum_no}] =>".$args[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
		}
		if(is_object($this->log))	$this->log->logPrint("====GetMonthlySumData==== ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//年報データ取得	[2007/01/24]
	function GetYearlyData($pnt_id,$curTime,&$args){
		$data		 	= new Data($this->log);
		$date			= DateConvert($curTime);
		$SumData	= $data->LoadSumData($pnt_id,"300",strtotime($date["year"]."/".$date["month"]."/".$date["day"]." 00:01"));

		if(is_object($this->log))	$this->log->logPrint("====GetYearlyData==== 年報レコード作成 ".$date["year"]."年".$date["month"]."月",REC_DEBUG,__LINE__,__FILE__);

		foreach($this->OutField["400"] as $key){
			if(!array_key_exists($key,$args)) continue;
			$fld	=	$this->AryFld400[$key][0];
			$code = $this->AryFld400[$key][1];
			$args[$key] = $SumData[$code][$fld];
			if(is_object($this->log))	$this->log->logPrint("====GetYearlyData==== fld_id => ".sprintf("%-20s",$key)."fld => ".sprintf("%-20s",$fld)." sum_no => ".sprintf("%-5s",$code)." Data => ".sprintf("%10s",$args[$key]),REC_DEBUG,__LINE__,__FILE__,0,1);
		}
		return true;
	}

	//年報集計データ取得	[2007/01/24]
	function GetYearlySumData($pnt_id,$curTime,&$args){
		//日付設定
		$date = DateConvert($curTime);
		$trg1	= $date["year"]."00000000";
		$trg2	= $date["year"].$date["month"].$date["day"]."9999";

		$dbs = new CONN;

		$ArySumFld = array(
			"avg_ws [平均風速]平均値"									=> array("40","avg_ws",					"300","avg_ws"),
			"avg_ws [平均風速]最高値" 								=> array("50","avg_ws",					"300","avg_ws"),
			"avg_ws	[平均風速]最高値起時" 						=> array("55","avg_ws",					"300","avg_ws"),
			"avg_wd [平均風向]平均値"									=> array("40","avg_wd",					"300","avg_wd"),
			"avg_wd	[平均風向]最高値"									=> array("50","avg_wd",					"300","avg_wd"),
			"max_ws	[最大風速]最高値" 								=> array("50","max_ws",					"300","max_ws"),
			"max_ws	[最大風速]最高値起時"				 			=> array("55","max_ws",					"300","max_ws"),
			"max_wd	[最大風向]最高値"									=> array("50","max_wd",					"300","max_wd"),
			"avg_temp [平均気温]平均値"								=> array("40","avg_temp",				"300","avg_temp"),
			"max_temp [最高気温]最高値" 							=> array("50","max_temp",				"300","max_temp"),
			"max_temp	[最高気温]最高値起時"						=> array("55","max_temp",				"300","max_temp"),
			"max_temp	[最高気温]日数"									=> array("80","max_temp",				"300","max_temp"),
			"min_temp [最低気温]最低値" 							=> array("60","min_temp",				"300","min_temp"),
			"min_temp	[最低気温]最低値起時"						=> array("65","min_temp",				"300","min_temp"),
			"min_temp	[最低気温]日数"									=> array("80","min_temp",				"300","min_temp"),
			"avg_humid1 [平均相対湿度]平均値" 				=> array("40","avg_humid1",			"300","avg_humid1"),
			"max_humid1	[最高相対湿度]最高値" 				=> array("50","max_humid1",			"300","max_humid1"),
			"max_humid1	[最高相対湿度]最高値起時" 		=> array("55","max_humid1",			"300","max_humid1"),
			"min_humid1	[最低相対湿度]最低値" 				=> array("60","min_humid1",			"300","min_humid1"),
			"min_humid1	[最低相対湿度]最低値起時"			=> array("65","min_humid1",			"300","min_humid1"),
			"humid2 [平均実効湿度]平均値" 						=> array("40","humid2",					"300","humid2"),
			"humid2	[最高実効湿度]最高値" 						=> array("50","humid2",					"300","humid2"),
			"humid2	[最高実効湿度]最高値起時" 				=> array("55","humid2",					"300","humid2"),
			"humid2	[最低実効湿度]最低値" 						=> array("60","humid2",					"300","humid2"),
			"humid2	[最低実効湿度]最低値起時"					=> array("65","humid2",					"300","humid2"),
			"press1 [現地気圧]平均値" 								=> array("40","press1",					"300","press1"),
			"press1	[現地気圧]最高値" 								=> array("50","press1",					"300","press1"),
			"press1	[現地気圧]最高値起時" 						=> array("55","press1",					"300","press1"),
			"press1	[現地気圧]最低値" 								=> array("60","press1",					"300","press1"),
			"press1	[現地気圧]最低値起時"							=> array("65","press1",					"300","press1"),
			"press2 [海面気圧]平均値" 								=> array("40","press2",					"300","press2"),
			"press2	[海面気圧]最高値" 								=> array("50","press2",					"300","press2"),
			"press2	[海面気圧]最高値起時" 						=> array("55","press2",					"300","press2"),
			"press2	[海面気圧]最低値" 								=> array("60","press2",					"300","press2"),
			"press2	[海面気圧]最低値起時"							=> array("65","press2",					"300","press2"),
			"max_rain_10min [10分最大]最高値"					=> array("50","max_rain_10min",	"300","max_rain_10min"),
			"max_rain_10min [10分最大]最高値起時"			=> array("55","max_rain_10min",	"300","max_rain_10min"),
			"max_rain_hour [時間最大]最高値"					=> array("50","max_rain_hour",	"300","max_rain_hour"),
			"max_rain_hour 	[時間最大]最高値起時"			=> array("55","max_rain_hour",	"300","max_rain_hour"),
			"max_rain_day [日最大]最高値"							=> array("50","rain_day",				"300","max_rain_day"),
			"max_rain_day	[日最大]最高値起時"					=> array("55","rain_day",				"300","max_rain_day"),
			"rain_month [月積算]最高値"								=> array("50","rain_month",			"400","rain_month"),
			"rain_month	[月積算]最高値起時"						=> array("55","rain_month",			"400","rain_month"),
			"rain_month [月積算]積算値"								=> array("70","rain_month",			"400","rain_month"),
			"rain_month	[月積算]日数"									=> array("80","rain_day",				"300","rain_month"),

			"ins_day	   [月積算日射]積算値" => array("70","ins_day","400","ins_day"),
			"sun_day	   [月積算日照]積算値" => array("70","sun_day","400","sun_day"),

			"rain_fall   [降雨強度]最高値"								=> array("50","rain_fall",	"300","rain_fall"),
			"rain_fall   [降雨強度]最高値起時" 						=> array("55","rain_fall",	"300","rain_fall"),

			"snow_level [積雪深]最高値"								=> array("50","snow_level",			"300","snow_level"),
			"snow_level [積雪深]最高値起時"						=> array("55","snow_level",			"300","snow_level"),
			"snow_month [月積算積雪]最高値"						=> array("50","snow_day",				"300","snow_month"),
			"snow_month [月積算積雪]最高値起時"				=> array("55","snow_day",				"300","snow_month"),
			"snow_month [月積算積雪]積算値"						=> array("70","snow_month",			"400","snow_month"),
			"avg_water_month [月平均水位]平均" 				=> array("40","avg_water_day",	"300","avg_water_month"),
			"max_water_month [月最高水位]最高値"			=> array("50","max_water_day",	"300","max_water_month"),
			"max_water_month [月最高水位]最高値起時"	=> array("55","max_water_day",	"300","max_water_month"),
			"min_water_month [月最低水位]最低値"			=> array("60","min_water_day",	"300","min_water_month"),
			"min_water_month [月最低水位]最低値起時"	=> array("65","min_water_day",	"300","min_water_month"),
			"風向頻度"									 							=> array("90","avg_ws",					"300","avg_ws")
		);

		//日数算出定義の設定
		$Points	= new Points($this->log);
		$Point 	= $Points->LoadPointInfo($pnt_id);
		$cmp_value = array();
		$cmp_value["max_temp"]["value"]		=	$Point["pnt_summer"];
		$cmp_value["max_temp"]["comp"]		=	">=";
		$cmp_value["min_temp"]["value"]		=	$Point["pnt_winter"];
		$cmp_value["min_temp"]["comp"]		=	"<=";
		$cmp_value["rain_month"]["value"]	=	0;
		$cmp_value["rain_month"]["comp"]	=	">";

		$AryDate = array();
		foreach($ArySumFld as $name => $val){
			$sum_no = $val[0];	//集計No
			$fld		= $val[1];	//問合せ用フィールド
			$sys_no	= $val[2];	//問合せ用テーブル
			$fld_id	= $val[3];	//処理フィールド

			//問合せSQL生成
			$tbl				= "t{$sys_no}_dat_".$date["year"];
			$fld_date		=	"t{$sys_no}_date";
			$strWhere 	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
			$strWhere2	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code = {$sum_no}";

			//ロガー出力項目チェック
			if(!in_array($fld_id,$this->OutField["400"]))	continue;
			if(!array_key_exists($fld_id,$args[$sum_no])) continue;

			//集計データ取得処理
			if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name}",REC_DEBUG,__LINE__,__FILE__);
			switch($sum_no){
				case "40":
					//平均
					if(strpos(" ".$fld_id,"_wd")>0){
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere2} AND $fld <> '' GROUP BY {$fld} ORDER BY COUNT(*) DESC LIMIT 1;";
						$row		= $dbs->Query($strSql);
						if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = $row[0];
					}else{
						$strSql = "SELECT AVG(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '';";
						$row = $dbs->Query($strSql);
						if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						if(strpos(" ".$fld_id,"water_")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}else{
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					break;
				case "50":
					//最高値
					if(strpos(" ".$fld_id,"_wd")>0){
						//風向最高値
						$tmp_fld_id =	str_replace("_wd","_ws",$fld_id);
						$tmp_value	= $args[$sum_no][$tmp_fld_id];
						if(is_numeric($tmp_value)){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere2} AND $tmp_fld_id = '{$tmp_value}' ORDER BY {$fld_date} DESC LIMIT 1;";
							$row		= $dbs->Query($strSql);
							if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = sprintf("%02d",$row[0]);
						}
					}else{
						if($fld_id=="rain_month"){
							$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						}else{
							$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '';";
						}
						$row = $dbs->Query($strSql);
						if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						if(strpos(" ".$fld_id,"water_")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}elseif(strpos(" ".$fld_id,"snow")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 0, ".", "");
						}else{
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					//最高値起時用に日付データを取得
					if(is_numeric($args[$sum_no][$fld_id])){
						if($fld_id=="rain_month"){
							$strSql = "SELECT $fld_date FROM {$tbl} WHERE {$strWhere} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') = (SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '') ORDER BY $fld_date DESC LIMIT 1;";
						}else{
							$strSql = "SELECT $fld_date FROM {$tbl} WHERE {$strWhere2} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') = (SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '') ORDER BY $fld_date DESC LIMIT 1;";
						}
						if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$strSql}",REC_DEBUG,__LINE__,__FILE__);
						$row			= $dbs->Query($strSql);
						$tmp_date = DateConvert2($row[0]);
						$AryDate[$sum_no][$fld_id] = $tmp_date["year"].$tmp_date["month"];
						if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} Date[{$fld_id}][{$sum_no}] =>".$AryDate[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
					}
					break;
				case "60":
					//最低値
					$strSql = "SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '';";
					$row = $dbs->Query($strSql);
					if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					if(is_numeric($row[0])){
						if(strpos(" ".$fld_id,"water_")>0){
							$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}elseif(strpos(" ".$fld_id,"snow")>0){
							$args[$sum_no][$fld_id] = number_format($row[0], 0, ".", "");
						}else{
							$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					//最低値起時用に日付データを取得
					if(is_numeric($args[$sum_no][$fld_id])){
						$strSql = "SELECT $fld_date FROM {$tbl} WHERE {$strWhere2} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') = (SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere2} AND $fld <> '') ORDER BY $fld_date DESC LIMIT 1;";
						$row			= $dbs->Query($strSql);
						$tmp_date = DateConvert2($row[0]);
						$AryDate[$sum_no][$fld_id] = $tmp_date["year"].$tmp_date["month"];
						if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} Date[{$fld_id}][{$sum_no}] =>".$AryDate[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
					}
					break;
				case "55":
				case "65":
					//起時
					$sum			 	= $sum_no - 5;
					$tmp_value	= $args[$sum][$fld_id];
					$tmp_date		= $AryDate[$sum][$fld_id];
					if(is_numeric($tmp_value)){
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere2} AND {$fld_date} LIKE '{$tmp_date}%';";
						$row 		= $dbs->Query($strSql);
						if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);

						$tmp_date = DateConvert2($tmp_date);
						if(!is_numeric($tmp_date["month"])) break;
						switch($fld_id){
							case "rain_month":
								$args[$sum_no][$fld_id] = $tmp_date["month"];
								break;
							default:
								$args[$sum_no][$fld_id] = $row[0];
						}
						//雨量データ０時は起時を空白にする
						if(strpos(" ".$fld_id,"rain")>0){
							if(!is_numeric($args[$sum][$fld_id]))	$args[$sum_no][$fld_id] = "";
							if($args[$sum][$fld_id]==0)						$args[$sum_no][$fld_id] = "";
						}
					}
					break;
				case "70":
					//積算値
					$strSql = "SELECT SUM(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					$row		= $dbs->Query($strSql);
					if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$args[$sum_no][$fld_id] = $row[0];
					break;
				case "80":
					$v = $cmp_value[$fld_id]["value"];
					$c = $cmp_value[$fld_id]["comp"];

					//日数
					//$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') {$c} {$v} LIMIT 1;";
					$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' LIMIT 1;";
					if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					if($row[0]!=""){
						$strSql = "SELECT COUNT({$fld}) FROM {$tbl} WHERE {$strWhere} AND $fld <> '' AND TO_NUMBER($fld,'9999.99') {$c} {$v};";
						if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== {$name} ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						$args[$sum_no][$fld_id] = $row[0];
					}
					break;
				case "90":
					$args[$sum_no][$fld_id] = $this->GetDirection($pnt_id,"400",$curTime);
					break;
			}
			if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== [{$sum_no}][{$fld_id}] =>".$args[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
		}
		if(is_object($this->log))	$this->log->logPrint("====GetYearlySumData==== ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//手動収集処理 [2007/01/30]
	function GetLoggerCollection($pnt_id,$sys_no,$trg1,$trg2=""){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		switch($sys_no){
			case "100":
				$function = "GetLoggerMinutelyCollection";
				break;
			case "200":
				$function = "GetLoggerDailyCollection";
				break;
			case "300":
				$function = "GetLoggerMonthlyCollection";
				break;
			case "400":
				$function = "GetLoggerYearlyCollection";
				break;
			default:
				return false;
				break;
		}
		if($ret=$this->$function($pnt_id,$trg1,$trg2)){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetLoggerCollection[{$function}] Succeeded",REC_DEBUG,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint($this->id." GetLoggerCollection[{$function}] Failed",		REC_DEBUG,__LINE__,__FILE__);
		}
		return $ret;
	}
	//日報データ手動収集（取得データを連想配列に格納する）[2007/01/30]
	function GetLoggerDailyCollection($pnt_id,$trg1,$trg2=""){
		$this->execute_collection = 1;	//手動収集フラグON

		//連想配列初期化
		$this->AryColData			= array();
		$this->AryColSumData	= array();

		if($trg2!=""){
			$time1 = DateConvert3($trg1);
			$time2 = DateConvert3($trg2);
		}else{
			$time1 = DateConvert3($trg1);
			$time2 = $time1;
		}

		$wrk_date = DateConvert($time1-86400);
		while($time1<=$time2){
			$date = DateConvert($time1);
			if($wrk_date["day"]!=$date["day"]){
				$this->GetDailyLoggerData($pnt_id,$time1);
				$wrk_date = DateConvert($time1);
			}
			$time1 += 3600;
		}

		$this->execute_collection = 0;	//手動収集フラグOFF
		$this->SendCmd();
		return true;
	}

	//手動収集したデータ取得処理 [2007/01/31]
	function GetCollectionData($pnt_id,$sys_no,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		switch($sys_no){
			case "100":
				$function = "GetMinutelyCollectionData";
				break;
			case "200":
				$function = "GetDailyCollectionData";
				break;
			case "300":
				$function = "GetMonthlyCollectionData";
				break;
			case "400":
				$function = "GetYearlyCollectionData";
				break;
			default:
				return false;
				break;
		}
		if($ret=$this->$function($pnt_id,$curTime,$args)){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetLoggerCollection[{$function}] Succeeded",REC_DEBUG,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint($this->id." GetLoggerCollection[{$function}] Failed",		REC_DEBUG,__LINE__,__FILE__);
		}
		return $ret;
	}
	//時報データ手動収集（連想配列に格納したデータを取得) [2007/01/31]
	function GetMinutelyCollectionData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		if(is_array($this->AryColData[ $date["date"] ])){
			$AryTmp = $this->AryColData[ $date["date"] ];

			foreach($this->OutField["100"] as $key){
				if(!array_key_exists($key,$args)) continue;
				if(!array_key_exists($key,$AryTmp)) continue;

				if(strpos($key,"_time")!==false){
					$args[$key] = $AryTmp[$key];
				}else{
					if(!is_numeric($AryTmp[$key])) continue;
					if(strpos(" ".$key,"_wd")>0){
						$args[$key] = sprintf("%02d",$AryTmp[$key]);
					}else{
						switch($key){
							case "move_snow_10min":
							case "snow_level":
							case "move_snow_hour":
							case "sun_day":
								$args[$key] = number_format($AryTmp[$key],0,".","");
								break;
							case "ins_10min":
							case "ins_day":
								$args[$key] = number_format($AryTmp[$key],2,".","");
								break;
							default:
								$args[$key] = number_format($AryTmp[$key],1,".","");
						}
					}
				}
			}
			return true;
		}else{
			if(is_object($this->log)) $this->log->logPrint($this->id." GetMinutelyCollectionData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}
	//日報データ手動収集 [2007/01/30]
	function GetDailyCollectionData($pnt_id,$curTime,&$args){
		return $this->GetDailyData($pnt_id,$curTime,$args);
	}

	//手動収集した集計データデータ取得処理 [2007/01/30]
	function GetCollectionSumData($pnt_id,$sys_no,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		switch($sys_no){
			case "200":
				$function = "GetDailyCollectionSumData";
				break;
			case "300":
				$function = "GetMonthlyCollectionSumData";
				break;
			case "400":
				$function = "GetYearlyCollectionSumData";
				break;
			default:
				return false;
				break;
		}
		if($ret=$this->$function($pnt_id,$curTime,$args)){
			if(is_object($this->log))	$this->log->logPrint($this->id." {$function} Succeeded",REC_DEBUG,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint($this->id." {$function} Failed",REC_DEBUG,__LINE__,__FILE__);
		}
		return $ret;
	}
	//日報集計データ手動収集 [2007/01/30]
	function GetDailyCollectionSumData($pnt_id,$curTime,&$args){
		return $this->GetDailySumData($pnt_id,$curTime,$args);
	}


	/////ダミーメソッド/////
	function CheckConnection($pnt_id){
		$this->LoadLoggerInfo($pnt_id);
		return "通信チェック未実装";
	}

	function SetDate($pnt_id){
		$this->LoadLoggerInfo($pnt_id);
		return "時刻補正未実装";
	}


	//移動積算算出処理	[2006/12/29]
	function GetMoveRain($pnt_id,$curTime,$args){
		//日積算を元に一分積算を求め、それを元に10分、1時間、24時間の移動積算をもとめる
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 移動積算算出処理",REC_DEBUG,__LINE__,__FILE__);

		//日積算を取得
		$rain_day_now = $args["rain_day"];
		//日積算が数値ではない場合データが異常なので終了
		if(!is_numeric($rain_day_now)){
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ABEND [".print_r($args,TRUE)."]",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 当日日積算 => {$rain_day_now}",REC_DEBUG,__LINE__,__FILE__);

		$dbs	= new CONN;
		$data	= array();
		$now	= DateConvert($curTime);
		//前日の最終日積算を取得
		$prev		= DateConvert($curTime-86400);
		$tbl		= "t000_dat_".$prev["year"];
		$trg		= "t000_date = '".$prev["year"].$prev["month"].$prev["day"]."2400'";
		$strSql = "SELECT rain_day FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		if($row=$dbs->Query($strSql))	$rain_day_last = $row[0];
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 前日日積算 => {$rain_day_last}",REC_DEBUG,__LINE__,__FILE__);

		//1分積算算出
		$prev		= DateConvert($curTime-60);
		$tbl		= "t000_dat_".$prev["year"];
		$trg		= "t000_date = '".$prev["date"]."'";
		$strSql = "SELECT rain_day FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);

		$row = $dbs->Query($strSql);
		if($now["day"]==$prev["day"]){
			if(is_numeric($row[0])){
				if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 当日日積算 - 当日日積算（１分前）",	REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$rain_day_now." - ".$row[0],			REC_DEBUG,__LINE__,__FILE__);
				$rain_1min = $rain_day_now - $row[0];
				if($rain_1min<0){
					if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| マイナス値検出[{$rain_1min}]　データ補正[0.0]",REC_DEBUG,__LINE__,__FILE__);
					$rain_1min = 0;
				}
				$data["rain_1min"] = number_format($rain_1min,1, ".", "");
			}else{
				//１分前データが取得できない時
				if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 当日日積算（１分前）なし",REC_DEBUG,__LINE__,__FILE__);
				$data["rain_1min"] = "0.0";
			}
		}else{
			//日付が変わった場合、日積算を使用する
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 日付変更時",REC_DEBUG,__LINE__,__FILE__);
			$data["rain_1min"] = number_format($rain_day_now,1, ".", "");
		}
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 1分積算 => ".$data["rain_1min"],REC_DEBUG,__LINE__,__FILE__);

		//10分移動積算算出
		$data["move_rain_10min"] = $data["rain_1min"];
		$prev	= DateConvert($curTime-540);
		if($now["year"]==$prev["year"]){
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_rain_10min"] = number_format($row[0] + $data["move_rain_10min"],1, ".", "");
		}else{
			//前年データ取得
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$prev["year"]."12312400'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_rain_10min"] = number_format($row[0] + $data["move_rain_10min"],1, ".", "");
			//当年データ取得
			$tbl		= "t000_dat_".$now["year"];
			$trg		= "t000_date BETWEEN '".$now["year"]."01010001' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			$tmp = 0;
			if(is_numeric($row[0]))	$data["move_rain_10min"] = number_format($row[0] + $data["move_rain_10min"],1, ".", "");
		}
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 10分移動 => ".$data["move_rain_10min"],REC_DEBUG,__LINE__,__FILE__);

		//1時間移動積算算出
		$data["move_rain_hour"] = $data["rain_1min"];
		$prev	= DateConvert($curTime-3540);
		if($now["year"]==$prev["year"]){
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_rain_hour"] = number_format($row[0] + $data["move_rain_hour"],1, ".", "");
		}else{
			//前年データ取得
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$prev["year"]."12312400'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_rain_hour"] = number_format($row[0] + $data["move_rain_hour"],1, ".", "");
			//当年データ取得
			$tbl		= "t000_dat_".$now["year"];
			$trg		= "t000_date BETWEEN '".$now["year"]."01010001' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			$tmp = 0;
			if(is_numeric($row[0]))	$data["move_rain_hour"] = number_format($row[0] + $data["move_rain_hour"],1, ".", "");
		}
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 1時間移動 => ".$data["move_rain_hour"],REC_DEBUG,__LINE__,__FILE__);

		//24時間移動積算算出
		$data["move_rain_day"] = $data["rain_1min"];
		$prev	= DateConvert($curTime-86340);
		if($now["year"]==$prev["year"]){
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_rain_day"] = number_format($row[0] + $data["move_rain_day"],1, ".", "");
		}else{
			//前年データ取得
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$prev["year"]."12312400'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_rain_day"] = number_format($row[0] + $data["move_rain_day"],1, ".", "");
			//当年データ取得
			$tbl		= "t000_dat_".$now["year"];
			$trg		= "t000_date BETWEEN '".$now["year"]."01010001' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			$tmp = 0;
			if(is_numeric($row[0]))	$data["move_rain_day"] = number_format($row[0] + $data["move_rain_day"],1, ".", "");
		}
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 24時間移動 => ".$data["move_rain_day"],REC_DEBUG,__LINE__,__FILE__);

		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| ".print_r($data,TRUE),REC_DEBUG,__LINE__,__FILE__);
		if(is_object($this->log))	$this->log->logPrint("||||GetMoveRain|||| 終了",								REC_DEBUG,__LINE__,__FILE__);
		return $data;
	}

	//積算雨量データ算出	[2006/12/29]
	function GetRain($pnt_id,$curTime,$rain_1min){
		//現在テーブルの1分データを使用して10分、時間、日積算を算出する

		$dbs	= new CONN;
		$data	= array();
		$now	= DateConvert($curTime);

		//正10分以外の場合、処理時刻を調整する
		if(substr($now["min"],-1)!="0"){
			if(is_object($this->log))	$this->log->logPrint("####GetRain#### AJUST => ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);
			$curTime += 600 - (substr($now["min"],-1) * 60);
			$now	= DateConvert($curTime);
			if(is_object($this->log))	$this->log->logPrint("####GetRain#### AJUST => ".date("Y/m/d H:i",$curTime),REC_DEBUG,__LINE__,__FILE__);
		}else{
			//正10分時1分データが0以上だったら01分~09分までの問い合わせにする
			if($rain_1min>0) $now = DateConvert($curTime-60);
		}
		//10分積算算出
		$prev		= DateConvert($curTime-540);
		$tbl		= "t000_dat_".$prev["year"];
		$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$now["date"]."'";
		$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0 AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("####GetRain#### ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$data["rain_10min"] =	number_format($rain_1min,1, ".", "");	//正常に取得できなかった場合(1分積算を使用)
		if($row=$dbs->Query($strSql)){
			if(is_numeric($row[0])){
				$data["rain_10min"] =	number_format($row[0]+$data["rain_10min"],1, ".", "");
			}
		}
		//1時間積算算出
		$prev		= DateConvert($curTime-60);
		$tbl		= "t000_dat_".$prev["year"];
		$trg		= "t000_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["date"]."'";
		$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0 AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("####GetRain#### ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$data["rain_hour"] =	number_format($rain_1min,1, ".", "");	//正常に取得できなかった場合(1分積算を使用)
		if($row=$dbs->Query($strSql)){
			if(is_numeric($row[0])){
				$data["rain_hour"] =	number_format($row[0]+$data["rain_hour"],1, ".", "");
			}
		}
		//日積算算出
		$prev		= DateConvert($curTime-60);
		$tbl		= "t000_dat_".$prev["year"];
		$trg		= "t000_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"]."0001' AND '".$now["date"]."'";
		$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND rain_1min <> '' AND TO_NUMBER(rain_1min,'9999.99') > 0 AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("####GetRain#### ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$data["rain_day"] =	number_format($rain_1min,1, ".", "");	//正常に取得できなかった場合(1分積算を使用)
		if($row=$dbs->Query($strSql)){
			if(is_numeric($row[0])){
				$data["rain_day"] =	number_format($row[0]+$data["rain_day"],1, ".", "");
			}
		}
		if(is_object($this->log))	$this->log->logPrint("####GetRain#### ".print_r($data,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $data;
	}

	//日報用最大瞬間風速データ取得
	function GetDailyWind($pnt_id,$curTime){
		//指定時刻 (XX-1):01 から XX:00 までの最大瞬間風速、最大瞬間風速時風向を取得する
		$dbs	= new CONN;
		$data	= array();

		$now		= DateConvert($curTime);
		$prev		= DateConvert($curTime-3600);
		$tbl		= "t100_dat_".$now["year"];
		$trg		= "t100_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["year"].$now["month"].$now["day"].$now["hour"]."00'";
		$strSql = "SELECT TO_NUMBER(max_ws,'9999.9') as max_ws,max_wd FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} ORDER BY max_ws DESC,t100_date DESC LIMIT 1;";
		if(is_object($this->log))	$this->log->logPrint("////GetDailyWind//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row=$dbs->Query($strSql);
		$data["max_ws"] = $row["max_ws"];
		$data["max_wd"] = $row["max_wd"];

		if(is_object($this->log))	$this->log->logPrint("////GetDailyWind//// ".print_r($data,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $data;
	}

	//日報用雨量データ取得	[2006/12/31]
	function GetDailyRain($pnt_id,$curTime){
		//指定時刻 (XX-1):01 から XX:00 までの10分最大、時間積算、日積算を取得する

		$dbs	= new CONN;
		$data	= array();

		$now		= DateConvert($curTime);
		$prev		= DateConvert($curTime-3600);
		$tbl		= "t100_dat_".$now["year"];
		$trg		= "t100_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["year"].$now["month"].$now["day"].$now["hour"]."00'";
		$strSql = "SELECT * FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} ORDER BY t100_date DESC LIMIT 1;";

		//10分最大雨量
		$strSql = "SELECT MAX(TO_NUMBER(rain_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND rain_10min <> '' AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("++++GetDailyRain++++ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		if($row=$dbs->Query($strSql)){
			if(is_numeric($row[0])){
				$row[0] += $rain_1min;
				$data["max_rain_10min"] =	number_format($row[0],1, ".", "");
			}
		}

		//1時間積算雨量
		$strSql = "SELECT SUM(TO_NUMBER(rain_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND rain_10min <> '' AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("++++GetDailyRain++++ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		if($row=$dbs->Query($strSql)){
			if(is_numeric($row[0])){
				$row[0] += $rain_1min;
				$data["rain_hour"] =	number_format($row[0],1, ".", "");
			}
		}

		//日積算雨量
		$strSql = "SELECT MAX(TO_NUMBER(rain_day,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND rain_day <> '' AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("++++GetDailyRain++++ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		if($row=$dbs->Query($strSql)){
			if(is_numeric($row[0])){
				$row[0] += $rain_1min;
				$data["rain_day"] =	number_format($row[0],1, ".", "");
			}
		}
		//降雨強度
		$strSql = "SELECT MAX(TO_NUMBER(rain_fall,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND rain_fall <> '' AND {$trg};";
		if(is_object($this->log))	$this->log->logPrint("++++GetDailyRain++++ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		if($row=$dbs->Query($strSql)){
			if(is_numeric($row[0])){
				$data["rain_fall"] =	number_format($row[0],1, ".", "");
			}
		}
		if(is_object($this->log))	$this->log->logPrint("++++GetDailyRain++++ ".print_r($data,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $data;
	}

	//天気情報をファイルに設定	[2006/12/21]
	function SetWeather($pnt_id,$no){
		@unlink(WEATHER_DIR.$pnt_id);
		$fp = @fopen(WEATHER_DIR.$pnt_id, "a");
		if($fp){
			if(is_numeric($no)){
				@fwrite($fp,sprintf("%02d",$no));
			}else{
				@fclose($fp);
				return false;
			}
			@fclose($fp);
			return true;
		}else{
			return false;
		}
	}

	//天気情報をファイルから取得	[2006/12/29]
	function GetWeather($pnt_id){
		if($weather = @file(WEATHER_DIR.$pnt_id)){
			$weather = trim($weather[0]);
			if(is_numeric($weather)){
				return sprintf("%02d",$weather);
			}else{
				return "00";
			}
		}else{
			return "00";
		}
	}

	//積雪情報をファイルに設定	[2007/02/24]
	function SetSnow($pnt_id,$snow,$level){
		@unlink(SNOW_DIR.$pnt_id);
		$fp = @fopen(SNOW_DIR.$pnt_id, "a");
		if($fp){
			if(is_numeric($snow)&&is_numeric($level)){
				@fwrite($fp,"{$snow},{$level}");
			}else{
				@fclose($fp);
				return false;
			}
			@fclose($fp);
			return true;
		}else{
			return false;
		}
	}

	//積雪情報をファイルから取得	[2007/02/24]
	function GetSnow($pnt_id){
		if($line = @file(SNOW_DIR.$pnt_id)){
			$snow = trim($line[0]);
			$snow = explode(",",$snow);
			$snow[0] += 0;
			$snow[1] += 0;
			return join(",",$snow);
		}else{
			return "0,0";
		}
	}

	//天気情報をＤＢから取得	[2006/12/21]
	function GetWeatherData($pnt_id,$curTime){
		if(time()<$curTime) return "";
		$date		= DateConvert($curTime);
		$tbl		= "t000_dat_".$date["year"];
		$strSql = "SELECT weather FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND t000_date = '".$date["date"]."';";
		if(is_object($this->log))	$this->log->logPrint("****GetWeatherData**** ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs = new CONN;
		if(!$row = $dbs->Query($strSql)){
			return "";
		}
		if(is_numeric($row[0])){
			return sprintf("%02d",$row[0]);
		}else{
			return "";
		}
	}

	//積雪情報をＤＢから取得(深さ）	[2007/02/21]
	function GetSnowData1($pnt_id,$curTime){
		if(time()<$curTime) return "";
		$date		= DateConvert($curTime);
		$tbl		= "t000_dat_".$date["year"];
		$strSql = "SELECT snow_level FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND t000_date = '".$date["date"]."';";
		if(is_object($this->log))	$this->log->logPrint("****GetSnowData**** ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs = new CONN;
		if(!$row = $dbs->Query($strSql)){
			return "";
		}
		if(is_numeric($row[0])){
			return $row[0];
		}else{
			return "";
		}
	}

	//積雪情報をＤＢから取得(時間積算）	[2007/02/21]
	function GetSnowData2($pnt_id,$curTime){
		if(time()<$curTime) return "";
		$date		= DateConvert($curTime);
		$tbl		= "t000_dat_".$date["year"];
		$strSql = "SELECT snow_hour FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND t000_date = '".$date["date"]."';";
		if(is_object($this->log))	$this->log->logPrint("****GetSnowData2**** ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs = new CONN;
		if(!$row = $dbs->Query($strSql)){
			return "";
		}
		if(is_numeric($row[0])){
			return $row[0];
		}else{
			return "";
		}
	}

	//積雪情報を時報テーブルに書き出し[2007/02/27]
	function SetSnowData2($pnt_id,$curTime,$value){
		$date		= DateConvert($curTime);
		$tbl		= "t100_dat_".$date["year"];
		$strSql = "UPDATE {$tbl} SET snow_hour = '$value' WHERE t100_pnt_id = '{$pnt_id}' AND t100_date = '".$date["date"]."';";
		if(is_object($this->log))	$this->log->logPrint("****SetSnowData2**** ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	//指定日の天気情報を局情報の天気取得時間別にＤＢから取得(月報レコード作成時)	[2007/01/23]
	function GetWeatherData2($pnt_id,$curTime){
		if(time() < $curTime) return "";

		//局情報から天気取得時間を取得
		$pnt   		= new Points($this->log);
		$Point 		= $pnt->LoadPointInfo($pnt_id);
		$cls			= new Data($this->log);
		$AryData	= array();
		for($i=1;$i<4;$i++){
			$trg = date("Ymd",$curTime).sprintf("%02d",$Point["pnt_300_weather".$i])."00";
			$AryTmp		= $cls->LoadData($pnt_id,"000",DateConvert3($trg));
			//if(is_object($this->log))	$this->log->logPrint("<><>GetWeatherData2<><> ".print_r($AryTmp,TRUE),REC_DEBUG,__LINE__,__FILE__,0,1);

			$AryData["weather".$i] = $AryTmp[$trg]["weather"];
			if(is_object($this->log))	$this->log->logPrint("<><>GetWeatherData2<><> {$trg} => ".$AryData["weather".$i],REC_DEBUG,__LINE__,__FILE__);
		}
		if(is_object($this->log))	$this->log->logPrint("<><>GetWeatherData2<><> ".date("Y/m/d",$curTime)." ".print_r($AryData,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $AryData;
	}

	//累計雨量算出	[2006/12/29]
	function GetRuiRain($pnt_id,$curTime,$rain_1min){
		//1分積算データを元に累計雨量記録処理
		if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// 累計雨量算出開始",REC_DEBUG,__LINE__,__FILE__);
		//念のためデータ補正
		$rain_1min	= is_numeric($rain_1min)=="" ? 0 : $rain_1min;
		//処理時間取得
		$date_value = date("Y/m/d H:i:s",$curTime);

		//累計雨量テーブルより降雨中データ取得
		$strSql = "SELECT * FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
		if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain t500_dat => ".print_r($row,TRUE),REC_DEBUG,__LINE__,__FILE__);
		$rain_rui = $row["t500_value"];

		if($rain_1min>0){
			//現在降雨あり
			if($pnt_id==$row["t500_pnt_id"]){
				//降雨中
				if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain Update",										REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain LastValue  => ".$rain_rui,	REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain AddValue   => ".$rain_1min,	REC_DEBUG,__LINE__,__FILE__);
				$rain_value = number_format($rain_rui + $rain_1min,1, ".", "");
				if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain TotalValue => ".$rain_value,REC_DEBUG,__LINE__,__FILE__);
				$strSql = "UPDATE t500_dat SET t500_value = {$rain_value},t500_last = '{$date_value}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
			}else{
				//降り始め
				if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain Start",REC_DEBUG,__LINE__,__FILE__);
				$rain_value = number_format($rain_1min,1, ".", "");
				if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain TotalValue => ".$rain_value,REC_DEBUG,__LINE__,__FILE__);
				$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'{$pnt_id}','{$date_value}',NULL,'{$date_value}',{$rain_value});";
			}
			if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);

			if($dbs->Execute($strSql)){
				$rain = $rain_value;
			}else{
				if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
				$rain = "";
			}
		}else{
			//現在降雨なし
			if($pnt_id==$row["t500_pnt_id"]){
				if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain Reset Check",REC_DEBUG,__LINE__,__FILE__);

				//累計リセット時間取得
				$pnt   = new Points($this->log);
				$Point = $pnt->LoadPointInfo($pnt_id);
				$reset = $Point["pnt_tmg_rain"];

				//処理時間取得
				$date				= DateConvert($curTime);
				$date_reset = DateConvert($curTime - $reset * 3600);
				if($date["year"]==$date_reset["year"]){
					$tbl		= "t000_dat_".$date["year"];
					$trg		= "t000_date > '".$date_reset["date"]."'";
					$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND rain_1min <> '' AND {$trg};";
					if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row 		= $dbs->Query($strSql);
					$rain_value = $row[0] + 0;
				}else{
					//去年
					$tbl		= "t000_dat_".$date_reset["year"];
					$trg		= "t000_date > '".$date_reset["date"]."'";
					$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND rain_1min <> '' AND {$trg};";
					if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row		= $dbs->Query($strSql);
					$rain_value = $row[0] + 0;
					//今年
					$tbl		= "t000_dat_".$date["year"];
					$trg		= "t000_date <= '".$date["date"]."'";
					$strSql = "SELECT SUM(TO_NUMBER(rain_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND rain_1min <> '' AND {$trg};";
					if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row 		= $dbs->Query($strSql);
					$rain_value = $rain_value + ($row[0]+0);
				}
				if($rain_value==0){
					//降雨終了処理
					$strSql = "UPDATE t500_dat SET t500_edt = '{$date_value}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
					if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					if($dbs->Execute($strSql)){
						$rain = "0.0";
					}else{
						if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
						$rain = "";
					}
				}
				$rain = number_format($rain_rui,1, ".", "");
			}else{
				//累計雨量、現在降雨なし
				if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// Rain Not Falling",REC_DEBUG,__LINE__,__FILE__);
				$rain = "0.0";
			}
		}
		if(is_object($this->log))	$this->log->logPrint("////GetRuiRain//// 累計雨量算出終了 累積雨量 => {$rain}",REC_DEBUG,__LINE__,__FILE__);
		return $rain;
	}

	////////////////////
	//  累計雨量設定　//
	////////////////////
	function SetRuiRain($pnt_id,$curTime,$rain_rui){
		//累計雨量出力タイプはロガー累積雨量をもとに累計雨量記録処理

		if(is_object($this->log))	$this->log->logPrint("======SetRuiRain=======",REC_DEBUG,__LINE__,__FILE__);

		if(!is_numeric($rain_rui)){
			if(is_object($this->log))	$this->log->logPrint("======累計雨量データなし=======",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}

		//処理時間取得
		$date_value = date("Y/m/d H:i:s",$curTime);

		//累計雨量テーブルより降雨中データ取得
		$strSql = "SELECT * FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		if(is_object($this->log))	$this->log->logPrint("Rain t500_dat => ".print_r($row,TRUE),REC_DEBUG,__LINE__,__FILE__);
		//$rain_rui = $row["t500_value"];

		if($rain_rui > 0){
			//現在降雨あり
			if($pnt_id==$row["t500_pnt_id"]){
				if($rain_rui > $row["t500_value"]){
					//降雨中
					if(is_object($this->log))	$this->log->logPrint("=== Rain Update ===",						REC_DEBUG,__LINE__,__FILE__);
					$rain_value = number_format($rain_rui,1, ".", "");
					if(is_object($this->log))	$this->log->logPrint("Rain TotalValue => ".$rain_value,				REC_DEBUG,__LINE__,__FILE__);
					$strSql = "UPDATE t500_dat SET t500_value = {$rain_value},t500_last = '{$date_value}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
				}
			}else{
				//降り始め
				if(is_object($this->log))	$this->log->logPrint("=== Rain Start ===",REC_DEBUG,__LINE__,__FILE__);
				$rain_value = number_format($rain_rui,1, ".", "");
				if(is_object($this->log))	$this->log->logPrint("Rain TotalValue => ".$rain_value,REC_DEBUG,__LINE__,__FILE__);
				$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'{$pnt_id}','{$date_value}',NULL,'{$date_value}',{$rain_value});";
			}
			if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

			if($dbs->Execute($strSql)){
				if(is_object($this->log))	$this->log->logPrint("=== GetRuiRain Succeeded ===",REC_DEBUG,__LINE__,__FILE__);
				return $rain_value;
			}else{
				if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
				return "";
			}
		}else{

			//累計雨量なし
			//降雨終了処理
			$strSql = "UPDATE t500_dat SET t500_edt = '{$date_value}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
			if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
			if($dbs->Execute($strSql)){
				if(is_object($this->log))	$this->log->logPrint("=== GetRuiRain Succeeded ===",REC_DEBUG,__LINE__,__FILE__);
				//return "0.0";
			}else{
				if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
				//return "";
			}

		}
	}

	//電文用BCC作成	[2006/12/29]
	function BCC($str){
		for($i = 0; $i<strLen($str); $i++) {
			if($i == 0) {
				$tmp = substr($str, $i, 1);
			} else {
				$tmp = $tmp ^ substr($str, $i, 1);
			}
		}
		$tmp = sprintf("%08b", ord($tmp));
		$ret = Array();
		if(strlen($tmp) <=4){
			$ret[0] = "0";
		} else {
			$ret[0] = strtoupper(dechex(bindec(substr($tmp, 0, strlen($tmp) - 4))));
		}
		$ret[1] = strtoupper(dechex(bindec(substr($tmp, strlen($tmp) - 4, 4))));
		return $ret;
	}

	//余計な制御文字削除	[2006/12/29]
	function ClearBuf($buf){
		$buf = str_replace(chr(2),"", 	$buf);	//STX
		$buf = str_replace(chr(3),"", 	$buf);	//ETX
		$buf = str_replace(chr(4),"",		$buf);	//EOT
		$buf = str_replace("*","", 			$buf);	//欠測データ
		$buf = str_replace("//://","", 	$buf);	//欠測データ(CX1000)
		$buf = str_replace("/,",",", 		$buf);	//欠測データ(CX1000)
		$buf = trim($buf);											//空白およびその他の制御文字
		return $buf;
	}

	//カンマ区切りデータを余計な空白文字を削除して配列にセパレート	[2007/01/16]
	function ClearBuf2($buf){
		$AryTmp = explode(",",$buf);
		$tmp		= array();
		foreach($AryTmp as $val){
			if(strpos(" ".$val,"-")>0){
				$val = str_replace("-","",$val);
				$val = trim($val);
				$val = "-".$val;
			}
			$val = trim($val);

			$tmp[] = $val;
		}
		return $tmp;
	}

	//風向頻度算出	[2007/05/14]
	function GetDirection($pnt_id,$sys_no,$curTime){
		global $dir,$dir2;
		$dbs		= new CONN;
		$date		= DateConvert($curTime);

		$total	= 0;
		$AryDir	= array();

		switch($sys_no){
			case "200":
				$sys		= "100";
				$tbl		= "t{$sys}_dat_".$date["year"];
				$trg		= "t{$sys}_date BETWEEN '".$date["year"].$date["month"].$date["day"]."0001' AND '".$date["year"].$date["month"].$date["day"].$date["hour"]."00'";

				for($i=0;$i<17;$i++){
					$strSql = "SELECT COUNT(*) FROM {$tbl} WHERE t{$sys}_pnt_id = '{$pnt_id}' AND avg_wd <> '' AND (TO_NUMBER(avg_wd,'9999.99')) = {$i} AND {$trg};";
					if(is_object($this->log))	$this->log->logPrint("----GetDirection---- ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row		= $dbs->Query($strSql);
					if(is_numeric($row[0])){
						$AryDir[ $dir[$i] ]	=	 $row[0];
						$total += $row[0];
					}
				}
				if(is_object($this->log))	$this->log->logPrint("----GetDirection---- ".print_r($AryDir,TRUE),REC_DEBUG,__LINE__,__FILE__);

				break;
			case "300":
//					$sys		= "100";
//					$tbl		= "t{$sys}_dat_".$date["year"];
//					$trg		= "t{$sys}_date BETWEEN '".$date["year"].$date["month"]."000001' AND '".$date["year"].$date["month"].$date["day"].$date["hour"]."00'";
//
//					for($i=0;$i<17;$i++){
//						$strSql = "SELECT COUNT(*) FROM {$tbl} WHERE t{$sys}_pnt_id = '{$pnt_id}' AND avg_wd <> '' AND (TO_NUMBER(avg_wd,'9999.99')) = {$i} AND {$trg};";
//						if(is_object($this->log))	$this->log->logPrint("----GetDirection---- ".$strSql,REC_DEBUG,__LINE__,__FILE__);
//						$row		= $dbs->Query($strSql);
//						if(is_numeric($row[0])){
//							$AryDir[ $dir[$i] ]	=	 $row[0];
//							$total += $row[0];
//						}
//					}
//					break;
				$time1 = strtotime($date["year"]."/".$date["month"]."/01 00:01");
				$time2 = strtotime($date["year"]."/".$date["month"]."/".$date["day"]." 00:01");
				while($time1<=$time2){
					$data  	=	new Data;
					$tmp		= $data->LoadSumData($pnt_id,"200",$time1);
					if(is_object($this->log))	$this->log->logPrint("----GetDirection---- "."[".date("Y/m/d H:i",$time1)."]".$tmp["90"]["avg_ws"],REC_DEBUG,__LINE__,__FILE__);
					$time1 += 86400;

					$tmp_dir  = @explode(",",$tmp["90"]["avg_ws"]);
					for($i=0;$i<=16;$i++){
						if(!is_numeric($tmp_dir[$i])) continue;
						$value = $tmp_dir[$i] * 10;
						$AryDir[ $dir2[$i] ] += $value;
						$total += $value;
					}
				}
				break;
			case "400":
//					$sys		= "100";
//					$tbl		= "t{$sys}_dat_".$date["year"];
//					$trg		= "t{$sys}_date BETWEEN '".$date["year"]."00000001' AND '".$date["year"].$date["month"].$date["day"].$date["hour"]."00'";
//
//					for($i=0;$i<17;$i++){
//						$strSql = "SELECT COUNT(*) FROM {$tbl} WHERE t{$sys}_pnt_id = '{$pnt_id}' AND avg_wd <> '' AND (TO_NUMBER(avg_wd,'9999.99')) = {$i} AND {$trg};";
//						if(is_object($this->log))	$this->log->logPrint("----GetDirection---- ".$strSql,REC_DEBUG,__LINE__,__FILE__);
//						$row		= $dbs->Query($strSql);
//						if(is_numeric($row[0])){
//							$AryDir[ $dir[$i] ]	=	 $row[0];
//							$total += $row[0];
//						}
//					}
//					break;
				for($m=1;$m<=$date["month"];$m++){
					$time = strtotime($date["year"]."/".$m."/01 00:01");
					$data	=	new Data;
					$tmp	= $data->LoadSumData($pnt_id,"300",$time);
					if(is_object($this->log))	$this->log->logPrint("----GetDirection---- "."[".date("Y/m/d H:i",$time)."]".$tmp["90"]["avg_ws"],REC_DEBUG,__LINE__,__FILE__);
					$tmp_dir  = @explode(",",$tmp["90"]["avg_ws"]);
					for($i=0;$i<=16;$i++){
						if(!is_numeric($tmp_dir[$i])) continue;
						$value = $tmp_dir[$i] * 10;
						$AryDir[ $dir2[$i] ] += $value;
						$total += $value;
					}
				}
				break;
			default:
				return false;
		}

		$silent = 100;
		if($total>0){
			for($i=0;$i<16;$i++){
				$AryDir[ $dir2[$i] ] = @floor($AryDir[ $dir2[$i] ] / $total * 1000);
				$AryDir[ $dir2[$i] ] = $AryDir[ $dir2[$i] ] / 10;
				$AryDir[ $dir2[$i] ] = number_format($AryDir[ $dir2[$i] ], 1, ".", "");
				$silent = $silent - $AryDir[ $dir2[$i] ];
			}
			$AryDir[ $dir2[16] ] = number_format($silent, 1, ".", "");
		}else{
			for($i=0;$i<=16;$i++){
				$AryDir[ $dir2[$i] ] = "";
			}
		}

		if(is_object($this->log))	$this->log->logPrint("----GetDirection---- ".print_r($AryDir,TRUE),REC_DEBUG,__LINE__,__FILE__);

		$AryTmp = array();
		for($i=0;$i<17;$i++)	$AryTmp[]	= $AryDir[ $dir2[$i] ];
		return join(",",$AryTmp);
	}
//		//風向頻度算出	[2006/12/30]
//		function GetDirection($pnt_id,$sys_no,$curTime){
//			global $dir,$dir2;
//			$dbs		= new CONN;
//			$date		= DateConvert($curTime);
//			switch($sys_no){
//				case "200":
//					$sys		= "100";
//					$tbl		= "t100_dat_".$date["year"];
//					$trg		= "t100_date BETWEEN '".$date["year"].$date["month"].$date["day"]."0001' AND '".$date["year"].$date["month"].$date["day"].$date["hour"]."00'";
//					break;
//				case "300":
//					$sys		= "200";
//					$tbl		= "t200_dat_".$date["year"];
//					$trg		= "t200_date BETWEEN '".$date["year"].$date["month"]."000001' AND '".$date["year"].$date["month"].$date["day"].$date["hour"]."00'";
//					break;
//				case "400":
//					$sys		= "300";
//					$tbl		= "t300_dat_".$date["year"];
//					$trg		= "t300_date BETWEEN '".$date["year"]."00000001' AND '".$date["year"].$date["month"].$date["day"].$date["hour"]."00'";
//					break;
//				default:
//					return false;
//			}
//
//			$total	= 0;
//			$AryDir	= array();
//			for($i=0;$i<17;$i++){
//				$strSql = "SELECT COUNT(*) FROM {$tbl} WHERE t{$sys}_pnt_id = '{$pnt_id}' AND avg_wd <> '' AND (TO_NUMBER(avg_wd,'9999.99')) = {$i} AND {$trg};";
//				if(is_object($this->log))	$this->log->logPrint("----GetDirection---- ".$strSql,REC_DEBUG,__LINE__,__FILE__);
//				$row		= $dbs->Query($strSql);
//				if(is_numeric($row[0])){
//					$AryDir[ $dir[$i] ]	=	 $row[0];
//					$total += $row[0];
//				}
//			}
//			if(is_object($this->log))	$this->log->logPrint("----GetDirection---- ".print_r($AryDir,TRUE),REC_DEBUG,__LINE__,__FILE__);
//
//			$silent = 100;
//			if($total>=0){
//				for($i=0;$i<16;$i++){
//					$AryDir[ $dir2[$i] ] = @floor($AryDir[ $dir2[$i] ] / $total * 100);
//					$AryDir[ $dir2[$i] ] = $AryDir[ $dir2[$i] ] / 100;
//					$AryDir[ $dir2[$i] ] = number_format($AryDir[ $dir2[$i] ], 1, ".", "");
//					$silent = $silent - $AryDir[ $dir2[$i] ];
//				}
//				$AryDir[ $dir2[16] ] = number_format($silent, 1, ".", "");
//			}
//
//			if(is_object($this->log))	$this->log->logPrint("----GetDirection---- ".print_r($AryDir,TRUE),REC_DEBUG,__LINE__,__FILE__);
//
//			$AryTmp = array();
//			for($i=0;$i<17;$i++)	$AryTmp[]	= $AryDir[ $dir2[$i] ];
//			return join(",",$AryTmp);
//		}

	function OpenCheck(){
		if(is_object($this->log))	$this->log->logPrint("---- OpenCheck ---- Start",																	REC_DEBUG,__LINE__,__FILE__);
		if(is_object($this->log))	$this->log->logPrint("---- OpenCheck ---- LastCloseTime => ".$this->LastCloseTime,REC_DEBUG,__LINE__,__FILE__);
		if(is_object($this->log))	$this->log->logPrint("---- OpenCheck ---- time          => ".time(),							REC_DEBUG,__LINE__,__FILE__);
		//ソケット連続オープン防止用チェック
		while( $this->LastCloseTime >= time() ){
			sleep(1);
		}
		if(is_object($this->log))	$this->log->logPrint("---- OpenCheck ---- Start",																	REC_DEBUG,__LINE__,__FILE__);
	}

	//保存するフィールドIDがロガーと出力用配列に存在するかチェックする
	function OutCheck($key,$sys_no,&$args){
		if( array_key_exists($key,$args) && in_array($key,$this->OutField[$sys_no]) ){
			return true;
		}else{
			return false;
		}
	}

	function ChkFld($key,&$args,$sys_no){
		if(array_key_exists($key,$args)&&in_array($key,$this->OutField[$sys_no])){
			return true;
		}else{
			return false;
		}
	}

	function GetPresentMoveSnow($pnt_id,$curTime,$args){
		//積雪深を元に一分積算降雪深を求め、それを元に10分移動降雪深、1時間移動降雪深を求める
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 移動積算算出処理",REC_DEBUG,__LINE__,__FILE__);

		//積雪深を取得
		$snow_level = $args["snow_level"];
		//積雪深が数値ではない場合データが異常なので終了
		if(!is_numeric($snow_level)){
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ABEND 積雪深 => {$snow_level}",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 積雪深 => {$snow_level}",REC_DEBUG,__LINE__,__FILE__);

		$dbs	= new CONN;
		$data	= array();

		//前1分の積雪深を取得
		$prev		= DateConvert($curTime-60);
		$strSql = "SELECT snow_level FROM t000_dat_".$prev["year"]." WHERE t000_pnt_id = '{$pnt_id}' AND t000_date = '".$prev["date"]."';";
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		if($row=$dbs->Query($strSql)){
			$prev_snow_level = trim($row[0]);
		}
		if(!is_numeric($prev_snow_level)){
			return false;
		}
		$now_snow_level = $snow_level - $prev_snow_level;
		if($now_snow_level < 0){
			$now_snow_level = 0;
		}
		$data["snow_1min"] =  number_format($now_snow_level,0, ".", "");
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 1分積算降雪深 => ".$data["snow_1min"],REC_DEBUG,__LINE__,__FILE__);

		$now = DateConvert($curTime);

		//10分移動積算算出
		$data["move_snow_10min"] = $data["snow_1min"];
		$prev	= DateConvert($curTime-540);
		if($now["year"]==$prev["year"]){
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(snow_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND snow_1min <> '' AND TO_NUMBER(snow_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_snow_10min"] = number_format($row[0] + $data["move_snow_10min"],0, ".", "");
		}else{
			//前年データ取得
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$prev["year"]."12312400'";
			$strSql = "SELECT SUM(TO_NUMBER(snow_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND snow_1min <> '' AND TO_NUMBER(snow_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_snow_10min"] = number_format($row[0] + $data["move_snow_10min"],0, ".", "");
			//当年データ取得
			$tbl		= "t000_dat_".$now["year"];
			$trg		= "t000_date BETWEEN '".$now["year"]."01010001' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(snow_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND snow_1min <> '' AND TO_NUMBER(snow_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			$tmp = 0;
			if(is_numeric($row[0]))	$data["move_snow_10min"] = number_format($row[0] + $data["move_snow_10min"],0, ".", "");
		}
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 10分移動降雪深 => ".$data["move_snow_10min"],REC_DEBUG,__LINE__,__FILE__);

		//1時間移動積算算出
		$data["move_snow_hour"] = $data["snow_1min"];
		$prev	= DateConvert($curTime-3540);
		if($now["year"]==$prev["year"]){
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(snow_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND snow_1min <> '' AND TO_NUMBER(snow_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_snow_hour"] = number_format($row[0] + $data["move_snow_hour"],0, ".", "");
		}else{
			//前年データ取得
			$tbl		= "t000_dat_".$prev["year"];
			$trg		= "t000_date BETWEEN '".$prev["date"]."' AND '".$prev["year"]."12312400'";
			$strSql = "SELECT SUM(TO_NUMBER(snow_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND snow_1min <> '' AND TO_NUMBER(snow_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_snow_hour"] = number_format($row[0] + $data["move_snow_hour"],0, ".", "");
			//当年データ取得
			$tbl		= "t000_dat_".$now["year"];
			$trg		= "t000_date BETWEEN '".$now["year"]."01010001' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(snow_1min,'9999.99')) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND {$trg} AND snow_1min <> '' AND TO_NUMBER(snow_1min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			$tmp = 0;
			if(is_numeric($row[0]))	$data["move_snow_hour"] = number_format($row[0] + $data["move_snow_hour"],0, ".", "");
		}
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 1時間移動降雪深 => ".$data["move_snow_hour"],REC_DEBUG,__LINE__,__FILE__);

		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".print_r($data,TRUE),REC_DEBUG,__LINE__,__FILE__);
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 終了",								REC_DEBUG,__LINE__,__FILE__);
		return $data;
	}

	function GetMinutelyMoveSnow($pnt_id,$curTime,$args){
		//積雪深を元に一分積算降雪深を求め、それを元に10分移動降雪深、1時間移動降雪深を求める
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 移動積算算出処理",REC_DEBUG,__LINE__,__FILE__);

		//積雪深を取得
		$snow_level = $args["snow_level"];
		//積雪深が数値ではない場合データが異常なので終了
		if(!is_numeric($snow_level)){
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ABEND 積雪深 => {$snow_level}",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 積雪深 => {$snow_level}",REC_DEBUG,__LINE__,__FILE__);

		$dbs	= new CONN;
		$data	= array();

		//前10分の積雪深を取得
		$prev		= DateConvert($curTime-600);
		$strSql = "SELECT snow_level FROM t100_dat_".$prev["year"]." WHERE t100_pnt_id = '{$pnt_id}' AND t100_date = '".$prev["date"]."';";
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		if($row=$dbs->Query($strSql)){
			$prev_snow_level = trim($row[0]);
		}
		if(!is_numeric($prev_snow_level)){
			return false;
		}
		$now_snow_level = $snow_level - $prev_snow_level;
		if($now_snow_level < 0){
			$now_snow_level = 0;
		}
		$data["move_snow_10min"] =  number_format($now_snow_level,0, ".", "");
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 10分移動降雪深 => ".$data["move_snow_10min"],REC_DEBUG,__LINE__,__FILE__);

		$now = DateConvert($curTime);

		//1時間移動積算算出
		$data["move_snow_hour"] = $data["move_snow_10min"];
		$prev	= DateConvert($curTime-3540);
		if($now["year"]==$prev["year"]){
			$tbl		= "t100_dat_".$prev["year"];
			$trg		= "t100_date BETWEEN '".$prev["date"]."' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(move_snow_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} AND move_snow_10min <> '' AND TO_NUMBER(move_snow_10min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_snow_hour"] = number_format($row[0] + $data["move_snow_hour"],0, ".", "");
		}else{
			//前年データ取得
			$tbl		= "t100_dat_".$prev["year"];
			$trg		= "t100_date BETWEEN '".$prev["date"]."' AND '".$prev["year"]."12312400'";
			$strSql = "SELECT SUM(TO_NUMBER(move_snow_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} AND move_snow_10min <> '' AND TO_NUMBER(move_snow_10min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			if(is_numeric($row[0]))	$data["move_snow_hour"] = number_format($row[0] + $data["move_snow_hour"],0, ".", "");
			//当年データ取得
			$tbl		= "t100_dat_".$now["year"];
			$trg		= "t100_date BETWEEN '".$now["year"]."01010001' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(move_snow_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} AND move_snow_10min <> '' AND TO_NUMBER(move_snow_10min,'9999.99') > 0;";
			if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$dbs = new CONN;
			$row = $dbs->Query($strSql);
			$tmp = 0;
			if(is_numeric($row[0]))	$data["move_snow_hour"] = number_format($row[0] + $data["move_snow_hour"],0, ".", "");
		}
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 1時間移動降雪深 => ".$data["move_snow_hour"],REC_DEBUG,__LINE__,__FILE__);

		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| ".print_r($data,TRUE),REC_DEBUG,__LINE__,__FILE__);
		if(is_object($this->log))	$this->log->logPrint("||||".__FUNCTION__."|||| 終了",								REC_DEBUG,__LINE__,__FILE__);
		return $data;
	}
	//手動収集時
	function GetMinutelyColMoveSnow($pnt_id,$curTime,$args){
		$snow_level = $args["snow_level"];
		if(!is_numeric($snow_level)){
			if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [手動収集時] snow_level-> No Data[{$snow_level}]",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [手動収集時] snow_level-> {$snow_level}",REC_DEBUG,__LINE__,__FILE__);

		$AryData = array();

		$date		= DateConvert($curTime-600);
		$AryTmp = $this->AryColData[ $date["date"] ];
		if(is_array($AryTmp)){
			$prev_snow_level = trim($AryTmp["snow_level"]);
			if(!is_numeric($prev_snow_level)){
				if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [手動収集時] prev_snow_level-> No Data",REC_DEBUG,__LINE__,__FILE__);
				return false;
			}
			if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [手動収集時] prev_snow_level-> [{$prev_snow_level}]",REC_DEBUG,__LINE__,__FILE__);

			$now_snow_level = $snow_level - $prev_snow_level;
			if($now_snow_level < 0){
				$now_snow_level = 0;
			}
			$AryData["move_snow_10min"] = number_format($now_snow_level,0, ".", "");
		}
		if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [手動収集時] move_snow_10min-> [{$now_snow_level}]",REC_DEBUG,__LINE__,__FILE__);

		if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [". $date["date"]."] + ".$AryTmp["move_snow_10min"],REC_DEBUG,__LINE__,__FILE__);
		$AryData["move_snow_hour"] = $AryTmp["move_snow_10min"];
		for($i=6;$i>1;$i--){
			$date		= DateConvert($curTime-$i*600);
			$AryTmp = $this->AryColData[ $date["date"] ];
			if(is_array($AryTmp)){
				$AryData["move_snow_hour"] += $AryTmp["move_snow_10min"];
				if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [". $date["date"]."] + ".$AryTmp["move_snow_10min"],REC_DEBUG,__LINE__,__FILE__);
			}else{
				if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [". $date["date"]."] + No Data",REC_DEBUG,__LINE__,__FILE__);
			}
		}
		if(is_numeric($AryData["move_snow_hour"])){
			$AryData["move_snow_hour"] = number_format($AryData["move_snow_hour"],0, ".", "");
		}
		return $AryData;
	}

}
?>