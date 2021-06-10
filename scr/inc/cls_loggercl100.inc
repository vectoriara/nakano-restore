<?
	//========================================================//
	//	RS0001ロガークラス									   //
	//--------------------------------------------------------//
	//	処理概要											   //
	//		省電力ロガー用処理クラス							//
	//--------------------------------------------------------//
	//	変更履歴											   //
	//		2015/11/01	N.ARAI								  //
	//		・新規作成										   //
	//========================================================//
class LoggerCL100 extends BaseLogger
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2014.09.18";
	var $memo		= "";
	var $id			= "CL100" ;
	var $type		= "1";		//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
	var $ReCal		= "1";		//再計算処理可否
	var $editWeather = false;
	var $editWarning = false;

	var $SettingTemplate = "lgr_cl100.tmpl";

	var $map		= array("lgr_id"		=> "lgr_id",
							"lgr_pnt_id"	=> "lgr_pnt_id",
							"lgr_enable"	=> "lgr_enable",
							"lgr_time"		=> "lgr_time",
							"lgr_ip"		=> "lgr_col_1",
							"lgr_port"		=> "lgr_col_2",
							"lgr_no"		=> "lgr_col_3",
							);
	var $EditSys	= array("100");	//編集可能なデータ帳票
	var $ColSys		= array("100");	//手動収集可能なデータ帳票
	var $DelSys		= array("100");	//削除可能なデータ帳票

	//月報レコード作成時参照先レコード（ロガー取得機能がない場合用）
	var	$AryFld300 = array(
		"avg_ws"			=> array("avg_ws",				"40"),
		"avg_wd"			=> array("avg_wd",				"40"),
		"max_ws"			=> array("max_ws",				"50"),
		"max_wd"			=> array("max_wd",				"50"),
		"avg_temp" 			=> array("temp",				"40"),
		"max_temp"			=> array("temp",				"50"),
		"min_temp" 			=> array("temp",				"60"),
		"avg_humid1"		=> array("humid1",				"40"),
		"max_humid1"		=> array("humid1",				"50"),
		"min_humid1"		=> array("humid1",				"60"),
		"humid2"			=> array("humid2",				"40"),
		"press1"			=> array("press1",				"40"),
		"avg_press1"		=> array("press1",				"40"),
		"max_press1"		=> array("press1",				"50"),
		"min_press1"		=> array("press1",				"60"),
		"press2"			=> array("press2",				"40"),
		"avg_press2"		=> array("press2",				"40"),
		"max_press2"		=> array("press2",				"50"),
		"min_press2"		=> array("press2",				"60"),
		"max_rain_10min"	=> array("max_rain_10min","50"),
		"max_rain_hour"		=> array("rain_hour",			"50"),
		"rain_day"			=> array("rain_day",			"70"),
		"water_level"		=> array("water_level",			"40"),
		"max__water"		=> array("max__water",			"50"),
		"min__water"		=> array("min__water",			"60"),
		"snow_level"		=> array("snow_level",			"50")
	);

	//年報レコード作成時参照先レコード（ロガー取得機能がない場合用）
	var	$AryFld400 = array(
		"avg_ws"				=> array("avg_ws",				"40"),
		"avg_wd"				=> array("avg_wd",				"40"),
		"max_ws"				=> array("max_ws",				"50"),
		"max_wd"				=> array("max_wd",				"50"),
		"avg_temp"				=> array("avg_temp",			"40"),
		"max_temp"				=> array("max_temp",			"50"),
		"min_temp"				=> array("min_temp",			"60"),
		"avg_humid1"			=> array("avg_humid1",			"40"),
		"max_humid1"			=> array("max_humid1",			"50"),
		"min_humid1"			=> array("min_humid1",			"60"),
		"humid2"				=> array("humid2",				"40"),
		"avg_humid2"			=> array("humid2",				"40"),
		"max_humid2"			=> array("humid2",				"50"),
		"min_humid2"			=> array("humid2",				"60"),
		"press1"				=> array("press1",				"40"),
		"avg_press1"			=> array("press1",				"40"),
		"max_press1"			=> array("press1",				"50"),
		"min_press1"			=> array("press1",				"60"),
		"press2"				=> array("press2",				"40"),
		"avg_press2"			=> array("press2",				"40"),
		"max_press2"			=> array("press2",				"50"),
		"min_press2"			=> array("press2",				"60"),
		"max_rain_10min"	=> array("max_rain_10min","50"),
		"max_rain_hour"		=> array("max_rain_hour",	"50"),
		"max_rain_day"			=> array("rain_day",			"50"),
		"rain_month"			=> array("rain_day",			"70"),
		"water_level"			=> array("water_level",			"40"),
		"max__water"			=> array("max__water",			"50"),
		"min__water"			=> array("min__water",			"60"),
		"snow_level"			=> array("snow_level",			"50")
	);

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
							"000" => array(
											"avg_wd",
											"avg_ws",
											"max_ws",
											"max_wd",
											"temp",
											"press1",
											"humid1",
											"rain_hour",
											"rain_day",
											"move_rain_10min",
											"move_rain_hour",
											"rain_rui",
											"rain_fall",
											"ins_wd",
											"ins_ws",
											"avg__temp",
											"max__temp",
											"min__temp",
											"avg__press1",
											"max__press1",
											"min__press1",
											"water_level",
											"avg__water",
											"max__water",
											"min__water",
											"snow_level",
											"mainte"
											),
							"100" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"temp",
											"press1",
											"rain_10min",
											"move_rain_10min",
											"max__move_rain_10min",
											"max__move_rain_10min_time",
											"rain_hour",
											"move_rain_hour",
											"max__move_rain_hour",
											"max__move_rain_hour_time",
											"rain_day",
											"max__avg_ws",
											"max__avg_ws_time",
											"max__avg_wd",
											"max__max_ws_time",
											"max__temp",
											"max__temp_time",
											"min__temp",
											"min__temp_time",
											"humid1",
											"max__humid1",
											"max__humid1_time",
											"min__humid1",
											"min__humid1_time",
											"max__press1",
											"max__press1_time",
											"min__press1",
											"min__press1_time",
											"water_level",
											"avg__water",
											"max__water",
											"max__water_time",
											"min__water",
											"min__water_time",
											"snow_level",
											"mainte"
											),
							"200" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"temp",
											"humid1",
											"max__humid1",
											"max__humid1_time",
											"min__humid1",
											"min__humid1_time",
											"press1",
											"move_rain_10min",
											"max_rain_10min",
											"rain_hour",
											"rain_day",
											"water_level",
											"avg__water",
											"max__water",
											"min__water",
											"snow_level",
											"mainte"
											),
							"300" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"avg_temp",
											"max_temp",
											"min_temp",
											"avg_humid1",
											"max_humid1",
											"min_humid1",
											"press1",
											"avg_press1",
											"max_press1",
											"min_press1",
											"max_rain_10min",
											"max_rain_hour",
											"rain_day",
											"water_level",
											"max__water",
											"min__water",
											"snow_level"
											),
							"400" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"avg_temp",
											"max_temp",
											"min_temp",
											"avg_humid1",
											"max_humid1",
											"min_humid1",
											"press1",
											"avg_press1",
											"max_press1",
											"min_press1",
											"max_rain_10min",
											"max_rain_hour",
											"max_rain_day",
											"rain_month",
											"water_level",
											"max__water",
											"min__water",
											"snow_level"
											)
							);

						//帳票毎でこのロガーが編集できる項目を定義
						var $EditField = array(
												"100" => array(
													"avg_ws",
													"max__avg_ws",
													"avg_wd",
													"max__avg_wd",
													"max__avg_ws_time",
													"max_ws",
													"max__max_ws_time",
													"max_wd",
													"temp",
													"max__temp",
													"max__temp_time",
													"min__temp",
													"min__temp_time",
													"press1",
													"rain_10min",
													"max__move_rain_10min",
													"max__move_rain_10min_time",
													"rain_hour",
													"move_rain_hour",
													"max__move_rain_hour",
													"max__move_rain_hour_time",
													"rain_day",
													"humid1",
													"max__humid1",
													"max__humid1_time",
													"min__humid1_time",
													"min__humid1",
													"min__humid1_time",
													"ins_wd",
													"ins_ws",
													"snow_level"
													)
												);
						var $EditWater = array(
							"100" => [
								"rain_10min",
								"max__move_rain_10min",
								"max__move_rain_10min_time",
								"rain_hour",
								"move_rain_hour",
								"max__move_rain_hour",
								"max__move_rain_hour_time",
								"rain_day",
								"water_level",
								"max__water",
								"max__water_time",
								"min__water",
								"min__water_time"
							]
						);

	//通信チェック [2006/12/31]
	function CheckConnection($pnt_id){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		$buf = $this->SendCmd($this->setting["lgr_no"]."R0".chr(13).chr(10));
		$buf = $this->ClearBuf($buf);
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		return $buf;
	}

	//時刻補正 [2006/12/31]
	function SetDate($pnt_id){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$buf = $this->SendCmd($this->setting["lgr_no"]."C1,".date("Y/m/d H:i:s").chr(13).chr(10));
			$buf = trim($buf);
			if($buf=="OK"){
				if(is_object($this->log)) $this->log->logPrint("時刻補正が完了しました",REC_NOTE,__LINE__,__FILE__);
				$ret = 1;
			}else{
				if(is_object($this->log)) $this->log->logPrint("時刻補正に失敗しました",REC_NOTE,__LINE__,__FILE__);
				$ret = false;
			}
		}else{
			if(is_object($this->log)) $this->log->logPrint("時刻補正は許可されていません",REC_DEBUG,__LINE__,__FILE__);
			$ret = 2;
		}
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		return $ret;
	}

	//ロガー電文を各項目にセパレートする
	function parsePresentData($AryTmp){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

		$AryData = array();
		for($i=2;$i<count($AryTmp);$i++) $AryTmp[$i] = trim(str_replace("/","",$AryTmp[$i]));

		//気温
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="T1"){
				$AryData["temp"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__temp"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["min__temp"] = is_numeric($AryTmp[$j+3]) ? number_format($AryTmp[$j+3]/10,1,".","")	: "";
						$AryData["avg__temp"] = is_numeric($AryTmp[$j+5]) ? number_format($AryTmp[$j+5]/10,1,".","")	: "";
						break 2;
					}
				}
				break;
			}
		}
		//10分移動積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RA"){
				$AryData["move_rain_10min"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//日積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RV"){
				$AryData["rain_day"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//1時間積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RW"){
				$AryData["rain_hour"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//1時間移動積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RC"){
				$AryData["move_rain_hour"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//累計雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RS"){
				$AryData["rain_rui"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				if($AryData["rain_rui"]==""&&is_numeric($AryData["rain_day"])){
					if(is_object($this->log)) $this->log->logPrint("RS[rain_rui]Undefined Change to [0.0]",REC_NOTE,__LINE__,__FILE__);
					$AryData["rain_rui"] = "0.0";
				}
				break;
			}
		}
		//最大瞬間風速時風向風速
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="WS"){
				$AryData["ins_ws"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","")	: "";
				$AryData["ins_wd"] = is_numeric($AryTmp[$i+3]) ? sprintf("%02d",$AryTmp[$i+3])				: "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max_ws"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["max_wd"] = is_numeric($AryTmp[$j+2]) ? sprintf("%02d",$AryTmp[$j+2])				: "";
						break 2;
					}
				}
				break;
			}
		}
		//平均風速
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="AS"){
				$AryData["avg_ws"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//平均風向
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="AD"){
				$AryData["avg_wd"] = is_numeric($AryTmp[$i+1]) ? sprintf("%02d",$AryTmp[$i+1]) : "";
				break;
			}
		}
		//現地気圧
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="BP"){
				$AryData["press1"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__press1"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["min__press1"] = is_numeric($AryTmp[$j+3]) ? number_format($AryTmp[$j+3]/10,1,".","")	: "";
						$AryData["avg__press1"] = is_numeric($AryTmp[$j+5]) ? number_format($AryTmp[$j+5]/10,1,".","")	: "";
						break 2;
					}
				}
				break;
			}
		}
		//海面気圧
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SP"){
				$AryData["press2"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//相対湿度
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RH"){
				$AryData["humid1"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//実効湿度
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="EH"){
				$AryData["humid2"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//水位
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="WL"){
				$AryData["water_level"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/100,2,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__water"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/100,2,".","")	: "";
						$AryData["min__water"] = is_numeric($AryTmp[$j+3]) ? number_format($AryTmp[$j+3]/100,2,".","")	: "";
						$AryData["avg__water"] = is_numeric($AryTmp[$j+5]) ? number_format($AryTmp[$j+5]/100,2,".","")	: "";
						break 2;
					}
				}
				break;
			}
		}
		// 積雪深
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SN"){
				$AryData["snow_level"] = is_numeric($AryTmp[$i+1]) ? number_format(round($AryTmp[$i+1]/10),0,".","") : "";
				break;
			}
		}
		//HK1
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="H1"){
				$AryData["HK1"] = trim($AryTmp[$i+1]);
				break;
			}
		}
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		return $AryData;
	}

	//ロガー電文を各項目にセパレートする
	function parseMinutelyData($AryTmp){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

		$AryData = array();
		for($i=2;$i<count($AryTmp);$i++) $AryTmp[$i] = trim(str_replace("/","",$AryTmp[$i]));

		//気温
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="T1"){
				$AryData["temp"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__temp"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["max__temp_time"] = trim($AryTmp[$j+2]);
						$AryData["min__temp"] = is_numeric($AryTmp[$j+4]) ? number_format($AryTmp[$j+4]/10,1,".","")	: "";
						$AryData["min__temp_time"] = trim($AryTmp[$j+5]);
						$AryData["avg__temp"] = is_numeric($AryTmp[$j+7]) ? number_format($AryTmp[$j+7]/10,1,".","")	: "";
						break 2;
					}
				}
				break;
			}
		}
		//10分積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RA"){
				$AryData["rain_10min"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						//10分移動積算雨量
						$AryData["max__move_rain_10min"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["max__move_rain_10min_time"] = trim($AryTmp[$j+2]);
						break 2;
					}
				}
				break;
			}
		}
		//日積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RV"){
				$AryData["rain_day"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//1時間積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RW"){
				$AryData["rain_hour"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//1時間移動積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RC"){
				$AryData["move_rain_hour"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__move_rain_hour"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["max__move_rain_hour_time"] = trim($AryTmp[$j+2]);
						break 2;
					}
				}
				break;
			}
		}
		//累計雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RS"){
				$AryData["rain_rui"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				$AryData["rain_rui_stime"] = trim($AryTmp[$i+2]);
				$AryData["rain_rui_etime"] = trim($AryTmp[$i+3]);
				if($AryData["rain_rui"]==""&&is_numeric($AryData["rain_day"])){
					$AryData["rain_rui"] = "0.0";
				}
				break;
			}
		}
		//最大瞬間風速時風向風速
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="WS"){
				$AryData["ins_ws"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","")	: "";
				$AryData["ins_wd"] = is_numeric($AryTmp[$i+3]) ? sprintf("%02d",$AryTmp[$i+3])				: "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max_ws"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["max__max_ws_time"] = trim($AryTmp[$j+2]);
						$AryData["max_wd"] = is_numeric($AryTmp[$j+3]) ? sprintf("%02d",$AryTmp[$j+3])	: "";
						break 2;
					}
				}
				break;
			}
		}
		//平均風速
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="AS"){
				$AryData["avg_ws"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//平均風向
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="AD"){
				$AryData["avg_wd"] = is_numeric($AryTmp[$i+1]) ? sprintf("%02d",$AryTmp[$i+1]) : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__avg_ws"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["max__avg_ws_time"] = trim($AryTmp[$j+2]);
						$AryData["max__avg_wd"] = is_numeric($AryTmp[$j+3]) ? sprintf("%02d",$AryTmp[$j+3])	: "";
						break 2;
					}
				}
				break;
			}
		}
		//現地気圧
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="BP"){
				$AryData["press1"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__press1"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["max__press1_time"] = trim($AryTmp[$j+2]);
						$AryData["min__press1"] = is_numeric($AryTmp[$j+4]) ? number_format($AryTmp[$j+4]/10,1,".","")	: "";
						$AryData["min__press1_time"] = trim($AryTmp[$j+5]);
						$AryData["avg__press1"] = is_numeric($AryTmp[$j+7]) ? number_format($AryTmp[$j+7]/10,1,".","")	: "";
						break 2;
					}
				}
				break;
			}
		}
		//相対湿度
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RH"){
				$AryData["humid1"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__humid1"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","")	: "";
						$AryData["max__humid1_time"] = trim($AryTmp[$j+2]);
						$AryData["min__humid1"] = is_numeric($AryTmp[$j+4]) ? number_format($AryTmp[$j+4]/10,1,".","")	: "";
						$AryData["min__humid1_time"] = trim($AryTmp[$j+5]);
						$AryData["avg__humid1"] = is_numeric($AryTmp[$j+7]) ? number_format($AryTmp[$j+7]/10,1,".","")	: "";
						break 2;
					}
				}
				break;
			}
		}
		//実効湿度
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="EH"){
				$AryData["humid2"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//海面気圧
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SP"){
				$AryData["press2"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="WL"){
				$AryData["water_level"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/100,2,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max__water"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/100,2,".","")	: "";
						$AryData["max__water_time"] = trim($AryTmp[$j+2]);
						$AryData["min__water"] = is_numeric($AryTmp[$j+4]) ? number_format($AryTmp[$j+4]/100,2,".","")	: "";
						$AryData["min__water_time"] = trim($AryTmp[$j+5]);
						$AryData["avg__water"] = is_numeric($AryTmp[$j+7]) ? number_format($AryTmp[$j+7]/100,2,".","")	: "";
						break 2;
					}
				}
				break;
			}
		}
		// 積雪深
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SN"){
				$AryData["snow_level"] = is_numeric($AryTmp[$i+1]) ? number_format(round($AryTmp[$i+1]/10),0,".","") : "";
				break;
			}
		}
		//HK1
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="H1"){
				$AryData["HK1"] = trim($AryTmp[$i+1]);
				break;
			}
		}
		return $AryData;
	}

	/**
	 * ロガーHK情報から点検ステータスを取得する
	 *
	 * @param string $HK ロガーHK情報文字列（16進数4桁）
	 * @return integer 点検中：１、通常：０
	 */
	function CheckHK($HK){
		$bit = sprintf("%016d",base_convert($HK, 16, 2));
		if(substr($bit,11,1)=="1"){
			return "1";
		}else{
			return "0";
		}
	}

	//現在データ取得処理
	function GetPresentData($pnt_id,$curTime,&$args){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd($this->setting["lgr_no"]."R0".chr(13).chr(10))){
			if(is_object($this->log)) $this->log->logPrint("[Exit False]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		$buf    = $this->ClearBuf($buf);
		$AryTmp = $this->ClearBuf2($buf);

		//日付チェック
		$date = DateConvert($curTime);
		$chkDate = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		if(is_object($this->log)) $this->log->logPrint(print_r($AryTmp,true),REC_DEBUG,__LINE__,__FILE__);

		if($AryTmp[1]==$chkDate){
			$AryData = $this->parsePresentData($AryTmp);

			if($this->CheckHK($AryData["HK1"])=="0"){
				foreach($this->OutField["000"] as $key){
					if($this->OutCheck($key,"000",$args)){
						$args[$key] = $AryData[$key];
					}
				}
				if($this->OutCheck("rain_fall","000",$args)){
					$args["rain_fall"] = is_numeric($AryData["move_rain_10min"]) ? number_format($AryData["move_rain_10min"]*6,1,".","") : "";
				}
				//HK情報解析
				if($this->OutCheck("mainte","000",$args)) $args["mainte"] = $this->CheckHK($AryData["HK1"]);	
			}
			// $failure = new FAILURE($this->log);
			// $failure->ParseHK($pnt_id,$curTime,$AryData["HK1"]);
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => ".$chkDate,REC_SOCK,__LINE__,__FILE__);
			if(is_object($this->log)) $this->log->logPrint("[Exit False]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__."::".print_r($args,TRUE),REC_NOTE,__LINE__,__FILE__);
		return true;
	}

	//時報データ取得処理
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$trg = date("Y/m/d H:i",$curTime);
		if(!$buf=$this->SendCmd($this->setting["lgr_no"]."R3,".$trg.",".$trg.chr(13).chr(10))){
			if(is_object($this->log)) $this->log->logPrint("[Exit False]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		print $buf."\n";
		$buf    = $this->ClearBuf($buf);
		$AryTmp = explode(chr(13).chr(10),$buf);
		$AryTmp = $this->ClearBuf2($AryTmp[0]);

		//日付チェック
		$date = DateConvert($curTime);
		$chkDate = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		if(is_object($this->log)) $this->log->logPrint(print_r($AryTmp,true),REC_DEBUG,__LINE__,__FILE__);

		if($AryTmp[1]==$chkDate){
			$AryData = $this->parseMinutelyData($AryTmp);

			if($this->CheckHK($AryData["HK1"])=="0"){
				foreach($this->OutField["100"] as $key){
					if($this->OutCheck($key,"100",$args)){
						$args[$key] = $AryData[$key];
					}
				}
				$this->SetRuiRain($pnt_id,$AryData["rain_rui"],$AryData["rain_rui_stime"],$AryData["rain_rui_etime"]);	
			}
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => ".$chkDate,REC_SOCK,__LINE__,__FILE__);
			if(is_object($this->log)) $this->log->logPrint("[Exit False]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__."::".print_r($args,TRUE),REC_NOTE,__LINE__,__FILE__);
		return true;
	}



	//累計雨量更新処理
	function SetRuiRain($pnt_id,$rain_rui,$start_time="",$end_time=""){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

		if(!is_numeric($rain_rui)){
			if(is_object($this->log)) $this->log->logPrint("[End Not Numeric]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		$dbs = new CONN;

		//計測中の累計雨量レコードを取得
		$flg_rain = false;
		$strSql = "SELECT COUNT(*) FROM t500_dat WHERE t500_pnt_id = '".$pnt_id."' AND t500_edt IS NULL;";
		if(is_object($this->log)) $this->log->logPrint(__FUNCTION__." ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		if(is_object($this->log)) $this->log->logPrint(__FUNCTION__." Rain Record Count[".$row[0]."]",REC_NOTE,__LINE__,__FILE__);
		if($row[0]>0) $flg_rain = true;

		$strSql = "";
		if($rain_rui>0) {
			$year	= substr($start_time,0,4);
			$month	= substr($start_time,4,2);
			$day	= substr($start_time,6,2);
			$hour	= substr($start_time,8,2);
			$min	= substr($start_time,11,2);
			$s_trg 	= $year."-".$month."-".$day." ".$hour.":".$min;
			if(1>@strtotime($s_trg)){
				if(is_object($this->log)) $this->log->logPrint("[End]Not Start Time Format[".$s_trg."]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
				return false;
			}
			$year	= substr($end_time,0,4);
			$month	= substr($end_time,4,2);
			$day	= substr($end_time,6,2);
			$hour	= substr($end_time,8,2);
			$min	= substr($end_time,11,2);
			$e_trg 	= $year."-".$month."-".$day." ".$hour.":".$min;
			if(1>@strtotime($e_trg)){
				if(is_object($this->log)) $this->log->logPrint("[End]Not End Time Format[".$e_trg."]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
				return false;
			}
			if($flg_rain){
				$strSql = "UPDATE t500_dat SET t500_value = ".$rain_rui.",t500_last = '".$e_trg."' WHERE t500_pnt_id = '".$pnt_id."' AND t500_sdt = '".$s_trg."';";
			}else{
				$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'".$pnt_id."','".$s_trg."',NULL,'".$e_trg."',".$rain_rui.");";
			}
		}else{
			if($flg_rain){
				$strSql = "UPDATE t500_dat SET t500_edt = now() WHERE t500_pnt_id = '".$pnt_id."' AND t500_edt IS NULL;";
			}
		}
		if($strSql!=""){
			if(is_object($this->log)) $this->log->logPrint(__FUNCTION__." ".$strSql,REC_NOTE,__LINE__,__FILE__);
			if(!$dbs->Execute($strSql)){
				if(is_object($this->log)) $this->log->logPrint(__FUNCTION__." ".$dbs->getError(),REC_NOTE,__LINE__,__FILE__);
				if(is_object($this->log)) $this->log->logPrint("[End False]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
				return false;
			}
		}
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		return true;
	}

	//累計雨量更新処理
	function SaveRuiRain($pnt_id,$rain_rui,$start_time="",$end_time=""){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

		if(!is_numeric($rain_rui)){
			if(is_object($this->log)) $this->log->logPrint("[End Not Numeric]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		$year	= substr($start_time,0,4);
		$month	= substr($start_time,4,2);
		$day	= substr($start_time,6,2);
		$hour	= substr($start_time,8,2);
		$min	= substr($start_time,11,2);
		$s_trg 	= $year."-".$month."-".$day." ".$hour.":".$min;
		if(1>@strtotime($s_trg)){
			if(is_object($this->log)) $this->log->logPrint("[End]Not Start Time Format[".$s_trg."]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		$year	= substr($end_time,0,4);
		$month	= substr($end_time,4,2);
		$day	= substr($end_time,6,2);
		$hour	= substr($end_time,8,2);
		$min	= substr($end_time,11,2);
		$e_trg 	= $year."-".$month."-".$day." ".$hour.":".$min;
		if(1>@strtotime($e_trg)){
			if(is_object($this->log)) $this->log->logPrint("[End]Not End Time Format[".$e_trg."]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		$dbs = new CONN;

		//計測中の累計雨量レコードを取得
		$strSql = "SELECT COUNT(*) FROM t500_dat WHERE t500_pnt_id = '".$pnt_id."' AND t500_sdt = '".$s_trg."';";
		if(is_object($this->log)) $this->log->logPrint(__FUNCTION__." ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		if(is_object($this->log)) $this->log->logPrint(__FUNCTION__." Rain Record Count[".$row[0]."]",REC_NOTE,__LINE__,__FILE__);

		if($row[0]>0){
			$strSql = "UPDATE t500_dat SET t500_value = ".$rain_rui.",t500_last = '".$e_trg."' WHERE t500_pnt_id = '".$pnt_id."' AND t500_sdt = '".$s_trg."';";
		}else{
			$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'".$pnt_id."','".$s_trg."','".$e_trg."','".$e_trg."',".$rain_rui.");";
		}
		if(is_object($this->log)) $this->log->logPrint(__FUNCTION__." ".$strSql,REC_NOTE,__LINE__,__FILE__);
		if(!$dbs->Execute($strSql)){
			if(is_object($this->log)) $this->log->logPrint(__FUNCTION__." ".$dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			if(is_object($this->log)) $this->log->logPrint("[End False]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		return true;
	}

	//ロガーから補填時にまとめてデータを連想配列へ取得する
	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

		//格納用連想配列初期化
		$this->AryColData = array();

		//連続取得制限期間（時間）
		$limit_hour = 9;

		//データ取得用配列を取得（初期化用）
		$data	= new Data;
		$AryRec	= $data->GetDataField("100");

		$buf = "";
		if($trg2!=""){
			//期間指定取得
			$time1 = DateConvert3($trg1);
			$time2 = DateConvert3($trg2);

			if(($time2 - $time1) > (3600*$limit_hour)){
				//取得期間が連続取得制限期間より大きい場合
				$flg = 1;
				while($flg){
					if(($time2 - $time1) > (3600*$limit_hour)){
						$tmp  = $this->SendCmd($this->setting["lgr_no"]."R3,".date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time1+3600*$limit_hour).chr(13).chr(10));
						$buf .= $tmp;
						$time1 = $time1 + (3600*$limit_hour) + 600;
					}else{
						$buf .= $this->SendCmd($this->setting["lgr_no"]."R3,".date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time2).chr(13).chr(10));
						$flg  = 0;
					}
					if(is_object($this->log)) $this->log->logPrint("[Wait 3 Sec]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
					sleep(1);
				}
			}else{
				//取得期間が連続取得制限期間より小さい場合
				$time1	 = DateConvert3($trg1);
				$time2	 = DateConvert3($trg2);
				$buf	 = $this->SendCmd($this->setting["lgr_no"]."R3,".date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time2).chr(13).chr(10));
			}
		}else{
			//単一期間指定
			$time1	= DateConvert3($trg1);
			$buf	= $this->SendCmd($this->setting["lgr_no"]."R3,".date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time1).chr(13).chr(10));
		}

		$AryRain = array();

		//取得データを解析する
		$AryTmp = explode(chr(13).chr(10),$buf);
		foreach($AryTmp as $val){
			$val = trim($val);
			if($val == "END" || $val=="") continue;

			//格納用に初期化用配列を代入する
			$args = $AryRec;

			//取得データをデータ格納用配列へ取得する
			$tmp = explode(",",$val);

			$AryData = $this->parseMinutelyData($tmp);
			foreach($this->OutField["100"] as $key){
				$args[$key] = $AryData[$key];
			}
			$args["mainte"] = $this->CheckHK($AryData["HK1"]);

			//取得データの処理日付を取得
			$trg = trim($tmp[1]);
			$trg = str_replace("/","",$trg);
			$trg = str_replace(":","",$trg);
			$trg = str_replace(" ","",$trg);
			if(is_numeric($trg)){
				$this->AryColData[$trg] = $args;
				$s_time = trim($AryData["rain_rui_stime"]);
				$s_chk = str_replace("/","",$s_time);
				$s_chk = str_replace(":","",$s_chk);
				$e_time = trim($AryData["rain_rui_etime"]);
				$e_chk = str_replace("/","",$e_time);
				$e_chk = str_replace(":","",$e_chk);

				if(is_numeric($s_chk)&&is_numeric($e_chk)){
					$AryRain["rain_rui"]	= $AryData["rain_rui"];
					$AryRain["s_time"]		= $s_time;
					$AryRain["e_time"]		= $e_time;
				}else{
					if(count($AryRain)>0){
						$this->SaveRuiRain($pnt_id,$AryRain["rain_rui"],$AryRain["s_time"],$AryRain["e_time"]);
						$AryRain = array();
					}

				}
			}
		}
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		return true;
	}

	//日報データ取得(共通タイプ）
	function GetDailyData($pnt_id,$curTime,&$args){
		//指定時刻 (XX-1):01 から XX:00 までの時報データを使用して日報データを作成
		$dbs = new CONN;

		$now		= DateConvert($curTime);
		$prev		= DateConvert($curTime-3600);
		$trg		= "t100_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["year"].$now["month"].$now["day"].$now["hour"]."00'";

		//10分最大雨量
		$rain = array();
		$strSql = "SELECT TO_NUMBER(max__move_rain_10min,'9999.9') as max_rain_10min FROM t100_dat_".$now["year"]." WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} AND mainte <> '1' ORDER BY rain_10min DESC,t100_date DESC LIMIT 1;";
		if(is_object($this->log))	$this->log->logPrint("////".__FUNCTION__."//// [".$strSql."]",REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		if(is_numeric($row[0]))	$rain["max_rain_10min"] = number_format($row[0], 1,".","");

		//最大瞬間風速データ取得
		$wind = array();
		$strSql = "SELECT TO_NUMBER(max_ws,'9999.9') as max_ws,max_wd FROM t100_dat_".$now["year"]." WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} AND mainte <> '1' ORDER BY max_ws DESC,t100_date DESC LIMIT 1;";
		if(is_object($this->log))	$this->log->logPrint("////".__FUNCTION__."//// [".$strSql."]",REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		if(is_numeric($row["max_ws"])){
			$wind["max_ws"] = number_format($row["max_ws"], 1,".","");
			$wind["max_wd"] = $row["max_wd"];
		}

		$strSql = "SELECT * FROM t100_dat_".$now["year"]." WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} AND mainte <> '1' ORDER BY t100_date DESC LIMIT 1;";
		if(is_object($this->log))	$this->log->logPrint("////".__FUNCTION__."//// [".$strSql."]",REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);

		foreach($this->OutField["200"] as $key){
			if(!array_key_exists($key,$args)) continue;

			switch($key){
				case "max_ws":
				case "max_wd":
					$args[$key] = $wind[$key];
					break;
				case "max_rain_10min":
					$args[$key] = $rain[$key];
					break;
				case "water_level":
				case "max__water":
				case "min__water":
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 2, ".", "");
					break;
				case "snow_level":
				case "snow_hour":
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 0, ".", "");
					break;
				default:
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 1, ".", "");
			}
		}
		if(is_object($this->log))	$this->log->logPrint($this->id."////".__FUNCTION__."//// ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//日報集計データ取得	[2006/12/29]
	function GetDailySumData($pnt_id,$curTime,&$args){
		//日付設定
		$date = DateConvert($curTime);
		$trg1	= $date["year"].$date["month"].$date["day"]."0000";
		$trg2	= $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];

		$dbs = new CONN;

		$ArySumFld = array(
			"avg_ws			[平均風速]平均値" 		=> array("40","avg_ws","100","avg_ws"),
			"avg_wd			[平均風向]平均値"		=> array("40","avg_wd","100","avg_wd"),
			"avg_ws			[平均風速]最高値" 		=> array("50","max__avg_ws","100","avg_ws"),
			"avg_ws			[平均風速]最高値起時" 	=> array("55","max__avg_ws","100","avg_ws"),
			"avg_wd			[平均風向]最高値"		=> array("50","max__avg_wd","100","avg_wd"),
			"max_ws			[最大風速]最高値" 		=> array("50","max_ws","100","max_ws"),
			"max_ws			[最大風速]最高値起時" 	=> array("55","max_ws","100","max_ws"),
			"max_wd			[最大風向]最高値"		=> array("50","max_wd","100","max_wd"),
			"temp			[気温]平均" 				=> array("40","temp","100","temp"),
			"temp  			[気温]最高値" 			=> array("50","max__temp","100","temp"),
			"temp  			[気温]最高値起時"		=> array("55","max__temp","100","temp"),
			"temp  			[気温]最低値" 			=> array("60","min__temp","100","temp"),
			"temp  			[気温]最低値起時"		=> array("65","min__temp","100","temp"),
			"humid1			[現地気圧]平均" 			=> array("40","humid1","100","humid1"),
			"humid1			[現地気圧]最高値" 		=> array("50","max__humid1","100","humid1"),
			"humid1			[現地気圧]最高値起時" 	=> array("55","max__humid1","100","humid1"),
			"humid1			[現地気圧]最低値" 		=> array("60","min__humid1","100","humid1"),
			"humid1			[現地気圧]最低値起時"		=> array("65","min__humid1","100","humid1"),
			"press1			[現地気圧]平均" 			=> array("40","press1","100","press1"),
			"press1			[現地気圧]最高値" 		=> array("50","max__press1","100","press1"),
			"press1			[現地気圧]最高値起時" 	=> array("55","max__press1","100","press1"),
			"press1			[現地気圧]最低値" 		=> array("60","min__press1","100","press1"),
			"press1			[現地気圧]最低値起時"		=> array("65","min__press1","100","press1"),
			"max_rain_10min [10分最大]最高値"		=> array("50","max__move_rain_10min",	"100","max_rain_10min"),
			"max_rain_10min [10分最大]最高値起時"	=> array("55","max__move_rain_10min",	"100","max_rain_10min"),
			"rain_hour [時間積算]最高値"				=> array("50","max__move_rain_hour",	"100","rain_hour"),
			"rain_hour [時間積算]最高値起時" 			=> array("55","max__move_rain_hour",	"100","rain_hour"),
			"rain_day [日積算]積算値"				=> array("70","rain_day",				"100","rain_day"),

			"snow_level [積雪深]最高値"				=> array("50","snow_level",	"100","snow_level"),
			"snow_level [積雪深]最高値起時"			=> array("55","snow_level",	"100","snow_level"),
			"snow_hour [時間降雪量]最高値"			=> array("50","snow_hour",	"100","snow_hour"),
			"snow_hour [時間降雪量]最高値起時" 		=> array("55","snow_hour",	"100","snow_hour"),
			"snow_hour [時間降雪量]積算値"			=> array("70","snow_hour",	"200","snow_hour"),

			"water_level	[外水位]平均" 		=> array("40","water_level","100","water_level"),
			"water_level	[外水位]最高値" 		=> array("50","max__water","100","max__water"),
			"water_level	[外水位]最高値起時" 	=> array("55","max__water","100","max__water"),
			"water_level	[外水位]最低値" 		=> array("60","min__water","100","min__water"),
			"water_level	[外水位]最低値起時"	=> array("65","min__water","100","min__water"),

			"風向頻度"							=> array("90","avg_ws",					"200","avg_ws")
		);

		foreach($ArySumFld as $name => $val){
			$sum_no = $val[0];	//集計No
			$fld	= $val[1];	//問合せ用フィールド
			$sys_no	= $val[2];	//問合せ用テーブル
			$fld_id	= $val[3];	//処理フィールド
//			print $name."\n";
//			print $sum_no." ";
//			print $fld." ";
//			print $sys_no." ";
//			print $fld_id."\n";

			//問合せSQL生成
			$tbl		= "t{$sys_no}_dat_".$date["year"];
			$fld_date	= "t{$sys_no}_date";
			$strWhere 	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code IS NULL AND mainte <> '1'";

			//ロガー出力項目チェック
			if(!in_array($fld_id,$this->OutField["200"]))	continue;
			if(!array_key_exists($fld_id,$args[$sum_no]))	continue;

			//集計データ取得処理
			if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== {$name}",REC_DEBUG,__LINE__,__FILE__);
			switch($sum_no){
				case "40"://平均値
					if(strpos(" ".$fld_id,"_wd")>0){
						//平均値（風向）
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' GROUP BY {$fld} ORDER BY COUNT(*) DESC LIMIT 2;";
						if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if($row[0]=="00"){
							$new = $dbs->Next();
							if($new[0]!=""){
								$row = $new;
								if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== Wind Dir [00] Next Record Read ==> [".$row[0]."]",REC_DEBUG,__LINE__,__FILE__);
							}
						}
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = $row[0];
					}else{
						//平均値（風向）
						$strSql = "SELECT AVG(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(strpos(" ".$fld_id,"water")>0){
							if(is_numeric($row[0])) $args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}else{
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					break;
				case "50":
					//最高値
					if($fld_id=="avg_wd"||$fld_id=="max_wd"){
						$tmp_fld = str_replace("wd","ws",$fld);
						$strSql = "SELECT {$fld},TO_NUMBER($tmp_fld,'9999.99') as {$tmp_fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' ORDER BY {$tmp_fld} DESC,t{$sys_no}_date DESC LIMIT 1;";
						if(is_object($this->log)) $this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row[0])){
							$args[$sum_no][$fld_id] = $row[0];
						}
					}else{
						$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log)) $this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(strpos(" ".$fld_id,"water")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}elseif(strpos(" ".$fld_id,"snow")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 0, ".", "");
						}else{
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					break;
				case "60":
					//最低値
					$strSql = "SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					if(is_object($this->log)) $this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					if(strpos(" ".$fld_id,"water")>0){
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
					}else{
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
					}
					break;
				case "55":
				case "65":
					//起時
					$sum = $sum_no - 5;
					if(is_numeric($args[$sum][$fld_id])){
//						if(strpos(" ".$fld_id,"rain")>0){
//							if($args[$sum][$fld_id]==0){
//								if(is_object($this->log)) $this->log->logPrint("雨量データ０ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
//								break;
//							}
//						}
						if($fld=="snow_level"||$fld=="snow_hour"){
							$fld_date = "t100_date";
							$strSql = "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY t{$sys_no}_date DESC LIMIT 1;";
							if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							$date = DateConvert2($row[$fld_date]);
							$row[$fld_date] = $date["hour"].":".$date["min"];
						}else{
							$fld_date = $fld."_time";
							if($fld=="max_ws") $fld_date = "max__".$fld_date;//無理やり
							$strSql = "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY t{$sys_no}_date DESC LIMIT 1;";;
							if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
						}

						//$date = DateConvert2($row[$fld_date]);
						//$args[$sum_no][$fld_id] = $date["hour"].":".$date["min"];
						$args[$sum_no][$fld_id] = $row[$fld_date];
					}
					break;
				case "70":
					//積算値
					$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					if($fld=="snow_hour"){
						$strWhere 	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
						$strSql = "SELECT SUM(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					}
					if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					$args[$sum_no][$fld_id] = $row[0];
					break;
				case "90":
					$args[$sum_no][$fld_id] = $this->GetDirection($pnt_id,"200",$curTime);
					break;
			}
			if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== [{$sum_no}][{$fld_id}] =>".$args[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
		}
		if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//ロガー電文送信
	function SendCmd($cmd="",$close=1){
		if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

		//コマンド種別取得
		$type = substr($cmd,2,2);

		//引数なしで呼び出した場合ファイルハンドルクローズ
		if($cmd==""&&$close){
			if($this->fp) {
				if (is_object($this->log)) $this->log->logPrint("close", REC_SOCK, __LINE__, __FILE__);
				@fclose($this->fp);
				$this->fp = false;
			}
			if(is_object($this->log)) $this->log->logPrint("[Exit True]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return true;
		}

		//ファイルハンドルが既にオープンされているかチェック
		if(!$this->fp){
			$ip		=	$this->setting["lgr_ip"];
			$port   =   $this->setting["lgr_port"];

			$address = "tcp://{$ip}:{$port}";
			$this->fp = @stream_socket_client($address,$err_no,$err_str,5);
			if($this->fp){
				@stream_set_timeout($this->fp,2);
				if(is_object($this->log))	$this->log->logPrint("open:{$ip}:{$port}",REC_SOCK,__LINE__,__FILE__);
			}else{
				if(is_object($this->log))	$this->log->logPrint("open-error:IP={$ip}:{$port}:ERR_NO={$err_no}:ERR_STR={$err_str}",REC_SOCK,__LINE__,__FILE__);
				return false;
			}
		}

		//電文送信 ※各クラスで主にこの部分がちがう可能性あり
		if(is_object($this->log)) $this->log->logPrint("send:".$cmd,REC_SOCK,__LINE__,__FILE__);
		fwrite($this->fp, $cmd);
		$flg		= 1;
		$buf		= "";
		$prev		= "";
		$byt_cnt	= 0;
		$debug_ord  = "";//Debug
		$debug_chr  = "";//Debug
		while( (!feof($this->fp) && $flg)){
			$tmp  = fgetc($this->fp);
			$buf  .= $tmp;
			$prev .= $tmp;
			if($tmp==chr(10)){
				if($type=="R3"){
					if(trim($prev)=="END"){
						$flg = 0;
					}else{
						fputs($this->fp,"OK".chr(13).chr(10));
						$prev = "";
					}
				}else{
					$flg = 0;
				}
			}
			$debug_ord .= "[".ord($tmp)."]";//Debug
			$debug_chr .= "[".$tmp."]";		//Debug
			$byt_cnt++;
			if($byt_cnt>100000){
				if(is_object($this->log))	$this->log->logPrint("send:Error Over Flow[{$byt_cnt}]",REC_SOCK,__LINE__,__FILE__);
				break;
			}
		}
		if(is_object($this->log)) $this->log->logPrint("debug_ord:[{$debug_ord}]",REC_DEBUG,__LINE__,__FILE__);	//Debug
		if(is_object($this->log)) $this->log->logPrint("debug_chr:[{$debug_chr}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

		if(is_object($this->log)) $this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__,0,0,array("EUC-JP","SJIS"));

		//クローズ処理
		if($close){
			if(is_object($this->log)) $this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
		}
		if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		return $buf;
	}

	public function ResetRainRui($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$Point = new Points;
		$info = $Point->LoadPointInfo($pnt_id);

		$reset_rain = $info["pnt_tmg_rain"];
		if(!is_numeric($reset_rain)){
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Setting Non A Numeric Error] value[".$reset_rain."]",REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		if($reset_rain>24){
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Setting Over Value Error] value[".$reset_rain."]",REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		// $head = sprintf("%02d",$this->setting["lgr_no"])."S1,";
		// if(!$buf=$this->SendCmd($head.sprintf("%02d",$reset_rain).",".chr(13).chr(10))){
		if(!$buf=$this->SendCmd("int:Ra:No_Rain_Time ".sprintf("%02d",$reset_rain).",".chr(13).chr(10))){
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Send Command Error]",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);

		if($buf==="OK"){
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Result OK]",REC_NOTE,__LINE__,__FILE__);
			$flg = true;
		}else{
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Result NG]",REC_NOTE,__LINE__,__FILE__);
			$flg = false;
		}

		$buf = $this->GetResetRainRui($pnt_id);
		$time = str_replace("hour","",trim($buf));
		if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Result (".$time.")]",REC_NOTE,__LINE__,__FILE__);
		// ロガー累計雨量リセット設置値を記録
		$rain_path = ROOT_INC."rain_reset_".$pnt_id.".dat";
		$rain_buf = date("Y/m/d H:i").",".$time;
		@file_put_contents($rain_path,$rain_buf);

		return $flg;
	}

	public function GetResetRainRui($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		
		if(!$buf=$this->SendCmd("int:Ra:No_Rain_Time?".chr(13).chr(10))){
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Get Reset Rain Send Command Error]",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);

		// if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Result (".$buf.")]",REC_NOTE,__LINE__,__FILE__);

		return $buf;
	}

}
?>

