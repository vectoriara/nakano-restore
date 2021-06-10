<?
//============================================================//
//	����������������饹																			//
//------------------------------------------------------------//
//	��������																									//
//	���Ⲭ��ɹ������																					//
//------------------------------------------------------------//
//	�ѹ�����																									//
//		2013/10/01	N.ARAI																			//
//		����������																							//
//============================================================//
class LoggerREMOTE_TYPE0 extends BaseLogger
{
	//�ѿ����
	var $varsion	= "1.0";
	var $update		= "2013.10.01";
	var $memo			= "";
	var $id				= "REMOTE_TYPE0";
	var $type			= "1";														//�����������ס�1 => ���ݥ�����	 2 => ���̥�����
	var $ReCal		= "0";														//�Ʒ׻���������
	var $map			= array("lgr_id"				=> "lgr_id",
		"lgr_pnt_id"		=> "lgr_pnt_id",
		"lgr_enable"		=> "lgr_enable",
		"lgr_time"			=> "lgr_time",
		"lgr_ip"				=> "lgr_col_1",
		"lgr_pid"				=> "lgr_col_2"
	);
	var $EditSys	= array();												//�Խ���ǽ�ʥǡ���Ģɼ
	var $ColSys		= array("100","200","300","400");	//��ư������ǽ�ʥǡ���Ģɼ
	var $DelSys		= array("100","200","300","400");	//�����ǽ�ʥǡ���Ģɼ

	var $flg_edit_rainrui = true;//�߷ױ����Խ�

	var $SkipSave = array("100" => true,"200" => true,"300" => true,"400" => true);

	var $SumFieldIdx = array(
		"40" => 0,
		"50" => 1,
		"55" => 2,
		"60" => 3,
		"65" => 4,
		"70" => 5,
		"80" => 6
	);

	//Ģɼ��Ǥ��Υ����������ϤǤ�����ܤ����
	var $OutField = array(
		"000" => array(	"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2",
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
			"humid1",
			"humid2",
			"press1",
			"press2",
			"rain_10min",
			"rain_hour",
			"rain_day"
		),
		"200" => array(	"avg_ws",
			"avg_wd",
			"max_ws",
			"max_wd",
			"temp",
			"humid1",
			"humid2",
			"press1",
			"press2",
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
			"avg_humid1",
			"max_humid1",
			"min_humid1",
			"humid2",
			"press1",
			"avg_press1",
			"max_press1",
			"min_press1",
			"press2",
			"avg_press2",
			"max_press2",
			"min_press2",
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
			"avg_humid1",
			"max_humid1",
			"min_humid1",
			"humid2",
			"avg_humid2",
			"max_humid2",
			"min_humid2",
			"press1",
			"avg_press1",
			"max_press1",
			"min_press1",
			"press2",
			"avg_press2",
			"max_press2",
			"min_press2",
			"max_rain_10min",
			"max_rain_hour",
			"max_rain_day",
			"rain_month"
		)
	);

	//Ģɼ��Ǥ��Υ��������Խ��Ǥ�����ܤ����
	var $EditField = array();

	function GetDirIdx($val){
		global $dir;
		$idx = array_search(trim($val),$dir);
		if($idx===false){
			return "";
		}
		return sprintf("%02d",$idx);
	}
	function GetWeatherIdx($val){
		global $weather;
		$idx = array_search(trim($val),$weather);
		if($idx===false){
			return "";
		}
		return sprintf("%02d",$idx);
	}

	//�̿������å�
	function CheckConnection($pnt_id){
		if(!$this->load_flg) $this->LoadLoggerInfo($pnt_id);
		$date = DateConvert(time()-60);
		$url = "http://".$this->setting["lgr_ip"]."/present_xml.php?pid=".$this->setting["lgr_pid"]."&trg=".$date["date"];
		$buf = @file($url);
		if(trim($buf)=="") return false;
		$buf = join("",$buf);
		return $buf;
	}
	//��������
	function SetDate($pnt_id){
		if(is_object($this->log))	$this->log->logPrint("���������ϵ��Ĥ���Ƥ��ޤ���[{$this->id}]",REC_DEBUG,__LINE__,__FILE__);
		return 2;
	}

	function SearchProcField($key1,$sys_no,$args){
		if(!is_array($this->OutField[$sys_no])) return false;

		$flg = false;
		foreach($this->OutField[$sys_no] as $key){
			if(array_key_exists($key,$args)){
				$flg = true;
				break;
			}
		}
		return $flg;
	}

