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
class LoggerWU900 extends BaseLogger
{
	//変数宣言
	var $varsion	= "1.01";
	var $update		= "2015.11.05";
	var $memo			= "";
	var $id				= "WU900";
	var $type			= "1";														//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
	var $ReCal		= "1";														//再計算処理可否
	public $isEditDaily = false;
	var $map			= array("lgr_id"				=> "lgr_id",
		"lgr_pnt_id"		=> "lgr_pnt_id",
		"lgr_enable"		=> "lgr_enable",
		"lgr_time"			=> "lgr_time",
		"lgr_ip"				=> "lgr_col_1",
		"lgr_port"			=> "lgr_col_2",
		"lgr_humid"			=> "lgr_col_3",
		"lgr_alt"				=> "lgr_col_4",
	);
	var $EditSys	= array("100","200");							//編集可能なデータ帳票
	var $ColSys		= array("100");										//手動収集可能なデータ帳票
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
	//通信チェック [2006/12/21]
	function CheckConnection($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		$buf = $this->SendCmd("PT,".date("Ym").",".date("dHi").",");
		$buf = $this->ClearBuf($buf);
		return $buf;
	}

	//時刻補正 [2006/12/21]
	function SetDate($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$curTime = time();
			$buf = $this->SendCmd("TT,".date("Ym",$curTime).",".date("dHis",$curTime).",");
			//結果チェック
			$tmp = explode(",",$buf);
			if(date("YmdHis",$curTime)==$tmp[1].$tmp[2]){
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
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//時報データ取得処理	[2006/12/31]
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd("IT,".date("Ym,dHi,", $curTime),0)){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf		= $this->ClearBuf($buf);
		$AryTmp =	$this->ClearBuf2($buf);

		//手動収集以外はソケットクローズ
		if(!$this->execute_collection) $this->SendCmd();

		if(trim($AryTmp[3])=="80"||trim($AryTmp[3])=="10"||trim($AryTmp[3])=="20") return true;

		//日付チェック
		$chkDate = date("YmdHi",$curTime);
		if($AryTmp[1].$AryTmp[2]==$chkDate){
			if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp[4])		? number_format($AryTmp[4],	1, ".", "")  	: "");
			if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp[5])		? sprintf("%02d",$AryTmp[5])							: "");
			if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	(is_numeric($AryTmp[6])		? number_format($AryTmp[6],	1, ".", "")  	: "");
			if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	(is_numeric($AryTmp[7])		? sprintf("%02d",$AryTmp[7])							: "");
			if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp[8]) 	? number_format($AryTmp[8],	1, ".", "") 	: "");
			if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp[9]) 	? number_format($AryTmp[9],	1, ".", "")  	: "");
			if(array_key_exists("humid2",$args))			$args["humid2"]			=	(is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],1, ".", "")  	: "");
			if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],1, ".", "")  	: "");
			if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp[12]) 	? number_format($AryTmp[12],1, ".", "")  	: "");
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($AryTmp[13]) 	? number_format($AryTmp[13],1, ".", "")  	: "");
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($AryTmp[14]) 	? number_format($AryTmp[14],1, ".", "")  	: "");
			if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	(is_numeric($AryTmp[15]) 	? number_format($AryTmp[15],1, ".", "")  	: "");

			//WU900は10分、時間積算が移動の値の為、時間積算を計算する
			$args["rain_hour"] = $this->GetRainHour($pnt_id,$curTime,$args["rain_10min"]);
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		//正時データの場合は日報集計値保存処理
		if(date("i",$curTime)=="00"){
			$this->SetDailySumData($pnt_id,$curTime,$AryTmp);
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//手動収集時
	function GetRainHour($pnt_id,$curTime,$rain_10min){
		if(!is_numeric($rain_10min)) return "";

		$rain_hour = $rain_10min;

		if($this->execute_collection){
			//手動収集時
			if(is_object($this->log))	$this->log->logPrint("++++GetRainHour+++ [手動収集時] rain_10min-> {$rain_hour}",REC_DEBUG,__LINE__,__FILE__);
			if(date("i",$curTime)!="10"){
				$start = date("i",$curTime)=="00" ? "50" : date("i",$curTime) - 10;
				for($i=$start;$i>=10;$i-=10){
					$date = DateConvert($curTime-$i*60);

					if(is_array($this->AryColData[ $date["date"] ])){
						$AryTmp = $this->AryColData[ $date["date"] ];
						$rain_hour += $AryTmp["rain_10min"];
						if(is_object($this->log))	$this->log->logPrint("++++GetRainHour+++ [". $date["date"]."] + ".$AryTmp["rain_10min"],REC_DEBUG,__LINE__,__FILE__);
					}
				}
			}
		}else{
			//定期処理時
			if(is_object($this->log))	$this->log->logPrint("++++GetRainHour+++ [定期処理時] rain_10min-> {$rain_hour}",REC_DEBUG,__LINE__,__FILE__);
			$dbs		= new CONN;
			$now		= DateConvert($curTime);
			$prev		= DateConvert($curTime-60);
			$tbl		= "t100_dat_".$now["year"];
			$trg		= "t100_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["date"]."'";
			$strSql = "SELECT SUM(TO_NUMBER(rain_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND rain_10min <> '' AND {$trg};";
			if(is_object($this->log))	$this->log->logPrint("++++GetRainHour++++ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$row=$dbs->Query($strSql);
			$rain_hour += $row[0]+0;
		}
		$rain_hour = number_format($rain_hour,1, ".", "");
		if(is_object($this->log))	$this->log->logPrint("++++GetRainHour+++ 時間積算雨量[{$rain_hour}]",REC_DEBUG,__LINE__,__FILE__);
		return $rain_hour;
	}

	//日報集計値保存処理	[2006/12/21]
	function SetDailySumData($pnt_id,$curTime,$AryTmp){
		if($curTime > time()) return false;

		$date = DateConvert($curTime);

		if(is_object($this->log))	$this->log->logPrint("====SetDailySumData==== Start Target=> ".$date["date"],REC_DEBUG,__LINE__,__FILE__);
		/*
                    //WU900が24時の集計値にバグがあるため、24時の場合は23:50のデータを取得する  2006/07/27 kubota
                    if($date["hour"]=="24"){
                        if(is_object($this->log))	$this->log->logPrint("====SetDailySumData==== hour=> 24",REC_NOTE,__LINE__,__FILE__);
                        if($this->execute_collection){
                            $buf		= $this->SendCmd("IT,".date("Ym,dHi,", $curTime-600),0);
                        }else{
                            $buf		= $this->SendCmd("IT,".date("Ym,dHi,", $curTime-600));
                        }
                        //リトライ処理 Add 2013/10/03
                        if(trim($buf)==""){
                            sleep(5);
                            if(is_object($this->log))	$this->log->logPrint("====SetDailySumData==== hour=> 24[Retry]",REC_NOTE,__LINE__,__FILE__);
                            if($this->execute_collection){
                                $buf = $this->SendCmd("IT,".date("Ym,dHi,", $curTime-600),0);
                            }else{
                                $buf = $this->SendCmd("IT,".date("Ym,dHi,", $curTime-600));
                            }
                        }

                        $buf		= $this->ClearBuf($buf);
                        $AryTmp =	$this->ClearBuf2($buf);
                     if(trim($AryTmp[3])=="80"||trim($AryTmp[3])=="10"||trim($AryTmp[3])=="20") return true;
                    }
        */
		$data		= new Data;
		$AryRec	= $data->GetSumDataField("200");

		$args = $AryRec["40"];
		if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp[18])	? number_format($AryTmp[18],	1, ".", "")  	: "");
		if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp[19])	? sprintf("%02d",$AryTmp[19])							 	: "");
		if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp[22]) 	? number_format($AryTmp[22],1, ".", "")  		: "");
		if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp[23]) 	? number_format($AryTmp[23],1, ".", "")  		: "");
		if(array_key_exists("humid2",$args))			$args["humid2"]			=	(is_numeric($AryTmp[24]) 	? number_format($AryTmp[24],1, ".", "")  		: "");
		if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp[25]) 	? number_format($AryTmp[25],1, ".", "")  		: "");
		if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp[26]) 	? number_format($AryTmp[26],1, ".", "")  		: "");
		$AryRec["40"] = $args;

		$args = $AryRec["50"];
		if(array_key_exists("avg_ws",$args))					$args["avg_ws"]					=	(is_numeric($AryTmp[30])	? number_format($AryTmp[30],	1, ".", "")		: "");
		if(array_key_exists("avg_wd",$args))					$args["avg_wd"]					=	(is_numeric($AryTmp[31])	? sprintf("%02d",$AryTmp[31])								: "");
		if(array_key_exists("max_ws",$args))					$args["max_ws"]					=	(is_numeric($AryTmp[32])	? number_format($AryTmp[32],	1, ".", "") 	: "");
		if(array_key_exists("max_wd",$args))					$args["max_wd"] 				=	(is_numeric($AryTmp[33])	? sprintf("%02d",$AryTmp[33])								: "");
		if(array_key_exists("temp",$args))						$args["temp"]						=	(is_numeric($AryTmp[34]) 	? number_format($AryTmp[34],	1, ".", "")		: "");
		if(array_key_exists("humid1",$args))					$args["humid1"]					=	(is_numeric($AryTmp[35]) 	? number_format($AryTmp[35],	1, ".", "")  	: "");
		if(array_key_exists("press1",$args))					$args["press1"]					=	(is_numeric($AryTmp[37]) 	? number_format($AryTmp[37],	1, ".", "")  	: "");
		if(array_key_exists("press2",$args))					$args["press2"]					=	(is_numeric($AryTmp[38]) 	? number_format($AryTmp[38],	1, ".", "")  	: "");
		if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]	= (is_numeric($AryTmp[39]) 	? number_format($AryTmp[39],	1, ".", "")  	: "");
		if(array_key_exists("rain_hour",$args))				$args["rain_hour"]			= (is_numeric($AryTmp[40]) 	? number_format($AryTmp[40],	1, ".", "")  	: "");
		$AryRec["50"] = $args;

		$args = $AryRec["55"];
		if(array_key_exists("avg_ws",$args))					$args["avg_ws"]					=	$AryTmp[42];
		if(array_key_exists("max_ws",$args))					$args["max_ws"]					=	$AryTmp[44];
		if(array_key_exists("temp",$args))						$args["temp"]						=	$AryTmp[46];
		if(array_key_exists("humid1",$args))					$args["humid1"]					=	$AryTmp[47];
		if(array_key_exists("press1",$args))					$args["press1"]					=	$AryTmp[49];
		if(array_key_exists("press2",$args))					$args["press2"]					=	$AryTmp[50];
		if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]	=	$AryTmp[51];
		if(array_key_exists("rain_hour",$args))				$args["rain_hour"]			=	$AryTmp[52];
		$AryRec["55"] = $args;

		$args = $AryRec["60"];
		if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp[58]) 	? number_format($AryTmp[58],1, ".", "") 	: "");
		if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp[59]) 	? number_format($AryTmp[59],1, ".", "")  	: "");
		if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp[61]) 	? number_format($AryTmp[61],1, ".", "")  	: "");
		if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp[62]) 	? number_format($AryTmp[62],1, ".", "")  	: "");
		$AryRec["60"] = $args;

		$args = $AryRec["65"];
		if(array_key_exists("temp",$args))				$args["temp"]				=	$AryTmp[70];
		if(array_key_exists("humid1",$args))			$args["humid1"]			=	$AryTmp[71];
		if(array_key_exists("press1",$args))			$args["press1"]			=	$AryTmp[73];
		if(array_key_exists("press2",$args))			$args["press2"]			=	$AryTmp[74];
		$AryRec["65"] = $args;

		$args = $AryRec["70"];
		if(array_key_exists("rain_day",$args))		$args["rain_day"]	= (is_numeric($AryTmp[89]) 	? number_format($AryTmp[89],	1, ".", "")  	: "");
		$AryRec["70"] = $args;

		$args = $AryRec["90"];
		if(array_key_exists("avg_ws",$args))			$args["avg_ws"]	= join(",",array_slice($AryTmp,90,17));
		$AryRec["90"] = $args;

		$data->SaveSumData($pnt_id,"200",$curTime,$AryRec);
	}

	//日報集計データ取得	WU900にて時報作成時に集計値を保存してるので既存値を取得する	[2006/12/21]
	function GetDailySumData($pnt_id,$curTime,&$args){
		$data			= new Data;
		$AryData	= $data->LoadSumData($pnt_id,"200",$curTime);
		foreach($args as $sum_no =>	$value){
			foreach($value as $key => $val){
				$args[$sum_no][$key] = $AryData[$sum_no][$key];
			}
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetDailySumData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

//		//時報データ手動収集（取得データを連想配列に格納する	[2006/12/21]
//		function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
//			$this->execute_collection = 1;	//手動収集フラグON
//
//			//格納用連想配列初期化
//			$this->AryColData = array();
//
//			if($trg2!=""){
//				$time1 = DateConvert3($trg1);
//				$time2 = DateConvert3($trg2);
//			}else{
//				$time1 = DateConvert3($trg1);
//				$time2 = $time1;
//			}
//
//			//取得データ範囲を強制的に10分にする
//			$date = DateConvert($time1-60);
//			$trg1 = $date["year"].$date["month"].$date["day"].$date["hour"]."10";
//			$time1 = DateConvert3($trg1);
//
//			$data		= new Data;
//			$AryRec	= $data->GetDataField("100");
//			while($time1 <= $time2){
//				$date = DateConvert($time1);
//				$args = $AryRec;
//				//sleep(1);
//				$this->GetMinutelyData($pnt_id,$time1,$args);
//				$this->AryColData[ $date["date"] ] = $args;
//				$time1 += 600;
//			}
//
//			$this->SendCmd();
//			$this->execute_collection = 0;	//手動収集フラグOFF
//			return true;
//		}

//		//時報データ手動収集（連想配列に格納したデータを取得	[2006/12/21]
//		function GetMinutelyCollectionData($pnt_id,$curTime,&$args){
//			$date = DateConvert($curTime);
//			if(is_array($this->AryColData[ $date["date"] ])){
//				$AryTmp = $this->AryColData[ $date["date"] ];
//				if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp["avg_ws"])			? number_format($AryTmp["avg_ws"],			1, ".", "")  : "");
//				if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp["avg_wd"])			? $AryTmp["avg_wd"] : "");
//				if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	(is_numeric($AryTmp["max_ws"])			? number_format($AryTmp["max_ws"],			1, ".", "")  : "");
//				if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	(is_numeric($AryTmp["max_wd"])			? $AryTmp["max_wd"] : "");
//				if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp["temp"]) 				? number_format($AryTmp["temp"],				1, ".", "")  : "");
//				if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp["humid1"]) 			? number_format($AryTmp["humid1"], 			1, ".", "")  : "");
//				if(array_key_exists("humid2",$args))			$args["humid2"]			=	(is_numeric($AryTmp["humid2"]) 			? number_format($AryTmp["humid2"], 			1, ".", "")  : "");
//				if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp["press1"]) 			? number_format($AryTmp["press1"],			1, ".", "")  : "");
//				if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp["press2"]) 			? number_format($AryTmp["press2"],			1, ".", "")  : "");
//				if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	= (is_numeric($AryTmp["rain_10min"])	? number_format($AryTmp["rain_10min"],	1, ".", "")  : "");
//				if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	= (is_numeric($AryTmp["rain_hour"]) 	? number_format($AryTmp["rain_hour"],		1, ".", "")  : "");
//				if(array_key_exists("rain_day",$args))		$args["rain_day"]		= (is_numeric($AryTmp["rain_day"]) 		? number_format($AryTmp["rain_day"],		1, ".", "")  : "");
//			}
//			return true;
//		}

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
		$cmd = $this->CreateCmd($cmd);
		if(is_object($this->log))	$this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
		fwrite($this->fp,$cmd);
		$flg = 1;
		$buf = "";
		while( (!feof($this->fp) && $flg)){
			$tmp = fgetc($this->fp);

			//応答電文の先頭がSTX以外なら強制終了
			if($buf==""&&chr(2)!=$tmp){
				if(is_object($this->log))	$this->log->logPrint("STX Get Error",REC_SOCK,__LINE__,__FILE__);
				$flg = 0;
			}

			$buf .= $tmp;
			if(chr(3)==$tmp){
				//ETXを読み込んだら残りのBCCを読み込んで終了
				$bcc_chk 	= @fgetc($this->fp);
				$bcc_chk .= @fgetc($this->fp);
				$flg = 0;
			}
			$debug_ord .= "[".ord($tmp)."]";	//Debug
			$debug_chr .= "[".$tmp."]";				//Debug
		}
		if(is_object($this->log))	$this->log->logPrint("debug_ord:[{$debug_ord}]",REC_DEBUG,__LINE__,__FILE__);	//Debug
		if(is_object($this->log))	$this->log->logPrint("debug_chr:[{$debug_chr}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

		//BCCチェック
		$bcc = $this->BCC(substr($buf,1));
		if($bcc[0].$bcc[1]==$bcc_chk){
			if(is_object($this->log))	$this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("BCC Check Error [".$bcc_chk."]==[".$bcc[0].$bcc[1]."]",REC_DEBUG,__LINE__,__FILE__);
			$buf = "";
		}

		if($close){
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
		}
		return $buf;
	}

	//電文作成 [2006/12/21]
	function CreateCmd($cmd){
		$strCmd = $cmd.chr(03);
		$bcc = $this->BCC($strCmd);
		return chr(02).$strCmd.$bcc[0].$bcc[1];
	}

	function SettingWU900($ip,$port,$st1,$st2,$pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$this->setting["pnt_id"]		= $pnt_id;
		$this->setting["lgr_humid"]	= $st1;
		$this->setting["lgr_alt"]		= $st2;

		$ip		= trim($ip);
		$port	= trim($port);
		$st1	= trim($st1);
		$st2	= trim($st2);

		if(trim($ip)=="") 		return "IPを設定してください";
		if(trim($port)=="")		return "TCPポートを設定してください";
		if(trim($st1)=="") 		return "前日実効湿度を設定してください";
		if(trim($st2)=="")		return "標高を設定してください";
		if(!is_numeric($st1)) return "前日実効湿度は整数部２桁、少数部１桁の数値で入力してください";
		if(!is_numeric($st2)) return "標高は整数部３桁の数値で入力してください";

		if(!preg_match("/[0-9]{2}\.[0.9]/", $st1)){
			return "前日実効湿度は整数部２桁、少数部１桁の数値で入力してください";
		}

		if(strpos(" ".$st2,".")>0)	return "標高は整数部４桁の数値で入力してください";
		if(strlen($st2)>4)					return "標高は整数部４桁の数値で入力してください";

		$this->setting["lgr_ip"]		= $ip;
		$this->setting["lgr_port"]	= $port;

		$st1 = str_replace(".","",$st1);
		$st1 = sprintf("%03d",$st1);
		$st2 = sprintf("%04d",$st2);

		$date1 = date("Ym");
		$date2 = date("dHi");

		$buf = $this->SendCmd2("CT,{$date1},{$date2},{$st1},{$st2},");

		$buf = $this->ClearBuf($buf);

		$buf		= $this->ClearBuf($buf);
		$AryTmp =	$this->ClearBuf2($buf);

		$chk1	= explode(",","CR,{$date1},{$date2},{$st1},{$st2},");
		$chk2	= $AryTmp;

		//if(is_object($this->log))	$this->log->logPrint($chk1."===".$chk2,REC_SOCK,__LINE__,__FILE__);

		if($chk1[0]	==$chk2[0]){
			$this->SaveLoggerInfo($this->setting);
			return "OK";
		}else{
			return "書き込みできません";
		}
	}


///////////////////////////////////
	//電文送信 [2007/01/30]
	function SendCmd2($cmd="",$close=1){

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
		$cmd = $this->CreateCmd2($cmd);
		if(is_object($this->log))	$this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
		fwrite($this->fp,$cmd);
		$flg = 1;
		$buf = "";
		while( (!feof($this->fp) && $flg)){
			$tmp = fgetc($this->fp);

			//応答電文の先頭がSTX以外なら強制終了
			if($buf==""&&chr(2)!=$tmp){
				if(is_object($this->log))	$this->log->logPrint("STX Get Error",REC_SOCK,__LINE__,__FILE__);
				$flg = 0;
			}

			$buf .= $tmp;
			if(chr(3)==$tmp){
				//ETXを読み込んだら残りのBCCを読み込んで終了
				$bcc_chk 	= @fgetc($this->fp);
				$bcc_chk .= @fgetc($this->fp);
				$flg = 0;
			}
			$debug_ord .= "[".ord($tmp)."]";	//Debug
			$debug_chr .= "[".$tmp."]";				//Debug
		}
		if(is_object($this->log))	$this->log->logPrint("debug_ord:[{$debug_ord}]",REC_DEBUG,__LINE__,__FILE__);	//Debug
		if(is_object($this->log))	$this->log->logPrint("debug_chr:[{$debug_chr}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

//			//BCCチェック
//			$bcc = $this->BCC2(substr($buf,1));
//			if($bcc[0].$bcc[1]==$bcc_chk){
//				if(is_object($this->log))	$this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__);
//			}else{
//				if(is_object($this->log))	$this->log->logPrint("BCC Check Error [".$bcc_chk."]==[".$bcc[0].$bcc[1]."]",REC_DEBUG,__LINE__,__FILE__);
//				$buf = "";
//			}

		if($close){
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
		}
		return $buf;
	}
	//電文作成 [2006/12/21]
	function CreateCmd2($cmd){
		$strCmd = $cmd.chr(03);
		$bcc = $this->BCC2($strCmd);
		return chr(02).$strCmd.$bcc[0].$bcc[1];
	}
	//電文用BCC作成	[2006/12/29]
	function BCC2($str){
		for($i = 0; $i<strLen($str); $i++){
			$tmp = substr($str, $i, 1);
			$tmp = ord($tmp);
			$tmp = sprintf("%b",$tmp);
			$tmp = sprintf("%08d",$tmp);
			if($tmp0==""){
				$tmp0 = $tmp;
			}else{
				$tmp0 = $tmp0 ^ $tmp;
			}
		}
		$ret[0] = strtoupper(dechex(bindec(substr($tmp0, 0, strlen($tmp0) - 4))));
		$ret[1] = strtoupper(dechex(bindec(substr($tmp0, strlen($tmp0) - 4, 4))));
		return $ret;
	}
/////////////////////////////////////////



}
?>