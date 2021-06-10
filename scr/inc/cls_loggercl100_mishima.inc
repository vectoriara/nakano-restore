<?
	//========================================================//
	//	RS0001���������饹									   //
	//--------------------------------------------------------//
	//	��������											   //
	//		�����ϥ������ѽ������饹							//
	//--------------------------------------------------------//
	//	�ѹ�����											   //
	//		2015/11/01	N.ARAI								  //
	//		����������										   //
	//========================================================//
class LoggerCL100_MISHIMA extends LoggerCL100
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2020.10.01";
	var $memo		= "";
	var $id			= "CL100_MISHIMA" ;

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
	var $OutField = array(
							"000" => array(
											"avg_wd",
											"avg_ws",
											"max_ws",
											"max_wd",
											"temp",
											"humid1",
											"rain_hour",
											"rain_day",
											"move_rain_10min",
											"move_rain_hour",
											"rain_rui",
											"avg__temp",
											"max__temp",
											"min__temp",
											"snow_level",
											"mainte"
											),
							"100" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"temp",
											"rain_10min",
											"move_rain_10min",
											"max__move_rain_10min",
											"max__move_rain_10min_time",
											"rain_hour",
											"move_rain_hour",
											"max__move_rain_hour",
											"max__move_rain_hour_time",
											"rain_day",
											"max__avg_ws",
											"max__avg_ws_time",
											"max__avg_wd",
											"max__max_ws_time",
											"max__temp",
											"max__temp_time",
											"min__temp",
											"min__temp_time",
											"humid1",
											"max__humid1",
											"max__humid1_time",
											"min__humid1",
											"min__humid1_time",
											"snow_level",
											"mainte"
											),
							"200" => array(
											"avg_ws",
											"avg_wd",
											"max_ws",
											"max_wd",
											"temp",
											"humid1",
											"max__humid1",
											"max__humid1_time",
											"min__humid1",
											"min__humid1_time",
											"move_rain_10min",
											"max__move_rain_10min",
											"rain_hour",
											"rain_day",
											"snow_level",
											"mainte"
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
											"max__move_rain_10min",
											"max__move_rain_hour",
											"rain_day",
											"snow_level"
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
											"max__move_rain_10min",
											"max__move_rain_hour",
											"max_rain_day",
											"rain_month",
											"snow_level"
											)
							);

						//Ģɼ��Ǥ��Υ��������Խ��Ǥ�����ܤ����
						var $EditField = array(
												"100" => array(
													"avg_ws",
													"max__avg_ws",
													"avg_wd",
													"max__avg_wd",
													"max__avg_ws_time",
													"max_ws",
													"max__max_ws_time",
													"max_wd",
													"temp",
													"max__temp",
													"max__temp_time",
													"min__temp",
													"min__temp_time",
													"rain_10min",
													"max__move_rain_10min",
													"max__move_rain_10min_time",
													"rain_hour",
													"move_rain_hour",
													"max__move_rain_hour",
													"max__move_rain_hour_time",
													"rain_day",
													"humid1",
													"max__humid1",
													"max__humid1_time",
													"min__humid1_time",
													"min__humid1",
													"min__humid1_time",
													"snow_level"
													)
												);

	//����ǡ�������(���̥����ס�
	function GetDailyData($pnt_id,$curTime,&$args){
		//������� (XX-1):01 ���� XX:00 �ޤǤλ���ǡ�������Ѥ�������ǡ��������
		$dbs = new CONN;

		$now		= DateConvert($curTime);
		$prev		= DateConvert($curTime-3600);
		$trg		= "t100_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["year"].$now["month"].$now["day"].$now["hour"]."00'";

		//10ʬ���籫��
		$rain = array();
		$strSql = "SELECT TO_NUMBER(max__move_rain_10min,'9999.9') as max_rain_10min FROM t100_dat_".$now["year"]." WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} ORDER BY rain_10min DESC,t100_date DESC LIMIT 1;";
		if(is_object($this->log))	$this->log->logPrint("////".__FUNCTION__."//// [".$strSql."]",REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		if(is_numeric($row[0]))	$rain["max_rain_10min"] = number_format($row[0], 1,".","");

		//����ִ���®�ǡ�������
		$wind = array();
		$strSql = "SELECT TO_NUMBER(max_ws,'9999.9') as max_ws,max_wd FROM t100_dat_".$now["year"]." WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} ORDER BY max_ws DESC,t100_date DESC LIMIT 1;";
		if(is_object($this->log))	$this->log->logPrint("////".__FUNCTION__."//// [".$strSql."]",REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		if(is_numeric($row["max_ws"])){
			$wind["max_ws"] = number_format($row["max_ws"], 1,".","");
			$wind["max_wd"] = $row["max_wd"];
		}

		$strSql = "SELECT * FROM t100_dat_".$now["year"]." WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} ORDER BY t100_date DESC LIMIT 1;";
		if(is_object($this->log))	$this->log->logPrint("////".__FUNCTION__."//// [".$strSql."]",REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);

		foreach($this->OutField["200"] as $key){
			if(!array_key_exists($key,$args)) continue;

			switch($key){
				case "max_ws":
				case "max_wd":
					$args[$key] = $wind[$key];
					break;
				case "max_rain_10min":
					$args[$key] = $rain[$key];
					break;
				case "water_level":
				case "max__water":
				case "min__water":
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 2, ".", "");
					break;
				case "snow_level":
				case "snow_hour":
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 0, ".", "");
					break;
				default:
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 1, ".", "");
			}
		}
		if(is_object($this->log))	$this->log->logPrint($this->id."////".__FUNCTION__."//// ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//���󽸷ץǡ�������	[2006/12/29]
	function GetDailySumData($pnt_id,$curTime,&$args){
		//��������
		$date = DateConvert($curTime);
		$trg1	= $date["year"].$date["month"].$date["day"]."0000";
		$trg2	= $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];

		$dbs = new CONN;

		$ArySumFld = array(
			"avg_ws			[ʿ����®]ʿ����" 		=> array("40","avg_ws","100","avg_ws"),
			"avg_wd			[ʿ������]ʿ����"		=> array("40","avg_wd","100","avg_wd"),
			"avg_ws			[ʿ����®]�ǹ���" 		=> array("50","max__avg_ws","100","avg_ws"),
			"avg_ws			[ʿ����®]�ǹ��͵���" 	=> array("55","max__avg_ws","100","avg_ws"),
			"avg_wd			[ʿ������]�ǹ���"		=> array("50","max__avg_wd","100","avg_wd"),
			"max_ws			[������®]�ǹ���" 		=> array("50","max_ws","100","max_ws"),
			"max_ws			[������®]�ǹ��͵���" 	=> array("55","max_ws","100","max_ws"),
			"max_wd			[��������]�ǹ���"		=> array("50","max_wd","100","max_wd"),
			"temp			[����]ʿ��" 				=> array("40","temp","100","temp"),
			"temp  			[����]�ǹ���" 			=> array("50","max__temp","100","temp"),
			"temp  			[����]�ǹ��͵���"		=> array("55","max__temp","100","temp"),
			"temp  			[����]������" 			=> array("60","min__temp","100","temp"),
			"temp  			[����]�����͵���"		=> array("65","min__temp","100","temp"),
			"humid1			[���ϵ���]ʿ��" 			=> array("40","humid1","100","humid1"),
			"humid1			[���ϵ���]�ǹ���" 		=> array("50","max__humid1","100","humid1"),
			"humid1			[���ϵ���]�ǹ��͵���" 	=> array("55","max__humid1","100","humid1"),
			"humid1			[���ϵ���]������" 		=> array("60","min__humid1","100","humid1"),
			"humid1			[���ϵ���]�����͵���"		=> array("65","min__humid1","100","humid1"),
			"press1			[���ϵ���]ʿ��" 			=> array("40","press1","100","press1"),
			"press1			[���ϵ���]�ǹ���" 		=> array("50","max__press1","100","press1"),
			"press1			[���ϵ���]�ǹ��͵���" 	=> array("55","max__press1","100","press1"),
			"press1			[���ϵ���]������" 		=> array("60","min__press1","100","press1"),
			"press1			[���ϵ���]�����͵���"		=> array("65","min__press1","100","press1"),
			"max_rain_10min [10ʬ����]�ǹ���"		=> array("50","max__move_rain_10min",	"100","max_rain_10min"),
			"max_rain_10min [10ʬ����]�ǹ��͵���"	=> array("55","max__move_rain_10min",	"100","max_rain_10min"),
			"rain_hour [�����ѻ�]�ǹ���"				=> array("50","max__move_rain_hour",	"100","rain_hour"),
			"rain_hour [�����ѻ�]�ǹ��͵���" 			=> array("55","max__move_rain_hour",	"100","rain_hour"),
			"rain_day [���ѻ�]�ѻ���"				=> array("70","rain_day",				"100","rain_day"),

			"snow_level [���㿼]�ǹ���"				=> array("50","snow_level",	"100","snow_level"),
			"snow_level [���㿼]�ǹ��͵���"			=> array("55","snow_level",	"100","snow_level"),
			"snow_hour [���ֹ�����]�ǹ���"			=> array("50","snow_hour",	"100","snow_hour"),
			"snow_hour [���ֹ�����]�ǹ��͵���" 		=> array("55","snow_hour",	"100","snow_hour"),
			"snow_hour [���ֹ�����]�ѻ���"			=> array("70","snow_hour",	"200","snow_hour"),

			"water_level	[�����]ʿ��" 		=> array("40","water_level","100","water_level"),
			"water_level	[�����]�ǹ���" 		=> array("50","max__water","100","max__water"),
			"water_level	[�����]�ǹ��͵���" 	=> array("55","max__water","100","max__water"),
			"water_level	[�����]������" 		=> array("60","min__water","100","min__water"),
			"water_level	[�����]�����͵���"	=> array("65","min__water","100","min__water"),

			"��������"							=> array("90","avg_ws",					"200","avg_ws")
		);

		foreach($ArySumFld as $name => $val){
			$sum_no = $val[0];	//����No
			$fld	= $val[1];	//��礻�ѥե������
			$sys_no	= $val[2];	//��礻�ѥơ��֥�
			$fld_id	= $val[3];	//�����ե������
//			print $name."\n";
//			print $sum_no." ";
//			print $fld." ";
//			print $sys_no." ";
//			print $fld_id."\n";

			//��礻SQL����
			$tbl		= "t{$sys_no}_dat_".$date["year"];
			$fld_date	= "t{$sys_no}_date";
			$strWhere 	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code IS NULL ";

			//���������Ϲ��ܥ����å�
			if(!in_array($fld_id,$this->OutField["200"]))	continue;
			if(!array_key_exists($fld_id,$args[$sum_no]))	continue;

			//���ץǡ�����������
			if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== {$name}",REC_DEBUG,__LINE__,__FILE__);
			switch($sum_no){
				case "40"://ʿ����
					if(strpos(" ".$fld_id,"_wd")>0){
						//ʿ���͡�������
						$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' GROUP BY {$fld} ORDER BY COUNT(*) DESC LIMIT 2;";
						if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if($row[0]=="00"){
							$new = $dbs->Next();
							if($new[0]!=""){
								$row = $new;
								if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== Wind Dir [00] Next Record Read ==> [".$row[0]."]",REC_DEBUG,__LINE__,__FILE__);
							}
						}
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = $row[0];
					}else{
						//ʿ���͡�������
						$strSql = "SELECT AVG(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(strpos(" ".$fld_id,"water")>0){
							if(is_numeric($row[0])) $args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}else{
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					break;
				case "50":
					//�ǹ���
					if($fld_id=="avg_wd"||$fld_id=="max_wd"){
						$tmp_fld = str_replace("wd","ws",$fld);
						$strSql = "SELECT {$fld},TO_NUMBER($tmp_fld,'9999.99') as {$tmp_fld} FROM {$tbl} WHERE {$strWhere} AND $fld <> '' ORDER BY {$tmp_fld} DESC,t{$sys_no}_date DESC LIMIT 1;";
						if(is_object($this->log)) $this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row[0])){
							$args[$sum_no][$fld_id] = $row[0];
						}
					}else{
						$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
						if(is_object($this->log)) $this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(strpos(" ".$fld_id,"water")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
						}elseif(strpos(" ".$fld_id,"snow")>0){
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 0, ".", "");
						}else{
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
						}
					}
					break;
				case "60":
					//������
					$strSql = "SELECT MIN(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					if(is_object($this->log)) $this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					if(strpos(" ".$fld_id,"water")>0){
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 2, ".", "");
					}else{
						if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = number_format($row[0], 1, ".", "");
					}
					break;
				case "55":
				case "65":
					//����
					$sum = $sum_no - 5;
					if(is_numeric($args[$sum][$fld_id])){
//						if(strpos(" ".$fld_id,"rain")>0){
//							if($args[$sum][$fld_id]==0){
//								if(is_object($this->log)) $this->log->logPrint("���̥ǡ����� ".$strSql,REC_DEBUG,__LINE__,__FILE__);
//								break;
//							}
//						}
						if($fld=="snow_level"||$fld=="snow_hour"){
							$fld_date = "t100_date";
							$strSql = "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY t{$sys_no}_date DESC LIMIT 1;";
							if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							$date = DateConvert2($row[$fld_date]);
							$row[$fld_date] = $date["hour"].":".$date["min"];
						}else{
							$fld_date = $fld."_time";
							if($fld=="max_ws") $fld_date = "max__".$fld_date;//̵�����
							$strSql = "SELECT {$fld_date} FROM {$tbl} WHERE {$strWhere} AND {$fld} <> '' AND TO_NUMBER($fld,'9999.99') = '".$args[$sum][$fld_id]."' ORDER BY t{$sys_no}_date DESC LIMIT 1;";;
							if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
						}

						//$date = DateConvert2($row[$fld_date]);
						//$args[$sum_no][$fld_id] = $date["hour"].":".$date["min"];
						$args[$sum_no][$fld_id] = $row[$fld_date];
					}
					break;
				case "70":
					//�ѻ���
					$strSql = "SELECT MAX(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					if($fld=="snow_hour"){
						$strWhere 	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
						$strSql = "SELECT SUM(TO_NUMBER($fld,'9999.99')) FROM {$tbl} WHERE {$strWhere} AND $fld <> '';";
					}
					if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
					$row = $dbs->Query($strSql);
					$args[$sum_no][$fld_id] = $row[0];
					break;
				case "90":
					$args[$sum_no][$fld_id] = $this->GetDirection($pnt_id,"200",$curTime);
					break;
			}
			if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== [{$sum_no}][{$fld_id}] =>".$args[$sum_no][$fld_id],REC_DEBUG,__LINE__,__FILE__);
		}
		if(is_object($this->log))	$this->log->logPrint("====".__FUNCTION__."==== ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}
}
?>
