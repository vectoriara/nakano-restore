<?
class LoggerCA100 extends BaseLogger
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2009.03.14";
	var $memo			= "";
	var $id				= "CA100";
	var $type			= "1";														//�����������ס�1 => ���ݥ�����	 2 => ���̥�����
	var $ReCal		= "1";														//�Ʒ׻���������
	var $map			= array("lgr_id"				=> "lgr_id",
		"lgr_pnt_id"		=> "lgr_pnt_id",
		"lgr_enable"		=> "lgr_enable",
		"lgr_time"			=> "lgr_time",
		"lgr_ip"				=> "lgr_col_1",
		"lgr_port"			=> "lgr_col_2"
	);
	var $EditSys	= array("100","200");							//�Խ���ǽ�ʥǡ���Ģɼ
	var $ColSys		= array("100","200");							//��ư������ǽ�ʥǡ���Ģɼ
	var $DelSys		= array("100","200");							//�����ǽ�ʥǡ���Ģɼ

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
	var $OutField = array(
		"000" => array(	"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
//																					"tion1",
//																					"tion2",
//																					"tion3",
			"humid1",
			"ins_day",
			"sun_day",
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
//																					"tion1",
//																					"tion2",
//																					"tion3",
			"humid1",
			"ins_day",
			"sun_day",
			"rain_10min",
			"rain_hour",
			"rain_day"
		),
		"200" => array(	"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
//																					"tion1",
//																					"tion2",
//																					"tion3",
			"humid1",
			"ins_day",
			"sun_day",
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
//																					"tion1",
//																					"tion2",
//																					"tion3",
			"avg_humid1",
			"max_humid1",
			"min_humid1",
			"ins_day",
			"sun_day",
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
//																					"tion1",
//																					"tion2",
//																					"tion3",
			"avg_humid1",
			"max_humid1",
			"min_humid1",
			"ins_day",
			"sun_day",
			"max_rain_10min",
			"max_rain_hour",
			"max_rain_day",
			"rain_month"
		)
	);

	//Ģɼ��Ǥ��Υ��������Խ��Ǥ�����ܤ����
	var $EditField = array(
		"100" => array(	"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
//																					"tion1",
//																					"tion2",
//																					"tion3",
			"humid1",
			"ins_day",
			"sun_day",
			"rain_10min",
			"rain_hour",
			"rain_day"
		),
		"200" => array(	"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
//																					"tion1",
//																					"tion2",
//																					"tion3",
			"humid1",
			"ins_day",
			"sun_day",
			"max_rain_10min",
			"rain_hour",
			"rain_day",
			"weather"
		)
	);


	//�̿������å�
	function CheckConnection($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);


		$cls = new PREFERENCE($this->log);
		if(!$cls->StartPreference($pnt_id,8)){
			if(is_object($this->log))	$this->log->logPrint("�����������̿����Ǥ��ޤ���Ǥ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
			return "�̿������ϤǤ��ޤ���Ǥ���";
		}

		$buf = $this->SendCmd("PT01".date("YmdHi",time()));

		$cls->EndPreference();
		$buf = $this->ClearBuf($buf);
		return $buf;
	}

	//�������ơ�����Ƚ��
	function GetStatus($val){
		$AryStatus	= array();
		$status 		= trim($val);
		for($i=0;$i<6;$i++){
			$tmp = substr($status,$i,1);
			$tmp = sprintf("%07d",decbin(hexdec($tmp)));
			if(is_object($this->log))	$this->log->logPrint("####GetMinutelyRain####sts[{$i}] {$tmp}",REC_DEBUG,__LINE__,__FILE__);
			for($j=0;$j<7;$j++){
				$AryStatus[$i][7-$j] = substr($tmp,$j,1);
			}
		}
		return $AryStatus;
	}

	//��������
	function SetDate($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){

			$cls = new PREFERENCE($this->log);
			if(!$cls->StartPreference($pnt_id,8)){
				if(is_object($this->log))	$this->log->logPrint("�����������̿������ϤǤ��ޤ���Ǥ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return false;
			}

			$curTime = time();
			$buf = $this->SendCmd("TT01".date("YmdHis",$curTime));

			$cls->EndPreference();

			//��̥����å�
			$buf = trim($buf);
			if(is_object($this->log))	$this->log->logPrint("{$buf}",REC_NOTE,__LINE__,__FILE__);
			if(substr($buf,1,2)=="TR"){
				if(is_object($this->log))	$this->log->logPrint("������������λ���ޤ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return 1;
			}else{
				if(is_object($this->log))	$this->log->logPrint("���������˼��Ԥ��ޤ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return false;
			}
		}else{
			if(is_object($this->log))	$this->log->logPrint("���������ϵ��Ĥ���Ƥ��ޤ���[{$this->id}]",REC_DEBUG,__LINE__,__FILE__);
			return 2;
		}
	}

	function GetRain1min($curTime,$move_rain_10min){
		$dbs = new CONN;

		if(is_numeric($move_rain_10min)){
			$strSql = "INSERT INTO t_rain VALUES('".date("Y-m-d H:i",$curTime)."',{$move_rain_10min});";
		}else{
			$strSql = "INSERT INTO t_rain VALUES('".date("Y-m-d H:i",$curTime)."',NULL);";
		}
		$dbs->Execute($strSql);

		$fld = "rain_001";
		//��ư�ѻ����̼���
		$strSql = "SELECT {$fld} FROM t_rain WHERE date = '".date("Y-m-d H:i",$curTime)."';";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
		$row	=	$dbs->Query($strSql);
		$rain = $row[0];
		if(is_object($this->log))	$this->log->logPrint("10ʬ�ְ�ư�ѻ����� => [{$rain}]",REC_DEBUG,__LINE__,__FILE__);

		if(!is_numeric($rain)){
			if(is_object($this->log))	$this->log->logPrint("10ʬ�ְ�ư�ѻ����̼������顼",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}

		if($rain>0){
			//��ư�ѻ�����0.0mm�򸡺�
			$strSql = "SELECT date FROM t_rain WHERE {$fld} = 0 ORDER BY date DESC LIMIT 1;";
			if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
			$row	= $dbs->Query($strSql);
			$time = strtotime($row["date"]);
			if($time>0){
				//���̣�mm�������鸽�ߤޤǤ����ǡ�������
				$strSql = "SELECT {$fld},date FROM t_rain WHERE date >= '".date("Y-m-d H:i",$time)."' ORDER BY date;";
				if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);

				$AryMoveRain = array();

				$row	= $dbs->Query($strSql);
				$time = strtotime($row["date"]);
				if(!($time>0)){
					//���¶�ǡ�����ͭ���ʥ쥳���ɤ�̵�����
					print "Get Time Error [t_rain Record Time Error]\n";
					if(is_object($this->log))	$this->log->logPrint("Get Time Error [t_rain Record Time Error]",REC_DEBUG,__LINE__,__FILE__);
					return false;
				}
				$chk_time = $time;
				while($row){
					$time = strtotime($row["date"]);
					if($time>0){
						//���������ǡ�����Ϣ³��������Ǥ��뤫�����å�
						if($chk_time!=$time){
							print "Connect Time Error\n";
							if(is_object($this->log))	$this->log->logPrint("Connect Time Error",REC_DEBUG,__LINE__,__FILE__);
							return false;
						}
						if(is_numeric($row[$fld])){
							$AryMoveRain[$time-60] = $row[$fld];
							$chk_time = $time + 60;
						}else{
							//���¶�ǡ�����ͭ���ʥǡ�����̵�����ʷ�¬�ʤɡ�
							print "Get MoveRain Error\n";
							if(is_object($this->log))	$this->log->logPrint("Get MoveRain Error",REC_DEBUG,__LINE__,__FILE__);
							return false;
						}
					}else{
						//���¶�ǡ�����ͭ���ʥ쥳���ɤ�̵�����
						print "Get Time Error [t_rain Record Time Error]\n";
						if(is_object($this->log))	$this->log->logPrint("Get Time Error [t_rain Record Time Error]",REC_DEBUG,__LINE__,__FILE__);
						return false;
					}
					$row = $dbs->Next();
				}
			}else{
				//���¶�ǡ�����ͭ���ʥ쥳���ɤ�̵�����
				print "Get Time Error [t_rain Record Time Error]\n";
				if(is_object($this->log))	$this->log->logPrint("Get Time Error [t_rain Record Time Error]",REC_DEBUG,__LINE__,__FILE__);
				return false;
			}

			$sum_rain_1min = "";

			$AryRain1min = array();
			foreach($AryMoveRain as $time => $val){
				if(is_object($this->log))	$this->log->logPrint("==== MoveRain10min[".date("Y/m/d H:i",$time)."][{$val}]",REC_DEBUG,__LINE__,__FILE__);

				$rain_sum = 0;
				for($i=1;$i<10;$i++){
					$trgTime = $time-$i*60;
					if(is_numeric($AryRain[ $trgTime ]["rain_1min"])){
						$rain_sum += $AryRain[ $trgTime ]["rain_1min"];
					}
				}

				if(!is_numeric($AryRain[ ($time-9*60) ]["rain_1min"])){
					//9ʬ�֤Υ쥳���ɤ�������ʤ���礽�λ������ѻ�����Ѥ���
					print "==== No Rain 1min[".date("Y/m/d H:i",($time-9*60))."]\n";
					if(is_object($this->log))	$this->log->logPrint("==== No Rain 1min[".date("Y/m/d H:i",($time-9*60))."]",REC_DEBUG,__LINE__,__FILE__);
					if(is_numeric($sum_rain_1min)){
						if($rain_sum==0){
							$rain_sum = $sum_rain_1min;
							if(is_object($this->log))	$this->log->logPrint("==== rain_sum <== sum_rain_1min[{$sum_rain_1min}]",REC_DEBUG,__LINE__,__FILE__);
						}
					}else{
						$rain_sum = 0;
					}
				}
				print "==== rain_sum => {$rain_sum}\n";
				if(is_object($this->log))	$this->log->logPrint("==== rain_sum => {$rain_sum}",REC_DEBUG,__LINE__,__FILE__);

				$rain_1min = "";
				if(is_numeric($AryMoveRain[$time])){
					$tmp_rain		= $AryMoveRain[$time] - $rain_sum;
					$rain_1min	= $tmp_rain<0 ? 0 : $tmp_rain;
					$AryRain[ $time ]["rain_1min"] = $rain_1min;
					if(!is_numeric($sum_rain_1min)){
						$sum_rain_1min = $rain_1min; //���ֺǽ�Υ쥳���ɤξ��Τ߽���
						if(is_object($this->log))	$this->log->logPrint("==== set sum_rain_1min[{$sum_rain_1min}]",REC_DEBUG,__LINE__,__FILE__);

					}
				}
				print "==== rain_1min[".date("Y/m/d H:i",$time)."] => {$rain_1min}\n";
				if(is_object($this->log))	$this->log->logPrint("==== rain_1min[".date("Y/m/d H:i",$time)."] => {$rain_1min}",REC_DEBUG,__LINE__,__FILE__);
			}
			$rain = $rain_1min;
		}else{
			print "Rain Data 0.0mm\n";
			if(is_object($this->log))	$this->log->logPrint("Rain Data 0.0mm",REC_DEBUG,__LINE__,__FILE__);
		}

		$strSql = "DELETE FROM t_rain WHERE date < '".date("Y-m-d H:i",$curTime-84600)."';";
		if(is_object($this->log))	$this->log->logPrint($strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs->Execute($strSql);
		return number_format($rain,1,".","");
	}

	//���ߥǡ�����������
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$cls = new PREFERENCE($this->log);
		if(!$cls->StartPreference($pnt_id)){
			if(is_object($this->log))	$this->log->logPrint("�̿����Ǥ��ޤ���Ǥ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		$buf = $this->SendCmd("PT01".date("YmdHi",$curTime));

		$cls->EndPreference();

		if(!$buf){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		$buf		= $this->ClearBuf($buf);
		$AryTmp =	$this->ClearBuf2($buf);

		$sts = $this->GetStatus($AryTmp[1]);
		if($sts[0][4]=="1"||$sts[2][3]=="1"||$sts[2][4]=="1"){
			return true;
		}

		//���ե����å�
		$chkDate = "PR01".date("YmdHi",$curTime);

		if($AryTmp[0]==$chkDate){
			if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	is_numeric($AryTmp[3])		? number_format($AryTmp[3]/10,	1, ".", "")  	: "";
			if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	is_numeric($AryTmp[2])		? sprintf("%02d",$AryTmp[2])									: "";
			if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	is_numeric($AryTmp[7])		? number_format($AryTmp[7]/10,	1, ".", "")  	: "";
			if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	is_numeric($AryTmp[8])		? sprintf("%02d",$AryTmp[8])									: "";
			if(array_key_exists("temp",$args))				$args["temp"]				=	is_numeric($AryTmp[13]) 	? number_format($AryTmp[13]/10,	1, ".", "") 	: "";
			if(array_key_exists("tion1",$args))				$args["tion1"]			=	is_numeric($AryTmp[14]) 	? number_format($AryTmp[14]/10,	1, ".", "") 	: "";
			if(array_key_exists("tion2",$args))				$args["tion2"]			=	is_numeric($AryTmp[15]) 	? number_format($AryTmp[15]/10,	1, ".", "") 	: "";
			if(array_key_exists("tion3",$args))				$args["tion3"]			=	is_numeric($AryTmp[16]) 	? number_format($AryTmp[16]/10,	1, ".", "") 	: "";
			if(array_key_exists("humid1",$args))			$args["humid1"]			=	is_numeric($AryTmp[12]) 	? number_format($AryTmp[12]/10,	1, ".", "") 	: "";
			if(array_key_exists("ins_day",$args))			$args["ins_day"]		=	is_numeric($AryTmp[10]) 	? number_format($AryTmp[10]/100,2, ".", "") 	: "";
			if(array_key_exists("sun_day",$args))			$args["sun_day"]		=	is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],		1, ".", "") 	: "";

			if(array_key_exists("move_rain_10min",$args)) $args["move_rain_10min"] = is_numeric($AryTmp[20]) ? number_format($AryTmp[20]/10,1,".","") : "";
			////////////////////////////////////////////////////////////////////////////////////////////
			if($rain_1min=$this->GetRain1min($curTime,$args["move_rain_10min"])){
				$args["rain_1min"] =	$rain_1min;
			}
			///////////////////////////////////////////////////////////////////////////////////////////
			//1ʬ�ѻ����Ȥ��ѻ��ǡ��������
			$rain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	is_numeric($rain["rain_10min"])	? number_format($rain["rain_10min"],1,".","") : "";
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	is_numeric($rain["rain_hour"])	? number_format($rain["rain_hour"],	1,".","") : "";
			if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	is_numeric($rain["rain_day"])		? number_format($rain["rain_day"],	1,".","")	: "";
			//��ư�ѻ�����
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);
			if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	= $AryMove["move_rain_10min"];
			if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		= $AryMove["move_rain_hour"];
			if(array_key_exists("move_rain_day",$args))		$args["move_rain_day"]		= $AryMove["move_rain_day"];

			//�߷ױ��̻��С���Ͽ����
			if(array_key_exists("rain_rui",$args))	$args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
			//ŷ������
			if(array_key_exists("weather",$args))		$args["weather"]	= $this->GetWeather($pnt_id);
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//����ǡ�����������
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$cls = new PREFERENCE($this->log);
		if($this->execute_collection){
			$Preference = 9;
		}else{
			$Preference = 1;
		}
		if(!$cls->StartPreference($pnt_id,$Preference)){
			if(is_object($this->log))	$this->log->logPrint("�̿����Ǥ��ޤ���Ǥ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		$buf = $this->SendCmd("IT01".date("YmdHi",$curTime),0);

		if($this->execute_collection){
			if($cls->CheckPreferenceLevel($pnt_id,$Preference)){
				$this->SendCmd();
			}
		}

		$cls->EndPreference();

		if(!$buf){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(!$this->execute_collection) $this->SendCmd();	//��ư�������ʳ��ϥ�������

		$buf		= $this->ClearBuf($buf);
		$AryTmp =	$this->ClearBuf2($buf);

		$sts = $this->GetStatus($AryTmp[1]);
		if($sts[0][4]=="1"||$sts[2][3]=="1"||$sts[2][4]=="1"){
			return true;
		}

		//���ե����å�
		$chkDate = "IR01".date("YmdHi",$curTime);
		if($AryTmp[0]==$chkDate){
			if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	is_numeric($AryTmp[3])		? number_format($AryTmp[3]/10,	1, ".", "")  	: "";
			if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	is_numeric($AryTmp[2])		? sprintf("%02d",$AryTmp[2])									: "";
			if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	is_numeric($AryTmp[7])		? number_format($AryTmp[7]/10,	1, ".", "")  	: "";
			if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	is_numeric($AryTmp[8])		? sprintf("%02d",$AryTmp[8])									: "";
			if(array_key_exists("temp",$args))				$args["temp"]				=	is_numeric($AryTmp[13]) 	? number_format($AryTmp[13]/10,	1, ".", "") 	: "";
			if(array_key_exists("tion1",$args))				$args["tion1"]			=	is_numeric($AryTmp[14]) 	? number_format($AryTmp[14]/10,	1, ".", "") 	: "";
			if(array_key_exists("tion2",$args))				$args["tion2"]			=	is_numeric($AryTmp[15]) 	? number_format($AryTmp[15]/10,	1, ".", "") 	: "";
			if(array_key_exists("tion3",$args))				$args["tion3"]			=	is_numeric($AryTmp[16]) 	? number_format($AryTmp[16]/10,	1, ".", "") 	: "";
			if(array_key_exists("humid1",$args))			$args["humid1"]			=	is_numeric($AryTmp[12]) 	? number_format($AryTmp[12]/10,	1, ".", "") 	: "";
			if(array_key_exists("ins_day",$args))			$args["ins_day"]		=	is_numeric($AryTmp[10]) 	? number_format($AryTmp[10]/100,2, ".", "") 	: "";
			if(array_key_exists("sun_day",$args))			$args["sun_day"]		=	is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],		1, ".", "") 	: "";
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	is_numeric($AryTmp[20]) 	? number_format($AryTmp[20]/10,	1, ".", "")  	: "";

			$Rain = $this->GetMinutelyRain($pnt_id,$curTime,$args["rain_10min"]);
			$args["rain_hour"]	=	$Rain["rain_hour"];
			$args["rain_day"]		=	$Rain["rain_day"];
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	function GetMinutelyRain($pnt_id,$curTime,$rain_10min){
		if(!is_numeric($rain_10min)){
			if(is_object($this->log))	$this->log->logPrint("####GetMinutelyRain#### No rain_10min",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}

		$AryRain = array();

		//0:10�λ�
		$date = DateConvert($curTime);
		$trg	= $date["hour"].$date["min"];
		if($trg=="0010"){
			$AryRain["rain_hour"]		= number_format($rain_10min,1,".","");
			$AryRain["rain_day"]		= number_format($rain_10min,1,".","");
			if(is_object($this->log))	$this->log->logPrint("####GetMinutelyRain#### No Proc Data 00:10",REC_DEBUG,__LINE__,__FILE__);
			return $AryRain;
		}

		$dbs 	= new CONN;
		$date = DateConvert($curTime);
		$tbl	= "t100_dat_".$date["year"];

		//��Ŷ���ϰ�ö�ǡ�������¸
		if($this->execute_collection){
			$date 	= DateConvert($curTime);
			$trg		= $date["date"];
			$strSql = "SELECT COUNT(*) FROM {$tbl} WHERE t100_date = '{$trg}';";
			if(is_object($this->log))	$this->log->logPrint("####GetMinutelyRain#### ==={$strSql}",REC_DEBUG,__LINE__,__FILE__);
			$row 		= $dbs->Query($strSql);
			if($row[0]>0){
				$strSql = "UPDATE {$tbl} SET rain_10min = '{$rain_10min}' WHERE t100_pnt_id = '{$pnt_id}' AND t100_date = '{$trg}';";
			}else{
				$strSql = "INSERT INTO {$tbl}(t100_pnt_id,t100_date,rain_10min) VALUES('{$pnt_id}','{$trg}','{$rain_10min}');";
			}
			if(is_object($this->log))	$this->log->logPrint("####GetMinutelyRain#### ==={$strSql}",REC_DEBUG,__LINE__,__FILE__);
			$dbs->Execute($strSql);
		}

		//1�����ѻ�����
		$date 	= DateConvert($curTime-600);
		$trg1		= $date["year"].$date["month"].$date["day"].$date["hour"]."10" ;
		$date 	= DateConvert($curTime-600);
		$trg2		= $date["date"];
		$where	= "t100_date BETWEEN '{$trg1}' AND '{$trg2}'";
		$strSql = "SELECT SUM(TO_NUMBER(rain_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND rain_10min <> '' AND TO_NUMBER(rain_10min,'9999.99') > 0 AND {$where};";
		if(is_object($this->log))	$this->log->logPrint("####GetRain#### ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row		=	$dbs->Query($strSql);
		$AryRain["rain_hour"] = number_format($rain_10min+$row[0],1,".","");

		//���ѻ�����
		$date 	= DateConvert($curTime-600);
		$trg1		= $date["year"].$date["month"].$date["day"]."0010" ;
		$date 	= DateConvert($curTime-600);
		$trg2		= $date["date"];
		$where	= "t100_date BETWEEN '{$trg1}' AND '{$trg2}'";
		$strSql = "SELECT SUM(TO_NUMBER(rain_10min,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND rain_10min <> '' AND TO_NUMBER(rain_10min,'9999.99') > 0 AND {$where};";
		if(is_object($this->log))	$this->log->logPrint("####GetRain#### ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row		=	$dbs->Query($strSql);
		$AryRain["rain_day"] = number_format($rain_10min+$row[0],1,".","");

		if(is_object($this->log))	$this->log->logPrint("####GetRain#### ".print_r($AryRain,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return $AryRain;
	}

	//����ǡ�����ư�����ʼ����ǡ�����Ϣ������˳�Ǽ����
	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		if(is_object($this->log))	$this->log->logPrint($this->id." GetLoggerMinutelyCollection Start",REC_DEBUG,__LINE__,__FILE__);

		$this->execute_collection = 1;	//��ư�����ե饰ON

		//��Ǽ��Ϣ����������
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
		$this->execute_collection = 0;	//��ư�����ե饰OFF
		return true;
	}

	//���󽸷ץǡ�������
	function GetDailySumData($pnt_id,$curTime,&$args){

		$date = DateConvert($curTime);
		$trg	= $date["year"].$date["month"].$date["day"];

		$cls = new PREFERENCE($this->log);
		if(!$cls->StartPreference($pnt_id)){
			if(is_object($this->log))	$this->log->logPrint("�̿����Ǥ��ޤ���Ǥ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
			return false;
		}

		$buf = $this->SendCmd("ST01".$trg."0000",0);

		$cls->EndPreference();

		if(!$buf){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(!$this->execute_collection) $this->SendCmd();	//��ư�������ʳ��ϥ�������

		$buf		= $this->ClearBuf($buf);
		$AryTmp	=	$this->ClearBuf2($buf);

		$sts = $this->GetStatus($AryTmp[1]);
		if($sts[0][4]=="1"||$sts[2][3]=="1"||$sts[2][4]=="1"){
			return true;
		}

		$dbs = new CONN;

		//ʿ����®
		$args["40"]["avg_ws"]	= is_numeric($AryTmp[2])	? number_format($AryTmp[2]/10,	1,".", "")						: "";
		$args["50"]["avg_ws"]	= is_numeric($AryTmp[3])	? number_format($AryTmp[3]/10,	1,".", "")						: "";
		$args["55"]["avg_ws"]	= $AryTmp[5]!=""					? substr($AryTmp[5],0,2).":".substr($AryTmp[5],2,2) 	: "";

		//ʿ������
		//=======================DB�������=============================
		$date = DateConvert($curTime);
		$tbl	= "t100_dat_".$date["year"];
		$trg1	= $date["year"].$date["month"].$date["day"]."0000";
		$trg2	= $date["year"].$date["month"].$date["day"].$date["hour"]."00";

		$strSql = "SELECT avg_wd FROM t100_dat_".$date["year"]." WHERE t100_pnt_id = '{$pnt_id}' AND t100_date BETWEEN '{$trg1}' AND '{$trg2}' AND avg_wd <> '' GROUP BY avg_wd ORDER BY COUNT(*) DESC LIMIT 1;";
		if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);
		if(is_numeric($row[0]))	$args["40"]["avg_wd"] = sprintf("%02d",$row[0]);
		//=======================DB�������=============================
		$args["50"]["avg_wd"]	= is_numeric($AryTmp[4])	? sprintf("%02d",$AryTmp[4]) 													: "";

		//����ִ���®
		$args["50"]["max_ws"]	= is_numeric($AryTmp[6])	? number_format($AryTmp[6]/10,	1,".", "")						: "";
		$args["55"]["max_ws"]	= $AryTmp[8]!=""					? substr($AryTmp[8],0,2).":".substr($AryTmp[8],2,2) 	: "";
		//����ִ�����
		$args["50"]["max_wd"]	= is_numeric($AryTmp[7])	? sprintf("%02d",$AryTmp[7]) 													: "";
		//����
		$args["40"]["temp"]		= is_numeric($AryTmp[16])	? number_format($AryTmp[16]/10,	1,".", "")						: "";
		$args["50"]["temp"]		= is_numeric($AryTmp[17])	? number_format($AryTmp[17]/10,	1,".", "")						: "";
		$args["55"]["temp"]		= $AryTmp[18]!=""					? substr($AryTmp[18],0,2).":".substr($AryTmp[18],2,2) : "";
		$args["60"]["temp"]		= is_numeric($AryTmp[19])	? number_format($AryTmp[19]/10,	1,".", "")						: "";
		$args["65"]["temp"]		= $AryTmp[20]!=""					? substr($AryTmp[20],0,2).":".substr($AryTmp[20],2,2) : "";
//			//�ϲ�1
//			$args["40"]["tion1"]	= is_numeric($AryTmp[21])	? number_format($AryTmp[21]/10,	1,".", "")						: "";
//			$args["50"]["tion1"]	= is_numeric($AryTmp[22])	? number_format($AryTmp[22]/10,	1,".", "")						: "";
//			$args["55"]["tion1"]	= $AryTmp[23]!=""					? substr($AryTmp[23],0,2).":".substr($AryTmp[23],2,2) : "";
//			$args["60"]["tion1"]	= is_numeric($AryTmp[24])	? number_format($AryTmp[24]/10,	1,".", "")						: "";
//			$args["65"]["tion1"]	= $AryTmp[25]!=""					? substr($AryTmp[25],0,2).":".substr($AryTmp[25],2,2) : "";
//			//�ϲ�2
//			$args["40"]["tion2"]	= is_numeric($AryTmp[26])	? number_format($AryTmp[26]/10,	1,".", "")						: "";
//			$args["50"]["tion2"]	= is_numeric($AryTmp[27])	? number_format($AryTmp[27]/10,	1,".", "")						: "";
//			$args["55"]["tion2"]	= $AryTmp[28]!=""					? substr($AryTmp[28],0,2).":".substr($AryTmp[28],2,2) : "";
//			$args["60"]["tion2"]	= is_numeric($AryTmp[29])	? number_format($AryTmp[29]/10,	1,".", "")						: "";
//			$args["65"]["tion2"]	= $AryTmp[30]!=""					? substr($AryTmp[30],0,2).":".substr($AryTmp[30],2,2) : "";
//			//�ϲ�3
//			$args["40"]["tion3"]	= is_numeric($AryTmp[31])	? number_format($AryTmp[31]/10,	1,".", "")						: "";
//			$args["50"]["tion3"]	= is_numeric($AryTmp[32])	? number_format($AryTmp[32]/10,	1,".", "")						: "";
//			$args["55"]["tion3"]	= $AryTmp[33]!=""					? substr($AryTmp[33],0,2).":".substr($AryTmp[33],2,2) : "";
//			$args["60"]["tion3"]	= is_numeric($AryTmp[34])	? number_format($AryTmp[34]/10,	1,".", "")						: "";
//			$args["65"]["tion3"]	= $AryTmp[35]!=""					? substr($AryTmp[35],0,2).":".substr($AryTmp[35],2,2) : "";
		//���м���
		$args["40"]["humid1"]	= is_numeric($AryTmp[11])	? number_format($AryTmp[11]/10,	1,".", "")					: "";
		$args["50"]["humid1"]	= is_numeric($AryTmp[12])	? number_format($AryTmp[12]/10,	1,".", "")					: "";
		$args["55"]["humid1"]	= $AryTmp[13]!=""					? substr($AryTmp[13],0,2).":".substr($AryTmp[13],2,2) : "";
		$args["60"]["humid1"]	= is_numeric($AryTmp[14])	? number_format($AryTmp[14]/10,	1,".", "")					: "";
		$args["65"]["humid1"]	= $AryTmp[15]!=""					? substr($AryTmp[15],0,2).":".substr($AryTmp[15],2,2) : "";

		//10ʬ���籫��
		$args["50"]["max_rain_10min"]	= is_numeric($AryTmp[52])					? number_format($AryTmp[52]/10,	1,".", "")						: "";
		$args["55"]["max_rain_10min"]	= $AryTmp[53]!=""&&$AryTmp[52]>0	? substr($AryTmp[53],0,2).":".substr($AryTmp[53],2,2) : "";

		//���ֱ���
		$args["50"]["rain_hour"]			= is_numeric($AryTmp[54])					? number_format($AryTmp[54]/10,	1,".", "")						: "";
		$args["55"]["rain_hour"]			= $AryTmp[55]!=""&&$AryTmp[54]>0	? substr($AryTmp[55],0,2).":".substr($AryTmp[55],2,2) : "";
		//���ѻ�����
		$args["70"]["rain_day"]				= is_numeric($AryTmp[51])					? number_format($AryTmp[51]/10,	1,".", "")						: "";


//			//=======================DB�������=============================
//			$date = DateConvert($curTime);
//			$tbl	= "t100_dat_".$date["year"];
//			$trg1	= $date["year"].$date["month"].$date["day"]."0000";
//			$trg2	= $date["year"].$date["month"].$date["day"].$date["hour"]."00";
//
//			$strSql = "SELECT MAX(TO_NUMBER(ins_day,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND t100_date BETWEEN '{$trg1}' AND '{$trg2}' AND t100_code IS NULL AND ins_day <> '';";
//			if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
//			$row = $dbs->Query($strSql);
//			if(is_numeric($row[0]))	$args["50"]["ins_day"] = number_format($row[0],2,".","");
//			$strSql = "SELECT t100_date FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND t100_date BETWEEN '{$trg1}' AND '{$trg2}' AND t100_code IS NULL AND ins_day = '".$row[0]."' ORDER BY t100_date LIMIT 1;";
//			$row = $dbs->Query($strSql);
//			if(is_numeric($row[0]))	$args["55"]["ins_day"] = substr($row[0],8,2).":".substr($row[0],10,2);
//
//			$strSql = "SELECT MAX(TO_NUMBER(sun_day,'9999.99')) FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND t100_date BETWEEN '{$trg1}' AND '{$trg2}' AND t100_code IS NULL AND sun_day <> '';";
//			if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
//			$row = $dbs->Query($strSql);
//			if(is_numeric($row[0]))	$args["50"]["sun_day"] = number_format($row[0],1,".","");
//			$strSql = "SELECT t100_date FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND t100_date BETWEEN '{$trg1}' AND '{$trg2}' AND t100_code IS NULL AND sun_day = '".$row[0]."' ORDER BY t100_date LIMIT 1;";
//			$row = $dbs->Query($strSql);
//			if(is_numeric($row[0]))	$args["55"]["sun_day"] = substr($row[0],8,2).":".substr($row[0],10,2);
//			//=======================DB�������=============================

		//���ѻ�����
		$args["70"]["ins_day"]				= is_numeric($AryTmp[9])					? number_format($AryTmp[9]/100,	2,".", "")	: "";
		//���ѻ�����
		$args["70"]["sun_day"]				= is_numeric($AryTmp[10])					? number_format($AryTmp[10],1,".", "")			: "";

		$args["90"]["avg_ws"]	= $this->GetDirection($pnt_id,"200",$curTime);
		return true;
	}

	//����ǡ�������(���̥����ס�
	function GetDailyData($pnt_id,$curTime,&$args){
		//������� (XX-1):01 ���� XX:00 �ޤǤλ���ǡ�������Ѥ�������ǡ��������
		$dbs = new CONN;

		//���̥ǡ�������
		if(in_array("max_rain_10min",$this->OutField["200"])||in_array("rain_hour",$this->OutField["200"])||in_array("rain_day",$this->OutField["200"])){
			$rain = $this->GetDailyRain($pnt_id,$curTime);
		}

		//����ִ���®�ǡ�������
		if(in_array("max_ws",$this->OutField["200"])||in_array("max_wd",$this->OutField["200"])){
			$wind = $this->GetDailyWind($pnt_id,$curTime);
		}

		$now		= DateConvert($curTime);
		$prev		= DateConvert($curTime-3600);
		$tbl		= "t100_dat_".$now["year"];
		$trg		= "t100_date BETWEEN '".$prev["year"].$prev["month"].$prev["day"].$prev["hour"]."01' AND '".$now["year"].$now["month"].$now["day"].$now["hour"]."00'";
		$strSql = "SELECT * FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND {$trg} ORDER BY t100_date DESC LIMIT 1;";

		if(is_object($this->log))	$this->log->logPrint("****GetDailyData**** ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);

		foreach($this->OutField["200"] as $key){
			if(!array_key_exists($key,$args)) continue;

			switch($key){
				case "ins_day":
					$date		= DateConvert($curTime-3600);
					if($date["hour"]=="24"){
						if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 2, ".", "");
					}else{
						$tbl		= "t100_dat_".$date["year"];
						$trg		= $date["date"];
						$strSql = "SELECT {$key} FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND t100_date ='{$trg}';";
						$dbs2		= new CONN;
						$row2 	= $dbs2->Query($strSql);
						if(is_numeriC($row[$key])&&is_numeriC($row2[0])){
							$args[$key] = number_format($row[$key]-$row2[0], 2, ".", "");
						}
					}
					break;
				case "sun_day":
					$date		= DateConvert($curTime-3600);
					if($date["hour"]=="24"){
						if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 1, ".", "");
					}else{
						$tbl		= "t100_dat_".$date["year"];
						$trg		= $date["date"];
						$strSql = "SELECT {$key} FROM {$tbl} WHERE t100_pnt_id = '{$pnt_id}' AND t100_date ='{$trg}';";
						$dbs2		= new CONN;
						$row2 	= $dbs2->Query($strSql);
						if(is_numeriC($row[$key])&&is_numeriC($row2[0])){
							$args[$key] = number_format($row[$key]-$row2[0], 1, ".", "");
						}
					}
					break;
				case "max_rain_10min":
				case "rain_hour":
				case "rain_day":
					$args[$key] = $rain[$key];
					break;
				case "max_ws":
				case "max_wd":
					$args[$key] = $wind[$key];
					break;
				case "weather":
					$args[$key]	=	$this->GetWeatherData($pnt_id,$curTime);
				case "avg_wd":
					if(is_numeric($row[$key]))	$args[$key] = $row[$key];
					break;
				default:
					if(is_numeric($row[$key]))	$args[$key] = number_format($row[$key], 1, ".", "");
			}
		}
		if(is_object($this->log))	$this->log->logPrint($this->id."****GetDailyData**** ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//���������������������ǡ����������Ϣ������˳�Ǽ
	function GetDailyLoggerData($pnt_id,$curTime){
		if($curTime > time()) return false;
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$date		= DateConvert($curTime);
		$trg1		= $date["year"].$date["month"].$date["day"]."0100";
		$trg2		= $date["year"].$date["month"].$date["day"]."2400";
		$time1	= DateConvert3($trg1);
		$time2	= DateConvert3($trg2);

		$data		= new Data;
		$AryRec	= $data->GetDataField("200");

		while($time1<=$time2){
			$date	= DateConvert($curTime);
			$args = $AryRec;
			$this->GetDailyData($pnt_id,$time1,$args);
			$this->AryColData[ $date["date"] ] = $args;
			$time1 += 3600;
		}

//			if(!$this->execute_collection){
//				$args = array();
//				$this->GetDailySumData($pnt_id,$curTime,$args);
//				$this->AryColSumData[ $date["year"].$date["month"].$date["day"] ] = $args;
//			}
		return true;
	}

	//��ʸ����
	function SendCmd($cmd="",$close=1){

		//�����ʤ��ǸƤӽФ������ե�����ϥ�ɥ륯������
		if($cmd==""&&$close){
			if(!$this->fp) return true;
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
			return true;
		}

		//�ե�����ϥ�ɥ뤬���˥����ץ󤵤�Ƥ��뤫�����å�
		if(!$this->fp){
			$ip		= $this->setting["lgr_ip"];
			$port =	$this->setting["lgr_port"];

			$this->OpenCheck();

			$this->fp = @stream_socket_client("tcp://{$ip}:{$port}", $errno, $errstr, 10);
			if($this->fp){
				@stream_set_timeout($this->fp, 10);
				if(is_object($this->log))	$this->log->logPrint("open:{$ip}:{$port}",																					REC_SOCK,__LINE__,__FILE__);
			}else{
				if(is_object($this->log))	$this->log->logPrint("open-error:IP={$ip}:{$port}:ERRNO={$errno}:ERRSTR={$errstr}",	REC_SOCK,__LINE__,__FILE__);
				$this->LastCloseTime = time();
				return false;
			}
		}

		//��ʸ����
		$cmd = $this->CreateCmd($cmd);
		if(is_object($this->log))	$this->log->logPrint("send:{$cmd}",REC_SOCK,__LINE__,__FILE__);
		fwrite($this->fp,$cmd);
		$flg = 1;
		$buf = "";
		while( (!feof($this->fp) && $flg)){
			$tmp = @fgetc($this->fp);

			//������ʸ����Ƭ��STX�ʳ��ʤ鶯����λ
			if($buf==""&&chr(2)!=$tmp){
				if(is_object($this->log))	$this->log->logPrint("STX Get Error",REC_SOCK,__LINE__,__FILE__);
				$flg = 0;
			}

			$buf .= $tmp;
			if(chr(3)==$tmp){
				$debug_ord .= "[".ord($tmp)."]";	//Debug
				$debug_chr .= "[".$tmp."]";				//Debug

				//ETX���ɤ߹������Ĥ��BCC���ɤ߹���ǽ�λ
				$tmp = @fgetc($this->fp);
				$debug_ord 	.= "[".ord($tmp)."]";		//Debug
				$debug_chr 	.= "[".$tmp."]";				//Debug
				$bcc_chk		 = $tmp;

				$flg = 0;
			}else{
				$debug_ord .= "[".ord($tmp)."]";		//Debug
				$debug_chr .= "[".$tmp."]";					//Debug
			}
		}
		if(is_object($this->log))	$this->log->logPrint("debug_ord:[{$debug_ord}]",REC_DEBUG,__LINE__,__FILE__);	//Debug
		if(is_object($this->log))	$this->log->logPrint("debug_chr:[{$debug_chr}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

		//BCC�����å�
		$bcc = $this->BCC($buf);
		if(is_object($this->log))	$this->log->logPrint("bcc_chk[{$bcc_chk}]==bcc[{$bcc}]",REC_DEBUG,__LINE__,__FILE__);	//Debug

		if($bcc==$bcc_chk){
			if(is_object($this->log))	$this->log->logPrint("recv:[{$buf}] BCC[{$bcc_chk}]",REC_SOCK,__LINE__,__FILE__);
		}else{
			if(is_object($this->log))	$this->log->logPrint("BCC Check Error bcc_chk[{$bcc_chk}]==bcc[{$bcc}]",REC_SOCK,__LINE__,__FILE__);
			$buf = "";
		}

		if($close){
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
		}
		return $buf;
	}

	//��ʸ����
	function CreateCmd($cmd){
		$strCmd = chr(02).$cmd.chr(03).$this->BCC(chr(02).$cmd.chr(03));
		return $strCmd;
	}
	function BCC($str){
		$bcc = ord(substr($str, 1, 1));
		for ($i = 2; $i < strlen($str); $i++) {
			$ch = substr($str, $i, 1);
			$bcc = $bcc ^ ord($ch);
		}
		return sprintf("%c", $bcc);
	}

}
?>