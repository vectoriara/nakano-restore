<?
//==========================================================//
//	CP-7100ロガークラス										//
//----------------------------------------------------------//
//	処理概要												//
//	　CP-7100用												//
//----------------------------------------------------------//
//	変更履歴												//
//		2006/11/11	N.ARAI									//
//		・新規作成											//
//==========================================================//
class LoggerCP7100 extends BaseLogger
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2007.01.30";
	var $memo		= "";
	var $id			= "CP7100";
	var $type		= "1";	//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
	var $ReCal		= "0";	//再計算処理可否
	var $map		= array(
		"lgr_id"		=> "lgr_id",
		"lgr_pnt_id"	=> "lgr_pnt_id",
		"lgr_enable"	=> "lgr_enable",
		"lgr_time"		=> "lgr_time",
		"lgr_ip"		=> "lgr_col_1",
		"lgr_port"		=> "lgr_col_2",
	);
	var $EditSys	= array();							//編集可能なデータ帳票
	var $ColSys		= array("100","200","300","400");	//手動収集可能なデータ帳票
	var $DelSys		= array("100","200","300","400");	//削除可能なデータ帳票

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
		"000" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2",
			"rain_10min",
			"rain_hour",
			"rain_day",
			"move_rain_10min",
			"move_rain_hour",
			"move_rain_day",
			"rain_1min",
			"weather",
			"rain_rui"
		),
		"100" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2",
			"rain_10min",
			"rain_hour",
			"rain_day"
		),
		"200" => array(
			"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2",
			"max_rain_10min",
			"rain_hour",
			"rain_day",
			"weather"
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
			"humid2",
			"press1",
			"avg_press1",
			"max_press1",
			"min_press1",
			"press2",
			"avg_press2",
			"max_press2",
			"min_press2",
			"max_rain_10min",
			"max_rain_hour",
			"rain_day",
			"weather1",
			"weather2",
			"weather3"
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
			"humid2",
			"avg_humid2",
			"max_humid2",
			"min_humid2",
			"press1",
			"avg_press1",
			"max_press1",
			"min_press1",
			"press2",
			"avg_press2",
			"max_press2",
			"min_press2",
			"max_rain_10min",
			"max_rain_hour",
			"max_rain_day",
			"rain_month"
		)
	);

	//帳票毎でこのロガーが編集できる項目を定義
	var $EditField = array();

	//通信チェック [2007/01/30]
	function CheckConnection($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		$buf = $this->SendCmd("D00000000000000");
		$buf = $this->ClearBuf($buf);
		return $buf;
	}

	//時刻補正 [2007/01/30]
	function SetDate($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$buf = $this->SendCmd("T".date("YmdHis"));
			if($buf==chr(6)){
				if(is_object($this->log)) $this->log->logPrint("時刻補正が完了しました[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return 1;
			}else{
				if(is_object($this->log)) $this->log->logPrint("時刻補正に失敗しました[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return false;
			}
		}else{
			if(is_object($this->log)) $this->log->logPrint("時刻補正は許可されていません[{$this->id}]",REC_DEBUG,__LINE__,__FILE__);
			return 2;
		}
	}

	//現在データ取得処理 [2007/01/30]
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd("D00000000000000")){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//日付チェック
		$chkDate = date("Ymd,H:i",$curTime);
		if(strpos(" ".$buf,$chkDate)>0){
			if(array_key_exists("avg_ws",$args))		$args["avg_ws"]		=	(is_numeric($AryTmp[2])		? number_format($AryTmp[2],	1, ".", "")	: "");
			if(array_key_exists("avg_wd",$args))		$args["avg_wd"]		=	(is_numeric($AryTmp[3])		? sprintf("%02d",$AryTmp[3])						: "");
			if(array_key_exists("max_ws",$args))		$args["max_ws"]		=	(is_numeric($AryTmp[4])		? number_format($AryTmp[4],	1, ".", "") : "");
			if(array_key_exists("max_wd",$args))		$args["max_wd"] 	=	(is_numeric($AryTmp[5])		? sprintf("%02d",$AryTmp[5])						: "");
			if(array_key_exists("temp",$args))			$args["temp"]		=	(is_numeric($AryTmp[6]) 	? number_format($AryTmp[6],	1, ".", "") : "");
			if(array_key_exists("humid1",$args))		$args["humid1"]		=	(is_numeric($AryTmp[7]) 	? number_format($AryTmp[7], 1, ".", "") : "");
			if(array_key_exists("humid2",$args))		$args["humid2"]		=	(is_numeric($AryTmp[8]) 	? number_format($AryTmp[8], 1, ".", "")	: "");
			if(array_key_exists("press1",$args))		$args["press1"]		=	(is_numeric($AryTmp[9]) 	? number_format($AryTmp[9],	1, ".", "") : "");
			if(array_key_exists("press2",$args))		$args["press2"]		=	(is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],1, ".", "") : "");
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],1, ".", "") : "");
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($AryTmp[12]) 	? number_format($AryTmp[12],1, ".", "") : "");
			if(array_key_exists("rain_day",$args))		$args["rain_day"]	=	(is_numeric($AryTmp[13])	? number_format($AryTmp[13],1, ".", "") : "");
			//移動積算取得
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);
			if(array_key_exists("rain_1min",$args))			$args["rain_1min"]			= $AryMove["rain_1min"];
			if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	= $AryMove["move_rain_10min"];
			if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		= $AryMove["move_rain_hour"];
			if(array_key_exists("move_rain_day",$args))		$args["move_rain_day"]		= $AryMove["move_rain_day"];
			//累計雨量算出＆記録処理
			if(array_key_exists("rain_rui",$args)) $args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
			//天気取得
			if(array_key_exists("weather",$args)) $args["weather"] = $this->GetWeather($pnt_id);
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//時報データ取得処理 [2007/01/30]
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$date = DateConvert($curTime);
		if(!$buf=$this->SendCmd("A".$date["year"].$date["month"].$date["day"].$date["hour"].$date["min"]."00")){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetMinutelyData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//日付チェック
		$chkDate = $date["hour"].":".$date["min"];
		if($AryTmp[0]==$chkDate){
			if(array_key_exists("avg_ws",$args))	$args["avg_ws"]	=	(is_numeric($AryTmp[2])		? number_format($AryTmp[2],	1, ".", "")  : "");
			if(array_key_exists("avg_wd",$args))	$args["avg_wd"]	=	(is_numeric($AryTmp[3])		? $AryTmp[3] : "");
			if(array_key_exists("max_ws",$args))	$args["max_ws"]	=	(is_numeric($AryTmp[4])		? number_format($AryTmp[4],	1, ".", "")  : "");
			if(array_key_exists("max_wd",$args))	$args["max_wd"] =	(is_numeric($AryTmp[5])		? $AryTmp[5] : "");
			if(array_key_exists("temp",$args))		$args["temp"]	=	(is_numeric($AryTmp[6])		? number_format($AryTmp[6],	1, ".", "")  : "");
			if(array_key_exists("humid1",$args))	$args["humid1"]	=	(is_numeric($AryTmp[7])		? number_format($AryTmp[7], 1, ".", "")  : "");
			if(array_key_exists("humid2",$args))	$args["humid2"]	=	(is_numeric($AryTmp[8])		? number_format($AryTmp[8], 1, ".", "")  : "");
			if(array_key_exists("press1",$args))	$args["press1"]	=	(is_numeric($AryTmp[9])		? number_format($AryTmp[9],	1, ".", "")  : "");
			if(array_key_exists("press2",$args))	$args["press2"]	=	(is_numeric($AryTmp[10])	? number_format($AryTmp[10],1, ".", "")  : "");

			//積算データ取得
			$AryRain = $this->GetRain($pnt_id,$curTime,$AryTmp[11]);
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	= $AryRain["rain_10min"];
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	= $AryRain["rain_hour"];
			if(array_key_exists("rain_day",$args))		$args["rain_day"]	= $AryRain["rain_day"];
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//ロガーから指定日の日報データを取得し連想配列に格納 [2007/01/30]
	function GetDailyLoggerData($pnt_id,$curTime){
		if($curTime > time()) return false;
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		//電文作成
		$date = DateConvert($curTime);
		$cmd = "D".$date["year"].$date["month"].$date["day"]."000000";

		//同じ電文の送信抑制
		if($this->SendCmdCnt[$cmd] > 0){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetDailyLoggerData SendCmd Exists [{$cmd}]",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}

		//データ取得
		if(!$buf=$this->SendCmd($cmd,0)){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetDailyLoggerData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(!$this->execute_collection) $this->SendCmd();	//手動収集時以外はクローズ
		if($buf==chr(4)||$buf==chr(21)||$buf==chr(6)) return false;

		$buf = $this->ClearBuf($buf);

		//同じ電文の送信抑制
		$this->SendCmdCnt[$cmd] = $this->SendCmdCnt[$cmd] + 1;

		$AryTmp = explode("\r",$buf);

		$tmp_sum = "";
		foreach($AryTmp as $val){
			$key = "";
			$args = array();

			//日付データ補正
			$val = str_replace(" 00:00"," 24:00",$val);

			$tmp = explode(",",trim($val));
			if(is_numeric(substr($tmp[0],0,2))){
				$key = $tmp[0];
			}else{
				switch(mb_convert_encoding($tmp[0],"EUC-JP","SJIS")){
					case "平均":
						$key = "40";
						break;
					case "最高":
						$key = "50";
						$tmp_sum = "最高";
						break;
					case "最低":
						$key = "60";
						$tmp_sum = "最低";
						break;
					case "起時":
						if($tmp_sum=="最高") $key = "55";
						if($tmp_sum=="最低") $key = "65";
						break;
					case "積算":
						$key = "70";
						break;
					case "頻度":
						$key = "90";
						break;
				}
			}

			if($key!=""){
				switch($key){
					case "55":
					case "65":
						$args["avg_ws"]			=	(trim($tmp[1])	?	substr(trim($tmp[1]),11,5)	:	"");
						$args["avg_wd"]			=	(trim($tmp[2])	?	substr(trim($tmp[2]),11,5)	:	"");
						$args["max_ws"]			=	(trim($tmp[3])	?	substr(trim($tmp[3]),11,5)	:	"");
						$args["max_wd"] 		=	(trim($tmp[4])	?	substr(trim($tmp[4]),11,5)	:	"");
						$args["temp"]			=	(trim($tmp[5]) 	?	substr(trim($tmp[5]),11,5)	:	"");
						$args["humid1"]			=	(trim($tmp[6]) 	?	substr(trim($tmp[6]),11,5)	:	"");
						$args["humid2"]			=	(trim($tmp[7]) 	?	substr(trim($tmp[7]),11,5)	:	"");
						$args["press1"]			=	(trim($tmp[8]) 	?	substr(trim($tmp[8]),11,5)	:	"");
						$args["press2"]			=	(trim($tmp[9]) 	?	substr(trim($tmp[9]),11,5)	:	"");
						$args["max_rain_10min"]	=	(trim($tmp[10]) ?	substr(trim($tmp[10]),11,5)	:	"");
						$args["rain_hour"]		=	(trim($tmp[11]) ?	substr(trim($tmp[11]),11,5)	:	"");
						$args["rain_day"]		=	(trim($tmp[12])	?	substr(trim($tmp[12]),11,5)	:	"");
						break;
					case "90":
						//風向頻度
						$args["avg_ws"]	= substr(trim($val),5);
						break;
					default:
						$args["avg_ws"]			=	(is_numeric($tmp[1])	? number_format($tmp[1],	1, ".", "")	: "");
						$args["avg_wd"]			=	(is_numeric($tmp[2])	? sprintf("%02d",$tmp[2])	: "");
						$args["max_ws"]			=	(is_numeric($tmp[3])	? number_format($tmp[3],	1, ".", "")	: "");
						$args["max_wd"] 		=	(is_numeric($tmp[4])	? sprintf("%02d",$tmp[4])	: "");
						$args["temp"]			=	(is_numeric($tmp[5]) 	? number_format($tmp[5],	1, ".", "")	: "");
						$args["humid1"]			=	(is_numeric($tmp[6]) 	? number_format($tmp[6], 	1, ".", "")	: "");
						$args["humid2"]			=	(is_numeric($tmp[7]) 	? number_format($tmp[7], 	1, ".", "")	: "");
						$args["press1"]			=	(is_numeric($tmp[8]) 	? number_format($tmp[8],	1, ".", "")	: "");
						$args["press2"]			=	(is_numeric($tmp[9]) 	? number_format($tmp[9],	1, ".", "")	: "");
						$args["max_rain_10min"]	=	(is_numeric($tmp[10])	? number_format($tmp[10],	1, ".", "")	: "");
						$args["rain_hour"]		=	(is_numeric($tmp[11]) 	? number_format($tmp[11],	1, ".", "")	: "");
						$args["rain_day"]		=	(is_numeric($tmp[12]) 	? number_format($tmp[12],	1, ".", "")	: "");

						if(strpos(" ".$key,":")>0){
							$key = str_replace(":","",$key);

							if($this->weather_out){
								//ロガーから天気が取得可能な場合
								$args["weather"] = $AryTmp[13];
							}else{
								//ロガーから天気が取得できない場合
								$args["weather"] = $this->GetWeatherData($pnt_id,DateConvert3($date["year"].$date["month"].$date["day"].$key));
							}
						}
				}

				switch($key){
					case "40":
					case "50":
					case "55":
					case "60":
					case "65":
					case "70":
					case "80":
					case "90":
						$this->AryColSumData[ $date["year"].$date["month"].$date["day"] ][$key] = $args;
						break;
					default:
						$this->AryColData[ $date["year"].$date["month"].$date["day"].$key ] = $args;
						break;
				}
			}
		}
		//if(is_object($this->log)) $this->log->logPrint($this->id." GetDailyLoggerData this->AryColData\n".print_r($this->AryColData,TRUE),			REC_DEBUG,__LINE__,__FILE__,0,1);
		//if(is_object($this->log)) $this->log->logPrint($this->id." GetDailyLoggerData this->AryColSumData\n".print_r($this->AryColSumData,TRUE),REC_DEBUG,__LINE__,__FILE__,0,1);
		return true;
	}
	//日報データ取得 [2007/01/31]
	function GetDailyData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		$trg = $date["date"];
		if(!is_array($this->AryColData[$trg])) $this->GetDailyLoggerData($pnt_id,$curTime);

		if(is_array($this->AryColData[$trg])){
			$AryTmp = $this->AryColData[$trg];
			foreach($this->OutField["200"] as $key){
				if(!array_key_exists($key,$args))	continue;
				if(!array_key_exists($key,$AryTmp)) continue;
				$args[$key] = $AryTmp[$key];
			}
			return true;
		}else{
			if(is_object($this->log)) $this->log->logPrint($this->id." GetDailyData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}
	//日報データ集計値取得 [2007/01/31]
	function GetDailySumData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		$trg = $date["year"].$date["month"].$date["day"];
		if(!is_array($this->AryColSumData[$trg]))	$this->GetDailyLoggerData($pnt_id,$curTime);

		if(is_array($this->AryColSumData[$trg])){
			$AryTmp = $this->AryColSumData[$trg];

			foreach($args as $sum_no => $val){
				if($sum_no=="90"){
					$args[$sum_no]["avg_ws"] = $AryTmp[$sum_no]["avg_ws"];
				}else{
					foreach($this->OutField["200"] as $key){
						if(!array_key_exists($key,$args[$sum_no]))		continue;
						if(!array_key_exists($key,$AryTmp[$sum_no]))	continue;
						$args[$sum_no][$key] = $AryTmp[$sum_no][$key];
					}
				}
			}
			return true;
		}else{
			if(is_object($this->log)) $this->log->logPrint($this->id." GetDailySumData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}

	//ロガーから指定月の月報データを取得し連想配列に格納 [2007/01/30]
	function GetMonthlyLoggerData($pnt_id,$curTime){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		//手動収集の場合はコネクションを維持するように設定
		if($this->execute_collection){
			$close_flg = 0;
		}else{
			$close_flg = 1;
		}

		//電文作成
		$date = DateConvert($curTime);
		$cmd	= "D".$date["year"].$date["month"]."00000000";

		//同じ電文の送信抑制
		if($this->SendCmdCnt[$cmd] > 0){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetMonthlyLoggerData SendCmd Exists [{$cmd}]",REC_DEBUG,__LINE__,__FILE__);
			return true;
		}

		//データ取得
		if(!$buf=$this->SendCmd($cmd,$close_flg)){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetMonthlyLoggerData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);

		//同じ電文の送信抑制
		$this->SendCmdCnt[$cmd] = $this->SendCmdCnt[$cmd] + 1;

		$AryTmp = explode("\r",$buf);

		$tmp_sum = "";

		//天気情報と集計データ取得用にデータクラス定義
		$cls = new Data($this->log);
		foreach($AryTmp as $val){
			$key	= "";
			$args = array();

			//日付データ補正
			$val = str_replace(" 00:00"," 24:00",$val);

			$tmp = explode(",",trim($val));
			if(is_numeric(substr($tmp[0],0,2))){
				$key = $tmp[0];
			}else{
				switch(mb_convert_encoding ($tmp[0],"EUC-JP","SJIS")){
					case "平均":
						$key = "40";
						break;
					case "最高":
						$key = "50";
						$tmp_sum = "最高";
						break;
					case "最低":
						$key = "60";
						$tmp_sum = "最低";
						break;
					case "起時":
						if($tmp_sum=="最高") $key = "55";
						if($tmp_sum=="最低") $key = "65";
						break;
					case "積算":
						$key = "70";
						break;
					case "日数":
						$key = "80";
						break;
					case "頻度":
						$key = "90";
						break;
				}
			}

			if($key!=""){
				switch($key){
					case "55":
					case "65":
						$args["avg_ws"]			=	(trim($tmp[1])	? substr(trim($tmp[1]),5,11)	:	"");
						$args["avg_wd"]			=	(trim($tmp[2])	? substr(trim($tmp[2]),5,11)	:	"");
						$args["max_ws"]			=	(trim($tmp[3])	? substr(trim($tmp[3]),5,11)	:	"");
						$args["max_wd"] 		=	(trim($tmp[4])	? substr(trim($tmp[4]),5,11)	:	"");
						$args["avg_temp"]		=	(trim($tmp[5]) 	? substr(trim($tmp[5]),5,11)	:	"");
						$args["max_temp"]		=	(trim($tmp[6]) 	? substr(trim($tmp[6]),5,11)	:	"");
						$args["min_temp"]		=	(trim($tmp[7]) 	? substr(trim($tmp[7]),5,11)	:	"");
						$args["avg_humid1"]		=	(trim($tmp[8]) 	? substr(trim($tmp[8]),5,11)	:	"");
						$args["max_humid1"]		=	(trim($tmp[9]) 	? substr(trim($tmp[9]),5,11)	:	"");
						$args["min_humid1"]		=	(trim($tmp[10])	? substr(trim($tmp[10]),5,11)	:	"");
						$args["humid2"]			=	(trim($tmp[11]) ? substr(trim($tmp[11]),5,11)	:	"");
						$args["press1"]			=	(trim($tmp[12]) ? substr(trim($tmp[12]),5,11)	:	"");
						$args["press2"]			=	(trim($tmp[13]) ? substr(trim($tmp[13]),5,11)	:	"");
						$args["max_rain_10min"]	=	(trim($tmp[14]) ? substr(trim($tmp[14]),5,11)	:	"");
						$args["max_rain_hour"]	=	(trim($tmp[15]) ? substr(trim($tmp[15]),5,11)	:	"");
						$args["rain_day"]		=	(trim($tmp[16])	? substr(trim($tmp[16]),5,5)	:	"");
						break;
					case "80":
						$args["max_temp"]	=	(is_numeric($tmp[6]) 	? $tmp[6]	: "");
						$args["min_temp"]	=	(is_numeric($tmp[7]) 	? $tmp[7]	: "");
						$args["rain_day"]	=	(is_numeric($tmp[16])	? $tmp[16]	: "");
						//日数
						break;
					case "90":
						//風向頻度
						$args["avg_ws"] = substr(trim($val),5);
						break;
					default:
						$args["avg_ws"]			=	(is_numeric($tmp[1])	? number_format($tmp[1],1, ".", "") : "");
						$args["avg_wd"]			=	(is_numeric($tmp[2])	? sprintf("%02d",$tmp[2])	: "");
						$args["max_ws"]			=	(is_numeric($tmp[3])	? number_format($tmp[3],1, ".", "") : "");
						$args["max_wd"] 		=	(is_numeric($tmp[4])	? sprintf("%02d",$tmp[4]) 	: "");
						$args["avg_temp"]		=	(is_numeric($tmp[5]) 	? number_format($tmp[5],1, ".", "") : "");
						$args["max_temp"]		=	(is_numeric($tmp[6]) 	? number_format($tmp[6],1, ".", "") : "");
						$args["min_temp"]		=	(is_numeric($tmp[7]) 	? number_format($tmp[7],1, ".", "") : "");
						$args["avg_humid1"]		=	(is_numeric($tmp[8]) 	? number_format($tmp[8],1, ".", "") : "");
						$args["max_humid1"]		=	(is_numeric($tmp[9]) 	? number_format($tmp[9],1, ".", "") : "");
						$args["min_humid1"]		=	(is_numeric($tmp[10])	? number_format($tmp[10],1, ".", "") : "");
						$args["humid2"]			=	(is_numeric($tmp[11]) 	? number_format($tmp[11],1, ".", "") : "");
						$args["press1"]			=	(is_numeric($tmp[12]) 	? number_format($tmp[12],1, ".", "") : "");
						$args["press2"]			=	(is_numeric($tmp[13]) 	? number_format($tmp[13],1, ".", "") : "");
						$args["max_rain_10min"]	=	(is_numeric($tmp[14]) 	? number_format($tmp[14],1, ".", "") : "");
						$args["max_rain_hour"]	=	(is_numeric($tmp[15]) 	? number_format($tmp[15],1, ".", "") : "");
						$args["rain_day"]		=	(is_numeric($tmp[16]) 	? number_format($tmp[16],1, ".", "") : "");

						if($key < 40){
							if(time() > strtotime($date["year"]."/".$date["month"]."/".$key." 00:01") + 86400){
								$AryData = $cls->LoadSumData($pnt_id,"200",strtotime($date["year"]."/".$date["month"]."/".$key." 00:01"));
								$args["avg_press1"]	=	(is_numeric($AryData[40]["press1"])	? number_format($AryData[40]["press1"],1, ".", "")  : "");
								$args["max_press1"]	=	(is_numeric($AryData[50]["press1"])	? number_format($AryData[50]["press1"],1, ".", "")  : "");
								$args["min_press1"]	=	(is_numeric($AryData[60]["press1"])	? number_format($AryData[60]["press1"],1, ".", "")  : "");
								$args["avg_press2"]	=	(is_numeric($AryData[40]["press2"])	? number_format($AryData[40]["press2"],1, ".", "")  : "");
								$args["max_press2"]	=	(is_numeric($AryData[50]["press2"])	? number_format($AryData[50]["press2"],1, ".", "")  : "");
								$args["min_press2"]	=	(is_numeric($AryData[60]["press2"])	? number_format($AryData[60]["press2"],1, ".", "")  : "");
								if($this->weather_out){
									//ロガーから天気が取得可能な場合
									$args["weather1"] = $AryTmp[17];
									$args["weather2"] = $AryTmp[18];
									$args["weather3"] = $AryTmp[19];
								}else{
									//ロガーから天気が取得できない場合
									$AryData = $this->GetWeatherData2($pnt_id,strtotime($date["year"]."/".$date["month"]."/".$key." 00:01"));
									$args["weather1"] = $AryData["weather1"];
									$args["weather2"] = $AryData["weather2"];
									$args["weather3"] = $AryData["weather3"];
								}
							}
						}
				}

				switch($key){
					case "40":
					case "50":
					case "55":
					case "60":
					case "65":
					case "70":
					case "80":
					case "90":
						$this->AryColSumData[ $date["year"].$date["month"] ][$key] = $args;
						break;
					default:
						$this->AryColData[ $date["year"].$date["month"].$key ] = $args;
						break;
				}
			}
		}
		//if(is_object($this->log)) $this->log->logPrint($this->id." GetMonthlyLoggerData this->AryColData\n".print_r($this->AryColData,TRUE),			REC_DEBUG,__LINE__,__FILE__,0,1);
		//if(is_object($this->log)) $this->log->logPrint($this->id." GetMonthlyLoggerData this->AryColSumData\n".print_r($this->AryColSumData,TRUE),REC_DEBUG,__LINE__,__FILE__,0,1);
		return true;
	}
	//月報データ取得 [2007/01/30]
	function GetMonthlyData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		$trg = $date["year"].$date["month"].$date["day"];
		if(!is_array($this->AryColData[$trg])){
			$this->GetMonthlyLoggerData($pnt_id,$curTime);
		}
		if(is_array($this->AryColData[$trg])){
			foreach($args as $key => $val){
				if(!in_array($key,$this->OutField["300"])) continue;
				$args[$key] = $this->AryColData[$trg][$key];
			}
			return true;
		}else{
			if(is_object($this->log)) $this->log->logPrint($this->id." GetMonthlyData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}
	//月報集計データ取得 [2007/01/30]
	function GetMonthlySumData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		$trg = $date["year"].$date["month"];
		if(!is_array($this->AryColSumData[$trg])){
			$this->GetMonthlyLoggerData($pnt_id,$curTime);
		}
		$ArySum = $this->AryColSumData[$trg];
		if(is_array($this->AryColSumData[$trg])){
			foreach($args as $sum_no => $data){
				if($sum_no=="90"){
					$args[$sum_no]["avg_ws"] = $ArySum[$sum_no]["avg_ws"];
				}else{
					foreach($data as $fld_id => $val){
						if(!in_array($fld_id,$this->OutField["300"])) continue;
						$args[$sum_no][$fld_id] = $ArySum[$sum_no][$fld_id];
					}
				}
			}
			return true;
		}else{
			if(is_object($this->log)) $this->log->logPrint($this->id." GetMonthlySumData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}

	//指定年の年報データをロガーから取得して連想配列に格納 [2007/01/30]
	function GetYearlyLoggerData($pnt_id,$curTime){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		//手動収集の場合はコネクションを維持するように設定
		if($this->execute_collection){
			$close_flg = 0;
		}else{
			$close_flg = 1;
		}

		//電文作成
		$date = DateConvert($curTime);
		$cmd = "D".$date["year"]."0000000000";

		//同じ電文の送信抑制
		if($this->SendCmdCnt[$cmd] > 0){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetYearlyLoggerData SendCmd Exists [{$cmd}]",REC_DEBUG,__LINE__,__FILE__);
			return true;
		}

		//データ取得
		if(!$buf=$this->SendCmd($cmd,$close_flg)){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetYearlyLoggerData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);

		//同じ電文の送信抑制
		$this->SendCmdCnt[$cmd] = $this->SendCmdCnt[$cmd] + 1;

		$AryTmp = explode("\r",$buf);

		$tmp_sum = "";

		//集計データ取得用にデータクラス定義
		$cls = new Data($this->log);
		foreach($AryTmp as $val){
			$key = "";
			$args = array();

			//日付データ補正
			$val = str_replace(" 00:00"," 24:00",$val);

			$tmp = explode(",",trim($val));
			if(is_numeric(substr($tmp[0],0,2))){
				$key = $tmp[0];
			}else{
				switch(mb_convert_encoding ($tmp[0],"EUC-JP","SJIS")){
					case "平均":
						$key = "40";
						break;
					case "最高":
						$key = "50";
						$tmp_sum = "最高";
						break;
					case "最低":
						$key = "60";
						$tmp_sum = "最低";
						break;
					case "起時":
						if($tmp_sum=="最高") $key = "55";
						if($tmp_sum=="最低") $key = "65";
						break;
					case "積算":
						$key = "70";
						break;
					case "日数":
						$key = "80";
						break;
					case "頻度":
						$key = "90";
						break;
				}
			}

			if($key!=""){
				switch($key){
					case "55":
					case "65":
						$args["avg_ws"]			=	(trim($tmp[1])	? substr(trim($tmp[1]),5,11)	:	"");
						$args["avg_wd"]			=	(trim($tmp[2])	? substr(trim($tmp[2]),5,11)	:	"");
						$args["max_ws"]			=	(trim($tmp[3])	? substr(trim($tmp[3]),5,11)	:	"");
						$args["max_wd"] 		=	(trim($tmp[4])	? substr(trim($tmp[4]),5,11)	:	"");
						$args["avg_temp"]		=	(trim($tmp[5]) 	? substr(trim($tmp[5]),5,11)	:	"");
						$args["max_temp"]		=	(trim($tmp[6]) 	? substr(trim($tmp[6]),5,11)	:	"");
						$args["min_temp"]		=	(trim($tmp[7]) 	? substr(trim($tmp[7]),5,11)	:	"");
						$args["avg_humid1"]		=	(trim($tmp[8]) 	? substr(trim($tmp[8]),5,11)	:	"");
						$args["max_humid1"]		=	(trim($tmp[9]) 	? substr(trim($tmp[9]),5,11)	:	"");
						$args["min_humid1"]		=	(trim($tmp[10])	? substr(trim($tmp[10]),5,11)	:	"");
						$args["humid2"]			=	(trim($tmp[11]) ? substr(trim($tmp[11]),5,11)	:	"");
						$args["press1"]			=	(trim($tmp[12]) ? substr(trim($tmp[12]),5,11)	:	"");
						$args["press2"]			=	(trim($tmp[13]) ? substr(trim($tmp[13]),5,11)	:	"");
						$args["max_rain_10min"]	=	(trim($tmp[14]) ? substr(trim($tmp[14]),5,11)	:	"");
						$args["max_rain_hour"]	=	(trim($tmp[15]) ? substr(trim($tmp[15]),5,11)	:	"");
						$args["max_rain_day"]	=	(trim($tmp[16]) ? substr(trim($tmp[16]),5,5)	:	"");
						$args["rain_month"]		=	(trim($tmp[17])	? substr(trim($tmp[17]),5,2)	:	"");
						break;
					case "80":
						$args["max_temp"]	=	(is_numeric($tmp[6]) 	? $tmp[6]	: "");
						$args["min_temp"]	=	(is_numeric($tmp[7]) 	? $tmp[7]	: "");
						$args["rain_month"]	=	(is_numeric($tmp[17])	? $tmp[17]	: "");
						//日数
						break;
					case "90":
						//風向頻度
						$args["avg_ws"]	= substr(trim($val),5);
						break;
					default:
						$args["avg_ws"]			=	(is_numeric($tmp[1])	? number_format($tmp[1],1, ".", "")  : "");
						$args["avg_wd"]			=	(is_numeric($tmp[2])	? $tmp[2] : "");
						$args["max_ws"]			=	(is_numeric($tmp[3])	? number_format($tmp[3],1, ".", "")  : "");
						$args["max_wd"] 		=	(is_numeric($tmp[4])	? $tmp[4] : "");
						$args["avg_temp"]		=	(is_numeric($tmp[5]) 	? number_format($tmp[5],1, ".", "")  : "");
						$args["max_temp"]		=	(is_numeric($tmp[6]) 	? number_format($tmp[6],1, ".", "")  : "");
						$args["min_temp"]		=	(is_numeric($tmp[7]) 	? number_format($tmp[7],1, ".", "")  : "");
						$args["avg_humid1"]		=	(is_numeric($tmp[8]) 	? number_format($tmp[8],1, ".", "")  : "");
						$args["max_humid1"]		=	(is_numeric($tmp[9]) 	? number_format($tmp[9],1, ".", "")  : "");
						$args["min_humid1"]		=	(is_numeric($tmp[10])	? number_format($tmp[10],1, ".", "")  : "");
						$args["humid2"]			=	(is_numeric($tmp[11]) 	? number_format($tmp[11],1, ".", "")  : "");
						$args["press1"]			=	(is_numeric($tmp[12]) 	? number_format($tmp[12],1, ".", "")  : "");
						$args["press2"]			=	(is_numeric($tmp[13]) 	? number_format($tmp[13],1, ".", "")  : "");
						$args["max_rain_10min"]	=	(is_numeric($tmp[14]) 	? number_format($tmp[14],1, ".", "")  : "");
						$args["max_rain_hour"]	=	(is_numeric($tmp[15]) 	? number_format($tmp[15],1, ".", "")  : "");
						$args["max_rain_day"]	=	(is_numeric($tmp[16]) 	? number_format($tmp[16],1, ".", "")  : "");
						$args["rain_month"]		=	(is_numeric($tmp[17]) 	? number_format($tmp[17],1, ".", "")  : "");

						if($key < 40){
							if(strtotime($date["year"]."/".$key."/01 00:01") < mktime(0, 0, 0, date("m"), 1, date("Y"))){
								$AryData = $cls->LoadSumData($pnt_id,"300",strtotime($date["year"]."/".$key."/01 00:01"));
								$args["avg_humid2"]	=	(is_numeric($AryData[40]["humid2"])	? number_format($AryData[40]["humid2"],1, ".", "")  : "");
								$args["max_humid2"]	=	(is_numeric($AryData[50]["humid2"])	? number_format($AryData[50]["humid2"],1, ".", "")  : "");
								$args["min_humid2"]	=	(is_numeric($AryData[60]["humid2"])	? number_format($AryData[60]["humid2"],1, ".", "")  : "");
								$args["avg_press1"]	=	(is_numeric($AryData[40]["press1"])	? number_format($AryData[40]["press1"],1, ".", "")  : "");
								$args["max_press1"]	=	(is_numeric($AryData[50]["press1"])	? number_format($AryData[50]["press1"],1, ".", "")  : "");
								$args["min_press1"]	=	(is_numeric($AryData[60]["press1"])	? number_format($AryData[60]["press1"],1, ".", "")  : "");
								$args["avg_press2"]	=	(is_numeric($AryData[40]["press2"])	? number_format($AryData[40]["press2"],1, ".", "")  : "");
								$args["max_press2"]	=	(is_numeric($AryData[50]["press2"])	? number_format($AryData[50]["press2"],1, ".", "")  : "");
								$args["min_press2"]	=	(is_numeric($AryData[60]["press2"])	? number_format($AryData[60]["press2"],1, ".", "")  : "");
							}
						}
				}

				switch($key){
					case "40":
					case "50":
					case "55":
					case "60":
					case "65":
					case "70":
					case "80":
					case "90":
						$this->AryColSumData[ $date["year"] ][$key] = $args;
						break;
					default:
						$this->AryColData[ $date["year"].$key ] = $args;
						break;
				}

			}
		}
		//if(is_object($this->log)) $this->log->logPrint($this->id." GetMonthlyLoggerData this->AryColData\n".print_r($this->AryColData,TRUE),			REC_DEBUG,__LINE__,__FILE__,0,1);
		//if(is_object($this->log)) $this->log->logPrint($this->id." GetMonthlyLoggerData this->AryColSumData\n".print_r($this->AryColSumData,TRUE),REC_DEBUG,__LINE__,__FILE__,0,1);
		return true;
	}

	//年報データ取得 [2007/01/30]
	function GetYearlyData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		$trg = $date["year"].$date["month"];
		if(!is_array($this->AryColData[$trg])){
			$this->GetYearlyLoggerData($pnt_id,$curTime);
		}
		if(is_array($this->AryColData[$trg])){
			foreach($args as $key => $val){
				if(!in_array($key,$this->OutField["400"])) continue;
				$args[$key] = $this->AryColData[$trg][$key];
			}
			return true;
		}else{
			if(is_object($this->log)) $this->log->logPrint($this->id." GetYearlyData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}

	//年報集計データ取得 [2007/01/30]
	function GetYearlySumData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		$trg = $date["year"];
		if(!is_array($this->AryColSumData[$trg])){
			$this->GetYearlyLoggerData($pnt_id,$curTime);
		}
		$ArySum = $this->AryColSumData[$trg];
		if(is_array($this->AryColSumData[$trg])){
			foreach($args as $sum_no => $data){
				if($sum_no=="90"){
					$args[$sum_no]["avg_ws"] = $ArySum[$sum_no]["avg_ws"];
				}else{
					foreach($data as $fld_id => $val){
						if(!in_array($fld_id,$this->OutField["400"])) continue;
						$args[$sum_no][$fld_id] = $ArySum[$sum_no][$fld_id];
					}
				}
			}
			return true;
		}else{
			if(is_object($this->log)) $this->log->logPrint($this->id." GetYearlySumData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}



	//指定日の1分データをすべて取得し現在データを更新する（現状では必要最小限の項目のみ更新)	[2007/01/30]
	function SetPresentData($pnt_id,$year,$month,$day){
		if(is_object($this->log)) $this->log->logPrint($this->id." SetPresentData Start {$year}/{$month}/{$day}",REC_DEBUG,__LINE__,__FILE__);

		//テーブル情報取得
		$trg = $year.$month.$day;
		$tbl = "t000_dat_{$year}";

		//指定日の全ての1分データを取得
		if(!$buf=$this->SendCmd("A{$trg}000000")){
			if(is_object($this->log)) $this->log->logPrint($this->id." SetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if($buf==chr(04)){
			if(is_object($this->log)) $this->log->logPrint($this->id." SetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);

		$rain_10min = "";
		$rain_hour 	= "";
		$rain_day 	= "";

		$AryTmp = explode("\r",$buf);
		foreach($AryTmp as $val){
			$line = explode(",",$val);
			if($line[1]!="+") continue;

			//日付取得
			$curTime	= DateConvert3($trg.sprintf("%04d",str_replace(":","",trim($line[0]))));
			$date		= DateConvert($curTime);

			$args = array();
			$args["avg_ws"]		=	(is_numeric($line[2])	? number_format($line[2],1, ".", "") : "");
			$args["avg_wd"]		=	(is_numeric($line[3])	? sprintf("%02d",$line[3]) : "");
			$args["max_ws"]		=	(is_numeric($line[4])	? number_format($line[4],1, ".", "") : "");
			$args["max_wd"] 	=	(is_numeric($line[5])	? sprintf("%02d",$line[5]) : "");
			$args["temp"]		=	(is_numeric($line[6]) 	? number_format($line[6],1, ".", "") : "");
			$args["humid1"]		=	(is_numeric($line[7]) 	? number_format($line[7],1, ".", "") : "");
			$args["humid2"]		=	(is_numeric($line[8]) 	? number_format($line[8],1, ".", "") : "");
			$args["press1"]		=	(is_numeric($line[9]) 	? number_format($line[9],1, ".", "") : "");
			$args["press2"]		=	(is_numeric($line[10]) 	? number_format($line[10],1, ".", "") : "");
			$args["rain_1min"]	=	(is_numeric($line[11]) 	? number_format($line[11],1, ".", "") : "");

			if(is_numeric($args["rain_1min"])){
				$rain_10min += $args["rain_1min"];
				$rain_hour 	+= $args["rain_1min"];
				$rain_day 	+= $args["rain_1min"];
			}

			$args["rain_10min"]	= $rain_10min;
			$args["rain_hour"]	= $rain_hour;
			$args["rain_day"]	= $rain_day;


			if($date["min"]=="00"){
				$rain_hour = "";
			}
			if(substr($date["min"],-1)=="0"){
				$rain_10min = "";

				//データ記録
				$this->AryColData[ $date["date"] ] = $args;
			}
		}
		return true;
	}

	//時報データ手動収集（ロガーデータを連想配列に格納する）[2007/01/31]
	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		if(is_object($this->log)) $this->log->logPrint($this->id." GetLoggerMinutelyCollection Start",REC_DEBUG,__LINE__,__FILE__);

		//格納用連想配列初期化
		$this->AryColData = array();

		$SetPresentError = array();

		//現在テーブル更新処理を起動
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
				$wrk_date = DateConvert($time1);
				//現在データ更新処理
				if(!$this->SetPresentData($pnt_id,$date["year"],$date["month"],$date["day"])){
					$SetPresentError[] = $date["year"].$date["month"].$date["day"];
				}
			}
			$time1 += 600;
		}
		return true;
	}

	//月報データ手動収集（取得データを連想配列に格納する）[2007/01/30]
	function GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2=""){

		//手動収集フラグON
		$this->execute_collection = 1;

		//連想配列初期化
		$this->AryColData		= array();
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
			if($time1 > time()) break;
			$date = DateConvert($time1);
			if($wrk_date["month"]!=$date["month"]){
				if(!$this->GetMonthlyLoggerData($pnt_id,$time1)) return false;
				$wrk_date = DateConvert($time1);
			}
			$time1 += 86400;
		}

		//手動収集フラグOFF
		$this->execute_collection = 0;
		$this->SendCmd();

		return true;
	}
	//月報データ手動収集 [2007/01/30]
	function GetMonthlyCollectionData($pnt_id,$curTime,&$args){
		return $this->GetMonthlyData($pnt_id,$curTime,$args);
	}
	//月報データ集計値取得 [2007/01/30]
	function GetMonthlyCollectionSumData($pnt_id,$curTime,&$args){
		return $this->GetMonthlySumData($pnt_id,$curTime,$args);
	}

	//年報データ手動収集（取得データを連想配列に格納する）[2007/01/30]
	function GetLoggerYearlyCollection($pnt_id,$trg1,$trg2=""){

		//手動収集フラグON
		$this->execute_collection = 1;

		//連想配列初期化
		$this->AryColData		= array();
		$this->AryColSumData	= array();

		$time1 = DateConvert3($trg1);
		if(!$this->GetYearlyLoggerData($pnt_id,$time1)) return false;

		//手動収集フラグOFF
		$this->execute_collection = 0;
		$this->SendCmd();

		return true;
	}
	//年報データ手動収集 [2007/01/30]
	function GetYearlyCollectionData($pnt_id,$curTime,&$args){
		return $this->GetYearlyData($pnt_id,$curTime,$args);
	}
	//年報集計データ手動収集 [2007/01/30]
	function GetYearlyCollectionSumData($pnt_id,$curTime,&$args){
		return $this->GetYearlySumData($pnt_id,$curTime,$args);
	}

	//電文送信 [2007/01/30]
	function SendCmd($cmd="",$close=1){

		//引数なしで呼び出した場合ファイルハンドルクローズ
		if($cmd==""&&$close){
			if(!$this->fp) return true;
			if(is_object($this->log)) $this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
			return true;
		}

		//ファイルハンドルが既にオープンされているかチェック
		if(!$this->fp){
			$ip = $this->setting["lgr_ip"];
			$port = $this->setting["lgr_port"];

			$this->OpenCheck();

			$this->fp = @stream_socket_client("tcp://{$ip}:{$port}", $errno, $errstr, 5);
			if($this->fp){
				@stream_set_timeout($this->fp, 2);
				if(is_object($this->log)) $this->log->logPrint("open:{$ip}:{$port}",																				REC_SOCK,__LINE__,__FILE__);
			}else{
				if(is_object($this->log)) $this->log->logPrint("open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}",REC_SOCK,__LINE__,__FILE__);
				$this->LastCloseTime = time();
				return false;
			}
		}

		//電文送信 ※各クラスで主にこの部分がちがう可能性あり
		if(is_object($this->log)) $this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
		fwrite($this->fp, $cmd);
		$flg = 1;
		$buf = "";
		while( (!feof($this->fp) && $flg)){
			$tmp  = fgetc($this->fp);
			$buf .= $tmp;
			if($tmp==chr(4)||$tmp==chr(21)||$tmp==chr(6)) $flg = 0;

			$debug_ord .= "[".ord($tmp)."]";	//Debug
			$debug_chr .= "[".$tmp."]";			//Debug
		}
		if(is_object($this->log)) $this->log->logPrint("debug_ord:[{$debug_ord}]",REC_DEBUG,__LINE__,__FILE__);	//Debug
		if(is_object($this->log)) $this->log->logPrint("debug_chr:[{$debug_chr}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

		if(is_object($this->log)) $this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__,0,0,array("EUC-JP","SJIS"));

		//クローズ処理
		if($close){
			if(is_object($this->log)) $this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
		}
		return $buf;
	}
}
?>