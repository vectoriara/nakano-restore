<?
//============================================================//
//	CK-4100ロガークラス																				//
//------------------------------------------------------------//
//	処理概要																									//
//	　CK-4100用																								//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/10/01	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class LoggerCK4100 extends BaseLogger
{
	//変数宣言
	var $log;
	var $varsion	= "1.0";
	var $update		= "2006.10.26";
	var $memo			= "";
	var $id				= "CK4100" ;

	//とりえあず今回のみロガーに閾値設定追加
	var $alert_w				= 0;
	var $alert_w_value	= "";
	var $alert_r				= 0;
	var $alert_r_value	= "";

	var $map	= array("lgr_id"				=> "lgr_id",
		"lgr_pnt_id"		=> "lgr_pnt_id",
		"lgr_enable"		=> "lgr_enable",
		"lgr_ip"				=> "lgr_col_1",
		"lgr_tcp_port"	=> "lgr_col_2",
		"lgr_udp_port"	=> "lgr_col_3",
		"lgr_file_dir"	=> "lgr_col_4",
		"lgr_file_id"		=> "lgr_col_5",
		"lgr_alert_wind1"	=> "lgr_col_6",
		"lgr_alert_wind2"	=> "lgr_col_7",
		"lgr_alert_rain1"	=> "lgr_col_8",
		"lgr_alert_rain2"	=> "lgr_col_9"
	);

	//通信チェック
	function CheckConnection($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		//過去データ取得にて通信チェックする
		return $this->SendCmd("01R2",date("Y/m/d H:i",time()-60).",".date("Y/m/d H:i",time()-60));
	}

	//時刻補正
	function SetDate($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$fp = fsockopen($this->setting["lgr_ip"],$this->setting["lgr_tcp_port"], $errno, $errstr,10);
		if($fp){
			//接続成功
			if(is_object($this->log))	$this->log->logPrint("open:".$this->setting["lgr_ip"].":".$this->setting["lgr_tcp_port"],REC_SOCK,__LINE__,__FILE__);
			//電文送信
			$strCmd = "01C1,".date("Y/m/d H:i:s").chr(13).chr(10);
			fputs($fp, $strCmd);
			if(is_object($this->log))	$this->log->logPrint("send:{$strCmd}",REC_SOCK,__LINE__,__FILE__);
			//電文受信
			$rvalue = fread($fp, 30);
			if(is_object($this->log))	$this->log->logPrint("recv:{$rvalue}",REC_SOCK,__LINE__,__FILE__);
			if(substr($rvalue,0,2)=="OK"){
				$result = "時刻補正が完了しました[".$this->id."]";
			}else{
				$result = "時刻補正に失敗しました[".$this->id."]";
			}
			fclose($fp);
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("open-error:IP=".$this->setting["lgr_ip"].":".$this->setting["lgr_tcp_port"].":ERRNO=".$errno.":ERRSTR=".$errstr,REC_SOCK,__LINE__,__FILE__);
		}
		return $result;
	}


	//現在データ取得処理
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$file = $this->setting["lgr_file_dir"].$this->setting["lgr_file_id"]."_present_now.dat";

		if($AryTmp = @file($file)){
			if(is_object($this->log))	$this->log->logPrint("Target File Open [{$file}]",REC_NOTE,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("Target File Open Error [{$file}]",REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		if(is_object($this->log))	$this->log->logPrint($AryTmp[0],REC_SOCK,__LINE__,__FILE__);

		$AryTmp = explode(",",str_replace("\n","",$AryTmp[0]));

		//日付チェック
		$date = DateConvert($curTime);
		$AryTmp[0] = substr(trim($AryTmp[0]),0,16);

		//$chkDate = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		$chkDate = date("Y/m/d H:i",$curTime);

		if($AryTmp[0]==$chkDate){
			//欠測データをスペースに変換
			for($i=3;$i<count($AryTmp);$i++) $AryTmp[$i] = str_replace("/","",$AryTmp[$i]);

			if(array_key_exists("wd_inst",$args))				$args["wd_inst"]			=	$AryTmp[3];
			if(array_key_exists("ws_inst",$args))				$args["ws_inst"]			=	(is_numeric($AryTmp[4]) ? number_format($AryTmp[4] / 10, 1, ".", "") : "");
			if(array_key_exists("ccw_wd_1min",$args))		$args["ccw_wd_1min"]	=	$AryTmp[5];
			if(array_key_exists("avg_wd_1min",$args))		$args["avg_wd_1min"]	=	$AryTmp[6];
			if(array_key_exists("cw_wd_1min",$args))		$args["cw_wd_1min"]		=	$AryTmp[7];
			if(array_key_exists("max_ws_1min",$args))		$args["max_ws_1min"]	=	(is_numeric($AryTmp[8]) ? number_format($AryTmp[8] / 10, 1, ".", "")  : "");
			if(array_key_exists("avg_ws_1min",$args))		$args["avg_ws_1min"]	=	(is_numeric($AryTmp[9]) ? number_format($AryTmp[9] / 10, 1, ".", "")  : "");
			if(array_key_exists("min_ws_1min",$args))		$args["min_ws_1min"]	=	(is_numeric($AryTmp[10]) ? number_format($AryTmp[10] / 10, 1, ".", "")  : "");
			if(array_key_exists("ccw_wd_10min",$args))	$args["ccw_wd_10min"]	=	$AryTmp[11];
			if(array_key_exists("avg_wd_10min",$args))	$args["avg_wd_10min"]	=	$AryTmp[12];
			if(array_key_exists("cw_wd_10min",$args))		$args["cw_wd_10min"]	=	$AryTmp[13];
			if(array_key_exists("max_ws_10min",$args))	$args["max_ws_10min"]	=	(is_numeric($AryTmp[14]) ? number_format($AryTmp[14] / 10, 1, ".", "")  : "");
			if(array_key_exists("avg_ws_10min",$args))	$args["avg_ws_10min"]	=	(is_numeric($AryTmp[15]) ? number_format($AryTmp[15] / 10, 1, ".", "")  : "");
			if(array_key_exists("min_ws_10min",$args))	$args["min_ws_10min"]	=	(is_numeric($AryTmp[16]) ? number_format($AryTmp[16] / 10, 1, ".", "")  : "");
			if(array_key_exists("temp",$args))					$args["temp"]					=	(is_numeric($AryTmp[18]) ? number_format($AryTmp[18] / 10, 1, ".", "")  : "");

			if(array_key_exists("temp",$args)){
				$max_temp_1min = (is_numeric($AryTmp[25]) ? number_format($AryTmp[25] / 10, 1, ".", "")  : "");
			}

			if(array_key_exists("temp",$args)){
				$min_temp_1min = (is_numeric($AryTmp[26]) ? number_format($AryTmp[26] / 10, 1, ".", "")  : "");
			}

			if(array_key_exists("rehumid",$args))				$args["rehumid"]			=	(is_numeric($AryTmp[20]) ? number_format($AryTmp[20] / 10, 1, ".", "")  : "");
			if(array_key_exists("move_rain",$args))			$args["move_rain"]		=	(is_numeric($AryTmp[22]) ? number_format($AryTmp[22] / 10, 1, ".", "")  : "");
			if(array_key_exists("total_rain",$args))		$args["total_rain"]		=	(is_numeric($AryTmp[24]) ? number_format($AryTmp[24] / 10, 1, ".", "")  : "");

			//現在データ追加データ取得
			$trg = $date["year"].$date["month"].$date["day"]."%";
			$year = $date["year"];
			//日最大時間雨量
			if(array_key_exists("max_move_rain",$args)){
				$strSql = "SELECT MAX(to_number(move_rain,'999.9')) as max FROM t100_dat_{$year} WHERE t100_pnt_id = '$pnt_id' AND t100_date LIKE '$trg' AND move_rain <> '';";
				$dbs = new CONN;
				$row=$dbs->Query($strSql);
				$args["max_move_rain"] = ($args["move_rain"] > $row["max"] ? $args["move_rain"]	: $row["max"]);
			}
			//日最高気温
			if(array_key_exists("max_temp",$args)){
				$strSql = "SELECT MAX(to_number(temp,'999.9')) as max FROM t100_dat_{$year} WHERE t100_pnt_id = '$pnt_id' AND t100_date LIKE '$trg' AND temp <> '';";
				if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
				$dbs = new CONN;
				$row=$dbs->Query($strSql);
				if(is_object($this->log))	$this->log->logPrint("max =>".$row["max"],REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("temp =>{$max_temp_1min}",REC_DEBUG,__LINE__,__FILE__);
				//$args["max_temp"] = ($args["temp"] > $row["max"] ? $args["temp"]	: $row["max"]);
				$args["max_temp"] = ($max_temp_1min > $row["max"] ? $max_temp_1min	: $row["max"]);
			}
			//日最低気温
			if(array_key_exists("min_temp",$args)){
				$strSql = "SELECT MIN(to_number(temp,'999.9')) as min FROM t100_dat_{$year} WHERE t100_pnt_id = '$pnt_id' AND t100_date LIKE '$trg' AND temp <> '';";
				if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
				$dbs = new CONN;
				$row=$dbs->Query($strSql);
				//$args["min_temp"] = ($args["temp"] > $row["min"] ? $row["min"]	: $args["temp"]);
				$args["min_temp"] = ($min_temp_1min > $row["min"] ? $row["min"]	: $min_temp_1min);
				if(is_object($this->log))	$this->log->logPrint("min =>".$row["min"],REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("temp =>{$min_temp_1min}",REC_DEBUG,__LINE__,__FILE__);
				if(!is_numeric($row["min"]))	$args["min_temp"] = $args["temp"];
			}

			//時報用風速風向データ取得
			if(array_key_exists("avg_ws",$args))	$args["avg_ws"] = (is_numeric($AryTmp[15]) ?  number_format($AryTmp[15] / 10, 1, ".", "") : "");
			if(array_key_exists("avg_wd",$args))	$args["avg_wd"] =	GetDirectionCode($AryTmp[12],$AryTmp[15]);
			if(array_key_exists("max_ws",$args))	$args["max_ws"] = (is_numeric($AryTmp[8]) ?  number_format($AryTmp[8] / 10, 1, ".", "") : "");
			if(array_key_exists("max_wd",$args))	$args["max_wd"] = GetDirectionCode($AryTmp[6],$AryTmp[8]);


			//閾値判断
			$val					= $args["avg_ws_1min"] + 0;
			$alert_value1 = $this->setting["lgr_alert_wind1"] + 0;
			$alert_value2 = $this->setting["lgr_alert_wind2"] + 0;
			if($val >= $alert_value2){
				$this->alert_w				= 2;
				$this->alert_w_value	= "風速警報発生";
			}else{
				if($val >= $alert_value1){
					$this->alert_w				= 1;
					$this->alert_w_value	= "風速注意報発生";
				}
			}
			//閾値判断
			$val					= $args["move_rain"] + 0;
			$alert_value1 = $this->setting["lgr_alert_rain1"] + 0;
			$alert_value2 = $this->setting["lgr_alert_rain2"] + 0;
			if($val >= $alert_value2){
				$this->alert_w				= 2;
				$this->alert_w_value	= "雨量警報発生";
			}else{
				if($val >= $alert_value1){
					$this->alert_r				= 1;
					$this->alert_r_value	= "雨量注意報発生";
				}
			}
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}

		if(is_object($this->log))	$this->log->logPrint($this->id." ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}
	//時報データ取得処理
//		function GetMinutelyData($pnt_id,$curTime,&$args){
//			return $this->GetPresentData($pnt_id,$curTime,$args);
//		}
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$file = $this->setting["lgr_file_dir"].$this->setting["lgr_file_id"]."_present_now.dat";

		if($AryTmp = @file($file)){
			if(is_object($this->log))	$this->log->logPrint("Target File Open [{$file}]",REC_NOTE,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("Target File Open Error [{$file}]",REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		if(is_object($this->log))	$this->log->logPrint($AryTmp[0],REC_SOCK,__LINE__,__FILE__);

		$AryTmp = explode(",",str_replace("\n","",$AryTmp[0]));

		//日付チェック
		$date = DateConvert($curTime);
		$AryTmp[0] = substr(trim($AryTmp[0]),0,16);

		//$chkDate = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		$chkDate = date("Y/m/d H:i",$curTime);

		if($AryTmp[0]==$chkDate){
			//欠測データをスペースに変換
			for($i=3;$i<count($AryTmp);$i++) $AryTmp[$i] = str_replace("/","",$AryTmp[$i]);

			if(array_key_exists("wd_inst",$args))				$args["wd_inst"]			=	$AryTmp[3];
			if(array_key_exists("ws_inst",$args))				$args["ws_inst"]			=	(is_numeric($AryTmp[4]) ? number_format($AryTmp[4] / 10, 1, ".", "") : "");
			if(array_key_exists("ccw_wd_1min",$args))		$args["ccw_wd_1min"]	=	$AryTmp[5];
			if(array_key_exists("avg_wd_1min",$args))		$args["avg_wd_1min"]	=	$AryTmp[6];
			if(array_key_exists("cw_wd_1min",$args))		$args["cw_wd_1min"]		=	$AryTmp[7];
			if(array_key_exists("max_ws_1min",$args))		$args["max_ws_1min"]	=	(is_numeric($AryTmp[8]) ? number_format($AryTmp[8] / 10, 1, ".", "")  : "");
			if(array_key_exists("avg_ws_1min",$args))		$args["avg_ws_1min"]	=	(is_numeric($AryTmp[9]) ? number_format($AryTmp[9] / 10, 1, ".", "")  : "");
			if(array_key_exists("min_ws_1min",$args))		$args["min_ws_1min"]	=	(is_numeric($AryTmp[10]) ? number_format($AryTmp[10] / 10, 1, ".", "")  : "");
			if(array_key_exists("ccw_wd_10min",$args))	$args["ccw_wd_10min"]	=	$AryTmp[11];
			if(array_key_exists("avg_wd_10min",$args))	$args["avg_wd_10min"]	=	$AryTmp[12];
			if(array_key_exists("cw_wd_10min",$args))		$args["cw_wd_10min"]	=	$AryTmp[13];
			if(array_key_exists("max_ws_10min",$args))	$args["max_ws_10min"]	=	(is_numeric($AryTmp[14]) ? number_format($AryTmp[14] / 10, 1, ".", "")  : "");
			if(array_key_exists("avg_ws_10min",$args))	$args["avg_ws_10min"]	=	(is_numeric($AryTmp[15]) ? number_format($AryTmp[15] / 10, 1, ".", "")  : "");
			if(array_key_exists("min_ws_10min",$args))	$args["min_ws_10min"]	=	(is_numeric($AryTmp[16]) ? number_format($AryTmp[16] / 10, 1, ".", "")  : "");

			//１分最大気温
			if(array_key_exists("temp",$args))					$args["temp"]					=	(is_numeric($AryTmp[25]) ? number_format($AryTmp[25] / 10, 1, ".", "")  : "");

			if(array_key_exists("temp",$args)){
				$max_temp_1min = (is_numeric($AryTmp[25]) ? number_format($AryTmp[25] / 10, 1, ".", "")  : "");
			}

			if(array_key_exists("rehumid",$args))				$args["rehumid"]			=	(is_numeric($AryTmp[20]) ? number_format($AryTmp[20] / 10, 1, ".", "")  : "");
			if(array_key_exists("move_rain",$args))			$args["move_rain"]		=	(is_numeric($AryTmp[22]) ? number_format($AryTmp[22] / 10, 1, ".", "")  : "");
			if(array_key_exists("total_rain",$args))		$args["total_rain"]		=	(is_numeric($AryTmp[24]) ? number_format($AryTmp[24] / 10, 1, ".", "")  : "");

			//現在データ追加データ取得
			$trg = $date["year"].$date["month"].$date["day"]."%";
			$year = $date["year"];
			//日最大時間雨量
			if(array_key_exists("max_move_rain",$args)){
				$strSql = "SELECT MAX(to_number(move_rain,'999.9')) as max FROM t100_dat_{$year} WHERE t100_pnt_id = '$pnt_id' AND t100_date LIKE '$trg' AND move_rain <> '';";
				$dbs = new CONN;
				$row=$dbs->Query($strSql);
				$args["max_move_rain"] = ($args["move_rain"] > $row["max"] ? $args["move_rain"]	: $row["max"]);
			}
			//日最高気温
			if(array_key_exists("max_temp",$args)){
				$strSql = "SELECT MAX(to_number(temp,'999.9')) as max FROM t100_dat_{$year} WHERE t100_pnt_id = '$pnt_id' AND t100_date LIKE '$trg' AND temp <> '';";
				if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
				$dbs = new CONN;
				$row=$dbs->Query($strSql);
				if(is_object($this->log))	$this->log->logPrint("max =>".$row["max"],REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("temp =>".$args["temp"],REC_DEBUG,__LINE__,__FILE__);
				//$args["max_temp"] = ($args["temp"] > $row["max"] ? $args["temp"]	: $row["max"]);
				$args["max_temp"] = ($max_temp_1min > $row["max"] ? $max_temp_1min	: $row["max"]);
			}
			//日最低気温
			if(array_key_exists("min_temp",$args)){
				$strSql = "SELECT MIN(to_number(temp,'999.9')) as min FROM t100_dat_{$year} WHERE t100_pnt_id = '$pnt_id' AND t100_date LIKE '$trg' AND temp <> '';";
				if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
				$dbs = new CONN;
				$row=$dbs->Query($strSql);
				//$args["min_temp"] = ($args["temp"] > $row["min"] ? $row["min"]	: $args["temp"]);
				$args["min_temp"] = ($max_temp_1min > $row["min"] ? $row["min"]	: $max_temp_1min);
				if(is_object($this->log))	$this->log->logPrint("min =>".$row["min"],REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("temp =>".$args["temp"],REC_DEBUG,__LINE__,__FILE__);
				if(!is_numeric($row["min"]))	$args["min_temp"] = $args["temp"];
			}

			//時報用風速風向データ取得
			if(array_key_exists("avg_ws",$args))	$args["avg_ws"] = (is_numeric($AryTmp[15]) ?  number_format($AryTmp[15] / 10, 1, ".", "") : "");
			if(array_key_exists("avg_wd",$args))	$args["avg_wd"] =	GetDirectionCode($AryTmp[12],$AryTmp[15]);
			if(array_key_exists("max_ws",$args))	$args["max_ws"] = (is_numeric($AryTmp[8]) ?  number_format($AryTmp[8] / 10, 1, ".", "") : "");
			if(array_key_exists("max_wd",$args))	$args["max_wd"] = GetDirectionCode($AryTmp[6],$AryTmp[8]);


			//閾値判断
			$val					= $args["avg_ws_1min"] + 0;
			$alert_value1 = $this->setting["lgr_alert_wind1"] + 0;
			$alert_value2 = $this->setting["lgr_alert_wind2"] + 0;
			if($val >= $alert_value2){
				$this->alert_w				= 2;
				$this->alert_w_value	= "風速警報発生";
			}else{
				if($val >= $alert_value1){
					$this->alert_w				= 1;
					$this->alert_w_value	= "風速注意報発生";
				}
			}
			//閾値判断
			$val					= $args["move_rain"] + 0;
			$alert_value1 = $this->setting["lgr_alert_rain1"] + 0;
			$alert_value2 = $this->setting["lgr_alert_rain2"] + 0;
			if($val >= $alert_value2){
				$this->alert_w				= 2;
				$this->alert_w_value	= "雨量警報発生";
			}else{
				if($val >= $alert_value1){
					$this->alert_r				= 1;
					$this->alert_r_value	= "雨量注意報発生";
				}
			}
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}

		if(is_object($this->log))	$this->log->logPrint($this->id." ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//局のロガー設定を保存
	function SaveLoggerInfo($args){
		$AryTmp = array();

		foreach($this->map as $key => $val){
			if($val=="lgr_enable"){
				$AryTmp[] = "$val = ".($args[$key]=="" ? 0 : 1)."";
			}else{
				if(strpos(" ".$key,"lgr_alert_value")>0){
					$AryTmp[] = "$val = '".number_format($args[$key]+0, 2, ".", "")."'";
				}else{
					$AryTmp[] = "$val = '".$args[$key]."'";
				}
			}
		}

		$strSql = "UPDATE t_lgrdat SET ".join(",",$AryTmp)." WHERE lgr_id = '".$args["lgr_id"]."' AND lgr_pnt_id = '".$args["lgr_pnt_id"]."';";

		if(is_object($this->log))	$this->log->logPrint($strSql,REC_NOTE,__LINE__,__FILE__);

		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			$this->load_flg = 0;
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		if(is_object($this->log))	$this->log->logPrint("GetLoggerMinutelyCollection",REC_NOTE,__LINE__,__FILE__);
		if($trg2!=""){
			$time1 = DateConvert3($trg1);
			$time2 = DateConvert3($trg2);
			if(($time2 - $time1) > (3600*9)){
				$flg = 1;
				while($flg){
					if(($time2 - $time1) > (3600*9)){
						$tmp = $this->SendCmd("01R2",date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time1+3600*9));
						$buf  .= $tmp;
						$time1 = $time1 + (3600*9) + 60;
					}else{
						$buf .= $this->SendCmd("01R2",date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time2));
						$flg  = 0;
					}
				}
				//指定日時が10時間以上の場合はきざんで処理する（一応こわいから9時間に・・しとこ
			}else{
				$time1 = DateConvert3($trg1);
				$time2 = DateConvert3($trg2);
				$buf = $this->SendCmd("01R2",date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time2));
			}
		}else{
			$time1 = DateConvert3($trg1);
			$buf = $this->SendCmd("01R2",date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time1));
		}
		//$AryTmp = explode(chr(13).chr(10),$buf);
		$AryTmp = explode(chr(13).chr(10),$buf);

		$AryData = array();
		foreach($AryTmp as $val){
			$val = trim($val);
			if($val=="") continue;			//空白は処理しない
			$buf = explode(",",$val);
			if(strtotime($buf[1])==-1){
				continue;	//日付でない場合も処理しない
			}else{
				$date = DateConvert(strtotime($buf[1]));
			}
			//欠測データ変換
			$buf = explode(",",str_replace("/","",$val));

			if(trim($buf[3])=="N"){
				$rec = array();
				$rec["wd_inst"]				=	$buf[4];
				$rec["ws_inst"]				=	(is_numeric($buf[5]) ? number_format($buf[5] / 10, 1, ".", "") : "");
				$rec["ccw_wd_1min"]		=	$buf[6];
				$rec["avg_wd_1min"]		=	$buf[7];
				$rec["cw_wd_1min"]		=	$buf[8];
				$rec["max_ws_1min"]		=	(is_numeric($buf[9])  ? number_format($buf[9] / 10, 1, ".", "")  : "");
				$rec["avg_ws_1min"]		=	(is_numeric($buf[10]) ? number_format($buf[10] / 10, 1, ".", "")  : "");
				$rec["min_ws_1min"]		=	(is_numeric($buf[11]) ? number_format($buf[11] / 10, 1, ".", "")  : "");
				$rec["ccw_wd_10min"]	=	$buf[12];
				$rec["avg_wd_10min"]	=	$buf[13];
				$rec["cw_wd_10min"]		=	$buf[14];
				$rec["max_ws_10min"]	=	(is_numeric($buf[15]) ? number_format($buf[15] / 10, 1, ".", "")  : "");
				$rec["avg_ws_10min"]	=	(is_numeric($buf[16]) ? number_format($buf[16] / 10, 1, ".", "")  : "");
				$rec["min_ws_10min"]	=	(is_numeric($buf[17]) ? number_format($buf[17] / 10, 1, ".", "")  : "");
				$rec["temp"]					=	(is_numeric($buf[35]) ? number_format($buf[35] / 10, 1, ".", "")  : "");
				$rec["rehumid"]				=	(is_numeric($buf[37]) ? number_format($buf[37] / 10, 1, ".", "")  : "");
				$rec["move_rain"]			=	(is_numeric($buf[39]) ? number_format($buf[39] / 10, 1, ".", "")  : "");
				$rec["total_rain"]		=	(is_numeric($buf[41]) ? number_format($buf[41] / 10, 1, ".", "")  : "");
				$rec["avg_ws"] 				= (is_numeric($buf[16]) ? number_format($buf[16] / 10, 1, ".", "") : "");
				$rec["avg_wd"] 				=	GetDirectionCode($buf[13],$buf[16]);
				$rec["max_ws"] 				= (is_numeric($buf[9]) ?  number_format($buf[9] / 10, 1, ".", "") : "");
				$rec["max_wd"] 				= GetDirectionCode($buf[7],$buf[9]);
				$AryData["001"][ $date["date"] ]	= $rec;
			}

			if(trim($buf[19])=="N"){
				$rec = array();
				$rec["wd_inst"]				=	$buf[20];
				$rec["ws_inst"]				=	(is_numeric($buf[21]) ? number_format($buf[21] / 10, 1, ".", "") : "");
				$rec["ccw_wd_1min"]		=	$buf[22];
				$rec["avg_wd_1min"]		=	$buf[23];
				$rec["cw_wd_1min"]		=	$buf[24];
				$rec["max_ws_1min"]		=	(is_numeric($buf[25]) ? number_format($buf[25] / 10, 1, ".", "")  : "");
				$rec["avg_ws_1min"]		=	(is_numeric($buf[26]) ? number_format($buf[26] / 10, 1, ".", "")  : "");
				$rec["min_ws_1min"]		=	(is_numeric($buf[27]) ? number_format($buf[27] / 10, 1, ".", "")  : "");
				$rec["ccw_wd_10min"]	=	$buf[28];
				$rec["avg_wd_10min"]	=	$buf[29];
				$rec["cw_wd_10min"]		=	$buf[30];
				$rec["max_ws_10min"]	=	(is_numeric($buf[31]) ? number_format($buf[31] / 10, 1, ".", "")  : "");
				$rec["avg_ws_10min"]	=	(is_numeric($buf[32]) ? number_format($buf[32] / 10, 1, ".", "")  : "");
				$rec["min_ws_10min"]	=	(is_numeric($buf[33]) ? number_format($buf[33] / 10, 1, ".", "")  : "");
				$rec["temp"]					=	"";
				$rec["rehumid"]				=	"";
				$rec["move_rain"]			=	"";
				$rec["total_rain"]		=	"";
				$rec["avg_ws"] 				= (is_numeric($buf[32]) ?  number_format($buf[32] / 10, 1, ".", "") : "");
				$rec["avg_wd"] 				=	GetDirectionCode($buf[29],$buf[32]);
				$rec["max_ws"] 				= (is_numeric($buf[25]) ?  number_format($buf[25] / 10, 1, ".", "") : "");
				$rec["max_wd"] 				= GetDirectionCode($buf[23],$buf[25]);
				$AryData["002"][ $date["date"] ]	= $rec;
			}
		}
		return $AryData;
	}

	function GetMinutelyCollectionData($pnt_id,$curTime,&$args,&$Data){
		$date = DateConvert($curTime);
		$trg	= $date["date"];
		$rec = $Data[$pnt_id][$trg];

		if(array_key_exists("wd_inst",$args))				$args["wd_inst"]			=	$rec["wd_inst"];
		if(array_key_exists("ws_inst",$args))				$args["ws_inst"]			=	$rec["ws_inst"];
		if(array_key_exists("ccw_wd_1min",$args))		$args["ccw_wd_1min"]	=	$rec["ccw_wd_1min"];
		if(array_key_exists("avg_wd_1min",$args))		$args["avg_wd_1min"]	=	$rec["avg_wd_1min"];
		if(array_key_exists("cw_wd_1min",$args))		$args["cw_wd_1min"]		=	$rec["cw_wd_1min"];
		if(array_key_exists("max_ws_1min",$args))		$args["max_ws_1min"]	=	$rec["max_ws_1min"];
		if(array_key_exists("avg_ws_1min",$args))		$args["avg_ws_1min"]	=	$rec["avg_ws_1min"];
		if(array_key_exists("min_ws_1min",$args))		$args["min_ws_1min"]	=	$rec["min_ws_1min"];
		if(array_key_exists("ccw_wd_10min",$args))	$args["ccw_wd_10min"]	=	$rec["ccw_wd_10min"];
		if(array_key_exists("avg_wd_10min",$args))	$args["avg_wd_10min"]	=	$rec["avg_wd_10min"];
		if(array_key_exists("cw_wd_10min",$args))		$args["cw_wd_10min"]	=	$rec["cw_wd_10min"];
		if(array_key_exists("max_ws_10min",$args))	$args["max_ws_10min"]	=	$rec["max_ws_10min"];
		if(array_key_exists("avg_ws_10min",$args))	$args["avg_ws_10min"]	=	$rec["avg_ws_10min"];
		if(array_key_exists("min_ws_10min",$args))	$args["min_ws_10min"]	=	$rec["min_ws_10min"];
		if(array_key_exists("temp",$args))					$args["temp"]					=	$rec["temp"];
		if(array_key_exists("rehumid",$args))				$args["rehumid"]			=	$rec["rehumid"];
		if(array_key_exists("move_rain",$args))			$args["move_rain"]		=	$rec["move_rain"];
		if(array_key_exists("total_rain",$args))		$args["total_rain"]		=	$rec["total_rain"];
		//時報用風速風向データ取得
		if(array_key_exists("avg_ws",$args))	$args["avg_ws"] = $rec["avg_ws"];
		if(array_key_exists("avg_wd",$args))	$args["avg_wd"] =	$rec["avg_wd"];
		if(array_key_exists("max_ws",$args))	$args["max_ws"] = $rec["max_ws"];
		if(array_key_exists("max_wd",$args))	$args["max_wd"] = $rec["max_wd"];
		return true;
	}

	function SendCmd($cmd,$data=""){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$ip		= $this->setting["lgr_ip"];
		$port =	$this->setting["lgr_tcp_port"];

		$fp = @stream_socket_client("tcp://{$ip}:{$port}", $errno, $errstr, 30);
		if(!$fp){
			if(is_object($this->log))	$this->log->logPrint("open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}",REC_SOCK,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("open:{$ip}:{$port}",REC_SOCK,__LINE__,__FILE__);
			//電文送信
			$cmd = $this->CreateCmd($cmd,$data);
			if(is_object($this->log))	$this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
			fwrite($fp, $cmd);
			$result = "";
			$flg = 1;
			while( (!feof($fp)) && $flg){
				$tmp = fgetc($fp);

				//if($l=="")	if(is_object($this->log))	$this->log->logPrint("first recv {$tmp}",REC_DEBUG,__LINE__,__FILE__);

				$result .= $tmp;
				if($tmp==chr(10)){
					if(strpos(" ".$l,"END")>0){
						$flg = 0;
					}
					if(strpos(" ".$l,"NG")>0){
						$flg = 0;
					}
					if(strpos(" ".$l,"OK")>0){
						$flg = 0;
					}
					//if(is_object($this->log))	$this->log->logPrint("recv line:{$l}",REC_DEBUG,__LINE__,__FILE__);
					fwrite($fp, "OK".chr(13).chr(10));
					$l="";
				}else{
					//ライン記録
					$l .= $tmp;
				}
			}
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			fclose($fp);
		}
		if(is_object($this->log))	$this->log->logPrint("recv:[{$result}]",REC_SOCK,__LINE__,__FILE__);
		return $result;
	}

	function CreateCmd($cmd,$data=""){
		return $cmd.",".$data.chr(13).chr(10);
	}
}
?>