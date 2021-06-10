<?
//============================================================//
//	フィールド処理クラス																			//
//------------------------------------------------------------//
//	処理概要																									//
//	　フィールド情報の設定＆取得															//
//------------------------------------------------------------//
//	変更履歴																									//
//		2006/11/10	N.ARAI																			//
//		・新規作成																							//
//============================================================//
class Fields
{
	var $varsion	= "1.0";
	var $update		= "2006/11/10";

	//変数宣言
	var $log;

	//コンストラクタ
	function __construct(&$log = ""){
		if(is_object($log))	$this->log = $log;
	}

	//フィールド情報取得（フィールドIDをキーとする）
	function GetFields($sys_no=""){
		$Field	= array();
		if($sys_no==""){
			$strSql = "SELECT * FROM t_fldmst WHERE  fld_enable = 1 ORDER BY fld_sort;";
		}else{
			$strSql = "SELECT * FROM t_fldmst WHERE  fld_enable = 1 AND fld_{$sys_no}_enable = 1 ORDER BY fld_sort;";
		}
		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		while($row){
			$fld_id = $row["fld_id"];
			$Field[ $fld_id ]["fld_no"]			= $row["fld_no"];
			$Field[ $fld_id ]["fld_id"]			= $row["fld_id"];
			$Field[ $fld_id ]["fld_type"]		= $row["fld_type"];
			$Field[ $fld_id ]["fld_name1"]	= $row["fld_name1"];
			$Field[ $fld_id ]["fld_name2"]	= $row["fld_name2"];
			$Field[ $fld_id ]["fld_name3"]	= $row["fld_name3"];
			$Field[ $fld_id ]["fld_unit"]		= $row["fld_unit"];
			$Field[ $fld_id ]["fld_scale"]	= $row["fld_scale"];
			$row = $dbs->Next();
		}
		return $Field;
	}

	//フィールド情報取得
	function LoadFieldInfo($sys_no=""){
		$Fields = array();

		switch($sys_no){
			case "000":
				$strWhere = "WHERE fld_enable = 1 AND fld_000_enable = 1";
				break;
			case "100":
				$strWhere = "WHERE fld_enable = 1 AND fld_100_enable = 1";
				break;
			case "200":
				$strWhere = "WHERE fld_enable = 1 AND fld_200_enable = 1";
				break;
			case "300":
				$strWhere = "WHERE fld_enable = 1 AND fld_300_enable = 1";
				break;
			case "400":
				$strWhere = "WHERE fld_enable = 1 AND fld_400_enable = 1";
				break;
		}

		$strSql = "SELECT * FROM t_fldmst {$strWhere} ORDER BY fld_sort";

		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		$cnt = 0;
		while($row){
			$Fields[] = $row;
			foreach($row as $key => $val){
				if(strpos($key,"_enable")||strpos($key,"00_graph")||strpos($key,"00_out")){
					if($val) $Fields[ count($Fields)-1 ][$key."_value"] = "checked";
				}
			}
			if($cnt%2) $Fields[ count($Fields)-1 ]["bg"]	= "bgcolor = '#D3D3D3'";


			$Fields[ count($Fields)-1 ]["bg2"] = "style = 'border:0px solid;'";
			if($cnt%2) $Fields[ count($Fields)-1 ]["bg2"] = "style = 'background:#D3D3D3; border:0px solid;'";

			$cnt++;

			$Fields[ count($Fields)-1 ]["no"] = $cnt;

			$row = $dbs->Next();
		}
		return $Fields;
	}

	//フィールド情報追加
	function AddField(){
		$dbs = new CONN;

		$LastValue = $dbs->GetSeqLastValue("t_fldmst_seq") + 1;
		$strSql = "INSERT INTO t_fldmst VALUES (default,'field{$LastValue}','field{$LastValue}','field{$LastValue}','field{$LastValue}','field{$LastValue}',{$LastValue},'','',0,0,0,0,0,0,0,0,0,0,0,0,0,0);";

		if(is_object($this->log))	$this->log->logPrint($strSql,REC_NOTE,__LINE__,__FILE__);

		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}

	//フィールド情報保存
	function SaveFieldInfo($args){
		$dbs = new CONN;
		$fields = $dbs->GetMetaData("t_fldmst");

		foreach($args["fld_no"] as $fld_no){
			$AryTmp = array();
			foreach($fields as $key => $val){
				if($key=="fld_no") continue;
				//if(!array_key_exists($key,$args)) continue;
				switch($val["type"]){
					case "varchar":
						if($args[$key][$fld_no]!=""){
							$AryTmp[] = "$key = '".$args[$key][$fld_no]."'";
						}else{
							$AryTmp[] = "$key = ''";
						}
						break;
					default:
						if($args[$key][$fld_no]!=""){
							$AryTmp[] = "$key = ".$args[$key][$fld_no];
						}else{
							if($args["sys_no"]==""){
								$AryTmp[] = "$key = 0";
							}else{
								if(strpos(" ".$key,$args["sys_no"])){
									$AryTmp[] = "$key = 0";
								}
							}
						}
						break;
				}
			}
			$strSql = "UPDATE t_fldmst SET ".join(",",$AryTmp)." WHERE fld_no = '{$fld_no}';";

			if(is_object($this->log))	$this->log->logPrint($strSql,REC_NOTE,__LINE__,__FILE__);

			if(!$dbs->Execute($strSql))	return false;
		}
		return true;
	}

	//フィールド情報削除
	function DeleteFieldInfo($fld_no){
		$strSql = "DELETE FROM t_fldmst WHERE fld_no = {$fld_no};";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_NOTE,__LINE__,__FILE__);

		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			return false;
		}
	}


}
?>