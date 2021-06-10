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
class LoggerCK4500 extends BaseLogger
{
	//変数宣言
	var $varsion	= "1.0";
	var $update		= "2011.12.10";
	var $memo			= "";
	var $id				= "CK4500";
	var $type			= "2";	//ロガータイプ　1 => 気象ロガー	 2 => 雨量ロガー
	var $ReCal		= "1";	//再計算処理可否
	var $map	= array("lgr_id"				=> "lgr_id",
		"lgr_pnt_id"		=> "lgr_pnt_id",
		"lgr_enable"		=> "lgr_enable",
		"lgr_time"			=> "lgr_time",
		"lgr_ip"				=> "lgr_col_1",
		"lgr_port"			=> "lgr_col_2",
		"lgr_no"				=> "lgr_col_3"
	);

	var $EditSys	= array("100","200");	//編集可能なデータ帳票
	var $ColSys		= array("100");				//手動収集可能なデータ帳票
	var $DelSys		= array("100","200");	//削除可能なデータ帳票

	var $Recal1minFlg		= true;					//1分データを使用しての計算が可能／不可能
	var $RecalDisregard	= false;				//1分データチェックを無視して処理をするかの可／否

	//帳票毎でこのロガーが出力できる項目を定義
	var $OutField = array(
		"000" => array(	"snow_level",
			"move_snow_10min",
			"move_snow_hour"
		),
		"100" => array(	"snow_level",
			"move_snow_10min",
			"move_snow_hour"
		),
		"200" => array(	"snow_level",
			"move_snow_hour"
		),
		"300" => array(	"snow_level",
			"snow_day"
		),
		"400" => array(	"snow_level",
			"snow_month"
		)
	);

	//帳票毎でこのロガーが編集できる項目を定義
	var $EditField = array(
		"100" => array(	"snow_level",
			"move_snow_10min",
			"move_snow_hour"
		),
		"200" => array(	"snow_level",
			"move_snow_hour"
		)
	);


	//通信チェック
	function CheckConnection($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		return $this->SendCmd($this->setting["lgr_no"]."R2",date("Y/m/d H:i",time()).",".date("Y/m/d H:i",time()));
	}

