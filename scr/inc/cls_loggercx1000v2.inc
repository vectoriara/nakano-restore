<?
//==========================================================//
//	CX1000ロガークラス										//
//----------------------------------------------------------//
//	処理概要												//
//	　CX1000用												//
//	　丹波市用として再調整バージョン						//
//----------------------------------------------------------//
//	変更履歴												//
//		2019/02/13	N.ARAI									//
//		・新規作成											//
//==========================================================//
class LoggerCX1000V2 extends BaseLogger
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2019.02.13";
	var $memo		= "";
	var $id			= "CX1000V2";
	var $type		= "1";	//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
	var $ReCal		= "1";	//再計算処理可否
	var $map		= array(
		"lgr_id"		=> "lgr_id",
		"lgr_pnt_id"	=> "lgr_pnt_id",
		"lgr_enable"	=> "lgr_enable",
		"lgr_time"		=> "lgr_time",
		"lgr_ip"		=> "lgr_col_1",
		"lgr_port"		=> "lgr_col_2",
		"lgr_no"		=> "lgr_col_3",
		"lgr_rain"		=> "lgr_col_4",// ※勝手に削除はしないこと
		"lgr_snow"		=> "lgr_col_5",// ※勝手に削除はしないこと
		"lgr_reset_rui"	=> "lgr_col_6"
	);
	var $SettingTemplate = "lgr_cx1000v2.tmpl";

	var $cx1000_input_rain_mode = true;	// ロガー設定項目に雨量入力切替の有無（CX1000限定）
	var $cx1000_input_lgr_snow = true;	// ロガー設定項目に積雪入力切替の有無（CX1000限定）

	var $EditSys	= array("100","200");	//編集可能なデータ帳票
	var $ColSys		= array("100");			//手動収集可能なデータ帳票
	var $DelSys		= array("100","200");	//削除可能なデータ帳票

	var $Recal1minFlg	= true;				//1分データを使用しての計算が可能／不可能
	var $RecalDisregard	= false;			//1分データチェックを無視して処理をするかの可／否

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
			//"rain_fall",// ※未検証のフィールドはコメントアウトとする
			"weather",
			"rain_rui",
			//"snow_level",// ※未検証のフィールドはコメントアウトとする
			//"move_snow_10min",// ※未検証のフィールドはコメントアウトとする
			//"move_snow_hour",// ※未検証のフィールドはコメントアウトとする
			"ins_day",
			"sun_day",
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
			"rain_day",
			//"rain_fall",// ※未検証のフィールドはコメントアウトとする
			//"snow_level",// ※未検証のフィールドはコメントアウトとする
			//"move_snow_10min",// ※未検証のフィールドはコメントアウトとする
			//"move_snow_hour",// ※未検証のフィールドはコメントアウトとする
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time",
			"ins_day",
			"sun_day"
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
			//"rain_fall",// ※未検証のフィールドはコメントアウトとする
			"weather",
			//"snow_level",// ※未検証のフィールドはコメントアウトとする
			//"move_snow_hour",// ※未検証のフィールドはコメントアウトとする
			"ins_day",
			"sun_day"
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
			//"rain_fall",// ※未検証のフィールドはコメントアウトとする
			"weather1",
			"weather2",
			"weather3",
			//"snow_level",// ※未検証のフィールドはコメントアウトとする
			//"snow_day",// ※未検証のフィールドはコメントアウトとする
			"ins_day",
			"sun_day"
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
			"rain_month",
			//"rain_fall",// ※未検証のフィールドはコメントアウトとする
			//"snow_level",// ※未検証のフィールドはコメントアウトとする
			//"snow_month",// ※未検証のフィールドはコメントアウトとする
			"ins_month",
			"sun_month"
		)
	);

	//帳票毎でこのロガーが編集できる項目を定義
	var $EditField = array(
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
			"rain_day",
			//"rain_fall",// ※未検証のフィールドはコメントアウトとする
			//"snow_level",// ※未検証のフィールドはコメントアウトとする
			//"move_snow_10min",// ※未検証のフィールドはコメントアウトとする
			//"move_snow_hour",// ※未検証のフィールドはコメントアウトとする
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time",
			"ins_day",
			"sun_day"
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
			//"rain_fall",// ※未検証のフィールドはコメントアウトとする
			"weather",
			//"snow_level",// ※未検証のフィールドはコメントアウトとする
			//"move_snow_hour",// ※未検証のフィールドはコメントアウトとする
			"ins_day",
			"sun_day"
		)
	);

	//局のロガー設定を取得 [2019/02/13]
	function LoadLoggerInfo($pnt_id){
		$strSql = "SELECT * FROM t_lgrdat WHERE lgr_id = '".$this->id."' AND lgr_pnt_id = '".$pnt_id."';";
		$dbs 	= new CONN;
		if(!$row=$dbs->Query($strSql)){
			if(is_object($this->log)) $this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		foreach($this->map as $key => $val){
			$this->setting[$key] = $row[$val];
		}
		$this->setting["lgr_enable_value"]	= ($this->setting["lgr_enable"]	? "checked" : "");
		$this->setting["lgr_time_value"] 	= ($this->setting["lgr_time"]	? "checked" : "");

		// ロガー設定項目に雨量入力切替の有無（CX1000限定）
		if($this->cx1000_input_rain_mode){
			$this->setting["lgr_rain"] = is_numeric($this->setting["lgr_rain"]) ? $this->setting["lgr_rain"] : "0";
			$this->setting["lgr_rain_value".$this->setting["lgr_rain"]] = " selected";
			$this->setting["lgr_rain_def"] = 1;
		}else{
			//設定機能OFFの場合は強制的に入力有り
			$this->setting["lgr_rain"] = 1;
		}
		// ロガー設定項目に積雪入力切替の有無（CX1000限定）
		if($this->cx1000_input_lgr_snow){
			$this->setting["lgr_snow"]	= is_numeric($this->setting["lgr_snow"]) ? $this->setting["lgr_snow"] : "0";
			$this->setting["lgr_snow_value".$this->setting["lgr_snow"]] = " selected";
			$this->setting["lgr_snow_def"] = 1;
		}else{
			//設定機能OFFの場合は強制的に入力無し
			$this->setting["lgr_snow"] = 0;
		}

		$this->load_flg = 1;
		return $this->setting;
	}

	//通信チェック [2007/02/17]
	function CheckConnection($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		$head	= sprintf("%02d",$this->setting["lgr_no"])."R2,";
		$date	= DateConvert(time()-60);
		$trg	= $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		$buf	= $this->SendCmd($head.$trg.",".$trg.chr(13).chr(10));
		$buf	= $this->ClearBuf($buf);
		return $buf;
	}

	//時刻補正 [2007/02/17]
	function SetDate($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$head = sprintf("%02d",$this->setting["lgr_no"])."C1,";
			$buf = $this->SendCmd($head.date("Y/m/d H:i:s").chr(13).chr(10));

			//結果チェック
			$buf = trim($buf);
			if($buf=="OK"){
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

	//現在データ取得処理 [2011/12/10]
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		$head = sprintf("%02d",$this->setting["lgr_no"])."R2,";
		$foot = sprintf("%02d",$this->setting["lgr_no"])."A2";

		$date	= DateConvert($curTime);
		$trg	= $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];

		if(!$buf=$this->SendCmd($head.$trg.",".$trg.chr(13).chr(10))){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}

		$buf	= $this->ClearBuf($buf);
		$AryTmp = explode("\r",$buf);
		$AryTmp = $this->ClearBuf2($AryTmp[0]);

		//データチェック
		$chkDate = date("Y/m/d H:i",$curTime);

		$date = DateConvert($curTime);
		$chkDate = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		if($AryTmp[0]==$foot&&$AryTmp[1]==$chkDate){
			$AryData = $this->SetPresentLoggerData($AryTmp);

			if(array_key_exists("avg_ws",$args))	$args["avg_ws"]	= $AryData["avg_ws"];
			if(array_key_exists("avg_wd",$args))	$args["avg_wd"]	= $AryData["avg_wd"];
			$AryWind = $this->GetMaxWind($pnt_id,$curTime,$AryData["ins_max_ws"],$AryData["ins_max_wd"]);
			if(array_key_exists("max_ws",$args))	$args["max_ws"]	= is_numeric($AryWind["max_ws"]) ? $AryWind["max_ws"]	: "";
			if(array_key_exists("max_wd",$args))	$args["max_wd"]	= is_numeric($AryWind["max_wd"]) ? $AryWind["max_wd"]	: "";
			if(array_key_exists("temp",$args))		$args["temp"]	= $AryData["temp"];
			if(array_key_exists("humid1",$args))	$args["humid1"]	= $AryData["humid1"];
			if(array_key_exists("humid2",$args))	$args["humid2"]	= $AryData["humid2"];
			if(array_key_exists("press1",$args))	$args["press1"]	= $AryData["press1"];
			if(array_key_exists("press2",$args))	$args["press2"]	= $AryData["press2"];

			if($this->setting["lgr_snow"]){
				if(array_key_exists("snow_level",$args)) $args["snow_level"] = $AryData["snow_level"];

				//移動積算降雪深取得
				$ArySnow = $this->GetPresentMoveSnow($pnt_id,$curTime,$args);
				if(array_key_exists("snow_1min",$args)) $args["snow_1min"] = $ArySnow["snow_1min"];
				if(array_key_exists("move_snow_10min",$args)) $args["move_snow_10min"] = $ArySnow["move_snow_10min"];
				if(array_key_exists("move_snow_hour",$args)) $args["move_snow_hour"] = $ArySnow["move_snow_hour"];
			}else{
				if(array_key_exists("snow_level",$args)) $args["snow_level"] = "-";
			}

			if(array_key_exists("ins_day",$args)) $args["ins_day"] = $AryData["ins_day"];
			if(array_key_exists("sun_day",$args)) $args["sun_day"] = $AryData["sun_day"];

			if(array_key_exists("rain_1min",$args)) $args["rain_1min"] = $AryData["rain_1min"];
			if(array_key_exists("move_rain_10min",$args)) $args["move_rain_10min"] = $AryData["move_rain_10min"];
			if(array_key_exists("move_rain_hour",$args)) $args["move_rain_hour"] = $AryData["move_rain_hour"];
			if(array_key_exists("rain_day",$args)) $args["rain_day"] = $AryData["rain_day"];
			if(array_key_exists("rain_rui",$args)) $args["rain_rui"] = $AryData["rain_rui"];
			if(array_key_exists("rain_fall",$args)) $args["rain_fall"] = $AryData["rain_fall"];

			//移動積算雨量取得
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);
			//if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	= $AryMove["move_rain_10min"];
			//if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		= $AryMove["move_rain_hour"];
			if(array_key_exists("move_rain_day",$args)) $args["move_rain_day"] = $AryMove["move_rain_day"];

			//1分積算をもとに積算雨量データを取得
			$rain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);
			if(array_key_exists("rain_10min",$args)) $args["rain_10min"] = is_numeric($rain["rain_10min"]) ? number_format($rain["rain_10min"],1, ".", "") : "";
			if(array_key_exists("rain_hour",$args)) $args["rain_hour"] = is_numeric($rain["rain_hour"]) ? number_format($rain["rain_hour"],	1, ".", "") : "";

			// ※もしかしたら現在データ取得時に累計雨量を記録する必要がある場合の想定関数（未実装）
			//累計雨量記録処理
//			if(array_key_exists("rain_rui",$args)){
//				$this->SetRuiRainPresent(
//					$pnt_id,
//					$curTime,
//					$args["rain_rui"],
//					$args["rain_rui_start_time"],
//					$args["rain_rui_start_time"],
//					$args["rain_rui_end_time"],
//					$args["rain_rui"]
//				);
//			}

			// ロガー累計雨量リセット設置値を記録
			$rain_path = ROOT_INC."rain_reset_".$pnt_id.".dat";
			$rain_buf = date("Y/m/d H:i",$curTime).",".$AryData["rain_rui_limit"];
			@file_put_contents($rain_path,$rain_buf);

			//天気取得
			if(array_key_exists("weather",$args)) $args["weather"] = $this->GetWeather($pnt_id);

			///// エラー情報保存 /////
			$path = ROOT_INC."HK_{$pnt_id}.dat";
			$line = @file($path);
			if(!is_array($line)) $line = array();
			$AryHK = array();
			foreach($line as $value){
				$AryTmp = explode(",",$value);
				$AryHK[ $AryTmp[0] ] = trim($AryTmp[1]).",".trim($AryTmp[2]).",".trim($AryTmp[3]);
			}
			$AryBuf = array();
			$time1 = $curTime - 360;
			$time2 = $curTime;
			while($time1<$time2){
				$trg = date("Y/m/d H:i",$time1);
				$AryBuf[] = $trg.",".$AryHK[ $trg ];
				$time1 += 60;
			}
			$trg = date("Y/m/d H:i",$time1);
			$tmp  = $trg.",";
			$tmp .= sprintf("%016s",base_convert(sprintf("%04s",dechex($AryData["HK1"])),16,2)).",";
			$tmp .= sprintf("%016s",base_convert(sprintf("%04s",dechex($AryData["HK2"])),16,2)).",";
			$tmp .= sprintf("%016s",base_convert(sprintf("%04s",dechex($AryData["HK3"])),16,2)).",";
			$AryBuf[] = $tmp;

			$test_log = new Recorder("bit_test_{$pnt_id}");
			if(is_object($test_log)) $test_log->logPrint($tmp,REC_DEBUG,__LINE__,__FILE__);
			if($fp=@fopen($path,"w")){
				@fputs($fp,join("\n",$AryBuf)."\n");
				@fclose($fp);
				@chmod($path,0777);
			}
			///// エラー情報保存 /////
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//時報データ取得処理 [2011/12/10]
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$head = sprintf("%02d",$this->setting["lgr_no"])."R3,";
		$foot = sprintf("%02d",$this->setting["lgr_no"])."A3";
		$date = DateConvert($curTime);
		$trg = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];

		if(!$buf=$this->SendCmd($head.$trg.",".$trg.chr(13).chr(10))){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf	= $this->ClearBuf($buf);
		$AryTmp = explode("\r",$buf);
		$AryTmp = $this->ClearBuf2($AryTmp[0]);

		//データチェック
		$date = DateConvert($curTime);
		$chkDate = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		if($AryTmp[0]==$foot&& $AryTmp[1]==$chkDate){
			//print $buf."\n";
			$AryData = $this->SetMinutelyLoggerData($AryTmp);
			print_r($AryData);

			if(array_key_exists("avg_ws",$args))	$args["avg_ws"]	= $AryData["avg_ws"];
			if(array_key_exists("avg_wd",$args))	$args["avg_wd"]	= $AryData["avg_wd"];
			if(array_key_exists("max_ws",$args))	$args["max_ws"]	= $AryData["max_ws"];
			if(array_key_exists("max_wd",$args))	$args["max_wd"]	= $AryData["max_wd"];
			if(array_key_exists("temp",$args))		$args["temp"]	= $AryData["temp"];
			if(array_key_exists("humid1",$args))	$args["humid1"]	= $AryData["humid1"];
			if(array_key_exists("humid2",$args))	$args["humid2"]	= $AryData["humid2"];
			if(array_key_exists("press1",$args))	$args["press1"]	= $AryData["press1"];
			if(array_key_exists("press2",$args))	$args["press2"]	= $AryData["press2"];
			if($this->setting["lgr_snow"]){
				if(array_key_exists("snow_level",$args)) $args["snow_level"] = $AryData["snow_level"];

				$ArySnow = $this->GetMinutelyMoveSnow($pnt_id,$curTime,$args);
				if(array_key_exists("move_snow_10min",$args)) $args["move_snow_10min"] = $ArySnow["move_snow_10min"];
				if(array_key_exists("move_snow_hour",$args)) $args["move_snow_hour"] = $ArySnow["move_snow_hour"];
			}else{
				if(array_key_exists("snow_level",$args)) $args["snow_level"] = "-";
			}

			if(array_key_exists("ins_day",$args))	$args["ins_day"]	= $AryData["ins_day"];
			if(array_key_exists("sun_day",$args))	$args["sun_day"]	= $AryData["sun_day"];

			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	= $AryData["rain_10min"];
			//if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	= $this->GetMinutelyRainHour($pnt_id,$curTime,$args["rain_10min"]);
			if(array_key_exists("rain_day",$args))		$args["rain_day"]	= $AryData["rain_day"];
			if(array_key_exists("rain_fall",$args))		$args["rain_fall"]	= $AryData["rain_fall"];

			if(array_key_exists("rain_hour",$args))					$args["rain_hour"]	= $AryData["move_rain_hour"];
			if(array_key_exists("rain_10min_max",$args))			$args["rain_10min_max"]	= $AryData["rain_10min_max"];
			if(array_key_exists("rain_10min_max_time",$args))		$args["rain_10min_max_time"] = $AryData["rain_10min_max_time"];
			if(array_key_exists("move_rain_hour_max",$args))		$args["move_rain_hour_max"]	= $AryData["move_rain_hour_max"];
			if(array_key_exists("move_rain_hour_max_time",$args))	$args["move_rain_hour_max_time"] = $AryData["move_rain_hour_max_time"];

			$this->SetRuiRainMinutely(
				$pnt_id,
				$curTime,
				$AryData["rain_rui"],			// 累計雨量
				$AryData["rain_rui_start_day"],	// 降り始め日
				$AryData["rain_rui_start_time"],// 降り始め時間
				$AryData["rain_rui_end_time"]	// 最終降雨時間
			);
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//時報データ手動収集（取得データを連想配列に格納する	[2006/12/31]
	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$head = sprintf("%02d",$this->setting["lgr_no"])."R3,";
		$foot = sprintf("%02d",$this->setting["lgr_no"])."A3";

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

		$cmd = $head.date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time2).chr(13).chr(10);
		if(!$buf=$this->SendCmd($cmd)){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf = $this->ClearBuf($buf);
		$AryTmp0 = explode("\r",$buf);

		$data = new Data;
		$AryRec	= $data->GetDataField("100");

		foreach($AryTmp0 as $line){
			$AryTmp	= explode(",",trim($line));
			if(!is_array($AryTmp))	break;
			if($AryTmp[0]!=$foot)	break;
			if($AryTmp[1]=="END")	break;

			$year	= substr($AryTmp[1],0,4);
			$month	= substr($AryTmp[1],5,2);
			$day	= substr($AryTmp[1],8,2);
			$hour	= substr($AryTmp[1],11,2);
			$min	= substr($AryTmp[1],14,2);
			$trg	= $year.$month.$day.$hour.$min;
			$args	= $AryRec;

			$AryData = $this->SetMinutelyLoggerData($AryTmp);

			if(array_key_exists("avg_ws",$args))	$args["avg_ws"]	=	$AryData["avg_ws"];
			if(array_key_exists("avg_wd",$args))	$args["avg_wd"]	=	$AryData["avg_wd"];
			if(array_key_exists("max_ws",$args))	$args["max_ws"]	=	$AryData["max_ws"];
			if(array_key_exists("max_wd",$args))	$args["max_wd"]	=	$AryData["max_wd"];
			if(array_key_exists("temp",$args))		$args["temp"]	=	$AryData["temp"];
			if(array_key_exists("humid1",$args))	$args["humid1"]	=	$AryData["humid1"];
			if(array_key_exists("humid2",$args))	$args["humid2"]	=	$AryData["humid2"];
			if(array_key_exists("press1",$args))	$args["press1"]	=	$AryData["press1"];
			if(array_key_exists("press2",$args))	$args["press2"]	=	$AryData["press2"];

			if($this->setting["lgr_snow"]){
				if(array_key_exists("snow_level",$args)) $args["snow_level"] = $AryData["snow_level"];
				$ArySnow = $this->GetMinutelyColMoveSnow($pnt_id,DateConvert3($trg),$args);
				if(array_key_exists("move_snow_10min",$args))	$args["move_snow_10min"]	= $ArySnow["move_snow_10min"];
				if(array_key_exists("move_snow_hour",$args))	$args["move_snow_hour"]		= $ArySnow["move_snow_hour"];
			}else{
				if(array_key_exists("snow_level",$args))	$args["snow_level"] = "-";
			}

			if(array_key_exists("ins_day",$args))	$args["ins_day"]	= $AryData["ins_day"];
			if(array_key_exists("sun_day",$args))	$args["sun_day"]	= $AryData["sun_day"];

			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	= $AryData["rain_10min"];
			//if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	= $this->GetMinutelyColRainHour($pnt_id,DateConvert3($trg),$args["rain_10min"]);
			if(array_key_exists("rain_day",$args))		$args["rain_day"]	= $AryData["rain_day"];
			if(array_key_exists("rain_fall",$args))		$args["rain_fall"]	= $AryData["rain_fall"];

			if(array_key_exists("rain_hour",$args))					$args["rain_hour"]	= $AryData["move_rain_hour"];
			if(array_key_exists("rain_10min_max",$args))			$args["rain_10min_max"]	= $AryData["rain_10min_max"];
			if(array_key_exists("rain_10min_max_time",$args))		$args["rain_10min_max_time"] = $AryData["rain_10min_max_time"];
			if(array_key_exists("move_rain_hour_max",$args))		$args["move_rain_hour_max"]	= $AryData["move_rain_hour_max"];
			if(array_key_exists("move_rain_hour_max_time",$args))	$args["move_rain_hour_max_time"] = $AryData["move_rain_hour_max_time"];

			$this->AryColData[ $trg ] = $args;
		}
		$this->execute_collection = 0;	//手動収集フラグOFF
		return true;
	}

	//日報集計データ取得 [2011/11/26]
	function GetDailySumData($pnt_id,$curTime,&$args){
		//日付設定
		$date = DateConvert($curTime);
		$trg1 = $date["year"].$date["month"].$date["day"]."0000";
		$trg2 = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];

		$dbs = new CONN;

		if($this->Recal1minFlg){
			if(!$this->RecalDisregard){
				$tbl	= "t000_dat_".$date["year"];
				$strSql = "SELECT COUNT(*) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND t000_date BETWEEN '{$trg1}' AND '{$trg2}' AND t000_code IS NULL";
				if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== DATA COUNT[".$strSql."]",REC_DEBUG,__LINE__,__FILE__);
				$row = $dbs->Query($strSql);
				$rec_count = $row[0]+0;
				if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== REC COUNT[".$rec_count."]",REC_DEBUG,__LINE__,__FILE__);
				$chk_count = (DateConvert3($trg2) - DateConvert3($trg1)) / 60;
				if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== CHK COUNT[".$chk_count."]",REC_DEBUG,__LINE__,__FILE__);

				$parcent = sprintf("%5.1f",$rec_count / $chk_count * 100);
				if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== Record [".$parcent."%]",REC_DEBUG,__LINE__,__FILE__);
				if($parcent<80){
					if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== CALL[BaseLogger::GetDailySumData]",REC_DEBUG,__LINE__,__FILE__);
					return BaseLogger::GetDailySumData($pnt_id,$curTime,$args);
				}
			}
		}else{
			if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== CALL[BaseLogger::GetDailySumData]",REC_DEBUG,__LINE__,__FILE__);
			return BaseLogger::GetDailySumData($pnt_id,$curTime,$args);
		}
		if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== CALL[CX1000::GetDailySumData]",REC_DEBUG,__LINE__,__FILE__);

		$ArySumFld = array(
			"avg_ws [平均風速]平均値" 		=> array("40","avg_ws",	"000","avg_ws"),
			"avg_ws [平均風速]最高値" 		=> array("50","avg_ws",	"000","avg_ws"),
			"avg_ws [平均風速]最高値起時"	=> array("55","avg_ws",	"000","avg_ws"),
			"avg_wd [平均風向]平均値"		=> array("40","avg_wd",	"000","avg_wd"),
			"avg_wd [平均風向]最高値"		=> array("50","avg_wd",	"000","avg_wd"),
			"max_ws [最大風速]最高値" 		=> array("50","max_ws",	"000","max_ws"),
			"max_ws [最大風速]最高値起時" 	=> array("55","max_ws",	"000","max_ws"),
			"max_wd	[最大風向]最高値"		=> array("50","max_wd",	"000","max_wd"),
			"temp	[気温]平均"				=> array("40","temp",	"000","temp"),
			"temp  	[気温]最高値" 			=> array("50","temp",	"000","temp"),
			"temp  	[気温]最高値起時"		=> array("55","temp",	"000","temp"),
			"temp  	[気温]最低値" 			=> array("60","temp",	"000","temp"),
			"temp  	[気温]最低値起時"		=> array("65","temp",	"000","temp"),
			"humid1	[相対湿度]平均" 		=> array("40","humid1",	"000","humid1"),
			"humid1	[相対湿度]最高値" 		=> array("50","humid1",	"000","humid1"),
			"humid1	[相対湿度]最高値起時" 	=> array("55","humid1",	"000","humid1"),
			"humid1	[相対湿度]最低値" 		=> array("60","humid1",	"000","humid1"),
			"humid1	[相対湿度]最低値起時"	=> array("65","humid1",	"000","humid1"),
			"humid2	[実効湿度]平均" 		=> array("40","humid2",	"000","humid2"),
			"press1	[現地気圧]平均" 		=> array("40","press1",	"000","press1"),
			"press1	[現地気圧]最高値" 		=> array("50","press1",	"000","press1"),
			"press1	[現地気圧]最高値起時" 	=> array("55","press1",	"000","press1"),
			"press1	[現地気圧]最低値" 		=> array("60","press1",	"000","press1"),
			"press1	[現地気圧]最低値起時"	=> array("65","press1",	"000","press1"),
			"press2	[海面気圧]平均" 		=> array("40","press2",	"000","press2"),
			"press2	[海面気圧]最高値" 		=> array("50","press2",	"000","press2"),
			"press2	[海面気圧]最高値起時" 	=> array("55","press2",	"000","press2"),
			"press2	[海面気圧]最低値" 		=> array("60","press2",	"000","press2"),
			"press2	[海面気圧]最低値起時"	=> array("65","press2",	"000","press2"),

			//"max_rain_10min [10分最大]最高値"		=> array("50","move_rain_10min","000","max_rain_10min"),
			//"max_rain_10min [10分最大]最高値起時"	=> array("55","move_rain_10min","000","max_rain_10min"),
			//"rain_hour   	[時間積算]最高値"		=> array("50","move_rain_hour",	"000","rain_hour"),
			//"rain_hour   	[時間積算]最高値起時" 	=> array("55","move_rain_hour",	"000","rain_hour"),
			"max_rain_10min [10分最大]最高値"	=> array("50","rain_10min_max",		"100","max_rain_10min"),
			"move_rain_hour [時間積算]最高値"	=> array("50","move_rain_hour_max",	"100","rain_hour"),

			"rain_day		[日積算]積算値"			=> array("70","rain_day",		"000","rain_day"),

			"rain_fall   [降雨強度]最高値"			=> array("50","rain_fall",	"000","rain_fall"),
			"rain_fall   [降雨強度]最高値起時"		=> array("55","rain_fall",	"000","rain_fall"),

			"ins_day	   [日射]積算値" => array("70","ins_day","000","ins_day"),
			"sun_day	   [日照]積算値" => array("70","sun_day","000","sun_day"),

			"snow_level		[積雪深]最高値"		=> array("50","snow_level",		"000","snow_level"),
			"snow_level		[積雪深]最高値起時"	=> array("55","snow_level",		"000","snow_level"),
			"move_snow_hour	[降雪深]最高値"		=> array("50","move_snow_hour",	"000","move_snow_hour"),
			"move_snow_hour	[積雪深]最高値起時"	=> array("55","move_snow_hour",	"000","move_snow_hour"),
			"move_snow_hour	[降雪深]積算値"		=> array("70","move_snow_10min","100","move_snow_hour"),

			"water_10min [水位]平均" 		=> array("40","water_10min","000","water_10min"),
			"water_10min [水位]最高値" 		=> array("50","water_10min","000","water_10min"),
			"water_10min [水位]最高値起時"	=> array("55","water_10min","000","water_10min"),
			"water_10min [水位]最低値" 		=> array("60","water_10min","000","water_10min"),
			"water_10min [水位]最低値起時"	=> array("65","water_10min","000","water_10min"),
			"風向頻度"						=> array("90","avg_ws",		"200","avg_ws")
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
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' GROUP BY {$fld} ORDER BY COUNT(*) DESC LIMIT 1;";
						if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
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
						if(is_numeric($args[$sum_no][$tmp_fld_id])){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $tmp_fld_id = '".$args[$sum_no][$tmp_fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
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

						if($fld_id=="max_ws"){///最大瞬間風速特殊対応
							$strWhere = "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '".substr($trg1,0,8)."0010' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
							$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						}

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
					if($fld_id=="max_ws"){///最大瞬間風速特殊対応
						$sum = $sum_no - 5;
						if(is_numeric($args[$sum][$fld_id])){
							$strWhere = "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '".substr($trg1,0,8)."0010' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
							$strSql = "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
							if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row 	= $dbs->Query($strSql);
							$time1 	= DateConvert3($row[$fld_date])-540;
							$time2 	= DateConvert3($row[$fld_date]);
							$date1 	= DateConvert($time1);
							$date2 	= DateConvert($time2);
//								//日をまたいだ場合
//								if($date1["day"]!=$date2["day"]){
//									$time3 	= DateConvert3($row[$fld_date]) + 60;
//									$date3 	= DateConvert($time3);
//									//再検索
//									$strWhere = "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '".$date3["date"]."' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
//									$strSql		= "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY {$fld_date} LIMIT 1;";
//									if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
//									$row 		= $dbs->Query($strSql);
//									$time1 	= DateConvert3($row[$fld_date])-540;
//									$time2 	= DateConvert3($row[$fld_date]);
//									$date1 	= DateConvert($time1);
//									$date2 	= DateConvert($time2);
//								}
							//再検索
							$strWhere = "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '".$date1["date"]."' AND '".$date2["date"]."' AND t{$sys_no}_code IS NULL";
							$strSql = "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY {$fld_date} LIMIT 1;";
							if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							$date = DateConvert2($row[$fld_date]);
							$args[$sum_no][$fld_id] = $date["hour"].":".$date["min"];
						}
						break;
					}

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
						if(is_object($this->log)) $this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						$date = DateConvert2($row[$fld_date]);
						$args[$sum_no][$fld_id] = $date["hour"].":".$date["min"];
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

	//電文送信 [2007/02/17]
	function SendCmd($cmd="",$close=1){
		// 引数なしで呼び出した場合ファイルハンドルクローズ
		if($cmd==""&&$close){
			if(!$this->fp) return true;
			if(is_object($this->log)) $this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
			return true;
		}
		// ファイルハンドルが既にオープンされているかチェック
		if(!$this->fp){
			$ip		= $this->setting["lgr_ip"];
			$port	=	$this->setting["lgr_port"];

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

		// 電文送信
		if(is_object($this->log)) $this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
		fwrite($this->fp,$cmd);
		$flg	= 1;
		$buf	= "";
		$l		= "";
		$debug_ord = "";
		$debug_chr = "";
		while( (!feof($this->fp) && $flg)){
			$tmp  = fgetc($this->fp);
			$buf .= $tmp;
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
				fwrite($this->fp, "OK".chr(13).chr(10));
				$l="";
			}else{
				$l .= $tmp;
			}
			$debug_ord .= "[".ord($tmp)."]";	//Debug
			$debug_chr .= "[".$tmp."]";			//Debug
		}
		if(is_object($this->log))	$this->log->logPrint("debug_ord:[{$debug_ord}]",REC_DEBUG,__LINE__,__FILE__);	//Debug
		if(is_object($this->log))	$this->log->logPrint("debug_chr:[{$debug_chr}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

		if(is_object($this->log))	$this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__);

		if($close){
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
		}
		return $buf;
	}

	// ロガー電文を各項目にセパレートする
	function SetPresentLoggerData($AryTmp){
		// スラッシュはすべて空白に変換
		for($i=2;$i<count($AryTmp);$i++) $AryTmp[$i] = trim(str_replace("/","",$AryTmp[$i]));

		$AryData = array();

		//平均風向
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="AD"){
				$AryData["avg_wd"] = is_numeric($AryTmp[$i+1]) ? sprintf("%02d",$AryTmp[$i+1]) : "";
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
		//１分間最大瞬間風速時風向風速
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="WS"){
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["ins_max_ws"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","") : "";
						$AryData["ins_max_wd"] = is_numeric($AryTmp[$j+2]) ? sprintf("%02d",$AryTmp[$j+2]) : "";
						break 2;
					}
				}
				break;
			}
		}
		//気温
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="T1"){
				$AryData["temp"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
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
		//現地気圧
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="BP"){
				$AryData["press1"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
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
		//積雪
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SN"){
				$AryData["snow_level"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1],0,".","") : "";
				if($AryData["snow_level"]<0) $AryData["snow_level"] = 0;
				break;
			}
		}
		//日積算日射
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="IX"){
				$AryData["ins_day"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/100,2,".","") : "";
				break;
			}
		}
		//日積算日照
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SX"){
				$AryData["sun_day"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1],0,".","") : "";
				break;
			}
		}
		//1分積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RA"){
				$AryData["rain_1min"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//10分移動積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RT"){
				$AryData["move_rain_10min"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//10分移動積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RC"){
				$AryData["move_rain_hour"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
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
		//降雨強度
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RI"){
				$AryData["rain_fall"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//累計雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RS"){
				$AryData["rain_rui"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				$AryData["rain_rui_start_day"]	= trim($AryTmp[$i+3]);	// 降り始め日
				$AryData["rain_rui_start_time"]	= trim($AryTmp[$i+4]);	// 降り始め時間
				$AryData["rain_rui_limit"]		= trim($AryTmp[$i+6]);	// 累計雨量リセット時間
				$AryData["rain_rui_end_time"]	= trim($AryTmp[$i+7]);	// 最終降雨時間
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
		//HK2
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="H2"){
				$AryData["HK2"] = trim($AryTmp[$i+1]);
				break;
			}
		}
		//HK3
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="H3"){
				$AryData["HK3"] = trim($AryTmp[$i+1]);
				break;
			}
		}
		return $AryData;
	}

	// 10分ロガー電文を各項目にセパレートする
	function SetMinutelyLoggerData($AryTmp){
		// スラッシュはすべて空白に変換
		for($i=2;$i<count($AryTmp);$i++) $AryTmp[$i] = trim(str_replace("/","",$AryTmp[$i]));

		$AryData = array();

		//平均風向
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="AD"){
				$AryData["avg_wd"] = is_numeric($AryTmp[$i+1]) ? sprintf("%02d",$AryTmp[$i+1]) : "";
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
		//１0分間最大瞬間風速時風向風速
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="WS"){
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["max_ws"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","") : "";
						$AryData["max_wd"] = is_numeric($AryTmp[$j+3]) ? sprintf("%02d",$AryTmp[$j+3]) : "";
						break 2;
					}
				}
				break;
			}
		}
		//気温
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="T1"){
				$AryData["temp"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
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
		//現地気圧
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="BP"){
				$AryData["press1"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
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
		//積雪
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SN"){
				$AryData["snow_level"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1],0,".","") : "";
				if($AryData["snow_level"]<0) $AryData["snow_level"] = 0;
				break;
			}
		}
		/*
		//10分積算日射
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="IS"){
				$AryData["ins_10min"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//時間積算日射
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="IZ"){
				$AryData["ins_hour"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/100,2,".","") : "";
				break;
			}
		}
		*/
		//日積算日射
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="IX"){
				$AryData["ins_day"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/100,2,".","") : "";
				break;
			}
		}
		/*
		//10分積算日照
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SD"){
				$AryData["sun_10min"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1],0,".","") : "";
				break;
			}
		}
		//時間積算日照
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SZ"){
				$AryData["sun_hour"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1],0,".","") : "";
				break;
			}
		}
		*/
		//日積算日照
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="SX"){
				$AryData["sun_day"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1],0,".","") : "";
				break;
			}
		}
		//10分積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RA"){
				$AryData["rain_10min"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				break;
			}
		}
		//10分積算雨量起時
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RT"){
				$AryData["rain_10min_max"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				$AryData["rain_10min_max_time"] = trim($AryTmp[$i+2]);
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
		//1時間移動積算雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RC"){
				$AryData["move_rain_hour"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				for($j=$i;$i<count($AryTmp);$j++){
					if($AryTmp[$j]=="MX"){
						$AryData["move_rain_hour_max"] = is_numeric($AryTmp[$j+1]) ? number_format($AryTmp[$j+1]/10,1,".","") : "";
						$AryData["move_rain_hour_max_time"] = trim($AryTmp[$j+2]);
						break 2;
					}
				}
				break;
			}
		}
		/*
		//降雨強度
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RI"){
				$AryData["rain_fall"] = is_numeric($AryTmp[$i+3]) ? number_format($AryTmp[$i+3]/10,1,".","") : "";
				break;
			}
		}
		*/
		//累計雨量
		for($i=2;$i<count($AryTmp);$i++){
			if($AryTmp[$i]=="RS"){
				$AryData["rain_rui"] = is_numeric($AryTmp[$i+1]) ? number_format($AryTmp[$i+1]/10,1,".","") : "";
				$AryData["rain_rui_start_day"]	= trim($AryTmp[$i+3]);	// 降り始め日
				$AryData["rain_rui_start_time"] = trim($AryTmp[$i+4]);	// 降り始め時間
				$AryData["rain_rui_limit"]		= trim($AryTmp[$i+6]);	// 累計雨量リセット時間
				$AryData["rain_rui_end_time"]	= trim($AryTmp[$i+7]);	// 最終降雨時間

				// おまえのコードが当てにならないのでカット
//				preg_match("@^[0-9]{4}@", $AryTmp[1], $m);
//				$year = isset($m[0]) ? $m[0] : "";
//
//				#年はアテにならない。が、t500テーブルに入っているのでこの値は最初しか使われないと想定する。
//				$AryData["rain_rui_start_time"] = trim($AryTmp[$i+3]).trim($AryTmp[$i+4])!="" ?
//					$year."/".preg_replace('/([0-9]{2})([0-9]{2})/', '$1/$2', $AryTmp[$i+3])." ".$AryTmp[$i+4] :
//					"";
//				$AryData["rain_rui_end_time"]   = $AryTmp[$i+7] != "" ?
//					$year."/".preg_replace('/([0-9]{2})([0-9]{2})/', '$1/$2', $AryTmp[$i+3])." ".$AryTmp[$i+7] :
//					"";
				break;
			}
		}
		return $AryData;
	}

	//1時間積算雨量算出（時報定時収集時用）
	function GetMinutelyRainHour($pnt_id,$curTime,$rain_10min){
		if(!is_numeric($rain_10min)) return "";
		$rain_hour = $rain_10min;

		if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."++++ [定期処理時] rain_10min-> {$rain_hour}",REC_DEBUG,__LINE__,__FILE__);
		$dbs	= new CONN;
		$now	= DateConvert($curTime);
		$prev	= DateConvert($curTime-60);
		$tbl	= "t100_dat_".$now["year"];
		$trg	= "t100_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["date"]."'";
		$strSql = "SELECT SUM(TO_NUMBER(rain_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND rain_10min <> '' AND {$trg};";
		if(is_object($this->log)) $this->log->logPrint("++++".__FUNCTION__."++++ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row=$dbs->Query($strSql);
		$rain_hour += $row[0]+0;
		$rain_hour = number_format($rain_hour,1, ".", "");
		if(is_object($this->log)) $this->log->logPrint("++++".__FUNCTION__."++++ 時間積算雨量[{$rain_hour}]",REC_DEBUG,__LINE__,__FILE__);
		return $rain_hour;
	}

	// 1時間積算雨量算出（時報手動収集時用）
	function GetMinutelyColRainHour($pnt_id,$curTime,$rain_10min){
		if(!is_numeric($rain_10min)) return "";
		$rain_hour = $rain_10min;

		if(is_object($this->log)) $this->log->logPrint("----".__FUNCTION__."---- [手動収集時] rain_10min-> {$rain_hour}",REC_DEBUG,__LINE__,__FILE__);
		if(date("i",$curTime)!="10"){
			$start = date("i",$curTime)=="00" ? "50" : date("i",$curTime) - 10;
			for($i=$start;$i>=10;$i-=10){
				$date = DateConvert($curTime-$i*60);
				if(is_array($this->AryColData[ $date["date"] ])){
					$AryTmp = $this->AryColData[ $date["date"] ];
					$rain_hour += $AryTmp["rain_10min"];
					if(is_object($this->log))	$this->log->logPrint("----".__FUNCTION__."---- [". $date["date"]."] + ".$AryTmp["rain_10min"],REC_DEBUG,__LINE__,__FILE__);
				}
			}
		}
		$rain_hour = number_format($rain_hour,1, ".", "");
		if(is_object($this->log))	$this->log->logPrint("----".__FUNCTION__."---- 時間積算雨量[{$rain_hour}]",REC_DEBUG,__LINE__,__FILE__);
		return $rain_hour;
	}

	//現在データから10分移動最大瞬間風速風向の算出
	function GetMaxWind($pnt_id,$curTime,$ws,$wd){
		$path = ROOT_INC."wind_{$pnt_id}.dat";
		$line = @file($path);
		$AryTmp = explode(",",$line[0]);
		$old_curTime = strtotime($AryTmp[0]);
		$old_ws = $AryTmp[1] + 0;
		$old_wd = $AryTmp[2];
		$flg = 0;

		if($old_curTime > 0){
			$this->put_ws_and_wd($pnt_id, $curTime, $ws, $wd);
			if($old_ws < $ws){
				$old_curTime = $curTime;
				$old_ws = $ws;
				$old_wd = $wd;
				$flg = 1;
			}else{
				if($old_curTime < ($curTime-600)){
					$max = $this->get_max_ws($pnt_id);
					$old_curTime = $max["curTime"];
					$old_ws = $max["ws"];
					$old_wd = $max["wd"];
					$flg = 1;
				}
			}
		}else{
			$old_curTime = $curTime;
			$old_ws = $ws;
			$old_wd = $wd;
			$flg = 1;
		}

		if($old_wd=="") $old_ws = "";
		if($flg){
			if($fp=@fopen($path,"w")){
				@fputs($fp,date("Y/m/d H:i",$old_curTime).",".$old_ws.",".$old_wd);
				@fclose($fp);
				@chmod($path,0777);
			}
		}
		$AryTmp = array();
		$AryTmp["max_ws"] = is_numeric($old_ws) ? number_format($old_ws,1,".","") : "";
		$AryTmp["max_wd"] = is_numeric($old_wd) ? sprintf("%02d",$old_wd) : "";
		if(is_object($this->log)) $this->log->logPrint("++++GetMaxWind++++ ".print_r($AryTmp,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $AryTmp;
	}

	/**
	 * 1分最大瞬間風速風向の保存
	 * @param $pnt_id
	 * @param $curTime
	 * @param $ws
	 * @param $wd
	 */
	private function put_ws_and_wd($pnt_id, $curTime, $ws, $wd){
		$path = ROOT_INC."wind_1min_{$pnt_id}.dat";

		if(file_exists($path) == false){
			touch($path);
			chmod($path, 0777);
		}

		$_1mins = file($path, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);

		//古い時間が入っていた場合は取り除く
		$_1mins_new = array();
		foreach($_1mins as $_1min){
			list($curTime_1min) = explode(",", $_1min);
			if($curTime_1min >= ($curTime-600)){
				$_1mins_new[] = $_1min;
			}
		}

		//10以上の値があった場合は取り除く(10分移動だから)
		if(count($_1mins_new) >= 10){
			array_shift($_1mins_new);
		}

		$_1mins_new[] = $curTime.",".$ws.",".$wd;
		if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."+++[".$curTime.",".$ws.",".$wd."]",REC_DEBUG,__LINE__,__FILE__);

		file_put_contents($path, implode("\n", $_1mins_new));
	}

	/**
	 * 1分最大瞬間風速風向から、過去10分の最大値を求めます
	 * @param $pnt_id
	 * @return array
	 */
	private function get_max_ws($pnt_id){
		$path = ROOT_INC."wind_1min_{$pnt_id}.dat";
		$max = array(
			"curTime" => 0,
			"ws" => 0,
			"wd" => 0
		);

		if(file_exists($path)){
			$_1mins = file($path, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);

			foreach($_1mins as $_1min){
				list($curTime, $ws, $wd) = explode(",", $_1min);
				if($ws >= $max["ws"]){
					$max["curTime"] = $curTime;
					$max["ws"] = $ws;
					$max["wd"] = $wd;
				}
			}
		}

		if(is_object($this->log))	$this->log->logPrint("++++".__FUNCTION__."+++[".print_r($max, true)."]",REC_DEBUG,__LINE__,__FILE__);
		return $max;
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
		if($reset_rain>99){
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Setting Over Value Error] value[".$reset_rain."]",REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		$head = sprintf("%02d",$this->setting["lgr_no"])."S1,";
		if(!$buf=$this->SendCmd($head.sprintf("%02d",$reset_rain).",".chr(13).chr(10))){
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
		return $flg;
	}

	/**
	 * ロガーから来た累計雨量のデータを保存します
	 */
	public function SetRuiRainMinutely($pnt_id,$curTime,$rain_rui,$rain_rui_start_day,$rain_rui_start_time,$rain_rui_end_time){
		$tmp = "rain_rui[".$rain_rui."] start_day[".$rain_rui_start_day."] start_time[".$rain_rui_start_time."] end_time[".$rain_rui_end_time."]";
		if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// ".$tmp,REC_DEBUG,__LINE__,__FILE__);

		// 累計雨量
		$rain_rui = number_format($rain_rui, 1, ".", "");
		// 累計雨量開始時刻
		$trg_start  = date("Y",$curTime-60)."-".substr($rain_rui_start_day,0,2)."-".substr($rain_rui_start_day,2,2)." ";
		$trg_start .= $rain_rui_start_time;
		// 累計雨量最終降雨時刻
		$trg_end  = date("Y-m-d",$curTime-60)." ".$rain_rui_end_time;

		$dbs = new CONN;

		$strSql = "SELECT * FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
		//if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		//if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// Rain t500_dat => ".print_r($row,TRUE),REC_DEBUG,__LINE__,__FILE__);
		if(!is_numeric($row["t500_no"])){
			//if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// 記録中累計雨量レコードなし",REC_DEBUG,__LINE__,__FILE__);

			// 累計雨量レコードなし＆電文上に累計雨量の記録が無い場合は処理しない
			if($rain_rui_start_day==""){
				if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// Rain Skip",REC_DEBUG,__LINE__,__FILE__);
				return;
			}

			// 電文上の開始時刻と同じ累計雨量レコードがないか検索
			$strSql = "SELECT * FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND t500_sdt = '".$trg_start."';";
			if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			$row_tmp = $dbs->Query($strSql);
			if($row_tmp["t500_no"]!=""){
				// 既に電文上の累計雨量開始時刻の累計雨量レコードがある場合は処理しない
				if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// t500_dat => ".print_r($row_tmp,TRUE),REC_DEBUG,__LINE__,__FILE__);
				if(is_object($this->log))	$this->log->logPrint("////".__FUNCTION__."//// 累計雨量記録済み",REC_DEBUG,__LINE__,__FILE__);
				return;
			}

			// 累計雨量記録開始
			$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'".$pnt_id."','".$trg_start."',NULL,'".$trg_start."','".$rain_rui."');";
			if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
			if(!$dbs->Execute($strSql)){
				if(is_object($this->log)) $this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
				return false;
			}
			if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// 累計雨量記録開始",REC_DEBUG,__LINE__,__FILE__);
		}else{
			// 累計雨量レコードあり

			if($rain_rui>$row["t500_value"]) {
				// 累計雨量レコード更新
				$strSql = "UPDATE t500_dat SET t500_value = ".$rain_rui.",t500_last = '".$trg_end."' WHERE t500_no = ".$row["t500_no"].";";
				if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// ".$strSql, REC_DEBUG, __LINE__, __FILE__);
				if($dbs->Execute($strSql)){
					if(is_object($this->log)) $this->log->logPrint($dbs->getError(), REC_NOTE, __LINE__, __FILE__);
					return false;
				}
				if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// 累計雨量記録更新", REC_DEBUG, __LINE__, __FILE__);
			}elseif($rain_rui==0){
				// 累計雨量レコード終了
				$strSql = "UPDATE t500_dat SET t500_edt = '".$row["t500_last"]."' WHERE t500_pnt_id = '".$pnt_id."' AND t500_edt IS NULL;";
				if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
				if(!$dbs->Execute($strSql)){
					if(is_object($this->log)) $this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
					return false;
				}
				if(is_object($this->log)) $this->log->logPrint("////".__FUNCTION__."//// 累計雨量記録終了",REC_DEBUG,__LINE__,__FILE__);
			}
		}
		return;
	}
}
?>