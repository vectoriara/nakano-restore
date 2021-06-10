<?
//============================================================//
//	�ɾ��󥯥饹																							//
//------------------------------------------------------------//
//	��������																									//
//	���ɾ���Υ����ɡ������֤ʤ�															//
//------------------------------------------------------------//
//	�ѹ�����																									//
//		2006/11/10	N.ARAI																			//
//		����������																							//
//============================================================//
class Points
{
	var $varsion	= "1.0";
	var $update		= "2006/12/29";

	//�ѿ����
	var $log;

	//���󥹥ȥ饯��
	function __construct(&$log = ""){
		if(is_object($log))	$this->log = $log;
	}

	//���ݶɤζ�ID����̾�μ�����Form�ѡ�
	function GetWeatherPointsForForm($select=""){
		$AryTmp = $this->GetWeatherPoints();
		foreach($AryTmp as $row){
			$Points .= "<option value=\"{$row["pnt_id"]}\"".($select==$row["pnt_id"]? " selected":"").">{$row["pnt_name"]}</option>\n";
		}
		return $Points;
	}

	//��ID����̾�μ�����Form�ѡ�
	function GetPointsForForm($select=""){
		$Points = "";
		$strSql = "SELECT pnt_id,pnt_name,pnt_enable FROM t_pntmst ORDER BY pnt_sort;";

		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		while($row){
			$Points .= "<option value=\"{$row["pnt_id"]}\"".($select==$row["pnt_id"]? " selected":"").">{$row["pnt_name"]}</option>\n";
			$row = $dbs->Next();
		}
		return $Points;
	}

	//��ID�����̾�μ���
	function GetPointNameById($pnt_id){
		$dbs = new CONN;
		$strSql = "SELECT pnt_name FROM t_pntmst WHERE pnt_id = '$pnt_id';";
		if(!$row=$dbs->Query($strSql))	return false;
		return $row['pnt_name'];
	}

	//��ID����̾�μ���	[2007/02/16]
	function GetPoints($select="",$enable=0){
		$Points = array();
		$strSql = "SELECT * FROM t_pntmst ORDER BY pnt_sort;";

		$dbs = new CONN;
		$row = $dbs->Query($strSql);
		while($row){
			if($enable==1 && $row["pnt_enable"]=="0"){
				$row = $dbs->Next();
				continue;
			}
			$Points[]["pnt_id"]												= $row["pnt_id"];
			$Points[ count($Points)-1 ]["pnt_name"]		= $row["pnt_name"];
			$Points[ count($Points)-1 ]["pnt_name2"]	= $row["pnt_name2"];
			$Points[ count($Points)-1 ]["pnt_enable"]	= $row["pnt_enable"];

			//���ϥե�����ʤɤǤ�select�������������
			if($select==$row["pnt_id"]){
				$Points[ count($Points)-1 ]["selected"]	= 1;
			}
			$row = $dbs->Next();
		}
		return $Points;
	}

