<?
//============================================================//
//	WU-900ロガークラス																				//
//------------------------------------------------------------//
//	処理概要																									//
//	　WU-900用																								//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/12/21	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class LoggerWU900B extends LoggerWU900
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2007.10.15";
	var $memo			= "天気計からＦＴＰにて天気取得タイプ";
	var $id				= "WU900B";
	var $type			= "1";														//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
	var $ReCal		= "1";														//再計算処理可否
	var $map			= array("lgr_id"				=> "lgr_id",
		"lgr_pnt_id"		=> "lgr_pnt_id",
		"lgr_enable"		=> "lgr_enable",
		"lgr_time"			=> "lgr_time",
		"lgr_ip"				=> "lgr_col_1",
		"lgr_port"			=> "lgr_col_2",
		"lgr_humid"			=> "lgr_col_3",
		"lgr_alt"				=> "lgr_col_4",
		"lgr_ftp_ip"		=> "lgr_col_5",
		"lgr_ftp_id"		=> "lgr_col_6",
		"lgr_ftp_pwd"		=> "lgr_col_7",
		"lgr_conv_ip"		=> "lgr_col_8",
		"lgr_conv_port"	=> "lgr_col_9",
		"lgr_conv_tmg"	=> "lgr_col_10"
	);

	var $ftp;					//FTPオブジェクト
	var $weather_err;	//天気取得時エラーステータス
	var $hard_err;		//天気計用ハードエラーステータス
	var $site_err;		//天気計用視程エラーステータス


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

		$this->setting["tmg_".$this->setting["lgr_conv_tmg"]] = "selected";

		$this->load_flg = 1;
		return $this->setting;
	}

	//現在データ取得処理 [2007/01/16]
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd("PT,".date("Ym",$curTime).",".date("dHi",$curTime).",")){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf		= $this->ClearBuf($buf);
		$AryTmp =	$this->ClearBuf2($buf);

		if(trim($AryTmp[3])=="80"||trim($AryTmp[3])=="10"||trim($AryTmp[3])=="20") return true;

		//日付チェック
		$chkDate = date("YmdHi",$curTime);
		if($AryTmp[1].$AryTmp[2]==$chkDate){
			if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp[4])		? number_format($AryTmp[4],	1, ".", "")		: "");
			if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp[5])		? sprintf("%02d",$AryTmp[5]) 							: "");
			if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	(is_numeric($AryTmp[6])		? number_format($AryTmp[6],	1, ".", "")  	: "");
			if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	(is_numeric($AryTmp[7])		? sprintf("%02d",$AryTmp[7])							: "");
			if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp[8]) 	? number_format($AryTmp[8],	1, ".", "")  	: "");
			if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp[9]) 	? number_format($AryTmp[9], 1, ".", "")  	: "");
			if(array_key_exists("humid2",$args))			$args["humid2"]			=	(is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],1, ".", "")  	: "");
			if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],1, ".", "")  	: "");
			if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp[12]) 	? number_format($AryTmp[12],1, ".", "")  	: "");
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($AryTmp[13]) 	? number_format($AryTmp[13],1, ".", "")  	: "");
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($AryTmp[14]) 	? number_format($AryTmp[14],1, ".", "")  	: "");
			if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	(is_numeric($AryTmp[15]) 	? number_format($AryTmp[15],1, ".", "")  	: "");
			//移動積算取得
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);
			if(array_key_exists("rain_1min",$args))				$args["rain_1min"]				= $AryMove["rain_1min"];
			if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	= $AryMove["move_rain_10min"];
			if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		= $AryMove["move_rain_hour"];
			if(array_key_exists("move_rain_day",$args))		$args["move_rain_day"]		= $AryMove["move_rain_day"];
			//1分積算をもとに積算データを再取得
			$rain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($rain["rain_10min"]) 		? number_format($rain["rain_10min"],1, ".", "")  : "");
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($rain["rain_hour"]) 		? number_format($rain["rain_hour"],	1, ".", "")  : "");
			if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	(is_numeric($rain["rain_day"]) 			? number_format($rain["rain_day"],	1, ".", "")  : "");
			//累計雨量算出＆記録処理
			if(array_key_exists("rain_rui",$args))	$args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
			//天気取得
			if(array_key_exists("weather",$args))		$args["weather"]	= $this->GetWeather($pnt_id);
			if($this->weather_err!=""){
				if(is_object($this->log))	$this->log->logPrint("天気取得情報 [".$this->weather_err."]",REC_SOCK,__LINE__,__FILE__);
			}

		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//ＦＴＰオブジェクト生成 [2007/10/15]
	function CreateFTP($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$ip		= $this->setting["lgr_ftp_ip"];
		$id 	=	$this->setting["lgr_ftp_id"];
		$pwd	=	$this->setting["lgr_ftp_pwd"];

		//FTP接続
		$this->ftp = new ftp();
		//$this->ftp->debug			= TRUE;
		$this->ftp->timeout		= 10;
		$this->ftp->stimeout	= 2;

		if(is_object($this->log))	$this->log->logPrint("FTP Connect Trying",REC_DEBUG,__LINE__,__FILE__);
		if($this->ftp->ftp_connect($ip)){
			if(is_object($this->log))	$this->log->logPrint("FTP Connect",REC_DEBUG,__LINE__,__FILE__);
			if($this->ftp->ftp_login($id,$pwd)){
				if(is_object($this->log))	$this->log->logPrint("FTP Login Current => ".$this->ftp->ftp_pwd(),REC_DEBUG,__LINE__,__FILE__);
			}else{
				if(is_object($this->log))	$this->log->logPrint("FTP Login Error",REC_NOTE,__LINE__,__FILE__);
				return false;
			}
		}else{
			if(is_object($this->log))	$this->log->logPrint("FTP Connect Error",REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		return true;
	}

	//天気情報を天気計から取得	[2007/10/15]
	function GetWeather($pnt_id,$mset=0,$ftp_ip="",$ftp_id="",$ftp_pwd=""){
		if($mset){
			$this->setting["lgr_ftp_ip"]	=	$ftp_ip;
			$this->setting["lgr_ftp_id"]	= $ftp_id;
			$this->setting["lgr_ftp_pwd"]	= $ftp_pwd;
			$this->load_flg = 1;
		}else{
			if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		}

		if($this->CreateFTP($pnt_id)){
			$localfile	= ROOT_INC."{$pnt_id}_weather.dat";
			$remotefile	= "weather.dat";
			@unlink($localfile);
			if($this->ftp->ftp_get($localfile, $remotefile)){
				$this->ftp->ftp_quit();

				//天気コード取得
				$line	=	@file($localfile);
				if(is_array($line)){
					$AryTmp = explode(",",$line[0]);
					if(is_array($AryTmp)){
						//日付チェック
						$year		= substr($AryTmp[0],0,4);
						$month	= substr($AryTmp[0],4,2);
						$day		= substr($AryTmp[0],6,2);
						$t			= strtotime("{$year}/{$month}/{$day} ".$AryTmp[1]);
						$t1 		= time()-300;
						$t2 		= time()+300;
						if($t>$t1&&$t<$t2){
							$code = "00";
							switch($AryTmp[4]){
								case 12: $code = "02"; break;
								case 14: $code = "04"; break;
								case 17: $code = "08"; break;
								case 18: $code = "09"; break;
								case 19: $code = "10"; break;
								case 21: $code = "13"; break;
								case 22: $code = "14"; break;
								case 23: $code = "16"; break;
							}
							$this->SetWeather($pnt_id,$code);

							//天気計ハードウェアステータス記録
							$this->hard_err = "";
							switch(substr($AryTmp[5],0,1)){
								case 1:	$this->hard_err = "ハードウェアーエラー";			break;
								case 2:	$this->hard_err = "ハードウェアーワーニング";	break;
								case 3:	$this->hard_err = "後方散乱光警告";						break;
							}
							$this->site_err = "";
							switch(substr($AryTmp[5],1,1)){
								case 1:	$this->site_err = "視程アラーム１";	break;
								case 2:	$this->site_err = "視程アラーム２";	break;
								case 3:	$this->site_err = "視程アラーム３";	break;
							}
							if($this->hard_err!=""||$this->site_err!=""){
								if(is_object($this->log))	$this->log->logPrint("天気計ステータス　ハードエラー[".$this->hard_err."] 視程エラー[".$this->site_err."]",REC_SOCK,__LINE__,__FILE__);
							}
							return sprintf("%02d",$code);
						}else{
							$this->weather_err = "取得データの時刻が一致していません";
							$this->SetWeather($pnt_id,"00");
							return "00";
						}
					}else{
						$this->weather_err = "取得データの時刻が正しくありません";
						return "00";
					}
				}else{
					$this->weather_err = "天気情報が取得できません";
					return "00";
				}
			}else{
				$this->weather_err = "天気情報が取得できません";
				$this->ftp->ftp_quit();
				return "00";
			}
		}else{
			$this->weather_err = "接続できません";
			$this->ftp->ftp_quit();
			return "00";
		}
	}



	//天気情報を天気計から取得	[2007/10/15]
	function PutWeatherData($pnt_id,$curTime){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$ip		= $this->setting["lgr_conv_ip"];
		$port = $this->setting["lgr_conv_port"];
		$tmg	= $this->setting["lgr_conv_tmg"];

		if(!CheckStart($tmg,$curTime)) return false;
		if(is_object($this->log))	$this->log->logPrint($this->id." PutWeatherData Start",REC_DEBUG,__LINE__,__FILE__);


		//現在データ取得
		$data			= new Data($log);
		$AryTmp		= $data->LoadData($pnt_id,"000",$curTime);
		$date			= DateConvert($curTime);
		$AryData	=	$AryTmp[ $date["date"] ];

		if(!is_array($AryData)){
			if(is_object($this->log))	$this->log->logPrint($this->id." PutWeatherData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}

		if(is_object($this->log))	$this->log->logPrint($this->id." PutWeatherData Data[".print_r($AryData,TRUE)."]",REC_DEBUG,__LINE__,__FILE__);


		$AryTmp = array();
		$AryTmp[] = chr(160);
		$AryTmp[] = "01";
		$AryTmp[] = $AryData["avg_wd"];
		$AryTmp[] = sprintf("%03d",str_replace(".","",$AryData["max_ws"]));
		$AryTmp[] = sprintf("%03d",str_replace(".","",$AryData["avg_ws"]));
		$AryTmp[] = sprintf("%04d",floor($AryData["press1"]));
		$tmp = str_replace(".","",$AryData["temp"]);
		if($tmp<0){
			$AryTmp[] = "-".sprintf("%03d",$tmp);
		}else{
			$AryTmp[] = sprintf("%04d",$tmp);
		}
		$AryTmp[] = sprintf("%03d",str_replace(".","",$AryData["humid1"]));
		$AryTmp[] = sprintf("%03d",str_replace(".","",$AryData["humid2"]));
		$AryTmp[] = sprintf("%04d",str_replace(".","",$AryData["rain_day"]));

		$code = "";
		switch($AryData["weather"]){
			case "02": $code = "12"; break;
			case "04": $code = "14"; break;
			case "08": $code = "17"; break;
			case "09": $code = "18"; break;
			case "10": $code = "19"; break;
			case "13": $code = "21"; break;
			case "14": $code = "22"; break;
			case "16": $code = "23"; break;
		}
		$AryTmp[] = $code;
		$AryTmp[] = "";

		$buf = join("/",$AryTmp);
		if(is_object($this->log))	$this->log->logPrint($this->id." PutWeatherData PutData[{$buf}]",REC_SOCK,__LINE__,__FILE__);

		$fp = @stream_socket_client("tcp://{$ip}:{$port}", $errno, $errstr, 5);
		if($fp){
			@stream_set_timeout($fp, 2);
			if(is_object($this->log))	$this->log->logPrint("open:{$ip}:{$port}",																				REC_SOCK,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		//電文送信 ※各クラスで主にこの部分がちがう可能性あり
		$cmd = chr(02).$buf.chr(03);

//			for($i=0;$i<strlen($cmd);$i++){
//				if(is_object($this->log))	$this->log->logPrint("[".ord(substr($cmd,$i,1))."]",REC_DEBUG,__LINE__,__FILE__);
//			}

		if(is_object($this->log))	$this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
		@fwrite($fp,$cmd);
		@fclose($fp);
		return true;
	}

}
?>