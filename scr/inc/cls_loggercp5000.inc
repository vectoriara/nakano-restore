<?
//============================================================//
//	CP-5000ロガークラス																				//
//------------------------------------------------------------//
//	処理概要																									//
//	　CP-5000用																								//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/12/17	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class LoggerCP5000 extends BaseLogger
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2007.01.31";
	var $memo			= "";
	var $id				= "CP5000";
	var $type			= "1";														//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
	var $ReCal		= "1";														//再計算処理可否
	var $map			= array("lgr_id"			=> "lgr_id",
		"lgr_pnt_id"	=> "lgr_pnt_id",
		"lgr_enable"	=> "lgr_enable",
		"lgr_time"		=> "lgr_time",
		"lgr_ip"			=> "lgr_col_1",
		"lgr_port"		=> "lgr_col_2",
	);
	var $EditSys	= array("100","200");							//編集可能なデータ帳票
	var $ColSys		= array("100","200");							//手動収集可能なデータ帳票
	var $DelSys		= array("100","200");							//削除可能なデータ帳票

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
		"000" => array(	"avg_ws",
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
		"100" => array(	"avg_ws",
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
		"200" => array(	"avg_ws",
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
		"300" => array(	"avg_ws",
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
		"400" => array("avg_ws",
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

	//ロガー編集可能項目を定義
	var $EditField = array(
		"100" => array(	"avg_ws",
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
		"200" => array(	"avg_ws",
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
		)
	);

	//通信チェック [2006/12/31]
	function CheckConnection($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		$buf = $this->SendCmd("D000000000000");
		$buf = $this->ClearBuf($buf);
		return $buf;
	}

	//時刻補正 [2006/12/31]
	function SetDate($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$buf = $this->SendCmd("T".date("ymdHis"));
			if($buf==chr(6)){
				if(is_object($this->log))	$this->log->logPrint("時刻補正が完了しました[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return 1;
			}else{
				if(is_object($this->log))	$this->log->logPrint("時刻補正に失敗しました[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return false;
			}
		}else{
			if(is_object($this->log))	$this->log->logPrint("時刻補正は許可されていません[{$this->id}]",REC_DEBUG,__LINE__,__FILE__);
			return 2;
		}
	}

	//現在データ取得処理 [2007/01/31]
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd("D000000000000")){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//日付チェック
		$chkDate = date("1H.i", $curTime);
		if($AryTmp[0]==$chkDate){
			if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp[1])		? number_format($AryTmp[1],	1, ".", "") : "");
			if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp[2])		? sprintf("%02d",$AryTmp[2])						: "");
			if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	(is_numeric($AryTmp[3])		? number_format($AryTmp[3],	1, ".", "") : "");
			if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	(is_numeric($AryTmp[4])		? sprintf("%02d",$AryTmp[4])						: "");
			if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp[5]) 	? number_format($AryTmp[5],	1, ".", "") : "");
			if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp[6]) 	? number_format($AryTmp[6], 1, ".", "") : "");
			if(array_key_exists("humid2",$args))			$args["humid2"]			=	(is_numeric($AryTmp[7]) 	? number_format($AryTmp[7], 1, ".", "") : "");
			if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp[8]) 	? number_format($AryTmp[8],	1, ".", "") : "");
			if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp[9]) 	? number_format($AryTmp[9],	1, ".", "") : "");
			$tmpRain=$AryTmp[(10+substr(date("i", $curTime),0,1))];
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($tmpRain)			? number_format($tmpRain,		1, ".", "") : "");
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($AryTmp[16]) 	? number_format($AryTmp[16],1, ".", "") : "");
			if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	(is_numeric($AryTmp[17]) 	? number_format($AryTmp[17],1, ".", "") : "");

			//日積算補正
			if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% 日積算補正開始",REC_DEBUG,__LINE__,__FILE__);

			$data = new Data($this->log);
			if(date("Hi",$curTime)=="0000"){
				//ロガーの日積算（リセット直後値）＋ＤＢ当日日積算（23:59まで)　＝　24:00時点の日積算とする
				$AryData	= $data->LoadData($pnt_id,"000",$curTime-60);
				$date 		= DateConvert($curTime-60);
				$rain_day	= $AryData[ $date["date"] ]["rain_day"];

				if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% ロガーの日積算（リセット直後値）＋ＤＢ当日日積算（23:59まで)",	REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% ".$args["rain_day"]."＋ {$rain_day}",													REC_DEBUG,__LINE__,__FILE__);
				$args["rain_day"] = number_format($args["rain_day"] + $rain_day, 1, ".", "");
				if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% 補正後日積算 => ".$args["rain_day"],REC_DEBUG,__LINE__,__FILE__);
			}else{
				$date 		= DateConvert($curTime-86400);
				$trg			= $date["year"].$date["month"].$date["day"]."2400";
				$date 		= DateConvert($curTime);
				$AryData	= $data->LoadData($pnt_id,"000",strtotime($date["year"]."/".$date["month"]."/".$date["day"]." 00:00"));

				if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% trg => {$trg}",REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% ".print_r($AryData,TRUE),REC_DEBUG,__LINE__,__FILE__);
				if(is_array($AryData[$trg])){
					if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% ロガーの日積算 - 1分データ 24:00時点",									REC_DEBUG,__LINE__,__FILE__);
					if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% ".$args["rain_day"]." - ".$AryData[$trg]["rain_1min"],	REC_DEBUG,__LINE__,__FILE__);
					$args["rain_day"] = number_format($args["rain_day"] - $AryData[$trg]["rain_1min"],1, ".", "");
					if(is_object($this->log))	$this->log->logPrint("%%%%GetPresentData%%%% 補正後日積算 => ".$args["rain_day"],REC_DEBUG,__LINE__,__FILE__);
				}
			}

			//移動雨量取得取得
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);
			if(array_key_exists("rain_1min",$args))				$args["rain_1min"]				= $AryMove["rain_1min"];
			if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	= $AryMove["move_rain_10min"];
			if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		= $AryMove["move_rain_hour"];
			if(array_key_exists("move_rain_day",$args))		$args["move_rain_day"]		= $AryMove["move_rain_day"];
			//1分積算をもとに積算データを再取得
			$rain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($rain["rain_10min"])	? number_format($rain["rain_10min"],1, ".", "")  : "");
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($rain["rain_hour"]) 	? number_format($rain["rain_hour"],	1, ".", "")  : "");
			//if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	(is_numeric($rain["rain_day"]) 		? number_format($rain["rain_day"],	1, ".", "")  : "");
			//累計雨量算出＆記録処理
			if(array_key_exists("rain_rui",$args))	$args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
			//天気取得
			if(array_key_exists("weather",$args))		$args["weather"]	= $this->GetWeather($pnt_id);
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//ロガーから指定日の日報データを取得し連想配列に格納 [2007/01/30]
	function GetDailyLoggerData($pnt_id,$curTime){
		if($curTime > time()) return false;
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		//電文作成
		$date = DateConvert($curTime);
		$cmd  = "D".substr($date["year"],-2).$date["month"].$date["day"]."000000";

		//同じ電文の送信抑制
		if($this->SendCmdCnt[$cmd] > 0){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetDailyLoggerData SendCmd Exists [{$cmd}]",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}

		//データ取得
		if(!$buf=$this->SendCmd($cmd,0)){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetDailyLoggerData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(!$this->execute_collection) $this->SendCmd();	//手動収集時以外はクローズ
		if($buf==chr(4)||$buf==chr(21)||$buf==chr(6)) return false;

		$buf = $this->ClearBuf($buf);

		//同じ電文の送信抑制
		$this->SendCmdCnt[$cmd] = $this->SendCmdCnt[$cmd] + 1;

		$AryTmp = explode("\r",$buf);
		$AryTmp = array_slice($AryTmp,1,count($AryTmp)-1);

		$strDate = date("Y/m/d", $curTime);	//CP5000の起時日付として使用

		foreach($AryTmp as $val){
			$key	= "";
			$args = array();
			$tmp	= explode(",",trim($val));

			$trim = array();
			foreach($tmp as $col)	$trim[] = trim($col);
			$tmp = $trim;

			switch($tmp[0]){
				case $tmp[0] >= 1 && 24 >=$tmp[0]:
					$key	= sprintf("%02d",$tmp[0])."00";
					/////10分最大雨量を事前に調査/////
					//10分間積算部分を配列で抜き出す
					$tmpRain = array_slice($tmp, 12, 6);
					rsort($tmpRain);
					//1時から24時のデータ
					$line = $key.				//時間
						",".$tmp[1].		//風速
						",".$tmp[2].		//風向
						",".$tmp[3].		//瞬間風速
						",".$tmp[4].		//同時風向
						",".$tmp[5].		//気温
						",".$tmp[8].		//相対湿度
						",".$tmp[9].		//実効湿度
						",".$tmp[10].		//現地気圧
						",".$tmp[11].		//海面気圧
						",".$tmpRain[0].//10分間最大雨量
						",".$tmp[18].		//時間雨量
						",".$tmp[19].		//日雨量
						",".$data[20].	//水位
						"";
					break;
				case "25"://平均
					$key = "40";
					$line = "平均".										//ヘッダ
						",".$tmp[1].									//風速
						",".sprintf("%02d",$tmp[2]).	//風向
						",".													//瞬間風速
						",".													//同時風向
						",".$tmp[5].									//気温
						",".$tmp[8].									//相対湿度
						",".$tmp[9].									//実効湿度
						",".$tmp[10].									//現地気圧
						",".$tmp[11].									//海面気圧
						",".					   							//10分間雨量
						",".													//時間雨量
						",".													//日雨量
						",".$tmp[20].									//水位
						"";
					break;
				case "26"://最高
					$key = "50";
					$line = "最高".										//ヘッダ
						",".$tmp[1].									//風速
						",".sprintf("%02d",$tmp[2]).	//風向
						",".$tmp[3].									//瞬間風速
						",".sprintf("%02d",$tmp[4]).	//同時風向
						",".$tmp[6].									//気温
						",".$tmp[8].									//相対湿度
						",".													//実効湿度
						",".$tmp[10].									//現地気圧
						",".$tmp[11].									//海面気圧
						",".$tmp[17].									//10分間雨量
						",".$tmp[18].									//時間雨量
						",".													//日雨量
						",".$tmp[20].									//水位
						"";
					break;
				case "27"://最高起時
					$key = "55";
					$line = "起時".																																										//ヘッダ
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[1],-5) )).		//風速
						",".																																													//風向
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[3],-5) )).		//瞬間風速
						",".																																													//同時風向
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[6],-5) )).		//気温
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[8],-5) )).		//相対湿度
						",".																																													//実効湿度
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[10],-5) )).	//現地気圧
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[11],-5) )).	//海面気圧
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[17],-5) )).	//10分間雨量
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[18],-5) )).	//時間雨量
						",".												  																																//日雨量
						",$strDate ".str_replace("00:00", "00:00", str_replace(".", ":", substr("0".$tmp[20],-5) )).	//水位
						"";
					break;
				case "28"://最低
					$key = "60";
					$line = "最低".		//ヘッダ
						",".					//風速
						",".					//風向
						",".					//瞬間風速
						",".					//同時風向
						",".$tmp[7].	//気温
						",".$tmp[8].	//相対湿度
						",".					//実効湿度
						",".$tmp[10].	//現地気圧
						",".$tmp[11].	//海面気圧
						",".					//10分間雨量
						",".					//時間雨量
						",".					//日雨量
						",".$tmp[20].	//水位
						"";
					break;
				case "29"://最低起時
					$key = "65";
					$line = "起時".																																										//ヘッダ
						",".																																													//風速
						",".																																													//風向
						",".																																													//瞬間風速
						",".																																													//同時風向
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[7],-5) )).		//気温
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[8],-5) )).		//相対湿度
						",".																																													//実効湿度
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[10],-5) )).	//現地気圧
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[11],-5) )).	//海面気圧
						",".					   					  																																	//10分間雨量
						",".																																													//時間雨量
						",".																																													//日雨量
						",$strDate ".str_replace("00:00", "24:00", str_replace(".", ":", substr("0".$tmp[20],-5) )).	//水位
						"";
					break;
				case "30"://積算
					$key = "70";
					$line = "積算".		//ヘッダ
						",".					//風速
						",".					//風向
						",".					//瞬間風速
						",".					//同時風向
						",".					//気温
						",".					//相対湿度
						",".					//実効湿度
						",".					//現地気圧
						",".					//海面気圧
						",".					//10分間雨量
						",".					//時間雨量
						",".$tmp[19].	//日雨量
						",".
						"";
					break;
				case "31":
					$key = "90";
					$tmp = array_slice($tmp,1,count($tmp)-2);
					$line = join(",",$tmp);
					break;
				default:
					continue;
			}
			$tmp = explode(",",$line);

			if($key!=""){
				switch($key){
					case "55":
					case "65":
						$args["avg_ws"]					=	(trim($tmp[1])		?	substr(trim($tmp[1]),	11,5)	:	"");
						$args["avg_wd"]					=	(trim($tmp[2])		?	substr(trim($tmp[2]),	11,5)	:	"");
						$args["max_ws"]					=	(trim($tmp[3])		?	substr(trim($tmp[3]),	11,5)	:	"");
						$args["max_wd"] 				=	(trim($tmp[4])		?	substr(trim($tmp[4]),	11,5)	:	"");
						$args["temp"]						=	(trim($tmp[5]) 		?	substr(trim($tmp[5]),	11,5)	:	"");
						$args["humid1"]					=	(trim($tmp[6]) 		?	substr(trim($tmp[6]),	11,5)	:	"");
						$args["humid2"]					=	(trim($tmp[7]) 		?	substr(trim($tmp[7]),	11,5)	:	"");
						$args["press1"]					=	(trim($tmp[8]) 		?	substr(trim($tmp[8]),	11,5)	:	"");
						$args["press2"]					=	(trim($tmp[9]) 		?	substr(trim($tmp[9]),	11,5)	:	"");
						$args["max_rain_10min"]	=	(trim($tmp[10]) 	?	substr(trim($tmp[10]),11,5)	:	"");
						$args["rain_hour"]			=	(trim($tmp[11]) 	?	substr(trim($tmp[11]),11,5)	:	"");
						$args["rain_day"]				=	(trim($tmp[12]) 	?	substr(trim($tmp[12]),11,5)	:	"");
						//起時特殊対応
						if(!($this->AryColSumData[ $date["year"].$date["month"].$date["day"] ][($key-5)]["max_rain_10min"]>0)){
							$args["max_rain_10min"]	=	"";
						}
						if(!($this->AryColSumData[ $date["year"].$date["month"].$date["day"] ][($key-5)]["rain_hour"]>0)){
							$args["rain_hour"]	=	"";
						}
						break;
					case "90":
						//風向頻度
						$args["avg_ws"]					= $line;
						break;
					default:
						$args["avg_ws"]					=	(is_numeric($tmp[1])		? number_format($tmp[1],	1, ".", "")  : "");
						$args["avg_wd"]					=	(is_numeric($tmp[2])		? $tmp[2] : "");
						$args["max_ws"]					=	(is_numeric($tmp[3])		? number_format($tmp[3],	1, ".", "")  : "");
						$args["max_wd"] 				=	(is_numeric($tmp[4])		? $tmp[4] : "");
						$args["temp"]						=	(is_numeric($tmp[5]) 		? number_format($tmp[5],	1, ".", "")  : "");
						$args["humid1"]					=	(is_numeric($tmp[6]) 		? number_format($tmp[6], 	1, ".", "")  : "");
						$args["humid2"]					=	(is_numeric($tmp[7]) 		? number_format($tmp[7], 	1, ".", "")  : "");
						$args["press1"]					=	(is_numeric($tmp[8]) 		? number_format($tmp[8],	1, ".", "")  : "");
						$args["press2"]					=	(is_numeric($tmp[9]) 		? number_format($tmp[9],	1, ".", "")  : "");
						$args["max_rain_10min"]	=	(is_numeric($tmp[10]) 	? number_format($tmp[10],	1, ".", "")  : "");
						$args["rain_hour"]			=	(is_numeric($tmp[11]) 	? number_format($tmp[11],	1, ".", "")  : "");
						$args["rain_day"]				=	(is_numeric($tmp[12]) 	? number_format($tmp[12],	1, ".", "")  : "");

						//天気取得
						if($key>=100)	$args["weather"]	=	$this->GetWeatherData($pnt_id,DateConvert3($date["year"].$date["month"].$date["day"].$key));
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
//			if(is_object($this->log))	$this->log->logPrint($this->id." GetDailyLoggerData this->AryColData\n".print_r($this->AryColData,TRUE),			REC_DEBUG,__LINE__,__FILE__,0,1);
//			if(is_object($this->log))	$this->log->logPrint($this->id." GetDailyLoggerData this->AryColSumData\n".print_r($this->AryColSumData,TRUE),REC_DEBUG,__LINE__,__FILE__,0,1);
		return true;
	}
	//日報データ取得 [2007/01/31]
	function GetDailyData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		$trg	= $date["date"];
		if(!is_array($this->AryColData[$trg])) $this->GetDailyLoggerData($pnt_id,$curTime);

		if(is_array($this->AryColData[$trg])){
			$AryTmp = $this->AryColData[$trg];
			foreach($this->OutField["200"] as $key){
				if(!array_key_exists($key,$args))		continue;
				if(!array_key_exists($key,$AryTmp)) continue;
				$args[$key] = $AryTmp[$key];
			}
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($this->id." GetDailyData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}
	//日報データ集計値取得 [2007/01/31]
	function GetDailySumData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		$trg	= $date["year"].$date["month"].$date["day"];
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
			if(is_object($this->log))	$this->log->logPrint($this->id." GetDailySumData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}

	//電文送信 [2007/01/30]
	function SendCmd($cmd="",$close=1){

		//引数なしで呼び出した場合ファイルハンドルクローズ
		if($cmd==""&&$close){
			if(!$this->fp) return true;
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
			return true;
		}

		//ファイルハンドルが既にオープンされているかチェック
		if(!$this->fp){
			$ip		= $this->setting["lgr_ip"];
			$port =	$this->setting["lgr_port"];

			$this->OpenCheck();

			$this->fp = @stream_socket_client("tcp://{$ip}:{$port}", $errno, $errstr, 5);
			if($this->fp){
				@stream_set_timeout($this->fp, 2);
				if(is_object($this->log))	$this->log->logPrint("open:{$ip}:{$port}",																				REC_SOCK,__LINE__,__FILE__);
			}else{
				if(is_object($this->log))	$this->log->logPrint("open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}",REC_SOCK,__LINE__,__FILE__);
				$this->LastCloseTime = time();
				return false;
			}
		}

		//電文送信 ※各クラスで主にこの部分がちがう可能性あり
		if(is_object($this->log))	$this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
		fwrite($this->fp, chr(02).$cmd.chr(03));
		$flg = 1;
		$buf = "";
		while( (!feof($this->fp) && $flg)){
			$tmp  = fgetc($this->fp);
			$buf .= $tmp;
			if($tmp==chr(4)||$tmp==chr(21)||$tmp==chr(6))	$flg = 0;

			$debug_ord .= "[".ord($tmp)."]";	//Debug
			$debug_chr .= "[".$tmp."]";				//Debug
		}
		if(is_object($this->log))	$this->log->logPrint("debug_ord:[{$debug_ord}]",REC_DEBUG,__LINE__,__FILE__);	//Debug
		if(is_object($this->log))	$this->log->logPrint("debug_chr:[{$debug_chr}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

		if(is_object($this->log))	$this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__,0,0,array("EUC-JP","SJIS"));

		//クローズ処理
		if($close){
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
		}
		return $buf;
	}

}
?>