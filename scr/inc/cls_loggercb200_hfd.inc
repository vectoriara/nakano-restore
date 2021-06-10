<?
//==========================================================//
//	CB200ロガークラス										//
//----------------------------------------------------------//
//	処理概要												//
//	　CB200を広島消防雨量無し対応に変更したバージョン		//
//----------------------------------------------------------//
//	変更履歴						　						//
//		2019/02/02	N.ARAI									//
//		・新規作成											//
//      H=hiroshima F=Fire D=Department						//
//==========================================================//
class LoggerCB200_HFD extends LoggerWU900
{
	//変数宣言
	var $version	= "1.0";
	var $update		= "2019.02.02";
	var $memo		= "";
	var $id			= "CB200_HFD";
	var $tplName	= "CB200";//ロガー設定用に使用するテンプレート

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
//			"rain_10min",
//			"rain_hour",
//			"rain_day",
//			"move_rain_10min",
//			"move_rain_hour",
//			"move_rain_day",
//			"rain_1min",
			"weather"//,
//			"rain_rui"
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
//			"rain_10min",
//			"rain_hour",
//			"rain_day"
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
//			"max_rain_10min",
//			"rain_hour",
//			"rain_day",
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
//			"max_rain_10min",
//			"max_rain_hour",
//			"rain_day",
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
//			"max_rain_10min",
//			"max_rain_hour",
//			"max_rain_day",
//			"rain_month"
		)
	);

	//帳票毎でこのロガーが編集できる項目を定義
	var $EditField = array(
		"100" => array(	"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2"//,
//			"rain_10min",
//			"rain_hour",
//			"rain_day"
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
//			"max_rain_10min",
//			"rain_hour",
//			"rain_day",
			"weather"
		)
	);

	//現在データ取得処理
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd("PT,".date("Ym",$curTime).",".date("dHi",$curTime).",")){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);
		$AryTmp = $this->ClearBuf2($buf);

		if(trim($AryTmp[3])=="80"||trim($AryTmp[3])=="10"||trim($AryTmp[3])=="20") return true;

		//日付チェック
		$chkDate = date("YmdHi",$curTime);
		if($AryTmp[1].$AryTmp[2]==$chkDate){
			if(array_key_exists("avg_ws",$args))			$args["avg_ws"]		=	(is_numeric($AryTmp[4])		? number_format($AryTmp[4],	1, ".", "")		: "");
			if(array_key_exists("avg_wd",$args))			$args["avg_wd"]		=	(is_numeric($AryTmp[5])		? sprintf("%02d",$AryTmp[5]) 				: "");
			if(array_key_exists("max_ws",$args))			$args["max_ws"]		=	(is_numeric($AryTmp[6])		? number_format($AryTmp[6],	1, ".", "")  	: "");
			if(array_key_exists("max_wd",$args))			$args["max_wd"] 	=	(is_numeric($AryTmp[7])		? sprintf("%02d",$AryTmp[7])				: "");
			if(array_key_exists("temp",$args))				$args["temp"]		=	(is_numeric($AryTmp[8]) 	? number_format($AryTmp[8],	1, ".", "")  	: "");
			if(array_key_exists("humid1",$args))			$args["humid1"]		=	(is_numeric($AryTmp[9]) 	? number_format($AryTmp[9], 1, ".", "")  	: "");
			if(array_key_exists("humid2",$args))			$args["humid2"]		=	(is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],1, ".", "")  	: "");
			if(array_key_exists("press1",$args))			$args["press1"]		=	(is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],1, ".", "")  	: "");
			if(array_key_exists("press2",$args))			$args["press2"]		=	(is_numeric($AryTmp[12]) 	? number_format($AryTmp[12],1, ".", "")  	: "");
