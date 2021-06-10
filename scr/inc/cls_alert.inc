<?
//============================================================//
//	警報処理クラス																						//
//------------------------------------------------------------//
//	処理概要																									//
//	　気象庁警報＆閾値警報の設定、判断												//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/11/10	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class Alert
{
	var $varsion	= "1.0";
	var $update		= "2007.02.15";

	//変数宣言
	var $log;
	var $Field_000;

	//コンストラクタ [2007/02/03]
	function __construct(&$log = ""){
		if(is_object($log))	$this->log = $log;

		//比較対象フィールド情報を取得（現在フィールド固定）
		$cls = new Fields;
		$this->Field_000 = $cls->GetFields("000");
	}

	//フィールド情報取得 [2007/02/03]
	function GetAlertSetting($pnt_id){
		$Data = array();
		$strSql = "SELECT * FROM t_altdat WHERE alt_pnt_id = '{$pnt_id}' ORDER BY alt_no;";

		$dbs = new CONN;
		$row = $dbs->Query($strSql);

		$cnt = 0;
		while($row){
			$Data[] = $row;
			foreach($row as $key => $val){
				if(strpos($key,"_enable")){
					if($val) $Data[ $cnt ][$key."_value"] = "checked";
				}
				if(strpos(" ".$key,"alt_1_comp_")||strpos(" ".$key,"alt_2_comp_")){
					$Data[$cnt][$key] = $this->GetComp($row[$key]);
				}
				if(strpos(" ".$key,"alt_1_conn_")||strpos(" ".$key,"alt_2_conn_")){
					$Data[$cnt][$key] = $this->GetConn($row[$key]);
				}
				if(strpos(" ".$key,"alt_1_fld_id_")||strpos(" ".$key,"alt_2_fld_id_")){
					$Data[$cnt][$key] = $this->GetField($pnt_id,"000",$row[$key]);
				}
				if(strpos(" ".$key,"alt_fld_id")){
					$Data[$cnt][$key] = $this->GetField($pnt_id,"000",$row[$key]);
				}
			}
			$Data[ $cnt ]["no"] = $cnt + 1;

			$cnt++;
			$row = $dbs->Next();
		}
		return $Data;
	}

	//リストボックス生成用配列作成 [2007/02/03]
	function GetComp($select=""){
		$Data		 = array();
		$AryComp = array("未満","超","以下","以上","時");

		$cnt = 0;
		foreach($AryComp as $val){
			$Data[$cnt]["value"] = $val;
			if($select==$val)	$Data[$cnt]["selected"] = 1;
			$cnt++;
		}
		return $Data;
	}
	//リストボックス生成用配列作成 [2007/02/03]
	function GetConn($select=""){
		$Data    = array();
		$AryComp = array("かつ","または");

		$cnt = 0;
		foreach($AryComp as $val){
			$Data[$cnt]["value"] = $val;
			if($select==$val)	$Data[$cnt]["selected"] = 1;
			$cnt++;
		}
		return $Data;
	}
	//リストボックス生成用配列作成 [2007/02/03]
	function GetField($pnt_id,$sys_no,$select=""){
		$Data = array();
		$pnt = new Points;
		$AryField = $pnt->GetOutField($pnt_id,$sys_no);

		$cnt = 0;
		foreach($AryField as $key => $val){
			if(strpos(" ".$key,"weather"))	continue;
			if(strpos(" ".$key,"_wd"))			continue;

			$Data[$cnt]["fld_id"]			= $key;
			$Data[$cnt]["fld_name2"]	= $val;
			if($select==$key)	$Data[$cnt]["selected"] = 1;
			$cnt++;
		}
		return $Data;
	}

	//閾値情報保存 [2007/02/03]
	function SaveAlertInfo($args){
		$dbs		= new CONN;
		$fields = $dbs->GetMetaData("t_altdat");

		$pnt_id = $args["pnt_id"];

		if(!is_array($args["alt_no"])) return false;

		foreach($args["alt_no"] as $alt_no){
			$AryTmp = array();
			foreach($fields as $key => $val){
				if($key=="alt_no")			continue;
				if($key=="alt_pnt_id")	continue;
				if($key=="alt_enable")	continue;
				switch($val["type"]){
					case "varchar":
						if($args[$key][$alt_no]!=""){
							$AryTmp[] = "$key = '".$args[$key][$alt_no]."'";
						}else{
							$AryTmp[] = "$key = ''";
						}
						break;
					default:
						if($args[$key][$alt_no]!=""){
							$AryTmp[] = "$key = ".$args[$key][$alt_no];
						}else{
							$AryTmp[] = "$key = NULL";
						}
						break;
				}
			}
			if($args["alt_enable"][$alt_no]=="1"){
				$AryTmp[] = "alt_enable = 1";
			}else{
				$AryTmp[] = "alt_enable = 0";
				//閾値設定が動作不可の時現在発令中の警報情報を停止する
				$this->StopAlert($pnt_id,$alt_no);
			}
			$strSql = "UPDATE t_altdat SET ".join(",",$AryTmp)." WHERE alt_no = '{$alt_no}' AND alt_pnt_id = '{$pnt_id}';";

			if(!$dbs->Execute($strSql)){
				if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			}
		}
		return true;
	}

	//指定した警報設定を強制終了 [2007/02/03]
	function StopAlert($pnt_id,$alt_no=""){
		$dbs = new CONN;
		if($alt_no!=""){
			$strSql = "UPDATE t700_dat SET t700_edt = now() WHERE t700_pnt_id = '{$pnt_id}' AND t700_alt_no = {$alt_no} AND t700_edt IS NULL;";
		}else{
			$strSql = "UPDATE t700_dat SET t700_edt = now() WHERE t700_pnt_id = '{$pnt_id}' AND t700_edt IS NULL;";
		}
		if(!$dbs->Execute($strSql)){
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
		}
	}

	//フィールド情報追加  [2007/02/03]
	function AddAlert($pnt_id){
		$dbs = new CONN;
		$LastValue = $dbs->GetSeqLastValue("t_altdat_seq") + 1;
		$strSql = "INSERT INTO t_altdat (alt_no,alt_pnt_id,alt_name,alt_enable) VALUES (default,'{$pnt_id}','名称{$LastValue}',0);";
		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	//フィールド情報削除 [2007/02/03]
	function DeleteAlertInfo($alt_no){
		$dbs = new CONN;
		$strSql = "DELETE FROM t_altdat WHERE alt_no = {$alt_no};";
		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	//閾値判断用配列生成 [2007/02/15]
	function LoadAlertSetting($pnt_id,$all=false){
		$Data = array();
		$strSql = "SELECT * FROM t_altdat WHERE alt_pnt_id = '{$pnt_id}' AND alt_enable = 1 ORDER BY alt_no;";
		if($all){
			$strSql = "SELECT * FROM t_altdat WHERE alt_pnt_id = '{$pnt_id}' ORDER BY alt_no;";
		}

		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		$cnt = 0;
		while($row){
			for($kind=1;$kind<3;$kind++){
				$Data[$cnt]["alt_no"]			= $row["alt_no"];
				$Data[$cnt]["alt_pnt_id"]	= $row["alt_pnt_id"];
				$Data[$cnt]["alt_name"]		= $row["alt_name"];
				$Data[$cnt]["alt_kind"]		= $kind;
				for($i=1;$i<5;$i++){
					$Data[$cnt]["alt_fld_id_{$i}"]	= $row["alt_{$kind}_fld_id_{$i}"];
					$Data[$cnt]["alt_value_{$i}"]		= $row["alt_{$kind}_value_{$i}"];
					$Data[$cnt]["alt_comp_{$i}"]		= $row["alt_{$kind}_comp_{$i}"];
					if($i!=4){
						$Data[$cnt]["alt_conn_{$i}"]	= $row["alt_{$kind}_conn_{$i}"];
					}
				}
				$Data[$cnt]["alt_hold"]		= $row["alt_{$kind}_hold"];
				$Data[$cnt]["alt_fld_id"]	= $row["alt_fld_id"];
				$cnt++;
			}
			$row = $dbs->Next();
		}
		if($cnt==0){
			return false;
		}else{
			return $Data;
		}
	}

	//閾値警報発令チェック
	function StartAlertCheck($pnt_id,$curTime,$alt){
		$no		= $alt["alt_no"];
		$kind	= $alt["alt_kind"];
		$hold	= $alt["alt_hold"];
		$trg	= date("Y/m/d H:i",$curTime - $hold * 60);

		$dbs = new CONN;
		$strSql = "SELECT COUNT(*) FROM t700_dat WHERE t700_pnt_id = '{$pnt_id}' AND t700_alt_no = $no AND t700_alt_type = {$kind} AND (t700_edt IS NULL OR t700_edt > '$trg');";
		if(is_object($this->log))	$this->log->logPrint("====StartAlertCheck==== {$strSql}",REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		if($row[0]<1){
			if(is_object($this->log))	$this->log->logPrint("====StartAlertCheck==== 発令可能",REC_DEBUG,__LINE__,__FILE__);
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint("====StartAlertCheck==== 発令不可",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}

	//閾値警報発令
	function StartAlert($pnt_id,$curTime,$alt,$info1,$info2){
		if(!$this->StartAlertCheck($pnt_id,$curTime,$alt)) return false;

		$no			= $alt["alt_no"];
		$kind		= $alt["alt_kind"];
		$fld_id	= $alt["alt_fld_id"];
		$sdt		= date("Y/m/d H:i:s",$curTime);
		$strSql = "INSERT INTO t700_dat VALUES(DEFAULT,'{$pnt_id}',{$no},{$kind},'{$info1}','{$info2}','{$sdt}',NULL);";

		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	//アラート終了処理
	function EndAlert($pnt_id,$curTime,$alt){
		$no				= $alt["alt_no"];
		$kind			= $alt["alt_kind"];
		$strWhere = "t700_pnt_id = '{$pnt_id}' AND t700_alt_no = {$no} AND t700_alt_type = {$kind} AND t700_edt IS NULL";

		$dbs = new CONN;
		$row = $dbs->Query("SELECT COUNT(*) FROM t700_dat WHERE {$strWhere};");
		if($row[0]<1) return false;

		$trg		= date("Y/m/d H:i:s",$curTime);
		$strSql = "UPDATE t700_dat SET t700_edt = '{$trg}' WHERE {$strWhere};";

		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	///// 閾値判断 ///// [2007/02/03]
	function CheckAlert($pnt_id,$curTime,&$args){
		if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== ",								REC_DEBUG,__LINE__,__FILE__);
		if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== ///// 閾値判断 開始 /////",	REC_DEBUG,__LINE__,__FILE__);

		//閾値設定読込み
		if(!$Alert=$this->LoadAlertSetting($pnt_id)){
			if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== 閾値設定はありません",	REC_DEBUG,__LINE__,__FILE__);
			if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== 閾値判断 終了",				REC_DEBUG,__LINE__,__FILE__);
			return false;
		}

		if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== 現在データ => ".print_r($args,TRUE), REC_DEBUG,__LINE__,__FILE__);

		//閾値チェック
		if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== 警報チェック 開始",REC_DEBUG,__LINE__,__FILE__);
		foreach($Alert as $alt){
			if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== 警報チェック alt_no => ".$alt["alt_no"]." alt_kind => ".$alt["alt_kind"]." alt_name => ".$alt["alt_name"],REC_DEBUG,__LINE__,__FILE__);
			if($this->CheckValue($alt,$args,$info1,$info2)){
				if($this->StartAlert($pnt_id,$curTime,$alt,$info1,$info2)){
					if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== 警報発令",REC_DEBUG,__LINE__,__FILE__);
					if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== {$info1}",REC_DEBUG,__LINE__,__FILE__);
					if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== {$info2}",REC_DEBUG,__LINE__,__FILE__);
				}
			}else{
				if($this->EndAlert($pnt_id,$curTime,$alt)){
					if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== 警報解除",REC_DEBUG,__LINE__,__FILE__);
				}
			}
		}
		if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== ///// 警報チェック　終了 /////",REC_DEBUG,__LINE__,__FILE__);


//			//アラート開始チェック
//			foreach($Alert as $alt){
//				if($this->CheckValue($alt,$args,$info1,$info2)){
//					if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== args => ".print_r($args,TRUE),	REC_DEBUG,__LINE__,__FILE__);
//					if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== alert=> ".print_r($alt,TRUE),	REC_DEBUG,__LINE__,__FILE__);
//					if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== info1=> ".$info1,							REC_DEBUG,__LINE__,__FILE__);
//					if(is_object($this->log))	$this->log->logPrint("====CheckAlert==== info2=> ".$info2,							REC_DEBUG,__LINE__,__FILE__);
//					if(is_object($this->log))	$this->log->logPrint("CheckAlert => StartAlert",												REC_DEBUG,__LINE__,__FILE__);
//					$this->StartAlert($pnt_id,$curTime,$alt,$info1,$info2);
//				}
//			}
	}

	//閾値チェック
	function CheckValue($alt,$args,&$info1="",&$info2=""){
		$info1			= "";
		$info2			= "";
		$tmp_info1	= "";	//閾値情報

		if(is_object($this->log))	$this->log->logPrint("||||CheckValue|||| 値チェック　開始",	REC_DEBUG,__LINE__,__FILE__);

		for($i=1;$i<5;$i++){
			$flg = 0;

			$fld		= $alt["alt_fld_id_".$i];
			$val		= $alt["alt_value_".$i];
			$comp		= $alt["alt_comp_".$i];
			$value 	= $args[$fld];
			$conn		= $alt["alt_conn_".$i];

			if(is_object($this->log))	$this->log->logPrint("||||CheckValue|||| 設定{$i} フィールド名 => {$fld} 設定値 => {$val} {$comp} 実測値 => {$value} 連結子 => {$conn}",	REC_DEBUG,__LINE__,__FILE__);

			//比較項目がそろわないときは終了
			if($fld=="")						break;
			if(!is_numeric($val))		break;
			if($comp=="")						break;
			if(!is_numeric($value))	break;

			$tmp_info1  = $this->Field_000[$fld]["fld_name2"].":".number_format($val,	1, ".", "").$this->Field_000[$fld]["fld_unit"].$comp;
			$info2		 .= $this->Field_000[$fld]["fld_name2"].":".number_format($value,1, ".", "").$this->Field_000[$fld]["fld_unit"]." ";

			switch($comp){
				case "未満":	if($val > $value) 	$flg = 1;	break;
				case "超":		if($val < $value) 	$flg = 1;	break;
				case "以下":	if($val >= $value) 	$flg = 1;	break;
				case "以上":	if($val <= $value) 	$flg = 1;	break;
				case "時":		if($val == $value) 	$flg = 1;	break;
			}

			if($conn_prev!=""){
				$info1 .= " ".$conn_prev." ".$tmp_info1;
				switch($conn_prev){
					case "かつ":
						if($flg && $flg_prev){
							$flg = 1;
						}else{
							$flg = 0;
						}
						break;
					case "または":
						if($flg || $flg_prev){
							$flg = 1;
						}else{
							$flg = 0;
						}
						break;
				}
			}else{
				$info1 .= $tmp_info1;
			}

			$flg_prev		= 0;
			$conn_prev	= "";
			if($conn==""){
				break;
			}else{
				$flg_prev		= $flg;
				$conn_prev	= $conn;
				//$info1 .= $tmp_info1;
			}

		}

		if($info1=="") $info1 .= $tmp_info1;
		if(is_object($this->log))	$this->log->logPrint("||||CheckValue|||| 値チェック 終了  結果 => {$flg}",REC_DEBUG,__LINE__,__FILE__);
		return $flg;
	}

	//現在発令中の警報を取得(局別）
	function GetAlert($pnt_id,$curTime){
		$dbs = new CONN;
		$AryAlertData = array();

		//気象庁発表の注意報警報の最新6件を取得
		$strSql		= "SELECT * FROM t600_dat INNER JOIN t_warmst ON t600_war_no = war_no ";
		$strSql  .= "WHERE t600_pnt_id = '{$pnt_id}' AND  t600_edt IS NULL ORDER BY t600_sdt DESC,war_kind DESC,war_no DESC LIMIT 6;";

		//警報情報
		$AryAlertData["tab"]["warning_1"] = array("no" => "1","date" => "",			"time" => "",				"type" => "",		"value" => "");
		$AryAlertData["tab"]["warning_2"] = array("no" => "2","date" => "",			"time" => "",				"type" => "",		"value" => "");
		$AryAlertData["tab"]["warning_3"] = array("no" => "3","date" => "",			"time" => "",				"type" => "",		"value" => "");
		$AryAlertData["tab"]["warning_4"] = array("no" => "4","date" => "",			"time" => "",				"type" => "",		"value" => "");
		$AryAlertData["tab"]["warning_5"] = array("no" => "5","date" => "",			"time" => "",				"type" => "",		"value" => "");
		$AryAlertData["tab"]["warning_6"] = array("no" => "6","date" => "",			"time" => "",				"type" => "",		"value" => "");

		$cnt = 1;
		$row = $dbs->Query($strSql);
		$AryAlertData["warning_level"] = 0;
		while($row){
			$warTime = strtotime(str_replace("-","/",$row["t600_sdt"]));

			//気象庁警報の警報レベル更新
			if($AryAlertData["warning_level"] < $row["war_kind"])	$AryAlertData["warning_level"] = $row["war_kind"];

			$AryTmp = array();
			$AryTmp["no"]		= $cnt;
			$AryTmp["date"]	= date("m/d",$warTime);
			$AryTmp["time"]	= date("H:i",$warTime);
			$AryTmp["type"]	= $row["war_kind"];
			if($row["war_kind"]=="1"){
				$AryTmp["value"] =  $row["war_name"]."注意報";
			}elseif($row["war_kind"]=="2"){
				$AryTmp["value"] =  $row["war_name"]."警報";
			}elseif($row["war_kind"]=="3"){
				$AryTmp["value"] =  $row["war_name"]."特別警報";
			}
			//気象庁警報用タブ情報設定
			$AryAlertData["tab"]["warning_".$cnt] = $AryTmp;
			$row = $dbs->Next();
			$cnt++;
		}

		//警報フィードバック用配列
		$AryRef = array();

		//閾値警報情報取得
		$strSql		= "SELECT * FROM t700_dat INNER JOIN t_altdat ON t700_alt_no = alt_no ";
		$strSql  .= "WHERE t700_pnt_id = '{$pnt_id}' AND t700_edt IS NULL ORDER BY t700_sdt,t700_alt_no,t700_alt_type;";
		$AryAlertData["alert_date"]		= "0";
		$AryAlertData["alert_level"]	= "0";
		$row = $dbs->Query($strSql);
		while($row){
			$altTime = strtotime(str_replace("-","/",$row["t700_sdt"]));
			//閾値警報日付設定
			if($AryAlertData["alert_date"]	< $altTime)								$AryAlertData["alert_date"]		= $altTime;
			//閾値警報レベル設定
			if($AryAlertData["alert_level"] < $row["t700_alt_type"])	$AryAlertData["alert_level"]	= $row["t700_alt_type"];

			$fld_id_ref = trim($row["alt_fld_id"]);
			if($fld_id_ref!=""){
				$AryRef[$fld_id_ref] = $row["t700_alt_type"];
			}
			$row = $dbs->Next();
		}

		//トータルレベル（一応出力・・・・）
		$AryAlertData["total_level"] = $AryAlertData["alert_level"];
		//最新アラート日付
		if($AryAlertData["alert_date"]>0){
			$date = DateConvert($AryAlertData["alert_date"]);
			$AryAlertData["alert_date"] = $date["date"];
		}

		//警報フィードバック用データ追加
		foreach($AryRef as $fld => $type){
			$AryAlertData["reflection"][$fld] = $type;
		}
		return $AryAlertData;
	}

	//現在発令中の気象庁警報取得
	function GetWarning($pnt_id){
		#$strSql	= "SELECT * FROM t600_dat INNER JOIN t_warmst ON t600_war_no = war_no WHERE t600_pnt_id = '{$pnt_id}' AND t600_edt IS NULL ORDER BY t600_sdt;";
		//2012/07/10横浜工場の森考敬さん起票のＵＭＩＴより仕様変更
		$strSql	= "SELECT * FROM t600_dat INNER JOIN t_warmst ON t600_war_no = war_no WHERE t600_pnt_id = '{$pnt_id}' AND t600_edt IS NULL ORDER BY t600_sdt DESC,war_kind DESC,war_no DESC;";

		$AryData = array();
		$dbs = new CONN;
		if(!$row=$dbs->Query($strSql))	return false;
		$cnt=0;
		while($row){
			$warTime = strtotime(str_replace("-","/",$row["t600_sdt"]));
			$AryData[$cnt]["no"]		= $cnt;
			$AryData[$cnt]["date"]	= date("Y/m/d H:i",$warTime);

			$AryData[$cnt]["type"]	= $row["war_kind"];
			$AryData[$cnt]["war_no"]	= $row["war_no"];
			if($row["war_kind"]=="1"){
				$AryData[$cnt]["value"]	=  $row["war_name"]."注意報";
			}elseif($row["war_kind"]=="2"){
				$AryData[$cnt]["value"] =  $row["war_name"]."警報";
			}elseif($row["war_kind"]=="3"){
				$AryData[$cnt]["value"] =  $row["war_name"]."特別警報";
			}elseif($row["war_kind"]=="4"){
				$AryData[$cnt]["value"] =  $row["war_name"]."気象情報";
			}
			$row = $dbs->Next();
			$cnt++;
		}
		return $AryData;
	}
	//警報設定処理
	function SetWarning($pnt_id,$curTime,$war_no){
		$tmp = $this->GetWarning($pnt_id);
		if(is_array($tmp)){
			foreach($tmp as $key => $val){
				if($val["war_no"]==$war_no) return false;
			}
		}
		$date		= date("Y-m-d H:i",$curTime);
		$strSql = "INSERT INTO t600_dat VALUES('{$pnt_id}','{$date}',NULL,{$war_no});";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//警報設定処理
	function ClearWarning($pnt_id,$curTime,$war){
		$date		= date("Y-m-d H:i",$curTime);
		$war_value = join(",",$war);
		$strSql = "UPDATE t600_dat SET t600_edt = '{$date}' WHERE t600_pnt_id='{$pnt_id}' AND t600_war_no IN ($war_value) AND t600_edt IS NULL;";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}
	//警報設定処理
	function DeleteWarning($pnt_id,$date,$war){
		$strSql = "DELETE FROM t600_dat WHERE t600_pnt_id='{$pnt_id}' AND t600_sdt = '{$date}' AND t600_war_no = {$war};";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}
	//警報設定処理
	function InsertWarning($pnt_id,$date,$date2,$war){
		if(trim($date2)==""){
			$date2 = "NULL";
		}else{
			$date2 = "'{$date2}'";
		}
		$strSql = "INSERT INTO t600_dat VALUES('{$pnt_id}','{$date}',{$date2},{$war});";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//閾値
	//警報設定処理
	function DeleteAlert($t700_no){
		$strSql = "DELETE FROM t700_dat WHERE t700_no = {$t700_no};";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//警報設定処理
	function InsertAlert($t700_no,$t700_pnt_id,$t700_alt_no,$t700_alt_type,$t700_info1,$t700_info2,$t700_sdt,$t700_edt){
		if($t700_no!=""){
			$this->DeleteAlert($t700_no);
		}else{
			$t700_no = "DEFAULT";
		}
		if(trim($t700_edt)==""){
			$t700_edt = "NULL";
		}else{
			$t700_edt = "'{$t700_edt}'";
		}
		$strSql  = "INSERT INTO t700_dat (t700_no,t700_pnt_id,t700_alt_no,t700_alt_type,t700_info1,t700_info2,t700_sdt,t700_edt)";
		$strSql .= "VALUES ({$t700_no},'{$t700_pnt_id}',{$t700_alt_no},{$t700_alt_type},'{$t700_info1}','{$t700_info2}','{$t700_sdt}',{$t700_edt});";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//雨量（なぜかここに・・)
	//警報設定処理
	function DeleteRain($t500_no){
		$strSql = "DELETE FROM t500_dat WHERE t500_no = {$t500_no};";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//警報設定処理
	function InsertRain($t500_no,$t500_pnt_id,$t500_sdt,$t500_edt,$t500_last,$t500_value){
		if($t500_no!=""){
			$this->DeleteRain($t500_no);
		}else{
			$t500_no = "DEFAULT";
		}
		if(trim($t500_edt)==""){
			$t500_edt = "NULL";
		}else{
			$t500_edt = "'{$t500_edt}'";
		}
		$strSql  = "INSERT INTO t500_dat (t500_no,t500_pnt_id,t500_sdt,t500_edt,t500_last,t500_value)";
		$strSql .= "VALUES ({$t500_no},'{$t500_pnt_id}','{$t500_sdt}',{$t500_edt},'{$t500_last}','{$t500_value}');";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//地震履歴削除
	function DeleteQuake($pnt_id,$t800_sdt){
		$strSql = "DELETE FROM t800_dat WHERE t800_pnt_id = '{$pnt_id}' AND t800_sdt = '{$t800_sdt}';";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//地震履歴更新
	function InsertQuake($pnt_id,$t800_old_sdt,$t800_sdt,$t800_quake2,$t800_si,$t800_gal){
		if($t800_old_sdt!=""){
			$this->DeleteQuake($pnt_id,$t800_old_sdt);
		}else{
			return false;
		}
		$t800_quake = $this->sindo2($t800_quake2);

		$strSql  = "INSERT INTO t800_dat (t800_pnt_id,t800_sdt,t800_quake,t800_quake2,t800_si,t800_gal,t800_ldt)";
		$strSql .= "VALUES ('{$pnt_id}','{$t800_sdt}','{$t800_quake}',{$t800_quake2},{$t800_si},{$t800_gal},NOW());";
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//日本語震度表示変換
	function sindo($quake){
		$quake /= 100;
		return $this->sindo2($t800_quake2);
	}
	//日本語震度表示変換
	function sindo2($quake){
		switch($quake){
			case 0.5 >$quake								:	$quake = "0";		break;
			case 0.5<=$quake && $quake < 1.5:	$quake = "1";		break;
			case 1.5<=$quake && $quake < 2.5:	$quake = "2";		break;
			case 2.5<=$quake && $quake < 3.5:	$quake = "3";		break;
			case 3.5<=$quake && $quake < 4.0:	$quake = "4弱";	break;
			case 4.0<=$quake && $quake < 4.5:	$quake = "4強";	break;
			case 4.5<=$quake && $quake < 5.0:	$quake = "5弱";	break;
			case 5.0<=$quake && $quake < 5.5:	$quake = "5強";	break;
			case 5.5<=$quake && $quake < 6.0:	$quake = "6弱";	break;
			case 6.0<=$quake && $quake < 6.5:	$quake = "6強";	break;
			case 6.5<=$quake								: $quake = "7";		break;
		}
		return trim($quake);
	}

}
?>