<?
	class FAILURE
	{
		//変数宣言
		var $log;

		var $HK_MAP = array(
													array("no"=> 1, 	"name" => "",		"level" => 0),
													array("no"=> 2, 	"name" => "",		"level" => 0),
													array("no"=> 3, 	"name" => "",		"level" => 0),
													array("no"=> 4, 	"name" => "",		"level" => 0),
													array("no"=> 5, 	"name" => "",		"level" => 0),
													array("no"=> 6, 	"name" => "",		"level" => 0),
													array("no"=> 7, 	"name" => "",		"level" => 0),
													array("no"=> 8, 	"name" => "",		"level" => 0),
													array("no"=> 9, 	"name" => "",		"level" => 0),
													array("no"=> 10,	"name" => "雨量接点異常","level" => 1),
													array("no"=> 11,	"name" => "",		"level" => 0),
													array("no"=> 12, 	"name" => "点検中",	"level" => 0),
													array("no"=> 13, 	"name" => "",		"level" => 0),
													array("no"=> 14, 	"name" => "",		"level" => 0),
													array("no"=> 15, 	"name" => "",		"level" => 0),
													array("no"=> 16, 	"name" => "",		"level" => 0),
													//ここからはカスタム
													array("no"=> 17, 	"name" => "通信異常","level" => 2),//現在処理時通信エラー発生で検知
//													//地震計用
//													array("no"=> 15, 	"name" => "地震計　電源異常",				"level"	=> 2),
//													array("no"=> 16, 	"name" => "地震計　メモリ異常",			"level" => 2),
//													array("no"=> 17, 	"name" => "地震計　温度異常",				"level" => 1),
//													array("no"=> 18, 	"name" => "地震計　A/D異常",				"level" => 1),
//													array("no"=> 19, 	"name" => "地震計　オフセット異常",	"level" => 1),
//													array("no"=> 20, 	"name" => "地震計　点検異常",				"level" => 1),
//													array("no"=> 21, 	"name" => "地震計　長時間起動継続",	"level" => 1),
//													array("no"=> 22, 	"name" => "地震計　設定値異常",			"level" => 1),
//													array("no"=> 23, 	"name" => "地震計　通信異常[DATA1]","level" => 2),
//													array("no"=> 24, 	"name" => "地震計　通信異常[DATA2]","level" => 2),
//													array("no"=> 25, 	"name" => "地震計　通信異常[DATA3]","level" => 2),
//													array("no"=> 26, 	"name" => "地震計　時刻補正失敗",		"level" => 1),//maintenance_cron.php DATEにて検知
//													array("no"=> 27, 	"name" => "地震計　通信異常",				"level" => 2),//maintenance_cron.php QUAKEにて検知
//													array("no"=> 28, 	"name" => "気象観測中央装置（１系）障害","level" => 2),
//													array("no"=> 29, 	"name" => "気象観測中央装置（２系）障害","level" => 2),
												);

		//コンストラクタ
		function __construct(&$log = ""){
			if(is_object($log))	$this->log = $log;
			if(is_object($this->log)) $this->log->logPrint("[Defined]".get_class($this),REC_NOTE,__LINE__,__FILE__);
		}

		//故障情報解析＆発令、解除処理
		function ParseHK($pnt_id,$curTime,$buf){
			if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

			$flg	= false;	//発生時記録用フラグ
			$AryTmp = array();	//発生時記録用

			$bit  = sprintf("%04d",base_convert(substr($buf,0,1), 16, 2));
			$bit .= sprintf("%04d",base_convert(substr($buf,1,1), 16, 2));
			$bit .= sprintf("%04d",base_convert(substr($buf,2,1), 16, 2));
			$bit .= sprintf("%04d",base_convert(substr($buf,3,1), 16, 2));

			print "========= Start ParseHK [".$bit."] =========\n";

			for($i=0;$i<strlen($bit);$i++){
				$tmp_buf = "No".sprintf("%02d",$i)." bit[".substr($bit,$i,1)."] HK_MAP[".$this->HK_MAP[$i]["name"]."]";
				print $tmp_buf."\n";
				$AryTmp[] = $tmp_buf;//発生時記録用
				if(substr($bit,$i,1)=="1"){
					#$this->Save($pnt_id,$curTime,$this->HK_MAP[$i]);
					$flg = true;
				}else{
					#$this->Save($pnt_id,$curTime,$this->HK_MAP[$i],true);
				}
			}
			if($flg){
				$this->log->logPrint("Recv HK [".$buf."](".$bit.")",REC_NOTE,__LINE__,__FILE__);
				foreach($AryTmp as $val){
					$this->log->logPrint($val,REC_NOTE,__LINE__,__FILE__);
				}
			}
			if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
		}

		//故障情報マニュアル保存
		function SaveManualFailure($pnt_id,$curTime,$err_no,$relese=false,$ext_msg=""){
			foreach($this->HK_MAP as $val){
				if((int)$err_no==(int)$val["no"]){
					if($ext_msg!="") $val["name"] .= $ext_msg;
					return $this->Save($pnt_id,$curTime,$val,$relese);
				}
			}
			return false;
		}

		//故障情報保存　release => 発令中の同警報を解除する
		function Save($pnt_id,$curTime,$data,$release=false){
			//if(is_object($this->log)) $this->log->logPrint("[Start]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);

			if($data["name"]==""){
				//if(is_object($this->log)) $this->log->logPrint("Unknown HK_MAP".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
				//if(is_object($this->log)) $this->log->logPrint("[Exit False]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
				return false;
			}

			$dbs = new CONN;
			$row = $dbs->Query("SELECT COUNT(*) FROM t900_dat WHERE t900_pnt_id = '".$pnt_id."' AND t900_no = ".$data["no"]." AND t900_ldt IS NULL");

			$strSql = "";
			if($release){
				//フラグ解消検知時
				if($row[0]>0){
					if(is_object($this->log))	$this->log->logPrint("[".$data["name"]."]解除(".date("Y-m-d H:i:s",$curTime).")",REC_NOTE,__LINE__,__FILE__);
					$strSql = "UPDATE t900_dat SET t900_ldt = '".date("Y-m-d H:i:s",$curTime)."' WHERE t900_pnt_id = '".$pnt_id."' AND t900_no = ".$data["no"];
					/*
					//自分と同じレベルの警報を検索
					$row2 = $dbs->Query("SELECT COUNT(*) FROM t900_dat WHERE t900_level = ".$data["level"]." AND t900_ldt IS NULL");
					//結果が1レコードの場合は全解除にあたるのでSNMPを送信する
					if($row2[0]==1){
						if($data["level"]=="2"){
							if(is_object($this->log))	$this->log->logPrint("SNMP[重故障解除]送信",REC_NOTE,__LINE__,__FILE__);
							SendSNMP(4,$this->log);
						}
						if($data["level"]=="1"){
							if(is_object($this->log))	$this->log->logPrint("SNMP[軽故障解除]送信",REC_NOTE,__LINE__,__FILE__);
							SendSNMP(2,$this->log);
						}
					}
					*/
				}
			}else{
				//フラグ発生検知時
				if($row[0]<1){
					if(is_object($this->log)) $this->log->logPrint("[".$data["name"]."]検知(".date("Y-m-d H:i:s",$curTime).")",REC_NOTE,__LINE__,__FILE__);
					$strSql = "INSERT INTO t900_dat VALUES('".$pnt_id."','".date("Y-m-d H:i:s",$curTime)."',".$data["no"].",'".$data["name"]."',".$data["level"].",NULL);";
					/*
					//重故障の場合はトラップ情報を送信する
					if($data["level"]=="2"){
						//ここに処理を記述する
						if(is_object($this->log))	$this->log->logPrint("SNMP[重故障]送信",REC_NOTE,__LINE__,__FILE__);
						SendSNMP(3,$this->log);
					}
					//軽故障の場合はトラップ情報を送信する
					if($data["level"]=="1"){
						//ここに処理を記述する
						if(is_object($this->log))	$this->log->logPrint("SNMP[軽故障]送信",REC_NOTE,__LINE__,__FILE__);
						SendSNMP(1,$this->log);
					}
					*/
				}
			}
			if($strSql!=""){
				if(is_object($this->log)) $this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
				if(!$dbs->Execute($strSql)){
					if(is_object($this->log)) $this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
					if(is_object($this->log)) $this->log->logPrint("[Exit False]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
					return false;
				}
			}
			//if(is_object($this->log)) $this->log->logPrint("[End]".__FUNCTION__,REC_NOTE,__LINE__,__FILE__);
			return true;
		}

		//障害履歴削除
		function DeleteFailure($pnt_id,$t900_sdt,$t900_no){
			$strSql = "DELETE FROM t900_dat WHERE t900_pnt_id = '{$pnt_id}' AND t900_sdt = '{$t900_sdt}' AND t900_no = {$t900_no};";
			$dbs = new CONN;
			if($dbs->Execute($strSql)){
				return true;
			}else{
				return false;
			}
		}

		//障害履歴更新
		function InsertFailure($pnt_id,$t900_old_sdt,$t900_sdt,$t900_no,$t900_value,$t900_level,$t900_ldt,$t900_old_no){
			if($t900_old_sdt!=""){
				$this->DeleteFailure($pnt_id,$t900_old_sdt,$t900_old_no);
				$this->DeleteFailure($pnt_id,$t900_old_sdt,$t900_no);
			}else{
				return false;
			}
			$strSql  = "INSERT INTO t900_dat (t900_pnt_id,t900_sdt,t900_no,t900_value,t900_level,t900_ldt)";
			$strSql .= "VALUES ('{$pnt_id}','{$t900_sdt}','{$t900_no}','{$t900_value}',{$t900_level},'{$t900_ldt}');";
			$dbs = new CONN;
			if($dbs->Execute($strSql)){
				return true;
			}else{
				return false;
			}
		}
	}
?>