//			if(array_key_exists("rain_10min",$args))		$args["rain_10min"]	=	(is_numeric($AryTmp[13]) 	? number_format($AryTmp[13],1, ".", "")  	: "");
//			if(array_key_exists("rain_hour",$args))			$args["rain_hour"]	=	(is_numeric($AryTmp[14]) 	? number_format($AryTmp[14],1, ".", "")  	: "");
//			if(array_key_exists("rain_day",$args))			$args["rain_day"]	=	(is_numeric($AryTmp[15]) 	? number_format($AryTmp[15],1, ".", "")  	: "");
//			//移動積算取得
//			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);
//			if(array_key_exists("rain_1min",$args))			$args["rain_1min"]			= $AryMove["rain_1min"];
//			if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	= $AryMove["move_rain_10min"];
//			if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		= $AryMove["move_rain_hour"];
//			if(array_key_exists("move_rain_day",$args))		$args["move_rain_day"]		= $AryMove["move_rain_day"];
//			//1分積算をもとに積算データを再取得
//			$rain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);
//			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($rain["rain_10min"])	? number_format($rain["rain_10min"],1, ".", "")  : "");
//			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($rain["rain_hour"])		? number_format($rain["rain_hour"],	1, ".", "")  : "");
//			if(array_key_exists("rain_day",$args))		$args["rain_day"]	=	(is_numeric($rain["rain_day"]) 		? number_format($rain["rain_day"],	1, ".", "")  : "");
//			//累計雨量算出＆記録処理
//			if(array_key_exists("rain_rui",$args))	$args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
			//天気取得
			if(array_key_exists("weather",$args))	$args["weather"] = $this->GetWeather($pnt_id);
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//時報データ取得処理
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd("IT,".date("Ym,dHi,", $curTime),0)){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);
		$AryTmp = $this->ClearBuf2($buf);

		//手動収集以外はソケットクローズ
		if(!$this->execute_collection) $this->SendCmd();

		if(trim($AryTmp[3])=="80"||trim($AryTmp[3])=="10"||trim($AryTmp[3])=="20") return true;

		//日付チェック
		$chkDate = date("YmdHi",$curTime);
		if($AryTmp[1].$AryTmp[2]==$chkDate){
			if(array_key_exists("avg_ws",$args))		$args["avg_ws"]			=	(is_numeric($AryTmp[4])		? number_format($AryTmp[4],	1, ".", "")  	: "");
			if(array_key_exists("avg_wd",$args))		$args["avg_wd"]			=	(is_numeric($AryTmp[5])		? sprintf("%02d",$AryTmp[5])				: "");
			if(array_key_exists("max_ws",$args))		$args["max_ws"]			=	(is_numeric($AryTmp[6])		? number_format($AryTmp[6],	1, ".", "")  	: "");
			if(array_key_exists("max_wd",$args))		$args["max_wd"] 		=	(is_numeric($AryTmp[7])		? sprintf("%02d",$AryTmp[7])				: "");
			if(array_key_exists("temp",$args))			$args["temp"]			=	(is_numeric($AryTmp[8]) 	? number_format($AryTmp[8],	1, ".", "") 	: "");
			if(array_key_exists("humid1",$args))		$args["humid1"]			=	(is_numeric($AryTmp[9]) 	? number_format($AryTmp[9],	1, ".", "")  	: "");
			if(array_key_exists("humid2",$args))		$args["humid2"]			=	(is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],1, ".", "")  	: "");
			if(array_key_exists("press1",$args))		$args["press1"]			=	(is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],1, ".", "")  	: "");
			if(array_key_exists("press2",$args))		$args["press2"]			=	(is_numeric($AryTmp[12]) 	? number_format($AryTmp[12],1, ".", "")  	: "");
//			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]		=	(is_numeric($AryTmp[13]) 	? number_format($AryTmp[13],1, ".", "")  	: "");
//			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]		=	(is_numeric($AryTmp[14]) 	? number_format($AryTmp[14],1, ".", "")  	: "");
//			if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	(is_numeric($AryTmp[15]) 	? number_format($AryTmp[15],1, ".", "")  	: "");

