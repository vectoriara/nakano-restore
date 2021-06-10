<?
//============================================================//
//	データ処理クラス																					//
//------------------------------------------------------------//
//	処理概要																									//
//	　各データのロード＆セーブなど														//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/11/10	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class Data
{
	var $varsion	= "1.0";
	var $update		= "2007/01/20";

	//変数宣言
	var $log;
	var $ArySum		= array(
		"200" => array("40","50","55","60","65","70","90"),
		"300" => array("40","50","55","60","65","70","80","90"),
		"400" => array("40","50","55","60","65","70","80","90")
	);
	var $ArySumName = array("40" => "平均値","50" => "最高値","55" => "最高値起時","60" => "最低値","65" => "最低値起時","70" => "積算値","80" => "日数","90" => "風向頻度");
	//集計データ取得時一時保存用配列
	var $ArySumData;

	//コンストラクタ
	function __construct(&$log = ""){
		if(is_object($log))	$this->log = $log;
	}

	//データ保存用配列生成	[2006/12/29]
	function GetDataField($sys_no){
		$Field	= array();
		$strSql = "SELECT fld_id FROM t_fldmst WHERE fld_enable = 1 AND fld_{$sys_no}_enable = 1 ORDER BY fld_sort;";

		$dbs = new CONN;
		if(!$row=$dbs->Query($strSql)){
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		while($row){
			$Field[ $row["fld_id"] ] = "";
			$row = $dbs->Next();
		}
		return $Field;
	}

	//集計データ保存用配列生成
	function GetSumDataField($sys_no){
		$Field	= array();
		$strSql = "SELECT * FROM t_fldmst WHERE fld_enable = 1 AND fld_{$sys_no}_enable = 1 ORDER BY fld_sort;";
		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		while($row){
			foreach($this->ArySum[$sys_no] as $sum_no){
				if($sum_no=="90"){
					//風向頻度固定
					$Field[ $sum_no ]["avg_ws"] = "";
				}else{
					$Field[ $sum_no ][ $row["fld_id"] ] = "";
				}
			}
			$row = $dbs->Next();
		}
		ksort($Field);
		return $Field;
	}

	//複合グラフ用項目一覧取得	[2007/01/20]
	function GetGlaphField($sys_no,$fld_id="",$key_type="1"){
		//key_type => 1	フィールド配列のキーを取得順にする場合
		//key_type => 2	フィールド配列のキーをフィールドIDにする場合
		$Field	= array();

		$dbs = new CONN;
		$row = $dbs->Query("SELECT * FROM t_fldmst WHERE  fld_enable = 1 AND fld_{$sys_no}_graph = 1 ORDER BY fld_sort;");
		if($key_type=="1") $cnt=0;
		while($row){
			if($key_type!="1")	$cnt = $row["fld_id"];
			$Field[ $cnt ]["fld_no"]		= $row["fld_no"];
			$Field[ $cnt ]["fld_id"]		= $row["fld_id"];
			$Field[ $cnt ]["fld_name2"] = $row["fld_name2"];
			$Field[ $cnt ]["fld_name3"] = $row["fld_name3"];
			$Field[ $cnt ]["fld_scale"] = $row["fld_scale"];

			if(trim($row["fld_scale"])!=""){
				//最大値、最小値取得
				$AryTmp = explode("，",$row["fld_scale"]);
				$Field[ $cnt ]["max"] = $AryTmp[6];
				$Field[ $cnt ]["min"] = $AryTmp[1];
				//グラフ基底値算出
				$Field[ $cnt ]["scale_value"]  = ($AryTmp[2] - $AryTmp[1]) * 5;
			}
			if($fld_id==$row["fld_id"]){
				$Field[ $cnt ]["selected"] = 1;
			}
			$row = $dbs->Next();
			if($key_type=="1") $cnt++;
		}
		return $Field;
	}

	function ExistData($pnt_id,$sys_no,$curTime,$code=""){
		$date	= DateConvert($curTime);
		$tbl	= "t{$sys_no}_dat_".$date["year"];
		switch($sys_no){
			case "000":
				$trg = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];
				break;
			case "100":
				$trg = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];
				if($code!=""){
					$trg = $date["year"].$date["month"].$date["day"]."%";
				}
				break;
			case "200":
				$trg = $date["year"].$date["month"].$date["day"].$date["hour"]."%";
				if($code!=""){
					$trg = $date["year"].$date["month"].$date["day"]."%";
				}
				break;
			case "300":
				$trg = $date["year"].$date["month"].$date["day"]."%";
				if($code!=""){
					$trg = $date["year"].$date["month"]."%";
				}
				break;
			case "400":
				$trg = $date["year"].$date["month"]."%";
				if($code!=""){
					$trg = $date["year"]."%";
				}
				break;
		}
		if($code==""){
			$strWhere  	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date LIKE '{$trg}' AND t{$sys_no}_code IS NULL";
		}else{
			$strWhere  	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date LIKE '{$trg}' AND t{$sys_no}_code = {$code}";
		}

		if(strpos(" ".$strWhere,"%")==0){
			$strWhere = str_replace("LIKE","=",$strWhere);
		}

		$dbs		= new CONN;
		$strSql = "SELECT COUNT(*) FROM {$tbl} WHERE {$strWhere};";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
		if($row=$dbs->Query($strSql)){
			return $row[0];
		}else{
			return false;
		}
	}

	//データ削除 [2007/01/27]
	function DeleteData($pnt_id,$sys_no,$curTime,$curTime2="",$code=""){
		//データ削除日付１
		$date	= DateConvert($curTime);
		$tbl	= "t{$sys_no}_dat_".$date["year"];
		switch($sys_no){
			case "000":
				$trg = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];
				break;
			case "100":
				if($code==""){
					$trg = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];
				}else{
					$trg = $date["year"].$date["month"].$date["day"]."%";
				}
				break;
			case "200":
				if($code==""){
					$trg = $date["year"].$date["month"].$date["day"].$date["hour"]."00";
				}else{
					if($curTime2==""){//怖いので日報のみ対応
						$trg = $date["year"].$date["month"].$date["day"]."%";
					}else{
						$trg = $date["year"].$date["month"].$date["day"]."0000";
					}
				}
				break;
			case "300":
				if($code==""){
					$trg = $date["year"].$date["month"].$date["day"]."0000";
				}else{
					if($curTime2==""){
						$trg = $date["year"].$date["month"]."%";
					}else{
						$trg = $date["year"].$date["month"]."000000";
					}
				}
				break;
			case "400":
				if($code==""){
					$trg = $date["year"].$date["month"]."000000";
				}else{
					if($curTime2==""){
						$trg = $date["year"]."%";
					}else{
						$trg = $date["year"]."00000000";
					}
				}
				break;
		}

		if($curTime2!=""){
			//データ削除日付２
			$date	= DateConvert($curTime2);
			switch($sys_no){
				case "000":
					$trg2 = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];
					break;
				case "100":
					if($code==""){
						$trg2 = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];
					}else{
						$trg2 = $date["year"].$date["month"].$date["day"]."%";
					}
					break;
				case "200":
					if($code==""){
						$trg2 = $date["year"].$date["month"].$date["day"].$date["hour"]."00";
					}else{
						$trg2 = $date["year"].$date["month"].$date["day"]."9999";//怖いので日報のみ対応
					}
					break;
				case "300":
					if($code==""){
						$trg2 = $date["year"].$date["month"].$date["day"]."0000";
					}else{
						$trg2 = $date["year"].$date["month"]."999999";
					}
					break;
				case "400":
					if($code==""){
						$trg2 = $date["year"].$date["month"]."000000";
					}else{
						$trg2 = $date["year"]."99999999";
					}
					break;
			}
			if($code==""){
				$strSql = "DELETE FROM {$tbl} WHERE t{$sys_no}_pnt_id ='{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg}' AND '{$trg2}' AND t{$sys_no}_code IS NULL;";
			}else{
				$strSql = "DELETE FROM {$tbl} WHERE t{$sys_no}_pnt_id ='{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg}' AND '{$trg2}' AND t{$sys_no}_code = {$code};";
			}
		}else{
			if($code==""){
				$strSql = "DELETE FROM {$tbl} WHERE t{$sys_no}_pnt_id ='{$pnt_id}' AND t{$sys_no}_date = '{$trg}' AND t{$sys_no}_code IS NULL;";
			}else{
				$strSql = "DELETE FROM {$tbl} WHERE t{$sys_no}_pnt_id ='{$pnt_id}' AND t{$sys_no}_date LIKE '{$trg}' AND t{$sys_no}_code = {$code};";
			}
		}
		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
			$result = 1;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			$result = 0;
		}
		//年界時対応
		if($curTime2!=""&&$result){
			$year = date("Y",$curTime2);
			if(date("Y",$curTime)!=$year){
				if($code==""){
					$strSql = "DELETE FROM t{$sys_no}_dat_{$year} WHERE t{$sys_no}_pnt_id ='{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$year}01010000' AND '{$trg2}' AND t{$sys_no}_code IS NULL;";
				}else{
					$strSql = "DELETE FROM t{$sys_no}_dat_{$year} WHERE t{$sys_no}_pnt_id ='{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$year}01010000' AND '{$trg2}' AND t{$sys_no}_code = {$code};";
				}
				$dbs = new CONN;
				if($dbs->Execute($strSql)){
					if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
					return true;
				}else{
					if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
					return false;
				}
			}
		}
		return $result;
	}

	//データ保存 [2007/01/27]
	function SaveData($pnt_id,$sys_no,$curTime,$rec,$code="",$overwrite=""){
		//データ存在チェック
		if($this->ExistData($pnt_id,$sys_no,$curTime,$code)){
			//データ上書きチェック
			if($overwrite){
				if(is_object($this->log))	$this->log->logPrint("DeleteData",REC_DEBUG,__LINE__,__FILE__);
				$this->DeleteData($pnt_id,$sys_no,$curTime,"",$code);
			}else{
				if(is_object($this->log))	$this->log->logPrint("ExistData",REC_DEBUG,__LINE__,__FILE__);
				return true;
			}
		}
		//配列にデータがない場合は保存を行わない
		if($code=="90"){
			$tmp_rec = trim(join("",$rec));
			$tmp_rec = explode(",",$tmp_rec);
		}else{
			$tmp_rec = $rec;
		}
		if(trim(join("",$tmp_rec))==""){
			if(is_object($this->log))	$this->log->logPrint("SaveData Exit",REC_DEBUG,__LINE__,__FILE__);
			return true;
		}

		//データ基本情報設定
		$date = DateConvert($curTime);
		//テーブル設定
		$tbl	= "t{$sys_no}_dat_".$date["year"];
		//局番号設定
		$rec["t{$sys_no}_pnt_id"] = $pnt_id;
		//日付設定
		switch($sys_no){
			case "000":
			case "100":
			case "200":
				if($code==""){
					$rec["t{$sys_no}_date"]	= $date["date"];
				}else{
					$rec["t{$sys_no}_code"]	= $code;
					$rec["t{$sys_no}_date"]	= $date["year"].$date["month"].$date["day"].$code."00";
				}
				break;
			case "300":
				if($code==""){
					$rec["t{$sys_no}_date"]	= $date["year"].$date["month"].$date["day"]."0000";
				}else{
					$rec["t{$sys_no}_code"]	= $code;
					$rec["t{$sys_no}_date"]	= $date["year"].$date["month"]."0000".$code;
				}
				break;
			case "400":
				if($code==""){
					$rec["t{$sys_no}_date"]	= $date["year"].$date["month"]."000000";
				}else{
					$rec["t{$sys_no}_code"]	= $code;
					$rec["t{$sys_no}_date"]	= $date["year"]."000000".$code;
				}
				break;
		}

		//SQL文設定
		$fld		= array();
		$value	= array();
		foreach($rec as $key => $val){
			$fld[]		= $key;
			$value[]	=	"'{$val}'";
		}
		$strSql = "INSERT INTO {$tbl} (".join(",",$fld).") VALUES (".join(",",$value).");";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

		$dbs = new CONN;

		//保存する際テーブルがない場合は強制作成
		if(!$dbs->GetMetaData($tbl)){
			//新規テーブル作成
			$dbs->Execute("CREATE TABLE {$tbl} AS SELECT * FROM t{$sys_no}_dat;");
			$dbs->Execute("ALTER TABLE {$tbl} ADD CONSTRAINT {$tbl}_pk PRIMARY KEY (t{$sys_no}_pnt_id,t{$sys_no}_date);");

			if(is_object($this->log)) $this->log->logPrint("CREATE TABLE {$tbl} AS SELECT * FROM t{$sys_no}_dat;",																				REC_DEBUG,__LINE__,__FILE__);
			if(is_object($this->log)) $this->log->logPrint("ALTER TABLE {$tbl} ADD CONSTRAINT {$tbl}_pk PRIMARY KEY (t{$sys_no}_pnt_id,t{$sys_no}_date);",REC_DEBUG,__LINE__,__FILE__);
			//新規テーブル作成チェック
			if($dbs->GetMetaData($tbl)){
				if(is_object($this->log)) $this->log->logPrint("{$tbl}を作成しました",REC_NOTE,__LINE__,__FILE__);
			}else{
				if(is_object($this->log)) $this->log->logPrint("{$tbl}を作成できません",REC_NOTE,__LINE__,__FILE__);
			}
		}
		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}

	//データ更新
	function UpdateData($pnt_id,$sys_no,$curTime,$rec,$code=""){
		if(!$this->ExistData($pnt_id,$sys_no,$curTime)){
			$this->SaveZeroData($pnt_id,$sys_no,$curTime);
		}
		//データ基本情報設定
		$date = DateConvert($curTime);
		//テーブル設定
		$tbl	= "t{$sys_no}_dat_".$date["year"];
		//局番号設定
		$strWhere = " WHERE t{$sys_no}_pnt_id = '$pnt_id' ";
		//日付設定
		switch($sys_no){
			case "000":
			case "100":
			case "200":
				if($code!=""){
					$strWhere .= "AND t{$sys_no}_code = ".$code;
					$strWhere .= "AND t{$sys_no}_date = '".$date["year"].$date["month"].$date["day"].$code."00'";
				}else{
					$strWhere .= "AND t{$sys_no}_date = '".$date["date"]."'";
				}
				break;
			case "300":
				if($code!=""){
					$strWhere .= "AND t{$sys_no}_code = ".$code;
					$strWhere .= "AND t{$sys_no}_date = '".$date["year"].$date["month"]."0000".$code."'";
				}else{
					$strWhere .= "AND t{$sys_no}_date = '".$date["year"].$date["month"].$date["day"]."0000'";
				}
				break;
			case "400":
				if($code!=""){
					$strWhere .= "AND t{$sys_no}_code = ".$code;
					$strWhere .= "AND t{$sys_no}_date = '".$date["year"]."000000".$code."'";
				}else{
					$strWhere .= "AND t{$sys_no}_date = '".$date["year"].$date["month"]."000000'";
				}
				break;
		}

		//SQL文設定
		$value	= array();
		foreach($rec as $key => $val)	$value[] = "{$key} = '{$val}'";
		$strSql = "UPDATE {$tbl} SET ".join(",",$value).$strWhere.";";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}


	//空白データ作成用
	function SaveZeroData($pnt_id,$sys_no,$curTime,$code=""){
		$rec = array();
		//データ基本情報設定
		$date = DateConvert($curTime);
		//テーブル設定
		$tbl	= "t{$sys_no}_dat_".$date["year"];
		//局番号設定
		$rec["t{$sys_no}_pnt_id"] = $pnt_id;
		//日付設定
		switch($sys_no){
			case "000":
			case "100":
			case "200":
				$rec["t{$sys_no}_date"]			= $date["date"];
				if($code!=""){
					$rec["t{$sys_no}_code"]		= $code;
					$rec["t{$sys_no}_date"]		= $date["year"].$date["month"].$date["day"]."00".$code;
				}
				break;
			case "300":
				$rec["t{$sys_no}_date"]			= $date["year"].$date["month"].$date["day"]."0000";
				if($code!=""){
					$rec["t{$sys_no}_code"]		= $code;
					$rec["t{$sys_no}_date"]		= $date["year"].$date["month"]."0000".$code;
				}
				break;
			case "400":
				$rec["t{$sys_no}_date"]			= $date["year"].$date["month"]."000000";
				if($code!=""){
					$rec["t{$sys_no}_code"]		= $code;
					$rec["t{$sys_no}_date"]		= $date["year"]."000000".$code;
				}
				break;
		}
		//SQL文設定
		$fld		= array();
		$value	= array();
		foreach($rec as $key => $val){
			$fld[]		= $key;
			$value[]	=	"'{$val}'";
		}
		$strSql = "INSERT INTO {$tbl} (".join(",",$fld).") VALUES (".join(",",$value).");";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

		$dbs = new CONN;

		//保存する際テーブルがない場合は強制作成
		if(!$dbs->GetMetaData($tbl)){
			//新規テーブル作成
			$dbs->Execute("CREATE TABLE {$tbl} AS SELECT * FROM t{$sys_no}_dat;");
			$dbs->Execute("ALTER TABLE {$tbl} ADD CONSTRAINT {$tbl}_pk PRIMARY KEY (t{$sys_no}_pnt_id,t{$sys_no}_date);");

			if(is_object($this->log)) $this->log->logPrint("CREATE TABLE {$tbl} AS SELECT * FROM t{$sys_no}_dat;",																				REC_DEBUG,__LINE__,__FILE__);
			if(is_object($this->log)) $this->log->logPrint("ALTER TABLE {$tbl} ADD CONSTRAINT {$tbl}_pk PRIMARY KEY (t{$sys_no}_pnt_id,t{$sys_no}_date);",REC_DEBUG,__LINE__,__FILE__);
			//新規テーブル作成チェック
			if($dbs->GetMetaData($tbl)){
				if(is_object($this->log)) $this->log->logPrint("{$tbl}を作成しました",REC_NOTE,__LINE__,__FILE__);
			}else{
				if(is_object($this->log)) $this->log->logPrint("{$tbl}を作成できません",REC_NOTE,__LINE__,__FILE__);
			}
		}

		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
	}

	//集計データ保存処理
	function SaveSumData($pnt_id,$sys_no,$curTime,$rec,$overwrite=1){
		$date = DateConvert($curTime);
		$tbl	= "t{$sys_no}_dat_".$date["year"];
		switch($sys_no){
			case "200":
				$trg = $date["year"].$date["month"].$date["day"];
				break;
			case "300":
				$trg = $date["year"].$date["month"];
				break;
			case "400":
				$trg = $date["year"];
				break;
		}
		foreach($rec as $code => $val){
			$this->SaveData($pnt_id,$sys_no,$curTime,$val,$code,$overwrite);
		}
		return true;
	}

	//データ読込み
	function LoadData($pnt_id,$sys_no,$curTime,$fld="",$limit=""){
		//取得フィールド設定
		$AryFld = array();
		if(is_array($fld)){
			//フィールド指定取得
			foreach($fld as $val){
				$AryFld[$val] = $val;
			}
		}else{
			switch($sys_no){
				case "500":
					//累計雨量
					$dbs		= new CONN;
					$Fields = $dbs->GetMetaData("t{$sys_no}_dat");
					$AryFld = array();
					foreach($Fields as $key => $val){
						$AryFld[$key] = $key;
					}
					break;
				case "600":
					//警報履歴（気象庁発表)
					$dbs		= new CONN;
					$Fields = $dbs->GetMetaData("t_warmst");
					$AryFld = array();
					foreach($Fields as $key => $val){
						$AryFld[$key] = $key;
					}
					$Fields = $dbs->GetMetaData("t{$sys_no}_dat");
					foreach($Fields as $key => $val){
						$AryFld[$key] = $key;
					}
					break;
				case "700":
					//閾値履歴
					$dbs		= new CONN;
					$Fields = $dbs->GetMetaData("t{$sys_no}_dat");
					$AryFld = array();
					foreach($Fields as $key => $val){
						$AryFld[$key] = $key;
					}
					$AryFld["alt_name"] = "alt_name";
					break;
				case "800":
					//地震履歴
					$dbs		= new CONN;
					$Fields = $dbs->GetMetaData("t{$sys_no}_dat");
					$AryFld = array();
					foreach($Fields as $key => $val){
						$AryFld[$key] = $key;
					}
					break;
				default:
					//全フィールド取得
					$AryFld = $this->GetDataField($sys_no);
			}
		}

		//データ日付設定
		$date = DateConvert($curTime);
		$tbl = "t{$sys_no}_dat_".$date["year"];
		switch($sys_no){
			case "000":
				$trg = $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];
				$sub_start	= 0;
				$sub_end 		= 12;
				break;
			case "100":
				$trg = $date["year"].$date["month"].$date["day"];
				$sub_start	= 8;
				$sub_end 		= 4;
				break;
			case "200":
				$trg = $date["year"].$date["month"].$date["day"];
				$sub_start	= 8;
				$sub_end 		= 4;
				break;
			case "300":
				$trg = $date["year"].$date["month"];
				$sub_start	= 6;
				$sub_end 		= 2;
				break;
			case "400":
				$trg = $date["year"];
				$sub_start	= 4;
				$sub_end 		= 2;
				break;
			case "500":
				//累計雨量
				$year		= $date["year"];
				$month	= $date["month"];
				$tbl		= "t{$sys_no}_dat";
				break;
			case "600":
				//閾値履歴
				$year		= $date["year"];
				$month	= $date["month"];
				$tbl		= "t{$sys_no}_dat";
				$tbl2		= "t_warmst";
				break;
			case "700":
				//閾値履歴
				$year		= $date["year"];
				$month	= $date["month"];
				$tbl		= "t{$sys_no}_dat";
				$tbl2		= "t_altdat";
				break;
			case "800":
				//地震帳票
				$year		= $date["year"];
				$month	= $date["month"];
				$tbl 		= "t{$sys_no}_dat";
				break;
			default:
				return false;
		}

		//データ取得開始
		$AryData	= array();
		switch($sys_no){
			case "500":
				//累計雨量
				$strSql = "SELECT * FROM {$tbl} WHERE t{$sys_no}_pnt_id = '{$pnt_id}' AND (( EXTRACT(YEAR FROM t500_sdt)={$year} AND EXTRACT(MONTH FROM t500_sdt)={$month} ) OR (EXTRACT(YEAR FROM t500_last)={$year} AND EXTRACT(MONTH FROM t500_last)={$month} AND t500_edt IS NULL)) ORDER BY t500_sdt;";
				break;
			case "600":
				//警報履歴
				$strSql = "SELECT * FROM {$tbl} INNER JOIN {$tbl2} ON t600_war_no = war_no WHERE t{$sys_no}_pnt_id = '{$pnt_id}' AND (( EXTRACT(YEAR FROM t600_sdt)={$year} AND EXTRACT(MONTH FROM t600_sdt)={$month} ) OR (EXTRACT(YEAR FROM t600_sdt)={$year} AND EXTRACT(MONTH FROM t600_sdt)={$month} AND t600_edt IS NULL)) ORDER BY t600_sdt;";
				break;
			case "700":
				//閾値履歴
				$strSql = "SELECT * FROM {$tbl} INNER JOIN {$tbl2} ON t700_alt_no = alt_no WHERE t{$sys_no}_pnt_id = '{$pnt_id}' AND (( EXTRACT(YEAR FROM t700_sdt)={$year} AND EXTRACT(MONTH FROM t700_sdt)={$month} ) OR (EXTRACT(YEAR FROM t700_sdt)={$year} AND EXTRACT(MONTH FROM t700_sdt)={$month} AND t700_edt IS NULL)) ORDER BY t700_sdt;";
				break;
			case "800":
				//地震帳票
				$strSql = "SELECT * FROM {$tbl} WHERE t{$sys_no}_pnt_id = '{$pnt_id}' AND EXTRACT(YEAR FROM t800_sdt)={$year} AND EXTRACT(MONTH FROM t800_sdt)={$month} AND t{$sys_no}_ldt IS NOT NULL ORDER BY t{$sys_no}_sdt;";
				break;
			default:
				$strWhere = "FROM {$tbl} WHERE t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date LIKE '{$trg}%' AND t{$sys_no}_code IS NULL;";
				if(is_array($fld)){
					$strSql	= "SELECT ".join(",",$fld).",t{$sys_no}_date ".$strWhere;
				}else{
					$strSql	= "SELECT * ".$strWhere;
				}
		}
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		while($row){
			$AryTmp = array();
			foreach($AryFld as $key => $val){
				if(is_array($limit)){
					//値を丸める場合
					if(is_numeric($row[$key])){
						$AryTmp[$key] = $row[$key];
						if($limit[$key]["max"] < $row[$key])	$AryTmp[$key] = $limit[$key]["max"];
						if($limit[$key]["min"] > $row[$key])	$AryTmp[$key] = $limit[$key]["min"];
					}else{
						$AryTmp[$key] = $row[$key];
					}
				}else{
					$AryTmp[$key] = $row[$key];
				}
			}
			if($sys_no=="800"||$sys_no=="500"||$sys_no=="600"||$sys_no=="700"){
				//時間データから秒を取り除く
				foreach($AryFld as $key => $val){
					$AryTmp[$key] = $row[$key];
					//累計雨量特殊対応
					if($key=="t500_edt"){
						if($AryTmp[$key]==""){
							$AryTmp["t500_last"]="";
							$row["t500_last"]="";
						}
					}
					if($key=="t500_value"){
						$AryTmp[$key] = number_format($row[$key],1, ".", "");
					}
					if($Fields[$key]["type"]=="timestamp"){
						if($t = @strtotime($AryTmp[$key])){
							$AryTmp[$key] = date("Y/m/d H:i",$t);
						}
					}
				}
				$AryData[]= $AryTmp;
			}else{
				$AryData[ substr($row["t{$sys_no}_date"],$sub_start,$sub_end) ]	= $AryTmp;
			}
			$row = $dbs->Next();
		}
		return $AryData;
	}

	//データを時間配列に格納して読込み
	function LoadAryData($pnt_id,$sys_no,$curTime,$fld="",$limit=""){
		global $dir,$weather;

		//取得フィールド設定
		$AryFld = array();
		if(is_array($fld)){
			//フィールド指定取得
			foreach($fld as $val){
				$AryFld[$val] = $val;
			}
		}else{
			//全フィールド取得
			$AryFld = $this->GetDataField($sys_no);
		}
		//データ取得
		$AryTmp		= $this->LoadData($pnt_id,$sys_no,$curTime,$fld,$limit);
		//ループ上限設定
		switch($sys_no){
			case "000":
				$total	= 1;
				break;
			case "100":
				$total	= 144;
				break;
			case "200":
				$total	= 24;
				break;
			case "300":
				$date = DateConvert($curTime);
				$y = $date["year"];
				$m = $date["month"];
				$m++;
				if($m==13){
					$y++;
					$m = 1;
				}
				$date		= DateConvert( strtotime("{$y}/{$m}/1 00:01")-86400 );
				$total	= $date["day"];
				break;
			case "400":
				$total	= 12;
				break;
		}

		//取得したデータを各時間枠に格納
		$AryData = array();
		if($sys_no=="100"){
			$date = DateConvert($curTime);
			$curTime = strtotime($date["year"]."/".$date["month"]."/".$date["day"]." 00:10");
		}
		for($i=0;$i<$total;$i++){
			switch($sys_no){
				case "000":
					$date = DateConvert($curTime);
					$time			= $date["date"];
					$key_time = $date["date"];
					break;
				case "100":
					$date = DateConvert($curTime + $i * 600);
					$time			= $date["hour"].$date["min"];
					$key_time = $date["hour"].":".$date["min"];
					break;
				case "200":
					$time			= sprintf("%02d",($i+1))."00";
					$key_time = sprintf("%02d",($i+1)).":00";
					break;
				default:
					$time			= sprintf("%02d",$i+1);
					$key_time = sprintf("%02d",$i+1);
			}
			foreach($AryFld as $key => $val){
				$value = trim($AryTmp[ $time ][ $key ]);
				if($value!=""){
					switch($key){
						case strpos(" ".$key,"_wd") > 0:
							$value = $dir[ (int) $value ];
							break;
						case strpos(" ".$key,"weather") > 0:
							$value = $weather[ (int) $value ];
							break;
					}
				}
				$AryData[$key_time][ $key ] = $value;
			}
		}
		return $AryData;
	}

	//単局グラフデータ取得用
	function LoadGraphData($pnt_id,$sys_no,$curTime,$gAry){
		$fld_mst = $this->GetGlaphField($sys_no,"","2");
		//取得フィールド設定
		$fld = array();
		$limit = array();
		foreach($gAry as $val){
			if($val!=""){
				$fld[] = $val;
				$limit[$val]["max"] =  $fld_mst[$val]["max"];
				$limit[$val]["min"] =  $fld_mst[$val]["min"];
			}
		}
		//print_r($limit);

		//データ取得
		$AryData = array();
		switch($sys_no){
			case "100":
			case "200":
				//基本日付の12時間前から12時間後のデータ取得
				$tmp = $this->LoadAryData($pnt_id,"100",$curTime-43200,$fld,$limit);
				$date = DateConvert($curTime-43200);
				foreach($tmp as $key => $val){
					$trg 	= $date["year"].$date["month"].$date["day"].str_replace(":","",$key);
					if($trg<$date["date"]) continue;
					$AryData[ $trg ] = $val;
				}
				$tmp = $this->LoadAryData($pnt_id,"100",$curTime+43200,$fld,$limit);
				$date = DateConvert($curTime+43200);
				foreach($tmp as $key => $val){
					$trg 	= $date["year"].$date["month"].$date["day"].str_replace(":","",$key);
					if($trg>$date["date"]) continue;
					$AryData[ $trg ] = $val;
				}
				break;
			case "300":
//					$tmp = $this->LoadAryData($pnt_id,$sys_no,$curTime-86400,$fld,$limit);
//					$date = DateConvert($curTime-86400);
//					foreach($tmp as $key => $val){
//						$trg 	= $date["year"].$date["month"].$key;
//						$AryData[ $trg ] = $val;
//					}
				$tmp = $this->LoadAryData($pnt_id,$sys_no,$curTime,$fld,$limit);
				$date = DateConvert($curTime);
				foreach($tmp as $key => $val){
					$trg 	= $date["year"].$date["month"].$key;
					$AryData[ $trg ] = $val;
				}
				break;
			case "400":
//					$tmp = $this->LoadAryData($pnt_id,$sys_no,$curTime-86400,$fld,$limit);
//					$date = DateConvert($curTime-86400);
//					foreach($tmp as $key => $val){
//						$trg 	= $date["year"].$key;
//						$AryData[ $trg ] = $val;
//					}
				$tmp = $this->LoadAryData($pnt_id,$sys_no,$curTime,$fld,$limit);
				$date = DateConvert($curTime);
				foreach($tmp as $key => $val){
					$trg 	= $date["year"].$key;
					$AryData[ $trg ] = $val;
				}
				break;
		}
		return $AryData;
	}


	//データをＨＴＭＬにして出力
	function GetDataTable($pnt_id,$sys_no,$curTime){
		//フィールド取得
		$AryField = $this->LoadAryHeader($sys_no);

		$AryData = $this->LoadAryData($pnt_id,$sys_no,$curTime);

		$AryTable = array();
		$row = 0;
		foreach($AryData as $time_key => $value){
			$AryTable[$row]["id"] = "id=\"33\"";

			$AryTmp = array();
			$AryTmp["id"]			= "id=\"1\"";
			$AryTmp["value"]	= $time_key;
			$AryTable[$row]["cols"][] = $AryTmp;
			foreach($AryField["fld_id"] as $fld){
				$AryTmp["id"] 		= "id=\"25\"";
				$AryTmp["value"]	= $AryData[$time_key][$fld]=="" ? "&nbsp" : $AryData[$time_key][$fld];
				$AryTable[$row]["cols"][] = $AryTmp;
			}
			$row++;
		}

		$args["data"] = $AryTable;

		require_once(ROOT_INC."htmltemplate.inc");
		$out = HtmlTemplate::t_Buffer(TEMPLATE_DIR."data.tmpl",$args);
		return $out;
	}

	//集計データデータ呼び出し（集計コート単位）
	function LoadSumData($pnt_id,$sys_no,$curTime){
		$AryData = $this->GetSumDataField($sys_no);

		$date = DateConvert($curTime);
		$tbl = "t{$sys_no}_dat_".$date["year"];
		switch($sys_no){
			case "200":
				$trg = $date["year"].$date["month"].$date["day"];
				break;
			case "300":
				$trg = $date["year"].$date["month"];
				break;
			case "400":
				$trg = $date["year"];
				break;
			default:
				return false;
		}

		//$AryData	= array();
		$strSql		= "SELECT * FROM {$tbl} WHERE t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date LIKE '{$trg}%' AND t{$sys_no}_code IS NOT NULL;";
		//print $strSql."\n";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

		$dbs = new CONN;
		$row = $dbs->Query($strSql);

		while($row){
			$code = $row["t{$sys_no}_code"];
			foreach($AryData[$code] as $fld_id => $val){
				$AryData[$code][$fld_id] = $row[$fld_id];
			}
			$row = $dbs->Next();
		}
		return $AryData;
	}

	//集計データデータ呼び出し（集計名単位)
	function LoadArySumData($pnt_id,$sys_no,$curTime){
		global $dir,$dir2,$weather;

		$AryTmp = $this->LoadSumData($pnt_id,$sys_no,$curTime);

		$AryData = array();
		foreach($AryTmp as $sum_no => $value){
			if($sum_no=="90") continue;
			foreach($value as $key => $val){
				$sum_name = $this->ArySumName[ $sum_no ];

				if($val!=""){
					switch($key){
						case strpos(" ".$key,"_wd") > 0:
							$val = $dir[ (int) $val ];
							break;
						case strpos(" ".$key,"weather") > 0:
							$val = $weather[ (int) $val ];
							break;
					}
				}
				$AryData[ $sum_name ][ $key ] = $val;
			}
		}

		$AryWind	= explode(",",$AryTmp["90"]["avg_ws"]);
		$AryTmp		= array();
		for($i=0;$i<count($dir2);$i++){
			$AryTmp[ $dir2[$i] ] = $AryWind[$i];
		}
		$sum_name = $this->ArySumName["90"];
		$AryData[$sum_name] = $AryTmp;

		return $AryData;
	}

	//集計データをＨＴＭＬにして出力
	function GetSumDataTable($pnt_id,$sys_no,$curTime){

		//フィールド取得
		$AryField = $this->LoadAryHeader($sys_no);

		$AryData = $this->LoadArySumData($pnt_id,$sys_no,$curTime);
		$this->ArySumData = $AryData;

		$AryTable = array();
		$row = 0;
		foreach($AryData as $sum_name => $value){
			if($sum_name=="風向頻度") continue;
			$AryTable[$row]["id"] = "id=\"33\"";

			$AryTmp = array();
			$AryTmp["id"]			= "id=\"1\"";
			$AryTmp["value"]	= strpos(" ".$sum_name,"起時") > 0 ? "起時" : $sum_name;
			$AryTable[$row]["cols"][] = $AryTmp;
			foreach($AryField["fld_id"] as $fld){
				$AryTmp["id"] 		= "id=\"25\"";
				$AryTmp["value"]	= $AryData[$sum_name][$fld]=="" ? "&nbsp" : $AryData[$sum_name][$fld];
				$AryTable[$row]["cols"][] = $AryTmp;
			}
			$row++;
		}

		$args["data"] = $AryTable;

		require_once(ROOT_INC."htmltemplate.inc");
		$out = HtmlTemplate::t_Buffer(TEMPLATE_DIR."data_sum.tmpl",$args);
		return $out;
	}

	//風向頻度データをＨＴＭＬにして出力
	function GetWindDataTable($pnt_id,$sys_no,$curTime){
		global $dir2;

		if(!is_array($this->ArySumData)){
			$this->ArySumData = $this->LoadArySumData($pnt_id,$sys_no,$curTime);
		}

		$AryTable = array();

		$AryTmp = array();
		$AryTmp["id"]			= "id=\"1\"";
		$AryTmp["value"]	= "風向頻度";
		$AryTable[0]["cols"][] = $AryTmp;
		for($i=0;$i<count($dir2);$i++){
			$AryTmp["id"]			= "id=\"1\"";
			$AryTmp["value"]	= $dir2[$i];
			$AryTable[0]["cols"][] = $AryTmp;
		}

		$AryTmp["id"]			= "id=\"1\"";
		$AryTmp["value"]	= "％";
		$AryTable[1]["cols"][] = $AryTmp;
		for($i=0;$i<count($dir2);$i++){
			$AryTmp["id"]			= "id=\"1\"";
			$AryTmp["value"]	= $this->ArySumData["風向頻度"][ $dir2[$i] ]=="" ? "&nbsp;" : $this->ArySumData["風向頻度"][ $dir2[$i] ];
			$AryTable[1]["cols"][] = $AryTmp;
		}
		$args["data"] = $AryTable;

		require_once(ROOT_INC."htmltemplate.inc");
		$out = HtmlTemplate::t_Buffer(TEMPLATE_DIR."data_wind.tmpl",$args);
		return $out;
	}

	//各帳票の連想配列の生成
	function LoadHeader($sys_no){
		$Field	= array();
		$strSql = "SELECT fld_id,fld_type,fld_name1,fld_name2,fld_unit FROM t_fldmst WHERE  fld_enable = 1 AND fld_{$sys_no}_enable = 1 ORDER BY fld_sort;";

		$dbs = new CONN;
		if(!$row = $dbs->Query($strSql)){
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
		}
		while($row){
			$key = $row["fld_id"];
			$Field[$key]["fld_id"]		= $row["fld_id"];
			$Field[$key]["fld_type"]	= $row["fld_type"];
			$Field[$key]["fld_name1"]	= $row["fld_name1"];
			$Field[$key]["fld_name2"]	= $row["fld_name2"];
			$Field[$key]["fld_name3"]	= $row["fld_name3"];
			$Field[$key]["fld_unit"]	= $row["fld_unit"];
			$Field[$key]["fld_scale"]	= $row["fld_scale"];
			$row = $dbs->Next();
		}
		if(is_object($this->log))	$this->log->logPrint("LoadAryHeader => ".print_r($Field,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $Field;
	}

	//各帳票の連想配列の生成
	function LoadAryHeader($sys_no){
		$Field	= array();
		if($sys_no=="000"){
			$strSql = "SELECT fld_id,fld_type,fld_name1,fld_name2,fld_unit FROM t_fldmst WHERE  fld_enable = 1 AND fld_{$sys_no}_enable = 1 ORDER BY fld_sort;";
		}else{
			$strSql = "SELECT fld_id,fld_type,fld_name1,fld_name2,fld_unit FROM t_fldmst WHERE  fld_enable = 1 AND fld_{$sys_no}_enable = 1 AND fld_{$sys_no}_out = 1 ORDER BY fld_sort;";
		}

		$dbs = new CONN;
		if(!$row = $dbs->Query($strSql)){
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
		}
		$fld_id			= array();
		$fld_type		= array();
		$fld_name1	= array();
		$fld_name2	= array();
		$fld_unit		= array();
		while($row){
			$fld_id[]			= $row["fld_id"];
			$fld_type[]		= $row["fld_type"];
			$fld_name1[]	= $row["fld_name1"];
			$fld_name2[]	= $row["fld_name2"];
			$fld_unit[]		= $row["fld_unit"];
			$row = $dbs->Next();
		}
		$Field["fld_id"]		= $fld_id;
		$Field["fld_type"]	= $fld_type;
		$Field["fld_name1"] = $fld_name1;
		$Field["fld_name2"] = $fld_name2;
		$Field["fld_unit"]	= $fld_unit;
		if(is_object($this->log))	$this->log->logPrint("LoadAryHeader => ".print_r($Field,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $Field;
	}

	//各帳票のヘッダーＨＴＭＬ出力
	function GetFieldTable($pnt_id,$sys_no){
		//局情報読込み
		$pnt   = new Points($this->log);
		$Point = $pnt->LoadPointInfo($pnt_id);
		//帳票毎の連想配列を読み込み
		$AryTmp = $this->LoadAryHeader($sys_no);
		switch($sys_no){
			case "100":
			case "200":
				$args["time"] ="時間";
				break;
			case "300":
				$args["time"] ="日";
				break;
			case "400":
				$args["time"] ="月";
				break;
		}

		//ＨＴＭＬ出力用に連想配列を生成
		$fld_type		= array();
		$fld_name1	= array();
		$fld_unit		= array();
		$col = 1;
		for($i=0;$i<count($AryTmp["fld_id"]);$i++){
			$fld_type[ $AryTmp["fld_type"][$i] ]["name"] = $AryTmp["fld_type"][$i];
			if($i>0){
				if($AryTmp["fld_type"][$i]==$AryTmp["fld_type"][($i-1)]){
					$col++;
					$fld_type[ $AryTmp["fld_type"][$i] ]["colspan"] = "colspan=\"{$col}\"";
				}else{
					$col = 1;
				}
			}

			//月報帳票は天気用処理
			if($sys_no=="300" && strpos(" ".$AryTmp["fld_id"][$i],"weather")>0){
				$AryTmp["fld_name1"][$i] = $Point["pnt_300_".$AryTmp["fld_id"][$i] ]."時";
			}


			if($AryTmp["fld_name1"][$i]!="")	$fld_name1[ $AryTmp["fld_name2"][$i] ]["name"]	= $AryTmp["fld_name1"][$i];
			if($AryTmp["fld_unit"][$i]!=""){
				$fld_unit[ 	$AryTmp["fld_name2"][$i] ]["name"] = "(".$AryTmp["fld_unit"][$i].")";
			}

			//rowspan設定
			if($AryTmp["fld_type"][$i]!="" && $AryTmp["fld_name1"][$i]=="" && $AryTmp["fld_unit"][$i]==""){
				$fld_type[ $AryTmp["fld_type"][$i] ]["rowspan"] = "rowspan=\"3\"";
			}
			//rowspan設定
			if($AryTmp["fld_type"][$i]!="" && $AryTmp["fld_name1"][$i]=="" && $AryTmp["fld_unit"][$i]!=""){
				$fld_type[ $AryTmp["fld_type"][$i] ]["rowspan"] = "rowspan=\"2\"";
			}
			//rowspan設定
			if($AryTmp["fld_type"][$i]!="" && $AryTmp["fld_name1"][$i]!="" && $AryTmp["fld_unit"][$i]==""){
				$fld_name1[ $AryTmp["fld_name2"][$i] ]["rowspan"] = "rowspan=\"2\"";
			}
		}

		//テンプレート用に配列を再配置
		foreach($fld_type as $val){
			$args["fld_type"][] = $val;
		}
		foreach($fld_name1 as $val){
			$args["fld_name1"][] = $val;
		}
		foreach($fld_unit as $val){
			$args["fld_unit"][] = $val;
		}

		if(is_object($this->log))	$this->log->logPrint("GetFieldTable => ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);

		require_once(ROOT_INC."htmltemplate.inc");
		$out = HtmlTemplate::t_Buffer(TEMPLATE_DIR."header.tmpl",$args);
		return $out;
	}


	//データ読込み
	function LoadDataMulti($sys_no,$curTime,$fld="",$limit=""){
		//取得フィールド設定
		$AryFld = array();
		if(is_array($fld)){
			//フィールド指定取得
			foreach($fld as $val){
				$AryFld[$val] = $val;
			}
		}else{
			//全フィールド取得
			$AryFld = $this->GetDataField($sys_no);
		}

		//データ日付設定
		$date = DateConvert($curTime);
		$tbl = "t{$sys_no}_dat_".$date["year"];
		switch($sys_no){
			case "100":
				$trg = $date["year"].$date["month"].$date["day"];
				$sub_start	= 8;
				$sub_end 		= 4;
				break;
			case "200":
				$trg = $date["year"].$date["month"].$date["day"];
				$sub_start	= 8;
				$sub_end 		= 4;
				break;
			case "300":
				$trg = $date["year"].$date["month"];
				$sub_start	= 6;
				$sub_end 		= 2;
				break;
			case "400":
				$trg = $date["year"];
				$sub_start	= 4;
				$sub_end 		= 2;
				break;
			default:
				return false;
		}

		//データ取得開始
		$AryData	= array();
		$strWhere = "FROM {$tbl} WHERE t{$sys_no}_date LIKE '{$trg}%' AND t{$sys_no}_code IS NULL;";
		if(is_array($fld)){
			$strSql	= "SELECT ".join(",",$fld).",t{$sys_no}_date,t{$sys_no}_pnt_id ".$strWhere;
		}else{
			$strSql	= "SELECT * ".$strWhere;
		}
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		while($row){
			$AryTmp = array();
			foreach($AryFld as $key => $val){
				if(is_array($limit)){
					//値を丸める場合
					$AryTmp[$key] = $row[$key];
					if($limit[$key]["max"] < $row[$key])	$AryTmp[$key] = $limit[$key]["max"];
					if($limit[$key]["min"] > $row[$key])	$AryTmp[$key] = $limit[$key]["min"];
				}else{
					$AryTmp[$key] = $row[$key];
				}
			}
			$pnt_id = $row["t{$sys_no}_pnt_id"];
			if($pnt_id!=""){
				$AryData[ substr($row["t{$sys_no}_date"],$sub_start,$sub_end) ][$pnt_id]	= $AryTmp;
			}
			$row = $dbs->Next();
		}
		return $AryData;
	}

	//データを時間配列に格納して読込み
	function LoadAryDataMulti($sys_no,$curTime,$fld="",$limit=""){
		global $dir,$weather;

		//全局ID取得
		$pnt = new Points;
		$AryPnt = array();
		foreach($pnt->GetPoints() as $val){
			if($val["pnt_enable"])	$AryPnt[] =	$val["pnt_id"];
		}

		//取得フィールド設定
		$AryFld = array();
		if(is_array($fld)){
			//フィールド指定取得
			foreach($fld as $val){
				$AryFld[$val] = $val;
			}
		}else{
			//全フィールド取得
			$AryFld = $this->GetDataField($sys_no);
		}
		//データ取得
		$AryTmp		= $this->LoadDataMulti($sys_no,$curTime,$fld,$limit);
		//ループ上限設定
		switch($sys_no){
			case "100":
				$total	= 144;
				break;
			case "200":
				$total	= 24;
				break;
			case "300":
				$date = DateConvert($curTime);
				$y = $date["year"];
				$m = $date["month"];
				$m++;
				if($m==13){
					$y++;
					$m = 1;
				}
				$date		= DateConvert( strtotime("{$y}/{$m}/1 00:01")-86400 );
				$total	= $date["day"];
				break;
			case "400":
				$total	= 12;
				break;
				return false;
		}

		//取得したデータを各時間枠に格納
		$AryData = array();
		if($sys_no=="100"){
			$date = DateConvert($curTime);
			$curTime = strtotime($date["year"]."/".$date["month"]."/".$date["day"]." 00:10");
		}
		for($i=0;$i<$total;$i++){
			switch($sys_no){
				case "100":
					$date = DateConvert($curTime + $i * 600);
					$time			= $date["hour"].$date["min"];
					$key_time = $date["hour"].":".$date["min"];
					break;
				case "200":
					$time			= sprintf("%02d",($i+1))."00";
					$key_time = sprintf("%02d",($i+1)).":00";
					break;
				default:
					$time			= sprintf("%02d",$i+1);
					$key_time = sprintf("%02d",$i+1);
			}

			foreach($AryPnt as $pnt_id){
				foreach($AryFld as $key => $val){
					$value = trim($AryTmp[ $time ][ $pnt_id ][ $key ]);
					if($value!=""){
						switch($key){
							case strpos(" ".$key,"_wd") > 0:
								$value = $dir[ (int) $value ];
								break;
							case strpos(" ".$key,"weather") > 0:
								$value = $weather[ (int) $value ];
								break;
						}
					}
					$AryData[$key_time][ $pnt_id ][ $key ] = $value;
				}
			}

		}
		return $AryData;
	}

	//集計データデータ呼び出し（集計コート単位）
	function LoadSumDataMulti($sys_no,$curTime,$fld=""){
		//取得フィールド設定
		$AryFld = array();
		if(is_array($fld)){
			//フィールド指定取得
			foreach($fld as $val){
				$AryFld[$val] = $val;
			}
		}else{
			//全フィールド取得
			$AryFld = $this->GetDataField($sys_no);
		}

		$date = DateConvert($curTime);
		$tbl = "t{$sys_no}_dat_".$date["year"];
		switch($sys_no){
			case "200":
				$trg = $date["year"].$date["month"].$date["day"];
				if(is_array($fld)){
					if(in_array("rain_hour",$fld))	$rain_flg = 1;
				}
				break;
			case "300":
				$trg = $date["year"].$date["month"];
				break;
			case "400":
				$trg = $date["year"];
				break;
			default:
				return false;
		}

		$AryData	= array();

		//データ取得開始
		$AryData	= array();
		$strWhere = "FROM {$tbl} WHERE t{$sys_no}_date LIKE '{$trg}%' AND t{$sys_no}_code IS NOT NULL;";
		if(is_array($fld)){
			if($rain_flg){
				$strSql	= "SELECT ".join(",",$fld).",rain_day,t{$sys_no}_date,t{$sys_no}_pnt_id,t{$sys_no}_code ".$strWhere;
			}else{
				$strSql	= "SELECT ".join(",",$fld).",t{$sys_no}_date,t{$sys_no}_pnt_id,t{$sys_no}_code ".$strWhere;
			}
		}else{
			$strSql	= "SELECT * ".$strWhere;
		}
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

		$dbs = new CONN;
		$row = $dbs->Query($strSql);

		$AryData = array();
		while($row){
			$code		= $row["t{$sys_no}_code"];

			//風向頻度は出力しない
			if($code=="90"){
				$row = $dbs->Next();
				continue;
			}

			$pnt_id = $row["t{$sys_no}_pnt_id"];
			foreach($AryFld as $fld_id => $val){
				$AryData[$code][$pnt_id][$fld_id] = $row[$fld_id];
				if($rain_flg){
					if($code=="70"&&$fld_id=="rain_hour"){
						//$AryData[$code][$pnt_id][$fld_id] = $row["rain_day"];
					}
				}
			}
			$row = $dbs->Next();
		}
		return $AryData;
	}

	//集計データデータ呼び出し（集計名単位)
	function LoadArySumDataMulti($sys_no,$curTime,$fld){
		global $dir,$dir2,$weather;

		//全局ID取得
		$pnt = new Points;
		$AryPnt = array();
		foreach($pnt->GetPoints() as $val){
			if($val["pnt_enable"])	$AryPnt[] =	$val["pnt_id"];
		}

		//取得フィールド設定
		$AryFld = array();
		if(is_array($fld)){
			//フィールド指定取得
			foreach($fld as $val){
				$AryFld[$val] = $val;
			}
		}else{
			//全フィールド取得
			$AryFld = $this->GetDataField($sys_no);
		}

		$AryTmp = $this->LoadSumDataMulti($sys_no,$curTime,$fld);

		$AryData = array();
		if(!is_array($AryTmp)) $AryTmp = array();
		foreach($AryTmp as $sum_no => $value){
			if($sum_no=="90") continue;

			foreach($AryPnt as $pnt_id){
				if(!is_array($value[$pnt_id])) continue;
				foreach($value[$pnt_id] as $key => $val){
					$sum_name = $this->ArySumName[ $sum_no ];

					if($val!=""){
						switch($key){
							case strpos(" ".$key,"_wd") > 0:
								$val = $dir[ (int) $val ];
								break;
							case strpos(" ".$key,"weather") > 0:
								$val = $weather[ (int) $val ];
								break;
						}
					}
					$AryData[ $sum_name ][ $pnt_id ][ $key ] = $val;
				}
			}

		}

//			$AryWind	= explode(",",$AryTmp["90"]["avg_ws"]);
//			$AryTmp		= array();
//			for($i=0;$i<count($dir2);$i++){
//				$AryTmp[ $dir2[$i] ] = $AryWind[$i];
//			}
//			$sum_name = $this->ArySumName["90"];
//			$AryData[$sum_name] = $AryTmp;

		return $AryData;
	}

	//天気情報をファイルに設定
	function SetWeather($pnt_id,$no){
		@unlink(WEATHER_DIR.$pnt_id);
		$fp = @fopen(WEATHER_DIR.$pnt_id, "a");
		if($fp){
			if(is_numeric($no)){
				@fwrite($fp,sprintf("%02d",$no));
			}else{
				@fclose($fp);
				return false;
			}
			@fclose($fp);
			return true;
		}else{
			return false;
		}
	}

	//天気情報をファイルから取得
	function GetWeather($pnt_id){
		if($weather = @file(WEATHER_DIR.$pnt_id)){
			$weather = trim($weather[0]);
			if(is_numeric($weather)){
				return sprintf("%02d",$weather);
			}else{
				return "00";
			}
		}else{
			return "00";
		}
	}


}
?>