	//�ɾ������	[2006/12/29]
	function LoadPointInfo($pnt_id){

		//�������������ͤ�ե����뤫���ɤ߹���
		$filepath = ROOT_INC."point_{$pnt_id}.dat";
		if(file_exists($filepath)){
			if($line = @file($filepath)){
				$args = array();
				foreach($line as $value){
					$tmp = explode("@@@@@",trim($value));
					$args[$tmp[0]] = $tmp[1];
				}
				return $args;
			}
		}

		if(!is_object($this->dbs)){
			if(is_object($this->log))	$this->log->logPrint("New DB Connect",REC_NOTE,__LINE__,__FILE__);
			$this->dbs = new CONN;
		}

		$dbs = new CONN;
		//�ɾ���ơ��֥�Υե�������������
		$fields = $dbs->GetMetaData("t_pntmst");

		$args = array();
		$strSql = "SELECT * FROM t_pntmst WHERE pnt_id = '$pnt_id';";
		if(!$row=$dbs->Query($strSql)){
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
		foreach($row as $key => $val){
			if(array_key_exists($key,$fields)){
				switch($key){
					case "pnt_tmg_000":
					case "pnt_tmg_100":
					case "pnt_tmg_200":
					case "pnt_tmg_300":
					case "pnt_tmg_400":
					case "pnt_tmg_time":
					case "pnt_log":
					case "pnt_tmg_weather":
						$args[$key."_".$val] = 1;
						$args[$key] = $val;
						break;
					case "pnt_enable":
						$args[$key] = ($val==0 ? "" : $val);
						break;
					default:
						$args[$key] = $val;
						break;
				}
			}
		}
		$args["pnt_id_old"] = $row["pnt_id"];

		//�������������ͤ�ե��������¸
		$fp = @fopen($filepath,"w");
		if($fp){
			foreach($args as $key => $val){
				@fputs($fp,"{$key}@@@@@{$val}\n");
			}
			@fclose($fp);
			@chmod($filepath, 0777);
			if(is_object($this->log))	$this->log->logPrint("Save Setting [{$filepath}] ".print_r($args,TRUE),REC_NOTE,__LINE__,__FILE__);
		}

		return $args;
	}

	//�ɾ�����¸
	function SavePointInfo($args){

		$dbs = new CONN;
		$fields = $dbs->GetMetaData("t_pntmst");

		$fld		= array();
		$value	= array();
		if($args["pnt_id_old"]==""){
			foreach($fields as $key => $val){
				$fld[] = $key;
				switch($val["type"]){
					case "varchar":
						if($args[$key]!=""){
							$value[] = "'".$args[$key]."'";
						}else{
							$value[] = "''";
						}
						break;
					default:
						if($args[$key]!=""){
							$value[] = $args[$key];
						}else{
							$value[] = 0;
						}
						break;
				}
			}
			$strSql = "INSERT INTO t_pntmst (".join(",",$fld).") VALUES (".join(",",$value).");";
		}else{
			$AryTmp = array();
			foreach($args as $key => $val){
				if(array_key_exists($key,$fields)){
					switch($fields[ $key ]["type"]){
						case "varchar":
							$AryTmp[] = "$key = '$val'";
							break;
						default:
							$AryTmp[] = "$key = $val";
							break;
					}
				}
			}

			if(!array_key_exists("pnt_enable",$args)){
				$AryTmp[] = "pnt_enable = 0";
				Alert::StopAlert($args["pnt_id_old"]);
			}

			$strSql = "UPDATE t_pntmst SET ".join(",",$AryTmp)." WHERE pnt_id = '".$args["pnt_id_old"]."';";
		}

		if(is_object($this->log))	$this->log->logPrint($strSql,REC_NOTE,__LINE__,__FILE__);

		if($dbs->Execute($strSql)){
			@unlink(ROOT_INC."point_".$args["pnt_id"].".dat");
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	//����������ɲ�
	function AddLogger($pnt,$logger){
		$strSql = "INSERT INTO t_lgrdat (lgr_id,lgr_pnt_id,lgr_enable,lgr_time) VALUES ('$pnt','$logger',0,0);";

		if(is_object($this->log))	$this->log->logPrint($strSql,REC_NOTE,__LINE__,__FILE__);

		$dbs = new CONN;
		if($dbs->Execute($strSql)){
			return true;
		}else{
			if(is_object($this->log))	$this->log->logPrint($dbs->getError(),REC_NOTE,__LINE__,__FILE__);
			return false;
		}
	}

	//�������������	[2006/12/29]
	function GetLoggers($pnt_id=""){
		$Loggers	= array();
		$dbs			= new CONN;
		$row			=	$dbs->Query("SELECT lgr_id,lgr_pnt_id,lgr_enable,lgr_time FROM t_lgrdat".($pnt_id=="" ? "" : " WHERE lgr_pnt_id = '{$pnt_id}';").";");
		while($row){
			$AryTmp = array();
			$AryTmp["lgr_id"]			= $row["lgr_id"];
			$AryTmp["lgr_pnt_id"] = $row["lgr_pnt_id"];
			$AryTmp["lgr_enable"] = $row["lgr_enable"];
			$AryTmp["lgr_time"] 	= $row["lgr_time"];
			$AryTmp["class"]			= "Logger".$row["lgr_id"];
			$Loggers[]	= $AryTmp;
			$row = $dbs->Next();
		}
		return $Loggers;
	}

	//�ɤ��ȤˤĤ��Ƥ���Ģɼ�ǽ��ϲ�ǽ����
	function GetOutField($pnt_id,$sys_no){
		$AryField = array();

		//�������������
		$AryTmp = array();
		$logger = $this->GetLoggers($pnt_id);
		for($i=0;$i<count($logger);$i++){
			$clsTmp		= $logger[$i]["class"];	//���饹̾����
			$cls[$i]	= new $clsTmp($log);		//���饹���
			$tmp = $cls[$i]->OutField;
			if(is_array($tmp[$sys_no])){
				foreach($tmp[$sys_no] as $val) $AryTmp[$val] = "";
			}
		}

		$cls		= new Data($this->log);
		$Field	= $cls->LoadAryHeader($sys_no);


		$cnt = 0;
		foreach($Field["fld_id"] as $fld_id){
			if(array_key_exists($fld_id,$AryTmp)){
				$AryField[$fld_id] = $Field["fld_name2"][$cnt];
			}
			$cnt++;
		}
		return $AryField;
	}


	//�ɤ��ȤˤĤ��Ƥ�����������ܤν��ϲ�ǽ�ե������̾
	function GetLoggerOutField($pnt_id,$sys_no){
		$AryField = array();

		//�������������
		$AryTmp = array();
		$logger = $this->GetLoggers($pnt_id);
		for($i=0;$i<count($logger);$i++){
			$clsTmp		= $logger[$i]["class"];	//���饹̾����
			$cls[$i]	= new $clsTmp($log);		//���饹���
			$tmp = $cls[$i]->OutField;
			if(is_array($tmp[$sys_no])){
				foreach($tmp[$sys_no] as $val) $AryTmp[] = $val;
			}
		}

		return $AryTmp;
	}



	//�ɤ��ȤˤĤ��Ƥ���������Խ�����
	function GetEditField($pnt_id,$sys_no){
		$AryField = array();

		//�������������
		$AryTmp = array();
		$logger = $this->GetLoggers($pnt_id);
		for($i=0;$i<count($logger);$i++){
			$clsTmp		= $logger[$i]["class"];	//���饹̾����
			$cls[$i]	= new $clsTmp($log);		//���饹���
			$tmp = $cls[$i]->EditField;
			if(is_array($tmp[$sys_no])){
				foreach($tmp[$sys_no] as $val) $AryTmp[$val] = "";
			}
		}
		$cls		= new Data($this->log);
		$Field	= $cls->LoadAryHeader($sys_no);

		$cnt = 0;
		foreach($Field["fld_id"] as $fld_id){
			if(array_key_exists($fld_id,$AryTmp)){
				$AryField[$fld_id] = $Field["fld_name2"][$cnt];
			}
			$cnt++;
		}
		return $AryField;
	}


	//���ɤˤĤ��Ƥ�������������ư�Խ��Ǥ���Ģɼ���������ʼ�ư�Խ���������Ǥ���Ģɼ��������ɽ���˻��ѡ�
	function GetEditSys($select=""){
		$Point = $this->GetPoints($select="");

		$AryTmp = array();
		foreach($Point as $pnt){
			if($pnt["pnt_enable"]){
				//�������������
				$logger = $this->GetLoggers($pnt["pnt_id"]);
				for($i=0;$i<count($logger);$i++){
					$clsTmp		= $logger[$i]["class"];	//���饹̾����
					$cls[$i]	= new $clsTmp($log);		//���饹���
					if(is_array($cls[$i]->EditSys)){
						foreach($cls[$i]->EditSys as $val) $AryTmp[$val] = $val;
					}
					if(is_array($cls[$i]->DelSys)){
						foreach($cls[$i]->DelSys as $val) $AryTmp[$val] = $val;
					}
					//�߷ױ��̤����뤫�ʤ���
					if(is_array($cls[$i]->OutField["000"])){
						if(in_array("rain_rui",$cls[$i]->OutField["000"]))	$AryTmp["rain"] = 1;
					}
					//���ݥ����������פ��ɤ���
					if($cls[$i]->is_edit_weather==true) $AryTmp["weather"] = 1;
					if($cls[$i]->is_edit_warning==true) $AryTmp["warning"] = 1;

					$AryTmp["alert"] = 1;
				}
			}
		}
		return $AryTmp;
	}
	//���ɤˤĤ��Ƥ�������������ư�����Ǥ���Ģɼ���������ʼ�ư�Խ���������Ǥ���Ģɼ��������ɽ���˻��ѡ�
	function GetColSys($select=""){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($pnt["pnt_enable"]){
				if($select!="" && !$pnt["selected"])	continue;
				//�������������
				$logger = $this->GetLoggers($pnt["pnt_id"]);
				for($i=0;$i<count($logger);$i++){
					$clsTmp		= $logger[$i]["class"];	//���饹̾����
					$cls[$i]	= new $clsTmp($log);		//���饹���
					if(is_array($cls[$i]->ColSys)){
						foreach($cls[$i]->ColSys as $val) $AryTmp[$val] = $val;
					}
				}
			}
		}
		return $AryTmp;
	}

	//Ģɼ��˼�ư�����Ǥ���ɾ�������
	function GetColPoints($sys_no,$select=""){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($pnt["pnt_enable"]){
				$flg = 0;
				//�������������
				$logger = $this->GetLoggers($pnt["pnt_id"]);
				for($i=0;$i<count($logger);$i++){
					$clsTmp		= $logger[$i]["class"];	//���饹̾����
					$cls[$i]	= new $clsTmp($log);		//���饹���
					if(is_array($cls[$i]->ColSys)){
						if(in_array($sys_no,$cls[$i]->ColSys))	$flg = 1;
					}
				}
				if($flg) $AryTmp[] = $pnt;
			}
		}
		return $AryTmp;
	}

	//Ģɼ��˼�ư�Խ��Ǥ���ɾ�������
	function GetEditPoints($sys_no,$select=""){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($pnt["pnt_enable"]){
				$flg = 0;
				//�������������
				$logger = $this->GetLoggers($pnt["pnt_id"]);
				for($i=0;$i<count($logger);$i++){
					$clsTmp		= $logger[$i]["class"];	//���饹̾����
					$cls[$i]	= new $clsTmp($log);		//���饹���
					if(is_array($cls[$i]->EditSys)){
						if(in_array($sys_no,$cls[$i]->EditSys))	$flg = 1;
					}
				}
				if($flg) $AryTmp[] = $pnt;
			}
		}
		return $AryTmp;
	}