	//���ߥǡ����������
	function SetPresentData(&$args,$AryIN){
		foreach($this->OutField["000"] as $key){
			if(!array_key_exists($key,$args)) continue;
			$args[$key] = $AryIN[$key];
		}
	}
	//���ߥǡ�����������
	function GetPresentData($pnt_id,$curTime,&$args){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		$date	= DateConvert($curTime);
		$url	= "http://".$this->setting["lgr_ip"]."/present_xml.php?pid=".$this->setting["lgr_pid"]."&trg=".$date["date"];
		$buf 	= @file($url);
		if(trim($buf)==""){
			if(is_object($this->log))	$this->log->logPrint("Error[Data Not Access][$url]",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		$buf		= join("",$buf);
		$AryTmp = explode(",",$buf);

		$AryBuf = array();
		$flg = false;
		for($i=2;$i<count($AryTmp);$i++){
			$tmp = explode(":",$AryTmp[$i]);
			$AryBuf[trim($tmp[0])] = trim($tmp[1]);
			if(is_numeric(trim($tmp[1])))	$flg = true;
		}
		if(!$flg){
			if(is_object($this->log))	$this->log->logPrint("Error[No Data][$buf]",REC_SOCK,__LINE__,__FILE__);
			return false;
		}

		//���ե����å�
		$chkDate = date("Y/m/d H:i",$curTime);
		if($AryTmp[0]." ".$AryTmp[1]==$chkDate){
			$this->SetPresentData($args,$AryBuf);
			//�߷ױ��̻��С���Ͽ����
			if($this->SearchProcField("rain_rui","000",$args))	$args["rain_rui"] = $this->GetRuiRain($pnt_id,$curTime,$args["rain_1min"]);
////				ŷ������
////				if(array_key_exists("weather",$args)) $args["weather"] = $this->GetWeather($pnt_id);
////				if(array_key_exists("weather",$args)) $args["weather"] = $AryBuf["weather"];
		}else{
			if(is_object($this->log))	$this->log->logPrint("Date Check Error Terget Date => {$chkDate}",REC_SOCK,__LINE__,__FILE__);
			return false;
		}
		if(is_object($this->log))	$this->log->logPrint($this->id." GetPresentData ".print_r($args,TRUE),REC_DEBUG,__LINE__,__FILE__);
		return true;
	}

	//����ǡ�����������
	function GetMinutelyData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime-3600);
		$trg1	= $date["year"].$date["month"].$date["day"]."0010";
		$date	= DateConvert($curTime);
		$trg2	= $date["year"].$date["month"].$date["day"]."2400";
		$this->GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2);

