<?
//============================================================//
//	RCL80���������饹																					//
//------------------------------------------------------------//
//	��������																									//
//	��RCL80��																									//
//------------------------------------------------------------//
//	�ѹ�����																									//
//		2006/11/11	N.ARAI																			//
//		����������																							//
//============================================================//
class LoggerRCL80 extends BaseLogger
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2007.01.31";
	var $memo			= "";
	var $id				= "RCL80" ;
	var $type			= "2";														//�����������ס�1 => ���ݥ�����	 2 => ���̥�����
	var $ReCal		= "1";														//�Ʒ׻���������
	var $map			= array("lgr_id"				=> "lgr_id",
		"lgr_pnt_id"		=> "lgr_pnt_id",
		"lgr_enable"		=> "lgr_enable",
		"lgr_time"			=> "lgr_time",
		"lgr_ip"				=> "lgr_col_1",
		"lgr_port"			=> "lgr_col_2",
	);
	var $EditSys	= array("100");										//�Խ���ǽ�ʥǡ���Ģɼ
	var $ColSys		= array("100");										//��ư������ǽ�ʥǡ���Ģɼ
	var $DelSys		= array("100");										//�����ǽ�ʥǡ���Ģɼ

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
	var $OutField = array(
		"000" => array(	"rain_10min",
			"rain_hour",
			"rain_day",
			"move_rain_10min",
			"move_rain_hour",
			"move_rain_day",
			"rain_1min",
			"rain_rui"
		),
		"100" => array(	"rain_10min",
			"rain_hour",
			"rain_day"
		),
		"200" => array(	"max_rain_10min",
			"rain_hour",
			"rain_day",
		),
		"300" => array(	"max_rain_10min",
			"max_rain_hour",
			"rain_day"
		),
		"400" => array( "max_rain_10min",
			"max_rain_hour",
			"max_rain_day",
			"rain_month"
		)
	);


	//Ģɼ��Ǥ��Υ��������Խ��Ǥ�����ܤ����
	var $EditField = array(
		"100" => array(	"rain_10min",
			"rain_day"
		)
	);

	//�̿������å� [2006/12/31]
	function CheckConnection($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);
		$buf = $this->SendCmd(chr(02)."D00000000000000".chr(03));
		$buf = $this->ClearBuf($buf);
		return $buf;
	}

	//��������	[2006/12/31]
	function SetDate($pnt_id){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if($this->setting["lgr_time"]){
			$buf = $this->SendCmd(chr(02)."T".date("YmdHis").chr(03));
			if($buf==chr(6)){
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

	//���ߥǡ����������� [2007/01/31]
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		if(!$buf=$this->SendCmd(chr(02)."D00000000000000".chr(03))){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if($buf==chr(4)||$buf==chr(21)||$buf==chr(6)) return false;
		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//���ե����å�
		$chkDate = date("H:i",$curTime);
		if($AryTmp[1]==$chkDate){
			if($this->OutCheck("rain_day",				"000",$args))	$args["rain_day"]					=	(is_numeric($AryTmp[4]) 	? number_format($AryTmp[4],1, ".", "")  : "");
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);					//��ư�ѻ������ؿ����1ʬ�ѻ���24���ְ�ư�ѻ�������ʤ���¾�ΰ�ư�ͤϥ���������Ѥ����
			if($this->OutCheck("rain_1min",				"000",$args))	$args["rain_1min"]				= $AryMove["rain_1min"];
			if($this->OutCheck("move_rain_day",		"000",$args))	$args["move_rain_day"]		= $AryMove["move_rain_day"];
			$AryRain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);	//�ѻ��ǡ�������
			if($this->OutCheck("rain_10min",			"000",$args))	$args["rain_10min"]				= $AryRain["rain_10min"];
			if($this->OutCheck("rain_hour",				"000",$args))	$args["rain_hour"]				= $AryRain["rain_hour"];
			if($this->OutCheck("move_rain_10min",	"000",$args))	$args["move_rain_10min"]	= (is_numeric($AryTmp[2]) 	? number_format($AryTmp[2],1, ".", "")  : "");
			if($this->OutCheck("move_rain_hour",	"000",$args))	$args["move_rain_hour"]		= (is_numeric($AryTmp[3]) 	? number_format($AryTmp[3],1, ".", "")  : "");
			//if($this->OutCheck("rain_rui",				"000",$args))	$args["rain_rui"]					= (is_numeric($AryTmp[5]) 	? number_format($AryTmp[5],1, ".", "")  : "");
			//�߷ױ��̻��С���Ͽ����
			if($this->OutCheck("rain_rui",				"000",$args))	$args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
			//$this->SetRuiRain($pnt_id,$curTime,$args["rain_rui"]);
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//����ǡ����������� [2007/01/31]
	function GetMinutelyData($pnt_id,$curTime,&$args){
		if($curTime > time()) return false;
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$date = DateConvert($curTime);
		$cmd	= chr(02)."A".$date["year"].$date["month"].$date["day"].$date["hour"].$date["min"]."00".chr(03);
		if(!$buf=$this->SendCmd($cmd,0)){
			if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyData [SendCmd] return false",REC_DEBUG,__LINE__,__FILE__);
			return false;
		}
		if(!$this->execute_collection) $this->SendCmd();	//��ư�������ʳ��ϥ�������
		if($buf==chr(4)||$buf==chr(21)||$buf==chr(6)) return false;

		$buf = $this->ClearBuf($buf);

		$AryTmp = explode(",",$buf);

		//���ե����å�
		$chkDate = $date["hour"].":".$date["min"];
		if($AryTmp[1]==$chkDate){
			if($this->OutCheck("rain_10min","100",$args))	$args["rain_10min"]	= (is_numeric($AryTmp[2]) 	? number_format($AryTmp[2],1, ".", "")  : "");
			if($this->OutCheck("rain_hour",	"100",$args))	$args["rain_hour"]	= (is_numeric($AryTmp[3]) 	? number_format($AryTmp[3],1, ".", "")  : "");
			if($this->OutCheck("rain_day",	"100",$args))	$args["rain_day"]		=	(is_numeric($AryTmp[4]) 	? number_format($AryTmp[4],1, ".", "")  : "");
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
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

			$this->fp = @stream_socket_client("tcp://{$ip}:{$port}", $errno, $errstr, 5);
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
			if(is_object($this->log))	$this->log->logPrint("close",REC_SOCK,__LINE__,__FILE__);
			@fclose($this->fp);
			$this->fp = false;
			$this->LastCloseTime = time();
		}
		return $buf;
	}

}
?>