	//時刻補正
	function SetDate($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$buf = $this->SendCmd($strCmd = $this->setting["lgr_no"]."C1,".date("Y/m/d H:i:s"));
			//結果チェック
			$buf = trim($buf);
			if($buf=="OK"){
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
		return $result;
	}


	//現在データ取得処理
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd($this->setting["lgr_no"]."R2",date("Y/m/d H:i",$curTime).",".date("Y/m/d H:i",$curTime))){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf		= $this->ClearBuf($buf);
		$AryTmp =	explode("\r",$buf);
		$AryTmp =	$this->ClearBuf2($AryTmp[0]);
		print_r($AryTmp);

		//データチェック
		$chkDate = date("Y/m/d H:i",$curTime);

//			$date		 = DateConvert($curTime);
//			$chkDate = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		$chkDate = date("Y/m/d H:i",$curTime);

		if($AryTmp[1]==$chkDate){
			$AryData = LoggerCX1000::SetPresentLoggerData($AryTmp);
			print_r($AryData);

			if(array_key_exists("snow_level",$args))	$args["snow_level"]	= $AryData["snow_level"];

			//移動積算降雪深取得
			$ArySnow = $this->GetPresentMoveSnow($pnt_id,$curTime,$args);
			if(array_key_exists("snow_1min",$args))				$args["snow_1min"]				= $ArySnow["snow_1min"];
			if(array_key_exists("move_snow_10min",$args))	$args["move_snow_10min"]	= $ArySnow["move_snow_10min"];
			if(array_key_exists("move_snow_hour",$args))	$args["move_snow_hour"]		= $ArySnow["move_snow_hour"];
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}


	function GetMinutelyData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd($this->setting["lgr_no"]."R2",date("Y/m/d H:i",$curTime).",".date("Y/m/d H:i",$curTime))){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf		= $this->ClearBuf($buf);
		$AryTmp =	explode("\r",$buf);
		$AryTmp =	$this->ClearBuf2($AryTmp[0]);

		//データチェック
		$chkDate = date("Y/m/d H:i",$curTime);

//			$date		 = DateConvert($curTime);
//			$chkDate = $date["year"]."/".$date["month"]."/".$date["day"]." ".$date["hour"].":".$date["min"];
		$chkDate = date("Y/m/d H:i",$curTime);
		if($AryTmp[1]==$chkDate){
			$AryData = LoggerCX1000::SetMinutelyLoggerData($AryTmp);

			if(array_key_exists("snow_level",$args))	$args["snow_level"]	= $AryData["snow_level"];

			//移動積算降雪深取得
			$ArySnow = $this->GetMinutelyMoveSnow($pnt_id,$curTime,$args);
			if(array_key_exists("move_snow_10min",$args))	$args["move_snow_10min"]	= $ArySnow["move_snow_10min"];
			if(array_key_exists("move_snow_hour",$args))	$args["move_snow_hour"]		= $ArySnow["move_snow_hour"];
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//時報データ手動収集（取得データを連想配列に格納する	[2006/12/31]
	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$head = $this->setting["lgr_no"]."R2";
		$foot = $this->setting["lgr_no"]."A2";

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

		$cmd = date("Y/m/d H:i",$time1).",".date("Y/m/d H:i",$time2);
		if(!$buf=$this->SendCmd($head,$cmd)){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf		= $this->ClearBuf($buf);
		$AryTmp0 =	explode("\r",$buf);

		$data		= new Data;
		$AryRec	= $data->GetDataField("100");

		foreach($AryTmp0 as $line){
			$AryTmp	= explode(",",trim($line));
			if(!is_array($AryTmp))	break;
			if($AryTmp[0]!=$foot)		break;
			if($AryTmp[1]=="END")		break;

			$year		= substr($AryTmp[1],0,4);
			$month	= substr($AryTmp[1],5,2);
			$day		= substr($AryTmp[1],8,2);
			$hour		= substr($AryTmp[1],11,2);
			$min		= substr($AryTmp[1],14,2);
			$date 	= DateConvert(strtotime($year."/".$month."/".$day." ".$hour.":".$min));
			$trg		= $date["date"];
			$args		= $AryRec;

			$AryData = LoggerCX1000::SetMinutelyLoggerData($AryTmp);

			if(array_key_exists("snow_level",$args))	$args["snow_level"] = $AryData["snow_level"];

			$ArySnow = $this->GetMinutelyColMoveSnow($pnt_id,DateConvert3($trg),$args);
			if(array_key_exists("move_snow_10min",$args))	$args["move_snow_10min"]	= $ArySnow["move_snow_10min"];
			if(array_key_exists("move_snow_hour",$args))	$args["move_snow_hour"]		= $ArySnow["move_snow_hour"];

			$this->AryColData[ $trg ] = $args;
		}
		$this->execute_collection = 0;	//手動収集フラグOFF
		return true;
	}

	//時報データ手動収集（連想配列に格納したデータを取得) [2007/01/31]
	function GetMinutelyCollectionData($pnt_id,$curTime,&$args){
		$date = DateConvert($curTime);
		if(is_array($this->AryColData[ $date["date"] ])){
			$AryTmp = $this->AryColData[ $date["date"] ];

			foreach($this->OutField["100"] as $key){
				if(!array_key_exists($key,$args))		continue;
				if(!array_key_exists($key,$AryTmp)) continue;
				if(!is_numeric($AryTmp[$key]))			continue;
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
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyCollectionData No Data",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}

	//日報集計データ取得	[2011/11/26]
	function GetDailySumData($pnt_id,$curTime,&$args){
		//日付設定
		$date = DateConvert($curTime);
		$trg1	= $date["year"].$date["month"].$date["day"]."0000";
		$trg2	= $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];

		$dbs = new CONN;

		if($this->Recal1minFlg){
			if(!$this->RecalDisregard){
				$tbl		= "t000_dat_".$date["year"];
				$strSql = "SELECT COUNT(*) FROM {$tbl} WHERE t000_pnt_id = '{$pnt_id}' AND t000_date BETWEEN '{$trg1}' AND '{$trg2}' AND t000_code IS NULL";
				if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== DATA COUNT[".$strSql."]",REC_DEBUG,__LINE__,__FILE__);
				$row		= $dbs->Query($strSql);
				$rec_count = $row[0]+0;
				if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== REC COUNT[".$rec_count."]",REC_DEBUG,__LINE__,__FILE__);
				$chk_count = (DateConvert3($trg2) - DateConvert3($trg1)) / 60;
				if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== CHK COUNT[".$chk_count."]",REC_DEBUG,__LINE__,__FILE__);

				$parcent = sprintf("%5.1f",$rec_count / $chk_count * 100);
				if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== Record [".$parcent."%]",REC_DEBUG,__LINE__,__FILE__);
				if($parcent<80){
					if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== CALL[BaseLogger::GetDailySumData]",REC_DEBUG,__LINE__,__FILE__);
					return BaseLogger::GetDailySumData($pnt_id,$curTime,$args);
				}
			}
		}else{
			if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== CALL[BaseLogger::GetDailySumData]",REC_DEBUG,__LINE__,__FILE__);
			return BaseLogger::GetDailySumData($pnt_id,$curTime,$args);
		}
		if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== CALL[CX1000::GetDailySumData]",REC_DEBUG,__LINE__,__FILE__);

		$ArySumFld = array(
			"avg_ws			[平均風速]平均値" 					=> array("40","avg_ws",					"000","avg_ws"),
			"avg_ws			[平均風速]最高値" 					=> array("50","avg_ws",					"000","avg_ws"),
			"avg_ws			[平均風速]最高値起時" 			=> array("55","avg_ws",					"000","avg_ws"),
			"avg_wd			[平均風向]平均値"						=> array("40","avg_wd",					"000","avg_wd"),
			"avg_wd			[平均風向]最高値"						=> array("50","avg_wd",					"000","avg_wd"),
			"max_ws			[最大風速]最高値" 					=> array("50","max_ws",					"000","max_ws"),
			"max_ws			[最大風速]最高値起時" 			=> array("55","max_ws",					"000","max_ws"),
			"max_wd			[最大風向]最高値"						=> array("50","max_wd",					"000","max_wd"),
			"temp				[気温]平均" 								=> array("40","temp",						"000","temp"),
			"temp  			[気温]最高値" 							=> array("50","temp",						"000","temp"),
			"temp  			[気温]最高値起時"						=> array("55","temp",						"000","temp"),
			"temp  			[気温]最低値" 							=> array("60","temp",						"000","temp"),
			"temp  			[気温]最低値起時"						=> array("65","temp",						"000","temp"),
			"humid1			[相対湿度]平均" 						=> array("40","humid1",					"000","humid1"),
			"humid1			[相対湿度]最高値" 					=> array("50","humid1",					"000","humid1"),
			"humid1			[相対湿度]最高値起時" 			=> array("55","humid1",					"000","humid1"),
			"humid1			[相対湿度]最低値" 					=> array("60","humid1",					"000","humid1"),
			"humid1			[相対湿度]最低値起時"				=> array("65","humid1",					"000","humid1"),
			"humid2			[実効湿度]平均" 						=> array("40","humid2",					"000","humid2"),
			"press1			[現地気圧]平均" 						=> array("40","press1",					"000","press1"),
			"press1			[現地気圧]最高値" 					=> array("50","press1",					"000","press1"),
			"press1			[現地気圧]最高値起時" 			=> array("55","press1",					"000","press1"),
			"press1			[現地気圧]最低値" 					=> array("60","press1",					"000","press1"),
			"press1			[現地気圧]最低値起時"				=> array("65","press1",					"000","press1"),
			"press2			[海面気圧]平均" 						=> array("40","press2",					"000","press2"),
			"press2			[海面気圧]最高値" 					=> array("50","press2",					"000","press2"),
			"press2			[海面気圧]最高値起時" 			=> array("55","press2",					"000","press2"),
			"press2			[海面気圧]最低値" 					=> array("60","press2",					"000","press2"),
			"press2			[海面気圧]最低値起時"				=> array("65","press2",					"000","press2"),
//													"max_rain_10min [10分最大]最高値"				=> array("50","rain_10min",			"000","max_rain_10min"),
//													"max_rain_10min [10分最大]最高値起時"		=> array("55","rain_10min",			"000","max_rain_10min"),
//													"rain_hour   [時間積算]最高値"					=> array("50","rain_hour",			"200","rain_hour"),
//													"rain_hour   [時間積算]最高値起時" 			=> array("55","rain_hour",			"200","rain_hour"),
			"max_rain_10min [10分最大]最高値"				=> array("50","move_rain_10min","000","max_rain_10min"),
			"max_rain_10min [10分最大]最高値起時"		=> array("55","move_rain_10min","000","max_rain_10min"),
			"rain_hour   [時間積算]最高値"					=> array("50","move_rain_hour",	"000","rain_hour"),
			"rain_hour   [時間積算]最高値起時" 			=> array("55","move_rain_hour",	"000","rain_hour"),
			"rain_day	   [日積算]積算値"						=> array("70","rain_day",				"000","rain_day"),
			"snow_level  [積雪深]最高値"						=> array("50","snow_level",			"000","snow_level"),
			"snow_level  [積雪深]最高値起時"				=> array("55","snow_level",			"000","snow_level"),
			"move_snow_hour  [降雪深]最高値"				=> array("50","move_snow_hour",	"000","move_snow_hour"),
			"move_snow_hour  [積雪深]最高値起時"		=> array("55","move_snow_hour",	"000","move_snow_hour"),
			"move_snow_hour  [降雪深]積算値"				=> array("70","move_snow_10min","100","move_snow_hour"),
			"water_10min [水位]平均" 								=> array("40","water_10min",		"000","water_10min"),
			"water_10min [水位]最高値" 							=> array("50","water_10min",		"000","water_10min"),
			"water_10min [水位]最高値起時"					=> array("55","water_10min",		"000","water_10min"),
			"water_10min [水位]最低値" 							=> array("60","water_10min",		"000","water_10min"),
			"water_10min [水位]最低値起時"					=> array("65","water_10min",		"000","water_10min"),
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
					if(strpos(" ".$fld_id,"_wd")>0){
						//最大風速時風向
						$tmp_fld_id =	str_replace("_wd","_ws",$fld_id);
						if(is_numeric($args[$sum][$tmp_fld_id])){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $tmp_fld_id = '".$args[$sum][$tmp_fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
							if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = (int) $row[0];
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
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
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


	function SendCmd($cmd,$data=""){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$ip		= $this->setting["lgr_ip"];
		$port =	$this->setting["lgr_port"];

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
					fwrite($fp, "OK".chr(13).chr(10));
					$l="";
				}else{
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