		$data = new Data($this->log);
		foreach($this->AryColData as $key => $val){
			$time = DateConvert3($key);
			$data->SaveData($pnt_id,"100",$time,$val,"",false);
		}
		return true;
	}

	//����ǡ����������
	function SetMinutelyData(&$args,$AryIN){
		foreach($this->OutFieldIdx["100"] as $key => $idx){
			if(!array_key_exists($key,$args)) continue;
			if($key=="avg_wd"||$key=="max_wd"){
				$args[$key] = $this->GetDirIdx($AryIN[$idx]);
			}else{
				$args[$key] = $AryIN[$idx];
			}
		}
	}

	//����ǡ�����ư�����ʼ����ǡ�����Ϣ������˳�Ǽ���� [2009/02/10]
	function GetLoggerMinutelyCollection($pnt_id,$trg1,$trg2=""){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		//�ǡ��������ե���������
		$data = new Data($log);
		$rec	= $data->GetDataField("100");

		if(is_object($this->log))	$this->log->logPrint("GetLoggerMinutelyCollection [{$trg1}] => [{$trg2}]",REC_SOCK,__LINE__,__FILE__);

		//��Ǽ��Ϣ����������
		$this->AryColData = array();

		if($trg2!=""){
			$time1 = DateConvert3($trg1);
			$time2 = DateConvert3($trg2);
		}else{
			$time1 = DateConvert3($trg1);
			$time2 = $time1;
		}

		$t1 = $time1;
		$t2 = $time2;
		while($t1<=$t2){
			if($wk!=date("Ymd",$t1)){
				$date	= DateConvert($t1);
				$url	= "http://".$this->setting["lgr_ip"]."/minutely_xml.php?pnt=".$this->setting["lgr_pid"]."&trg=".$date["date"]."&kind=csv";
				$buf 	= @file($url);
				if($buf===false){
					if(is_object($this->log))	$this->log->logPrint("Error[Data Not Access][$url]",REC_SOCK,__LINE__,__FILE__);
				}else{
					$buf 	= join("",$buf);
				}
				if(trim($buf)!=""){
					$buf	= mb_convert_encoding($buf,"EUC-JP","UTF-8");
					$line	= explode("\n",$buf);
					$line = array_slice($line,2);

					foreach($line as $value){
						$tmp		= explode(",",$value);
						$buf		= $this->ClearBuf($value);
						$AryTmp	=	$this->ClearBuf2($buf);

						$args = $rec;
						$this->SetMinutelyData($args,$AryTmp);

						$this->AryColData[ date("Ymd",$t1).str_replace(":","",$AryTmp[0]) ] = $args;
					}
				}else{
					if(is_object($this->log))	$this->log->logPrint("Error[No Data][$buf]",REC_SOCK,__LINE__,__FILE__);
				}
				$wk = date("Ymd",$t1);
			}
			$t1 += 600;
			if($t1>time()) break;
		}
		return true;
	}

	//����ǡ�����������
	function GetDailyData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime-7200);
		$trg1	= $date["year"].$date["month"].$date["day"]."0010";
		$date	= DateConvert($curTime);
		$trg2	= $date["year"].$date["month"].$date["day"]."2400";
		$this->GetLoggerDailyCollection($pnt_id,$trg1,$trg2);

		$args = $this->AryColData[$date["date"]];

		$data = new Data($this->log);
		foreach($this->AryColData as $key => $val){
			$time = DateConvert3($key);
			$data->SaveData($pnt_id,"200",$time,$val,"",false);
		}
		return true;
	}
	//���󽸷ץǡ�����������
	function GetDailySumData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime-7200);
		$trg1	= $date["year"].$date["month"].$date["day"];
		$date	= DateConvert($curTime);
		$trg2	= $date["year"].$date["month"].$date["day"];
		if(!(is_array($this->AryColSumData[$trg1])&&is_array($this->AryColSumData[$trg2]))){
			$this->GetLoggerDailyCollection($pnt_id,$trg1."0010",$trg2."2400");
		}

		$args = $this->AryColData[$date["year"].$date["month"].$date["day"]];

		$data = new Data($this->log);
		foreach($this->AryColSumData as $key => $val){
			$d2		= DateConvert2($key);
			$trg2	= $d2["year"].$d2["month"].$d2["day"];
			foreach($val as $sum_no => $rec){
				$data->SaveData($pnt_id,"200",strtotime($d2["year"]."/".$d2["month"]."/".$d2["day"]." 00:01"),$rec,$sum_no,true);
			}
		}
		return true;
	}
	//����ǡ�����ư������Ϣ������˳�Ǽ�����ǡ����������
	function GetDailyCollectionData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime);
		$trg	= $date["date"];
		foreach($args as $fld => $val){
			$args[$fld] = $this->AryColData[$trg][$fld];
		}
		return true;
	}
	//���󽸷ץǡ�����ư������Ϣ������˳�Ǽ�����ǡ����������
	function GetDailyCollectionSumData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime);
		$trg	= $date["year"].$date["month"].$date["day"];
		foreach($args as $sum_no => $rec){
			foreach($rec as $fld => $val){
				$args[$sum_no][$fld] = $this->AryColSumData[$trg][$sum_no][$fld];
			}
		}
		return true;
	}

	var $OutFieldIdx = array(
		"100" => array(
			"avg_ws"			=> 1,
			"avg_wd"		 	=> 2,
			"max_ws"			=> 3,
			"max_wd"			=> 4,
			"temp"				=> 5,
			"humid1"			=> 6,
			"humid2"			=> 7,
			"press1"			=> 8,
			"press2"			=> 9,
			"rain_10min"	=> 10,
			"rain_hour"		=> 11,
			"rain_day"		=> 12
		),
		"200" => array(
			"avg_ws"					=> 1,
			"avg_wd"		 			=> 2,
			"max_ws"					=> 3,
			"max_wd"					=> 4,
			"temp"						=> 5,
			"humid1"					=> 6,
			"humid2"					=> 7,
			"press1"					=> 8,
			"press2"					=> 9,
			"max_rain_10min"	=> 10,
			"rain_hour"				=> 11,
			"rain_day"				=> 12,
			"weather"					=> 13
		),
		"300" => array(
			"avg_ws"					=> 1,
			"avg_wd"		 			=> 2,
			"max_ws"					=> 3,
			"max_wd"					=> 4,
			"avg_temp"				=> 5,
			"max_temp"				=> 6,
			"min_temp"				=> 7,
			"avg_humid1"			=> 8,
			"max_humid1"			=> 9,
			"min_humid1"			=> 10,
			"humid2"					=> 11,
			"press1"					=> 12,
			"press2"					=> 13,
			"max_rain_10min"	=> 14,
			"max_rain_hour"		=> 15,
			"rain_day"				=> 16,
			"weather1"				=> 17,
			"weather2"				=> 18,
			"weather3"				=> 19
		),
		"400" => array(
			"avg_ws"					=> 1,
			"avg_wd"		 			=> 2,
			"max_ws"					=> 3,
			"max_wd"					=> 4,
			"avg_temp"				=> 5,
			"max_temp"				=> 6,
			"min_temp"				=> 7,
			"avg_humid1"			=> 8,
			"max_humid1"			=> 9,
			"min_humid1"			=> 10,
			"humid2"					=> 11,
			"press1"					=> 12,
			"press2"					=> 13,
			"max_rain_10min"	=> 14,
			"max_rain_hour"		=> 15,
			"max_rain_day"		=> 16,
			"rain_month"			=> 17
		)
	);

	var $OutSumFieldIdx = array(
		"200" => array(
			"avg_ws"					=> array(40,50,55),
			"avg_wd"					=> array(40,50),
			"max_ws"					=> array(50,55),
			"max_wd"					=> array(50),
			"temp"						=> array(40,50,55,60,65),
			"humid1"					=> array(40,50,55,60,65),
			"humid2"					=> array(40),
			"press1"					=> array(40,50,55,60,65),
			"press2"					=> array(40,50,55,60,65),
			"max_rain_10min"	=> array(50,55),
			"rain_hour"				=> array(50,55),
			"rain_day"				=> array(70)
		),
		"300" => array(
			"avg_ws"					=> array(40,50,55),
			"avg_wd"		 			=> array(40,50),
			"max_ws"					=> array(50,55),
			"max_wd"					=> array(50),
			"avg_temp"				=> array(40),
			"max_temp"				=> array(50,55,80),
			"min_temp"				=> array(60,65,80),
			"avg_humid1"			=> array(40),
			"max_humid1"			=> array(50,55),
			"min_humid1"			=> array(60,65),
			"humid2"					=> array(40,50,55,60,65),
			"press1"					=> array(40,50,55,60,65),
			"press2"					=> array(40,50,55,60,65),
			"max_rain_10min"	=> array(50,55),
			"max_rain_hour"		=> array(50,55),
			"rain_day"				=> array(50,55,70,80)
		),
		"400" => array(
			"avg_ws"					=> array(40,50,55),
			"avg_wd"		 			=> array(40,50),
			"max_ws"					=> array(50,55),
			"max_wd"					=> array(50),
			"avg_temp"				=> array(40),
			"max_temp"				=> array(50,55,80),
			"min_temp"				=> array(60,65,80),
			"avg_humid1"			=> array(40),
			"max_humid1"			=> array(50,55),
			"min_humid1"			=> array(60,65),
			"humid2"					=> array(40,50,55,60,65),
			"press1"					=> array(40,50,55,60,65),
			"press2"					=> array(40,50,55,60,65),
			"max_rain_10min"	=> array(50,55),
			"max_rain_hour"		=> array(50,55),
			"max_rain_day"		=> array(50,55),
			"rain_month"			=> array(50,55,70,80)
		)

	);

	//����ǡ����������
	function SetDailyData(&$args,$AryIN){
		foreach($this->OutFieldIdx["200"] as $key => $idx){
			if(!array_key_exists($key,$args)) continue;
			if($key=="avg_wd"||$key=="max_wd"){
				$args[$key] = $this->GetDirIdx($AryIN[$idx]);
			}elseif($key=="weather"){
				$args[$key] = $this->GetWeatherIdx($AryIN[$idx]);
			}else{
				$args[$key] = $AryIN[$idx];
			}
		}
	}
	//���󽸷ץǡ����������
	function SetDailySumData(&$args,$AryIN){
		foreach($this->OutSumFieldIdx["200"] as $key => $sum){
			foreach($sum as $sum_no){
				if(!array_key_exists($key,$args[$sum_no])) continue;
				$val = $AryIN[ $this->SumFieldIdx[$sum_no] ][ $this->OutFieldIdx["200"][$key] ];
				if($key=="avg_wd"||$key=="max_wd"){
					$args[$sum_no][$key] = $this->GetDirIdx($val);
				}else{
					$args[$sum_no][$key] = $val;
				}
			}
		}
	}

	//����ǡ�����ư�����ʼ����ǡ�����Ϣ������˳�Ǽ�����
	function GetLoggerDailyCollection($pnt_id,$trg1,$trg2=""){
		if(!$this->load_flg)	$this->LoadLoggerInfo($pnt_id);

		//�ǡ��������ե���������
		$data 		= new Data($log);
		$rec			= $data->GetDataField("200");
		$rec_sum	= $data->GetSumDataField("200");

		if(is_object($this->log))	$this->log->logPrint("GetLoggerDailyCollection [{$trg1}] => [{$trg2}]",REC_SOCK,__LINE__,__FILE__);

		//��Ǽ��Ϣ����������
		$this->AryColData = array();

		if($trg2!=""){
			$time1 = DateConvert3($trg1);
			$time2 = DateConvert3($trg2);
		}else{
			$time1 = DateConvert3($trg1);
			$time2 = $time1;
		}

		$t1 = $time1;
		$t2 = $time2;
		while($t1<=$t2){
			if($wk!=date("Ymd",$t1)){
				$date	= DateConvert($t1);
				$url	= "http://".$this->setting["lgr_ip"]."/daily_xml.php?pnt=".$this->setting["lgr_pid"]."&trg=".$date["date"]."&nochg=1";
				$buf 	= @file($url);
				if($buf===false){
					if(is_object($this->log))	$this->log->logPrint("Error[Data Not Access][$url]",REC_SOCK,__LINE__,__FILE__);
				}else{
					$buf = join("",$buf);
				}

				if(trim($buf)!=""){
					$buf	= mb_convert_encoding($buf,"EUC-JP","UTF-8");
					$data	= explode("\n",$buf);
					$line = array_slice($data,3,24);

					foreach($line as $value){
						$tmp		= explode(",",$value);
						$buf		= $this->ClearBuf($value);
						$AryTmp	=	$this->ClearBuf2($buf);

						$args = $rec;
						$this->SetDailyData($args,$AryTmp);
						$this->AryColData[ date("Ymd",$t1).str_replace(":","",$AryTmp[0]) ] = $args;
					}

					//�����ͼ���
					$line = array_slice($data,27,6);
					$AryTmp =array();
					foreach($line as $value){
						$tmp			= explode(",",$value);
						$buf			= $this->ClearBuf($value);
						$AryTmp[]	=	$this->ClearBuf2($buf);
					}

					$args = $rec_sum;
					$this->SetDailySumData($args,$AryTmp);
					$this->AryColSumData[ date("Ymd",$t1) ] = $args;

					if($this->SearchProcField("avg_ws","200",$args["90"])){
						//�������٥ǡ�������
						$line		= array_slice($data,34,1);
						$tmp		= explode(",",$line[0]);
						$AryTmp	= array_slice($tmp,1,17);
						$this->AryColSumData[ date("Ymd",$t1) ]["90"]["avg_ws"] = join(",",$AryTmp);
					}

				}else{
					if(is_object($this->log))	$this->log->logPrint("Error[No Data][$buf]",REC_SOCK,__LINE__,__FILE__);
				}
				$wk = date("Ymd",$t1);
			}
			$t1 += 3600;
			if($t1>time()) break;
		}
		return true;
	}

	//����ǡ�����������
	function GetMonthlyData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime-86400);
		$trg1	= $date["year"].$date["month"].$date["day"];
		$date	= DateConvert($curTime);
		$trg2	= $date["year"].$date["month"].$date["day"];
		$this->GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2);

		$data = new Data($this->log);
		foreach($this->AryColData as $key => $val){
			$d = DateConvert2($key);
			$data->SaveData($pnt_id,"300",strtotime($d["year"]."/".$d["month"]."/".$d["day"]." 00:01"),$val,"",false);
		}
		return true;
	}
	//���󽸷ץǡ�����������
	function GetMonthlySumData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime-86400);
		$trg1	= $date["year"].$date["month"];
		$date	= DateConvert($curTime);
		$trg2	= $date["year"].$date["month"];
		if(!(is_array($this->AryColSumData[$trg1])&&is_array($this->AryColSumData[$trg2]))){
			$this->GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2);
		}

		$data = new Data($this->log);
		foreach($this->AryColSumData as $key => $val){
			foreach($val as $sum_no => $rec){
				$d = DateConvert2($key);
				$data->SaveData($pnt_id,"300",strtotime($d["year"]."/".$d["month"]."/01 00:01"),$rec,$sum_no,true);
			}
		}
		return true;
	}
	//����ǡ�����ư������Ϣ������˳�Ǽ�����ǡ����������
	function GetMonthlyCollectionData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime);
		$trg	= $date["year"].$date["month"].$date["day"];
		foreach($args as $fld => $val){
			$args[$fld] = $this->AryColData[$trg][$fld];
		}
		return true;
	}
	//���󽸷ץǡ�����ư������Ϣ������˳�Ǽ�����ǡ����������
	function GetMonthlyCollectionSumData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime);
		$trg	= $date["year"].$date["month"];
		foreach($args as $sum_no => $rec){
			foreach($rec as $fld => $val){
				$args[$sum_no][$fld] = $this->AryColSumData[$trg][$sum_no][$fld];
			}
		}
		return true;
	}

	//���󽸷ץǡ����������
	function SetMonthlySumData(&$args,$AryIN){
		foreach($this->OutSumFieldIdx["300"] as $key => $sum){
			foreach($sum as $sum_no){
				if(!array_key_exists($key,$args[$sum_no])) continue;
				$val = $AryIN[ $this->SumFieldIdx[$sum_no] ][ $this->OutFieldIdx["300"][$key] ];
				if($key=="avg_wd"||$key=="max_wd"){
					$args[$sum_no][$key] = $this->GetDirIdx($val);
				}else{
					$args[$sum_no][$key] = $val;
				}
			}
		}
	}

	//����ǡ�����ư����
	function GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2=""){
		//�ǡ��������ե���������
		$data 		= new Data($log);
		$rec			= $data->GetDataField("300");
		$rec_sum	= $data->GetSumDataField("300");

		if($trg2=="")	$trg2 = $trg1;

		if(is_object($this->log))	$this->log->logPrint("GetLoggerMonthlyCollection [{$trg1}] => [{$trg2}]",REC_SOCK,__LINE__,__FILE__);

		//�������դ���Ū���Ѵ�
		$date = DateConvert2($trg1);
		$trg1	= $date["year"].$date["month"]."010010";
		$date = DateConvert2($trg2);
		$trg2	= $date["year"].$date["month"].LastDate($date["year"],$date["month"])."2400";
		if(!$this->yearly_mode){
			//���󽸷��ͤ����
			$this->GetLoggerDailyCollection($pnt_id,$trg1,$trg2);
			$AryDat = $this->AryColData;

			//��Ǽ��Ϣ����������
			$this->AryColData = array();

			//����ǡ��������
			foreach($this->AryColSumData as $trg => $ArySum){
				$args = $rec;
				if(array_key_exists("avg_ws",$args))					$args["avg_ws"]						=	$ArySum["40"]["avg_ws"];
				if(array_key_exists("avg_wd",$args))					$args["avg_wd"]						=	$ArySum["40"]["avg_wd"];
				if(array_key_exists("max_ws",$args))					$args["max_ws"]						=	$ArySum["50"]["max_ws"];
				if(array_key_exists("max_wd",$args))					$args["max_wd"]						=	$ArySum["50"]["max_wd"];
				if(array_key_exists("avg_temp",$args))				$args["avg_temp"] 				=	$ArySum["40"]["temp"];
				if(array_key_exists("max_temp",$args))				$args["max_temp"] 				=	$ArySum["50"]["temp"];
				if(array_key_exists("min_temp",$args))				$args["min_temp"] 				=	$ArySum["60"]["temp"];
				if(array_key_exists("avg_humid1",$args))			$args["avg_humid1"]				=	$ArySum["40"]["humid1"];
				if(array_key_exists("max_humid1",$args))			$args["max_humid1"]				=	$ArySum["50"]["humid1"];
				if(array_key_exists("min_humid1",$args))			$args["min_humid1"]				=	$ArySum["60"]["humid1"];
				if(array_key_exists("humid2",$args))					$args["humid2"]						=	$ArySum["40"]["humid2"];
				if(array_key_exists("press1",$args))					$args["press1"]						=	$ArySum["40"]["press1"];
				if(array_key_exists("avg_press1",$args))			$args["avg_press1"]				=	$ArySum["40"]["press1"];
				if(array_key_exists("max_press1",$args))			$args["max_press1"]				=	$ArySum["50"]["press1"];
				if(array_key_exists("min_press1",$args))			$args["min_press1"]				=	$ArySum["60"]["press1"];
				if(array_key_exists("press2",$args))					$args["press2"]						=	$ArySum["40"]["press2"];
				if(array_key_exists("avg_press2",$args))			$args["avg_press2"]				=	$ArySum["40"]["press2"];
				if(array_key_exists("max_press2",$args))			$args["max_press2"]				=	$ArySum["50"]["press2"];
				if(array_key_exists("min_press2",$args))			$args["min_press2"]				=	$ArySum["60"]["press2"];
				if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]		=	$ArySum["50"]["max_rain_10min"];
				if(array_key_exists("max_rain_hour",$args))		$args["max_rain_hour"]		=	$ArySum["50"]["rain_hour"];
				if(array_key_exists("rain_day",$args))				$args["rain_day"]					=	$ArySum["70"]["rain_day"];
				if(array_key_exists("weather1",$args))				$args["weather1"]					=	$AryDat[$trg."0900"]["weather"];
				if(array_key_exists("weather2",$args))				$args["weather2"]					=	$AryDat[$trg."1500"]["weather"];
				if(array_key_exists("weather3",$args))				$args["weather3"]					=	$AryDat[$trg."2400"]["weather"];
				if(array_key_exists("snow_level",$args))			$args["snow_level"]				=	$ArySum["50"]["snow_level"];
				if(array_key_exists("snow_day",$args))				$args["snow_day"]					=	$ArySum["70"]["move_snow_hour"];
				$this->AryColData[ $trg ] = $args;
			}
		}

		$date1 	= DateConvert2($trg1);
		$date2 	= DateConvert2($trg2);
		$t1 		= $date1["year"].$date1["month"];
		$t2 		= $date2["year"].$date2["month"];
		while($t1<=$t2){

			if($wk!=$t1){
				$date = DateConvert2($t1);

				$url	= "http://".$this->setting["lgr_ip"]."/monthly_xml.php?pnt=".$this->setting["lgr_pid"]."&trg=".$t1;
				$buf 	= @file($url);
				if($buf===false){
					if(is_object($this->log))	$this->log->logPrint("Error[Data Not Access][$url]",REC_SOCK,__LINE__,__FILE__);
				}else{
					$buf = join("",$buf);
				}

				if(trim($buf)!=""){
					$buf	= mb_convert_encoding($buf,"EUC-JP","UTF-8");
					$data	= explode("\n",$buf);

					//������
					$line = array_slice($data,LastDate($date["year"],$date["month"])+3,7);
					$AryTmp =array();
					foreach($line as $value){
						$tmp			= explode(",",$value);
						$buf			= $this->ClearBuf($value);
						$AryTmp[]	=	$this->ClearBuf2($buf);
					}
					$args = $rec_sum;
					$this->SetMonthlySumData($args,$AryTmp);
					$this->AryColSumData[ $t1 ] = $args;

					if($this->SearchProcField("avg_ws","300",$args["90"])){
						//�������٥ǡ�������
						$line		= array_slice($data,LastDate($date["year"],$date["month"])+11,1);
						$tmp		= explode(",",$line[0]);
						$AryTmp	= array_slice($tmp,1,17);
						$this->AryColSumData[ $t1 ]["90"]["avg_ws"] = join(",",$AryTmp);
					}
				}
				$wk = $t1;
			}
			$date		= DateConvert2($t1);
			$year 	= $date["year"];
			$month	= sprintf("%02d",$date["month"]+1);
			if($month=="13"){
				$month	= "01";
				$year 	= $date["year"] + 1;
			}
			$t1 = $year.$month;

			if($t1>date("Ym")) break;
		}
		return true;
	}

	//ǯ��ǡ�����������
	function GetYearlyData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime-86400);
		$trg1	= $date["year"];
		$date	= DateConvert($curTime);
		$trg2	= $date["year"];
		$this->GetLoggerYearlyCollection($pnt_id,$trg1,$trg2);

		$data = new Data($this->log);
		foreach($this->AryColData as $key => $val){
			$d = DateConvert2($key);
			$data->SaveData($pnt_id,"400",strtotime($d["year"]."/".$d["month"]."/01 00:01"),$val,"",false);
		}
		return true;
	}
	//ǯ�󽸷ץǡ�����������
	function GetYearlySumData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime-86400);
		$trg1	= $date["year"];
		$date	= DateConvert($curTime);
		$trg2	= $date["year"];
		if(!(is_array($this->AryColSumData[$trg1])&&is_array($this->AryColSumData[$trg2]))){
			$this->GetLoggerYearlyCollection($pnt_id,$trg1,$trg2);
		}

		$data = new Data($this->log);
		foreach($this->AryColSumData as $key => $val){
			foreach($val as $sum_no => $rec){
				$d = DateConvert2($key);
				$data->SaveData($pnt_id,"400",strtotime($d["year"]."/01/01 00:01"),$rec,$sum_no,true);
			}
		}
		return true;
	}
	//ǯ��ǡ�����ư������Ϣ������˳�Ǽ�����ǡ����������
	function GetYearlyCollectionData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime);
		$trg	= $date["year"].$date["month"];
		foreach($args as $fld => $val){
			$args[$fld] = $this->AryColData[$trg][$fld];
		}
		return true;
	}
	//ǯ�󽸷ץǡ�����ư������Ϣ������˳�Ǽ�����ǡ����������
	function GetYearlyCollectionSumData($pnt_id,$curTime,&$args){
		$date	= DateConvert($curTime);
		$trg	= $date["year"];
		foreach($args as $sum_no => $rec){
			foreach($rec as $fld => $val){
				$args[$sum_no][$fld] = $this->AryColSumData[$trg][$sum_no][$fld];
			}
		}
		return true;
	}

	//ǯ�󽸷ץǡ����������
	function SetYearlySumData(&$args,$AryIN){
		foreach($this->OutSumFieldIdx["400"] as $key => $sum){
			foreach($sum as $sum_no){
				if(!array_key_exists($key,$args[$sum_no])) continue;
				$val = $AryIN[ $this->SumFieldIdx[$sum_no] ][ $this->OutFieldIdx["400"][$key] ];
				if($key=="avg_wd"||$key=="max_wd"){
					$args[$sum_no][$key] = $this->GetDirIdx($val);
				}else{
					$args[$sum_no][$key] = $val;
				}
			}
		}
	}

	//ǯ��ǡ�����ư�����ʼ����ǡ�����Ϣ������˳�Ǽ�����
	function GetLoggerYearlyCollection($pnt_id,$trg1,$trg2=""){
		//�ǡ��������ե���������
		$data 		= new Data($log);
		$rec			= $data->GetDataField("400");
		$rec_sum	= $data->GetSumDataField("400");

		if($trg2=="")	$trg2 = $trg1;

		//�������դ���Ū���Ѵ�
		$date = DateConvert2($trg1);
		$trg1	= $date["year"]."01010010";
		$date = DateConvert2($trg2);
		$trg2	= $date["year"]."12312400";

		if(is_object($this->log))	$this->log->logPrint("GetLoggerYearlyCollection [{$trg1}] => [{$trg2}]",REC_SOCK,__LINE__,__FILE__);

		//���󽸷��ͤ����
		$this->yearly_mode = true;
		$this->GetLoggerMonthlyCollection($pnt_id,$trg1,$trg2);

		//��Ǽ��Ϣ����������
		$this->AryColData = array();

		//����ǡ��������
		foreach($this->AryColSumData as $trg => $ArySum){
			$args = $rec;
			if(array_key_exists("avg_ws",$args))					$args["avg_ws"]						=	$ArySum["40"]["avg_ws"];
			if(array_key_exists("avg_wd",$args))					$args["avg_wd"]						=	$ArySum["40"]["avg_wd"];
			if(array_key_exists("max_ws",$args))					$args["max_ws"]						=	$ArySum["50"]["max_ws"];
			if(array_key_exists("max_wd",$args))					$args["max_wd"]						=	$ArySum["50"]["max_wd"];
			if(array_key_exists("avg_temp",$args))				$args["avg_temp"] 				=	$ArySum["40"]["avg_temp"];
			if(array_key_exists("max_temp",$args))				$args["max_temp"] 				=	$ArySum["50"]["max_temp"];
			if(array_key_exists("min_temp",$args))				$args["min_temp"] 				=	$ArySum["60"]["min_temp"];
			if(array_key_exists("avg_humid1",$args))			$args["avg_humid1"]				=	$ArySum["40"]["avg_humid1"];
			if(array_key_exists("max_humid1",$args))			$args["max_humid1"]				=	$ArySum["50"]["max_humid1"];
			if(array_key_exists("min_humid1",$args))			$args["min_humid1"]				=	$ArySum["60"]["min_humid1"];
			if(array_key_exists("humid2",$args))					$args["humid2"]						=	$ArySum["40"]["humid2"];
			if(array_key_exists("avg_humid2",$args))			$args["avg_humid2"]				=	$ArySum["40"]["avg_humid2"];
			if(array_key_exists("max_humid2",$args))			$args["max_humid2"]				=	$ArySum["50"]["max_humid2"];
			if(array_key_exists("min_humid2",$args))			$args["min_humid2"]				=	$ArySum["60"]["min_humid2"];
			if(array_key_exists("press1",$args))					$args["press1"]						=	$ArySum["40"]["press1"];
			if(array_key_exists("avg_press1",$args))			$args["avg_press1"]				=	$ArySum["40"]["press1"];
			if(array_key_exists("max_press1",$args))			$args["max_press1"]				=	$ArySum["50"]["press1"];
			if(array_key_exists("min_press1",$args))			$args["min_press1"]				=	$ArySum["60"]["press1"];
			if(array_key_exists("press2",$args))					$args["press2"]						=	$ArySum["40"]["press2"];
			if(array_key_exists("avg_press2",$args))			$args["avg_press2"]				=	$ArySum["40"]["press2"];
			if(array_key_exists("max_press2",$args))			$args["max_press2"]				=	$ArySum["50"]["press2"];
			if(array_key_exists("min_press2",$args))			$args["min_press2"]				=	$ArySum["60"]["press2"];
			if(array_key_exists("max_rain_10min",$args))	$args["max_rain_10min"]		=	$ArySum["50"]["max_rain_10min"];
			if(array_key_exists("max_rain_hour",$args))		$args["max_rain_hour"]		=	$ArySum["50"]["max_rain_hour"];
			if(array_key_exists("max_rain_day",$args))		$args["max_rain_day"]			=	$ArySum["50"]["rain_day"];
			if(array_key_exists("rain_month",$args))			$args["rain_month"]				=	$ArySum["70"]["rain_day"];
			if(array_key_exists("sun_month",$args))				$args["sun_month"]				=	$ArySum["70"]["sun_day"];
			if(array_key_exists("snow_level",$args))			$args["snow_level"]				=	$ArySum["50"]["snow_level"];
			if(array_key_exists("snow_month",$args))			$args["snow_month"]				=	$ArySum["70"]["snow_day"];
			if($trg>=date("Ym")) continue;
			$this->AryColData[ $trg ] = $args;
		}

		//��Ǽ��Ϣ����������
		$this->AryColSumData = array();

		$date1 	= DateConvert2($trg1);
		$date2 	= DateConvert2($trg2);
		$t1 		= $date1["year"];
		$t2 		= $date2["year"];
		while($t1<=$t2){
			if($wk!=$t1){
				$date = DateConvert2($t1);

				$url	= "http://".$this->setting["lgr_ip"]."/yearly_xml.php?pnt=".$this->setting["lgr_pid"]."&trg=".$t1;
				$buf 	= @file($url);
				if($buf===false){
					if(is_object($this->log))	$this->log->logPrint("Error[Data Not Access][$url]",REC_SOCK,__LINE__,__FILE__);
				}else{
					$buf = join("",$buf);
				}

				if(trim($buf)!=""){
					$buf	= mb_convert_encoding($buf,"EUC-JP","UTF-8");
					$data	= explode("\n",$buf);

					//������
					$line = array_slice($data,15,7);
					$AryTmp =array();
					foreach($line as $value){
						$tmp			= explode(",",$value);
						$buf			= $this->ClearBuf($value);
						$AryTmp[]	=	$this->ClearBuf2($buf);
					}
					$args = $rec_sum;
					$this->SetYearlySumData($args,$AryTmp);
					$this->AryColSumData[ $t1 ] = $args;

					if($this->SearchProcField("avg_ws","400",$args["90"])){
						//�������٥ǡ�������
						$line		= array_slice($data,23,1);
						$tmp		= explode(",",$line[0]);
						$AryTmp	= array_slice($tmp,1,17);
						$this->AryColSumData[ $t1 ]["90"]["avg_ws"] = join(",",$AryTmp);
					}
				}
				$wk = $t1;
			}
			$t1++;
		}
		return true;
	}

}
?>