//			//WU900は10分、時間積算が移動の値の為、時間積算を計算する
//			$args["rain_hour"] = $this->GetRainHour($pnt_id,$curTime,$args["rain_10min"]);
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		//正時データの場合は日報集計値保存処理
		if(date("i",$curTime)=="00"){
			$this->SetDailySumData($pnt_id,$curTime,$AryTmp);
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//日報集計値保存処理
	function SetDailySumData($pnt_id,$curTime,$AryTmp){
		if($curTime > time()) return false;

		$date = DateConvert($curTime);

		if(is_object($this->log)) $this->log->logPrint("====SetDailySumData==== Start Target=> ".$date["date"],REC_DEBUG,__LINE__,__FILE__);

		$data	= new Data;
		$AryRec	= $data->GetSumDataField("200");

		$args = $AryRec["40"];
		if(array_key_exists("avg_ws",$args))	$args["avg_ws"]			=	(is_numeric($AryTmp[18])	? number_format($AryTmp[18],	1, ".", "")  	: "");
		if(array_key_exists("avg_wd",$args))	$args["avg_wd"]			=	(is_numeric($AryTmp[19])	? sprintf("%02d",$AryTmp[19])				 	: "");
		if(array_key_exists("temp",$args))		$args["temp"]			=	(is_numeric($AryTmp[22]) 	? number_format($AryTmp[22],1, ".", "")			: "");
		if(array_key_exists("humid1",$args))	$args["humid1"]			=	(is_numeric($AryTmp[23]) 	? number_format($AryTmp[23],1, ".", "")  		: "");
		if(array_key_exists("humid2",$args))	$args["humid2"]			=	(is_numeric($AryTmp[24]) 	? number_format($AryTmp[24],1, ".", "")  		: "");
		if(array_key_exists("press1",$args))	$args["press1"]			=	(is_numeric($AryTmp[25]) 	? number_format($AryTmp[25],1, ".", "")  		: "");
		if(array_key_exists("press2",$args))	$args["press2"]			=	(is_numeric($AryTmp[26]) 	? number_format($AryTmp[26],1, ".", "")  		: "");
		$AryRec["40"] = $args;

		$args = $AryRec["50"];
		if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp[30])	? number_format($AryTmp[30],	1, ".", "")		: "");
		if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp[31])	? sprintf("%02d",$AryTmp[31])					: "");
		if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	(is_numeric($AryTmp[32])	? number_format($AryTmp[32],	1, ".", "") 	: "");
		if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	(is_numeric($AryTmp[33])	? sprintf("%02d",$AryTmp[33])					: "");
		if(array_key_exists("temp",$args))				$args["temp"]			=	(is_numeric($AryTmp[34]) 	? number_format($AryTmp[34],	1, ".", "")		: "");
		if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp[35]) 	? number_format($AryTmp[35],	1, ".", "")  	: "");
		if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp[37]) 	? number_format($AryTmp[37],	1, ".", "")  	: "");
		if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp[38]) 	? number_format($AryTmp[38],	1, ".", "")  	: "");
//		if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]	= (is_numeric($AryTmp[39]) 		? number_format($AryTmp[39],	1, ".", "")  	: "");
//		if(array_key_exists("rain_hour",$args))			$args["rain_hour"]		= (is_numeric($AryTmp[40]) 		? number_format($AryTmp[40],	1, ".", "")  	: "");
		$AryRec["50"] = $args;

		$args = $AryRec["55"];
		if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	$AryTmp[42];
		if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	$AryTmp[44];
		if(array_key_exists("temp",$args))				$args["temp"]			=	$AryTmp[46];
		if(array_key_exists("humid1",$args))			$args["humid1"]			=	$AryTmp[47];
		if(array_key_exists("press1",$args))			$args["press1"]			=	$AryTmp[49];
		if(array_key_exists("press2",$args))			$args["press2"]			=	$AryTmp[50];
//		if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]	=	$AryTmp[51];
//		if(array_key_exists("rain_hour",$args))			$args["rain_hour"]		=	$AryTmp[52];
		$AryRec["55"] = $args;

		$args = $AryRec["60"];
		if(array_key_exists("temp",$args))		$args["temp"]	=	(is_numeric($AryTmp[58]) 	? number_format($AryTmp[58],1, ".", "") 	: "");
		if(array_key_exists("humid1",$args))	$args["humid1"]	=	(is_numeric($AryTmp[59]) 	? number_format($AryTmp[59],1, ".", "")  	: "");
		if(array_key_exists("press1",$args))	$args["press1"]	=	(is_numeric($AryTmp[61]) 	? number_format($AryTmp[61],1, ".", "")  	: "");
		if(array_key_exists("press2",$args))	$args["press2"]	=	(is_numeric($AryTmp[62]) 	? number_format($AryTmp[62],1, ".", "")  	: "");
		$AryRec["60"] = $args;

		$args = $AryRec["65"];
		if(array_key_exists("temp",$args))		$args["temp"]	=	$AryTmp[70];
		if(array_key_exists("humid1",$args))	$args["humid1"]	=	$AryTmp[71];
		if(array_key_exists("press1",$args))	$args["press1"]	=	$AryTmp[73];
		if(array_key_exists("press2",$args))	$args["press2"]	=	$AryTmp[74];
		$AryRec["65"] = $args;

		$args = $AryRec["70"];
//		if(array_key_exists("rain_day",$args))	$args["rain_day"]	= (is_numeric($AryTmp[89]) 	? number_format($AryTmp[89],	1, ".", "")  	: "");
		$AryRec["70"] = $args;

		$args = $AryRec["90"];
		if(array_key_exists("avg_ws",$args))			$args["avg_ws"]	= join(",",array_slice($AryTmp,90,17));
		$AryRec["90"] = $args;

		$data->SaveSumData($pnt_id,"200",$curTime,$AryRec);
	}

}
?>