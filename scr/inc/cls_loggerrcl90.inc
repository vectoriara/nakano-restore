<?
//==========================================================//
//	RCL80ロガークラス										//
//----------------------------------------------------------//
//	処理概要												//
//	　RCL80用												//
//----------------------------------------------------------//
//	変更履歴												//
//		2006/11/11	N.ARAI									//
//		・新規作成											//
//==========================================================//
class LoggerRCL90 extends BaseLogger
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2014.09.18";
	var $memo		= "";
	var $id			= "RCL90" ;
	var $type		= "2";	//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
	var $ReCal		= "1";	//再計算処理可否
	var $map		= array(
		"lgr_id"		=> "lgr_id",
		"lgr_pnt_id"	=> "lgr_pnt_id",
		"lgr_enable"	=> "lgr_enable",
		"lgr_time"		=> "lgr_time",
		"lgr_ip"		=> "lgr_col_1",
		"lgr_port"		=> "lgr_col_2"
	);
	var $EditSys	= array("100");	//編集可能なデータ帳票
	var $ColSys		= array("100");	//手動収集可能なデータ帳票
	var $DelSys		= array("100");	//削除可能なデータ帳票

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
		"000" => array(
			"rain_10min",
			"rain_hour",
			"rain_day",
			"move_rain_10min",
			"move_rain_hour",
			"move_rain_day",
			"rain_1min",
			"rain_rui"
		),
		"100" => array(
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time"
		),
		"200" => array(
			"max_rain_10min",
			"rain_hour",
			"rain_day",
		),
		"300" => array(
			"max_rain_10min",
			"max_rain_hour",
			"rain_day"
		),
		"400" => array(
			"max_rain_10min",
			"max_rain_hour",
			"max_rain_day",
			"rain_month"
		)
	);

	//帳票毎でこのロガーが編集できる項目を定義
	var $EditField = array(
		"100" => array(
			"rain_10min",
			"rain_hour",
			"rain_day",
			"rain_10min_max",
			"rain_10min_max_time",
			"move_rain_hour_max",
			"move_rain_hour_max_time"
		)
	);

	//通信チェック [2006/12/31]
	function CheckConnection($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		$buf = $this->SendCmd(chr(02)."D00000000000000".chr(03));
		$buf = $this->ClearBuf($buf);
		return $buf;
	}

	//時刻補正	[2006/12/31]
	function SetDate($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$buf = $this->SendCmd(chr(02)."T".date("YmdHis").chr(03));
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

	//現在データ取得処理 [2007/01/31]
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd(chr(02)."D00000000000000".chr(03))){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if($buf==chr(4)||$buf==chr(21)||$buf==chr(6)) return false;
		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//日付チェック
		$chkDate = date("H:i",$curTime);
		if(is_object($this->log)) $this->log->logPrint(print_r($AryTmp,true),REC_SOCK,__LINE__,__FILE__);
		if(is_object($this->log)) $this->log->logPrint($chkDate,REC_SOCK,__LINE__,__FILE__);
		if($AryTmp[1]==$chkDate){
			if($this->OutCheck("rain_day","000",$args))			$args["rain_day"] = (is_numeric($AryTmp[4]) ? number_format($AryTmp[4],1, ".", "") : "");
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args); //移動積算取得関数より1分積算と24時間移動積算を取得（その他の移動値はロガーを使用する）
			if($this->OutCheck("rain_1min","000",$args))		$args["rain_1min"] = $AryMove["rain_1min"];
			if($this->OutCheck("move_rain_day","000",$args))	$args["move_rain_day"] = $AryMove["move_rain_day"];
			$AryRain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);	//積算データ取得
			if($this->OutCheck("rain_10min","000",$args))		$args["rain_10min"] = $AryRain["rain_10min"];
			if($this->OutCheck("rain_hour","000",$args))		$args["rain_hour"] = $AryRain["rain_hour"];
			if($this->OutCheck("move_rain_10min","000",$args))	$args["move_rain_10min"] = (is_numeric($AryTmp[2]) ? number_format($AryTmp[2],1, ".", "") : "");
			if($this->OutCheck("move_rain_hour","000",$args))	$args["move_rain_hour"] = (is_numeric($AryTmp[3]) ? number_format($AryTmp[3],1, ".", "") : "");
			if($this->OutCheck("rain_rui","000",$args))			$args["rain_rui"] = (is_numeric($AryTmp[5]) ? number_format($AryTmp[5],1, ".", "") : "");
			//累計雨量算出＆記録処理
			#if($this->OutCheck("rain_rui","000",$args)) $args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
			//$this->SetRuiRain($pnt_id,$curTime,$args["rain_rui"]);
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//時報データ取得処理 [2007/01/31]
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if($curTime>time()) return false;
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$date = DateConvert($curTime);
		$cmd = chr(02)."B".$date["year"].$date["month"].$date["day"].$date["hour"].$date["min"]."00".chr(03);
		if(!$buf=$this->SendCmd($cmd,0)){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetMinutelyData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(!$this->execute_collection) $this->SendCmd();	//手動収集時以外はクローズ
		if($buf==chr(4)||$buf==chr(21)||$buf==chr(6)) return false;

		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//日付チェック
		$chkDate = $date["hour"].":".$date["min"];
		$chkTrg = $date["year"].":".$date["month"].":".$date["day"];
		if($AryTmp[1]==$chkDate){
			if($this->OutCheck("rain_10min","100",$args))	$args["rain_10min"]	= (is_numeric($AryTmp[2]) ? number_format($AryTmp[2],1, ".", "") : "");
			if($this->OutCheck("rain_hour",	"100",$args))	$args["rain_hour"]	= (is_numeric($AryTmp[5]) ? number_format($AryTmp[5],1, ".", "") : "");
			if($this->OutCheck("rain_day",	"100",$args))	$args["rain_day"]	= (is_numeric($AryTmp[8]) ? number_format($AryTmp[8],1, ".", "") : "");

			if($this->OutCheck("rain_10min_max","100",$args))			$args["rain_10min_max"] = (is_numeric($AryTmp[3]) ? number_format($AryTmp[3],1, ".", "") : "");
			if($this->OutCheck("rain_10min_max_time","100",$args))		$args["rain_10min_max_time"] = trim($AryTmp[4]);
			if($this->OutCheck("move_rain_hour_max","100",$args))		$args["move_rain_hour_max"]	= (is_numeric($AryTmp[6]) ? number_format($AryTmp[6],1, ".", "") : "");
			if($this->OutCheck("move_rain_hour_max_time","100",$args))	$args["move_rain_hour_max_time"] = trim($AryTmp[7]);

			if(is_object($this->log)) $this->log->logPrint("[___rain_10min_max_time___]".date("H:i",strtotime($AryTmp[0].$AryTmp[4])),REC_SOCK,__LINE__,__FILE__);

			if(!$this->execute_collection){
				//手動収集時以外の場合のみ処理を実施する
				//手動収集を実行してしまうと電文中に0.0が確認された時に降雨中データに振り終わりフラグが立ってしまう為
				$rain_rui_sdt = date("Y/m/d H:i",strtotime($AryTmp[10]));
				$rain_rui_time = date("Y/m/d H:i",strtotime($AryTmp[0].$AryTmp[11]));
				if(is_object($this->log))	$this->log->logPrint("////updateT500RainRui//// ".$rain_rui_sdt." : ".$AryTmp[10],REC_DEBUG,__LINE__,__FILE__);
				$this->updateT500RainRui($pnt_id, $AryTmp[9], $rain_rui_sdt, $rain_rui_time);
			}

			// ロガー累計雨量リセット設置値を記録
			if(!$this->execute_collection){
				$rain_path = ROOT_INC."rain_reset_".$pnt_id.".dat";
				$rain_buf = date("Y/m/d H:i",$curTime).",".$AryTmp[12];
				@file_put_contents($rain_path,$rain_buf);
			}
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	/**
	 * ロガーから来る10分累計雨量に関するデータを処理します。
	 * 降り始めになるとデータを新規登録し、降り最中はデータを更新し続けます。
	 * 雨量が0になった時は降り終わりとみなされ、降り終わり確定処理を行います。
	 *
	 * memo:
	 *      もうちょっと適切そうな名前があればリネームして下さい…。
	 *
	 * @param $pnt_id 局番号です
	 * @param $rain_rui 累計雨量です(ロガーから来る値のRS)
	 * @param $rain_rui_sdt 降り始め時間です(ロガーから来る値のFT)
	 * @param $rain_rui_time 降り続けている時間です。降り終わり時間ではない事に注意してください。(ロガーから来る値のET)
	 */
	private function updateT500RainRui($pnt_id,$rain_rui,$rain_rui_sdt,$rain_rui_time){
		$dbs = new CONN;

		$strSql = "SELECT * FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND t500_sdt = '{$rain_rui_sdt}';";
		if(is_object($this->log))	$this->log->logPrint("////updateT500RainRui//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);

		if(is_object($this->log))	$this->log->logPrint("////updateT500RainRui//// ".print_r($row, true),REC_DEBUG,__LINE__,__FILE__);

		if($row){
			#累計雨量カウント中
			if(isset($row["t500_value"]) && $rain_rui > $row["t500_value"]){
				$strSql = "UPDATE t500_dat SET t500_value = {$rain_rui},t500_last = '{$rain_rui_time}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_sdt = '{$rain_rui_sdt}';";
			}
		}else if($rain_rui > 0){
			#降り始め
			$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'{$pnt_id}','{$rain_rui_sdt}',NULL,'{$rain_rui_sdt}',{$rain_rui});";
		}else{
			#降り終わり確定
			$strSql = "UPDATE t500_dat SET t500_edt = CURRENT_TIMESTAMP(0) WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
		}

		if(is_object($this->log))	$this->log->logPrint("////updateT500RainRui//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs->Execute($strSql);
	}

	//時報データ手動収集（取得データを連想配列に格納する [2007/01/31]
	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		if(is_object($this->log))	$this->log->logPrint($this->id." GetLoggerMinutelyCollection Start",REC_DEBUG,__LINE__,__FILE__);

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
		$data	= new Data;
		$rec0	= $data->GetDataField("100");
		while($time1<=$time2){
			$date = DateConvert($time1);
			$args = $rec0;
			if($this->GetMinutelyData($pnt_id,$time1,$args)){
				$this->AryColData[ $date["date"] ] = $args;
			}
			$time1 += 600;
		}
		$this->SendCmd();
		$this->execute_collection = 0;	//手動収集フラグOFF
		return true;
	}

	//電文送信 [2007/01/31]
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

			$address = "tcp://{$ip}:{$port}";
			if(is_object($this->log))	$this->log->logPrint(print_r($this->setting, true),REC_SOCK,__LINE__,__FILE__);
			if(is_object($this->log))	$this->log->logPrint($address,REC_SOCK,__LINE__,__FILE__);
			$this->fp = @stream_socket_client($address, $errno, $errstr, 5);
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
		fwrite($this->fp, $cmd);
		$flg = 1;
		$buf = "";
		$byt_cnt = 0;
		while( (!feof($this->fp) && $flg)){
			$tmp  = fgetc($this->fp);
			$buf .= $tmp;
			if($tmp==chr(4)||$tmp==chr(21)||$tmp==chr(6))	$flg = 0;

			$debug_ord .= "[".ord($tmp)."]";	//Debug
			$debug_chr .= "[".$tmp."]";				//Debug
			$byt_cnt++;
			if($byt_cnt>5000){
				if(is_object($this->log))	$this->log->logPrint("send:Error Over Flow[{$byt_cnt}]",REC_SOCK,__LINE__,__FILE__);
				break;
			}
		}
		if(is_object($this->log))	$this->log->logPrint("debug_ord:[{$debug_ord}]",REC_DEBUG,__LINE__,__FILE__);	//Debug
		if(is_object($this->log))	$this->log->logPrint("debug_chr:[{$debug_chr}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

		if(is_object($this->log))	$this->log->logPrint("recv:[{$buf}]",REC_SOCK,__LINE__,__FILE__,0,0,array("EUC-JP","SJIS"));

		//クローズ処理
		if($close){
			if(is_object($this->log)) $this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
		}
		return $buf;
	}

	// 累計雨量リセット時間をロガーに設定する
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

		$year = '0000';
		$month = '00';
		$day = '00';
		$hour = '00';
		$min = '00';
		$sec = sprintf("%02d", $reset_rain);

		$cmd = chr(2)."W".$year.$month.$day.$hour.$min.$sec.chr(3);
		$buf = $this->SendCmd($cmd);
		$buf = trim($buf);

		if($buf===chr(6)){
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Result OK]",REC_NOTE,__LINE__,__FILE__);
			$flg = true;
		}else{
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Reset Rain Result NG]",REC_NOTE,__LINE__,__FILE__);
			$flg = false;
		}
		return $flg;
	}

	// 雨量ロガーのメモリをリセット
	public function ResetMemory($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$year = '0000';
		$month = '00';
		$day = '00';
		$hour = '00';
		$min = '00';
		$sec = '00';

		$cmd = chr(2)."R".$year.$month.$day.$hour.$min.$sec.chr(3);
		$buf = $this->SendCmd($cmd);
		$buf = trim($buf);

		if($buf===chr(6)){
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Memory Reset Result OK]",REC_NOTE,__LINE__,__FILE__);
			$flg = true;
		}else{
			if(is_object($this->log)) $this->log->logPrint("[{$pnt_id}][Memory Reset Result NG]",REC_NOTE,__LINE__,__FILE__);
			$flg = false;
		}
		return $flg;
	}

	// ※baselogger起時タイプに対応に伴い、GetDailySumData_dummyに名称変更（削除対象）　2019/02/23
	// というか雨量項目の処理なのに他の気象項目をろくに消さないでコピペしてる馬鹿の対応
	public function GetDailySumData_dummy($pnt_id,$curTime,&$args){
		//日付設定
		$date = DateConvert($curTime);
		$trg1	= $date["year"].$date["month"].$date["day"]."0000";
		$trg2	= $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];

		$dbs = new CONN;

		$ArySumFld = array(
			"avg_ws			[平均風速]平均値" 					=> array("40","avg_ws",					"100","avg_ws"),
			"avg_ws			[平均風速]最高値" 					=> array("50","avg_ws_max",			"100","avg_ws"),
			//"avg_ws			[平均風速]最高値起時" 			=> array("55","avg_ws",					"100","avg_ws"),
			"avg_wd			[平均風向]平均値"						=> array("40","avg_wd",					"100","avg_wd"),
			"avg_wd			[平均風向]最高値"						=> array("50","avg_wd_max",			"100","avg_wd"),
			"max_ws			[最大風速]最高値" 					=> array("50","max_ws",					"100","max_ws"),
			//"max_ws			[最大風速]最高値起時" 			=> array("55","max_ws",					"100","max_ws"),
			//"max_wd			[最大風向]最高値"						=> array("50","max_wd",					"100","max_wd"),

			"press1			[現地気圧]平均" 						=> array("40","press1",					"100","press1"),
			"press1			[現地気圧]最高値" 					=> array("50","press1_max",			"100","press1"),
			//"press1			[現地気圧]最高値起時" 			=> array("55","press1",					"100","press1"),
			"press1			[現地気圧]最低値" 					=> array("60","press1_min",			"100","press1"),
			//"press1			[現地気圧]最低値起時"				=> array("65","press1",					"100","press1"),

			"max_rain_10min [10分最大]最高値"				=> array("50","rain_10min_max",			"100","max_rain_10min"),
			#"rain_10min_max_time [10分最大]最高値起時"		=> array("55","rain_10min_max_time",			"100","max_rain_10min"),
////													"rain_hour   [時間積算]最高値"					=> array("50","rain_hour",			"200","rain_hour"),
////													"rain_hour   [時間積算]最高値起時" 			=> array("55","rain_hour",			"200","rain_hour"),
			"move_rain_hour   [時間積算]最高値"			=> array("50","move_rain_hour_max",	"100","rain_hour"),
			#"move_rain_hour_max_time   [時間積算]最高値起時"	=> array("55","move_rain_hour_max_time",	"200","max_move_rain_hour_time"),
			"rain_day	   [日積算]積算値"						=> array("70","rain_day",				"100","rain_day"),

			"風向頻度"								 							=> array("90","avg_ws",					"200","avg_ws")
		);

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

			if(!in_array($fld_id,$this->OutField["200"]))	continue;
			if(!array_key_exists($fld_id,$args[$sum_no])) continue;

			//集計データ取得処理
			if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== {$name}",REC_DEBUG,__LINE__,__FILE__);
			switch($sum_no){
				case "40":
					//平均
					if(strpos(" ".$fld_id,"_wd")>0){
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' GROUP BY {$fld} ORDER BY COUNT(*) DESC LIMIT 1;";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = $row[0];
					}else{
						$strSql = "SELECT AVG(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
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
					if($fld=="avg_ws_max"){	//平均風速最大値(CK4700特殊処理)
						$strSql = "SELECT * FROM {$tbl} WHERE {$strWhere} ORDER BY avg_ws_max DESC,t100_date DESC LIMIT 1;";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row["avg_ws_max"])){
							$args["50"][$fld_id] = $row["avg_ws_max"];
							$args["55"][$fld_id] = $row["avg_ws_max_time"];
						}
					}elseif($fld=="avg_wd_max"){//平均風速最大値(CK4700特殊処理)
						//最大風速時風向
						$tmp_fld_id =	str_replace("_wd","_ws",$fld_id);
						if(is_numeric($args[$sum_no][$tmp_fld_id])){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND {$tmp_fld_id} = '".$args[$sum_no][$tmp_fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
							if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = sprintf("%02d",(int) $row[0]);
						}
					}elseif($fld=="max_ws"||$fld=="press1_max"||$fld=="rain_10min_max"||$fld=="move_rain_hour_max"){	//平均風速最大値(CK4700特殊処理)
						$strSql = "SELECT * FROM {$tbl} WHERE {$strWhere} AND {$fld} <> ''ORDER BY TO_NUMBER({$fld},'9999.99') DESC,t100_date DESC LIMIT 1;";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						if(is_object($this->log))	$this->log->logPrint("_____".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row[$fld])){
							$args["50"][$fld_id] = $row[$fld];
							$args["55"][$fld_id] = $row[$fld."_time"];
							if($fld=="max_ws"){
								$args["50"]["max_wd"] = $row["max_wd"];
							}
							if(strpos(" ".$fld_id,"rain")>0){
								if($args["50"][$fld_id]==0){
									if(is_object($this->log))	$this->log->logPrint("雨量データ０[{$fld_id}]",REC_DEBUG,__LINE__,__FILE__);
									$args["55"][$fld_id] = "";
								}
							}
						}
					}elseif(strpos(" ".$fld_id,"_wd")>0){
						//最大風速時風向
						$tmp_fld_id =	str_replace("_wd","_ws",$fld_id);
						if(is_numeric($args[$sum_no][$tmp_fld_id])){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $tmp_fld_id = '".$args[$sum][$tmp_fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
							if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = sprintf("%02d",(int) $row[0]);
						}
					}else{
						//最高値算出
						$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
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
					if($fld=="press1_min"){	//平均風速最大値(CK4700特殊処理)
						$strSql = "SELECT * FROM {$tbl} WHERE {$strWhere} AND {$fld} <> ''ORDER BY TO_NUMBER({$fld},'9999.99'),t100_date DESC LIMIT 1;";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row[$fld])){
							$args["60"][$fld_id] = $row[$fld];
							$args["65"][$fld_id] = $row[$fld."_time"];
						}
					}else{
						$strSql = "SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
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
					}
					break;
				case "55":
				case "65":
					//起時
					$sum = $sum_no - 5;
					if(is_numeric($args[$sum][$fld_id])){
						if(strpos(" ".$fld_id,"rain")>0){
							if($args[$sum][$fld_id]==0){
								if(is_object($this->log))	$this->log->logPrint("雨量データ０ ".$strSql,REC_DEBUG,__LINE__,__FILE__);
								break;
							}
						}
						$strSql = "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData(55 or 65)==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						$date = DateConvert2($row[$fld_date]);
						if(is_object($this->log))	$this->log->logPrint("_____".print_r($fld_date, true),REC_DEBUG,__LINE__,__FILE__);
						if(is_object($this->log))	$this->log->logPrint("_____".print_r($row[$fld_date], true),REC_DEBUG,__LINE__,__FILE__);
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
					if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					$args[$sum_no][$fld_id] = $row[0];
					break;
				case "90":
					$args[$sum_no][$fld_id] = $this->GetDirection($pnt_id,"200",$curTime);
					break;
			}
			if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== [{$sum_no}][{$fld_id}] =>".$args[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
		}
		if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}
}
?>