	//Ģɼ��˼�ư����Ǥ���ɾ�������
	function GetDelPoints($sys_no,$select=""){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($pnt["pnt_enable"]){
				$flg = 0;
				//�������������
				$logger = $this->GetLoggers($pnt["pnt_id"]);
				for($i=0;$i<count($logger);$i++){
					$clsTmp		= $logger[$i]["class"];	//���饹̾����
					$cls[$i]	= new $clsTmp($log);		//���饹���
					if(is_array($cls[$i]->DelSys)){
						if(in_array($sys_no,$cls[$i]->DelSys))	$flg = 1;
					}
				}
				if($flg) $AryTmp[] = $pnt;
			}
		}
		return $AryTmp;
	}

	//���ݶɥ���������Ѥ��Ƥ���ɾ�������
	function GetWeatherPoints($select="",$enable="enable"){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($enable=="enable" && $pnt["pnt_enable"]=="0") continue;
			$flg = 0;
			//�������������
			$logger = $this->GetLoggers($pnt["pnt_id"]);
			for($i=0;$i<count($logger);$i++){
				$clsTmp		= $logger[$i]["class"];	//���饹̾����
				$cls[$i]	= new $clsTmp($log);		//���饹���
				//�ɥ����פ����Υ��������Ĥ��Ƥ���ɤ����
				if($cls[$i]->type=="1")	$flg = 1;
			}
			if($flg) $AryTmp[] = $pnt;
		}
		return $AryTmp;
	}

	//�׺Ʒ׻�����������Ѥ��Ƥ���ɾ�������
	function GetReCalPoints($select=""){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($pnt["pnt_enable"]){
				$flg = 0;
				//�������������
				$logger = $this->GetLoggers($pnt["pnt_id"]);
				for($i=0;$i<count($logger);$i++){
					$clsTmp		= $logger[$i]["class"];	//���饹̾����
					$cls[$i]	= new $clsTmp($log);		//���饹���
					//�ɥ����פ����Υ��������Ĥ��Ƥ���ɤ����
					if($cls[$i]->ReCal)	$flg = 1;
				}
				if($flg) $AryTmp[] = $pnt;
			}
		}
		return $AryTmp;
	}

	//�׺Ʒ׻�����������Ѥ��Ƥ���ɾ�������(1ʬ�׻���ǽ�ʥ��������饹)
	function GetReCalPoints2($select=""){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($pnt["pnt_enable"]){
				$flg = 0;
				//�������������
				$logger = $this->GetLoggers($pnt["pnt_id"]);
				for($i=0;$i<count($logger);$i++){
					$clsTmp		= $logger[$i]["class"];	//���饹̾����
					$cls[$i]	= new $clsTmp($log);		//���饹���
					//�ɥ����פ����Υ��������Ĥ��Ƥ���ɤ����
					if($cls[$i]->ReCal&&$cls[$i]->Recal1minFlg)	$flg = 1;
				}
				if($flg) $AryTmp[] = $pnt;
			}
		}
		return $AryTmp;
	}

	//�߷ױ����Խ�����ǽ�ʶɾ�������
	function GetRainPoints($select="",$enable="enable"){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($enable=="enable" && $pnt["pnt_enable"]=="0") continue;
			$flg = 0;
			//�������������
			$logger = $this->GetLoggers($pnt["pnt_id"]);
			for($i=0;$i<count($logger);$i++){
				$clsTmp		= $logger[$i]["class"];	//���饹̾����
				$cls[$i]	= new $clsTmp($log);		//���饹���
				//�ɥ����פ����Υ��������Ĥ��Ƥ���ɤ����
				//if($cls[$i]->flg_edit_rainrui=="1")	$flg = 1;
				foreach($cls[$i]->OutField["000"] as $val){
					if(strpos(" ".$val,"rain_rui")>0) $flg = 1;
				}
			}
			if($flg) $AryTmp[] = $pnt;
		}
		return $AryTmp;
	}

	//�������Ѥ���ǽ�ʶɾ�������
	function GetSnowPoints($select="",$enable="enable"){
		$Point = $this->GetPoints($select);

		$AryTmp = array();
		foreach($Point as $pnt){
			if($enable=="enable" && $pnt["pnt_enable"]=="0") continue;
			$flg = 0;
			//�������������
			$logger = $this->GetLoggers($pnt["pnt_id"]);
			for($i=0;$i<count($logger);$i++){
				$clsTmp		= $logger[$i]["class"];	//���饹̾����
				$cls[$i]	= new $clsTmp($log);		//���饹���
				//�ɥ����פ����Υ��������Ĥ��Ƥ���ɤ����
				//if($cls[$i]->flg_edit_rainrui=="1")	$flg = 1;
				foreach($cls[$i]->OutField["000"] as $val){
					if(strpos(" ".$val,"snow")>0) $flg = 1;
				}
			}
			if($flg) $AryTmp[] = $pnt;
		}
		return $AryTmp;
	}



}
?>