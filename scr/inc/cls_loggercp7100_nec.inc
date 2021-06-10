<?
//============================================================//
//	CP-7100���������饹																				//
//------------------------------------------------------------//
//	��������																									//
//	��CP-7100��																								//
//------------------------------------------------------------//
//	�ѹ�����																									//
//		2006/11/11	N.ARAI																			//
//		����������																							//
//============================================================//
class LoggerCP7100_NEC extends LoggerCP7100
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2006.12.12";
	var $memo			= "NEC�Ʒ���CP7100������";
	var $id				= "CP7100_NEC";

	//���ߥǡ�����������
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$buf = $this->SendCmd("D00000000000000");
		$buf = str_replace("\n",	"", $buf);
		$buf = str_replace(chr(4),"", $buf);
		$buf = trim($buf);

		$AryTmp = explode(",",$buf);

		//���ե����å�
		$chkDate = date("Ymd,H:i",$curTime);
		if(strpos(" ".$buf,$chkDate)>0){
			if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp[2])		? number_format($AryTmp[2],	1, ".", "")  : "");
			if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp[3])		? $AryTmp[3] : "");
			if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	(is_numeric($AryTmp[4])		? number_format($AryTmp[5],	1, ".", "")  : "");
			if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	(is_numeric($AryTmp[5])		? $AryTmp[5] : "");
			if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp[6]) 	? number_format($AryTmp[6],	1, ".", "")  : "");
			if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp[7]) 	? number_format($AryTmp[7], 1, ".", "")  : "");
			if(array_key_exists("humid2",$args))			$args["humid2"]			=	(is_numeric($AryTmp[8]) 	? number_format($AryTmp[8], 1, ".", "")  : "");
			if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp[9]) 	? number_format($AryTmp[9],	1, ".", "")  : "");
			if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],1, ".", "")  : "");
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	=	(is_numeric($AryTmp[11]) 	? number_format($AryTmp[11],1, ".", "")  : "");
			//if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	=	(is_numeric($AryTmp[12]) 	? number_format($AryTmp[12],1, ".", "")  : "");
			//NEC�Ʒ�Ǥϻ����ѻ�����ʬ�����ѻ������������ѻ��Ϸ׻��ˤ�äƻ��Ф���
			if(array_key_exists("rain_day",$args))		$args["rain_day"]		=	(is_numeric($AryTmp[12]) 	? number_format($AryTmp[12],1, ".", "")  : "");
			//��ư�ѻ�����
			$AryMove = $this->GetMoveRain($pnt_id,$curTime,$args);
			if(array_key_exists("rain_1min",$args))				$args["rain_1min"]				= $AryMove["rain_1min"];
			if(array_key_exists("move_rain_10min",$args))	$args["move_rain_10min"]	= $AryMove["move_rain_10min"];
			if(array_key_exists("move_rain_hour",$args))	$args["move_rain_hour"]		= $AryMove["move_rain_hour"];
			if(array_key_exists("move_rain_day",$args))		$args["move_rain_day"]		= $AryMove["move_rain_day"];

			//�ѻ��ǡ�������
			$AryRain = $this->GetRain($pnt_id,$curTime,$args["rain_1min"]);
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	= $AryRain["rain_hour"];

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

		$date = DateConvert($curTime);
		$buf = $this->SendCmd("A".$date["year"].$date["month"].$date["day"].$date["hour"].$date["min"]."00");
		$buf = str_replace("\n",	"", $buf);
		$buf = str_replace(chr(4),"", $buf);
		$buf = trim($buf);

		$AryTmp = explode(",",$buf);

		//���ե����å�
		$chkDate = $date["hour"].":".$date["min"];
		if($AryTmp[0]==$chkDate){
			if(array_key_exists("avg_ws",$args))			$args["avg_ws"]			=	(is_numeric($AryTmp[2])		? number_format($AryTmp[2],	1, ".", "")  : "");
			if(array_key_exists("avg_wd",$args))			$args["avg_wd"]			=	(is_numeric($AryTmp[3])		? $AryTmp[3] : "");
			if(array_key_exists("max_ws",$args))			$args["max_ws"]			=	(is_numeric($AryTmp[4])		? number_format($AryTmp[5],	1, ".", "")  : "");
			if(array_key_exists("max_wd",$args))			$args["max_wd"] 		=	(is_numeric($AryTmp[5])		? $AryTmp[5] : "");
			if(array_key_exists("temp",$args))				$args["temp"]				=	(is_numeric($AryTmp[6]) 	? number_format($AryTmp[6],	1, ".", "")  : "");
			if(array_key_exists("humid1",$args))			$args["humid1"]			=	(is_numeric($AryTmp[7]) 	? number_format($AryTmp[7], 1, ".", "")  : "");
			if(array_key_exists("humid2",$args))			$args["humid2"]			=	(is_numeric($AryTmp[8]) 	? number_format($AryTmp[8], 1, ".", "")  : "");
			if(array_key_exists("press1",$args))			$args["press1"]			=	(is_numeric($AryTmp[9]) 	? number_format($AryTmp[9],	1, ".", "")  : "");
			if(array_key_exists("press2",$args))			$args["press2"]			=	(is_numeric($AryTmp[10]) 	? number_format($AryTmp[10],1, ".", "")  : "");

			//�ѻ��ǡ�������
			$AryRain = $this->GetRain($pnt_id,$curTime,$AryTmp[11]);
			if(array_key_exists("rain_10min",$args))	$args["rain_10min"]	= $AryRain["rain_10min"];
			if(array_key_exists("rain_hour",$args))		$args["rain_hour"]	= $AryRain["rain_hour"];
			if(array_key_exists("rain_day",$args))		$args["rain_day"]		= $AryRain["rain_day"];
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetMinutelyData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}


}
?>