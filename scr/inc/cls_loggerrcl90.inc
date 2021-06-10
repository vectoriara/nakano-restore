<?
//==========================================================//
//	RCL80���������饹										//
//----------------------------------------------------------//
//	��������												//
//	��RCL80��												//
//----------------------------------------------------------//
//	�ѹ�����												//
//		2006/11/11	N.ARAI									//
//		����������											//
//==========================================================//
class LoggerRCL90 extends BaseLogger
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2014.09.18";
	var $memo		= "";
	var $id			= "RCL90" ;
	var $type		= "2";	//�����������ס�1 => ���ݥ�����	 2 => ���̥�����
	var $ReCal		= "1";	//�Ʒ׻���������
	var $map		= array(
		"lgr_id"		=> "lgr_id",
		"lgr_pnt_id"	=> "lgr_pnt_id",
		"lgr_enable"	=> "lgr_enable",
		"lgr_time"		=> "lgr_time",
		"lgr_ip"		=> "lgr_col_1",
		"lgr_port"		=> "lgr_col_2"
	);
	var $EditSys	= array("100");	//�Խ���ǽ�ʥǡ���Ģɼ
	var $ColSys		= array("100");	//��ư������ǽ�ʥǡ���Ģɼ
	var $DelSys		= array("100");	//�����ǽ�ʥǡ���Ģɼ

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
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

	//Ģɼ��Ǥ��Υ��������Խ��Ǥ�����ܤ����
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

	//�̿������å� [2006/12/31]
	function CheckConnection($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		$buf = $this->SendCmd(chr(02)."D00000000000000".chr(03));
		$buf = $this->ClearBuf($buf);
		return $buf;
	}

	//��������	[2006/12/31]
	function SetDate($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$buf = $this->SendCmd(chr(02)."T".date("YmdHis").chr(03));
			if($buf==chr(6)){
				if(is_object($this->log)) $this->log->logPrint("������������λ���ޤ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return 1;
			}else{
				if(is_object($this->log)) $this->log->logPrint("���������˼��Ԥ��ޤ���[{$this->id}]",REC_NOTE,__LINE__,__FILE__);
				return false;
			}
		}else{
			if(is_object($this->log)) $this->log->logPrint("���������ϵ��Ĥ���Ƥ��ޤ���[{$this->id}]",REC_DEBUG,__LINE__,__FILE__);
			return 2;
		}
	}

	//���ߥǡ����������� [2007/01/31]
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd(chr(02)."D00000000000000".chr(03))){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if($buf==chr(4)||$buf==chr(21)||$buf==chr(6)) return false;
		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//���ե����å�
		$chkDate = date("H:i",$curTime);
		if(is_object($this->log)) $this->log->logPrint(print_r($AryTmp,true),REC_SOCK,__LINE__,__FILE__);
		if(is_object($this->log)) $this->log->logPrint($chkDate,REC_SOCK,__LINE__,__FILE__);
		if($AryTmp[1]==$chkDate){
			if($this->OutCheck("rain_day","000",$args))			$args["rain_day"] = (is_numeric($AryTmp[4]) ? number_format($AryTmp[4],1, ".", "") : "");
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args); //��ư�ѻ������ؿ����1ʬ�ѻ���24���ְ�ư�ѻ�������ʤ���¾�ΰ�ư�ͤϥ���������Ѥ����
			if($this->OutCheck("rain_1min","000",$args))		$args["rain_1min"] = $AryMove["rain_1min"];
			if($this->OutCheck("move_rain_day","000",$args))	$args["move_rain_day"] = $AryMove["move_rain_day"];
			$AryRain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);	//�ѻ��ǡ�������
			if($this->OutCheck("rain_10min","000",$args))		$args["rain_10min"] = $AryRain["rain_10min"];
			if($this->OutCheck("rain_hour","000",$args))		$args["rain_hour"] = $AryRain["rain_hour"];
			if($this->OutCheck("move_rain_10min","000",$args))	$args["move_rain_10min"] = (is_numeric($AryTmp[2]) ? number_format($AryTmp[2],1, ".", "") : "");
			if($this->OutCheck("move_rain_hour","000",$args))	$args["move_rain_hour"] = (is_numeric($AryTmp[3]) ? number_format($AryTmp[3],1, ".", "") : "");
			if($this->OutCheck("rain_rui","000",$args))			$args["rain_rui"] = (is_numeric($AryTmp[5]) ? number_format($AryTmp[5],1, ".", "") : "");
			//�߷ױ��̻��С���Ͽ����
			#if($this->OutCheck("rain_rui","000",$args)) $args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
			//$this->SetRuiRain($pnt_id,$curTime,$args["rain_rui"]);
		}else{
			if(is_object($this->log)) $this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log)) $this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//����ǡ����������� [2007/01/31]
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if($curTime>time()) return false;
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);

		$date = DateConvert($curTime);
		$cmd = chr(02)."B".$date["year"].$date["month"].$date["day"].$date["hour"].$date["min"]."00".chr(03);
		if(!$buf=$this->SendCmd($cmd,0)){
			if(is_object($this->log)) $this->log->logPrint($this->id." GetMinutelyData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(!$this->execute_collection) $this->SendCmd();	//��ư�������ʳ��ϥ�������
		if($buf==chr(4)||$buf==chr(21)||$buf==chr(6)) return false;

		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//���ե����å�
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
				//��ư�������ʳ��ξ��Τ߽�����»ܤ���
				//��ư������¹Ԥ��Ƥ��ޤ�����ʸ���0.0����ǧ���줿���˹߱���ǡ����˿��꽪���ե饰��Ω�äƤ��ޤ���
				$rain_rui_sdt = date("Y/m/d H:i",strtotime($AryTmp[10]));
				$rain_rui_time = date("Y/m/d H:i",strtotime($AryTmp[0].$AryTmp[11]));
				if(is_object($this->log))	$this->log->logPrint("////updateT500RainRui//// ".$rain_rui_sdt." : ".$AryTmp[10],REC_DEBUG,__LINE__,__FILE__);
				$this->updateT500RainRui($pnt_id, $AryTmp[9], $rain_rui_sdt, $rain_rui_time);
			}

			// �������߷ױ��̥ꥻ�å������ͤ�Ͽ
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
	 * �������������10ʬ�߷ױ��̤˴ؤ���ǡ�����������ޤ���
	 * �ߤ�Ϥ�ˤʤ�ȥǡ����򿷵���Ͽ�����ߤ����ϥǡ����򹹿���³���ޤ���
	 * ���̤�0�ˤʤä����Ϲߤ꽪���Ȥߤʤ��졢�ߤ꽪�����������Ԥ��ޤ���
	 *
	 * memo:
	 *      �⤦����ä�Ŭ�ڤ�����̾��������Х�͡��ष�Ʋ������ġ�
	 *
	 * @param $pnt_id ���ֹ�Ǥ�
	 * @param $rain_rui �߷ױ��̤Ǥ�(��������������ͤ�RS)
	 * @param $rain_rui_sdt �ߤ�Ϥ���֤Ǥ�(��������������ͤ�FT)
	 * @param $rain_rui_time �ߤ�³���Ƥ�����֤Ǥ����ߤ꽪�����֤ǤϤʤ��������դ��Ƥ���������(��������������ͤ�ET)
	 */
	private function updateT500RainRui($pnt_id,$rain_rui,$rain_rui_sdt,$rain_rui_time){
		$dbs = new CONN;

		$strSql = "SELECT * FROM t500_dat WHERE t500_pnt_id = '{$pnt_id}' AND t500_sdt = '{$rain_rui_sdt}';";
		if(is_object($this->log))	$this->log->logPrint("////updateT500RainRui//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$row = $dbs->Query($strSql);

		if(is_object($this->log))	$this->log->logPrint("////updateT500RainRui//// ".print_r($row, true),REC_DEBUG,__LINE__,__FILE__);

		if($row){
			#�߷ױ��̥��������
			if(isset($row["t500_value"]) && $rain_rui > $row["t500_value"]){
				$strSql = "UPDATE t500_dat SET t500_value = {$rain_rui},t500_last = '{$rain_rui_time}' WHERE t500_pnt_id = '{$pnt_id}' AND t500_sdt = '{$rain_rui_sdt}';";
			}
		}else if($rain_rui > 0){
			#�ߤ�Ϥ�
			$strSql = "INSERT INTO t500_dat VALUES(DEFAULT,'{$pnt_id}','{$rain_rui_sdt}',NULL,'{$rain_rui_sdt}',{$rain_rui});";
		}else{
			#�ߤ꽪������
			$strSql = "UPDATE t500_dat SET t500_edt = CURRENT_TIMESTAMP(0) WHERE t500_pnt_id = '{$pnt_id}' AND t500_edt IS NULL;";
		}

		if(is_object($this->log))	$this->log->logPrint("////updateT500RainRui//// ".$strSql,REC_DEBUG,__LINE__,__FILE__);
		$dbs->Execute($strSql);
	}

	//����ǡ�����ư�����ʼ����ǡ�����Ϣ������˳�Ǽ���� [2007/01/31]
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

	//��ʸ���� [2007/01/31]
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

		//��ʸ���� ���ƥ��饹�Ǽ�ˤ�����ʬ����������ǽ������
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

		//������������
		if($close){
			if(is_object($this->log)) $this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
		}
		return $buf;
	}

	// �߷ױ��̥ꥻ�åȻ��֤�����������ꤹ��
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

	// ���̥������Υ����ꥻ�å�
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

	// ��baselogger���������פ��б���ȼ����GetDailySumData_dummy��̾���ѹ��ʺ���оݡˡ�2019/02/23
	// �Ȥ��������̹��ܤν����ʤΤ�¾�ε��ݹ��ܤ�����˾ä��ʤ��ǥ��ԥڤ��Ƥ��ϼ����б�
	public function GetDailySumData_dummy($pnt_id,$curTime,&$args){
		//��������
		$date = DateConvert($curTime);
		$trg1	= $date["year"].$date["month"].$date["day"]."0000";
		$trg2	= $date["year"].$date["month"].$date["day"].$date["hour"].$date["min"];

		$dbs = new CONN;

		$ArySumFld = array(
			"avg_ws			[ʿ����®]ʿ����" 					=> array("40","avg_ws",					"100","avg_ws"),
			"avg_ws			[ʿ����®]�ǹ���" 					=> array("50","avg_ws_max",			"100","avg_ws"),
			//"avg_ws			[ʿ����®]�ǹ��͵���" 			=> array("55","avg_ws",					"100","avg_ws"),
			"avg_wd			[ʿ������]ʿ����"						=> array("40","avg_wd",					"100","avg_wd"),
			"avg_wd			[ʿ������]�ǹ���"						=> array("50","avg_wd_max",			"100","avg_wd"),
			"max_ws			[������®]�ǹ���" 					=> array("50","max_ws",					"100","max_ws"),
			//"max_ws			[������®]�ǹ��͵���" 			=> array("55","max_ws",					"100","max_ws"),
			//"max_wd			[��������]�ǹ���"						=> array("50","max_wd",					"100","max_wd"),

			"press1			[���ϵ���]ʿ��" 						=> array("40","press1",					"100","press1"),
			"press1			[���ϵ���]�ǹ���" 					=> array("50","press1_max",			"100","press1"),
			//"press1			[���ϵ���]�ǹ��͵���" 			=> array("55","press1",					"100","press1"),
			"press1			[���ϵ���]������" 					=> array("60","press1_min",			"100","press1"),
			//"press1			[���ϵ���]�����͵���"				=> array("65","press1",					"100","press1"),

			"max_rain_10min [10ʬ����]�ǹ���"				=> array("50","rain_10min_max",			"100","max_rain_10min"),
			#"rain_10min_max_time [10ʬ����]�ǹ��͵���"		=> array("55","rain_10min_max_time",			"100","max_rain_10min"),
////													"rain_hour   [�����ѻ�]�ǹ���"					=> array("50","rain_hour",			"200","rain_hour"),
////													"rain_hour   [�����ѻ�]�ǹ��͵���" 			=> array("55","rain_hour",			"200","rain_hour"),
			"move_rain_hour   [�����ѻ�]�ǹ���"			=> array("50","move_rain_hour_max",	"100","rain_hour"),
			#"move_rain_hour_max_time   [�����ѻ�]�ǹ��͵���"	=> array("55","move_rain_hour_max_time",	"200","max_move_rain_hour_time"),
			"rain_day	   [���ѻ�]�ѻ���"						=> array("70","rain_day",				"100","rain_day"),

			"��������"								 							=> array("90","avg_ws",					"200","avg_ws")
		);

		foreach($ArySumFld as $name => $val){
			$sum_no = $val[0];	//����No
			$fld		= $val[1];	//��礻�ѥե������
			$sys_no	= $val[2];	//��礻�ѥơ��֥�
			$fld_id	= $val[3];	//�����ե������

			//��礻SQL����
			$tbl				= "t{$sys_no}_dat_".$date["year"];
			$fld_date		=	"t{$sys_no}_date";
			$strWhere 	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code IS NULL";
			$strWhere2	= "t{$sys_no}_pnt_id = '{$pnt_id}' AND t{$sys_no}_date BETWEEN '{$trg1}' AND '{$trg2}' AND t{$sys_no}_code = {$sum_no}";

			//���������Ϲ��ܥ����å�

			if(!in_array($fld_id,$this->OutField["200"]))	continue;
			if(!array_key_exists($fld_id,$args[$sum_no])) continue;

			//���ץǡ�����������
			if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== {$name}",REC_DEBUG,__LINE__,__FILE__);
			switch($sum_no){
				case "40":
					//ʿ��
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
					//�ǹ���
					if($fld=="avg_ws_max"){	//ʿ����®������(CK4700�ü����)
						$strSql = "SELECT * FROM {$tbl} WHERE {$strWhere} ORDER BY avg_ws_max DESC,t100_date DESC LIMIT 1;";
						if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
						$row = $dbs->Query($strSql);
						if(is_numeric($row["avg_ws_max"])){
							$args["50"][$fld_id] = $row["avg_ws_max"];
							$args["55"][$fld_id] = $row["avg_ws_max_time"];
						}
					}elseif($fld=="avg_wd_max"){//ʿ����®������(CK4700�ü����)
						//������®������
						$tmp_fld_id =	str_replace("_wd","_ws",$fld_id);
						if(is_numeric($args[$sum_no][$tmp_fld_id])){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND {$tmp_fld_id} = '".$args[$sum_no][$tmp_fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
							if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = sprintf("%02d",(int) $row[0]);
						}
					}elseif($fld=="max_ws"||$fld=="press1_max"||$fld=="rain_10min_max"||$fld=="move_rain_hour_max"){	//ʿ����®������(CK4700�ü����)
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
									if(is_object($this->log))	$this->log->logPrint("���̥ǡ�����[{$fld_id}]",REC_DEBUG,__LINE__,__FILE__);
									$args["55"][$fld_id] = "";
								}
							}
						}
					}elseif(strpos(" ".$fld_id,"_wd")>0){
						//������®������
						$tmp_fld_id =	str_replace("_wd","_ws",$fld_id);
						if(is_numeric($args[$sum_no][$tmp_fld_id])){
							$strSql = "SELECT {$fld} FROM {$tbl} WHERE {$strWhere} AND $tmp_fld_id = '".$args[$sum][$tmp_fld_id]."' ORDER BY {$fld_date} DESC LIMIT 1;";
							if(is_object($this->log))	$this->log->logPrint("====GetDailySumData==== ".$strSql,REC_DEBUG,__LINE__,__FILE__);
							$row = $dbs->Query($strSql);
							if(is_numeric($row[0]))	$args[$sum_no][$fld_id] = sprintf("%02d",(int) $row[0]);
						}
					}else{
						//�ǹ��ͻ���
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
					//������
					if($fld=="press1_min"){	//ʿ����®������(CK4700�ü����)
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
					//����
					$sum = $sum_no - 5;
					if(is_numeric($args[$sum][$fld_id])){
						if(strpos(" ".$fld_id,"rain")>0){
							if($args[$sum][$fld_id]==0){
								if(is_object($this->log))	$this->log->logPrint("���̥ǡ����� ".$strSql,REC_DEBUG,__LINE__,__FILE__);
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
//								//�����ʳ����ͤ����ξ��ϵ��������ˤ���
//								if(strpos(" ".$fld_id,"temp")<1 && strpos(" ".$fld_id,"water_")<1)	$args[$sum_no][$fld_id] = "";
//							}
//							//���̥ǡ��������ϵ��������ˤ���
//							if(strpos(" ".$fld_id,"rain")>0){
//								if(!is_numeric($args[$sum_no][$fld_id]))	$args[$sum_no][$fld_id] = "";
//								if($args[$sum_no][$fld_id]==0)						$args[$sum_no][$fld_id] = "";
//							}
					}
					break;
				case "70":
					//�ѻ���
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